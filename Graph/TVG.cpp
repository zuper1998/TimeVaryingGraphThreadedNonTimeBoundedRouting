#include "TVG.h"

#include <utility>
#include "../Algo/CalcBestPath.h"
std::vector<double> parseFloat(std::string const& basicString);
DataHolder getDataFromLine(const std::string& l);


std::atomic<int> curstate(0);

void TVG::addEdge(Edge* e)
{
	//Here the edge is already inicialized with ctor
	//Set nodes


	e->start->addEdge(e);
	e->end->addEdge(e);
}

void TVG::addEdge(const DataHolder& dt)
{
	Node* src = findNode(dt.src);
	Node* dst = findNode(dt.dst);
	Edge* e = new Edge(src, dst);
	//Check if edge already added
	if (!edgeAlreadyAdded(e)) {
		addEdge(e);
	}
	else {
		e = findEdge(e);
	}
	//If duplicate dont add
	VisibilityInterval tmp = VisibilityInterval(TVG_TIME(std::strtol(dt.start.c_str(), nullptr,10)),
                                                TVG_TIME(std::strtol(dt.end.c_str(), nullptr,10)),
                                                parseFloat(dt.values));

	e->addVisInterval(tmp);

}


Node* TVG::findNode(const std::string& findstr)
{

    auto find_fun = [&](Node const&n){return n.getName()==findstr;};
    auto found_node = std::find_if(begin(nodes),std::end(nodes),find_fun);
    if(found_node==std::end(nodes)){
        return nullptr;
    }

    return &*found_node;


}

void TVG::readFromFile(const std::string& f)
{
	std::ifstream file(f);

	std::vector<satTemp> st;
	while (!file.eof()) {

		satTemp cst;

		std::string name;
		std::string num_lines;
		getline(file, name, '|');
		getline(file, num_lines);

		cst.name = name;
		for (int i = 0; i < std::strtol(num_lines.c_str(),nullptr,10); i++) {

			std::string line; getline(file, line);
			cst.lines.push_back(line);
		}
		st.push_back(cst);

	}
	//Create all nodes
	for (auto& s : st) {
		nodes.emplace_back(s.name);
	}
	// Add all edges, watch out for the duplications!
	for (auto& s : st) {
		for (const std::string& l : s.lines) {
			DataHolder dt = getDataFromLine(l);
			dt.src = s.name;
			addEdge(dt);
		}
	}

	//Check for anomaly:
	for (auto& si : st) {
		auto s = *findNode(si.name);
		unsigned  long long sum = 0;
		for (auto e : s.getEdges()) {
			sum += e->getVisIntervalVal().size();
		}
		if (sum != si.lines.size()*2) {
			throw "SatLoad Anomaly at " + si.name + "!";
		}

	}



}

bool TVG::edgeAlreadyAdded(Edge* e)
{
	return findEdge(e) != nullptr;
}

Edge* TVG::findEdge(Edge* e)
{
	for (Edge* e_inner : e->start->getEdges()) {
		if (*e_inner == *e) 
			return e_inner;
	}
	return nullptr;
}




/// <summary>
/// Procuder
/// </summary>
/// <param name="src">Routes from</param>
/// <param name="dst">To</param>
/// <param name="p">Path for recursion</param>
/// <param name="paths">buffer</param>
void findRouteDFS(const Node* src,const Node* dst, const Path& p,Thread_safe_queue<Path>* paths, std::atomic_bool* allow) {
	//Faster termination
	if (!allow->load()) return;
	if (p.edges.size() > 5) return;
	if (src == dst) {
		//Paralelize here --> we can use cpu power while waiting this to finish
		// https://github.com/bshoshany/thread-pool
		paths->push(p);
		//Stat
		curstate += 1;

	}
	else {  // all adjectent vertixes
		for (Edge* e : p.getLastNodeEdges()) {
			//Next node
			Node* curr = e->start != p.getLastNode() ? e->start : e->end;
			//Is not visited
			if (!p.containNode(curr)) {
				findRouteDFS(curr, dst, Path(p, e), paths, allow);
			}
		}
	}

}
void findRouteDFSWrapper(const Node* src,const Node* dst, const Path& p,Thread_safe_queue<Path>* paths, std::atomic_bool* allow) {
    findRouteDFS(src,dst,p,paths,allow);
    //Notify threads that it is finished
    allow->store(false);
    paths->GenDone();
}

IntervalPath consumeRoutes(Thread_safe_queue<Path>* buffer, std::atomic_bool* allow, int index, double target,double *max) {
	IntervalPath ret;
	while (allow->load()) {

		//Do something
		Path p = buffer->pop();
        if(!p.init) continue;
		curstate -= 1;
		//Do calculation
		auto [lpath, lmax] = CalcBestPath::calculateBestPath(p);

		if (*max < lmax) {
			*max = lmax;
			ret = lpath;
		}

		if (*max - DefValues::targetWindow >= target) {
			allow->store(false);
			//p.RemoveVisIntervals(ret,max);


			return ret;
		}

	}
	// other threead finished return an empty path
	return ret;
}

[[noreturn]] void checkTPool(thread_pool* pool) {
	int lastVal= curstate.load();
	while (true) {
		int curVal = curstate.load();
		//std::cout << "\r" <<  curVal<< " Delta: " << curVal - lastVal << "                                      " << std::flush;
		lastVal = curVal;
        std::this_thread::sleep_for(100ms);
	}
}


//All routes are bidirectional, the routes are twice between all sats:
// This means that there are two bidirectional routes between all sats
std::vector<Path> TVG::findRoutesBetween(const std::string& src, const std::string& dst)
{
	std::vector<Path> main;
	std::vector<Path> ret;
	std::vector<std::future<Path>> out;
	Node* start = findNode(src);
	Node* end = findNode(dst);
	for (auto& e : start->getEdges()) {
		//Create a path with all the starting edges

		main.emplace_back(e, start);

	}


	auto* cache = new Thread_safe_queue<Path>[main.size()+2];
    auto rets = new double[main.size()+2];
	std::vector<std::shared_future<IntervalPath>> goodPath;
	auto* allowList = new std::atomic_bool[main.size()];

	//DFS faster
	pool->push_task(checkTPool, pool);

	int index = 0;
	for (auto p : main) {
		allowList[index] = true;
		double target_val = p.edges.front()->getLargestThrougput();
		std::cout << target_val << std::endl;
		pool->submit(findRouteDFSWrapper, p.getLastNode(), end, p, &cache[index], &allowList[index]);
		goodPath.emplace_back(pool->submit(consumeRoutes, &cache[index], &allowList[index], index, target_val, &rets[index]));
        goodPath.emplace_back(pool->submit(consumeRoutes, &cache[index], &allowList[index], index, target_val, &rets[index]));
        //goodPath.emplace_back(pool->submit(consumeRoutes, &cache[index], &allowList[index], index, target_val, &rets[index]));

		//break;

		index++;
	}

	for (std::shared_future<IntervalPath> const&  p : goodPath) {
        bool a = p.valid();
		IntervalPath pp = p.get();
		std::cout << pp.getThrougput() << std::endl;
	}

	return ret;
}


DataHolder getDataFromLine(const std::string& l) {
	std::stringstream ss(l);
	DataHolder ret;

	getline(ss, ret.values, '|');
	getline(ss, ret.dst, '|');
	getline(ss, ret.start, '|');
	getline(ss, ret.end, '|');

	return ret;
}
std::vector<double> parseFloat(std::string const& basicString) {
	std::vector<double> out;
	std::stringstream ss(basicString);
	for (std::string line; getline(ss, line, ','); out.push_back(std::stod(line)));
	return out;
}
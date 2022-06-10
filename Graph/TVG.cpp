#include "TVG.h"

#include "../Algo/CalcBestPath.h"

std::vector<double> parseFloat(std::string const &basicString);

DataHolder getDataFromLine(const std::string &l);


void iterOverEdges(const std::vector<Node *> &destinations, Thread_safe_queue<Path> *paths, std::vector<Path> &tmp_data,
                   const Path &path);

std::atomic<int> curstate(0);

void TVG::addEdge(Edge *e) {
    //Here the edge is already inicialized with ctor
    //Set nodes


    e->start->addEdge(e);
    e->end->addEdge(e);
}

void TVG::addEdge(const DataHolder &dt) {
    Node *src = findNode(dt.src);
    Node *dst = findNode(dt.dst);
    Edge *e = new Edge(src, dst);
    //Check if edge already added
    if (!edgeAlreadyAdded(e)) {
        addEdge(e);
    } else {
        e = findEdge(e);
    }
    //If duplicate dont add
    VisibilityInterval tmp = VisibilityInterval(TVG_TIME(std::strtol(dt.start.c_str(), nullptr, 10)),
                                                TVG_TIME(std::strtol(dt.end.c_str(), nullptr, 10)),
                                                parseFloat(dt.values));

    e->addVisInterval(tmp);

}


Node *TVG::findNode(const std::string &findstr) {

    auto find_fun = [&](Node const &n) { return n.getName() == findstr; };
    auto found_node = std::find_if(begin(nodes), std::end(nodes), find_fun);
    if (found_node == std::end(nodes)) {
        return nullptr;
    }

    return &*found_node;


}

void TVG::readFromFile(const std::string &f) {
    std::ifstream file(f);

    std::vector<satTemp> st;
    while (!file.eof()) {

        satTemp cst;

        std::string name;
        std::string num_lines;
        getline(file, name, '|');
        getline(file, num_lines);

        cst.name = name;
        for (int i = 0; i < std::strtol(num_lines.c_str(), nullptr, 10); i++) {

            std::string line;
            getline(file, line);
            cst.lines.push_back(line);
        }
        st.push_back(cst);

    }
    //Create all nodes
    for (auto &s: st) {
        nodes.emplace_back(s.name);
    }
    // Add all edges, watch out for the duplications!
    for (auto &s: st) {
        for (const std::string &l: s.lines) {
            DataHolder dt = getDataFromLine(l);
            dt.src = s.name;
            addEdge(dt);
        }
    }

    //Check for anomaly:
    for (auto &si: st) {
        auto s = *findNode(si.name);
        unsigned long long sum = 0;
        for (auto e: s.getEdges()) {
            sum += e->getVisIntervalVal().size();
        }
        if (sum != si.lines.size() * 2) {
            throw "SatLoad Anomaly at " + si.name + "!";
        }

    }


}

bool TVG::edgeAlreadyAdded(Edge *e) {
    return findEdge(e) != nullptr;
}

Edge *TVG::findEdge(Edge *e) {
    for (Edge *e_inner: e->start->getEdges()) {
        if (*e_inner == *e)
            return e_inner;
    }
    return nullptr;
}


static bool isDest(const std::vector<Node *> &destinations, Edge *const p) {
    return std::any_of(destinations.begin(), destinations.end(),
                       [&](Node const *n) { return *p->start == *n || *p->end == *n; });
}

/*
 * BFS walk of the graph, bounded at a constant value defined in constant
 */
void
findRouteBFSWrapper(Node *const src, const std::vector<Node *> &destinations, const std::vector<Path> &initialPaths,
                    Thread_safe_queue<Path> *paths, std::atomic_bool &allow) {
    std::vector<Path> data{Path()};
    std::vector<Path> tmp_data(initialPaths);
    //std::cout << "STARTING:" << src->getName() << std::endl;
    while (!data.empty()) {
        data.swap(tmp_data);
        tmp_data.clear();
        for (auto const &path: data) {
            if (tmp_data.size() > DefValues::maxSimPaths) break;
            iterOverEdges(destinations, paths, tmp_data, path);
        }
    }
    //allow.store(false); //This force stops all
    // std::cout << "Finishing:" << src->getName() << std::endl;


    paths->GenDone();


}

bool canBeAdded(const Path &path, Edge *const e) {
    Node *curr = e->start != path.getLastNode() ? e->start : e->end;
    return !path.containNode(curr);
}

void iterOverEdges(const std::vector<Node *> &destinations, Thread_safe_queue<Path> *paths, std::vector<Path> &tmp_data,
                   const Path &path) {
    for (auto const edge: path.getLastNode()->getEdges()) {
        if (isDest(destinations, edge)) {
            paths->push(Path(path, edge));
        } else if (canBeAdded(path, edge)) {
            tmp_data.emplace_back(path, edge);
        }
    }
}


CalculatedIntervals calculateInterval(Thread_safe_queue<Path> *buffer) {
    CalculatedIntervals ret{};

    Path retp;
    while (true) {
        Path p = buffer->pop();
        if (!p.init) break;
        auto[lpath, lmax] = CalcBestPath::calculateBestPath(p);
        ret.emplace_back(lpath, p);
    }
    return ret;
}

std::pair<IntervalPath, Path>
consumeRoutes(Thread_safe_queue<Path> *buffer, std::atomic_bool *allow, double *max) {
    IntervalPath ret;
    Path retp;
    while (allow->load()) {

        //Do something
        Path p = buffer->pop();
        if (!p.init) break;

        curstate -= 1;
        //Do calculation
        auto[lpath, lmax] = CalcBestPath::calculateBestPath(p);

        if (*max < lmax) {
            *max = lmax;
            ret = lpath;
            retp = p;
        }

    }
    // other thread finished return an empty path
    if (ret.getThrougput() == *max)
        return {ret, retp};
    else {
        return {};
    }
}


std::ostream &operator<<(std::ostream &os, std::pair<IntervalPath, Path> const &toPrint) {
    IntervalPath ip = toPrint.first;
    Path p = toPrint.second;

    os << p << std::endl;
    for (const VisibilityInterval &visibilityInterval: ip.intervals) {
        os << visibilityInterval.througput << " | ";
    }
    os << std::endl;
    return os;
}

void checkSize(Thread_safe_queue<Path> *const p) {
    while (true) {
        if(p->empty()) break;
        std::cout << "Queue :" << p->size() << "                 \r" << std::flush;
        std::this_thread::sleep_for(1s);
    }
}

//All routes are bidirectional, the routes are twice between all sats:
// This means that there are two bidirectional routes between all sats
std::vector<std::vector<std::tuple<IntervalPath, Path>>>
TVG::findRoutesBetween(const std::string &src, const std::vector<std::string> &dests) {
    std::vector<Path> main;
    Node *start = findNode(src);

    std::vector<Node *> destinations;

    for (auto &str: dests) {
        destinations.push_back(findNode(str));
    }
    for (auto &e: start->getEdges()) {
        //Create a path with all the starting edges
        main.emplace_back(e, start);

    }


    Thread_safe_queue<Path> cache{};
    double rets = 0;

    std::vector<std::shared_future<std::pair<IntervalPath, Path>>> goodPath;
    std::vector<std::shared_future<CalculatedIntervals> > paths;
    std::atomic_bool allowBool;
    allowBool.store(true);

    //BFS is more managable
    findRouteBFSWrapper(start, destinations, main, &cache, allowBool);


    /*goodPath.emplace_back(
            pool->submit(consumeRoutes, &cache, &allowBool, &rets));*/
    paths.emplace_back(
            pool->submit(calculateInterval, &cache)
    );
    paths.emplace_back(
            pool->submit(calculateInterval, &cache)
    );
    paths.emplace_back(
            pool->submit(calculateInterval, &cache)
    );
    //pool->submit(checkSize, &cache);

    std::vector<std::vector<std::tuple<IntervalPath, Path>>> ret;
    ret.reserve(paths.size());
    for(auto& p : paths){
        ret.push_back(p.get());
    }


    return ret;
}

std::vector<std::string> TVG::getCitiesWithout(const std::string& in) {
    std::vector<std::string> ret;
    for(const auto& satelliteNode : nodes){
        if(satelliteNode.getName().find("SAT")==std::string::npos && satelliteNode.getName() != in){
            ret.push_back(satelliteNode.getName());
        }
    }
    return ret;
}

std::vector<Node*> TVG::getNodesWithout(Node *pNode) {
    std::vector<Node*> ret;
    for(auto& satelliteNode : nodes){
        if(satelliteNode.getName().find("SAT")==std::string::npos && satelliteNode.getName() != pNode->getName()){
            ret.push_back(&satelliteNode);
        }
    }
    return ret;
}


DataHolder getDataFromLine(const std::string &l) {
    std::stringstream ss(l);
    DataHolder ret;

    getline(ss, ret.values, '|');
    getline(ss, ret.dst, '|');
    getline(ss, ret.start, '|');
    getline(ss, ret.end, '|');

    return ret;
}

std::vector<double> parseFloat(std::string const &basicString) {
    std::vector<double> out;
    std::stringstream ss(basicString);
    for (std::string line; getline(ss, line, ','); out.push_back(std::stod(line)));
    return out;
}
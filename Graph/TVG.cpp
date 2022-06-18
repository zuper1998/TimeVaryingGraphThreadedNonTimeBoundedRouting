#include "TVG.h"

#include "../Algo/CalcBestPath.h"

std::vector<double> parseFloat(std::string const &basicString);

DataHolder getDataFromLine(const std::string &l);


void iterOverEdges(const std::vector<Node *> &destinations, Thread_safe_queue<Path> *paths, std::vector<Path> &tmp_data,
                   const Path &path);



void TVG::addEdge(Edge *e) {
    //Here the edge is already inicialized with ctor
    //Set nodes
    edges.push_back(e);
    e->start->addEdge(e);
    e->end->addEdge(e);
}



void TVG::addEdge(const DataHolder &dt) {
    Node *src = findNode(dt.src);
    Node *dst = findNode(dt.dst);
    Edge *e = nullptr;

    //Check if edge already added
    if (!edgeAlreadyAdded(src,dst)) {
        e = new Edge(src, dst);
        addEdge(e);
    } else {
        e = findEdge(src,dst);
    }
    //If duplicate dont add
    VisibilityInterval tmp = VisibilityInterval(TVG_TIME(std::strtod(dt.start.c_str(), nullptr)),
                                                TVG_TIME(std::strtod(dt.end.c_str(), nullptr)),
                                                parseFloat(dt.values));
    // Not dealocated since it is only deleted when we found a new one
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


bool TVG::edgeAlreadyAdded(Node* n1, Node * n2) {
    return findEdge(n1,n2) != nullptr;
}


Edge *TVG::findEdge(Node *n1,Node*n2) {
    for(auto& e_inner : n1->getEdges()){
        if(e_inner->start == n1 && e_inner->end == n2){
            return e_inner;
        }
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
findRouteBFSWrapper(const std::vector<Node *> *destinations, const std::vector<Path> *initialPaths,
                    Thread_safe_queue<Path> *paths) {
    std::vector<Path> data{Path()};
    std::vector<Path> tmp_data(*initialPaths);
    //std::cout << "STARTING:" << std::endl;
    while (!data.empty()) {
        data.swap(tmp_data);
        tmp_data.clear();
        std::cout << data.size() << " | " << paths->size() << std::endl;
        for (auto const &path: data) {

            if (tmp_data.size() > DefValues::maxSimPaths) break;
            iterOverEdges(*destinations, paths, tmp_data, path);
        }
    }
    //allow.store(false); //This force stops all
    //std::cout << "Finishing:" << std::endl;


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


GraphDataStruct calculateInterval(Thread_safe_queue<Path> *buffer) {
    GraphDataStruct ret{};

    Path retp;
    while (true) {
        Path p = buffer->pop();
        if (!p.init) break;

        auto[lpath, lmax] = CalcBestPath::calculateBestPath(p);
        ret.addPath(lpath,p);
    }
    return ret;
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

[[noreturn]] void checkSize(Thread_safe_queue<Path> *const p) {
    while (true) {
        std::cout << "Queue :" << p->size() << "                 \r" << std::flush;
        std::this_thread::sleep_for(1s);
    }
}


//All routes are bidirectional, the routes are twice between all sats:
// This means that there are two bidirectional routes between all sats
GraphDataStruct
TVG::findRoutesBetween(const std::string &src, const std::vector<std::string> &dests) {
    std::vector<Path> main;
    Node *start = findNode(src);

    std::vector<Node *> destinations;

    destinations.reserve(dests.size());
    for (auto &str: dests) {
        destinations.push_back(findNode(str));
    }
    for (auto &e: start->getEdges()) {
        //Start should have edges that start from sats too
        main.emplace_back(e, start);

    }


    Thread_safe_queue<Path> cache;

    std::vector<std::shared_future<GraphDataStruct> > paths;




    //BFS is more managable
    pool->submit(findRouteBFSWrapper, &destinations, &main, &cache);

    /*goodPath.emplace_back(
            pool->submit(consumeRoutes, &cache, &allowBool, &rets));*/

    paths.reserve(TVG_THREADS);
    for(int i = 0; i< TVG_THREADS; i++){
        paths.emplace_back(
                pool->submit(calculateInterval, &cache)
        );
    }

#ifdef LOG
    pool->submit(checkSize, &cache);
#endif


    GraphDataStruct ret;
    for (auto &p: paths) {
        ret.concat(p.get());
    }


    ret.PrintNodes();

    return ret;
}

std::vector<std::string> TVG::getCitiesWithout(const std::string &in) {
    std::vector<std::string> ret;
    for (const auto &satelliteNode: nodes) {
        if (satelliteNode.getName().find("SAT") == std::string::npos && satelliteNode.getName() != in) {
            ret.push_back(satelliteNode.getName());
        }
    }
    return ret;
}

std::vector<Node *> TVG::getNodesWithout(Node *pNode) {
    std::vector<Node *> ret;
    for (auto &satelliteNode: nodes) {
        if (satelliteNode.getName().find("SAT") == std::string::npos && satelliteNode.getName() != pNode->getName()) {
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
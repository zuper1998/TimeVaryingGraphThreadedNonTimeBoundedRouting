#include "Node.h"

#include <utility>

Node::Node(std::string _name) : name(std::move(_name))
{

}

std::ostream& operator<<(std::ostream& os, Node n)
{
	os << n.name;
	return os;
}

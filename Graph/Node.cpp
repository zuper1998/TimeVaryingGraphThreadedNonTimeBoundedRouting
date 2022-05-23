#include "Node.h"

Node::Node(std::string _name)
{
	name = _name;
}

std::ostream& operator<<(std::ostream& os, Node n)
{
	os << n.name;
	return os;
}

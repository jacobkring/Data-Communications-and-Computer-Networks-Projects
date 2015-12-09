#include "messages.h"

RoutingMessage::RoutingMessage()
{}

RoutingMessage::RoutingMessage(const RoutingMessage &rhs) {
    *this = rhs;
}

RoutingMessage & RoutingMessage::operator=(const RoutingMessage & rhs) {
    /* For now.  Change if you add data members to the struct */
    return *this;
}

#if defined(GENERIC)
ostream &RoutingMessage::Print(ostream &os) const
{
    os << "Generic RoutingMessage()";
    return os;
}
#endif

#if defined(LINKSTATE)
ostream &RoutingMessage::Print(ostream &os) const
{
    os << "LinkState RoutingMessage()";
    return os;
}

RoutingMessage::RoutingMessage(int age, int source, int destination, int latency){
	age = age;
	src = source;
	dest = destination;
	lat = latency;
}
#endif

#if defined(DISTANCEVECTOR)
ostream &RoutingMessage::Print(ostream &os) const
{
    os << "DistanceVector RoutingMessage()";
    return os;
}
#endif

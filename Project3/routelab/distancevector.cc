#include "distancevector.h"

DistanceVector::DistanceVector(unsigned n, SimulationContext* c, double b, double l) :
    Node(n, c, b, l)
{}

DistanceVector::DistanceVector(const DistanceVector & rhs) :
    Node(rhs)
{
    *this = rhs;
}

DistanceVector & DistanceVector::operator=(const DistanceVector & rhs) {
    Node::operator=(rhs);
    return *this;
}

DistanceVector::~DistanceVector() {}


/** Write the following functions.  They currently have dummy implementations **/

void DistanceVector::LinkHasBeenUpdated(Link* l) {
    cerr << *this << ": Link Update: " << *l << endl;
    SendToNeighbors(new RoutingMessage());
}

void DistanceVector::ProcessIncomingRoutingMessage(RoutingMessage *m) {
    cerr << *this << " got a routing message: " << *m << " (ignored)" << endl;
}

void DistanceVector::TimeOut() {
    cerr << *this << " got a timeout: (ignored)" << endl;
}

Node* DistanceVector::GetNextHop(Node *destination) { 
    unsigned n = routing_table.hops[(*destination).GetNumber()];
    deque<Node*> mydeque = *GetNeighbors();
    deque<Node*>::iterator i = mydeque.begin();
    while(i != mydeque.end()){
        if((*i).GetNumber() == n){
            Node nextHop = new Node(i);
            return nextHop;
        }
        i++;
    }
    return NULL;
}

Table* DistanceVector::GetRoutingTable() {
    return Table *copy = new Table(routing_table);
}

ostream & DistanceVector::Print(ostream &os) const { 
    Node::Print(os);
    return os;
}

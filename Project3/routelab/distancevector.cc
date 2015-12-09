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
    deque<Node*> mynodes = *GetNeighbors();
    deque<Link*> mylinks = *GetOutgoingLinks();
    //Make an iterator for that deque
    deque<Node*>::iterator i = mynodes.begin();
    while(i != mynodes.end()){
        cerr << "\nNeighbor: " << **i++;
    }
    deque<Link*>::iterator j = mylinks.begin();
    while(j != mylinks.end()){
        cerr << "\nLink: " << **j++;
    }
}

void DistanceVector::ProcessIncomingRoutingMessage(RoutingMessage *m) {
    //cerr << *this << " got a routing message: " << *m << " (ignored)" << endl;
    // deque<Node*> mydeque = *GetNeighbors();
    // //Make an iterator for that deque
    // deque<Node*>::iterator i = mydeque.begin();
    // while(i != mydeque.end()){
    //     cerr << "\nNeighbor: " << **i++;
    // }
}

void DistanceVector::TimeOut() {
    cerr << *this << " got a timeout: (ignored)" << endl;
}

Node* DistanceVector::GetNextHop(Node *destination) { 
    
    deque<Node*> mydeque = *GetNeighbors();
    unsigned n = routing_table->GetNext(destination->GetNumber());
    return NULL;
}

Table* DistanceVector::GetRoutingTable() {
    return new Table(routing_table);
}

ostream & DistanceVector::Print(ostream &os) const { 
    Node::Print(os);
    return os;
}

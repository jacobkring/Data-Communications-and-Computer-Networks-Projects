#include "node.h"
#include "context.h"
#include "error.h"
#include "linkstate.h"
#include "distancevector.h"

Node::Node(unsigned n, SimulationContext *c, double b, double l) : 
    number(n), context(c), bw(b), lat(l), routing_table() 
{}

Node::Node() {
    throw GeneralException(); 
}

Node::Node(const Node &rhs) : 
  number(rhs.number), context(rhs.context), bw(rhs.bw), lat(rhs.lat),
  routing_table(rhs.routing_table) {}

Node & Node::operator=(const Node &rhs) 
{
  return *(new(this)Node(rhs));
}

void Node::SetNumber(unsigned n) 
{ number=n;}

unsigned Node::GetNumber() const 
{ return number;}

void Node::SetLatency(double l)
{ lat=l;}

double Node::GetLatency() const 
{ return lat;}

void Node::SetBW(double b)
{ bw=b;}

double Node::GetBW() const 
{ return bw;}

Node::~Node()
{}

// These functions post events to the event queue in the event simulator so that the 
// corresponding node(s) can recieve the ROUTING_MESSAGE_ARRIVAL event(s) at the proper time(s)
void Node::SendToNeighbors(RoutingMessage* m)
{
    deque<Link*> links = *GetOutgoingLinks();
    deque<Node*> neighbors = *GetNeighbors();
    deque<Link*>::iterator it_l;
    deque<Node*>::iterator it_n;

    for (it_n = neighbors.begin(); it_n != neighbors.end(); it_n++) {
        // For now, find link to get the correct latency
        // In future, latencies can be gotten from routing table
        for (it_l = links.begin(); it_l != links.end(); it_l++) {
            if ((*it_n)->GetNumber() == (*it_l)->GetDest()) {
                context->PostEvent(
                    new Event(context->GetTime() + (*it_l)->GetLatency(), ROUTING_MESSAGE_ARRIVAL, 
                        (void*)*it_n, (void*)m)
                );
                break;
            }
        }
    }
}

void Node::SendToNeighbor(Node* n, RoutingMessage* m)
{
    deque<Link*> links = *GetOutgoingLinks();
    deque<Link*>::iterator it;

    // For now, find link to get the correct latency
    // In future, latencies can be gotten from routing table
    for (it = links.begin(); it != links.end(); it++) {
        if (n->GetNumber() == (*it)->GetDest()) {
            context->PostEvent(
                new Event(context->GetTime() + (*it)->GetLatency(), ROUTING_MESSAGE_ARRIVAL, 
                    (void*)n, (void*)m)
            );
            break;
        }
    }
}

deque<Node*> *Node::GetNeighbors()
{
    return context->GetNeighbors(this);
}

deque<Link*> *Node::GetOutgoingLinks()
{
    return context->GetOutgoingLinks(this);
}

void Node::SetTimeOut(double timefromnow)
{
    context->TimeOut(this,timefromnow);
}

bool Node::Matches(Node* rhs)
{
    return number == rhs->number;
}

bool Node::Matches(unsigned rhs)
{
    return number == rhs;
}

/** Implementations of virtual functions for GENERIC use **/
void Node::LinkHasBeenUpdated(Link *l)
{
    cerr << *this << " got a link update: "<<*l<<endl;
}

void Node::ProcessIncomingRoutingMessage(RoutingMessage *m)
{
    cerr << *this << " got a routing messagee: "<<*m<<" Ignored "<<endl;
}


void Node::TimeOut()
{
    cerr << *this << " got a timeout: ignored"<<endl;
}

Node *Node::GetNextHop(Node *destination)
{
    return NULL;
}

Table *Node::GetRoutingTable()
{
    return NULL;
}

ostream & Node::Print(ostream &os) const
{
    os << "Node(number="<<number<<", lat="<<lat<<", bw=" << bw
        << ", routing_table=" << routing_table << ")";
    return os;
}

Node* createNode(unsigned n, SimulationContext *c, double b, double l) {
#if defined(LINKSTATE)
    return new LinkState(n, c, b, l);
#elif defined(DISTANCEVECTOR)
    return new DistanceVector(n, c, b, l);
#else
    return new Node(n, c, b, l);
#endif
}


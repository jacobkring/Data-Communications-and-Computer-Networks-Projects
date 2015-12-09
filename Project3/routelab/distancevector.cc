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

    int destination = (*l).GetDest();
    int latency = (*l).GetLatency();

    routing_table.neighbors[destination].cost = latency;
    routing_table.distance[destination].cost = -1;
    routing_table.topo[destination][destination].cost = 0;

    if(calculateDistance())
        SendToNeighbors(new RoutingMessage(number, routing_table.distance));


}

void DistanceVector::ProcessIncomingRoutingMessage(RoutingMessage *m) {
    cerr << *this << " got a routing message: " << *m << endl;

    int from_node = m->sendernode;

    map<int,TopoLink> vectors = m->vectors;
    map<int,TopoLink>::const_iterator msg_iter = vectors.begin();

    while(msg_iter != vectors.end()){
        routing_table.topo[from_node][msg_iter->first].cost = msg_iter->second.cost;
        int temp = routing_table.distance[msg_iter->first].cost;
        msg_iter++;
    }

    if(calculateDistance())
        SendToNeighbors(new RoutingMessage(number, routing_table.distance));
    
    
}

void DistanceVector::TimeOut() {
    cerr << *this << " got a timeout: (ignored)" << endl;
}

Node* DistanceVector::GetNextHop(Node *destination) { 

    cerr << "Next hop from current node " << number << " to destination " << (*destination).GetNumber() << "\n";
    unsigned n = routing_table.hops[(*destination).GetNumber()];
    cerr << "Next hop is " << n << "\n";
    deque<Node*> mydeque = *GetNeighbors();
    deque<Node*>::iterator i = mydeque.begin();
    Node *nextHop;
    while(i != mydeque.end()){
        if((*i)->GetNumber() == n){
            nextHop = new Node((*i)->GetNumber(), context, (*i)->GetBW(), (*i)->GetLatency());
        }
        i++;
    }
    return nextHop;
}

Table* DistanceVector::GetRoutingTable() {
    Table* table = new Table(routing_table);
    return table;
}

ostream & DistanceVector::Print(ostream &os) const { 
    Node::Print(os);
    return os;
}

bool DistanceVector::calculateDistance(){
    bool newDistance = false;

    map<int, TopoLink>::const_iterator distance_iter;
    map<int, TopoLink>::const_iterator neighbors_iter;

    for(distance_iter = routing_table.distance.begin(); distance_iter != routing_table.distance.end(); distance_iter++){

        unsigned cur_node_num = distance_iter->first;
        // when node number is equal to current node then this cost is 0
        if(cur_node_num==number){
            routing_table.distance[cur_node_num].cost = 0;
        }
        else{
            int cur_neighbor = -1;
            int cur_distance = -1;
            int min_distance = -1;
            bool begin= true;
            int min_neighbor = -1;

            // computing cost to neighbor and their distance vector to destination
            for(neighbors_iter = routing_table.neighbors.begin(); neighbors_iter != routing_table.neighbors.end(); neighbors_iter++){
                cur_neighbor = neighbors_iter->first;

                if(routing_table.neighbors[cur_neighbor].cost != -1 && routing_table.topo[cur_neighbor][cur_node_num].cost != -1){
                    cur_distance = routing_table.neighbors[cur_neighbor].cost + routing_table.topo[cur_neighbor][cur_node_num].cost;
                    if(begin || cur_distance < min_distance){
                        begin = false;
                        min_distance = cur_distance;
                        min_neighbor = cur_neighbor;
                    }
                
                }
            }
            if(!begin && min_distance != routing_table.distance[cur_node_num].cost){
                routing_table.distance[cur_node_num].cost = min_distance;
                routing_table.hops[cur_node_num] = min_neighbor;
                newDistance = true;
            }
        }
    }

    return newDistance;
}

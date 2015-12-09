#include "linkstate.h"

#define INFINITE 9999999

LinkState::LinkState(unsigned n, SimulationContext* c, double b, double l) :
    Node(n, c, b, l)
{}

LinkState::LinkState(const LinkState & rhs) :
    Node(rhs)
{
    *this = rhs;
}

LinkState & LinkState::operator=(const LinkState & rhs) {
    Node::operator=(rhs);
    return *this;
}

LinkState::~LinkState() {}


/** Write the following functions.  They currently have dummy implementations **/
void LinkState::LinkHasBeenUpdated(Link* l) {
    cerr << *this << ": Link Update: " << *l << endl;
    RoutingMessage *m = new RoutingMessage(routing_table.UpdateLink(l), l->GetSrc(), l->GetDest(), l->GetLatency());
    SendToNeighbors(m);
}

void LinkState::ProcessIncomingRoutingMessage(RoutingMessage *m) {
    cerr << *this << " got a routing message: " << *m << " (ignored)" << endl;
    //If a shorter path is found, send this update to neighbors
    if(routing_table.UpdateTable(m)){
        cerr << "Beep" << endl;
        SendToNeighbors(m);
    }
}

void LinkState::TimeOut() {
    cerr << *this << " got a timeout: (ignored)" << endl;
}

Node* LinkState::GetNextHop(Node *destination) { 
    int num_nodes = (routing_table.GetTopo()).size();

    //We need 3 vectors to store Djikstra's Algorithm values
    vector<int> dv(num_nodes, INFINITE); //Distance vectors
    vector<int> pv(num_nodes, INFINITE); //Previous node
    vector<bool> is_visited(num_nodes, false); //Denotes if node has been visited

    return DjikstraTime(dv, pv, is_visited, destination, num_nodes);
    
}

Table* LinkState::GetRoutingTable() {
    Table* table = new Table(routing_table);
    return table;
}

ostream & LinkState::Print(ostream &os) const { 
    Node::Print(os);
    return os;
}

Node* LinkState::DjikstraTime(vector<int> dv, vector<int>pv, vector<bool>is_visited, Node *dest, int num_nodes){
    //Get the current node
    int curr_distance = dv[GetNumber()] = 0;
    is_visited[GetNumber()] = true;
    pv[GetNumber()] = INFINITE;

    //Add the node's neighbors to the table
    for(map<int, TopoLink>::const_iterator i = routing_table.GetTopo()[GetNumber()].begin(); i != routing_table.GetTopo()[GetNumber()].end(); i++){
        dv[i->first] = i->second.cost;
        pv[i->first] = GetNumber();
    }

    //Next, find the node with the minimum distance to the current node
    for(int j = 0; j < num_nodes - 1; j++){
        int curr_node;
        int distance = INFINITE;
        //If new node in dv is not visited and distance is smaller than tracked distance,
        //Set that node as the current node and that distance as the tracked distance
        for(int j = 0; j < dv.size(); j++){
            if(is_visited[j] == false && distance > dv[j]){
                curr_node = j;
                distance = dv[j];
            }
        }

        //Visit the closest node, set the current distance to the min distance from previous node
        is_visited[curr_node] = true;
        if(distance != INFINITE){
            curr_distance = distance;
        }

        //Add closest node's neighbors to the table
        for(map<int, TopoLink>::const_iterator i = routing_table.GetTopo()[curr_node].begin(); i != routing_table.GetTopo()[curr_node].end(); i++){
            if(dv[i->first] > (i->second.cost + curr_distance) && is_visited[i->first] == false){
                dv[i->first] = i->second.cost + curr_distance;
                pv[i->first] = curr_node;
            }
        }
    }

    //Cycle back through vectors to find the next hop
    unsigned int curr_node = dest->GetNumber();
    unsigned int curr_parent = pv[dest->GetNumber()];
    while(curr_parent != GetNumber()){
        curr_node = curr_parent;
        curr_parent = pv[curr_node];
    }

    unsigned int desired_node_number = dest->GetNumber();
    unsigned int desired_node_parent = pv[desired_node_number];
    while(desired_node_parent != GetNumber()){
        desired_node_number = desired_node_parent;
        desired_node_parent = pv[desired_node_number];
    }

    //Find the node to return
    deque<Node*> *neighbors = this->GetNeighbors();
    for(deque<Node*>::const_iterator i = neighbors->begin(); i != neighbors->end(); i++){
        if(Node(curr_node, 0, 0, 0).Matches(*i)){
            return new Node(**i);
        }
    }

    return NULL;
}

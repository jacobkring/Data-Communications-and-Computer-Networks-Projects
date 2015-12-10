#include "linkstate.h"
#include "context.h"

#define INFINITE 99999

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
        SendToNeighbors(m);
    }
}

void LinkState::TimeOut() {
    cerr << *this << " got a timeout: (ignored)" << endl;
}

Node* LinkState::GetNextHop(Node *destination) { 
    int num_nodes = (routing_table.GetTopo()).size();

    set<int> nodes;
    set<int> is_visited;

    return DjikstraTime(nodes, is_visited, destination, num_nodes);
    
}

Table* LinkState::GetRoutingTable() {
    Table* table = new Table(routing_table);
    return table;
}

ostream & LinkState::Print(ostream &os) const { 
    Node::Print(os);
    return os;
}

Node* LinkState::DjikstraTime(set<int> nodes, set<int>is_visited, Node *dest, int num_nodes){
    map<int,int> dist; 
    map<int,int> prev;
    map<int, TopoLink> copy;

    map<int,int>::const_iterator dist_iter;
    map<int, TopoLink>::const_iterator copy_iter;
    map<int,int>::const_iterator prev_iter;
    set<int>::const_iterator node_iter;
    set<int>::const_iterator visited_iter;

    dist[number] = 0;
    prev[number] = number;
    
    //Set other dist to +INF. Set the previous paths to -(99999). 
    for (int i=0; i<num_nodes; i++)
    {
        if (i !=number)
        {
        dist[i] = INFINITE;
        prev[i] = -(INFINITE);
        }
    }
    
    //Start with this node 
    copy = routing_table.topo[number];
    is_visited.insert(number);
    
    //Iterate through this node's neighbors
    for (copy_iter = copy.begin(); copy_iter != copy.end(); copy_iter++)
    {
        int v = copy_iter->first; //Get the node number
        //Iterator will return the key_value pair. Check whether the key is visited
        if (is_visited.count(copy_iter->first) < 1 && (dist[number] + routing_table.topo[number][v].cost) < dist[v])
        {
            dist[v] = (dist[number] + routing_table.topo[number][v].cost);
            prev[v] = number;
            //Add this neighbor to our set of nodes
            nodes.insert(v);        
        }
    }

    while (! nodes.empty() ){
        int smallest = GetCurrentSmallestDistance(is_visited, nodes, dist);
        nodes.erase(smallest);
        is_visited.insert(smallest);
        copy = routing_table.topo[smallest];
        //Repeat the loop that we ran in the initialization step basically
           for (copy_iter = copy.begin(); copy_iter != copy.end(); copy_iter++)
            {
                //Iterator will return the key_value pair. Check the key is visited
                if (is_visited.count(copy_iter->first) < 1)
                {
                int v = copy_iter->first; //Get the key (i.e. node number)
                    if ( (dist[smallest] + routing_table.topo[smallest][v].cost) < dist[v])
                    {
                        dist[v] = (dist[smallest] + routing_table.topo[smallest][v].cost);
                        prev[v] = smallest;
                        //Add this neighbor to our queue
                        nodes.insert(v);
                    }
                            
                }

            }
                   
    }  
        //Djikstra's is done. Distances should hold the distances from this node.
        //Make a new copy of local_map

        map<int, int> new_map;
        new_map = CreateNewMap(prev, number);

        routing_table.linkstate_map = new_map;
        int y= dest->GetNumber();
        
        Node* target = new Node(new_map[y], NULL, 0, 0);
        Node* result = context->FindMatchingNode(const_cast<Node *>(target));
        return result;        
}

map<int, int> LinkState::CreateNewMap(map<int,int> prevpaths, int node_number)
{
    map<int, int> result;
    map<int, int>::const_iterator prevpaths_it; 
    int dest;
    int prev;
    int prev_prev;
    for (prevpaths_it = prevpaths.begin(); prevpaths_it != prevpaths.end(); prevpaths_it++)
    {
       //The key to the previous vector is the node which has the value as its previous 
       dest = prevpaths_it->first; 
       //The value is the last previous node calculated during Djikstra's
       prev_prev = prevpaths_it->second;
     
       if (prev_prev == node_number)
            result[dest] = dest;
       else
       {
            prev = prevpaths[prev_prev];
            while (prev != node_number)
            {
                prev_prev = prev;
                prev = prevpaths[prev];            
            }
            result[dest] = prev_prev;
       }
    }
    return result;
}

int LinkState::GetCurrentSmallestDistance(set<int> is_visited, set<int> nodes, map<int,int> dist)
{
    int min = INFINITE;
    int min_node = -1;
    for (int i=0; i< routing_table.topo.size(); i++)
    {        
        set<int>::const_iterator nodes_iter;    
        set<int>::const_iterator visited_iter;
        nodes_iter = nodes.find(i);

        /*If the current node is in the queue and unvisited, check to see if it has the min distance
          If so, track it.*/
        if (nodes_iter != nodes.end())
        {
            visited_iter = is_visited.find(i);
            if (visited_iter == is_visited.end())
            {
                if (dist[i] < min)
                {
                min = dist[i];
                min_node = i;
                }
            }
        }
    }
    if (min == INFINITE) 
        return -1; //Some error has happened
    else
        return min_node;
}



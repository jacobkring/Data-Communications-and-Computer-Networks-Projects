#include "linkstate.h"
#include "context.h"

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
        dist[i] = 99999;
        prev[i] = -(99999);
        }
    }
    
    //Start with this node 
    copy = routing_table.topo[number];
    is_visited.insert(number);
    
    //Iterate through this node's neighbors
    for (copy_iter = copy.begin(); copy_iter != copy.end(); copy_iter++)
    {
        //Iterator will return the key_value pair. Check whether the key is visited
        if (is_visited.count(copy_iter->first) < 1)
        {
        int v = copy_iter->first; //Get the key (i.e. node number)
            //D(v) = min( D(w) + c(w,v) , D(v) )
            if ( (dist[number] + routing_table.topo[number][v].cost) < dist[v])
            {
                dist[v] = (dist[number] + routing_table.topo[number][v].cost);
                prev[v] = number;
                //Add this neighbor to our queue
                nodes.insert(v);
            }
                    
        }
    
    }

    while (! nodes.empty() ){
        int x = GetCurrentSmallest(is_visited, nodes, dist);
        nodes.erase(x);
        is_visited.insert(x);
        copy = routing_table.topo[x];
        //Repeat the loop that we ran in the initialization step basically
           for (copy_iter = copy.begin(); copy_iter != copy.end(); copy_iter++)
            {
                //Iterator will return the key_value pair. Check the key is visited
                if (is_visited.count(copy_iter->first) < 1)
                {
                int v = copy_iter->first; //Get the key (i.e. node number)
                    //D(v) = min( D(w) + c(w,v) , D(v) )
                    if ( (dist[x] + routing_table.topo[x][v].cost) < dist[v])
                    {
                        dist[v] = (dist[x] + routing_table.topo[x][v].cost);
                        prev[v] = x;
                        //Add this neighbor to our queue
                        nodes.insert(v);
                    }
                            
                }

            }
                   
    }  
        //Djikstra's is done. Distances should hold the distances from this node. Now need to recursively go through previous to figure out proper routing.
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
    int errorcounter = 0;
    int destination_node;
    int current_previous;
    int old_previous;
    for (prevpaths_it = prevpaths.begin(); prevpaths_it != prevpaths.end(); prevpaths_it++)
    {
       // cout<<"For loop iteration!\n";
       //The key to the previous vector is the node which has the value as its previous 
       destination_node = prevpaths_it->first; 
       //The value is the last previous node calculated during Djikstra's
       old_previous = prevpaths_it->second;
     
       if (old_previous == node_number)
            result[destination_node] = destination_node; //Directly routing there is fastest
       
       else
       {
        //     cout <<"Current destination_node:" << destination_node << "\n";
        //     cout <<"Current current_previous:" << current_previous << "\n";
        // cout <<"Current old_previous:" << old_previous <<"\n";
            current_previous = prevpaths[old_previous];
            while (current_previous != node_number)
            {
                old_previous = current_previous;
                current_previous = prevpaths[current_previous];            
                errorcounter++;
                if (errorcounter <=10)
                  {
                  // cout <<"End of while loop:\n";
                  // cout <<"Node_number: " << node_number << "\n";
              //     cout <<"Now current_previous is:" << current_previous << "\n";
              //     cout <<"Now old_previous is:" << old_previous << "\n";
                  }
            }
            result[destination_node] = old_previous;
       }
    
    }

    return result;
}

int LinkState::GetCurrentSmallest(set<int> visited, set<int> nodes, map<int,int> distances)
{
//    int error_loop_counter = 0;
    int min_dist = 99999;
    int min_node = -1;
    for (int i=0; i< routing_table.topo.size(); i++)
    {        
        set<int>::const_iterator nodes_it;    
        set<int>::const_iterator visit_it;
        nodes_it = nodes.find(i);      
        //If the current i is in the node queue
        if (nodes_it != nodes.end())
        {
            //If the current i is unvisited, e.g., it is not found when you search visited
            visit_it = visited.find(i);
            if (visit_it == visited.end())
            {
                if (distances[i] < min_dist)
                {
                min_dist = distances[i];
                min_node = i;
                }
            
            }
        
        }
    
    }
    if (min_dist == 99999) 
        return -1; //An error has occurred; this shouldn't happen
    else
        return min_node;
}



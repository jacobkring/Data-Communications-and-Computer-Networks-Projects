#ifndef _linkstate
#define _linkstate

#include "node.h"
#include <vector>
#include <set>

class LinkState: public Node {
    private:
        // Anything you need in addition to Node members

    public:
        LinkState(unsigned, SimulationContext* , double, double);
        LinkState(const LinkState &);
        LinkState & operator=(const LinkState &);
        ~LinkState();

        // Inherited from Node
        void LinkHasBeenUpdated(Link *l);
        void ProcessIncomingRoutingMessage(RoutingMessage *m);
        void TimeOut();
        Node* GetNextHop(Node* destination);
        Table* GetRoutingTable();
        ostream & Print(ostream & os) const;

        // Anything else
        Node* DjikstraTime(set<int> nodes, set<int> is_visited, Node *dest, int num_nodes);
        map<int, int> CreateNewMap(map<int,int> prevpaths, int node_number);
        int GetCurrentSmallest(set<int> visited, set<int> nodes, map<int,int> distances);
};

inline ostream & operator<<(ostream & os, const LinkState & n) {
    return n.Print(os);
}

#endif

#ifndef _linkstate
#define _linkstate

#include "node.h"
#include <vector>

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
        Node* DjikstraTime(vector<int>, vector<int>, vector<bool>, Node *dest, int num_nodes);
};

inline ostream & operator<<(ostream & os, const LinkState & n) {
    return n.Print(os);
}

#endif

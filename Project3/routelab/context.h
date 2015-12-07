#ifndef _context
#define _context

#include <new>
#include <string>
#include <iostream>
#include <map>
#include <stdlib.h>
#include <string.h>
#include <stdio.h>

#include "topology.h"
#include "eventqueue.h"

class SimulationContext : public Topology, public EventQueue
{
    public:
        SimulationContext();
        virtual ~SimulationContext();

        void LoadTopology(string file);
        void LoadEvents(string file);
        void Init();

        void TimeOut(Node *src, double timefromnow);
        void CollectPathLinks(Node src, Node dest, deque<Link> &links);
        void DrawShortestPathTree(Node* node);
        void WriteShortestPathTreeDot(Node node, string s);
        void DrawPath(Link *l);
        void WritePathDot(Node src, Node dest, string s);
        void DumpTable(Node *src);
        void DispatchEvent(Event *e);
        ostream &Print(ostream &os);
};

inline ostream & operator<<(ostream &os, SimulationContext &s) {
    return s.Print(os);
}

#endif

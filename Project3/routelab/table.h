#ifndef _table
#define _table

#include <iostream>
#include <map>
#include <link.h>
#include "link.h"


using namespace std;
class RoutingMessage;

struct TopoLink {
    TopoLink(): cost(-1), age(0) {}

    TopoLink(const TopoLink & rhs) {
        *this = rhs;
    }

    TopoLink & operator=(const TopoLink & rhs) {
        this->cost = rhs.cost;
        this->age = rhs.age;

        return *this;
    }

    int cost;
    int age;
};

// Students should write this class
class Table {
    private:
        map < int, map < int, TopoLink > > topo;
    public:
        Table();
        Table(const Table &);
        Table & operator=(const Table &);

        ostream & Print(ostream &os) const;

        // Anything else you need

        #if defined(LINKSTATE)
        int UpdateLink(const Link *l);
        bool UpdateTable(const RoutingMessage *m);
        map < int, map < int, TopoLink > > GetTopo();
        void SetTopo(int source, int dest, int age, int cost);
        #endif

        #if defined(DISTANCEVECTOR)
        #endif
};

inline ostream & operator<<(ostream &os, const Table & t) { return t.Print(os);}


#endif

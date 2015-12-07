#ifndef _table
#define _table

#include <iostream>
#include <map>
#include <link.h>


using namespace std;

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
    public:
        Table();
        Table(const Table &);
        Table & operator=(const Table &);

        map < int, map < int, TopoLink > > topo;

        ostream & Print(ostream &os) const;

        // Anything else you need

        #if defined(LINKSTATE)
        #endif

        #if defined(DISTANCEVECTOR)

        // distance vector to each node
        map <int, TopoLink> distance;
        // length between node and neighbor nodes
        map <int, TopoLink> neighbors;
        // record of next hop
        map <int, int> hops;

        #endif
};

inline ostream & operator<<(ostream &os, const Table & t) { return t.Print(os);}


#endif
#ifndef _messages
#define _messages

#include <iostream>
#include "node.h"
#include "link.h"

struct RoutingMessage {
    RoutingMessage();
    RoutingMessage(const RoutingMessage &rhs);
    RoutingMessage &operator=(const RoutingMessage &rhs);

    ostream & Print(ostream &os) const;

    // Anything else you need

    #if defined(LINKSTATE)
    #endif
    #if defined(DISTANCEVECTOR)
        RoutingMessage(int sendnode, map <int, TopoLink> vect);
        int sendernode;
        map <int, TopoLink> vectors;
    #endif
};

inline ostream & operator<<(ostream &os, const RoutingMessage & m) { return m.Print(os);}

#endif

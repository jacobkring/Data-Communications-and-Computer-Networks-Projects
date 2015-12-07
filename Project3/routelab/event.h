#ifndef _event
#define _event

#include <iostream>
#include <queue>
#include <stdlib.h>

#include "messages.h"
#include "topology.h"

enum EventType { ADD_NODE, DELETE_NODE, ADD_LINK, DELETE_LINK,  
       CHANGE_NODE, CHANGE_LINK, DRAW_TOPOLOGY, DRAW_TREE, DRAW_PATH,
       DUMP_TABLE, ROUTING_MESSAGE_ARRIVAL, TIMEOUT, PRINT
}; 

class Event {
    private:
        double timestamp;
        EventType etype;
        void  *handler;
        void  *data;

    public:
        Event(double t, EventType e, void *handler, void *d);
        // default copy construct and operator= is OK
        virtual ~Event();

        double GetTimeStamp();

        void Dispatch();
        void Disassociate();

        bool IsLater(Event *rhs);

        ostream & Print(ostream &os) const;
};

inline ostream & operator<<(ostream &os, const Event &e) {
    return e.Print(os);
}

struct CompareEvents : public binary_function<Event *,Event *, bool> {
    inline bool operator ()(Event *lhs, Event *rhs) {
        return lhs->IsLater(rhs);
    }
};

#endif

#ifndef _eventqueue
#define _eventqueue

#include <vector>
#include <iostream>

#include "event.h"

class EventQueue {
    private:
        // Will be maintained in Heap Order
        priority_queue<Event*, deque<Event *>, CompareEvents> q;
        double curtime;
    public:
        virtual ~EventQueue();
        void  PostEvent(Event* e);
        double GetTime();
        Event* GetEarliestEvent();
        ostream &Print(ostream & os);
};

inline ostream & operator<<(ostream &os, EventQueue &e) {
    return e.Print(os);
}

#endif

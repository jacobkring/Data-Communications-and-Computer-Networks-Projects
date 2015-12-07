#include "eventqueue.h"


void EventQueue::PostEvent(Event *e) {
  q.push(e);
}

Event * EventQueue::GetEarliestEvent() {
  if (q.size()>0) {
    Event *e=q.top();
    q.pop();
    curtime=e->GetTimeStamp();
    return e;
  } else {
    return 0;
  }
}

double EventQueue::GetTime()
{
  return curtime;
}

EventQueue::~EventQueue() {
  Event *e;
  while ((e=GetEarliestEvent())) {
    delete e;
  }
}

ostream & EventQueue::Print(ostream &os)
{
  Event *e;
  deque<Event *> tq;

  os <<"EventQueue(curtime="<<curtime<<", eventlist={";
  while ((e=GetEarliestEvent())) {
    tq.push_back(e);
  }
  while (tq.size()>0) { 
    e=tq.front();
    tq.pop_front();
    os << (*e)<<", ";
    q.push(e);
  }
  os << "})";
  return os;
}


#include "event.h"
#include "context.h"

Event::Event(double t, EventType e, void *h, void *d) :
    timestamp(t), etype(e), handler(h), data(d) {}

void Event::Dispatch()
{
    switch (etype) { 
        case DRAW_TOPOLOGY:
            ((Topology*)handler)->DrawTopology();
            break;
        case DRAW_TREE:
            ((SimulationContext*)handler)->DrawShortestPathTree((Node*)data);
            break;
        case DRAW_PATH:
            ((SimulationContext*)handler)->DrawPath((Link*)data);
            break;
        case DUMP_TABLE: 
            ((SimulationContext*)handler)->DumpTable((Node*)data);
            break;

#if 0
        case WRITE_TOPOLOGY:
            ((Topology*)handler)->WriteDot(*(string*)data);
            break;
        case WRITE_TREE:
            ((SimulationContext*)handler)->WriteShortestPathTreeDot(*(string*)data);
            break;
#endif

        case ADD_NODE:
            ((Topology*)handler)->AddNode((Node*)data);
            break;
        case DELETE_NODE:
            ((Topology*)handler)->DeleteNode((Node*)data);
            break;
        case ADD_LINK:
            ((Topology*)handler)->AddLink((Link*)data);
            break;
        case DELETE_LINK:
            ((Topology*)handler)->DeleteLink((Link*)data);
            break;
        case CHANGE_NODE:
            ((Topology*)handler)->ChangeNode((Node*)data);
            break;
        case CHANGE_LINK:
            ((Topology*)handler)->ChangeLink((Link*)data);
            break;
        case TIMEOUT:
            ((Node*)handler)->TimeOut();
            break;
        case ROUTING_MESSAGE_ARRIVAL:
            ((Node*)handler)->ProcessIncomingRoutingMessage((RoutingMessage*)data);
            break;
        case PRINT:
            cout << ((char*) data);
            break;
        default:
            cerr <<"Unknown event type\n";
            exit(-1);
    }
}



ostream & Event::Print(ostream &os) const
{
    os << "Event(timestamp=" << timestamp << ", etype=" <<
        (etype==ADD_NODE ? "ADD_NODE" :
        etype==DELETE_NODE ? "DELETE_NODE" :
        etype==ADD_LINK ? "ADD_LINK" :
        etype==DELETE_LINK ? "DELETE_LINK" :
        etype==CHANGE_NODE ? "CHANGE_NODE" :
        etype==CHANGE_LINK ? "CHANGE_LINK" :
#if 0
        etype==WRITE_TOPOLOGY ? "WRITE_TOPOLOGY" :
        etype==WRITE_TREE ? "WRITE_TREE" :
#endif
        etype==DRAW_TOPOLOGY ? "DRAW_TOPOLOGY" :
        etype==DRAW_TREE ? "DRAW_TREE" :
        etype==DRAW_PATH ? "DRAW_PATH" :
        etype==DUMP_TABLE ? "DUMP_TABLE" :
        etype==TIMEOUT ? "TIMEOUT" :
        etype==PRINT ? "PRINT" :
        etype==ROUTING_MESSAGE_ARRIVAL ? "ROUTING_MESSAGE_ARRIVAL" :
        "UNKNOWN") << ", ";

    switch (etype) { 
        case DRAW_TOPOLOGY:
            break;
        case ADD_NODE:
        case DELETE_NODE:
        case CHANGE_NODE:
        case DRAW_TREE:
        case DUMP_TABLE:
        case TIMEOUT:
            os << *((Node*)data);
            break;
        case DRAW_PATH:
            os << *((Link*)data);
            break;
#if 0
        case WRITE_TOPOLOGY:
        case WRITE_TREE:
            os <<*((string*)data);
            break;
#endif
        case ADD_LINK:
        case DELETE_LINK:
        case CHANGE_LINK:
            os << *((Link*)data);
            break;
        case ROUTING_MESSAGE_ARRIVAL:
            os << *((RoutingMessage*)data);
            break;
        case PRINT:
            os << "\"" << ((char*)data) << "\"";
            break;
        default:
            os << "Unknown()";
    }

    os <<")";
    return os;
}


void Event::Disassociate()
{
    handler = 0;
    data = 0;
}

double Event::GetTimeStamp()
{
    return timestamp;
}


Event::~Event()
{
    if (data) { 
        switch (etype) { 
            case DRAW_TOPOLOGY:
                break;
#if 0
            case WRITE_TOPOLOGY:
            case WRITE_TREE:
            delete (string *)data;
            break;
#endif
            case DRAW_TREE:
            case ADD_NODE:
            case DELETE_NODE:
            case CHANGE_NODE:
            case DUMP_TABLE:
            case TIMEOUT:
                delete (Node *) data;
                break;
            case ADD_LINK:
            case DELETE_LINK:
            case CHANGE_LINK:
            case DRAW_PATH:
                delete (Link *) data;
                break;
            case ROUTING_MESSAGE_ARRIVAL:
                delete (RoutingMessage*) data;
                break;
            case PRINT:
                delete [] (char*) data;
                break;
            default:
                cerr <<"Unknown event type\n";
                exit(-1);
        }
    }
}

bool Event::IsLater(Event *rhs)
{
    return timestamp>rhs->timestamp;
}

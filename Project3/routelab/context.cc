#include "context.h"
#include "node.h"
using namespace __gnu_cxx;

SimulationContext::SimulationContext() {}

SimulationContext::~SimulationContext() {}

void SimulationContext::LoadEvents(string file)
{
    FILE* in;
    char b[1024];
    char *buf;
    char cmd[1024];
    unsigned num;
    unsigned src,dest;
    double lat, bw;
    double timestamp;


    if ((in = fopen(file.c_str(), "r"))==0) { 
        cerr << "Can't read events from "<<file<<endl;
        exit(-1);
    }

    while (!feof(in)) {
        buf = b;

        if (!fgets(buf,1024,in)) {
            break;
        }
        if (strlen(buf)==0){
            continue;
        }
        while (isspace(*buf)) { 
            buf++;
        }
        if (*buf==0) {
            continue;
        }
        if (toupper(buf[0])=='#') {
            continue;
        }

        //fprintf(stderr,"%s",buf);
        sscanf(buf,"%lf %s",&timestamp,cmd);
        if (!strcasecmp(cmd,"DRAW_TOPOLOGY")) {
            sscanf(buf,"%lf %s",&timestamp,cmd);
            PostEvent(new Event(timestamp,DRAW_TOPOLOGY,this,0));
            continue;
        }
        if (!strcasecmp(cmd,"DUMP_TABLE")) {
            sscanf(buf,"%lf %s %u",&timestamp,cmd,&num);
            PostEvent(new Event(timestamp,DUMP_TABLE,this,createNode(num,this,0,0)));
            continue;
        }
        if (!strcasecmp(cmd,"DRAW_TREE")) {
            sscanf(buf,"%lf %s %u",&timestamp,cmd,&num);
            PostEvent(new Event(timestamp,DRAW_TREE,this,createNode(num,this,0,0)));
            continue;
        }
        if (!strcasecmp(cmd,"DRAW_PATH")) {
            sscanf(buf,"%lf %s %u %u",&timestamp,cmd,&src,&dest);
            PostEvent(new Event(timestamp,DRAW_PATH,this,new Link(src,dest,this,0,0)));
            continue;
        }
#if 0
        if (!strcasecmp(cmd,"WRITE_TOPOLOGY")) {
            char file[1024];
            sscanf(buf,"%lf %s %s",&timestamp,cmd,file);
            PostEvent(new Event(timestamp,WRITE_TOPOLOGY,this,new string(file)));
            continue;
        }
        if (!strcasecmp(cmd,"WRITE_TREE")) {
            char file[1024];
            sscanf(buf,"%lf %s %s",&timestamp,cmd,file);
            PostEvent(new Event(timestamp,WRITE_TREE,this,new string(file)));
            continue;
        }
#endif
        if (!strcasecmp(cmd,"ADD_NODE")) {
            sscanf(buf,"%lf %s %u %lf %lf",&timestamp,cmd,&num,&lat,&bw);
            PostEvent(new Event(timestamp,ADD_NODE,this,createNode(num,this,bw,lat)));
            continue;
        }
        if (!strcasecmp(cmd,"DELETE_NODE")) {
            sscanf(buf,"%lf %s %u %lf %lf",&timestamp,cmd,&num,&lat,&bw);
            PostEvent(new Event(timestamp,DELETE_NODE,this,createNode(num,this,bw,lat)));
            continue;
        }
        if (!strcasecmp(cmd,"CHANGE_NODE")) {
            sscanf(buf,"%lf %s %u %lf %lf",&timestamp,cmd,&num,&lat,&bw);
            PostEvent(new Event(timestamp,CHANGE_NODE,this,createNode(num,this,bw,lat)));
            continue;
        }
        if (!strcasecmp(cmd,"ADD_LINK")) {
            sscanf(buf,"%lf %s %u %u %lf %lf",&timestamp,cmd,&src,&dest,&lat,&bw);
            PostEvent(new Event(timestamp,ADD_LINK,this,new Link(src,dest,this,bw,lat)));
            continue;
        }
        if (!strcasecmp(cmd,"DELETE_LINK")) {
            sscanf(buf,"%lf %s %u %u %lf %lf",&timestamp,cmd,&src,&dest,&lat,&bw);
            PostEvent(new Event(timestamp,DELETE_LINK,this,new Link(src,dest,this,bw,lat)));
            continue;
        }
        if (!strcasecmp(cmd,"CHANGE_LINK")) {
            sscanf(buf,"%lf %s %u %u %lf %lf",&timestamp,cmd,&src,&dest,&lat,&bw);
            PostEvent(new Event(timestamp,CHANGE_LINK,this,new Link(src,dest,this,bw,lat)));
            continue;
        }
        /*
        if (!strcasecmp(cmd,"PRINT")) {
            sscanf(buf,"%lf %s",&timestamp,cmd);
            char *start;
            char *data = new char [1024];
            start=strstr(buf,"PRINT")+5;
            while (*start!=0 && isspace(*start)) { 
                start++;
            }
            if (*start==0) { 
                strncpy(data,"Nothing to print!",1024);
            } else {
                strncpy(data,start,1024);
            }
            PostEvent(new Event(timestamp,PRINT,this,data));
            continue;
        }
        */
    }

    fclose(in);
}

void SimulationContext::Init()
{
    for (deque<Link*>::const_iterator i=links.begin();i!=links.end();++i) { 
        PostEvent(new Event(-9999, CHANGE_LINK,this,new Link(**i)));
    }
}

void SimulationContext::LoadTopology(string file)
{
    Event *e;

    LoadEvents(file);
    while ((e=GetEarliestEvent())) { 
        e->Dispatch();
        e->Disassociate();
        delete e;
    }
}

ostream & SimulationContext::Print(ostream &os)
{
    os << "SimulationContext(topology=";
    Topology::Print(os);
    os <<", eventqueue=";
    EventQueue::Print(os);
    os<<")";
    return os;
}


void SimulationContext::DrawShortestPathTree(Node *node)
{
    WriteShortestPathTreeDot(*node,"_tree.in");
    system("dot _tree.in > _tree.out");
    system("dotty _tree.out");
}

void SimulationContext::DispatchEvent(Event *e)
{
    e->Dispatch();
    e->Disassociate();
    delete e;
}

struct link_compare {
    bool operator()(Link l, Link r) {
        if (l.GetSrc() < r.GetSrc()) {
            return true;
        }
        else return l.GetDest() < r.GetDest();
    }
};

void SimulationContext::WriteShortestPathTreeDot(Node src, string s)
{
    //cerr << "********BEGIN WRITE TREE"<<endl;
    FILE* out;
    map<Link, int, link_compare> treelinks;
    deque<Link> path;
    Link l;

    if ((out = fopen(s.c_str(), "w")) == 0) { 
        return;
    }

    for (deque<Node*>::const_iterator i=nodes.begin();i!=nodes.end();++i) { 
        path.clear();
        CollectPathLinks(src,**i,path);

        for (deque<Link>::const_iterator j=path.begin();j!=path.end();++j) {
            treelinks[*j]=1;
        }
    }

    fprintf(out,"digraph tree {\n");
    for (deque<Node*>::const_iterator i=nodes.begin(); i!=nodes.end();++i) {
        fprintf(out,"%u\n",(*i)->GetNumber());
    }

    for (deque<Link*>::const_iterator i=links.begin(); i!=links.end();++i) {
        fprintf(out,"%u -> %u [ label=\"%5.1lf\" ];\n",(*i)->GetSrc(),(*i)->GetDest(), 
            (*i)->GetLatency());
    }

    for (map<Link,int>::const_iterator i=treelinks.begin();i!=treelinks.end();++i) {
        l = (*i).first;
        fprintf(out,"%u -> %u [ color=red ];\n",l.GetSrc(),l.GetDest());
    }

    fprintf(out,"}\n");
    fclose(out);
    //cerr << "********END WRITE TREE"<<endl;
}

void SimulationContext::WritePathDot(Node src, Node dest, string s)
{
    //cerr << "********BEGIN WRITE PATH"<<endl;
    FILE* out;
    deque<Link> path;

    if ((out = fopen(s.c_str(), "w")) == 0) { 
        return;
    }

    CollectPathLinks(src,dest,path);

    fprintf(out,"digraph path {\n");
    for (deque<Node*>::const_iterator i=nodes.begin(); i!=nodes.end();++i) {
        fprintf(out,"%u\n",(*i)->GetNumber());
    }

    for (deque<Link*>::const_iterator i=links.begin(); i!=links.end();++i) {
        fprintf(out,"%u -> %u [ label=\"%5.1lf\" ];\n",(*i)->GetSrc(), 
            (*i)->GetDest(), (*i)->GetLatency());
    }
    
    for (deque<Link>::const_iterator i=path.begin();i!=path.end();++i) {
        fprintf(out,"%u -> %u [ color=red ];\n",(*i).GetSrc(),(*i).GetDest());
    }
    
    fprintf(out,"}\n");
    fclose(out);
    //cerr << "********END WRITE PATH"<<endl;
}

void SimulationContext::DrawPath(Link *p)
{
    WritePathDot(Node(p->GetSrc(), 0, 0, 0), Node(p->GetDest(), 0, 0 ,0), string("_path.in"));
    system("dot _path.in > _path.out");
    system("dotty _path.out");
}

void SimulationContext::CollectPathLinks(Node src, Node dest, deque<Link> &path)
{
    Node *n, *next_node;
    unsigned last;

    if ((n = ((SimulationContext*)this)->FindMatchingNode(&src)) == 0) { 
        return;
    }

    last = n->GetNumber();

    while (n->GetNumber()!=dest.GetNumber()) {
        if ((next_node = n->GetNextHop(&dest)) == 0) {
            break;
        }

        if ((n = ((SimulationContext *)this)->FindMatchingNode(next_node)) == 0) {
            // delete next_node;
            break;
        }

        // cerr << last <<" -> " << n->GetNumber()<<endl;
        path.push_back(Link(last,n->GetNumber(),0,0,0));
        last=n->GetNumber();
    }
}


void SimulationContext::DumpTable(Node *src)
{
    cout <<*(FindMatchingNode(src))<< endl;
}


void SimulationContext::TimeOut(Node *src, double timefromnow)
{
    PostEvent(new Event(GetTime()+timefromnow,
	        TIMEOUT,
		    FindMatchingNode(src),
		    0));
}


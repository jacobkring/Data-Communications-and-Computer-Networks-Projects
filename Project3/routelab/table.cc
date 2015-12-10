#include "table.h"
#include "messages.h"

Table::Table() {
    topo.clear();
}

Table::Table(const Table & rhs) {
    *this = rhs;
}

Table & Table::operator=(const Table & rhs) {
    /* For now,  Change if you add more data members to the class */
    topo = rhs.topo;

    return *this;
}

#if defined(GENERIC)
ostream & Table::Print(ostream &os) const
{
  os << "Generic Table()";
  return os;
}
#endif

#if defined(LINKSTATE)
ostream & Table::Print(ostream &os) const
{
  os << "\nTable format: " << endl;
  os << "SourceNode: DestNode(ShortestDistanceToNode)" << endl;
  os << "\n================BEGIN ROUTING TABLE=================="<< endl;

  for(map<int, map<int, TopoLink> >::const_iterator i = topo.begin(); i != topo.end(); i++){
    os << i->first << ": ";
      for(map<int, TopoLink>::const_iterator j = i->second.begin(); j != i->second.end(); j++){
        os << j->first << "(" << j->second.cost << "), ";
      }
      os << endl;
  }

  os << "================END ROUTING TABLE====================\n" << endl;
  return os;
}

bool Table::UpdateTable(const RoutingMessage *m){
  //Get message source, destination, latency, and age values
  int src = m->src;
  int dest = m->dest;
  int lat = m->lat;
  int age = m->age;

  //If the age of the TopoLink is less than that of the message, or the cost is -1
  //Set the new age and cost to that in the message, thus updating the table 
  if((GetTopo()[src][dest].age < age) || (GetTopo()[src][dest].cost == -1)){
    SetTopo(src, dest, age, lat);
    return true;
  }
  //Otherwise, we should not update the table
  else{
    return false;
  }
}

int Table::UpdateLink(const Link *l){
  int src = l->GetSrc();
  int dest = l->GetDest();

  int new_age = ++GetTopo()[src][dest].age;
  GetTopo()[src][dest].age = new_age;
  GetTopo()[src][dest].cost = l->GetLatency();

  return new_age;
}

map < int, map < int, TopoLink > > Table::GetTopo(){
  return topo;
}

void Table::SetTopo(int source, int destination, int age, int cost){
  topo[source][destination].age = age;
  topo[source][destination].cost = cost;
}
#endif

#if defined(DISTANCEVECTOR)
ostream & Table::Print(ostream &os) const
{
	// Distance Vector
	/*
	1. Direction in which router or exit interface should be forwarded
	2. Distance from its destination
	*/

  os << "\nTable format: " << endl;
  os << "SourceNode: DestNode(ShortestDistanceToNode)" << endl;
  os << "\n================BEGIN ROUTING TABLE=================="<< endl;
  for(map<int, map<int, TopoLink> >::const_iterator i = topo.begin(); i != topo.end(); i++){
    os << i->first << ": ";
      for(map<int, TopoLink>::const_iterator j = i->second.begin(); j != i->second.end(); j++){
        os << j->first << "(" << j->second.cost << "), ";
      }
      os << endl;
  }

  // // prints out distances
  // os << "Distance Vectors:" << endl;
  // for(map <int, TopoLink>::const_iterator iter = this->distance.begin(); iter != this->distance.end(); iter++){
  // 	os << "Distance To: " << iter->first << " Cost: "<< iter->second.cost << endl;
  // }
  // // prints out neighbors
  // os <<"\nThis node's neighbors:" << endl;
  // for(map <int, TopoLink>::const_iterator iter = this->neighbors.begin(); iter != this->neighbors.end(); iter++){
  // 	os << "Neighbor: " << iter->first << " Cost: " << iter->second.cost <<endl;
  // }
  // // prints out hops
  // os << "\nHops:" << endl;
  // for(map <int, int>::const_iterator iter = this->hops.begin(); iter != this->hops.end(); iter++){
  // 	os << "to get from this node to " << iter->first << " hop to " << iter->second << endl;
  // }
  os << "================END ROUTING TABLE====================\n" << endl;
  return os;
}
#endif

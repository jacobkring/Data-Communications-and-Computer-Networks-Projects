#include "table.h"

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
  os << "LinkState Table()";
  return os;
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

  os << "DistanceVector Table()" << endl;

  // prints out distances
  os << "Distance Vectors:" << endl;
  for(map <int, TopoLink>::const_iterator iter = this->distance.begin(); iter != this->distance.end(); iter++){
  	os << "Distance To: " << iter->first << " Cost: "<< iter->second.cost << endl;
  }
  // prints out neighbors
  os <<"\nThis node's neighbors:" << endl;
  for(map <int, TopoLink>::const_iterator iter = this->neighbors.begin(); iter != this->neighbors.end(); iter++){
  	os << "Neighbor: " << iter->first << " Cost: " << iter->second.cost <<endl;
  }
  // prints out hops
  os << "\nHops:" << endl;
  for(map <int, int>::const_iterator iter = this->hops.begin(); iter != this->hops.end(); iter++){
  	os << "to get from this node to " << iter->first << " hop to " << iter->second << endl;
  }

  os << endl;




  return os;
}
#endif

#ifndef _topology
#define _topology

#include <deque>
#include <string>

#include "link.h"
#include "linkstate.h"

class Topology
{
 protected:
  deque<Node*> nodes;
  deque<Link*> links;

  deque<Node*>::iterator FindMatchingNodeIt(Node*);
  deque<Link*>::iterator FindMatchingLinkIt(Link*);
  
 public:
  Topology();
  virtual ~Topology();

  Node *FindMatchingNode(Node *n);
  deque<Link*> *GetOutgoingLinks(Node *n);
  deque<Node*> *GetNeighbors(Node *n);

  void CollectShortestPathTreeLinks(Node* src, deque<Link> &links);
  void CollectShortestPathLinks(Node* src, Node* dest, deque<Link> &links);

  void AddNode(Node *n);
  void DeleteNode(Node *n);
  void ChangeNode(Node *n);

  Link *FindMatchingLink(Link* l);

  void AddLink(Link *n);
  void DeleteLink(Link *n);
  void ChangeLink(Link *n);

  void WriteDot(string fn);
  void DrawTopology();

  ostream & Print(ostream &os);
} ;


inline ostream & operator<<(ostream &os, Topology t) { return t.Print(os);};


#endif


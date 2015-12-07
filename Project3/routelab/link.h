#ifndef _link
#define _link

#include <new>
#include <iostream>

using namespace std;

class SimulationContext;

class Link {
  unsigned src,dest;
  SimulationContext *context;
  double   bw;
  double   lat;
  

 public:
  Link(unsigned s, unsigned d, SimulationContext *c, double b, double l);
  Link();
  Link(const Link &rhs);
  Link & operator=(const Link &rhs);
  virtual ~Link();

  virtual bool Matches(Link rhs);
  virtual bool Matches(unsigned, unsigned);


  virtual void SetSrc(unsigned s);
  virtual unsigned GetSrc() const;
  virtual void SetDest(unsigned d);
  virtual unsigned GetDest() const;
  virtual void SetLatency(double l);
  virtual double GetLatency() const;
  virtual void SetBW(double b);
  virtual double GetBW() const;

  virtual ostream & Print(ostream &os) const;

};

inline ostream & operator<<(ostream &os, const Link & n) { return n.Print(os);}


#endif

#include "link.h"

Link::Link(unsigned s, unsigned d, SimulationContext *c, double b, double l) :
  src(s), dest(d), context(c), bw(b), lat(l) {}

Link::Link()
{}


Link::Link(const Link &rhs) :
  src(rhs.src), dest(rhs.dest), context(rhs.context), bw(rhs.bw), lat(rhs.lat) {}

Link & Link::operator=(const Link &rhs)
{
  return *(new (this) Link(rhs));
}

Link::~Link()
{}

bool Link::Matches(Link rhs)
{
  return src==rhs.src && dest==rhs.dest;
}

bool Link::Matches(unsigned src, unsigned dest)
{
  return this->src==src && this->dest==dest;
}

void Link::SetSrc(unsigned s) 
{ src=s;}

unsigned Link::GetSrc() const
{ return src;}

void Link::SetDest(unsigned d) 
{ dest=d;}

unsigned Link::GetDest() const 
{ return dest;}

void Link::SetLatency(double l)
{ lat=l;}

double Link::GetLatency() const 
{ return lat; }

void Link::SetBW(double b)
{ bw=b;}

double Link::GetBW() const
{ return bw;}

ostream & Link::Print(ostream &os) const
{
  os << "Link(src="<<src<<", dest="<<dest<<", lat="<<lat<<", bw="<<bw<<")";
  return os;
}


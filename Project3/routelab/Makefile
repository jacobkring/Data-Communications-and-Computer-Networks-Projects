AR = ar
CXX = g++
CXXFLAGS = -g -gstabs+ -ggdb -Wall
LDFLAGS = 

#TYPE = GENERIC
#TYPE = LINKSTATE
TYPE = DISTANCEVECTOR

#LIB_OBJS = node.o       \

BASE_OBJS = node.o     \
			link.o       \
			table.o      \
			messages.o   \
			topology.o   \
			event.o      \
			eventqueue.o \
			context.o 

EXEC_OBJS = routesim.o

ifeq ($(TYPE), LINKSTATE)
	LIB_OBJS = $(BASE_OBJS) linkstate.o
else 
	ifeq ($(TYPE), DISTANCEVECTOR)
		LIB_OBJS = $(BASE_OBJS) distancevector.o
	else
		LIB_OBJS = $(BASE_OBJS)
	endif
endif

OBJS = $(LIB_OBJS) $(EXEC_OBJS)

all: $(EXEC_OBJS:.o=)

%.o : %.cc
	$(CXX) $(CXXFLAGS) -D$(TYPE) -c $< -o $(@F)

libroutelab.a: $(LIB_OBJS)
	$(AR) ruv libroutelab.a $(LIB_OBJS)

routesim: routesim.o libroutelab.a
	$(CXX) $(LDFLAGS) routesim.o $(GENERIC) libroutelab.a -o routesim

depend:
	$(CXX) $(CXXFLAGS) -D$(TYPE) -MM $(OBJS:.o=.cc) > .dependencies

clean:
	rm -f $(OBJS) $(EXEC_OBJS:.o=) libroutelab.a


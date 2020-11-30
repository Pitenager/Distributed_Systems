CXX=mpiCC
CXXFLAGS=-I.
EXEC=mpiDisc
OBJS=arbol.o main.o raid.o slave.o terminal.o


all: $(OBJS)
	$(CXX) $(OBJS) -o $(EXEC)

%.o: %.cpp
	$(CXX) -c $(CXXFLAGS) $(INCPATH) -o "$@" "$<"
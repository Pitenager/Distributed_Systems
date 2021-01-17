CXX=mpiCC
CXXFLAGS=-Wall -pedantic -I.
EXEC=mpiDisc
OBJS=arbol.o main.o raid.o slave.o terminal.o
PROG=mpiDisc

all: $(OBJS)
	$(CXX) $(OBJS) -o $(EXEC)
	@echo "Compilation complete!"


%.o: %.cpp
	$(CXX) -c $(CXXFLAGS) $(INCPATH) -o "$@" "$<"
	@echo "Linking complete!"

clean:
	rm -f *.o $(PROG)
	@echo "Cleanup complete!"


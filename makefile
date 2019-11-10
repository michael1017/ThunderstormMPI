CXX = mpicxx
FLAG = -O3 -std=c++17

clean: 
	rm -f *.o *.out

all :TMPI.cc TMPI.hpp implement.cc
	$(CXX) $(FLAG) TMPI.cc implement.cc -o mpi
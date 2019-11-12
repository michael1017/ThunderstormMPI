CXX = mpicxx
FLAG = -O3 -std=c++17 -pthread

clean: 
	rm -f *.o *.out

all :mpi_d mpi_s
	

mpi_d : TMPI.cc TMPI.hpp dynamic_implement.cc
	$(CXX) $(FLAG) TMPI.cc dynamic_implement.cc -o mpi_d.out

mpi_s : TMPI.cc TMPI.hpp static_implement.cc
	$(CXX) $(FLAG) TMPI.cc static_implement.cc -o mpi_s.out
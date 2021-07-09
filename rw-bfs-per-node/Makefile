include ../Makefile.inc
.PHONY: all
all: 
	g++ -I ${GRAPHIT_DIR}/src/runtime_lib  -O3 -DOPENMP -fopenmp randomwalk.cpp -o randomwalk_openmp -g
	g++ -I ${GRAPHIT_DIR}/src/runtime_lib  -O3 -DCILK -fcilkplus randomwalk.cpp -o randomwalk_cilk -g
clean:
	- rm randomwalk_openmp
	- rm randomwalk_cilk

include ../Makefile.inc
.PHONY: all btest
all: 
	g++ -I ${GRAPHIT_DIR}/src/runtime_lib -O3 -DOPENMP -fopenmp randomwalk.cpp -o randomwalk_openmp -g
	g++ -I ${GRAPHIT_DIR}/src/runtime_lib -O3 -DCILK -fcilkplus randomwalk.cpp -o randomwalk_cilk -g

btest: bucket_queue.h btest.cpp
	g++ -I ${GRAPHIT_DIR}/src/runtime_lib -O3 -DOPENMP -fopenmp btest.cpp -o btest

clean:
	- rm randomwalk_openmp
	- rm randomwalk_cilk
	- rm btest

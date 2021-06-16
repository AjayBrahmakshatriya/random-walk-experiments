include ../Makefile.inc
.PHONY: all
all: 
	g++ -I ${GRAPHIT_DIR}/src/runtime_lib -O3 -DOPENMP -fopenmp randomwalk.cpp -o randomwalk
clean:
	rm randomwalk

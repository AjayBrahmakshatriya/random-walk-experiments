include ../Makefile.inc
.PHONY: all
all: 
	g++ -I ${GRAPHIT_DIR}/src/runtime_lib  -O3 -DOPENMP -DWEIGHTING_HEADER="\"${WEIGHT}\"" -fopenmp randomwalk.cpp -o randomwalk_openmp_$(basename $(notdir ${WEIGHT})) -g
	g++ -I ${GRAPHIT_DIR}/src/runtime_lib  -O3 -DCILK -DWEIGHTING_HEADER="\"${WEIGHT}\"" -fcilkplus randomwalk.cpp -o randomwalk_cilk_$(basename $(notdir ${WEIGHT})) -g
clean:
	- rm randomwalk_openmp_*
	- rm randomwalk_cilk_*

include ../Makefile.inc
.PHONY: all
all: 
	for step in ../step-functions/*.h; do\
		$(MAKE) --no-print-directory randomwalk_cilk_$$(basename $${step%.h});\
		$(MAKE) --no-print-directory randomwalk_openmp_$$(basename $${step%.h});\
	done

randomwalk_cilk_% : ../step-functions/%.h randomwalk.cpp
	g++  -I ${GRAPHIT_DIR}/src/runtime_lib  -O3 -DOPENMP -DWEIGHTING_HEADER="\"$<\"" -fcilkplus randomwalk.cpp -o $@ -g

randomwalk_openmp_% : ../step-functions/%.h randomwalk.cpp
	g++  -I ${GRAPHIT_DIR}/src/runtime_lib  -O3 -DOPENMP -DWEIGHTING_HEADER="\"$<\"" -fopenmp randomwalk.cpp -o $@ -g
.PHONY : clean
clean:
	- rm randomwalk_openmp_*
	- rm randomwalk_cilk_*

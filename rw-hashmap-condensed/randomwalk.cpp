#include "intrinsics.h"
#include <algorithm>
#include "../defs.h"

//int32_t walks[NUM_POINTS][NUM_WALKS][WALK_LENGTH];

int step(Graph &G, int curr, unsigned int* rand_p) {
	int n_count = G.out_degree(curr);
	if (n_count == 0)
		return curr;
	int rand_index = rand_r(rand_p) % n_count;
	return G.get_out_index_()[curr][rand_index];	
}
//typedef int wtype[NUM_WALKS][WALK_LENGTH];

int main(int argc, char* argv[]) {
	//wtype * walks = new wtype[NUM_POINTS];
	if (argc < 2) {
		printf("Usage: %s <graph filename>\n", argv[0]);
		return -1;
	}
	Graph G;
	G = builtin_loadEdgesFromFile(argv[1]);

	int32_t num_vertices = builtin_getVertices(G);

	// WALK_LENGTH * NUM_WALKS is the bound on how many vertices we can visit. 
	// This will always be smaller than num_vertices. 
	int32_t *scores_id = new int[NUM_POINTS * WALK_LENGTH * NUM_WALKS]; 
	int32_t *scores_count = new int[NUM_POINTS * WALK_LENGTH * NUM_WALKS];	
	std::unordered_map<int, int> *scores = new std::unordered_map<int, int>[NUM_POINTS];

	if (scores == nullptr) {
		printf("Out of mem\n");
		return -1;
	}

	int32_t *points = new int[NUM_POINTS];
	
	// We will use first n points just for reproducibility across implementations
	for (int i = 0; i < NUM_POINTS; i++) {
		points[i] = i;
	}	


	startTimer();
	for (int trial = 0; trial < 10; trial++) {	
		// Reset scores 
/*
		parallel_for (long long i = 0; i < NUM_POINTS * WALK_LENGTH * NUM_WALKS; i++) {
			scores_count[i] = 0;
		}	
*/
		// Perform the random walk, no need to reset
		parallel_for (int i = 0; i < NUM_POINTS; i++) {
			int seed = points[i];	
			// No paralleization for walks loop because map is not thread safe
			for (int j = 0; j < NUM_WALKS; j++) {
                                unsigned int rand_p = i + 123132141 + j;
				int curr = seed;
				for (int k = 0; k < WALK_LENGTH; k++) {
					curr = step(G, curr, &rand_p);
					//walks[i][j][k] = curr;
					scores[i][curr]++;	
				}
			}
		}
/*
		parallel_for(int i = 0; i < NUM_POINTS; i++) {
			int counter = 0;
			for (auto iter = scores[i].begin(); iter != scores[i].end(); iter++) {
				scores_id[counter] = iter->first;	
				scores_count[counter] = iter->second;	
			}
		}	
*/
		
	// End of trial
	}

	float elapsed = stopTimer();
	
	std::cout << "Total time elapsed = " << elapsed << std::endl;
	return 0;
	
}

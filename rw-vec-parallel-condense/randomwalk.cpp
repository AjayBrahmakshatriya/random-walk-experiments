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
typedef int wtype[NUM_WALKS][WALK_LENGTH];
int main(int argc, char* argv[]) {
	wtype * walks = new wtype[NUM_POINTS];
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

	if (scores_id == nullptr || scores_count == nullptr) {
		printf("Out of mem\n");
		return -1;
	}
	int32_t *points = new int[NUM_POINTS];
	
	// We will use first n points just for reproducibility across implementations
	for (int i = 0; i < NUM_POINTS; i++) {
		points[i] = i;
	}	


	startTimer();
	for (int trial = 0; trial < 1; trial++) {	

		// Reset scores 
		parallel_for (long long i = 0; i < NUM_POINTS * WALK_LENGTH * NUM_WALKS; i++) {
			scores_count[i] = 0;
		}	

		// Perform the random walk, no need to reset
		parallel_for (int i = 0; i < NUM_POINTS; i++) {
			int seed = points[i];	
			parallel_for (int j = 0; j < NUM_WALKS; j++) {
                                unsigned int rand_p = i + 123132141 + j;
				int curr = seed;
				for (int k = 0; k < WALK_LENGTH; k++) {
					curr = step(G, curr, &rand_p);
					walks[i][j][k] = curr;
				}
			}
		}
	

		// Now we will sort the visited vertices per starting point
		parallel_for (int i = 0; i < NUM_POINTS; i++) {
			std::sort(&walks[i][0][0], &walks[i][0][0] + NUM_WALKS * WALK_LENGTH);
		}

	
		// Create compressed scores for each starting point in parallel
		parallel_for (int i = 0; i < NUM_POINTS; i++) {
			// Each vertex inside is sorted
			int counter = -1;
			int last = -1;
			int* start = &walks[i][0][0];
			for (int j = 0; j < NUM_WALKS * WALK_LENGTH; j++) {
				if (start[j] != last) {
					last = start[j];
					counter++;
					scores_id[i * NUM_WALKS * WALK_LENGTH + counter] = start[j];
				}
				scores_count[i * NUM_WALKS * WALK_LENGTH + counter]++;	
			}	
		}		
	// End of trial
	}

	float elapsed = stopTimer();
	
	std::cout << "Total time elapsed = " << elapsed << std::endl;
	return 0;
	
}

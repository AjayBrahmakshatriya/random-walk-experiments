#include "intrinsics.h"
#include "../defs.h"
//int32_t walks[NUM_POINTS][NUM_WALKS][WALK_LENGTH];

int step(Graph &G, int curr, unsigned int * rand_p) {
	int n_count = G.out_degree(curr);
	if (n_count == 0)
		return curr;
	int rand_index = rand_r(rand_p) % n_count;
	return G.get_out_index_()[curr][rand_index];	
}

int main(int argc, char* argv[]) {
	if (argc < 2) {
		printf("Usage: %s <graph filename>\n", argv[0]);
		return -1;
	}
	Graph G;
	G = builtin_loadEdgesFromFile(argv[1]);

	int32_t num_vertices = builtin_getVertices(G);
	int32_t *scores = new int[NUM_POINTS * num_vertices];
	if (scores == nullptr) {
		printf("Out of mem\n");
		return -1;
	}
	int32_t *points = new int[NUM_POINTS];
	
	// We will use first n points just for reproducibility across implementations
	for (int i = 0; i < NUM_POINTS; i++) {
		points[i] = i;
	}	

	// Reset scores 
	parallel_for (long long i = 0; i < NUM_POINTS * num_vertices; i++) {
		scores[i] = 0;
	}	

	startTimer();
	for (int trial = 0; trial < 10; trial++) {	
		// Perform the random walk, no need to reset
		parallel_for (int i = 0; i < NUM_POINTS; i++) {
			int seed = points[i];			
			parallel_for (int j = 0; j < NUM_WALKS; j++) {
                                unsigned int rand_p = i * 123132141 + j;
				int curr = seed;
				for (int k = 0; k < WALK_LENGTH; k++) {
					curr = step(G, curr, &rand_p);
					//walks[i][j][k] = curr;
					writeAdd(&scores[((long long)i) * num_vertices + curr], 1);	
				}
			}
		}
	
/*	
		// Count score per starting point
		parallel_for (int i = 0; i < NUM_POINTS; i++) {
			parallel_for (int j = 0; j < NUM_WALKS; j++) {
				parallel_for (int k = 0; k < WALK_LENGTH; k++) {
					int curr = walks[i][j][k];
					writeAdd(&scores[((long long)i) * num_vertices + curr], 1);
				}
			}
		}		
*/
	// End of trial
	}

	float elapsed = stopTimer();
	
	std::cout << "Total time elapsed = " << elapsed << std::endl;
	return 0;
	
}

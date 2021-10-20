#include "intrinsics.h"
#include "../defs.h"

//int32_t walks[NUM_POINTS][NUM_WALKS][WALK_LENGTH];

int step(Graph &G, int curr) {
	int n_count = G.out_degree(curr);
	if (n_count == 0)
		return curr;
	int rand_index = rand() % n_count;
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
	for (long long i = 0; i < NUM_POINTS * num_vertices; i++) {
		scores[i] = 0;
	}	

	startTimer();

	for (int trial = 0; trial < 10; trial++) {	
		// Perform the random walk, no need to reset
		for (int i = 0; i < NUM_POINTS; i++) {
			int seed = points[i];
			
			for (int j = 0; j < NUM_WALKS; j++) {
				int curr = seed;
				for (int k = 0; k < WALK_LENGTH; k++) {
					curr = step(G, curr);
					walks[i][j][k] = curr;
				}
			}
		}
	
	
		// Count score per starting point
		for (int i = 0; i < NUM_POINTS; i++) {
			for (int j = 0; j < NUM_WALKS; j++) {
				for (int k = 0; k < WALK_LENGTH; k++) {
					int curr = walks[i][j][k];
					if (curr >= num_vertices) {
						printf("Bad walk\n");
					}		
					scores[((long long)i) * num_vertices + curr]++;
				}
			}
		}		
	// End of trial
	}

	float elapsed = stopTimer();
	
	//std::cout << "Total time elapsed = " << elapsed << std::endl;
	printf("%f\n", elapsed);
	return 0;
	
}

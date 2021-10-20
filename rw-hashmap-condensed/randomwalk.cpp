#include "intrinsics.h"
#include <algorithm>
#include WEIGHTING_HEADER
#include <sys/time.h>
#include <sparsehash/dense_hash_map>



int main(int argc, char* argv[]) {
	//wtype * walks = new wtype[NUM_POINTS];
	if (argc < 5) {
		printf("Usage: %s <graph filename> <NUM_POINTS> <NUM_WALKS> <WALK_LENGTH>\n", argv[0]);
		return -1;
	}
	const int NUM_POINTS = atoi(argv[2]);
	const int NUM_WALKS = atoi(argv[3]);
	const int WALK_LENGTH = atoi(argv[4]);
	Graph_T G;
	G = load_graph(argv[1]);

	int32_t num_vertices = builtin_getVertices(G);

	// WALK_LENGTH * NUM_WALKS is the bound on how many vertices we can visit. 
	// This will always be smaller than num_vertices. 
	int32_t *scores_id = new int[NUM_POINTS * WALK_LENGTH * NUM_WALKS]; 
	int32_t *scores_count = new int[NUM_POINTS * WALK_LENGTH * NUM_WALKS];	
	google::dense_hash_map<int, int> *scores = new google::dense_hash_map<int, int>[NUM_POINTS];
	for (int i = 0; i < NUM_POINTS; i++) {
		scores[i].set_empty_key(-1);
	}
	if (scores == nullptr) {
		printf("Out of mem\n");
		return -1;
	}

	int32_t *points = new int[NUM_POINTS];
	
	// We will use first n points just for reproducibility across implementations
	srand(17);
	for (int i = 0; i < NUM_POINTS; i++) {
		points[i] = rand() % num_vertices;
	}	

	float total_map_time = 0;
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
	printf("Runtime: %f\n", elapsed);
	//std::cout << "Total time elapsed = " << elapsed << std::endl;

	//printf("Total time in map: %f\n", total_map_time);
	return 0;
	
}

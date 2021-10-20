#include "intrinsics.h"
#include <algorithm>
#include WEIGHTING_HEADER
#include <set>

//typedef int wtype[NUM_WALKS][WALK_LENGTH];
int main(int argc, char* argv[]) {
	if (argc < 5) {
		printf("Usage: %s <graph filename> <NUM_POINTS> <NUM_WALKS> <WALK_LENGTH>\n", argv[0]);
		return -1;
	}
	const int NUM_POINTS = atoi(argv[2]);
	const int NUM_WALKS = atoi(argv[3]);
	const int WALK_LENGTH = atoi(argv[4]);
	Graph_T G;
	G = load_graph(argv[1]);
	int * walks = new int[NUM_POINTS * NUM_WALKS * WALK_LENGTH];

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
	srand(17);
	for (int i = 0; i < NUM_POINTS; i++) {
		points[i] = rand() % num_vertices;
	}	

	startTimer();
	for (int trial = 0; trial < 10; trial++) {	
		
		// Reset scores 
		parallel_for (long long i = 0; i < NUM_POINTS * WALK_LENGTH * NUM_WALKS; i++) {
			scores_count[i] = 0;
		}	

		//float elapsed = stopTimer();
		//printf("Resetting scores: %f\n", elapsed);
		//startTimer();
		// Perform the random walk, no need to reset
		parallel_for (int i = 0; i < NUM_POINTS; i++) {
			int seed = points[i];	
			parallel_for (int j = 0; j < NUM_WALKS; j++) {
                                unsigned int rand_p = i * 1231321 + j;
				int curr = seed;
				for (int k = 0; k < WALK_LENGTH; k++) {
					curr = step(G, curr, &rand_p);
					walks[(i* NUM_WALKS + j) * WALK_LENGTH + k] = curr;
				}
			}
		}
		//elapsed = stopTimer();
		//printf("Random walks: %f\n", elapsed);
		//startTimer();
		
		// Now we will sort the visited vertices per starting point
		parallel_for (int i = 0; i < NUM_POINTS; i++) {
			std::sort(&walks[i * NUM_WALKS * WALK_LENGTH], &walks[i * NUM_WALKS * WALK_LENGTH] + NUM_WALKS * WALK_LENGTH);
		}
		//elapsed = stopTimer();
		//printf("Sorting: %f\n", elapsed);
		//startTimer();
		
		// Create compressed scores for each starting point in parallel
		parallel_for (int i = 0; i < NUM_POINTS; i++) {
			// Each vertex inside is sorted
			int counter = -1;
			int last = -1;
			int* start = &walks[i * NUM_WALKS * WALK_LENGTH];
			for (int j = 0; j < NUM_WALKS * WALK_LENGTH; j++) {
				if (start[j] != last) {
					last = start[j];
					counter++;
					scores_id[i * NUM_WALKS * WALK_LENGTH + counter] = start[j];
				}
				scores_count[i * NUM_WALKS * WALK_LENGTH + counter]++;	
			}	
			
		}
		
		//elapsed = stopTimer();
		//printf("Condensing: %f\n", elapsed);
		//startTimer();		
	// End of trial
	}
	float elapsed = stopTimer();
	printf("Runtime: %f\n", elapsed);
	//std::cout << "Total time elapsed = " << elapsed << std::endl;

	return 0;
	//Repeated node info:
	std::set<int> all_visited;
	std::set<int> visited_by_step[WALK_LENGTH];
	std::set<int> visited_by_node[NUM_POINTS];
	for (int i = 0; i < NUM_POINTS; i++) {
		for (int w = 0; w < NUM_WALKS; w++) {
			for (int step = 0; step < WALK_LENGTH; step++) {
				int node = walks[(i* NUM_WALKS + w) * WALK_LENGTH + step];
				all_visited.insert(node);
				visited_by_node[i].insert(node);
				visited_by_step[step].insert(node);
			}
		}
	}
	float total_visits = NUM_WALKS * NUM_POINTS * WALK_LENGTH;
	printf("Overall uniques: %f\n", all_visited.size()/total_visits);

	long step_uniques = 0;
	for (int step = 0; step < WALK_LENGTH; step++) {
		step_uniques += visited_by_step[step].size();
	}
	printf("Within step uniques: %f\n", step_uniques/total_visits);

	long node_uniques = 0;
	for (int i = 0; i < NUM_POINTS; i++) {
		node_uniques += visited_by_node[i].size();
	}
	printf("Within each node uniques: %f\n", node_uniques/total_visits);

	return 0;
	
}

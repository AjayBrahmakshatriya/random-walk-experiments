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
typedef int wtype[WALK_LENGTH][NUM_WALKS];
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

	int64_t sorting_time = 0;
	startTimer();
	for (int trial = 0; trial < 10; trial++) {	
		
		// Reset scores 
		parallel_for (long long i = 0; i < NUM_POINTS * WALK_LENGTH * NUM_WALKS; i++) {
			scores_count[i] = 0;
		}	

		//float elapsed = stopTimer();
		//printf("Resetting scores: %f\n", elapsed);
		//startTimer();
			
		//this does not need to be a factor of the total number of walks.
		const int GRANULARITY = 500;

		parallel_for (long thread_num = 0; thread_num < NUM_POINTS * NUM_WALKS; thread_num += GRANULARITY) {
			unsigned int rand_p = thread_num * 1234567;
			for (long inner = thread_num; inner < NUM_POINTS * NUM_WALKS && inner < thread_num + GRANULARITY; inner++) {
				int i = inner / NUM_WALKS;
				int w = inner % NUM_WALKS;
				int start_node = points[i];
				walks[i][0][w] = step(G, start_node, &rand_p);
			}
		} 
		for (int steps = 1; steps < WALK_LENGTH; steps++) {
			parallel_for (long thread_num = 0; thread_num < NUM_POINTS * NUM_WALKS; thread_num += GRANULARITY) {
				unsigned int rand_p = thread_num * 123132141;
				for (long inner = thread_num; inner < NUM_POINTS * NUM_WALKS && inner < thread_num + GRANULARITY; inner++) {
					int i = inner / NUM_WALKS;
					int w = inner % NUM_WALKS;
					int curr = walks[i][steps-1][w];
					curr = step(G, curr, &rand_p);
					walks[i][steps][w] = curr;
				}
			}
		}
		//elapsed = stopTimer();
		//printf("Sorting: %f\n", elapsed);
		//startTimer();
		
		// Create compressed scores for each starting point in parallel
		parallel_for (int i = 0; i < NUM_POINTS; i++) {
			// Now we will sort the visited vertices per starting point
			std::sort(&walks[i][0][0], &walks[i][0][0] + NUM_WALKS * WALK_LENGTH);
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
		
		//elapsed = stopTimer();
		//printf("Condensing: %f\n", elapsed);
		//startTimer();		
	// End of trial
	}

	float elapsed = stopTimer();
	printf("%f\n", elapsed);

	//printf("CPU time spent sorting each step: %f\n", sorting_time/1.0e6);
	//std::cout << "Total time elapsed = " << elapsed << std::endl;
	return 0;
	
}

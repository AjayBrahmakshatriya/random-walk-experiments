#include "intrinsics.h"
#include <algorithm>
#include WEIGHTING_HEADER


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
		points[i] = rand()%num_vertices;
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
			

		parallel_for (int i = 0; i < NUM_POINTS; i++) {
			unsigned int rand_p = i * 123132141;
			for (int w=0; w<NUM_WALKS; w++) {
				int start_node = points[i];
				walks[(i * WALK_LENGTH + 0) * NUM_WALKS + w] = step(G, start_node, &rand_p);
			}
			for (int steps = 1; steps < WALK_LENGTH; steps++) {
				//sort the previous step to get a memory sweep
				/*
				struct timeval start_time_;
				gettimeofday(&start_time_, NULL);
				*/
				//std::sort(&walks[i][steps-1][0], &walks[i][steps-1][0] + NUM_WALKS);
				//timing code needs to avoid races
				/*
				struct timeval elapsed_time_;
				gettimeofday(&elapsed_time_, NULL);
    			elapsed_time_.tv_sec  -= start_time_.tv_sec;
    			elapsed_time_.tv_usec -= start_time_.tv_usec;
    			fetch_and_add(sorting_time, elapsed_time_.tv_sec*1e6 + elapsed_time_.tv_usec);*/

				for (int w=0; w<NUM_WALKS; w++) {
					int curr = walks[(i * WALK_LENGTH + steps - 1) * NUM_WALKS + w];
					curr = step(G, curr, &rand_p);
					walks[(i * WALK_LENGTH + steps) * NUM_WALKS + w] = curr;
				}
			}

			//elapsed = stopTimer();
			//printf("Random walks: %f\n", elapsed);
			//startTimer();
			// Now we will sort the visited vertices per starting point
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

	//printf("CPU time spent sorting each step: %f\n", sorting_time/1.0e6);
	//std::cout << "Total time elapsed = " << elapsed << std::endl;
	return 0;
	
}

#ifndef _STEP_FUNCTION_HISTOGRAM_H
#define _STEP_FUNCTION_HISTOGRAM_SEARCH_H

#include "intrinsics.h"
#include <algorithm>

typedef int32_t probability_t;

struct HistogramRow {
	int node1;
	int node2;
	probability_t p;
	probability_t weight;
};

int sample_histogram(HistogramRow* histogram, int num_edges, unsigned int* rand_p) {
	int row = rand_r(rand_p) % num_edges;
	if (rand_r(rand_p) % histogram[row].weight < histogram[row].p) {
		return histogram[row].node1;
	} else {
		return histogram[row].node2;
	}
}

void make_histogram(probability_t* weights, HistogramRow* histogram, int num_edges) {
	std::sort(weights, weights + num_edges);
	//make sure the total weight is divisible by num_edges
	probability_t total_weight = 0;
	for (int i = 0; i < num_edges; i++) {
		weights[i] *= num_edges;
		total_weight += weights[i];
	}
	probability_t equal_weight = total_weight / num_edges;

	//this could be a binary search
	int first_full_edge = 0;
	while (weights[first_full_edge] <= equal_weight) {
		first_full_edge++;
	}

	for (int i = 0; i < num_edges; i++) {

		histogram[i].node1 = i;
		histogram[i].node2 = first_full_edge;
		histogram[i].weight = equal_weight;
		histogram[i].p = weights[i];
		//take some probability from an overfull edge to fill in this bar
		weights[first_full_edge] -= equal_weight - weights[i];
		if (weights[first_full_edge] < equal_weight) {
			first_full_edge++;
		}
	}
}

//Test the histogram sampling
int main() {
	int n = 20;
	//i should be sampled approx i * 1000 times
	int samples = n * (n-1) * 1000/2;

	int weights[n];
	for (int i = 0; i < n; i++) {
		weights[i] = i;
	}

	HistogramRow histogram[n];
	make_histogram(weights, histogram, n);

	for (int i = 0; i < n; i++) {
		printf("%d: %d/%d else %d\n", histogram[i].node1, histogram[i].p, histogram[i].weight, histogram[i].node2);
	}

	int counts[n];
	for (int i = 0; i < n; i++) {
		counts[i] = 0;
	}
	unsigned int rand_p = 13;
	for (int i = 0; i < samples; i++) {
		int sample = sample_histogram(histogram, n, &rand_p);
		counts[sample]++;
	}

	for (int i = 0; i < n; i++) {
		printf("%d occurred %d times.\n", i, counts[i]);
	}
	return 0;
}

/*
typedef WGraph Graph_T;

Graph_T load_graph(std::string file_name) {
	
    Graph_T G = builtin_loadWeightedEdgesFromFile(file_name);
	startTimer();
	int vertecies = builtin_getVertices(G);
	parallel_for (int i=0; i<vertecies; i++) {
		WNode* edges = G.get_out_index_()[i];
		int edge_count = G.out_degree(i);
		edges[0].w += 1;
		for (int edge = 1; edge < edge_count; edge++) {
			//Replace weights with a prefix sum
			edges[edge].w += edges[edge-1].w + 1;
		}
		//if (edges[edge_count-1].w <=0) {
		//	//printf("Zero weight\n");
		//}
	}
	float elapsed = stopTimer();
	printf("Prefix sum time: %f\n", elapsed);
    return G;
}

int step(WGraph &G, int curr, unsigned int* rand_p) {
	int n_count = G.out_degree(curr);
	if (n_count == 0)
		return curr;
	
	WNode* edges = G.get_out_index_()[curr];
	int total_weight = edges[n_count-1].w;

	int rand_index = rand_r(rand_p) % total_weight;
	
	int lower_index = 0;
	int higher_index = n_count - 1;
	while (lower_index != higher_index) {
		int midpoint = (lower_index + higher_index) / 2;
		if (edges[midpoint].w < rand_index) {
			lower_index = midpoint + 1;
		} else {
			higher_index = midpoint;
		}
	}
	return edges[lower_index];	
}
*/
#endif


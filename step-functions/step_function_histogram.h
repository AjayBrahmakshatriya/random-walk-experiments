#ifndef _STEP_FUNCTION_HISTOGRAM_H
#define _STEP_FUNCTION_HISTOGRAM_SEARCH_H

#include "intrinsics.h"
#include <algorithm>

typedef int32_t probability_t;

struct HistogramRow {
	int node1;
	int node2;
	// p/weight is the probability of selecting node1
	probability_t p;
	probability_t weight;
};

int sample_bucket(HistogramRow* bucket, unsigned int* rand_p) {
	if (rand_r(rand_p) % bucket->weight < bucket->p) {
		return bucket->node1;
	} else {
		return bucket->node2;
	}
}

int sample_histogram(HistogramRow* histogram, int num_edges, unsigned int* rand_p) {
	int row = rand_r(rand_p) % num_edges;
	return sample_bucket(histogram + row, rand_p);
}

void make_histogram(probability_t* weights, HistogramRow* histogram, int num_edges) {
	//This sort is a problem as I don't sort the edges, so this swaps edge weights.
	//I will fix this as soon as I remove the sorting from this algorithm
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
		histogram[i].p = weights[i];
		histogram[i].weight = equal_weight;
		//take some probability from an overfull edge to fill in this bar
		weights[first_full_edge] -= equal_weight - weights[i];
		if (weights[first_full_edge] < equal_weight) {
			first_full_edge++;
		}
	}
}
/*
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
}*/


typedef WGraph Graph_T;
HistogramRow* buckets;
Graph_T load_graph(std::string file_name) {
	
    Graph_T G = builtin_loadWeightedEdgesFromFile(file_name);
	int vertecies = builtin_getVertices(G);
	int edges = G.num_edges();
	buckets = new HistogramRow[edges]();
    return G;
}

int step(WGraph &G, int curr, unsigned int* rand_p) {
	int n_count = G.out_degree(curr);
	if (n_count == 0)
		return curr;
	WNode* edges = G.get_out_index_()[curr];
	//The vertex array stores pointers into the edges array, so I need to covert to index
	int edges_index = edges - G.get_out_index_()[0];

	int bucket_id = edges_index + rand_r(rand_p) % n_count;


	//weight is always the last field set in a bucket
	//Therefore if this bucket has nonzero weight, some other thread finished constructing it.
	if (buckets[bucket_id].weight==0) {
		//Initialize a set of buckets;
		probability_t weights[n_count];
		for (int i=0; i<n_count; i++) {
			weights[i] = edges[i].w + 1;
		}
		//There may be races in this function, but as long as its deterministic they will be benign.
		make_histogram(weights, buckets + edges_index, n_count);
	}

	int selectedEdge = sample_bucket(buckets + bucket_id, rand_p);

	return edges[selectedEdge].v;	
}

#endif


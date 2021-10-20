#ifndef _STEP_FUNCTION_BINARTY_SEARCH_H
#define _STEP_FUNCTION_BINARTY_SEARCH_H

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

#endif


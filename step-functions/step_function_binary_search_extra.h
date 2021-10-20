#ifndef _STEP_FUNCTION_BINARTY_SEARCH_EXTRA_H
#define _STEP_FUNCTION_BINARTY_SEARCH_EXTRA_H

typedef WGraph Graph_T;
int* prefix_sums;
Graph_T load_graph(std::string file_name) {
    Graph_T G = builtin_loadWeightedEdgesFromFile(file_name);
	int vertecies = builtin_getVertices(G);
	int edges = G.num_edges();
	//initialized all to zero
	prefix_sums = new int[edges]();
	parallel_for (int i=0; i<edges; i++) {
		prefix_sums[i]=0;
	}
    return G;
}

int step(WGraph &G, int curr, unsigned int* rand_p) {
	int n_count = G.out_degree(curr);
	if (n_count == 0)
		return curr;
	WNode* edges = G.get_out_index_()[curr];
	//The vertex array stores pointers into the edges array, so I need to covert to index
	int edges_index = edges - G.get_out_index_()[0];
	if (prefix_sums[edges_index + n_count - 1]==0) {
		
		int sum = 0;
		for (int edge = 0; edge < n_count; edge++) {
			//+1 to remove nodes with zero total outgoing weight
			sum += edges[edge].w + 1;
			prefix_sums[edges_index + edge] = sum;
		}

	}
	int total_weight = prefix_sums[edges_index + n_count - 1];
	int rand_index = rand_r(rand_p) % total_weight;
	
	int lower_index = 0;
	int higher_index = n_count - 1;
	while (lower_index != higher_index) {
		int midpoint = (lower_index + higher_index) / 2;
		if (prefix_sums[edges_index + midpoint] < rand_index) {
			lower_index = midpoint + 1;
		} else {
			higher_index = midpoint;
		}
	}
	return edges[lower_index].v;	
}


#endif


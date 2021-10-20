#ifndef _STEP_FUNCTION_H
#define _STEP_FUNCTION_H


typedef WGraph Graph_T;

Graph_T load_graph(std::string file_name) {
    Graph_T G = builtin_loadWeightedEdgesFromFile(file_name);

    return G;
}

int step(WGraph &G, int curr, unsigned int* rand_p) {
	int n_count = G.out_degree(curr);
	if (n_count == 0)
		return curr;
	int total_weight = 0;
	WNode* edges = G.get_out_index_()[curr];
	for (int i=0; i<n_count; i++) {
		total_weight += edges[i].w + 1;
	}
	int rand_index = rand_r(rand_p) % total_weight;
	for (int i=0; i<n_count; i++) {
		rand_index -= edges[i].w + 1;
		if (rand_index < 0) {
			return edges[i].v;
		}
	}
	//This should only happen if all edge weights are zero
	//
	return curr;	
}

#endif


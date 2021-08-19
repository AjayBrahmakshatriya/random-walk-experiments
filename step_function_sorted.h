#ifndef _STEP_FUNCTION_SORTED_H
#define _STEP_FUNCTION_SORTED_H

typedef WGraph Graph_T;

Graph_T load_graph(std::string file_name) {
    Graph_T G = builtin_loadWeightedEdgesFromFile(file_name);
	int vertecies = builtin_getVertices(G);
	for (int i=0; i<vertecies; i++) {
		WNode* edges = G.get_out_index_()[i];
		int edge_count = G.out_degree(i);

		//sort in increasing order of weight
		
		std::sort( edges, edges+edge_count, [ ]( const WNode& lhs, const WNode& rhs )
			{
			return lhs.w < rhs.w;
			}
		);

		edges[0].w += 1;
		for (int edge = 1; edge < edge_count; edge++) {
			//Replace weights with a prefix sum
			edges[edge].w += edges[edge-1].w + 1;
		}

	}
	//printf("Loaded\n");
    return G;
}

int step(WGraph &G, int curr, unsigned int* rand_p) {
	int n_count = G.out_degree(curr);
	if (n_count == 0)
		return curr;
	
	WNode* edges = G.get_out_index_()[curr];
	int total_weight = edges[n_count-1].w;

	
	int rand_weight = rand_r(rand_p) % total_weight;
	

	for (int i = n_count - 2; i > 0; i--) {
		if (edges[i].w < rand_weight) {
			return edges[i+1];
		}
	}
	return edges[0];
}

#endif


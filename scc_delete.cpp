#include "scc_delete_split_and_condense.cpp"

//create the dag for the node
void create_node(int root_node, scc_tree* parent, int depth, int* vertices, int Nverts){
	tree_node* node = new tree_node;
	int* scc_list
	if(Nverts==1){
		DAG* dag = NULL;
		vertice_nodes[root_node] = node;
	}
	else{
		DAG* dag = split_and_condense();
	}
	node->parent = parent;
	node->dag = dag;
	node->scc_node = root_node;
	node->depth = depth;
	return node;
}	

//vertices is vertices belonging to same scc, Nverts is number of vertices that needs to be processed
void create_scctree(int root_node, int* vertices, int Nverts, Graph& g, scc_tree* parent, int*& scc_maps){
//	int* inverse_map = new int[Nverts];
//	for(int i=0; i<Nverts; i++){
//		inverse_map[i] = vertices[i];
//	}
	int scc_nos;
	tree_node* node = create_node(root_node, parent);
	if(Nverts==1){
		return;
	}
	else{
		Create_SCC_trees(vertices, Nverts, scc_maps, parent);	
	}
	delete [] inverse_map;
	delete [] dag_scc;
}

`void Create_SCC_trees(int vertices, int num_verts, int*& scc_maps, tree_node* parent){
	int root_count = 0;
	int *temp = new int[num_verts];
	int *inverse_scc = new int[num_verts]; // inverse_scc[index]=root_node of scc
	copy(scc_maps, scc_maps+num_verts, temp);
	for(int i=0;i<num_verts;i++){
		if(temp[scc_maps[vertices[i]]]==-1){
			continue;
		}
		else{
			inverse_scc[root_count++]=scc_maps[vertices[i]];
			temp[scc_maps[vertices[i]]]=-1;
		}
	}
	scc_counts = new int[root_count+1];
	ordered_vertices = new int[num_verts]; //grouping scc verts together
	for(int i=0;i<root_count+1;i++){
		scc_counts = 0;
	}
	for(int i=0;i<num_verts;i++){
		scc_count[inverse_scc[scc_maps[vertices[i]]]+1]++;
	}
	//will add adjacent values to create index to know where scc starts
	for(int i=0;i<root_count;i++){
		scc_count[i+1] = scc_count[i+1]+scc_count[i]; //can i=0  skip if needed, because scc_count[0]=0
	}
	copy(scc_count, scc_count+root_count, temp);
	for(int i=0;i<num_verts;i++){
		ordered_vertices[temp[inverse_scc[scc_maps[vertices[i]]]]++] = i;
	}
	
	for(int i=0;i<root_count;i++){
		int start = scc_count[i];
		int nverts = scc_count[i+1]-scc_counr[i];
		create_scctree(inverse_scc[i], &ordered_vertices[start], nverts, g, parent, scc_maps);
	}
	delete [] scc_counts;
	delete [] ordered_vertices;
	delete [] inverse_scc;
	delete [] temp;
}

void Find_Unreachable_down(Graph* g, int* verts, int num, int source, int* unreachable, int &unreachable_count){
	int* temp_degs = new int[g.n];
	for(int i=0; i<g.n; i++){
		temp_degs[i] = in_degree(g, i);
	}
	int* queue = new int[g.n];
	int* next_queue = new int[g.n];
	int* temp;
	int queue_size = 0;
	int nxtq_size = 0;
	for(int i=0; i<num; i++){
		if(verts[i]!=source){
			if(temp_degs[verts[i]]==0){
				queue[queue_size++] = verts[i];
			}
		}
	}
	while(queue_size){
		for(int i=0; i<queue_size; i++){
			vert = queue_size[i];
			unreachable[unreachable_count++] = vert;
			outs = out_vertices(g, vert);
			odeg = out_degree(g, vert);
			for(int j=0; j<odeg; j++){
				int outv = outs[j];
				temp_degs[outv]--;
				if(temp_degs[outv]==0){
					next_queue[nxtq_size++] = outv;
				}
			}
		}
		queue_size = nxtq_size;
		nxtq_size = 0;
		temp = next_queue;
		next_queue = queue;
		queue = temp;
	}
	delete [] queue;
	delete [] next_queue;
}

void Find_Unreachable_up(Graph* g, int* verts, int num, int source, int* unreachable, int &unreachable_count){
	int* temp_degs = new int[g.n];
	for(int i=0; i<g.n; i++){
		temp_degs[i] = out_degree(g, i);
	}
	int* queue = new int[g.n];
	int* next_queue = new int[g.n];
	int* temp;
	int queue_size = 0;
	int nxtq_size = 0;
	for(int i=0; i<num; i++){
		if(verts[i]!=source){
			if(temp_degs[verts[i]]==0){
				queue[queue_size++] = verts[i];
			}
		}
	}
	while(queue_size){
		for(int i=0; i<queue_size; i++){
			vert = queue_size[i];
			unreachable[unreachable_count++] = vert;
			ins = in_vertices(g, vert);
			ideg = in_degree(g, vert);
			for(int j=0; j<odeg; j++){
				int inv = ins[j];
				temp_degs[inv]--;
				if(temp_degs[inv]==0){
					next_queue[nxtq_size++] = inv;
				}
			}
		}
		queue_size = nxtq_size;
		nxtq_size = 0;
		temp = next_queue;
		next_queue = queue;
		queue = temp;
	}
	delete [] queue;
	delete [] next_queue;
}

void Find_unreachable(Graph* g, int* verts, int num, int source){
	int unreachable_count;
	int* unreachable = new int[g.n];
	Find_unreachable_up(g, verts, num, source, unreachable, unreachable_count);
	Find_unreachable_down(g, verts, num, source, unreachable, unreachable_count);
}



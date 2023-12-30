#include "scc_delete_split_and_condense.cpp"

//create the dag for the node
void create_node(int root_node, scc_tree* parent, int depth, int* vertices, int Nverts, int*& scc_map, Graph& g, tree_node** vertice_nodes){
	tree_node* node = new tree_node;
	int* scc_list
	if(Nverts==1){
		DAG* dag = NULL;
		vertice_nodes[root_node] = node;
	}
	else{
		DAG dag* = split_and_condense(root_node, Nverts, vertices, scc_map, g);
	}
	node->parent = parent;
	node->dag = dag;
	node->scc_node = root_node;
	node->depth = depth;
	return node;
}	

//vertices is vertices belonging to same scc, Nverts is number of vertices that needs to be processed
void create_scctree(int root_node, int* vertices, int Nverts, Graph& g, scc_tree* parent, int*& scc_maps, int depth, tree_node** vertice_nodes){
//	int* inverse_map = new int[Nverts];
//	for(int i=0; i<Nverts; i++){
//		inverse_map[i] = vertices[i];
//	}
	int scc_nos;
	tree_node* node = create_node(root_node, parent, depth, vertices, Nverts, scc_maps, g, vertice_nodes);
	if(Nverts==1){
		return;
	}
	else{
		Create_SCC_trees(vertices, Nverts, scc_maps, node, depth);	
	}
	delete [] inverse_map;
	delete [] dag_scc;
}

`void Create_SCC_trees(int vertices, int num_verts, int*& scc_maps, tree_node* parent, int depth, tree_node** vertice_nodes){
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
		create_scctree(inverse_scc[i], &ordered_vertices[start], nverts, g, parent, scc_maps, depth+1, vertice_nodes);
	}
	delete [] scc_counts;
	delete [] ordered_vertices;
	delete [] inverse_scc;
	delete [] temp;
}

void Find_Unreachable_down(){
	int* temp_degs = new int[dag->vert_no];
	for(int i=0; i<dag->vert_no; i++){
		temp_degs[i] = dag->in_deg_list[i+1]-dag->in_deg_list[i];
	}
	int* queue = new int[dag->vert_no];
	int* next_queue = new int[dag->vert_no];
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

void Find_Unreachable_up(){
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

void Find_unreachable(tree_node* node, int* verts, int Nverts){
	int unreachable_count;
	int* unreachable = new int[node->dag->vert_no];
	Find_unreachable_up(g, verts, num, source, unreachable, unreachable_count);
	Find_unreachable_down(g, verts, num, source, unreachable, unreachable_count);
}

tree_node* node_finder(tree_node*& node1, tree_node*& node2){
	if(node1->depth>node2->depth){
		int d_diff = node1->depth-node2->depth;
		for(int i=0; i<d_diff; i++){
			node1 = node1->parent;
		}
	}		
	else{
		int d_diff = node2->depth-node1->depth;
		for(int i=0; i<d_diff; i++){
			node2 = node2->parent;
		}
	}	

	if(node1->parent == node2->parent){
		return node1->parent;
	}
	else{
		node1 = node1->parent;
		node2 = node2->parent;
		return node_finder(node1, node2);
	}
}

void find_index(int* indexes, int* to_be_found, int* vertices){ //TO:DO check if better methods are available
	
}

void del_edge(int src, int dst, tree_node** vertice_nodes){
	if(scc[src]==scc[dst]){
		tree_node* node1 = vertice_nodes[src];
		tree_node* node2 = vertice_nodes[dst];
		tree_node* Pnode = node_finder(node1, node2);
	}

	else{
		return;
	}

	DAG* dag = Pnode->dag;
	int* vertices = dag->vertices;
	int indexes[2], temp[2];
	temp[0] = node1->scc_node; temp[1] = node2->scc_node;
	sort(temp); //TO:DO define sort
	find_index(indexes, temp, vertices); // use function temporarily 
	for(int i=dag->out_deg_list[src]; i<dag->out_deg_list[src+1]; i++){
		if(dag->outs[i]==dst){
			dag->outs[i]=-1;
			break;
		}
	}
	for(int i=dag->in_deg_list[dst]; i<dag->in_deg_list[dst+1]; i++){
		if(dag->ins[i]==src){
			dag->ins[i]=-1;
			break;
		}
	}

	//currently doing one by one, should discuss and look how to overall deletion
	int* src_and_sinks = new int[2];
	src_and_sinks[0] = src;
	src_and_sinks[1] = dst;
	Find_unreachable(Pnode, src_and_sinks, 2){
	}
}

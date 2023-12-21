#include "scc_delete_split_and_condense.cpp"

void create_scctree(int root_node, int* vertices, int Nverts, Graph& g)
	DAG* dag = new DAG;
       	scc_tree* tree = new scc_tree;	
	SpliAndCondense(dag, g, );
}

void Create_SCC_trees(int num_verts, int* scc_maps){
	int root_count = 0;
	int *temp = new int[num_verts];
	int *inverse_scc = new int[num_verts];
	copy(scc_maps, scc_maps+num_verts, temp);
	for(int i=0;i<num_verts;i++){
		if(temp[scc_maps[i]]==-1){
			continue;
		}
		else{
			inverse_scc[scc_maps[i]]=root_count++;
			temp[scc_maps[i]]=-1;
		}
	}
	scc_counts = new int[root_count+1];
	ordered_vertices = new int[num_verts]; //grouping scc verts together
	for(int i=0;i<root_count+1;i++){
		scc_counts = 0;
	}
	for(int i=0;i<num_verts;i++){
		scc_count[inverse_scc[scc_maps[i]]+1]++;
	}
	//will add adjacent values to create index to know where scc starts
	for(int i=0;i<root_count;i++){
		scc_count[i+1] = scc_count[i+1]+scc_count[i]; //can i=0  skip if needed, because scc_count[0]=0
	}
	copy(scc_count, scc_count+root_count, temp);
	for(int i=0;i<num_verts;i++){
		ordered_vertices[temp[inverse_scc[scc_maps[i]]]++] = i;
	}
	
	for(int i=0;i<root_count;i++){
		create_scctree();
	}

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



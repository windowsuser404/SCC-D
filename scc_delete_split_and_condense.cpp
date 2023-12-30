#include "aux.cpp"
#include "delscc.cpp"

struct DAG;
struct scc_tree;

struct DAG{
	int* outs;
	int* ins;
	int* in_deg_list;
	int* out_deg_list;
	int* vertices;
	int edge_no;
	int vert_no
};

struct tree_node{
        int scc_node;
	int depth;
        tree_node* parent;
        DAG* dag;
};

typedef struct DAG DAG;
typedef struct scc_tree scc_tree;

DAG* split_and_condense(int root_node, int Nverts, int* vertices, int*& scc_map, graph& g){// org_scc_map is the true one and will not change with new condensation
	bool* valid = new bool[Nverts];
	int scc_no=0;
	int dag_edges=0;
	int* valid_verts = new int[Nverts];
	for(int i=0; i<Nverts; i++){
	  valid[i] = true;
	  valid_verts[i] = vertices[i];
	}
	scc_no = delscc_color(g, valid, valid_verts, Nverts, scc_map, root_node);
	delete [] valid;
	delete [] valid_verts;

	DAG* dag = new DAG;
	DAG->vert_no = scc_no;
	int dag_verts = new int[scc_no];
	int temp_inv_map = new int[g.n];
	int temp_in = new int[g.n]; // should test and see if hash maps can be implemented.
	for(int i=0; i<g.n; i++){
		temp_in[i] = 0;
	}
	for(int i=0; i<Nverts; i++){
		temp_in[vertices[i]] = 1;
	}
	scc_no=0;
	for(int i=0; i<Nverts; i++){
		if(scc_map[vertices[i]]==vertices[i]){
			temp_inv_map[vertices[i]] = scc_no;
			dag_verts[scc_no++] = vertices[i];
		}
	}
	//sort(dag_verts, 0, scc_no-1);
	DAG->vertices = dag_verts;
	int* out_edge_counts = new int[scc_no+1];
	int* in_edge_counts = new int[scc_no+1];
	int* temp_counts = new int[scc_no+1];
	for(int i=0; i<scc_no+1: i++){
		out_edge_counts[i] = 0;
		in_edge_count[i] = 0;
	}
	int edges = 0;
	for(int i=0; i<Nverts; i++){
		int vert = vertices[i];
		outs = out_vertices(g, vert);
		out_deg = out_degree(g, vert);
		for(int j=0; j<out_deg; j++){
			if(temp_in[outs[j]]  && scc_map[vert]!=scc_map[outs[j]]){
				edges++;
				out_edge_counts[temp_inv_map[scc_map[vert]]+1]++;
				in_edge_counts[temp_inv_map[scc_map[outs[j]]]+1]++;
			}
		}
	}

	int* DAG_out = new int[edges];
	temp_counts[0] = 0;
	for(int i=0; i<scc_no; i++){
		out_edge_counts[i+1] = out_edge_counts[i+1] + out_edge_counts[i];
		temp[i+1] = out_edge_counts[i+1];
	}
	for(int i=0; i<Nverts; i++){
		int vert = vertices[i];
		outs = out_vertices(g, vert);
		out_deg = out_vertices(g, vert);
		for(int j=0; j<out_deg; j++){
			out_vert = outs[j];
			if(temp_in[out_vert] && scc_map[vert]!=scc_map[out_vert]){
				DAG_out[temp_counts[temp_inv_map[scc_map[vert]]]++] = scc_map[out_vert];
			}
		}
	}

	int* DAG_in = new int[edges];
	temp_counts[0]=0;
	for(int i=0; i<scc_no; i++){
		in_edge_counts[i+1] = in_edge_counts[i+1] + in_edge_counts[i];
		temp[i+1] = in_edge_counts[i+1];
	}
	for(int i=0; i<Nverts; i++){
		int vert = vertices[i];
		outs = out_vertices(g, vert);	
		out_deg = out_vertices(g, vert);
		for(int j=0; j<out_deg; j++){
			out_vert = outs[j];
			if(temp_in[out_vert] && scc_map[vert]!=scc_map[out_vert]){
				DAG_in[temp_counts[temp_inv_map[scc_map[out_vert]]]++] = scc_map[vert];
			}
		}
	}

	dag->edges = edges;
	dag->outs = DAG_out;
	dag->ins = DAG_ins;
	dag->in_deg_list = in_edge_counts;
	dag->out_deg_list = out_edges_counts;

	delete [] temp_counts;
	delete [] temp_in;
	delete [] temp_inv_map;

	return dag;
}

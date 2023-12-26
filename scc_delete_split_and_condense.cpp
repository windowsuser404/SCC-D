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

int* delscc_color_propagate(graph& g, bool* valid,
    int* valid_verts, int num_valid, int rootnode)
{
  int num_verts = g.n;
  bool* in_queue = new bool[num_verts];
  bool* in_queue_next = new bool[num_verts];
  int* queue = new int[num_verts];
  int* queue_next = new int[num_verts];
  copy(valid_verts, valid_verts + num_valid, queue);

  int* colors = new int[num_verts];
#pragma omp parallel for schedule(static)
  for (int i = 0; i < num_valid; ++i)
  { 
    int vert = valid_verts[i];
    colors[vert] = vert;
    in_queue[vert] = true;
    in_queue_next[vert] = false;
  }

  int next_size = 0; 
  int queue_size = num_valid; 
#pragma omp parallel
{
  int thread_queue[ THREAD_QUEUE_SIZE ];
  int thread_queue_size = 0;
  int thread_start;

  while (queue_size)
  {
#pragma omp for schedule(guided) nowait
    for (int i = 0; i < queue_size; ++i)
    {
      int vert = queue[i];
      if(vert==rootnode){//skip processing rootnode
	      continue;
      }
      in_queue[vert] = false;
      int color = colors[vert];
      bool changed = false;

      int out_degree = out_degree(g, vert);
      int* outs = out_vertices(g, vert);
      for (int j = 0; j < out_degree; ++j)
      {
        int out = outs[j];
        int out_color = colors[out];
	if(scc_maps[out]==scc_maps[in] && out!=rootnode){
          if (valid[out] && color > out_color){
            colors[out] = color;
            changed = true;

            if (!in_queue_next[out])
            {
              in_queue_next[out] = true;
              thread_queue[thread_queue_size++] = out;

              if (thread_queue_size == THREAD_QUEUE_SIZE)
              {
#pragma   omp atomic capture
                thread_start = next_size += thread_queue_size;
                
                thread_start -= thread_queue_size;
                for (int l = 0; l < thread_queue_size; ++l)
                  queue_next[thread_start+l] = thread_queue[l];
                thread_queue_size = 0;
              }
            }
          }
	}
      } 

      if (changed && !in_queue_next[vert])
      {
        in_queue_next[vert] = true;
        thread_queue[thread_queue_size++] = vert;

        if (thread_queue_size == THREAD_QUEUE_SIZE)
        {
#pragma omp atomic capture
          thread_start = next_size += thread_queue_size;
          
          thread_start -= thread_queue_size;
          for (int l = 0; l < thread_queue_size; ++l)
            queue_next[thread_start+l] = thread_queue[l];
          thread_queue_size = 0;
        }
      }
    }

#pragma omp atomic capture
    thread_start = next_size += thread_queue_size;
    
    thread_start -= thread_queue_size;
    for (int l = 0; l < thread_queue_size; ++l)
      queue_next[thread_start+l] = thread_queue[l];
    thread_queue_size = 0;
#pragma omp barrier

#pragma omp single
{
    queue_size = next_size;
    next_size = 0;
    int* temp = queue;
    queue = queue_next;
    queue_next = temp;
    bool* temp2 = in_queue;
    in_queue = in_queue_next;
    in_queue_next = temp2;
}
  } // end while
} // end parallel

  delete [] in_queue;
  delete [] in_queue_next;
  delete [] queue;
  delete [] queue_next;

  return colors;
}

int* delscc_color_get_roots(graph& g, bool* valid, //root node should remain in same color, so should be a root
    int* colors, int& num_roots,
    int* valid_verts, int num_valid,
    int* scc_maps, int rootnode)
{
  int* roots = new int[num_valid];
  num_roots = 0;

#pragma omp parallel
{
  int thread_queue[ THREAD_QUEUE_SIZE ];
  int thread_queue_size = 0;
  int thread_start;

#pragma omp for schedule(guided) nowait
  for (int i = 0; i < num_valid; ++i)
  {
    int vert = valid_verts[i];

    if (colors[vert] == vert)
    {
      valid[vert] = false;
      scc_maps[vert] = vert;
      thread_queue[thread_queue_size++] = vert;

      if (thread_queue_size == THREAD_QUEUE_SIZE)
      {
#pragma omp atomic capture
        thread_start = num_roots += thread_queue_size;
        
        thread_start -= thread_queue_size;
        for (int l = 0; l < thread_queue_size; ++l)
          roots[thread_start+l] = thread_queue[l];
        thread_queue_size = 0;
      }
    }
  }

#pragma omp atomic capture
  thread_start = num_roots += thread_queue_size;
  
  thread_start -= thread_queue_size;
  for (int l = 0; l < thread_queue_size; ++l)
    roots[thread_start+l] = thread_queue[l];
}

  return roots;  
}

void delscc_color_find_sccs(graph& g, bool* valid, 
  int* colors, int* roots, int num_roots,
  int* valid_verts, int num_valid,
  int* scc_maps, int rootnode)
{
  int num_verts = g.n;
  int* queue = new int[num_valid];
  int* queue_next = new int[num_valid];
  copy(roots, roots + num_roots, queue);
  int queue_size = num_roots; 
  int next_size = 0;

#pragma omp parallel 
{
  int thread_queue[ THREAD_QUEUE_SIZE ];
  int thread_queue_size = 0;
  int thread_start;

  while (queue_size)
  {

#pragma omp for schedule(guided)
  for (int i = 0; i < queue_size; ++i)
  {
    int vert = queue[i];

    if(vert==rootnode){
      continue;
    }

    int color_vert = colors[vert];

    int in_degree = in_degree(g, vert);
    int* ins = in_vertices(g, vert);
    for (int j = 0; j < in_degree; ++j) //not excluding rootnode here since it should be a different color and on its own scc automatically
    {
      int in = ins[j];
      int color_in = colors[in];

      if (valid[in] && color_in == color_vert)
      {
        valid[in] = false;
        scc_maps[in] = color_in;
        thread_queue[thread_queue_size++] = in;

        if (thread_queue_size == THREAD_QUEUE_SIZE)
        {
#pragma omp atomic capture
          thread_start = next_size += thread_queue_size;
          
          thread_start -= thread_queue_size;
          for (int l = 0; l < thread_queue_size; ++l)
            queue_next[thread_start+l] = thread_queue[l];
          thread_queue_size = 0;
        }
      }
    }
  }

#pragma omp atomic capture
  thread_start = next_size += thread_queue_size;
  
  thread_start -= thread_queue_size;
  for (int l = 0; l < thread_queue_size; ++l)
    queue_next[thread_start+l] = thread_queue[l];
  thread_queue_size = 0;
#pragma omp barrier

#pragma omp single
{
  queue_size = next_size;
  next_size = 0;
  int* temp = queue;
  queue = queue_next;
  queue_next = temp;
}
  }
}
  delete [] queue;
  delete [] queue_next;
}


void update_valid(graph& g, bool* valid, 
  int*& valid_verts, int& num_valid)
{  
  int new_num_valid = 0;
  int* new_valid_verts = new int[num_valid];

#pragma omp parallel 
{ 
  int thread_queue[ THREAD_QUEUE_SIZE ];
  int thread_queue_size = 0;
  int thread_start;

#pragma omp for nowait
  for (int i = 0; i < num_valid; ++i)
  {
    int vert = valid_verts[i];
    if (valid[vert])
    {
      thread_queue[thread_queue_size++] = vert;

      if (thread_queue_size == THREAD_QUEUE_SIZE)
      {
#pragma omp atomic capture
        thread_start = new_num_valid += thread_queue_size;
        
        thread_start -= thread_queue_size;
        for (int l = 0; l < thread_queue_size; ++l)
          new_valid_verts[thread_start+l] = thread_queue[l];
        thread_queue_size = 0;
      }
    }
  }

#pragma omp atomic capture
  thread_start = new_num_valid += thread_queue_size;
  
  thread_start -= thread_queue_size;
  for (int l = 0; l < thread_queue_size; ++l)
    new_valid_verts[thread_start+l] = thread_queue[l];
}

  num_valid = new_num_valid;

  delete [] valid_verts;
  valid_verts = new_valid_verts;
}


int delscc_color(graph& g, bool* valid,
    int*& valid_verts, int& num_valid, 
    int* scc_maps, int rootnode)
{
  int num_scc = 0;
  int num_roots = 0;

  while (num_valid > 0)
  {
    int* colors = delscc_color_propagate(g, valid,
      valid_verts, num_valid);

    int* roots = delscc_color_get_roots(g, valid, 
      colors, num_roots,
      valid_verts, num_valid,
      scc_maps);

    delscc_color_find_sccs(g, valid, 
      colors, roots, num_roots,
      valid_verts, num_valid,
      scc_maps);

    update_valid(g, valid,
      valid_verts, num_valid);

    num_scc += num_roots;

    delete [] colors;
    delete [] roots;
  }

  return num_scc;
}

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
}

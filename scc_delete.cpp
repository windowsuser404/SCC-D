#include "scc_delete_new_scc.cpp"

void fw_propagate(int rep_node, graph &g, int *reachable, int *fw_reach,
                  del_set &deleted_edges, int *&queue, int *&next_queue) {
#if FULL_DEBUG
  printf("Starting with the forwards marking from %d\n", rep_node);
#endif
  int i, j, t_index;
  int vert, Odeg, *Overts;
  int q_size = 0, nxt_q_size = 0;
  queue[q_size++] = rep_node;
  fw_reach[rep_node] = rep_node;

  // #pragma omp parallel private(j, t_index) shared(vert, Odeg, Overts)
  {
    while (q_size) {
      for (i = 0; i < q_size; i++) {
        vert = queue[i];
        Odeg = out_degree(g, vert);
        Overts = out_vertices(g, vert);
        // #pragma omp barrier
        // #pragma omp for
        for (j = 0; j < Odeg; j++) {
          int nVert = Overts[j];
          // checking for -1 as only rep_node is propagating hence other nodes
          // cant come here
          if ((rep_node != fw_reach[nVert]) and
              (g.scc_map[nVert] == g.scc_map[rep_node]) and
              not_deleted(vert, nVert, deleted_edges)) {
            // #pragma omp atomic capture
            t_index = nxt_q_size++;
            next_queue[t_index] = nVert;
#if DEBUG
            printf("Marking %d fw_reachable from %d\n", vert, rep_node);
#endif
            fw_reach[nVert] = rep_node;
          }
        }
      }
      // #pragma omp single
      {
        q_size = nxt_q_size;
        swap(queue, next_queue);
        nxt_q_size = 0;
      }
    }
  }

#if FULL_DEBUG
  printf("Successfull marked all nodes reachable from %d\n", rep_node);
#endif
}

void bw_propagate(int rep_node, graph &g, int *reachable, int *fw_arr,
                  int &unaffected, del_set &deleted_edges, int *&queue,
                  int *&next_queue) {
  int *scc_counts = g.count_in_sccs;
  int *scc_map = g.scc_map;
  int i, j, t_index;
  int vert, Ideg, *Iverts;
  int q_size = 0, nxt_q_size = 0;
  reachable[rep_node] = 1;
  queue[0] = rep_node;
  q_size++;

  // #pragma omp parallel private(j, t_index) shared(vert, Ideg, Iverts)
  {
    while (q_size) {
      for (i = 0; i < q_size; i++) {
        // #pragma omp single
        { unaffected++; }
        vert = queue[i];
        Ideg = in_degree(g, vert);
        Iverts = in_vertices(g, vert);
        // #pragma omp barrier
        // #pragma omp for
        for (j = 0; j < Ideg; j++) {
          int nVert = Iverts[j];
          if (scc_map[rep_node] == scc_map[nVert] and
              rep_node == fw_arr[nVert] and
              not_deleted(nVert, vert, deleted_edges) and !reachable[nVert]) {
            // #pragma omp atomic capture
            t_index = nxt_q_size++;
            next_queue[t_index] = nVert;
            reachable[nVert] = 1;
#if FULL_DEBUG
            printf("Marking %d as reachable as it still belongs to %d scc\n",
                   nVert, rep_node);
#endif
          }
        }
      }
      swap(queue, next_queue);
      q_size = nxt_q_size;
      nxt_q_size = 0;
    }
  }
}

void search(int rep_node, graph &g, int *reachable, int *fw_arr,
            int &unaffected, del_set &deleted_edges, int *&queue,
            int *&next_queue) {
#if FULL_DEBUG
  printf("Starting to search %d \n", rep_node);
#endif
  fw_propagate(rep_node, g, reachable, fw_arr, deleted_edges, queue,
               next_queue);
  bw_propagate(rep_node, g, reachable, fw_arr, unaffected, deleted_edges, queue,
               next_queue);
#if DEBUG
  printf("finished search \n");
#endif
}

void naive_delete(del_set &deleted_edges, graph &g, int *&out_q, int *&in_q,
                  int q_size) { // for now not using * _q arrays
  //	int* out_processed = new int[g.n];
  //	int* in_processed = new int[g.n];
  printf("Start naive\n");
  int *reachable = new int[g.n]; // to check if they are part of an scc (could
                                 // be formed later or retained in an scc)
  int *fw_reach = new int[g.n]; // useful to take into account all vertices that
                                // can be reached from rep_node

#if DEBUG
  printf("reachable = %p\n", reachable);
  printf("fw_reach = %p\n", fw_reach);
#endif
  int unaffected = 0; // takes count of unaffected nodes
  for (int i = 0; i < g.n; i++) {
    fw_reach[i] = -1;
  }
  for (int i = 0; i < g.n; i++) {
    reachable[i] = 0;
  }

  // defining for maximum capacity
  int *queue = new int[g.n];
  int *next_queue = new int[g.n];
  //
  for (int i = 0; i < g.scc_count; i++) {
    int vert = g.rep_nodes[i];
    if (!reachable[vert]) {
      search(vert, g, reachable, fw_reach, unaffected, deleted_edges, queue,
             next_queue);
    }
  }
  // deleted the pre allocated space
  delete[] queue;
  delete[] next_queue;
  //

  int *unreachable = new int[g.n - unaffected]; // array to keep list of verts
                                                // that needs to be changed
  int affected = 0;
  for (int i = 0; i < g.n; i++) {
    if (!reachable[i]) {
      unreachable[affected++] = i;
    }
  }

#if DEBUG
  for (int i = 0; i < affected; i++) {
    printf("%d is unreachable by original scc node %d\n", unreachable[i],
           g.scc_map[unreachable[i]]);
  }
#endif

  create_new_sccs(unreachable, reachable, affected, g, fw_reach,
                  deleted_edges); // unreachable is array filled with changed
                                  // verts (bad var names)
                                  //
#if DEBUG
  printf("reachable = %p\n", reachable);
  printf("fw_reach = %p\n", fw_reach);
#endif
  delete[] fw_reach;
  delete[] unreachable;
  delete[] reachable;

#if DEBUG
  for (int i = 0; i < g.n; i++) {
    printf("scc of %d is %d\n", i, g.scc_map[i]);
  }
#endif

#if DEBUG
  printf("Finished naive\n");
#endif
}

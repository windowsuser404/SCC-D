#include "scc_csr.cpp"
#include "scc_diff_color.cpp"

// typedef unordered_set<pair<int, int>, pair_hash> del_set;

void insert_condense(graph &g, int *&scc_maps, int num_verts, int edges,
                     int *&scc_outarr, int *&scc_inarr,
                     unsigned int *&scc_outdegree_list,
                     unsigned int *&scc_indegree_list, int &roots, int &C_edges,
                     int *&scc_index, int *&inverse_scc,
                     del_set &deleted_edges) {

  /// making the csr for condensed graph
  scc_index = new int[num_verts];   // scc_index[index] = representative scc
  inverse_scc = new int[num_verts]; // inverse_scc[scc_root_node] = vertex in
                                    // condensed graphs
  int root_count = 0, i;
  int condensed_edges = 0;
  int *temp = new int[num_verts];
  copy(scc_maps, scc_maps + num_verts, temp);
  for (i = 0; i < num_verts; i++) {
    if (temp[scc_maps[i]] == -1) {
      continue;
    } else {
      scc_index[root_count] = scc_maps[i];
      inverse_scc[scc_maps[i]] = root_count++;
      temp[scc_maps[i]] = -1;
    }
  }
  delete[] temp;
  int *tscc_src = new int[edges];
  int *tscc_dst = new int[edges];
  for (i = 0; i < num_verts; i++) {
    int *out_verts = out_vertices(g, i);
    int out_deg = out_degree(g, i);
    for (int j = 0; j < out_deg; j++) {
      std::pair temp = make_pair(i, out_verts[j]);
      if ((deleted_edges.find(temp) == deleted_edges.end())) {
        if (scc_maps[i] != scc_maps[out_verts[j]]) {
#if DEBUG
          printf("Adding edges %d->%d, i.e in condensed %d->%d\n", i,
                 out_verts[j], inverse_scc[scc_maps[i]],
                 inverse_scc[scc_maps[out_verts[j]]]);
#endif
          tscc_src[condensed_edges] = inverse_scc[scc_maps[i]];
          tscc_dst[condensed_edges] = inverse_scc[scc_maps[out_verts[j]]];
          condensed_edges++;
        }
      }
    }
  }
  int *scc_src = new int[condensed_edges];
  int *scc_dst = new int[condensed_edges];
  copy(tscc_src, tscc_src + condensed_edges, scc_src);
  copy(tscc_dst, tscc_dst + condensed_edges, scc_dst);
  delete[] tscc_src;
  delete[] tscc_dst;

  new_create_csr(root_count, condensed_edges, scc_src, scc_dst, scc_outarr,
                 scc_inarr, scc_outdegree_list, scc_indegree_list);
#if DEBUG
  printf("\n\ninarray at %p\n", &scc_inarr);
  for (int i = 0; i < condensed_edges; i++) {
    printf("%d\n", scc_inarr[i]);
  }
  printf("\n\n");
  printf("\n\noutarray at %p\n", &scc_outarr);
  for (int i = 0; i < condensed_edges; i++) {
    printf("%d\n", scc_outarr[i]);
  }
  printf("\n\n");
#endif
  /////////////////////////
  roots = root_count;
  C_edges = condensed_edges;
  delete[] scc_src;
  delete[] scc_dst;
}

void condense_scc_update(graph &g, int *scc_maps, int *scc_index,
                         int *inverse_scc, int *scc_diff_out, int *scc_diff_in,
                         unsigned int *scc_diff_outdeg_list,
                         unsigned int *scc_diff_indeg_list) {
  int *valid = new int[g.n];
  int *valid_verts = new int[g.n];
  for (int i = 0; i < g.n; i++) {
    valid[i] = 1;
    valid_verts[i] = i;
  }
  con_scc_color(g, valid, valid_verts, scc_maps, scc_index, inverse_scc,
                scc_diff_out, scc_diff_in, scc_diff_outdeg_list,
                scc_diff_indeg_list);
  delete[] valid;
  delete[] valid_verts;
}

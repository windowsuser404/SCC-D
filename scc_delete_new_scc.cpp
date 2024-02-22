// will test out different ways to create the new scc for the vert not part of
// original scc

#include "aux.cpp"
#include <string>
#include <unordered_map>
#include <unordered_set>
#include <utility>
#include <vector>

struct pair_hash {
  inline std::size_t operator()(const std::pair<int, int> &v) const {
    return v.first * 31 + v.second;
  }
};

typedef unordered_set<pair<int, int>, pair_hash> del_set;

bool not_deleted(int &src, int &dst, del_set &deleted_edges) {
  std::pair<int, int> temp = make_pair(src, dst);
  if (deleted_edges.find(temp) != deleted_edges.end()) {
    return false;
  } else {
    return true;
  }
}

void fw_new(int &node, graph &g, int *&fw_reach, int *&reachable,
            del_set &deleted_edges, int *&queue,
            int *&next_queue) { // TO:DO can be combined with fw_propagate in
                                // the other file, need to implement that
#if DEBUG
  printf("fw search %d\n", node);
#endif
  // int index = find_index(g.scc_map[node], g.rep_nodes, g.scc_count);
  // int temp_size = g.count_in_sccs[index];

#if ERROR_CHECK
  try {
    if (temp_size < 0) {
      throw(temp_size);
    }
  } catch (...) {
    printf("fw_new failed size, size was %d, %d was node from scc %d, %d is "
           "the index and %d is number of sccs\n",
           temp_size, node, g.scc_map[node], index, g.scc_count);
  }
#endif
  // int temp_size = g.n;
#if ERROR_CHECK
  printf("Making max size as %d\n", temp_size);
#endif
  int q_size = 0, nxt_q_size = 0;
  queue[q_size++] = node;
  fw_reach[node] = node;
  while (q_size) {
    //	for(int i=0; i<q_size; i++){ //doing separately to ensure we dont repeat
    // vertices later 		int vert = queue[i]; 		fw_reach[vert] =
    // node;
    //	}
    for (int i = 0; i < q_size; i++) {
      int vert = queue[i];
      int Odeg = out_degree(g, vert);
      int *Overts = out_vertices(g, vert);
      for (int j = 0; j < Odeg; j++) {
        int nVert = Overts[j];
        if (!reachable[nVert] and not_deleted(vert, nVert, deleted_edges) and
            g.scc_map[node] == g.scc_map[nVert] and fw_reach[nVert] != node) {
          next_queue[nxt_q_size++] = nVert;
          fw_reach[nVert] = node;
#if FULL_DEBUG
          printf("Marking %d as reachable from %d\n", nVert, node);
#endif
        }
      }
    }
    swap(queue, next_queue);
    q_size = nxt_q_size;
    nxt_q_size = 0;
#if FULL_DEBUG
    printf("Doing next a queue of size %d in fw_new function\n", q_size);
#endif

#if ERROR_CHECK
    if (q_size > temp_size) {
      printf("\n\n size execeeded \n\n");
      exit(1);
    }
#endif
  }

#if DEBUG
  printf("done fw search\n");
#endif
  return;
}

void find_scc(
    int &node, int *&to_change, int &num_changed, graph &g, int *&reachable,
    del_set &deleted_edges, int *&fw_reach, int *&queue,
    int *&next_queue) { // will return the list of verts that needs scc changed
#if DEBUG
  printf("Finding SCC of %d\n", node);
#endif
  // int index = find_index(g.scc_map[node], g.rep_nodes, g.scc_count);
  // int temp_size = g.count_in_sccs[index];

#if ERROR_CHECK
  try {
    if (temp_size < 0) {
      throw(temp_size);
    }
  } catch (...) {
    printf("find_scc failed size, size was %d, %d was node from scc %d, %d was "
           "the index and %d was scc number\n",
           temp_size, node, g.scc_map[node], index, g.scc_count);
  }
#endif

  int q_size = 0, nxt_q_size = 0;
  queue[q_size++] = node;
  to_change[num_changed++] = node;
  reachable[node] = 1;
  while (q_size) {
    for (int i = 0; i < q_size; i++) {
      int vert = queue[i];
      int Ideg = in_degree(g, vert);
      int *Iverts = in_vertices(g, vert);
      for (int j = 0; j < Ideg; j++) {
        int nVert = Iverts[j];
        if (not_deleted(nVert, vert, deleted_edges) and
            node == fw_reach[nVert] and !reachable[nVert] and
            g.scc_map[nVert] ==
                g.scc_map[node]) { // last confition might be redundant
          next_queue[nxt_q_size++] = nVert;
#if FULL_DEBUG
          printf("Marking %d reachable as reachable from %d\n", vert, node);
#endif
          reachable[nVert] = 1;
          to_change[num_changed++] =
              nVert; // adding this vertex beacuse it needs scc change
        }
      }
    }
    swap(queue, next_queue);
    q_size = nxt_q_size;
    nxt_q_size = 0;

#if FULL_DEBUG
    printf("Doing next a queue of size %d in fw_new function\n", q_size);
#endif

#if ERROR_CHECK
    if (q_size > temp_size) {
      printf("\n\n size execeeded \n\n");
      exit(1);
    }
#endif
  }
}

void update_sccs(int &vert, int *&to_change, int &size, graph &g) {
#if DEBUG
  printf("updating scc for %d\n", vert);
#endif
  int index = find_index(g.scc_map[vert], g.rep_nodes, g.scc_count);

#if ERROR_CHECK
  try {
    if (g.count_in_sccs[index] < size + 1) {
      throw(index);
    }
  } catch (...) {
    printf("SUS_MAX, %d rep_node's scc going from %d to %d\n", g.scc_map[vert],
           g.count_in_sccs[index], g.count_in_sccs[index] - size);
  }
#endif

  g.count_in_sccs[index] -= size; // reducing the number of scc nodes

  for (int i = 0; i < size; i++) {

#if FULL_DEBUG
    printf("changing %d's scc from %d to %d\n where scc_map is at %p\n",
           to_change[i], g.scc_map[to_change[i]], vert, g.scc_map);
#endif

    g.scc_map[to_change[i]] = vert;
  }

  g.rep_nodes[g.scc_count] = vert;
  g.count_in_sccs[g.scc_count] = size;
  g.scc_count++;
}

void make_scc(int &vert, graph &g, int *&fw_reach, int *&reachable,
              del_set &deleted_edges, int *&queue, int *&next_queue,
              int *&to_change) {
#if DEBUG
  printf("Starting to make scc of vertex %d\n", vert);
#endif
  // printf("Starting to make scc of vertex %d\n", vert);
  fw_new(vert, g, fw_reach, reachable, deleted_edges, queue, next_queue);
  int num_changed = 0;
  // int index = find_index(g.scc_map[vert], g.rep_nodes, g.scc_count);
  find_scc(vert, to_change, num_changed, g, reachable, deleted_edges, fw_reach,
           queue, next_queue);
  update_sccs(vert, to_change, num_changed, g);
#if DEBUG
  printf("done making scc\n");
#endif
}

void create_new_sccs(
    int *&unreachable_verts, int *&reachable, int &num_vrts, graph &g,
    int *&fw_reach,
    del_set &deleted_edges) { // currently we go vertex by vertex and form a new
                              // scc for that particular vertex
#if Nmessage
  printf("starting to create new sccs\n");
  printf("%d verts are unreachable out of %d in total\n", num_vrts, g.n);
#endif

  //
  int *queue = new int[g.n];
  int *next_queue = new int[g.n];
  int *to_change = new int[g.n];
  //

  for (int i = 0; i < num_vrts;
       i++) { // this part not yet to parallelise due to potential conflict in
              // forward update
    int vert = unreachable_verts[i];
    if (!reachable[vert]) {
      make_scc(vert, g, fw_reach, reachable, deleted_edges, queue, next_queue,
               to_change);
    }
  }

  //
  delete[] queue;
  delete[] next_queue;
  delete[] to_change;
//
#if Nmessage
  printf("Finished creating new sccs\n");
#endif
}

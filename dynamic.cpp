#include <cstdio>
#include <unordered_map>
#include <unordered_set>
#include <utility>

struct pair_hash {
  inline std::size_t operator()(const std::pair<int, int> &v) const {
    return v.first * 31 + v.second;
  }
};

typedef unordered_set<pair<int, int>, pair_hash> del_set;

void update_graph(graph &g, char *&file, double &avg_degree,
                  int &max_deg_vert) {
  FILE *Ufile = fopen(file, "r");
  int src, dst;
  char op;
  int add_cnt = 0;
  int del_cnt = 0;
  del_set deleted_edges;

  while (3 == fscanf(Ufile, "%c%*[ \t]%d%*[ \t]%d%*[\n]", &op, &src, &dst)) {
    if ('a' == op) {
      add_cnt++;
    }
  }
  fseek(Ufile, 0, SEEK_SET);
  printf("%d number of add was found\n", add_cnt);

  int *new_srcs = new int[add_cnt];
  int *new_dsts = new int[add_cnt];
  printf("new arrays created\n");

  add_cnt = 0;
  while (3 == fscanf(Ufile, "%c%*[ \t]%d%*[ \t]%d%*[\n]", &op, &src, &dst)) {
    if ('a' == op) {
      new_srcs[add_cnt] = src;
      new_dsts[add_cnt] = dst;
      add_cnt++;
    } else if ('d' == op) {
      deleted_edges.insert(make_pair(src, dst));
      del_cnt++;
    }
  }
  fseek(Ufile, 0, SEEK_SET);

  int *new_out_array = new int[add_cnt];
  int *new_in_array = new int[add_cnt];
  unsigned *new_out_defl = new unsigned[g.n + 1];
  unsigned *new_in_defl = new unsigned[g.n + 1];
  unsigned *temp_counts = new unsigned[g.n];

  for (unsigned i = 0; i < add_cnt; ++i)
    new_out_array[i] = 0;
  for (unsigned i = 0; i < add_cnt; ++i)
    new_in_array[i] = 0;
  for (int i = 0; i < g.n + 1; ++i)
    new_out_defl[i] = 0;
  for (int i = 0; i < g.n + 1; ++i)
    new_in_defl[i] = 0;

  for (int i = 0; i < g.n; ++i)
    temp_counts[i] = 0;
  for (unsigned i = 0; i < add_cnt; ++i)
    ++temp_counts[new_srcs[i]];
  for (int i = 0; i < g.n; ++i)
    new_out_defl[i + 1] = new_out_defl[i] + temp_counts[i];
  copy(new_out_defl, new_out_defl + g.n, temp_counts);
  for (unsigned i = 0; i < add_cnt; ++i)
    new_out_array[temp_counts[new_srcs[i]]++] = new_dsts[i];

  for (int i = 0; i < g.n; ++i)
    temp_counts[i] = 0;
  for (unsigned i = 0; i < add_cnt; ++i)
    ++temp_counts[new_dsts[i]];
  for (int i = 0; i < g.n; ++i)
    new_in_defl[i + 1] = new_in_defl[i] + temp_counts[i];
  copy(new_in_defl, new_in_defl + g.n, temp_counts);
  for (unsigned i = 0; i < add_cnt; ++i)
    new_in_array[temp_counts[new_dsts[i]]++] = new_srcs[i];

  for (int i = 0; i < g.n; ++i)
    temp_counts[i] = 0;

  int *final_out_array = new int[g.m + add_cnt - del_cnt];
  int *final_in_array = new int[g.m + add_cnt - del_cnt];
  unsigned *final_out_defl = new unsigned[g.n + 1];
  unsigned *final_in_defl = new unsigned[g.n + 1];

  for (unsigned i = 0; i < add_cnt; ++i)
    final_out_array[i] = 0;
  for (unsigned i = 0; i < add_cnt; ++i)
    final_in_array[i] = 0;
  for (int i = 0; i < g.n + 1; ++i)
    final_out_defl[i] = 0;
  for (int i = 0; i < g.n + 1; ++i)
    final_in_defl[i] = 0;

  // in_array
  for (int i = 0; i < g.n; i++) {
    int degree = 0;
    for (int j = g.in_degree_list[i]; j < g.in_degree_list[i + 1]; j++) {
      if (deleted_edges.find(make_pair(g.in_array[j], i)) ==
          deleted_edges.end()) {
        final_in_array[g.in_degree_list[i] + degree++] = g.in_array[j];
      }
    }
    for (int j = new_in_defl[i]; j < new_in_defl[i + 1]; j++) {
      if (deleted_edges.find(make_pair(new_in_array[j], i)) ==
          deleted_edges.end()) {
        final_in_array[g.in_degree_list[i] + degree++] = new_in_array[j];
      }
    }
    g.in_degree_list[i + 1] = degree;
  }

  // out_array
  for (int i = 0; i < g.n; i++) {
    int degree = 0;
    for (int j = g.out_degree_list[i]; j < g.out_degree_list[i + 1]; j++) {
      if (deleted_edges.find(make_pair(g.out_array[j], i)) ==
          deleted_edges.end()) {
        final_out_array[g.out_degree_list[i] + degree++] = g.out_array[j];
      }
    }
    for (int j = new_out_defl[i]; j < new_out_defl[i + 1]; j++) {
      if (deleted_edges.find(make_pair(new_out_array[j], i)) ==
          deleted_edges.end()) {
        final_out_array[g.out_degree_list[i] + degree++] = new_out_array[j];
      }
    }
    g.out_degree_list[i + 1] = degree;
  }

  delete[] g.in_array;
  delete[] g.out_array;
  delete[] g.in_degree_list;
  delete[] g.out_degree_list;

  g.in_array = final_in_array;
  g.out_array = final_out_array;
  g.out_degree_list = final_out_defl;
  g.in_degree_list = final_in_defl;
  g.m += add_cnt - del_cnt;

  delete[] new_out_array;
  delete[] new_in_array;
  delete[] new_in_defl;
  delete[] new_out_defl;
  delete[] temp_counts;

  delete[] new_dsts;
  delete[] new_srcs;

  avg_degree = 0.0;
  double max_degree = 0.0;
  for (int i = 0; i < g.n; ++i) {
    unsigned out_degree = g.out_degree_list[i + 1] - g.out_degree_list[i];
    unsigned in_degree = g.in_degree_list[i + 1] - g.in_degree_list[i];
    double degree = (double)out_degree * (double)in_degree;
    avg_degree += (double)out_degree;
    if (degree > max_degree) {
      max_deg_vert = i;
      max_degree = degree;
    }
  }
  avg_degree /= (double)g.n;
}

/*
//@HEADER
// *****************************************************************************
//
//       Multistep: (Strongly) Connected Components Algorithms
//              Copyright (2016) Sandia Corporation
//
// Under the terms of Contract DE-AC04-94AL85000 with Sandia Corporation,
// the U.S. Government retains certain rights in this software.
//
// Redistribution and use in source and binary forms, with or without
// modification, are permitted provided that the following conditions are
// met:
//
// 1. Redistributions of source code must retain the above copyright
// notice, this list of conditions and the following disclaimer.
//
// 2. Redistributions in binary form must reproduce the above copyright
// notice, this list of conditions and the following disclaimer in the
// documentation and/or other materials provided with the distribution.
//
// 3. Neither the name of the Corporation nor the names of the
// contributors may be used to endorse or promote products derived from
// this software without specific prior written permission.
//
// THIS SOFTWARE IS PROVIDED BY SANDIA CORPORATION "AS IS" AND ANY
// EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE
// IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR
// PURPOSE ARE DISCLAIMED. IN NO EVENT SHALL SANDIA CORPORATION OR THE
// CONTRIBUTORS BE LIABLE FOR ANY DIRECT, INDIRECT, INCIDENTAL, SPECIAL,
// EXEMPLARY, OR CONSEQUENTIAL DAMAGES (INCLUDING, BUT NOT LIMITED TO,
// PROCUREMENT OF SUBSTITUTE GOODS OR SERVICES; LOSS OF USE, DATA, OR
// PROFITS; OR BUSINESS INTERRUPTION) HOWEVER CAUSED AND ON ANY THEORY OF
// LIABILITY, WHETHER IN CONTRACT, STRICT LIABILITY, OR TORT (INCLUDING
// NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT OF THE USE OF THIS
// SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.
//
// Questions?  Contact  George M. Slota (gmslota@sandia.gov)
//                      Siva Rajamanickam (srajama@sandia.gov)
//                      Kamesh Madduri (madduri@cse.psu.edu)
//
// *****************************************************************************
//@HEADER
*/

using namespace std;

#include <assert.h>
#include <cstdlib>
#include <fstream>
#include <string.h>
#include <sys/time.h>
#include <vector>

#define VERBOSE 0
#define Nmessage 0
#define ERROR_CHECK 0
#define DEBUG 0
#define FULL_DEBUG 0
#define VERIFY 0
#define TIMING 0
#define TRIM_LEVEL 1
#define PRINTSCC 0
#define DYNAMIC_TIMING 1

#if DYNAMIC_TIMING
#include <omp.h>
#endif

#define THREAD_QUEUE_SIZE 2048
#define ALPHA 15.0
#define BETA 25

double timer() {
  struct timeval tp;
  gettimeofday(&tp, NULL);
  return ((double)(tp.tv_sec) + 1e-6 * tp.tv_usec);
}

typedef struct graph {
  int n;
  unsigned m;
  int *out_array;
  int *in_array;
  unsigned *out_degree_list;
  unsigned *in_degree_list;
  int scc_count; // no.of scc's
  int *scc_map;
  int *rep_nodes;     // array keeping count in
  int *count_in_sccs; //
} graph;
#define out_degree(g, n) (g.out_degree_list[n + 1] - g.out_degree_list[n])
#define in_degree(g, n) (g.in_degree_list[n + 1] - g.in_degree_list[n])
#define out_vertices(g, n) &g.out_array[g.out_degree_list[n]]
#define in_vertices(g, n) &g.in_array[g.in_degree_list[n]]

#include "scc_color.cpp"
#include "scc_dynamic_parts.cpp"
#include "scc_fwbw.cpp"
#include "scc_serial.cpp"
#include "scc_trim.cpp"
#include "scc_verify.cpp"

int common_read_edge(char *filename, int &n, unsigned &m, int *&srcs,
                     int *&dsts) {
  //  ifstream infile;
  //  string line;
  //  infile.open(filename);
  printf("reading %s\n", filename);
  FILE *file = fopen(filename, "r"); // opening the file
  if (fscanf(file, "%d %d", &n, &m) != 2) {
    fprintf(stderr, "Invalid format for n and m in the first line\n");
    fclose(file);
    return 1;
  }

#if DEBUG
  printf("vertices:%d, edges:%d\n", n, m);
#endif

  int src, dest;
  unsigned counter = 0;

  srcs = new int[m];
  dsts = new int[m];
  while (fscanf(file, "%d%*[ \t]%d", &src, &dest) == 2) {
#if DEBUG
//        printf("Source: %d, Destination: %d\n", src, dest);
#endif
    srcs[counter] = src;
    dsts[counter] = dest;
    ++counter;
  }

  fclose(file);
  return 0;
}

/*void read_edge(char* filename,
  int& n, unsigned& m,
  int*& srcs, int*& dsts)
{
  ifstream infile;
  string line;
  infile.open(filename);

  getline(infile, line, ' ');
  n = atoi(line.c_str());
  getline(infile, line);
  m = strtoul(line.c_str(), NULL, 10);
#if DEBUG
  printf("vertices:%d, edges:%d\n",n,m);
#endif

  int src, dst;
  unsigned counter = 0;

  srcs = new int[m];
  dsts = new int[m];
  for (unsigned i = 0; i < m; ++i)
  {
    getline(infile, line, ' ');
    src = atoi(line.c_str());
    getline(infile, line);
    dst = atoi(line.c_str());

    srcs[counter] = src;
    dsts[counter] = dst;
    ++counter;
  }

  infile.close();
}*/

void create_csr(int n, unsigned m, int *srcs, int *dsts, int *&out_array,
                int *&in_array, unsigned *&out_degree_list,
                unsigned *&in_degree_list, int &max_deg_vert,
                double &avg_degree) {
  out_array = new int[m];
  in_array = new int[m];
  out_degree_list = new unsigned[n + 1];
  in_degree_list = new unsigned[n + 1];

  for (unsigned i = 0; i < m; ++i)
    out_array[i] = 0;
  for (unsigned i = 0; i < m; ++i)
    in_array[i] = 0;
  for (int i = 0; i < n + 1; ++i)
    out_degree_list[i] = 0;
  for (int i = 0; i < n + 1; ++i)
    in_degree_list[i] = 0;

  unsigned *temp_counts = new unsigned[n];
  for (int i = 0; i < n; ++i)
    temp_counts[i] = 0;
  for (unsigned i = 0; i < m; ++i)
    ++temp_counts[srcs[i]];
  for (int i = 0; i < n; ++i)
    out_degree_list[i + 1] = out_degree_list[i] + temp_counts[i];
  copy(out_degree_list, out_degree_list + n, temp_counts);
  for (unsigned i = 0; i < m; ++i)
    out_array[temp_counts[srcs[i]]++] = dsts[i];

  for (int i = 0; i < n; ++i)
    temp_counts[i] = 0;
  for (unsigned i = 0; i < m; ++i)
    ++temp_counts[dsts[i]];
  for (int i = 0; i < n; ++i)
    in_degree_list[i + 1] = in_degree_list[i] + temp_counts[i];
  copy(in_degree_list, in_degree_list + n, temp_counts);
  for (unsigned i = 0; i < m; ++i)
    in_array[temp_counts[dsts[i]]++] = srcs[i];
  delete[] temp_counts;

  avg_degree = 0.0;
  double max_degree = 0.0;
  for (int i = 0; i < n; ++i) {
    unsigned out_degree = out_degree_list[i + 1] - out_degree_list[i];
    unsigned in_degree = in_degree_list[i + 1] - in_degree_list[i];
    double degree = (double)out_degree * (double)in_degree;
    avg_degree += (double)out_degree;
    if (degree > max_degree) {
      max_deg_vert = i;
      max_degree = degree;
    }
  }
  avg_degree /= (double)n;

#if DEBUG
  int max_out =
      out_degree_list[max_deg_vert + 1] - out_degree_list[max_deg_vert];
  int max_in = in_degree_list[max_deg_vert + 1] - in_degree_list[max_deg_vert];
  int max_tot = max_out > max_in ? max_out : max_in;
  printf("max deg vert: %d (%d), avg_degree %9.2lf\n", max_deg_vert, max_tot,
         avg_degree);
#endif
}

void output_scc(graph &g, int *scc_maps, char *output_file) {
  std::ofstream outfile;
  outfile.open(output_file);

  for (int i = 0; i < g.n; ++i)
    outfile << scc_maps[i] << std::endl;

  outfile.close();
}

void print_usage(char **argv) {
  printf("Usage: %s [graph] [optional: output file]\n", argv[0]);
  exit(0);
}

void run_scc(graph &g, int *&scc_maps, int max_deg_vert, double avg_degree,
             int vert_cutoff) {

  int num_trim = 0;
  int num_fwbw = 0;
  int num_color = 0;
  int num_serial = 0;

#if VERBOSE
  printf("Performing Trim step ... \n");
  double elt = timer();
  double start_time = elt;
#endif

  scc_maps = new int[g.n];
  bool *valid = new bool[g.n];
  int *valid_verts = new int[g.n];
  int num_valid = g.n;

#pragma omp parallel
  {
#pragma omp for nowait
    for (int i = 0; i < g.n; ++i)
      valid[i] = true;
#pragma omp for nowait
    for (int i = 0; i < g.n; ++i)
      scc_maps[i] = -1;
#pragma omp for nowait
    for (int i = 0; i < g.n; ++i)
      valid_verts[i] = i;
  }

  if (TRIM_LEVEL == 0) {
    /*  num_trim = scc_trim_none(g, valid,
        valid_verts, num_valid,
        scc_maps);*/
  } else if (TRIM_LEVEL == 1) {
    num_trim = scc_trim(g, valid, valid_verts, num_valid, scc_maps);
  } else if (TRIM_LEVEL == 2) {
    num_trim = scc_trim_complete(g, valid, valid_verts, num_valid, scc_maps);
    num_valid = 0;
    for (int i = 0; i < g.n; ++i)
      if (valid[i])
        valid_verts[num_valid++] = i;
  }

#if VERIFY
  scc_verify(g, scc_maps);
#endif

#if VERBOSE
  elt = timer() - elt;
  printf("\tDone, %9.6lf, %d verts trimmed\n", elt, num_trim);
  printf("Performing FWBW step ... \n");
  elt = timer();
#endif

  num_fwbw = scc_fwbw(g, valid, valid_verts, num_valid, max_deg_vert,
                      avg_degree, scc_maps);

#if VERIFY
  scc_verify(g, scc_maps);
#endif

#if VERBOSE
  elt = timer() - elt;
  printf("\tDone, %9.6lf, %d verts found\n", elt, num_fwbw);
  printf("Performing Coloring step ... \n");
  elt = timer();
#endif

  num_color =
      scc_color(g, valid, valid_verts, num_valid, vert_cutoff, scc_maps);

#if VERIFY
  scc_verify(g, scc_maps);
#endif

#if VERBOSE
  elt = timer() - elt;
  printf("\tDone, %9.6lf, %d sccs found\n", elt, num_color);
  printf("Performing Serial step ... \n");
  elt = timer();
#endif

  tarjan t(g, valid, valid_verts, num_valid, scc_maps);
  num_serial = t.run_valid();

#if VERBOSE
  elt = timer() - elt;
  printf("\tDone, %9.6lf, %d sccs found\n", elt, num_serial);
  start_time = timer() - start_time;
  printf("TOTAL: %9.6lf\n", start_time);
#endif

  delete[] valid;
  delete[] valid_verts;
}

void print_scc(graph &g) {
  for (int i = 0; i < g.n; i++) {
    printf("SCC of vertice %d is %d\n", i, g.scc_map[i]);
  }
}

void update_g_with_scc(graph &g) {
  int scc_count = 0;
  int *scc_map = g.scc_map;
  int *rep;
  int *counts;
  int curr_index = 0;
  unordered_map<int, int> temp_indexes;
  for (int i = 0; i < g.n; i++) {
    if (temp_indexes.find(scc_map[i]) == temp_indexes.end()) {
      temp_indexes[scc_map[i]] = curr_index++;
      scc_count++;
    }
  }
  // making it temporarily g.n, will see if realloncation can be done starting
  // with smaller arrays
  //	rep = new int[scc_count];
  //	counts = new int[scc_count];
  rep = new int[g.n];
  counts = new int[g.n];

  for (int i = 0; i < g.n; i++) {
    counts[i] = 0;
  }
  for (int i = 0; i < g.n; i++) {
    rep[temp_indexes[scc_map[i]]] = scc_map[i];
    counts[temp_indexes[scc_map[i]]]++;
  }
  g.scc_count = scc_count;
  g.rep_nodes = rep;
  g.count_in_sccs = counts;
}

int main(int argc, char **argv) {

  omp_set_dynamic(0);
  omp_set_num_threads(atoi(argv[3]));

#if DYNAMIC_TIMING
  double start = omp_get_wtime();
#endif
  setbuf(stdout, NULL);
  if (argc < 2)
    print_usage(argv);

  int *srcs;
  int *dsts;
  int n;
  unsigned m;
  int *out_array;
  int *in_array;
  unsigned *out_degree_list;
  unsigned *in_degree_list;
  int *scc_maps;
  int max_deg_vert;
  double avg_degree;
  int vert_cutoff = 10000;

#if VERBOSE
  double elt, start_time;
  printf("Reading %s ... ", argv[1]);
  elt = timer();
#endif

  common_read_edge(argv[1], n, m, srcs, dsts);

#if VERBOSE
  elt = timer() - elt;
  printf("Done, %9.6lf\n", elt);
  printf("Creating graph ... ");
  elt = timer();
#endif

#if DYNAMIC_TIMING
  double end = omp_get_wtime();
  printf("\n\n Took %f for reading files\n\n", end - start);
  start = omp_get_wtime();
#endif

  create_csr(n, m, srcs, dsts, out_array, in_array, out_degree_list,
             in_degree_list, max_deg_vert, avg_degree);
  graph g = {
      n, m,    out_array, in_array, out_degree_list, in_degree_list,
      0, NULL, NULL,      NULL}; // keeping NULL at start to create the obejct
  delete[] srcs;
  delete[] dsts;

#if VERBOSE
  elt = timer() - elt;
  printf("Done, %9.6lf, n: %d, m: %u\n", elt, n, m);
  printf("Doing Multistep ... ");
  elt = timer();
#endif

#if TIMING
  double exec_time = timer();
#endif

  run_scc(g, scc_maps, max_deg_vert, avg_degree, vert_cutoff);

#if TIMING
  exec_time = timer() - exec_time;
  printf("Multistep SCC time: %9.6lf\n", exec_time);
#endif

  //  scc_verify(g, scc_maps);

#if VERBOSE
  elt = timer() - elt;
  printf("Done, %9.6lf\n", elt);
#endif

#if DYNAMIC_TIMING
  end = omp_get_wtime();
  printf("\n\n Scc done in %f secs \n\n", end - start);
#endif

  //////////////////////////////////////////
  g.scc_map = scc_maps;
  update_g_with_scc(g);
// #if PRINTSCC
// print_scc(g);
// #endif

//////////////////////////////////////////
//////////////////////////////////////////
#if DYNAMIC_TIMING
  start = omp_get_wtime();
#endif
  dynamic(g, g.n, g.scc_map, argv[2]);
#if DYNAMIC_TIMING
  end = omp_get_wtime();
#endif

#if DYNAMIC_TIMING
  printf("\n\n Fully update in %f secs \n\n", end - start);
#endif

  //////////////////////////////////////////

  /*if (argc == 3)
    output_scc(g, scc_maps, argv[2]);*/

#if PRINTSCC
  print_scc(g);
#endif

  /*  for (int i = 0; i < g.n; i++) {
      printf("%d -> %d\n", i, g.scc_map[i]);
    } */

  clear_graph(g);

  return 0;
}

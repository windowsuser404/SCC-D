#include "scc_insert.cpp"
#include "scc_delete.cpp"
#include <iostream>
#include <string>

void printgraph(graph g, int* scc_maps, int* scc_index, int* inverse_scc){
#if DEBUG
	printf("\n\n");
	printf("Printing condensed graphs");
	printf("Condensed graph has %d vertices\n",g.n);
#endif
	for(int i=0; i<g.n; i++){
		printf("%d respresenting scc %d\n",i,scc_index[i]);
	}
	printf("\n\n");
	for(int i=0;i<g.n;i++){
		int out = in_degree(g, i);
		int* verts = in_vertices(g, i);
		for(int j=0; j<out; j++){
			printf("%d<-%d\n",i,verts[j]);
		}
		out = out_degree(g, i);
		verts = out_vertices(g, i);
		for(int j=0; j<out; j++){
			printf("%d->%d\n",i,verts[j]);
		}
	}
	printf("\n\n");
}

void edge_insertion(graph& g, FILE* file, int* insertsrc_avail, int* insertdst_avail, int& inserts, int num_verts, int* scc_maps){
	int root_count, condensed_edges, extra_condense_count=0;
	int *scc_outarr, *scc_inarr, *scc_index, *inverse_scc;
	unsigned int *scc_outdegree_list, *scc_indegree_list;
	printf("Going to condense\n");
       	insert_condense(g, scc_maps, num_verts, g.m, scc_outarr, 
			scc_inarr, scc_outdegree_list, scc_indegree_list, 
			root_count, condensed_edges, scc_index, inverse_scc);	
	printf("Finished condensing\n");

#if DEBUG
	printf("\n\ninarray at %p\n",&scc_inarr);
        for(int i=0;i<condensed_edges;i++){
         printf("%d\n",scc_inarr[i]);
        }
        printf("\n\n");
       printf("\n\noutarray at %p\n",&scc_outarr);
        for(int i=0;i<condensed_edges;i++){
         printf("%d\n",scc_outarr[i]);
        }
        printf("\n\n");
#endif



	graph condensed_g = {root_count, (unsigned int)condensed_edges, scc_outarr, scc_inarr, scc_outdegree_list, scc_indegree_list};
#if DEBUG
	printgraph(condensed_g, scc_maps, scc_index, inverse_scc);
#endif
	//updating diff csr
	char op;
	int* scc_diff_src = new int[inserts];
	int* scc_diff_dst = new int[inserts]; // for condensed graph
	int src, dst, extra_in=0, extra_out=0, insert_count=0;
	int* diff_in_src = new int[inserts]; // inserts is the upper bound
	int* diff_in_dst = new int[inserts];
	int* diff_out_src = new int[inserts];
	int* diff_out_dst = new int[inserts];
#if DEBUG
	for(int i=0; i<num_verts; i++){
		printf("%d has %d slots\n",i,insertsrc_avail[i]);
	}
#endif
	printf("Updating diff csr\n");
	while(fscanf(file, "%c%*[ \t]%d%*[ \t]%d%*[\n]", &op, &src, &dst) == 3){
		if(op=='a'){
			//printf("Line has %d -> %d\n",src,dst);
			////////////////////////adding src//////////////////////
			if(insertsrc_avail[src]>0){
				int out_deg = out_degree(g, src);
				int* out_verts = out_vertices(g, src);
				for(int i=0;i<out_deg;i++){
					if(out_verts[i]==-1){
						out_verts[i]=dst;
						insertsrc_avail[src]--;
						break;
					}
				}
			}
			else{
#if DEBUG
				printf("trying extra out extraout=%d\n",extra_out);
#endif
				diff_out_src[extra_out] = src;
				diff_out_dst[extra_out] = dst;
				extra_out++;	
			}
			
			if(scc_maps[src]!=scc_maps[dst]){
#if DEBUG
				printf("adding src %d, scc=%d, vertex %d in condensed\n",src,scc_maps[src],inverse_scc[scc_maps[src]]);
#endif
				scc_diff_src[extra_condense_count] = inverse_scc[scc_maps[src]];
			}
			////////////////////////////////////////////////////////
			////////////////////////adding dst/////////////////////

			if(insertdst_avail[dst]>0){
				int in_deg = in_degree(g, dst);
				int* in_verts = in_vertices(g, dst);
				for(int i=0;i<in_deg;i++){
					if(in_verts[i]==-1){
						in_verts[i]=src;
						insertdst_avail[dst]--;
						break;
					}
				}
			}
			else{
#if DEBUG
				printf("trying extra in\n");
#endif
				diff_in_src[extra_in] = src;
				diff_in_dst[extra_in] = dst;
				extra_in++;
			}
			if(scc_maps[src]!=scc_maps[dst]){
#if DEBUG
				printf("adding dst %d, scc=%d, vertex %d in condensed\n",dst,scc_maps[dst],inverse_scc[scc_maps[dst]]);
#endif
				scc_diff_dst[extra_condense_count] = inverse_scc[scc_maps[dst]]; 
				extra_condense_count++;
			}
			///////////////////////////////////////////////////////////
			insert_count++;
		}
	}

	printf("diff csr updated\n");

	int* placeholder1 = new int[inserts];
	unsigned int* placeholder2 = new unsigned int[inserts];
	int* diff_out_vert;
	unsigned int* diff_outdeg_list;
	int* diff_in_verts;
	unsigned int* diff_indeg_list;
	unsigned int* scc_diff_outdeg_list;
	unsigned int* scc_diff_indeg_list;
	int* scc_diff_out;
	int* scc_diff_in;

	new_create_csr(num_verts, extra_out, diff_out_src, 
			diff_out_dst, diff_out_vert, placeholder1, 
			diff_outdeg_list, placeholder2);
#if DEBUG
	printf("Line 95\n");
#endif

	new_create_csr(num_verts, extra_in, diff_in_src, 
			diff_in_dst, placeholder1, diff_in_verts, 
			placeholder2, diff_indeg_list);
#if DEBUG
	printf("Line 101\n");
#endif

	delete [] placeholder1;
	delete [] placeholder2;
	delete [] diff_out_src;
	delete [] diff_out_dst;
	delete [] diff_in_src;
	delete [] diff_in_dst;
	new_create_csr(root_count, extra_condense_count, scc_diff_src, 
			scc_diff_dst, scc_diff_out, scc_diff_in, 
			scc_diff_outdeg_list, scc_diff_indeg_list);
#if DEBUG
	printf("line 113\n");	
#endif

	delete [] scc_diff_src;
	delete [] scc_diff_dst;

	//scc algorithm on condensed graph
	condensed_g.m = condensed_g.m + extra_condense_count;
	printf("Strting scc on condensend graph\n");	
	condense_scc_update(condensed_g, scc_maps, scc_index, inverse_scc, scc_diff_out, scc_diff_in, scc_diff_outdeg_list, scc_diff_indeg_list); //assuming this updates and gives us the result

	//////////////////////////////////
	
	for(int i=0;i<num_verts;i++){
                scc_maps[i] = scc_maps[scc_maps[i]];
        }

 	delete [] scc_diff_outdeg_list;
        delete [] scc_diff_indeg_list;
        delete [] scc_diff_out;
        delete [] scc_diff_in;
	delete [] inverse_scc;
	delete [] scc_index;
}

void edge_deletion(graph& g, FILE* file, int* insertsrc_avail, int* insertdst_avail, int& inserts, int* scc_maps){
	// get data structure needed for deletion, those trees
	temp_verts = new int[g.n];
	for(int i=0; i<g.n; i++){
		temp_verts[i] = i; //temporary to initaite a block of vertices [0,1...n-1]
	}

	temp_scc = new int[g.n];
	for(int i=0; i<g.n; i++){
		temp_scc[i] = scc_maps[i]; // temporary to accomodate changes for creating trees
	}

	Create_SCC_trees(temp_verts, g.n, temp_scc, NULL , 0);
	delete [] temp_verts;
	delete [] temp_scc;

	// updating csr part
	char op;
	int src, dst, found;
	// deleting for diff csr
	while(fscanf(file, "%c%*[ \t]%d%*[ \t]%d%*[\n]", &op, &src, &dst) == 3){
		if(op=='d'){
			printf("Line has %d -> %d\n",src,dst);
			//changing in "out" side
			int out_deg = out_degree(g, src);
			int* out_verts = out_vertices(g, src);
			found = 0;
			for(int i=0;i<out_deg;i++){
				if(found){
					break; //might be useful later
				}
				if(out_verts[i]==dst){
					out_verts[i] = -1;
					found = 1;
					insertsrc_avail[src]++;	
				}
			}

			//changing in "in" side
			int in_deg = in_degree(g, dst);
			int* in_verts = in_vertices(g, dst);
			found = 0;
			for(int i=0;i<in_deg;i++){
				if(found){
					break;
				}
				if(in_verts[i]==src){
					in_verts[i] = -1;
					found = 1;
					insertdst_avail[dst]++;
				}
			}
		}
		else{
#if DEBUG
			printf("Dedected line %c %d %d\n",op,src,dst);
			printf("Adding one to inserts:%d\n",inserts);
#endif
			inserts++;
		}	
	}
}

void dynamic(graph& g, int verts, int* scc_maps){
	while(true){
		int inserts;
		string file_name ;//= "test.update"; //should add bound check later
		int* insertsrc_avail = new int[verts]; // array to count insertions available, might be useful in insertion of edges
		int* insertdst_avail = new int[verts]; // same but for dsts
		printf("Enter the new file name: ");
		getline(cin, file_name, '\n');
		cout << "file being opened"+file_name+"\n";
		//FILE* file = fopen(file_name.c_str(),"r");
		double start = omp_get_wtime();
		FILE* file = fopen("test.update","r");
		if(file){
			printf("file opening successful\n");
		}
		for(int i=0;i<verts;i++){
			insertsrc_avail[i] = 0;
			insertdst_avail[i] = 0;
		}
		edge_deletion(g, file, insertsrc_avail, insertdst_avail, inserts);
		if (feof(file)) {
			// End of file
			printf("Reached end of file\n");;
		}
		else{
			printf("We havent reached\n");
		}
		fseek(file, 0, SEEK_SET);
#if DEBUG
		printf("%d insertions dedected\n",inserts);
#endif
		edge_insertion(g, file, insertsrc_avail, insertdst_avail, inserts, verts, scc_maps);
		double end = omp_get_wtime();
		printf("\n\nUpdated in %f secs\n\n",end-start);
		break;
	}
}



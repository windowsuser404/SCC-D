// will test out different ways to create the new scc for the vert not part of original scc

bool not_deleted(int src, int dst, unordered_set<pair<int, int>> deleted_edges){
	pair temp = make_pair(src, dst);
	if(deleted_edges.find(temp)!=deleted_edges.end()){
		return false;
	}
	else{
		return true;
	}
}

void fw_new(int node){ //TO:DO can be combined with fw_propagate in the other file, need to implement that
	int* queue = new int[];
	int* nxt_queue = new int[]; // TO:DO figure out the sizes properly
	int q_size=0, nxt_q_size = 0;
	queue[q_size++] = node;
	while(q_size){
		for(int i=0; i<q_size; i++){ //doing separately to ensure we dont repeat vertices later
			int vert = queue[i];
			fw_reach[vert] = vert;
		}
		for(int i=0; i<q_size; i++){
			int vert = queue[i];
			int Odeg = out_degree(g, vert);
			int* Overts = out_vertices(g, vert);
			for(int j=0; j<Odeg; j++){
				int nVert = Overts[i];
				if(!reachable[nVert] and not_deleted(vert, nvert, deleted_edges) and scc_map[vert]==scc_map[nVert] and fw_reach[nVert]!=node){
					nxt_queue[nxt_q_size++] = nVert;
				}
			}
		}
		queue = nxt_queue;
		q_size = nxt_q_size;
		nxt_q_size = 0;
	}
	delete queue;
	delete nxt_queue;
}

int* find_scc(int node, int* to_change, int& num_changed, graph& g){ //will return the list of verts that needs scc changed
	int* queue = new int[g.count_in_sccs[g.scc_map[node]]];
	int* nxt_queue = new int[g.count_in_sccs[g.scc_map[node]]]; // TO:DO figure out the sizes properly
	int q_size=0, nxt_q_size=0;
	queue[q_size++] = node;
	while(q_size){
		for(int i=0; i<q_size; i++){
			int vert = queue[i];
			reachable[vert] = 1;
			to_change[num_changed++] = vert; //adding this vertex beacuse it needs scc change
			int Ideg = in_degree(g, vert);
			int* Iverts = in_vertices(g, vert);
			for(int j=0; j<Ideg; j++){
				int nVert = Overts[i];
				if(not_deleted(nVert, vert, deleted_edges) and vert==fw_reach[nVert] and !reachable[nVert]){ // last confition might be redundant
					nxt_queue[nxt_q_size++] = nVert;
				}
			}
		}
		queue = nxt_queue;
		q_size = nxt_q_size;
		nxt_q_size=0;
	}

	delete queue;
	delete nxt_queue;
}

void update_sccs(int vert, int* to_change, int size, graph& g){
	for(int i=0; i<size; i++){
		g.scc_map[to_change[i]] = vert;
	}
}

void make_scc(int vert, graph& g){
	fw_new(vert);
	int num_changed=0;
	int* to_change = new int[]; //TO:DO will decide size
	find_scc(vert, to_change, num_changed, g);
	update_sccs(vert, to_change, num_changed, g);
	delete [] to_change;
}

void create_new_sccs(int* unreachable_verts, int* reachable, int& num_vrts, graph& g, int* fw_reach){ // currently we go vertex by vertex and form a new scc for that particular vertex
	for(int i=0; i<num_vrts; i++){ // this part not yet to parallelise due to potential conflict in forward update
		int vert = unreachable_verts[i];
		if(!reachable[vert]){
			make_scc(vert, g);
		}
	}
}

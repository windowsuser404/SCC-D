void edge_correction(){
}

void lift_up(tree_node* node, tree_node* new_parent){
	node->parent = new_parent;
	new_parent->childs++;
	new_parent->children[childs] = node;
	new_parent->dag->vert_no++;
	new_parent->dag->vertices[vert_no] = node->scc_node;
}

void tree_update(int*& unreachable, int& count, tree_node* node){
	int siblings = node->parent->childs;
	tree_node* Parent = node->parent;
	for(int i=0; i<count; i++){
		tree_node* curr_node = node->children[unreachable[i]];
		node->children[unreachable[i]] = -1;
		lift_up(curr_node, parent);
	}
}

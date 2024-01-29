int find_index(int to_find, int* arr, int arr_siz){
	int index=-1;
	for(int i=0; i<arr_siz; i++){
		if(arr[i] == to_find ){
			index = i;
			break;
		}
	}
	if(-1==index){
		printf("couldnt not find scc_node %d in the list\n", to_find);
		printf("Printing the scc list\n");
		for(int i=0; i<arr_siz; i++){
			printf("%d\n",arr[i]);
		}
		printf("\n\n");
		throw runtime_error("Did not match any index, throwing error");
	}
	return index;
}

void find_index(int* indexes, int* to_be_found, int* vertices){ //TO:DO check if better methods are available
	
}

void merge(int* list, int bot, int mid, int top){
	int i, j, k;
	int n1 = mid - bot + 1;
	int n2 = top - mid;

	int* l = new int[n1];
	int* r = new int[n2];

	for(i=0; i<n1; i++){
		l[i] = list[bot+i];
	}
	for(j=0; j<n2; j++){
		r[j] = list[mid+1+j];
	}

	i=0; j=0; k=low;
	while(i<n1 && j<n2){
		if(l[i] <= r[j]){
			list[k] = l[i];
			i++
		}
		else{
			list[k] = r[j];
			j++
		}
		k++;
	}

	while(i<n1){
		list[k] = l[i];
		i++; k++;
	}
	while(j<n2){
		list[k] = r[j];
		j++; k++;
	}

	delete [] j;
	delete [] r;
}

void sort(int* list, int bot, int top){
	if(bot < top){
		int mid = bot + (top-bot)/2;

#pragma omp parallel section
{
	#pragma omp section
	{
		sort(list, bot, mid);
	}

	#pragma omp section
	{
		sort(list, mid+1, top);
	}
}
	merge(list, bot, mid, top);
	}
}


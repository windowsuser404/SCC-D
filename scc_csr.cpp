void new_create_csr(int n, unsigned m,
		int*& srcs, int*& dsts,
		int*& out_array, int*& in_array,
		unsigned*& out_degree_list, unsigned*& in_degree_list)
{
#if DEBUG
	printf("Starting csr\n");
#endif
	out_array = new int[m];
	in_array = new int[m];
	out_degree_list = new unsigned[n+1];
	in_degree_list = new unsigned[n+1];

	for (unsigned i = 0; i < m; ++i)
		out_array[i] = 0;
	for (unsigned i = 0; i < m; ++i)
		in_array[i] = 0;
	for (int i = 0; i < n+1; ++i)
		out_degree_list[i] = 0;
	for (int i = 0; i < n+1; ++i)
		in_degree_list[i] = 0;

	unsigned* temp_counts = new unsigned[n];
	for (int i = 0; i < n; ++i)
		temp_counts[i] = 0;
	for (unsigned i = 0; i < m; ++i)
		++temp_counts[srcs[i]];
	for (int i = 0; i < n; ++i)
		out_degree_list[i+1] = out_degree_list[i] + temp_counts[i];
	copy(out_degree_list, out_degree_list + n, temp_counts);
	for (unsigned i = 0; i < m; ++i)
		out_array[temp_counts[srcs[i]]++] = dsts[i];

	for (int i = 0; i < n; ++i)
		temp_counts[i] = 0;
	for (unsigned i = 0; i < m; ++i)
		++temp_counts[dsts[i]];
	for (int i = 0; i < n; ++i)
		in_degree_list[i+1] = in_degree_list[i] + temp_counts[i];
	copy(in_degree_list, in_degree_list + n, temp_counts);
	for (unsigned i = 0; i < m; ++i)
		in_array[temp_counts[dsts[i]]++] = srcs[i];
	delete [] temp_counts;

#if DEBUG
	printf("csr created\n");
#endif
}

struct DAG;
struct scc_tree;
typedef struct DAG DAG;
typedef struct scc_tree scc_tree;

struct DAG{
	scc_tree* member_trees;	
};

struct scc_tree{
        int scc_node;
        scc_tree* parent;
        DAG* dag;
};


//typedef struct tree_node TNODE;
struct tree_node {
	struct point *value;
	struct tree_node *left;
	struct tree_node *right;
};

struct tree_node *create_node(struct point *p);

struct tree_node *build_kdtree(struct point *points, int num_attr, int start, int end, int depth);

void free_kdtree(struct tree_node *kdtree);

struct point *nearest_nodes(struct tree_node *kdtree, struct point p, int num_attr, int K);

void print_nodes(struct tree_node *kdtree);
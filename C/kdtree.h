//typedef struct tree_node TNODE;
struct tree_node {
	struct point *value;
	struct tree_node *left;
	struct tree_node *right;
};

struct tree_node *create_node(struct point *p);

struct tree_node *build_kdtree(struct point **points, int index, int num_points, int depth);
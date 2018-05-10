#include <stdio.h>
#include <stdlib.h>

#include "utils.h"
#include "kdtree.h"


struct tree_node *create_node(struct point *p) {
	struct tree_node *node;
	node = (struct tree_node*) malloc(sizeof(struct tree_node));
	node->value = p;
	node->left = NULL;
	node->right = NULL;
	return node;
}


struct tree_node *build_kdtree(struct point *points, int num_attr, int start, int end, int depth) {

	if (start >= end) return NULL;

	int axis = depth % num_attr;

	sort_per_axis(points, end, num_attr, axis);

	int middle = ((start+end) / 2);
	struct tree_node *node = create_node(&points[middle]);

	node->left = build_kdtree(points, num_attr, start, middle, depth+1);
	node->right = build_kdtree(points, num_attr, middle+1, end, depth+1);

	return node;
}

struct point *nearest_nodes(struct tree_node *kdtree, struct point p, int num_attr, int K) {

	float dist = euclidean_dist(p, kdtree.value, num_attr);

	printf("%f\n", dist);

	return NULL;
}
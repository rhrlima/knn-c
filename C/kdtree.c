#include <stdio.h>
#include <stdlib.h>
#include "utils.h"
#include "kdtree.h"


struct tree_node *build_kdtree(struct point **points, int index, int num_points, int depth) {

	printf("%d, %d\n", index, num_points);

	if (index >= num_points) {printf("%d\n", points[index]->label); return NULL;}

	if (depth == 10) return NULL;

	int i;
	for (i = index; i < num_points; i++)
		printf("%d ", points[i]->label);
	printf("\n");

	struct tree_node *node = create_node(points[index]);

	node->left = build_kdtree(points, index+1, num_points/2, depth+1);
	node->right = build_kdtree(points, num_points/2+1, num_points, depth+1);

	return node;
}


struct tree_node *create_node(struct point *p) {
	struct tree_node *node;
	node = (struct tree_node*) malloc(sizeof(struct tree_node));
	node->value = p;
	node->left = NULL;
	node->right = NULL;
	return node;
}
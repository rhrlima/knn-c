#include <stdio.h>
#include <stdlib.h>

#include "utils.h"
#include "kdtree.h"

int main() {

	float vet[] = {0.0, 1.0, 2.0};

	struct point **p;
	struct tree_node *n;
	
	int i, size = 10;

	p = (struct point **) malloc(size * sizeof(struct point*));
	for (i = 0; i < size; i++) {
		p[i] = create_point(vet, i);
	}

	for (i = 0; i < size; i++) {
		printf("P %d\n", p[i]->label);
	}

	n = build_kdtree(p, 0, size, 0);

	return 0;
}
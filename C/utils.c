#include <stdlib.h>
#include "utils.h"


struct point *create_point(float *values, int label) {
	struct point *p;
	p = (struct point*) malloc(sizeof(struct point));
	p->values = values;
	p->label = label;
	return p;
}


void copy_point(struct point *src, struct point *dst, int size) {
	int i;
	#pragma omp parallel for
	for (i = 0; i < size; i++)
		dst->values[i] = src->values[i];
	dst->label = src->label;
}


float euclidean_dist(struct point pA, struct point pB, int num_attrs) {
	
	int i;
	float sum = 0.0;
	float diffs[num_attrs];

	for (i = 0; i < num_attrs; i++) {
		sum += (pA.values[i] - pB.values[i]) * (pA.values[i] - pB.values[i]);
	}

	return sqrt(sum);
}


void sort_per_axis(struct point *points, int num_points, int num_attrs, int axis) {
	int i, j;
	struct point *temp;

	temp = (struct point *) malloc(sizeof(struct point));
	temp->values = (float *) malloc(num_attrs * sizeof(float));

	for (i = 0; i < num_points; i++) {

		int min_index = i;
		float min_value = points[i].values[axis];

		for (j = i + 1; j < num_points; j++) {

			if (points[j].values[axis] < min_value) {

				min_index = j;
				min_value = points[j].values[axis];

				copy_point(&points[i], temp, num_attrs);
				copy_point(&points[min_index], &points[i], num_attrs);
				copy_point(temp, &points[min_index], num_attrs);
			}
		}
	}

	free(temp->values);
	free(temp);
}
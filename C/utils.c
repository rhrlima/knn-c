#include <stdlib.h>
#include "utils.h"




struct point *create_point(float *values, int label) {
	struct point *p;
	p = (struct point*) malloc(sizeof(struct point));
	p->values = values;
	p->label = label;
	return p;
}


int compare(struct point pA, struct point pB, int size) {
	int i, j;
	for (i = 0; i < size; i++) {
		for (j = 0; j < size; j++) {
			if (pA.values[i] < pB.values[j]) { //A < B
				return -1;
			} else if (pA.values[i] > pB.values[j]) { //B < A
				return 1;
			} else { //A = B
				continue;
			}
		}
	}
	return 0; //iguais
}



void copy_point(struct point *src, struct point *dst, int size) {
	int i;
	#pragma omp parallel for
	for (i = 0; i < size; i++)
		dst->values[i] = src->values[i];
	dst->label = src->label;
}


void insertion_sort(struct point * points, float dists[], int num_points, int num_attrs, int K) {
	int i, j;
	struct point *temp;
	float tempd;

	temp = (struct point *) malloc(sizeof(struct point));
	temp->values = (float *) malloc(num_attrs * sizeof(float));

	for (i = 0; i < K; i++) {

		int min_index = i;
		float min_value = dists[i];

		for (j = i + 1; j < num_points; j++) {

			if (dists[j] < min_value) {

				min_index = j;
				min_value = dists[j];

				copy_point(&points[i], temp, num_attrs);
				copy_point(&points[min_index], &points[i], num_attrs);
				copy_point(temp, &points[min_index], num_attrs);

				tempd = dists[i];
				dists[i] = dists[min_index];
				dists[min_index] = tempd;
			}
		}
	}

	free(temp->values);
	free(temp);
}
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <math.h>
#include <time.h>

#include <omp.h>


#include "utils.h"
#include "kdtree.h"


#define NAMEBUFFER 100

// struct point {
// 	float *values;
// 	int label;
// };

struct dataset {
	int num_points;
	int num_attrs;
	int num_class;
	struct point * points;
};


struct dataset * read_dataset(char * file_name, int max_lines);

void free_dataset(struct dataset * dset);

int knn(struct dataset *train, struct point *p, int K);

void get_k_small(struct point * points, float dists[], int num_points, int num_attrs, int K);

void get_confusion_matrix(struct dataset * test, int * pred);


int main(int argc, char *argv[]) {

	clock_t start, end, diff;
	int i, j, hit, rej, err, K, max_tr_lines, max_te_lines, *pred;
	char train_file[NAMEBUFFER], test_file[NAMEBUFFER];
	struct dataset *train, *test;

	if (argc == 4) {

		strcpy(train_file, argv[1]);
		strcpy(test_file, argv[2]);
		K = atoi(argv[3]);
		max_tr_lines = 0;
		max_te_lines = 0;

	} else if (argc == 6) {

		strcpy(train_file, argv[1]);
		strcpy(test_file, argv[2]);
		K = atoi(argv[3]);
		max_tr_lines = atoi(argv[4]);
		max_te_lines = atoi(argv[5]);

	} else {
		printf("Wrong number of parameters.\nExiting.\n");
		exit(0);
	}

	start = clock();

	train = read_dataset(train_file, max_tr_lines);
	test = read_dataset(test_file, max_te_lines);

	hit = rej = err = 0;
	pred = (int*) malloc(test->num_points * sizeof(int));

	printf("TR: %d\t TE: %d\n", train->num_points, test->num_points);

	for (i = 0; i < test->num_points; i++) {

		pred[i] = 0;//knn(train, &test->points[i], K);

		if (pred[i] == -1) rej++;
		else if (pred[i] == test->points[i].label) hit++;
		else err++;
	}

	err = test->num_points - (hit+rej);

	printf("TR: %d\t TE: %d\n", train->num_points, test->num_points);
	printf("acc: %5d \t %f\n", hit, (float) hit / test->num_points);
	printf("rej: %5d \t %f\n", rej, (float) rej / test->num_points);
	printf("err: %5d \t %5d\n", err, hit+rej+err);

	get_confusion_matrix(test, pred);

	//----
	struct tree_node *kdtree = build_kdtree(test->points, test->num_attrs, 0, test->num_points, 0);
	nearest_node(kdtree)
	//----

	free(pred);
	free_dataset(train);
	free_dataset(test);

	end = clock();

	printf("Time: %f\n", (double) (end - start) / CLOCKS_PER_SEC);

	return 0;
}


struct dataset * read_dataset(char * file_name, int max_lines) {

	FILE *file;

	int BUFFER_SIZE = 10000;
	char buffer[BUFFER_SIZE];

	int i, j;
	struct dataset * dset;

	file = fopen(file_name, "r");
	if (!file) {
		printf("Error: file %s was not found.\nExiting.\n", file_name);
		exit(0);
	}

	dset = (struct dataset *) malloc (sizeof(struct dataset));

	fgets(buffer, BUFFER_SIZE, file);

	dset->num_points = atoi(strtok(buffer, " "));
	dset->num_attrs  = atoi(strtok(NULL, " "));
	dset->num_class  = atoi(strtok(NULL, " "));

	dset->num_points = (max_lines == 0 || max_lines > dset->num_points) ? dset->num_points : max_lines;

	dset->points = (struct point *) malloc (dset->num_points * sizeof(struct point));

	for (i = 0; i < dset->num_points; i++) {
		dset->points[i].values = (float *) malloc( dset->num_attrs * sizeof(float) );

		fgets(buffer, BUFFER_SIZE, file);

		dset->points[i].values[0] = atof(strtok(buffer, " "));
		for (j = 1; j < dset->num_attrs; j++) {
			dset->points[i].values[j] = atof(strtok(NULL, " "));
		}
		dset->points[i].label = atoi(strtok(NULL, " "));
	}

	fclose(file);

	return dset;
}


void free_dataset(struct dataset * dset) {
	int i;
	#pragma omp parallel for
	for (i = 0; i < dset->num_points; i++) {
		free(dset->points[i].values);
	}
	free(dset->points);
	free(dset);
}


int knn(struct dataset *train, struct point *p, int K) {

	int i, hit = 0;
	float dists[train->num_points];
	int classes[train->num_class];

	#pragma omp parallel for
	for (i = 0; i < train->num_points; i++) {
		float d = euclidean_dist(train->points[i], *p, train->num_attrs);
		dists[i] = d;
	}

	//pre-sort the points putting K smallest items on the beginning
	get_k_small(train->points, dists, train->num_points, train->num_attrs, K);

	for (i = 0; i < train->num_class; i++) {
		classes[i] = 0;
	}

	for (i = 0; i < K; i++) {
		classes[train->points[i].label]++;
	}

	int max = 0;
	printf("%d ", classes[max]);
	for (i = 1; i < train->num_class; i++) {
		if (classes[i] > classes[max]) {
			max = i;
		}
		printf("%d ", classes[i]);
	}
	printf(" | %d : %d ", max, p->label);

	for (i = 0; i < train->num_class; i++) {
		if (i != max && classes[i] == classes[max]) {
			printf("rej\n");
			//rejeita
			return -1;
		}
	}
	printf("\n");

	return max;// == p->label;
}


void get_k_small(struct point * points, float dists[], int num_points, int num_attrs, int K) {
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


void get_confusion_matrix(struct dataset * test, int * pred) {
	int i, j;
	int matrix[test->num_class][test->num_class];

	for (i = 0; i < test->num_class; i++) {
		for (j = 0; j < test->num_class; j++) {
			matrix[i][j] = 0;
		}
	}

	for (i = 0; i < test->num_points; i++) {
		int e = test->points[i].label;
		int p = pred[i];
 		matrix[e][p]++;
	}

	for (i = 0; i < test->num_class; i++) {
		for (j = 0; j < test->num_class; j++) {
			printf("%5d ", matrix[i][j]);
		}
		printf("\n");
	}
}
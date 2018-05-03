#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <math.h>

#include <omp.h>

#define NAMEBUFFER 100

struct point {
	float *values;
	int label;
};

struct dataset {
	int num_points;
	int num_attrs;
	int num_class;
	struct point * points;
};


struct dataset * read_dataset(char * file_name);

void free_dataset(struct dataset * dset);

float euclidean_dist(struct point pA, struct point pB, int num_attrs);

int knn(struct dataset *train, struct point *p, int K);

void copy_point(struct point * src, struct point * dst, int num);

void get_k_small(struct point * points, float dists[], int num, int K);


int main(int argc, char *argv[]) {

	int i, j;
	char train_file[NAMEBUFFER];
	char test_file[NAMEBUFFER];
	int K;

	struct dataset *train, *test;

	if (argc > 3) {
		strcpy(train_file, argv[1]);
		strcpy(test_file, argv[2]);
		K = atoi(argv[3]);
	} else {
		printf("Wrong number of parameters.\nExiting.\n");
		exit(0);
	}

	train = read_dataset(train_file);
	test = read_dataset(test_file);

	//#pragma omp parallel for
	//test->num_points
	test->num_points = 10;

	int hit = 0;
	float d = 0.0;
	for (i = 0; i < test->num_points; i++) {
		//if (i % 1000 == 0) printf("%d\n", i);
		
		for (j = 0; j < train->num_points; j++) {
			d += euclidean_dist(train->points[j], test->points[i], test->num_attrs);
		}
		//hit += knn(train, &test->points[i], K);
	}
	printf("%f\n", d);
	//printf("acc: %f\n", (float)hit/1000);

	free_dataset(train);
	free_dataset(test);

	return 0;
}


struct dataset * read_dataset(char * file_name) {

	FILE *file;

	int BUFFER_SIZE = 10000;
	char buffer[BUFFER_SIZE];
	char **lines_buffer;

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

	printf("%d | %d | %d\n", dset->num_points, dset->num_attrs, dset->num_class);

	dset->points = (struct point *) malloc (dset->num_points * sizeof(struct point));

	lines_buffer = (char **) malloc(dset->num_points * sizeof(char*));
	for (i = 0; i < dset->num_points; i++) {
		dset->points[i].values = (float *) malloc( dset->num_attrs * sizeof(float) );
		lines_buffer[i] = (char *) malloc(BUFFER_SIZE * sizeof(char));
		fgets(lines_buffer[i], BUFFER_SIZE, file);
	}

	// ler todas as linhas pra memoria
	//#pragma omp parallel for private(j)
	for (i = 0; i < dset->num_points; i++) {

		dset->points[i].values[0] = atof(strtok(lines_buffer[i], " "));

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


float euclidean_dist(struct point pA, struct point pB, int num_attrs) {
	
	int i;
	float sum = 0.0;
	float diffs[num_attrs];

	#pragma omp reduction (+:sum)
	for (i = 0; i < num_attrs; i++) {
		sum += (pA.values[i] - pB.values[i]) * (pA.values[i] - pB.values[i]);
	}

	return sqrt(sum);
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
	get_k_small(train->points, dists, train->num_points, K);

	#pragma omp parallel for
	for (i = 0; i < train->num_class; i++) {
		classes[i] = 0;
	}

	for (i = 0; i < K; i++) {
		classes[train->points[i].label]++;
	}

	int max = 0;
	for (i = 1; i < train->num_class; i++) {
		if (classes[i] > classes[max]) {
			max = i;
		}
	}

	//printf("%d | %d | %d\n", max, p->label, max == p->label);

	return max == p->label;
}


void copy_point(struct point * src, struct point * dst, int num) {
	int i;

	#pragma omp parallel for
	for (i = 0; i < num; i++) {
		dst->values[i] = src->values[i];
	}
	dst->label = src->label;
}


void get_k_small(struct point * points, float dists[], int num, int K) {
	int i, j;
	struct point *temp;
	float tempd;

	temp = (struct point *) malloc(sizeof(struct point));
	temp->values = (float *) malloc(4 * sizeof(float));

	for (i = 0; i < K; i++) {

		int min_index = i;
		float min_value = dists[i];

		for (j = i + 1; j < num; j++) {

			if (dists[j] < min_value) {

				min_index = j;
				min_value = dists[j];

				copy_point(&points[i], temp, 4);
				copy_point(&points[min_index], &points[i], 4);
				copy_point(temp, &points[min_index], 4);

				tempd = dists[i];
				dists[i] = dists[min_index];
				dists[min_index] = tempd;
			}
		}
	}

	free(temp->values);
	free(temp);
}


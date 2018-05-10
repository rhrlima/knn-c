//typedef struct point POINT;
struct point {
	float *values;
	int label;
};

struct point *create_point(float *values, int label);

void copy_point(struct point *src, struct point *dst, int size);

float euclidean_dist(struct point pA, struct point pB, int num_attrs);

void sort_per_axis(struct point *points, int num_points, int num_attrs, int axis);
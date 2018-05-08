//typedef struct point POINT;
struct point {
	float *values;
	int label;
};

struct point *create_point(float *values, int label);
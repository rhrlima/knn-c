CC = gcc

FLAGS = -lm

all:
	echo No OMP
	$(CC) knn.c -o knn $(FLAGS)

omp:
	echo "With OMP"
	$(CC) knn.c -o knn $(FLAGS) -fopenmp
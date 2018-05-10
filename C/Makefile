CC = gcc

FLAGS = -lm -fopenmp

all:
	echo No OMP
	$(CC) utils.c kdtree.c knn.c -o knn $(FLAGS)

kdtree:
	$(CC) kdtree.c utils.c test.c -o test $(FLAGS)
CC = gcc

FLAGS = -lm -fopenmp

all:
	echo No OMP
	$(CC) knn.c -o knn $(FLAGS)
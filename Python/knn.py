import sys
import math


DELIMITER = ' '


class Point:

	def __init__(self, values, label):
		self.values = values
		self.label = label


def read_dataset(file_name, max_lines, flag=False):
	data = []

	file = open(file_name)
	line = file.readline().replace('\n', '').split(DELIMITER)

	num_exp 	= int(line[0])
	num_attr 	= int(line[1])
	num_class 	= int(line[2])

	num_exp = max_lines if (max_lines != 0 and max_lines < num_exp) else num_exp	

	for i in range(num_exp):
		line = file.readline().replace('\n', '').split(DELIMITER)
		vet = [float(f) for f in line[:-1]]
		label = int(line[-1:][0])
		data.append(Point(vet, label))

	if flag:
		return data, num_class
	return data


def euclidean_distance(pA, pB):
	size = len(pA.values)
	aux = 0
	for i in range(size):
		diff = pA.values[i] - pB.values[i]
		aux += diff * diff
	return math.sqrt(aux)


def knn(dataset, new_point, K):

	dists = []
	for i, p in enumerate(dataset):
		temp = euclidean_distance(new_point, p)
		dists.append((i, temp))

	dists.sort(key=lambda tup: tup[1])

	if K == 1:
		return dataset[dists[0][0]].label

	classes = {}
	for d in dists[:K]:
		key = dataset[d[0]].label
		if key in classes:
			classes[key] += 1
		else:
			classes[key] = 1

	temp = sorted(classes.items(), key=lambda value : value[1], reverse=True)

	if len(temp) == 1:
		return temp[0][0]

	if temp[0][1] == temp[1][1]:
		return -1

	return temp[0][0]


def classify(train, test, K):

	hit = 0
	miss = 0
	rej = 0

	pred = []
	for i, te in enumerate(test):
		temp = knn(train, te, K)
		pred.append(temp)
		if temp == -1:
			rej += 1
		elif temp == te.label:
			hit += 1
		else:
			miss += 1

	print(hit, miss, rej, hit+miss+rej)
	print('acc', hit / len(test))
	print('err', miss / len(test))
	print('rej', rej / len(test))


if __name__ == '__main__':

	if len(sys.argv) < 4:
		print('knn.py <train file> <test file> <K> [<num train> <num test>]')
		exit()

	train_file = sys.argv[1]
	test_file = sys.argv[2]
	K = int(sys.argv[3])
	
	tr_max = 0
	te_max = 0

	if len(sys.argv) > 4:
		tr_max = int(sys.argv[4])
		te_max = int(sys.argv[5])

	train, num_class = read_dataset(train_file, tr_max, True)
	test = read_dataset(test_file, te_max)

	print(len(train), len(test))

	classify(train, test, K)
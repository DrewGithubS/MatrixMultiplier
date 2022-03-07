#include <cstring>
#include <stdexcept>
#include <pthread.h>
#include <cstdio>
#include <random>

#include "MatrixMult.hpp"

const int THREADS = 8;

Matrix::Matrix() {}

Matrix::Matrix(unsigned int widthIn, unsigned int heightIn) {
	width = widthIn;
	height = heightIn;
	size = width * height;
	values = new double[size];
	threadsToUse = 1;
	memset(values, 0, size * sizeof(double));
}

Matrix::Matrix(unsigned int widthIn, unsigned int heightIn, double * valuesIn) {
	width = widthIn;
	height = heightIn;
	size = width * height;
	values = new double[size];
	threadsToUse = 1;
	memcpy(values, valuesIn, size * sizeof(double));
}

Matrix::Matrix(unsigned int widthIn, unsigned int heightIn, double ** valuesIn) {
	width = widthIn;
	height = heightIn;
	size = width * height;
	values = new double[size];
	threadsToUse = 1;
	memcpy(values, valuesIn, size * sizeof(double));
}

void Matrix::freeValues() {
	delete values;
}

double * Matrix::getValues() const {
	return values;
}

unsigned int Matrix::getWidth() const {
	return width;
}

unsigned int Matrix::getHeight() const {
	return height;
}

unsigned int Matrix::getSize() const {
	return size;
}

void Matrix::initializeToRandom() {
	double min = 0;
	double max = 10;
	std::uniform_real_distribution<double> unif(min, max);
	std::default_random_engine engine;
	for(int i = 0; i < size; i++) {
		values[i] = unif(engine);
	}
}

void Matrix::print() {
	double* value = values;
	for(int y = 0; y < height; y++) {
		printf("| ");
		for(int x = 0; x < width; x++) {
			if(x != 0) {
				printf(", ");
			}
			printf("%.2lf", *(value++));
		}
		printf(" |\n");
	}
}

struct Packet {
	const Matrix* first;
	const Matrix* second;
	Matrix* output;
	double** work;
    pthread_mutex_t * mutex;
    int threadID;
};

double* getWorkIndex(Packet * packet) {
	double* output;
	pthread_mutex_lock(packet->mutex);
	output = ((*packet->work)++);
	pthread_mutex_unlock(packet->mutex);
	double* lastPtr = (packet->output->getValues() + packet->output->getSize());
	return output < lastPtr ? (output) : NULL;
}

void* matrixMultiplyThreaded(void* data) {
	Packet packet = *((Packet*) data);
	const Matrix* first = packet.first;
	const Matrix* second = packet.second;
	Matrix* output = packet.output;
	double* currentWork;
	int x;
	int y;
	double* firstIter;
	double* secondIter;
	double* firstLast = first->getValues() + first->getSize();
	double* secondLast = second->getValues() + first->getSize();
	int firstSize = first->getSize();
	int secondSize = second->getSize();
	int index;
	while((currentWork = getWorkIndex(&packet)) != NULL) {
		index = currentWork - output->getValues();
		x = index % output->getWidth();
		y = index / output->getWidth();
		firstIter = first->getValues() + y * first->getWidth();
		secondIter = second->getValues() + x;
		for(;(firstIter < firstLast && secondIter < secondLast); firstIter++, secondIter += second->getWidth()) {
			*currentWork += (*firstIter) * (*secondIter);
		}
	}

	pthread_exit((void*) 0);
}

Matrix Matrix::operator* (const Matrix& second) {
	if(getWidth() != second.getHeight()) {
		throw std::length_error("Matrix width does not match second matrix's height.");
	}
	Matrix output = Matrix(getHeight(), second.getWidth());
	pthread_t * threads = new pthread_t[threadsToUse];

	void *status;

	pthread_attr_t attr;
	pthread_mutex_t mutex;

	pthread_mutex_init(&mutex, NULL);

    pthread_attr_init(&attr);
    pthread_attr_setdetachstate(&attr, PTHREAD_CREATE_JOINABLE);


    Packet * packet = new Packet[threadsToUse];

    double* outputIterator = output.getValues();

	for(int i = 0; i < threadsToUse; ++i) {
		packet[i] = {this, &second, &output, &outputIterator, &mutex, i};
		pthread_create(&threads[i], &attr, matrixMultiplyThreaded, (void*) &packet[i]);
	}

	pthread_attr_destroy(&attr);

    for(int i = 0; i < threadsToUse; i++) {
       pthread_join(threads[i], &status);
    }

    return output;
}
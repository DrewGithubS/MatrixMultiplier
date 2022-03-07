#include <cstdio>
#include <chrono>
#include <iostream>

#include "MatrixMult.hpp"

int main() {
	Matrix a = Matrix(1000, 1500);
	Matrix b = Matrix(1500, 1000);
	a.initializeToRandom();
	b.initializeToRandom();
	// a.print();
	printf("A printed\n");
	// b.print();
	printf("B printed\n");


	for(int i = 0; i < 9; i++) {
		auto start = std::chrono::high_resolution_clock::now();
		a.threadsToUse = i;
		volatile Matrix output = a * b;
		auto stop = std::chrono::high_resolution_clock::now();

		auto duration = std::chrono::duration_cast<std::chrono::microseconds>(stop - start);
	 
	    std::cout << "Time taken by function on " << i << " threads " 
	        << duration.count() << " microseconds" << std::endl;

	    // output.freeValues();
    }
	
	// output.print();
	printf("Output printed\n");
}
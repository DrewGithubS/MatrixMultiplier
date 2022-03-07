class Matrix {
private:
	double* values;
	unsigned int width;
	unsigned int height;
	unsigned int size;
public:
	int threadsToUse;
	Matrix();
	Matrix(unsigned int width, unsigned int height);
	Matrix(unsigned int width, unsigned int height, double* valuesIn);
	Matrix(unsigned int width, unsigned int height, double** valuesIn);

	void freeValues();
	void initializeToRandom();
	void print();

	unsigned int getWidth() const;
	unsigned int getHeight() const;
	unsigned int getSize() const;
	double* getValues() const;
	Matrix operator* (const Matrix& second);
};

#include <iostream>
#include <thread>
#include <immintrin.h>
#include <chrono>
#include <iomanip>
#include <immintrin.h> // SIMD
#include <cstring>
#include <mutex>
#include <condition_variable>

class Matrix {
    private:
        int n, m;

    public:
	int** M;
        Matrix(int a, int b);
        ~Matrix();
        void initialiseMatrix();
        void inputMatrix();
        void displayMatrix();
        int** T();
        Matrix* multiplyMatrix(Matrix* N);

        // helper function
        void set (int i, int j, int num) {
            M[i][j] = num;
        }

};

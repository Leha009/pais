#include <iostream>
#include <chrono>
#include <omp.h>
#include <immintrin.h>

#define THREAD_NUM 12

#define WARMUP_NUM 10
#define MEASURE_NUM 1000

#define M_ROWS 700
#define M_COLUMNS 500

#define RAND_LEFT -1000
#define RAND_RIGHT 1000

double* multiply_matrix_by_vector(double** matrix, int rows, int columns, double* vector);

int randomInt(int left, int right)
{
    return rand() % (right - left + 1) + left;
}

int main()
{
    std::srand(61771);
    double** matrix = new double*[M_ROWS];
    double* vector = new double[M_COLUMNS];

    for(int i = 0; i < M_ROWS; ++i)
    {
        matrix[i] = new double[M_COLUMNS];
        for(int j = 0; j < M_COLUMNS; ++j)
        {
            matrix[i][j] = (double)randomInt(RAND_LEFT, RAND_RIGHT);
        }
    }

    for(int i = 0; i < M_COLUMNS; ++i)
    {
        vector[i] = (double)randomInt(RAND_LEFT, RAND_RIGHT);
    }

    for(int i = 0; i < WARMUP_NUM; ++i)
        multiply_matrix_by_vector(matrix, M_ROWS, M_COLUMNS, vector);

    auto start = std::chrono::high_resolution_clock::now();
    for(int i = 0; i < MEASURE_NUM; ++i)
        multiply_matrix_by_vector(matrix, M_ROWS, M_COLUMNS, vector);
    auto stop = std::chrono::high_resolution_clock::now();

    auto duration = std::chrono::duration_cast<std::chrono::microseconds>(stop - start);
 
    std::cout << "Time taken by function: "
         << duration.count()/1e6 << " seconds" << std::endl;

    for(int i = 0; i < M_ROWS; ++i)
        delete matrix[i];

    delete [] matrix;
    delete [] vector;
    return 0;
}

double* multiply_matrix_by_vector(double** matrix, int rows, int columns, double* vector)
{
    double* m_result = new double[rows];
    #pragma omp parallel for num_threads(THREAD_NUM)
    for(int i = 0; i < rows; ++i)
    {
        __m256d t1, t2;
        for(int j = 0; j < columns-5; j += 4)
        {
            //t1 = _mm256_loadu_pd(&matrix[i][j]);
            t1 = _mm256_loadu_pd(&matrix[i][j]);
            t2 = _mm256_loadu_pd(&vector[j]);
            t2 = _mm256_mul_pd(t1, t2);
            m_result[i] += t2[0] + t2[1] + t2[2] + t2[3];
        }
        for(int j = std::max(columns-5, 0); j < columns; ++j)
            m_result[i] += matrix[i][j] * vector[j];
    }

    return m_result;
}
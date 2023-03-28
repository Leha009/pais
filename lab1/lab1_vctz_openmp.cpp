#include <iostream>
#include <ctime>
#include <chrono>
#include <omp.h>
#include <xmmintrin.h>

#define THREAD_NUM 12

#define likely(x) __builtin_expect(!!(x), 1)
#define unlikely(x) __builtin_expect(!!(x), 0)

#define WARMUP_NUM 10
#define MEASURE_NUM 100

#define ROWS_NUM 700
#define COLUMNS_NUM 500

#define RAND_LEFT -1000
#define RAND_RIGHT 1000

int randomInt(int left, int right)
{
    return rand() % (right - left + 1) + left;
}

double** solveGauss(double** matrix, int rows, int columns);
void sub_vector_from_vector2(double* vector1, double* vector2, int vectorSize, double multiplyBy);

int main()
{
    std::srand(61771);
    double** fMatrix = new double*[ROWS_NUM];
    if(fMatrix == NULL)
    {
        std::cout << "Failed to allocate memory for matrix!" << std::endl;
        return 0;
    }

    for(int i = 0; i < ROWS_NUM; ++i)
    {
        fMatrix[i] = new double[COLUMNS_NUM];
        if(fMatrix[i] == NULL)
        {
            for(int j = 0; j < i; ++j)
                delete fMatrix[j];

            delete fMatrix;

            std::cout << "Failed to allocate memory for row " << i << " in matrix!" << std::endl;
            return 0;
        }
    }
    //=========================== GEN MATRIX ============================//
    for(int i = 0; i < ROWS_NUM; ++i)
        for(int j = 0; j < COLUMNS_NUM; ++j)
            fMatrix[i][j] = (double)randomInt(RAND_LEFT, RAND_RIGHT);

    //=========================== SOLVE SYSTEM ==========================//
    for(int i = 0; i < WARMUP_NUM; ++i)
        solveGauss(fMatrix, ROWS_NUM, COLUMNS_NUM);

    auto start = std::chrono::high_resolution_clock::now();
    for(int i = 0; i < MEASURE_NUM; ++i)
        solveGauss(fMatrix, ROWS_NUM, COLUMNS_NUM);
    auto stop = std::chrono::high_resolution_clock::now();

    auto duration = std::chrono::duration_cast<std::chrono::microseconds>(stop - start);
 
    std::cout << "Time taken by function: "
         << duration.count()/1e6 << " seconds" << std::endl;
    //=========================== FREE MEMORY ===========================//
    for(int i = 0; i < ROWS_NUM; ++i)
        delete fMatrix[i];

    delete fMatrix;
    return 0;
}

/**
 * @brief решение СЛАУ методом Гаусса
 * 
 * @param matrix        изначальная матрица
 * @param rows          кол-во строк
 * @param columns       кол-во столбцов
 * @return double**      матрица решений
 */
double** solveGauss(double** matrix, int rows, int columns)
{
    double** solution = new double*[rows];
    #pragma omp parallel for num_threads(THREAD_NUM)
    for(int i = 0; i < rows; ++i)           // copy origin matrix
    {
        solution[i] = new double[columns];
        for(int j = 0; j < columns; ++j)
            solution[i][j] = matrix[i][j];
    }

    for(int i = 0; i < rows && i < columns-1; ++i) // проходим сверху вниз, слева направо
    {
        #pragma omp parallel for num_threads(THREAD_NUM)
        for(int j = i+1; j < rows; ++j) // проходим по всем строкам ниже
            sub_vector_from_vector2(solution[j], solution[i], columns, (solution[j][i]/solution[i][i]));
    }
    return solution;
}

/**
 * @brief Наивное вычитание векторов, домножив второе на значение. ВНИМАНИЕ! Изменяет vector1
 */
void sub_vector_from_vector2(double* vector1, double* vector2, int vectorSize, double multiplyBy)
{
    #pragma omp simd
    for(int i = 0; i < vectorSize; ++i)
        vector1[i] = vector1[i] - vector2[i] * multiplyBy;
}
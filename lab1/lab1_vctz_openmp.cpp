#include <iostream>
#include <ctime>
#include <chrono>
#include <omp.h>

#define THREAD_NUM 12

#define likely(x) __builtin_expect(!!(x), 1)
#define unlikely(x) __builtin_expect(!!(x), 0)

#define WARMUP_NUM 10
#define MEASURE_NUM 100

#define ROWS_NUM 700
#define COLUMNS_NUM 500

#define RAND_LEFT -1000
#define RAND_RIGHT 1000

double randomdouble()
{
    return (double)(rand()) / (double)(rand());
}

int randomInt(int left, int right)
{
    return rand() % (right - left + 1) + left;
}

void output_matrix(double** matrix, int rows, int columns)
{
    for(int i = 0; i < rows; ++i)
    {
        for(int j = 0; j < columns; ++j)
        {
            std::cout << matrix[i][j] << " ";
        }
        std::cout << std::endl;
    }
}


double** solveGauss(double** matrix, int rows, int columns);
void sub_vector_from_vector2(double* vector1, double* vector2, int vectorSize, double multiplyBy);
void swap_matrix_rows(double** matrix, int row1, int row2);

int main()
{
    //std::srand(time(NULL));
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
    for(int i = 0; i < rows; ++i)           // copy origin matrix
    {
        solution[i] = new double[columns];
        #pragma omp parallel for num_threads(THREAD_NUM)
        for(int j = 0; j < columns; ++j)
            solution[i][j] = matrix[i][j];
    }

    bool firstZero;
    int row = 0, column = 0;
    while(row < rows-1 && column < columns-1)
    {
        firstZero = false;
        //if(solution[row][column] == 0.0) // если на диагонали ноль - надо менять
        if(unlikely(solution[row][column] == 0.0)) // если на диагонали ноль - надо менять
        {
            firstZero = true;
            for(int j = row+1; firstZero && j < rows; ++j)
                if(solution[j][column] != 0.0)
                {
                    swap_matrix_rows(solution, row, j);
                    firstZero = false;
                }

            //if(firstZero)
            if(unlikely(firstZero))
            {
                ++column;
                continue;
            }
        }
        #pragma omp parallel for num_threads(THREAD_NUM)
        for(int j = row+1; j < rows; ++j) // проходим по всем строкам ниже
            sub_vector_from_vector2(solution[j], solution[row], columns, (solution[j][column]/solution[row][column]));
        ++row;
    }
    return solution;
}

/**
 * @brief Наивное вычитание векторов, домножив второе на значение. ВНИМАНИЕ! Изменяет vector1
 */
void sub_vector_from_vector2(double* vector1, double* vector2, int vectorSize, double multiplyBy)
{
    /*#pragma omp simd
    for(int i = 0; i < vectorSize; ++i)
    vector1[i] = vector1[i] - vector2[i] * multiplyBy;*/
    int i;
    #pragma omp simd
    for(i = 0; i < vectorSize-8; i += 8)
    {
        vector1[i] = vector1[i] - vector2[i] * multiplyBy;
        vector1[i+1] = vector1[i+1] - vector2[i+1] * multiplyBy;
        vector1[i+2] = vector1[i+2] - vector2[i+2] * multiplyBy;
        vector1[i+3] = vector1[i+3] - vector2[i+3] * multiplyBy;
        vector1[i+4] = vector1[i+4] - vector2[i+4] * multiplyBy;
        vector1[i+5] = vector1[i+5] - vector2[i+5] * multiplyBy;
        vector1[i+6] = vector1[i+6] - vector2[i+6] * multiplyBy;
        vector1[i+7] = vector1[i+7] - vector2[i+7] * multiplyBy;
    }
    for(;i < vectorSize; ++i)
        vector1[i] = vector1[i] - vector2[i] * multiplyBy;
}

void swap_matrix_rows(double** matrix, int row1, int row2)
{
    double* temp = matrix[row2];
    matrix[row2] = matrix[row1];
    matrix[row1] = temp;
}
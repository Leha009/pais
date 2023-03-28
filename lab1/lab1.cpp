#include <iostream>
#include <ctime>
#include <chrono>

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

void output_matrix(double** matrix, int rows, int columns);
double** solveGauss(double** matrix, int rows, int columns);
void get_vector_2_sub(double* vector, int vectorSize, double multiplyBy, double* vectorToStore);
void sub_vector_from_vector(double* vector1, double* vector2, int vectorSize);
void sub_vector_from_vector2(double* vector1, double* vector2, int vectorSize, double multiplyBy);
void multiply_vector_by_double(double* vector, int vectorSize, double multiplyBy);

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
    //output_matrix(fMatrix, ROWS_NUM, COLUMNS_NUM);
    //std::cout << std::endl;
    for(int i = 0; i < WARMUP_NUM; ++i)
        solveGauss(fMatrix, ROWS_NUM, COLUMNS_NUM);

    auto start = std::chrono::high_resolution_clock::now();
    //double** solution = solveGauss(fMatrix, ROWS_NUM, COLUMNS_NUM);
    for(int i = 0; i < MEASURE_NUM; ++i)
        solveGauss(fMatrix, ROWS_NUM, COLUMNS_NUM);
    auto stop = std::chrono::high_resolution_clock::now();

    auto duration = std::chrono::duration_cast<std::chrono::microseconds>(stop - start);
 
    std::cout << "Time taken by function: "
         << duration.count()/1e6 << " seconds" << std::endl;

    //output_matrix(solution, ROWS_NUM, COLUMNS_NUM);
    //=========================== FREE MEMORY ===========================//
    for(int i = 0; i < ROWS_NUM; ++i)
        delete fMatrix[i];

    delete fMatrix;
    return 0;
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
        for(int j = 0; j < columns; ++j)
            solution[i][j] = matrix[i][j];
    }

    for(int i = 0; i < rows && i < columns-1; ++i) // проходим сверху вниз, слева направо
    {
        //if(i == rows-1)
            //continue;

        // делаем так, чтобы первое не нулевое значение стало единицей
        //if(solution[i][i] != 1.0f) multiply_vector_by_double(solution[i], columns, 1.0f/solution[i][i]);

        for(int j = i+1; j < rows; ++j) // проходим по всем строкам ниже
        {
            //if(unlikely(solution[j][i] == 0.0)) continue;

            /*std::cout << "2SUB: \n";
            for(int j = 0; j < columns; ++j)
                std::cout << vectorToAdd[j] << ' ';
            std::cout << std::endl;*/

            sub_vector_from_vector2(solution[j], solution[i], columns, (solution[j][i]/solution[i][i]));


            //output_matrix(solution, rows, columns);
            //std::cout << i << ',' << j << ", diff: " << fDiff << std::endl;
        }
    }
    return solution;
}

/**
 * @brief Get the vector got from <vector> multiplied by some value
 * 
 * @param vector        origin vector
 * @param vectorSize    vector size
 * @param multiplyBy    value vector multiplied by
 * @param vectorToStore vector to store result
 */
void get_vector_2_sub(double* vector, int vectorSize, double multiplyBy, double* vectorToStore)
{
    for(int i = 0; i < vectorSize; ++i)
        vectorToStore[i] = vector[i] * multiplyBy;
}

/**
 * @brief Наивное вычитание векторов. ВНИМАНИЕ! Изменяет vector1
 * 
 * @param vector1       первый вектор (изменяемый)
 * @param vector2       второй вектор
 * @param vectorSize    размер векторов
 */
void sub_vector_from_vector(double* vector1, double* vector2, int vectorSize)
{
    for(int i = 0; i < vectorSize; ++i)
        vector1[i] = vector1[i] - vector2[i];
}

/**
 * @brief Наивное вычитание векторов, домножив второе на значение. ВНИМАНИЕ! Изменяет vector1
 */
void sub_vector_from_vector2(double* vector1, double* vector2, int vectorSize, double multiplyBy)
{
    /*for(int i = 0; i < vectorSize; ++i)
    vector1[i] = vector1[i] - vector2[i] * multiplyBy;*/
    int i;
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

/**
 * @brief Умножение вектора на число. ИЗМЕНЯЕТ ВЕКТОР!
 * 
 * @param vector            вектор
 * @param vectorSize        размер вектора
 * @param multiplyBy        число, на которое умножать
 */
void multiply_vector_by_double(double* vector, int vectorSize, double multiplyBy)
{
    for(int i = 0; i < vectorSize; ++i)
        vector[i] *= multiplyBy;
}
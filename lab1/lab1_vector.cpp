#include <iostream>
#include <ctime>
#include <chrono>
#include <vector>

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

void output_matrix(std::vector<std::vector<double>> matrix);
std::vector<std::vector<double>> solveGauss(std::vector<std::vector<double>> matrix);
void get_vector_2_sub(std::vector<double> vector, double multiplyBy, std::vector<double>* vectorToStore);
void sub_vector_from_vector(std::vector<double>* vector1, std::vector<double> vector2);
void sub_vector_from_vector2(std::vector<double>* vector1, std::vector<double> vector2, double multiplyBy);
void multiply_vector_by_double(std::vector<double>* vector, double multiplyBy);

int main()
{
    //std::srand(time(NULL));
    std::srand(61771);
    std::vector<std::vector<double>> fMatrix(ROWS_NUM, std::vector<double>(COLUMNS_NUM));

    //=========================== GEN MATRIX ============================//
    for(int i = 0; i < ROWS_NUM; ++i)
        for(int j = 0; j < COLUMNS_NUM; ++j)
            fMatrix[i][j] = (double)randomInt(RAND_LEFT, RAND_RIGHT);

    //=========================== SOLVE SYSTEM ==========================//
    //output_matrix(fMatrix);
    //std::cout << std::endl;
    for(int i = 0; i < WARMUP_NUM; ++i)
        solveGauss(fMatrix);

    std::vector<std::vector<double>> solution;
    auto start = std::chrono::high_resolution_clock::now();
    //double** solution = solveGauss(fMatrix, ROWS_NUM, COLUMNS_NUM);
    for(int i = 0; i < MEASURE_NUM; ++i)
        solution = solveGauss(fMatrix);
    auto stop = std::chrono::high_resolution_clock::now();

    auto duration = std::chrono::duration_cast<std::chrono::microseconds>(stop - start);
 
    std::cout << "Time taken by function: "
         << duration.count()/1e6 << " seconds" << std::endl;

    //output_matrix(solution);
    //=========================== FREE MEMORY ===========================//
    return 0;
}

void output_matrix(std::vector<std::vector<double>> matrix)
{
    int rows = matrix.size(),
        columns = matrix[0].size();
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
 */
std::vector<std::vector<double>> solveGauss(std::vector<std::vector<double>> matrix)
{
    int rows = matrix.size(),
        columns = matrix[0].size();

    std::vector<std::vector<double>> solution(rows, std::vector<double>(columns));
    for(int i = 0; i < rows; ++i)           // copy origin matrix
        for(int j = 0; j < columns; ++j)
            solution[i][j] = matrix[i][j];

    for(int i = 0; i < rows && i < columns-1; ++i) // проходим сверху вниз, слева направо
    {
        // делаем так, чтобы первое не нулевое значение стало единицей
        //if(solution[i][i] != 1.0f) multiply_vector_by_double(&solution[i], 1.0f/solution[i][i]);

        //#pragma omp parallel for num_threads(THREAD_NUM)
        for(int j = i+1; j < rows; ++j) // проходим по всем строкам ниже
        {
            //if(unlikely(solution[j][i] == 0.0)) continue;

            sub_vector_from_vector2(&solution[j], solution[i], (solution[j][i]/solution[i][i]));
        }
    }
    return solution;
}

/**
 * @brief Get the vector got from <vector> multiplied by some value
 */
void get_vector_2_sub(std::vector<double> vector, double multiplyBy, std::vector<double>* vectorToStore)
{
    int vectorSize = vector.size();
    for(int i = 0; i < vectorSize; ++i)
        vectorToStore->operator[](i) = vector[i] * multiplyBy;
}

/**
 * @brief Наивное вычитание векторов. ВНИМАНИЕ! Изменяет vector1
 */
void sub_vector_from_vector(std::vector<double>* vector1, std::vector<double> vector2)
{
    int vectorSize = vector1->size();
    for(int i = 0; i < vectorSize; ++i)
        vector1->operator[](i) = vector1->operator[](i) - vector2[i];
}

/**
 * @brief Наивное вычитание векторов, домножив второе на значение. ВНИМАНИЕ! Изменяет vector1
 */
void sub_vector_from_vector2(std::vector<double>* vector1, std::vector<double> vector2, double multiplyBy)
{
    int vectorSize = vector1->size();
    /*for(int i = 0; i < vectorSize; ++i)
        vector1->operator[](i) = vector1->operator[](i) - vector2[i] * multiplyBy;*/
    int i;
    for(i = 0; i < vectorSize-8; i += 8)
    {
        vector1->operator[](i) = vector1->operator[](i) - vector2[i] * multiplyBy;
        vector1->operator[](i+1) = vector1->operator[](i+1) - vector2[i+1] * multiplyBy;
        vector1->operator[](i+2) = vector1->operator[](i+2) - vector2[i+2] * multiplyBy;
        vector1->operator[](i+3) = vector1->operator[](i+3) - vector2[i+3] * multiplyBy;
        vector1->operator[](i+4) = vector1->operator[](i+4) - vector2[i+4] * multiplyBy;
        vector1->operator[](i+5) = vector1->operator[](i+5) - vector2[i+5] * multiplyBy;
        vector1->operator[](i+6) = vector1->operator[](i+6) - vector2[i+6] * multiplyBy;
        vector1->operator[](i+7) = vector1->operator[](i+7) - vector2[i+7] * multiplyBy;
    }
    for(;i < vectorSize; ++i)
        vector1->operator[](i) = vector1->operator[](i) - vector2[i] * multiplyBy;
}

/**
 * @brief Умножение вектора на число. ИЗМЕНЯЕТ ВЕКТОР!
 */
void multiply_vector_by_double(std::vector<double>* vector, double multiplyBy)
{
    int vectorSize = vector->size();
    for(int i = 0; i < vectorSize; ++i)
        vector->operator[](i) *= multiplyBy;
}
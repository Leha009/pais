#include <stdio.h>
#include <stdlib.h>
#include <time.h>

#define WARMUP_NUM 10
#define MEASURE_NUM 1000

#define M_ROWS 700
#define M_COLUMNS_V_ROWS 500

#define RAND_LEFT -1000
#define RAND_RIGHT 1000

double* multiply_matrix_by_vector(double** matrix, int rows, int columns, double* vector);

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
            printf("%lf ", matrix[i][j]);
        }
        printf("\n\n");
    }
}

void output_vector(double* vector, int columns)
{
    for(int j = 0; j < columns; ++j)
    {
        printf("%lf ", vector[j]);
    }
    printf("\n\n");
}

int main()
{
    srand(61771);
    double** matrix = malloc(sizeof(double*)*M_ROWS);
    double* vector = malloc(sizeof(double)*M_COLUMNS_V_ROWS);

    for(int i = 0; i < M_ROWS; ++i)
    {
        matrix[i] = malloc(sizeof(double)*M_COLUMNS_V_ROWS);
        for(int j = 0; j < M_COLUMNS_V_ROWS; ++j)
        {
            matrix[i][j] = (double)randomInt(RAND_LEFT, RAND_RIGHT);
        }
    }

    for(int i = 0; i < M_COLUMNS_V_ROWS; ++i)
    {
        vector[i] = (double)randomInt(RAND_LEFT, RAND_RIGHT);
    }

    /*output_matrix(matrix, M_ROWS, M_COLUMNS_V_ROWS);
    output_vector(vector, M_COLUMNS_V_ROWS);*/

    //double* result = multiply_matrix_by_vector(matrix, M_ROWS, M_COLUMNS_V_ROWS, vector);
    //output_vector(result, M_ROWS);

    for(int i = 0; i < WARMUP_NUM; ++i)
        multiply_matrix_by_vector(matrix, M_ROWS, M_COLUMNS_V_ROWS, vector);

    clock_t start = clock(), stop;
    for(int i = 0; i < MEASURE_NUM; ++i)
        multiply_matrix_by_vector(matrix, M_ROWS, M_COLUMNS_V_ROWS, vector);
    stop = clock();

    printf("%lf seconds\n", (stop-start)/(double)CLOCKS_PER_SEC);

    for(int i = 0; i < M_ROWS; ++i)
        free(matrix[i]);

    free(matrix);
    free(vector);
    //delete result;
    return 0;
}

double* multiply_matrix_by_vector(double** matrix, int rows, int columns, double* vector)
{
    double* m_result = malloc(sizeof(double)*M_ROWS);
    for(int i = 0; i < rows; ++i)
    {
        for(int j = 0; j < columns; ++j)
            m_result[i] += matrix[i][j] * vector[j];
    }

    return m_result;
}
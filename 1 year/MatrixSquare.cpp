// Написати функцію для обчислення матриці B=A^2, де для матриці А використано
// прямокутне зв'язне стисле індексне зберігання, а матриця B зберігається традиційним чином
// у вигляді двовимірного масиву.
#include <iostream>

struct Node
{
    int i;
    int j;
    double value;
    Node* nextRight;
    Node* nextDown;
};

struct Matrix
{
    int n; // кількість рядків
    int m; // кількість стовпчиків
    Node** row;
    Node** column;
};

double** MatrixSquare(Matrix* A)
{
    // створюємо двовимірну матрицю B та заповнюємо її нулями
    int n = A->n;
    int m = A->m;
    double** B = new double*[n];
    for (int i = 0; i < n; i++)
    {
        B[i] = new double[m];
        for (int j = 0; j < m; j++)
        {
            B[i][j] = 0;
        }
    }

    for (int i = 0; i < n; i++)
    {
        for (int j = 0; j < m; j++)
        {
            Node* curRow = A->row[i];
            Node* curCol = A->column[j];

            while (curRow && curCol)
            {
                if (curRow->j < curCol->i)
                {
                    curRow = curRow->nextRight;
                } 
                else if (curRow->j > curCol->i)
                {
                    curCol = curCol->nextDown;
                }
                else
                {
                    B[i][j] += curRow->value * curCol->value;
                    curRow = curRow->nextRight;
                    curCol = curCol->nextDown;
                }
            }
        }
    }
	
    return B;
}

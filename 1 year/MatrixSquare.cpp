// �������� ������� ��� ���������� ������� B=A^2, �� ��� ������� � �����������
// ���������� ��'���� ������ �������� ���������, � ������� B ���������� ����������� �����
// � ������ ����������� ������.
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
    int n; // ������� �����
    int m; // ������� ���������
    Node** row;
    Node** column;
};

double** MatrixSquare(Matrix* A)
{
    // ��������� ��������� ������� B �� ���������� �� ������
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

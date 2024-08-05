#include <iostream>
#include <string>
#include <vector>
#include <cmath>
#include "linear_regression.h"
using namespace std;

vector<vector<double>> transpose(vector<vector<double>> matrix)
{
    int rows = matrix.size();
    int cols = matrix[0].size();

    vector<vector<double>> result(cols, vector<double>(rows));
    // cout<<"enter"<<endl;
    for (int i = 0; i < rows; ++i)
    {
        for (int j = 0; j < cols; ++j)
        {
            result[j][i] = matrix[i][j];
        }
    }
    // cout<<"exit"<<endl;
    return result;
}
double determinant(vector<vector<double>> matrix)
{
    return matrix[0][0] * matrix[1][1] - matrix[0][1] * matrix[1][0];
}
vector<vector<double>> inverse(vector<vector<double>> matrix)
{
    int n = matrix.size();
    vector<vector<double>> result(n, vector<double>(n, 0));
    vector<vector<double>> augmented(n, vector<double>(2 * n));
    for (int i = 0; i < n; ++i)
    {
        for (int j = 0; j < n; ++j)
        {
            augmented[i][j] = matrix[i][j];
        }
        augmented[i][i + n] = 1;
    }
    // Perform row operations to get the identity matrix on the left side
    for (int i = 0; i < n; ++i)
    {
        // Divide each element of the current row by the diagonal element
        double divisor = augmented[i][i];
        for (int j = 0; j < 2 * n; ++j)
        {
            augmented[i][j] /= divisor;
        }

        // Subtract multiples of the current row from other rows to get zeros below the diagonal
        for (int k = 0; k < n; ++k)
        {
            if (k != i)
            {
                double factor = augmented[k][i];
                for (int j = 0; j < 2 * n; ++j)
                {
                    augmented[k][j] -= factor * augmented[i][j];
                }
            }
        }
    }
    // Extract the inverse from the augmented matrix
    for (int i = 0; i < n; ++i)
    {
        for (int j = 0; j < n; ++j)
        {
            result[i][j] = augmented[i][j + n];
        }
    }

    return result;
}
std::vector<std::vector<double>> multiply(const std::vector<std::vector<double>> &A, const std::vector<std::vector<double>> &B)
{
    int m = A.size();
    int n = A[0].size();
    int p = B[0].size();
    std::vector<std::vector<double>> result(m, std::vector<double>(p, 0));
    for (int i = 0; i < m; ++i)
    {
        for (int j = 0; j < p; ++j)
        {
            for (int k = 0; k < n; ++k)
            {
                result[i][j] += A[i][k] * B[k][j];
            }
        }
    }
    return result;
}
vector<double> solve_linear_system(vector<vector<double>> X, vector<double> Y)
{
    vector<vector<double>> Y_p(Y.size(), vector<double>(1, 0));
    for (int i = 0; i < Y.size(); i++)
    {
        Y_p[i][0] = Y[i];
    }
    vector<vector<double>> m1 = multiply(transpose(X), X);
    vector<vector<double>> i1 = inverse(m1);
    vector<vector<double>> m2 = multiply(i1, transpose(X));
    vector<vector<double>> mul = multiply(m2, Y_p);
    vector<double> beta(8, 0);
    for (int i = 0; i < mul.size(); i++)
    {
        beta[i] = mul[i][0];
    }
    return beta;
}
void linear_regression_model(vector<double> &paras, double p, int maxPos, vector<Node> &test_data, vector<transaction> &transactions)
{
    int pos = 0;
    for (int i = 1; i < test_data.size(); i++)
    {
        transaction trans;
        double predicted_close = paras[0] + paras[1] * test_data[i - 1].close +
                                 paras[2] * test_data[i - 1].open +
                                 paras[3] * test_data[i - 1].vwap +
                                 paras[4] * test_data[i - 1].low +
                                 paras[5] * test_data[i - 1].high +
                                 paras[6] * test_data[i - 1].trades +
                                 paras[7] * test_data[i].open;
        if (predicted_close >= test_data[i].close * (1 + p / 100) && pos < maxPos)
        {
            trans.direction = "BUY";
            trans.price = test_data[i].close;
            pos++;
        }
        else if (predicted_close <= test_data[i].close * (1 - p / 100) && pos > -1 * maxPos)
        {
            trans.direction = "SELL";
            trans.price = test_data[i].close;
            pos--;
        }
        trans.date = test_data[i].date;
        transactions.push_back(trans);
    }
    transaction trans;
    if (pos > 0)
    {
        trans.direction = "SQUARE_OF_SELL";
    }
    else if (pos < 0)
    {
        trans.direction = "SQUARE_OF_BUY";
    }
    trans.date = test_data.back().date;
    trans.price = test_data.back().close * (abs(pos));
    transactions.push_back(trans);
}
vector<double> learn_parameters(vector<Node> &data)
{
    vector<vector<double>> X(data.size() - 1, vector<double>(8, 1));
    vector<double> Y(data.size() - 1, 0);
    for (int i = 1; i < data.size(); i++)
    {
        X[i - 1] = {1, data[i].prev_close, data[i - 1].open, data[i - 1].vwap, data[i - 1].low, data[i - 1].high, data[i - 1].trades, data[i].open};
        Y[i - 1] = data[i].close;
    }
    vector<double> beta = solve_linear_system(X, Y);
    return beta;
}
void predict_lg(vector<transaction> &transactions, vector<Node> &test_data, int maxPos, double p, vector<Node> &train_data)
{
    vector<double> paras = learn_parameters(train_data);
    linear_regression_model(paras, p, maxPos, test_data, transactions);
}

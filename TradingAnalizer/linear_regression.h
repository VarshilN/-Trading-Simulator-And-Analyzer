#ifndef LINEAR_REGRESSION_H
#define LINEAR_REGRESSION_H
#include "Node.h"
#include "transaction.h"
#include <string>
#include <vector>
using namespace std;
vector<double> learn_parameters(const vector<Node> &train_data);
void linear_regression_model(vector<double> &paras, double p, int maxPos, vector<Node> &test_data, vector<transaction>&transactions);
void predict_lg(vector<transaction>&transactions, vector<Node> &test_data, int maxPos, double p, vector<Node> &train_data);
// Vector<double, Dynamic> learn_parameters_1(const vector<Node> &data);
#endif

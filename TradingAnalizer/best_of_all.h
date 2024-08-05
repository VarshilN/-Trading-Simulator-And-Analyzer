#ifndef BEST_OF_ALL_H
#define BEST_OF_ALL_H
#include "momentum_stratagy.h"
#include "linear_regression.h"
#include"cashFlow.h"
#include "file_operations.h"

void predict_best(vector<vector<Node>>&prices_sets, vector<Node> &prices_lg, vector<transaction> &transactions, const std::string &start_date, std::string filename);
#endif
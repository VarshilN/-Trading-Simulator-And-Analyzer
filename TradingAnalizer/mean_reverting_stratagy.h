#ifndef MEAN_REVERTING_H
#define MEAN_REVERTING_H
#include "Node.h"
#include "transaction.h"
#include <vector>
#include <string>
using namespace std;
void z_stratagy(vector<Node> &prices1, vector<Node> &prices2, vector<transaction> &transactions1, vector<transaction> &transaction2, int n, int th, int maxPos);
void sl_z_stratagy(vector<Node> &prices1, vector<Node> &prices2, vector<transaction> &transactions1, vector<transaction> &transactions2, int n, int slth, int th, int maxPos);
void predict_mean(int state, vector<Node> &prices1, vector<Node> &prices2, vector<transaction> &transactions1, vector<transaction> &transactions2, int n, int slth, int th, int maxPos);
#endif
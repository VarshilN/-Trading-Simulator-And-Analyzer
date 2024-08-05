#ifndef STRATEGY_OPERATIONS_H
#define STRATEGY_OPERATIONS_H

#include <vector>
#include <string>
#include "Node.h"
#include "cashFlow.h"
#include "transaction.h"
using namespace std;
void predict(int state, vector<Node> &data, vector<transaction> &transactions, int n, int x =-1, double p = -1, int max_hold_days = -1, double c1 = -1, double c2 = -1, double oversold_threshold = -1, double overbought_threshold = -1, double adx_thresold = -1);
void momentum_strategy_dma(int n, int maxPos, double p, vector<Node> &prices, vector<transaction> &transactions);
void momentum_stratagy_basic(int n, int maxPos, vector<Node> &prices, vector<transaction> &transactions);
void momentum_stratagy_dma_plus(const vector<Node> &prices, vector<transaction> &transactions, int n, double c1, double c2, double p, int maxHold, int maxPos);
void macd_stratagy(int maxPos, vector<Node> &prices, vector<transaction> &transactions);
void RSI_stratagy(int n, int maxPos, double os, double ob, vector<Node> &prices, vector<transaction> &transactions);
void adx_stratagy(int n, int maxPos, double adx_th, vector<Node> &prices, vector<transaction> &transactions);
#endif

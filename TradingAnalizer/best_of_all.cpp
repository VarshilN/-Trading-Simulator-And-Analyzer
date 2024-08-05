#include "best_of_all.h"
#include <iostream>
#include <string>
#include <cmath>
#include <vector>
#include<climits>
#include <omp.h>
using namespace std;

void predict_cal(vector<vector<Node>>&prices_sets, vector<Node> &prices_lg, vector<vector<transaction>>&transactions_set) {
    #pragma omp parallel for
    for (int i = 1; i <= 6; ++i) {
        if (i == 7) {
            predict_lg(transactions_set[6], prices_sets[0], 5, 5, prices_lg);
        } else {
            switch (i) {
                case 1:
                    predict(i, prices_sets[1], transactions_set[0], 7, 5); //BASIC
                    break;
                case 2:
                    predict(i, prices_sets[2], transactions_set[1], 50, 5, 2); //DMA
                    break;
                case 3:
                    predict(i, prices_sets[3], transactions_set[2], 14, 5, 5, 28, 2, 0.2); //DMA++
                    break;
                case 4:
                    predict(i, prices_sets[4], transactions_set[3], 0, 5); //MACD
                    break;
                case 5:
                    predict(i, prices_sets[3], transactions_set[4], 14, 5, -1, -1, -1, -1, 30, 70);//RSI
                    break;
                case 6:
                    predict(i, prices_sets[3], transactions_set[5], 14, 5, -1, -1, -1, -1, -1, -1, 25); //ADX
                    break;
                default:
                    break;
            }
        }
    }
}
void predict_best(vector<vector<Node>>&prices_sets,vector<Node>&prices_lg,vector<transaction>&transactions,const std::string &start_date,std::string filename)
{
    vector<vector<transaction>>transactions_set(7);
    prices_sets.resize(5);
    read_file(filename, prices_sets[0],start_date,1); //lg
    read_file(filename, prices_sets[1],start_date,7); //basic
    read_file(filename, prices_sets[2],start_date,50); //dma
    read_file(filename, prices_sets[3],start_date,14);  //dma++ rsi adx
    read_file(filename, prices_sets[4],start_date,0);  //macd
    predict_cal(prices_sets,prices_lg,transactions_set);
    double best=0;
    for( int i=0;i<7;i++)
    {
        double cur= calculatePnL(transactions_set[i]);
        if(best<cur)
        {
            transactions=transactions_set[i];
            best=cur;
        }
    }
    return;
}
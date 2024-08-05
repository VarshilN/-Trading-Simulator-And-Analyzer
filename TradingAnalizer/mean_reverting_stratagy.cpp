#include "mean_reverting_stratagy.h"
#include <iostream>
#include <string>
#include <cmath>
#include <vector>
#include "Node.h"
using namespace std;

double z_score(vector<Node> &window1, vector<Node> &window2, int n)
{
    // vector<double> spread;
    double sum = 0;
    double sqdf = 0;
    double rolling_std = 0;
    for (int i = 0; i < n; i++)
    {
        double sp = (window1[i].close - window2[i].close);
        sum += sp;
        sqdf += sp * sp;
    }
    double rolling_mean = sum / n;
    sqdf = sqdf / n - rolling_mean * rolling_mean;
    // double var = (sqdf/n) - (rolling_mean * rolling_mean);
    // cout << n << endl;
    rolling_std = sqrt(sqdf);
    // rolling_std = sqrt(var);
    double sp = (window1.back().close - window2.back().close);
    double z_sc = (sp - rolling_mean) / rolling_std;
    return z_sc;
}
void z_stratagy(vector<Node> &prices1, vector<Node> &prices2, vector<transaction> &transactions1, vector<transaction> &transactions2, int n, int th, int maxPos)
{
    vector<Node> window1(prices1.begin(), prices1.begin() + n);
    vector<Node> window2(prices2.begin(), prices2.begin() + n);
    int pos = 0;
    for (int i = n; i < prices1.size(); i++)
    {
        transaction trans1;
        transaction trans2;
        trans1.date = prices1[i].date;
        trans2.date = trans1.date;
        window1.erase(window1.begin());
        window1.push_back(prices1[i]);
        window2.erase(window2.begin());
        window2.push_back(prices2[i]);
        double z_sc = z_score(window1, window2, n);
        // cout << z_sc << endl;
        if (z_sc < -1 * th && pos < maxPos)
        {
            pos++;
            trans1.direction = "BUY";
            trans2.direction = "SELL";
            trans1.price = prices1[i].close;
            trans2.price = prices2[i].close;
        }
        else if (z_sc > th && pos > -1 * maxPos)
        {
            pos--;
            trans1.direction = "SELL";
            trans2.direction = "BUY";
            trans1.price = prices1[i].close;
            trans2.price = prices2[i].close;
        }
        transactions1.push_back(trans1);
        transactions2.push_back(trans2);
    }
    transaction trans1;
    transaction trans2;
    if (pos > 0)
    {
        trans1.direction = "SQUARE_OF_SELL";
        trans2.direction = "SQUARE_OF_BUY";
    }
    else if (pos < 0)
    {
        trans1.direction = "SQUARE_OF_BUY";
        trans2.direction = "SQUARE_OF_SELL";
    }
    // cout << pos << endl;
    trans1.price = prices1.back().close * (abs(pos));
    trans2.price = prices2.back().close * (abs(pos));
    trans1.date = prices1.back().date;
    trans2.date = trans1.date;
    transactions1.push_back(trans1);
    transactions2.push_back(trans2);
}
void sl_z_stratagy(vector<Node> &prices1, vector<Node> &prices2, vector<transaction> &transactions1, vector<transaction> &transactions2, int n, int slth, int th, int maxPos)
{
    // cout << "hi" << endl;
    vector<Node> window1(prices1.begin(), prices1.begin() + n);
    vector<Node> window2(prices2.begin(), prices2.begin() + n);

    int pos = 0;
    for (int i = n; i < prices1.size(); i++)
    {
        window1.erase(window1.begin());
        window1.push_back(prices1[i]);
        window2.erase(window2.begin());
        window2.push_back(prices2[i]);
        transaction trans1;
        transaction trans2;
        double z_sc = z_score(window1, window2, n);
        if (abs(z_sc) > abs(slth))
        {
            // cout << "enter" << endl;
            if (pos > 0)
            {
                pos--;
                trans1.direction = "SELL";
                trans2.direction = "BUY";
                trans1.price = prices1[i].close;
                trans2.price = prices2[i].close;
            }
            else
            {
                pos++;
                trans1.direction = "BUY";
                trans2.direction = "SELL";
                trans1.price = prices1[i].close;
                trans2.price = prices2[i].close;
            }
        }
        else if (z_sc > th && pos > -1 * maxPos)
        {
            pos--;
            trans1.direction = "SELL";
            trans2.direction = "BUY";
            trans1.price = prices1[i].close;
            trans2.price = prices2[i].close;
        }
        else if (z_sc < -1 * th && pos < maxPos)
        {
            pos++;
            trans1.direction = "BUY";
            trans2.direction = "SELL";
            trans1.price = prices1[i].close;
            trans2.price = prices2[i].close;
        }
        trans1.date = prices1[i].date;
        trans2.date = trans1.date;
        transactions1.push_back(trans1);
        transactions2.push_back(trans2);
    }
    transaction trans1;
    transaction trans2;
    if (pos > 0)
    {
        trans1.direction = "SQUARE_OF_SELL";
        trans2.direction = "SQUARE_OF_BUY";
    }
    else if (pos < 0)
    {
        trans1.direction = "SQUARE_OF_BUY";
        trans2.direction = "SQUARE_OF_SELL";
    }
    trans1.price = prices1.back().close * (abs(pos));
    trans2.price = prices2.back().close * (abs(pos));
    trans1.date = prices1.back().date;
    trans2.date = trans2.date;
    transactions1.push_back(trans1);
    transactions2.push_back(trans2);
}
void predict_mean(int state, vector<Node> &prices1, vector<Node> &prices2, vector<transaction> &transactions1, vector<transaction> &transactions2, int n, int slth, int th, int maxPos)
{
    if (state == 0)
    {
        z_stratagy(prices1, prices2, transactions1, transactions2, n, th, maxPos);
    }
    else
    {
        sl_z_stratagy(prices1, prices2, transactions1, transactions2, n, slth, th, maxPos);
    }
}
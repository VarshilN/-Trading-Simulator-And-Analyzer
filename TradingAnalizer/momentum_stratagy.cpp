#include "momentum_stratagy.h"
#include <climits>
#include <cmath>
#include <iostream>
#include <queue>
using namespace std;
// Function to check if a vector is monotonically increasing
bool inc_check(const vector<Node> &vec)
{
    size_t size = vec.size();
    for (size_t i = 0; i < size; ++i)
    {
        if (vec[i].close <= vec[i].prev_close)
        {
            return false;
        }
    }
    return true;
}

// Function to check if a vector is monotonically decreasing
bool dec_check(const vector<Node> &vec)
{
    size_t size = vec.size();
    for (size_t i = 0; i < size; ++i)
    {
        if (vec[i].close >= vec[i].prev_close)
        {
            return false;
        }
    }
    return true;
}

// Function to implement the basic momentum-based strategy
void momentum_stratagy_basic(int n, int maxPos, vector<Node> &prices, vector<transaction> &transactions)
{
    // vector<Node> prices(data.begin() + n, data.end());
    int size = prices.size();
    if (size < n)
    {
        return;
    }
    int pos = 0; // Current pos
    // 0-n-1 -> before start n- .. after start
    // st at n
    vector<Node> window(prices.begin(), prices.begin() + n); // window with [st-n,st) st is not in the window
    for (int i = n; i < size; i++)
    {
        window.erase(window.begin());
        window.push_back(prices[i]);
        transaction trans;
        if (inc_check(window))
        {
            if (pos < maxPos)
            {
                trans.price = prices[i].close;
                trans.direction = "BUY";
                ++pos;
            }
        }
        else if (dec_check(window))
        {
            if (pos > -1 * maxPos)
            {
                trans.price = prices[i].close;
                trans.direction = "SELL";
                --pos;
            }
        }
        trans.date = prices[i].date;
        transactions.push_back(trans);
    }
    // square of at last trading date
    transaction trans;
    if (pos > 0)
    {
        trans.direction = "SQUARE_OF_SELL";
    }
    else if (pos < 0)
    {
        trans.direction = "SQUARE_OF_BUY";
    }
    trans.price = prices.back().close * (abs(pos));
    transactions.push_back(trans);
}

pair<double, double> mean_std(const vector<Node> &vec)
{
    double sum = 0.0;
    int n = vec.size();
    double sqdf = 0.0;

    for (const Node &value : vec)
    {
        double val = value.close;
        sum += val;
    }

    double mean = sum / n;
    for (const Node &value : vec)
    {
        double val = value.close;
        sqdf += (val - mean) * (val - mean);
    }
    double var = (sqdf / n);
    double std = sqrt(var);
    return make_pair(mean, std);
}
// last n days includes current day price
void momentum_strategy_dma(int n, int maxPos, double p, vector<Node> &prices, vector<transaction> &transactions)
{
    int size = prices.size();
    if (size < n)
    {
        return;
    }
    int pos = 0;                                             // Current pos
    vector<Node> window(prices.begin(), prices.begin() + n); // doesn't have st price
    for (int i = n; i < size; i++)
    {
        window.erase(window.begin());
        window.push_back(prices[i]);
        double dma = mean_std(window).first;
        double std = mean_std(window).second; // calculating including current price
        transaction trans;
        trans.date = prices[i].date;
        if (prices[i].close >= dma + p * std && pos < maxPos)
        {
            trans.direction = "BUY";
            trans.price = prices[i].close;
            pos++;
        }
        else if (prices[i].close + p * std <= dma && pos > (-1) * maxPos)
        {
            trans.direction = "SELL";
            trans.price = prices[i].close;
            pos--;
        }
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
    trans.price = prices.back().close * (abs(pos));
    transactions.push_back(trans);
}
double eff_ratio(const vector<Node> &window)
{
    double rsum = 0;
    for (int i = 0; i < window.size(); i++)
    {
        rsum += abs(window[i].close - window[i].prev_close);
    }
    if (rsum == 0)
    {
        return INT_MIN;
    }
    double er = abs(window[0].close - window.back().close) / rsum;
    return er;
}
void momentum_stratagy_dma_plus(const vector<Node> &prices, vector<transaction> &transactions, int n, double c1, double c2, double p, int maxHold, int maxPos)
{
    vector<double> ama;
    vector<double> sft;
    int pos = 0;
    queue<myStock> Bucket;
    vector<Node> window(prices.begin(), prices.begin() + n);
    for (int i = n; i < prices.size(); i++)
    {
        window.erase(window.begin());
        window.push_back(prices[i]);
        transaction trans;
        if (!Bucket.empty())
        {
            int q_size = Bucket.size();
            for (int i = 0; i < q_size; ++i)
            {
                myStock head = Bucket.front();
                Bucket.pop();
                head.hold += 1;
                Bucket.push(head);
            }
        }
        double c = eff_ratio(window);
        if (c == INT_MIN)
        {
            continue; // skiping this days calculation
        }
        double x = 2 * c / (1 + c2);
        if (i == n)
        {
            sft.push_back(0.5);
            ama.push_back(prices[n].close);
        }
        else
        {
            sft.push_back((1 - c1) * sft.back() + c1 * ((x - 1) / (x + 1)));
            ama.push_back((1 - sft.back()) * ama.back() + sft.back() * prices[i].close);
        }
        if ((!Bucket.empty()) && (Bucket.front().hold >= maxHold))
        {
            if (Bucket.front().type == "stock")
            {
                trans.direction = "SELL";
                trans.price = prices[i].close;
                pos--;
            }
            else if (Bucket.front().type == "short")
            {
                trans.direction = "BUY";
                trans.price = prices[i].close;
                pos++;
            }
            Bucket.pop();
        }
        else if (prices[i].close > ama.back() * (1 + p / 100) && pos < maxPos)
        {
            trans.direction = "BUY";
            trans.price = prices[i].close;
            pos++;
            if (!Bucket.empty())
            {
                if (Bucket.front().type == "short")
                {
                    Bucket.pop();
                }
                else
                {
                    myStock Stck;
                    Stck.type = "stock";
                    Bucket.push(Stck);
                }
            }
            else
            {
                myStock Stck;
                Stck.type = "stock";
                Bucket.push(Stck);
            }
        }
        else if (prices[i].close < ama.back() * (1 - p / 100) && pos > (-1) * maxPos)
        {
            trans.direction = "SELL";
            trans.price = prices[i].close;
            pos--;
            if (!Bucket.empty())
            {
                if (Bucket.front().type == "stock")
                {
                    Bucket.pop();
                }
                else
                {
                    myStock Stck;
                    Stck.type = "short";
                    Bucket.push(Stck);
                }
            }
            else
            {
                myStock Stck;
                Stck.type = "short";
                Bucket.push(Stck);
            }
        }
        trans.date = prices[i].date;
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
    trans.price = prices.back().close * (abs(pos));
    transactions.push_back(trans);
}
void macd_stratagy(int maxPos, vector<Node> &prices, vector<transaction> &transactions)
{
    vector<double> ewm_12;
    vector<double> ewm_26;
    vector<double> ewm_macd;
    // ewm_macd.push_back(0);
    double macd = 0;
    int pos = 0;
    for (int i = 0; i < prices.size(); i++)
    {
        if (i == 0)
        {
            double p0 = prices[0].close;
            ewm_12.push_back(p0);
            ewm_26.push_back(p0);
            ewm_macd.push_back(0);
        }
        else
        {
            ewm_12.push_back(ewm_12.back() * (1 - 2.0 / 13) + (2.0 / 13) * prices[i].close);
            ewm_26.push_back(ewm_26.back() * (1 - 2.0 / 27) + (2.0 / 27) * prices[i].close);
            macd = ewm_12.back() - ewm_26.back();
            ewm_macd.push_back(ewm_macd.back() * (1 - 2.0 / 10) + (2.0 / 10) * macd);
        }
        transaction trans;
        if (macd > ewm_macd.back() && pos < maxPos)
        {
            trans.price = prices[i].close;
            trans.direction = "BUY";
            pos++;
        }
        else if (macd < ewm_macd.back() && pos > (-1) * maxPos)
        {
            trans.price = prices[i].close;
            trans.direction = "SELL";
            pos--;
        }
        trans.date = prices[i].date;
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
    trans.price = prices.back().close * (abs(pos));
    transactions.push_back(trans);
}
double rsi(const vector<Node> &window, int n)
{
    double psum = 0;
    double nsum = 0;
    for (int i = 0; i < n; i++)
    {
        double cur = window[i].close - window[i].prev_close;
        psum += cur > 0 ? cur : 0;
        nsum += cur < 0 ? cur : 0;
    }
    return 100 * (1 - 1 / (-1 * (psum / nsum) + 1));
}
void RSI_stratagy(int n, int maxPos, double os, double ob, vector<Node> &prices, vector<transaction> &transactions)
{
    vector<Node> window(prices.begin(), prices.begin() + n);
    int pos = 0;
    for (int i = n; i < prices.size(); i++)
    {
        transaction trans;
        window.erase(window.begin());
        window.push_back(prices[i]);
        double ri = rsi(window, n);
        if (ri < os && pos < maxPos)
        {
            trans.price = prices[i].close;
            trans.direction = "BUY";
            pos++;
        }
        else if (ri > ob && pos > -1 * maxPos)
        {
            trans.price = prices[i].close;
            trans.direction = "SELL";
            pos--;
        }
        trans.date = prices[i].date;
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
    trans.price = prices.back().close * (abs(pos));
    transactions.push_back(trans);
    return;
}
double true_range(Node p)
{
    double max_ = p.high - p.low;
    max_ = max_ > (p.high - p.prev_close) ? max_ : (p.high - p.prev_close);
    max_ = max_ > (p.low - p.prev_close) ? max_ : (p.low - p.prev_close);
    return max_;
}
void adx_stratagy(int n, int maxPos, double adx_th, vector<Node> &prices, vector<transaction> &transactions)
{
    // cout << prices[n].date << endl;
    vector<Node> window(prices.begin(), prices.begin() + n);
    double alpha = 2.0 / (n + 1);
    vector<double> atr;
    vector<double> di_plus;
    vector<double> di_minus;
    vector<double> adx;
    int pos = 0;
    for (int i = n; i < prices.size(); i++)
    {
        window.erase(window.begin());
        window.push_back(prices[i]);
        transaction trans;
        double cur1 = prices[i].high - prices[i - 1].high;
        double cur2 = prices[i].low - prices[i - 1].low;
        double dm_p = cur1 > 0 ? cur1 : 0;
        double dm_n = cur2 > 0 ? cur2 : 0;
        double dx = 0;
        if (i == n)
        {
            atr.push_back(true_range(prices[i]));
            di_plus.push_back(dm_p / atr[0]);
            di_minus.push_back(dm_n / atr[0]);
        }
        else
        {
            double tr = true_range(prices[i]);
            atr.push_back(atr.back() * (1 - alpha) + alpha * (tr));
            di_plus.push_back(di_plus.back() * (1 - alpha) + alpha * (dm_p / atr.back()));
            di_minus.push_back(di_minus.back() * (1 - alpha) + alpha * (dm_n / atr.back()));
        }
        if (di_plus.back() + di_minus.back() == 0)
        {
            dx = 0;
            if (i != n)
            {
                adx.push_back(adx.back() * (1 - alpha) + alpha * (dx));
            }
            else
            {
                adx.push_back(dx);
            }
            continue;
        }
        else
        {
            dx = ((di_plus.back() - di_minus.back()) / (di_plus.back() + di_minus.back())) * 100;
            if (i != n)
            {
                adx.push_back(adx.back() * (1 - alpha) + alpha * (dx));
            }
            else
            {
                adx.push_back(dx);
            }
        }
        if (adx.back() > adx_th && pos < maxPos)
        {
            trans.direction = "BUY";
            trans.price = prices[i].close;
            pos++;
        }
        else if (adx.back() < adx_th && pos > (-1) * maxPos)
        {
            trans.direction = "SELL";
            trans.price = prices[i].close;
            pos--;
        }
        trans.date = prices[i].date;
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
    trans.price = prices.back().close * (abs(pos));
    transactions.push_back(trans);
    return;
}
void predict(int state, vector<Node> &data, vector<transaction> &transactions, int n, int x, double p, int max_hold_days, double c1, double c2, double oversold_threshold, double overbought_threshold, double adx_thresold)
{
    if (state == 1)
    {
        momentum_stratagy_basic(n, x, data, transactions);
    }
    else if (state == 2)
    {
        momentum_strategy_dma(n, x, p, data, transactions);
    }
    else if (state == 3)
    {
        momentum_stratagy_dma_plus(data, transactions, n, c1, c2, p, max_hold_days, x);
    }
    else if (state == 4)
    {
        macd_stratagy(x, data, transactions);
    }
    else if (state == 5)
    {
        RSI_stratagy(n, x, oversold_threshold, overbought_threshold, data, transactions);
    }
    else if (state == 6)
    {
        adx_stratagy(n, x, adx_thresold, data, transactions);
    }
}
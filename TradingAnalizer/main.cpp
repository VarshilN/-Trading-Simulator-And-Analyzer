#include <iostream>
#include "momentum_stratagy.h"
#include "linear_regression.h"
#include "mean_reverting_stratagy.h"
#include "best_of_all.h"
#include "file_operations.h"
#include "cashFlow.h"
#include <ctime>
#include <sstream>
#include <iomanip>
using namespace std;
string oneYearBefore(const string &dateStr)
{
    tm t = {};
    istringstream ss(dateStr);
    ss >> get_time(&t, "%d/%m/%Y");

    if (ss.fail())
    {
        cerr << "Error: Invalid date format!" << std::endl;
        return "";
    }

    // Adjust the year
    t.tm_year -= 1;

    ostringstream oss;
    oss << put_time(&t, "%d/%m/%Y");
    return oss.str();
}
int main(int argc, char *argv[])
{
    // cout << argc << endl;
    if (argc < 2)
    {
        cerr << "Usage: " << argv[0] << " <strategy> [parameters...]" << endl;
        return 1;
    }

    int n = -1, x, max_hold_days;
    double p, c1, c2, oversold_threshold, overbought_threshold, adx_threshold, threshold, sl_threshold;
    string sym, st, end, sym1, sym2, train_start_date, train_end_date;
    vector<Node> data;
    vector<Node> data1;
    vector<Node> data2;
    vector<Node> train_data;
    vector<transaction> transactions;
    vector<transaction> transactions1;
    vector<transaction> transactions2;
    string stratagy = argv[1];
    // cin >> stratagy;
    int state;
    if (stratagy == "BASIC")
    {
        if (argc != 7)
        {
            cerr << "Usage: " << argv[0] << " BASIC <n> <x> <sym> <start_date> <end_date>" << endl;
            return 1;
        }
        sym = argv[2];
        n = stoi(argv[3]);
        x = stoi(argv[4]);
        st = argv[5];
        end = argv[6];
        state = 1;
    }
    else if (stratagy == "DMA")
    {
        // cout<<"hello"<<endl;
        n = stoi(argv[3]);
        x = stoi(argv[4]);
        p = stod(argv[5]);
        sym = argv[2];
        st = argv[6];
        end = argv[7];
        state = 2;
    }
    else if (stratagy == "DMA++")
    {
        sym = argv[2];
        x = stoi(argv[3]);
        p = stod(argv[4]);
        n = stoi(argv[5]);
        max_hold_days = stoi(argv[6]);
        c1 = stod(argv[7]);
        c2 = stod(argv[8]);
        st = argv[9];
        end = argv[10];

        state = 3;
    }
    else if (stratagy == "MACD")
    {
        sym = argv[2];
        x = stoi(argv[3]);
        st = argv[4];
        end = argv[5];
        state = 4;
        n = 0;
    }
    else if (stratagy == "RSI")
    {
        sym = argv[2];
        x = stoi(argv[3]);
        n = stoi(argv[4]);
        oversold_threshold = stoi(argv[5]);
        overbought_threshold = stoi(argv[6]);
        st = argv[7];
        end = argv[8];
        state = 5;
    }
    else if (stratagy == "ADX")
    {
        sym = argv[2];
        x = stoi(argv[3]);
        n = stoi(argv[4]);
        adx_threshold = stoi(argv[5]);
        st = argv[6];
        end = argv[7];
        state = 6;
    }
    else if (stratagy == "LINEAR_REGRESSION")
    {
        sym = argv[2];
        x = stoi(argv[3]);
        p = stoi(argv[4]);
        train_start_date = argv[5];
        train_end_date = argv[6];
        st = argv[7];
        end = argv[8];
        state = 7;
        n = 1;
    }
    else if (stratagy == "BEST_OF_ALL")
    {
        state = 8;
        sym = argv[2];
        st = argv[3];
        end = argv[4];
    }
    else if (stratagy == "PAIRS" && argc == 9)
    {
        sym1 = argv[2];
        sym2 = argv[3];
        x = stoi(argv[4]);
        n = stoi(argv[5]);
        threshold = stoi(argv[6]);
        st = argv[7];
        end = argv[8];
        state = 9;
    }
    else if (stratagy == "PAIRS" && argc == 10)
    {
        // cout << "hi" << endl;
        sym1 = argv[2];
        sym2 = argv[3];
        x = stoi(argv[4]);
        n = stoi(argv[5]);
        threshold = stoi(argv[6]);
        sl_threshold = stoi(argv[7]);
        st = argv[8];
        end = argv[9];
        state = 10;
    }
    else if (stratagy == "COMP")
    {
        state = 11;
    }
    if (state < 8)
    {
        create_file(sym, st, end, n, "output");
        read_file("output.csv", data, st, n);
    }
    if (state == 7 || state == 8)
    {
        if (state == 8)
        {
            train_start_date = oneYearBefore(st);
            train_end_date = oneYearBefore(end);
        }
        create_file(sym, train_start_date, train_end_date, 1, "train_output");
        read_file("train_output.csv", train_data, train_start_date, 1);
    }
    if (state == 9 || state == 10)
    {
        create_file(sym1, st, end, n, "output1");
        create_file(sym2, st, end, n, "output2");
        read_file("output1.csv", data1, st, n);
        read_file("output2.csv", data2, st, n);
    }
    if (state < 7)
    {
        predict(state, data, transactions, n, x, p, max_hold_days, c1, c2, oversold_threshold, overbought_threshold, adx_threshold);
    }
    else if (state == 7)
    {
        predict_lg(transactions, data, x, p, train_data);
    }
    else if (state == 8)
    {
        create_file(sym, st, end, 50, "output");
        vector<vector<Node>> data_sets;
        predict_best(data_sets, train_data, transactions, st, "output.csv");
    }
    else if (state == 9 || state == 10)
    {
        predict_mean(state - 9, data1, data2, transactions1, transactions2, n, sl_threshold, threshold, x);
    }
    if (state < 9)
    {
        writeCashFlow(transactions, "daily_cashflow.csv", "order_statistics.csv", "final_pnl.txt");
    }
    else if (state == 9 || state == 10)
    {
        writeCashFlow_mean(transactions1, "daily_cashflow.csv", "order_statistics_1.csv", "final_pnl.txt", "order_statistics_2.csv", transactions2);
    }
    return 0;
}

#include "cashFlow.h"
#include "transaction.h"
#include <fstream>
#include <iostream>
#include <iomanip>
#include <cmath>
using namespace std;
string convertDateFormat(const string &inputDate)
{
    // Assuming inputDate is always in the format "dd-mm-yyyy"
    string outputDate = inputDate;
    size_t pos = outputDate.find("-");
    while (pos != string::npos)
    {
        outputDate.replace(pos, 1, "/");
        pos = outputDate.find("-", pos + 1);
    }
    return outputDate;
}
void dailyCashflow(const vector<transaction> &transactions, vector<string> &dates, vector<double> &cashflow, vector<transaction> &order, transaction &square_of, double &res)
{
    double cur = 0.0;

    for (const auto &day : transactions)
    {
        if (day.direction == "SQUARE_OF_BUY" || day.direction == "SQUARE_OF_SELL")
        {
            square_of = day;
            if (day.direction == "SQUARE_OF_BUY")
            {
                cur -= day.price;
            }
            if (day.direction == "SQUARE_OF_SELL")
            {
                cur += day.price;
            }
            // what to do here?
        }
        else
        {
            if (day.direction == "BUY")
            {
                order.push_back(day);
                cur += (-1) * day.quantity * day.price;
            }
            else if (day.direction == "SELL")
            {
                order.push_back(day);
                cur += 1 * day.quantity * day.price;
            }
            cashflow.push_back(cur);
            dates.push_back(day.date);
        }
    }
    res = cur;
    return;
}
void dailyCashflow_mean(const vector<transaction> &transactions1, const vector<transaction> &transactions2, vector<string> &dates, vector<double> &cashflow, vector<transaction> &order1, vector<transaction> &order2, transaction &square_of1, transaction &square_of2, double &res)
{
    double cur = 0.0;
    for (int i = 0; i < transactions1.size(); i++)
    {
        transaction day1 = transactions1[i];
        transaction day2 = transactions2[i];

        if (day1.direction == "SQUARE_OF_BUY" || day1.direction == "SQUARE_OF_SELL")
        {
            cout << cur << endl;
            square_of1 = day1;
            square_of2 = day2;
            cout << day1.price << endl;
            cout << day2.price << endl;

            if (day1.direction == "SQUARE_OF_BUY")
            {
                cur -= day1.price;
                cur += day2.price;
            }
            if (day1.direction == "SQUARE_OF_SELL")
            {
                cur += day1.price;
                cur -= day2.price;
            }
            cout << cur << endl;
        }
        else
        {
            if (day1.direction == "BUY")
            {
                order1.push_back(day1);
                order2.push_back(day2);
                cur += (-1) * day1.quantity * day1.price;
                cur += day2.quantity * day2.price;
            }
            else if (day1.direction == "SELL")
            {
                order1.push_back(day1);
                order2.push_back(day2);
                cur += day1.quantity * day1.price;
                cur += -1 * day2.quantity * day2.price;
            }
            cashflow.push_back(cur);
            dates.push_back(day1.date);
        }
    }
    res = cur;
    return;
}
int calculatePnL(vector<transaction> &transactions)
{
    double cur = 0.0;
    // vector<string> dates;    vector<transaction> tempTransactions = transactions; // Make a copy
    for (const auto &day : transactions)
    {
        if (day.direction == "SQUARE_OF_BUY" || day.direction == "SQUARE_OF_SELL")
        {
            if (day.direction == "SQUARE_OF_BUY")
            {
                cur -= day.price;
            }
            if (day.direction == "SQUARE_OF_SELL")
            {
                cur += day.price;
            }
            // what to do here?
        }
        else
        {
            if (day.direction == "BUY")
            {

                cur += (-1) * day.quantity * day.price;
            }
            else if (day.direction == "SELL")
            {

                cur += 1 * day.quantity * day.price;
            }
        }
    }
    return cur;
}
void writeCashFlow(const vector<transaction> &transactions1, const string &filename1, const string &filename2, const string &filename3)
{
    ofstream file1(filename1);
    ofstream file2(filename2);
    ofstream file3(filename3);
    vector<transaction> order;
    vector<string> dates;
    transaction square_of;
    vector<double> cashflow;
    double res = 0;
    dailyCashflow(transactions1, dates, cashflow, order, square_of, res);
    file1 << "Date,Cashflow" << endl;
    file2 << "Date,Order_dir,Quantity,Price" << endl;

    for (int i = 0; i < dates.size(); i++)
    {
        string outputDate = convertDateFormat(dates[i]);
        file1 << outputDate << "," << fixed << setprecision(2) << cashflow[i] << endl;
    }
    for (int i = 0; i < order.size(); i++)
    {
        string outputDate = convertDateFormat(order[i].date);
        file2 << outputDate << "," << order[i].direction << "," << order[i].quantity << "," << fixed << setprecision(2) << order[i].price << endl;
    }
    file3 << res << endl;
    file1.close();
    file2.close();
    file3.close();
    return;
}
void writeCashFlow_mean(const vector<transaction> &transactions1, const string &filename1, const string &filename2, const string &filename3, const string &filename4, const vector<transaction> transactions2)
{
    ofstream file1(filename1);
    ofstream file2(filename2);
    ofstream file3(filename3);
    ofstream file4(filename4);
    vector<transaction> order1;
    vector<transaction> order2;
    transaction square_of1;
    transaction square_of2;
    vector<string> dates;
    vector<double> cashflow;
    double res = 0;
    dailyCashflow_mean(transactions1, transactions2, dates, cashflow, order1, order2, square_of1, square_of2, res);
    file1 << "Date,Cashflow" << endl;
    file2 << "Date,Order_dir,Quantity,Price" << endl;
    file4 << "Date,Order_dir,Quantity,Price" << endl;
    for (int i = 0; i < dates.size(); i++)
    {
        // std::cout << "Cashflow: " << std::fixed << std::setprecision(2) << cashflow[i] << std::endl;
        string outputDate = convertDateFormat(dates[i]);
        file1 << outputDate << "," << fixed << setprecision(2) << cashflow[i] << endl;
    }

    for (int i = 0; i < order1.size(); i++)
    {
        string outputDate = convertDateFormat(order1[i].date);
        file2 << outputDate << "," << order1[i].direction << "," << order1[i].quantity << "," << fixed << setprecision(2) << order1[i].price << endl;
        file4 << outputDate << "," << order2[i].direction << "," << order2[i].quantity << "," << fixed << setprecision(2) << order2[i].price << endl;
    }
    file3 << res << endl;
    file1.close();
    file2.close();
    file3.close();
    file4.close();
    return;
}
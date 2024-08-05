#ifndef NODE_H
#define NODE_H

#include <string>

struct Node
{
    std::string date;
    double open;
    double close;
    double prev_close;
    double ltp;
    double vwap;
    double high52w;
    double low52w;
    double volume;
    double high;
    double low;
    double value;
    double trades;
};
struct myStock
{
    std::string type; //"stock" or "short"
    int hold = 0;
};
#endif // NODE_H

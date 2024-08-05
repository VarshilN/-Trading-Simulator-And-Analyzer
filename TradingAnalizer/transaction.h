#ifndef TRANSACION_H
#define TRANSACION_H

#include <string>

struct transaction
{
    std::string date;
    std::string direction = ""; // BUY or SELL
    int quantity = 1;
    double price = 0;
};
#endif
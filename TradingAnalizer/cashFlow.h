#ifndef CASH_FLOW_OPERATIONS_H
#define CASH_FLOW_OPERATIONS_H

#include <vector>
#include <string>
#include "Node.h"
#include "transaction.h"
using namespace std;
void writeCashFlow(const std::vector<transaction> &transactions1, const std::string &filename1, const std::string &filename2, const std::string &filename3);
void writeCashFlow_mean(const std::vector<transaction> &transactions1, const std::string &filename1, const std::string &filename2, const std::string &filename3,const std::string&filename4,const std::vector<transaction>transaction2);
int calculatePnL(std::vector<transaction>&transactions);
#endif

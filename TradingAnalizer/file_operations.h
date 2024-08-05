#ifndef FILE_OPERATIONS_H
#define FILE_OPERATIONS_H

#include <string>
#include <vector>
#include "Node.h"

int create_file(const std::string &sym, const std::string &st, const std::string &end, int n, std::string filename);
// read_file for n days back data
void read_file(const std::string filename, std::vector<Node> &nodes, const std::string start_date,int n);

#endif // FILE_OPERATIONS_H

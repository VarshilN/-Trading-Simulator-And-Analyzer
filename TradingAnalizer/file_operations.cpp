#include "file_operations.h"
#include <fstream>
#include <sstream>
#include <iostream>
#include <vector>
#include <algorithm>
#include <iostream>
using namespace std;
int create_file(const std::string &sym, const std::string &st, const std::string &end, int n, std::string filename)
{
    // load a data from st to end
    string command = "python3";
    command += " ";
    command += "data_api.py";
    command += " ";
    command += sym;
    command += " ";
    command += st;
    command += " ";
    command += end;
    command += " ";
    command += std::to_string(n);
    command += " ";

    command += filename;
    system(command.c_str());
    return 0;
}

// read_file for momentum strategies
void read_file(string filename, vector<Node> &nodes, string start_date, int n)
{
    std::stringstream dateStream(start_date);
    std::string day, month, year;
    std::getline(dateStream, day, '/');
    std::getline(dateStream, month, '/');
    std::getline(dateStream, year, '/');
    int start = stoi(year + month + day);
    std::ifstream file(filename);
    if (!file.is_open())
    {
        std::cerr << "Error opening file!\n";
        return;
    }
    std::string line;
    int cnt = 0;
    while (std::getline(file, line))
    {
        std::vector<std::string> cells;
        std::stringstream lineStream(line);
        std::string cell;
        while (std::getline(lineStream, cell, ','))
        {
            cells.push_back(cell);
        }
        if (cnt == 0)
        {
            cnt = 1;
            continue;
        }
        Node node;
        std::stringstream dateStream(cells[1]);
        std::string year, month, day;
        std::getline(dateStream, year, '-');
        std::getline(dateStream, month, '-');
        std::getline(dateStream, day, '-');
        int node_date = stoi(year + month + day);

        // Rearrange components in DD/MM/YY format
        // cout << cells[1] << endl;
        if (node_date >= start)
        {
            node.date = day + "-" + month + "-" + year;
            node.open = std::stod(cells[3]);
            node.prev_close = std::stod(cells[6]);
            node.close = std::stod(cells[8]);
            node.high = std::stod(cells[4]);
            node.low = std::stod(cells[5]);
            node.ltp = std::stod(cells[7]);
            node.vwap = std::stod(cells[9]);
            node.high52w = std::stod(cells[10]);
            node.low52w = std::stod(cells[11]);
            node.volume = std::stoi(cells[12]);
            node.value = std::stod(cells[13]);
            node.trades = std::stoi(cells[14]);
            nodes.push_back(node);
        }
        else if (cnt <= n)
        {
            node.date = day + "-" + month + "-" + year;
            node.open = std::stod(cells[3]);
            node.prev_close = std::stod(cells[6]);
            node.close = std::stod(cells[8]);
            node.high = std::stod(cells[4]);
            node.low = std::stod(cells[5]);
            node.ltp = std::stod(cells[7]);
            node.vwap = std::stod(cells[9]);
            node.high52w = std::stod(cells[10]);
            node.low52w = std::stod(cells[11]);
            node.volume = std::stoi(cells[12]);
            node.value = std::stod(cells[13]);
            node.trades = std::stoi(cells[14]);
            nodes.push_back(node);
            cnt++;
        }
    }
    std::reverse(nodes.begin(), nodes.end());
    // cout << nodes[n].date << endl;
    file.close();
    return;
}

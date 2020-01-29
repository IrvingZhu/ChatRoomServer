#pragma once

#include <iostream>
#include <cstdlib>
#include <cstring>

std::string getNextKey(const std::string &srcString)
{
    // this function is used to id+1
    char id[32];
    memset(id, 0, strlen(id));

    strcpy(id, srcString.c_str());
    long long num_id = atoll(id);
    num_id += 1;                    // id+1

    char char_id[32];
    memset(char_id, 0, strlen(char_id));

    sprintf(char_id, "%lld", num_id);
    std::string result(char_id);

    return result;                  // id to string and return.
}

void print_returnInfo(std::string info)
{
    cout << "the operate result is : " << info << endl;
}
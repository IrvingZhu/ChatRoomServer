#pragma once

#include <iostream>
#include <cstdlib>
#include <cstring>

<<<<<<< HEAD
std::string getNextKey(const std::string &srcString)
=======
std::string getNextKey(std::string srcString)
>>>>>>> CRS_Project_Mod
{
    // this function is used to id+1
    char *id = new char[32];
    memset(id, 0, strlen(id));

    strcpy(id, srcString.c_str());
    long long num_id = atoll(id);
    num_id += 1;                    // id+1

    char *char_id = new char[32];
    memset(char_id, 0, strlen(char_id));

    sprintf(char_id, "%lld", num_id);
    std::string result(char_id);
    return result;                  // id to string and return.
}
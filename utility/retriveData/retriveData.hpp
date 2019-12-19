#pragma once

#include <iostream>
#include <string>
#include <vector>

using namespace std;

vector<string> retriveData(string src, int num)
{
    int i = 0, start = 0, end = 0;
    vector<string> res;
    if (num == 0)
    {
        return res;
    }
    if (num == 1)
    {
        res.push_back(src);
        return res;
    }
    for (i; i < num; i++)
    {
        auto posi = src.find(" ");
        end = posi;
        res.push_back(src.substr(start, end));
        src = src.substr(posi + 1);
        start = 0, end = 0;
    }
    return res;
}
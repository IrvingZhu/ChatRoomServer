#pragma once

#include <iostream>

void singalHandler(int signum)
{
    std::cout << "Interrupt immediately for signal: " << signum << "\n";
    exit(signum);
}
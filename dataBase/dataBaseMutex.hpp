#pragma once
#include <mutex>

#ifndef DBMTX_EXIST
    static std::mutex DBmtx;
    #define DBMTX_EXIST
#endif



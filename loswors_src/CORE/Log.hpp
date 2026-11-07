#pragma once
#include "raylib.h"

#include <string>
#include <vector>


enum LogType {L_INFO, L_OK, L_WARNING, L_ERROR};

class one_log {
private:
    std::string log;
    LogType type;

public:

    one_log(const std::string& txt, LogType ttp) : log(txt), type(ttp) {}

    one_log() : log(""), type(L_INFO) {}

    const char* getStr() const;

    Color getColor();
};



class Log {
private:
    // get data hh:mm:ss:msms
    static std::string getTime();

    static std::vector<one_log> Logs;

public:
    Log() = delete;
    

    static void add(const std::string& log, LogType type);

    static one_log GetLast();

    static one_log GetById(int id);
    static int GetSize();
};

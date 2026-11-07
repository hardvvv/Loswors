#include "Log.hpp"

#include <chrono>
//#include <sstream>
//#include <iomanip>
//#include <ctime>


#include "Const.hpp"
#include "../UI/Theme/Theme.hpp"

//                     _           
//      ___ _ _  ___  | |___  __ _ 
//     / _ \ ' \/ -_) | / _ \/ _` |
//     \___/_||_\___| |_\___/\__, |
// ------------------------- |___/ -----------------
const char* one_log::getStr() const {
    return log.c_str();
}
Color one_log::getColor() {
    Color ret = Theme::C::log_norm;
    if (type == L_OK) ret = Theme::C::log_ok;
    else if (type == L_WARNING) ret = Theme::C::log_warning;
    else if (type == L_ERROR) ret = Theme::C::log_error;
    return ret;
}


//      _              
//     | |   ___  __ _ 
//     | |__/ _ \/ _` |
//     |____\___/\__, |
// ------------- |___/ -----------------------------
std::string Log::getTime() {
    using namespace std::chrono;

    auto now = system_clock::now();
    auto ms = duration_cast<milliseconds>(now.time_since_epoch()) % 1000;
    std::time_t t = system_clock::to_time_t(now);

    std::tm tm_struct;
#if defined(_WIN32) || defined(_WIN64)
    localtime_s(&tm_struct, &t);   // Windows
#else
    localtime_r(&t, &tm_struct);   // POSIX (Linux, macOS)
#endif

    std::ostringstream oss;
    oss << std::setfill('0')
        << std::setw(2) << tm_struct.tm_hour << "h:"
        << std::setw(2) << tm_struct.tm_min << "m:"
        << std::setw(2) << tm_struct.tm_sec << "s:"
        << std::setw(3) << ms.count() << "ms| >> ";

    return oss.str();
}

std::vector<one_log> Log::Logs;

void Log::add(const std::string& log, LogType type)
{
    if (Logs.size() >= Const::MAX_LOG_COUNT)
    {
        Logs.erase(Logs.begin());
    }
    Logs.emplace_back(one_log(getTime() + log, type));
}
one_log Log::GetLast() {
    if (Logs.empty()) return one_log();
    return Logs.back();
}
one_log Log::GetById(int id) {
    if (id < 0 || id >= Logs.size()) {
        return one_log();
    }
    return Logs[id];
}
int Log::GetSize() {
    return Logs.size();
}

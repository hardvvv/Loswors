#include "timer.hpp"
#include <chrono>

timer::timer() {
	Start();
}

float timer::ns_2_s(int64_t time) {
	return time / 1'000'000'000.0;
}

int64_t timer::GetNow() {
	return std::chrono::duration_cast<std::chrono::nanoseconds>(
		std::chrono::steady_clock::now().time_since_epoch()
	).count();
}


void timer::Start() {
	start = GetNow();
};

float timer::GetTime() {
	return ns_2_s(GetNow() - start);
};

int64_t timer::GetTime_ns() {
	return GetNow() - start;
};

float timer::GetTimeReset() {
	int64_t now = GetNow();
	int64_t ret = now - start;
	start = now;
	return ns_2_s(ret);
};

int64_t timer::GetTime_ns_Reset() {
	int64_t now = GetNow();
	int64_t ret = now - start;
	start = now;
	return ret;
};


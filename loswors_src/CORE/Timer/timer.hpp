#pragma once
#include <cstdint>


/*
	@brief Simple Master Timer
	* master <chrono> user
*/
class timer {
private:

	int64_t start = 0;

public:
	/*
		@brief Create obj. timer
		* | START the timer
	*/
	timer();


	/*
		@brief Start timer / RESET timer
	*/
	void Start();

	/*
		@brief Get Sec. after last Start / RESET
		* NO reset the timer
	*/
	float GetTime();

	/*
		@brief Get Nanosec. after last Start / RESET
		* NO reset the timer
	*/
	int64_t GetTime_ns();

	/*
		@brief Get Sec. after last Start / RESET
		* & RESET the timer
	*/
	float GetTimeReset();

	/*
		@brief Get Nanosec. after last Start / RESET
		* & RESET the timer
	*/
	int64_t GetTime_ns_Reset();

	/*
		@brief Get Nanosec. since epoch | static inside engine-only func
	*/
	static int64_t GetNow();


	static float ns_2_s(int64_t time);

};
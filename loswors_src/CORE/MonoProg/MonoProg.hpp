#pragma once

class MonoProg {
public:
	MonoProg() = delete;
	
	/**
		@brief Set curent Process active (kill all other)
	*/
	static void _INIT_();
	
	/**
		@brief Check The MonoProg file (Call in loop After CORE init)

		Checks if needs to close & writes data for parsing between processes if need
		Or get path from prev. process

		@return true (need close curent app) / false (continue)
	*/
	static bool IsKillSelf();

};
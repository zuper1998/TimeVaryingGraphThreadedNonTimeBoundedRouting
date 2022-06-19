#pragma once
namespace DefValues {
	/// <summary>
	/// Max window in seconds
	/// </summary>
	static double maxWindow = 3600;
	static double EntangledRate = 3.51 * 1000; /* 3.51 kHZ*/
    static double targetWindow= 100;


#ifndef test
    static size_t queue_max_size = 30000;
    const bool isTest = false;
    const size_t maxSimPaths = 1000;
#endif
#ifdef test
    static size_t queue_max_size = 15000;
    const bool isTest = true;
    const size_t maxSimPaths = 10000;
#endif


}
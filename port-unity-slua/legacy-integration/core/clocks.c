﻿/*
** LuaProfiler
** Copyright Kepler Project 2005-2007 (http://www.keplerproject.org/luaprofiler)
** $Id: clocks.c,v 1.4 2007-08-22 19:23:53 carregal Exp $
*/

/*****************************************************************************
clocks.c:
   Module to register the time (seconds) between two events

Design:
   'lprofC_start_timer()' marks the first event
   'lprofC_get_seconds()' gives you the seconds elapsed since the timer
                          was started
*****************************************************************************/

#include <stdio.h>
#include "clocks.h"

#ifdef _MSC_VER
#include <time.h>
#else
#include <sys/time.h>
#endif
/*
   Here you can choose what time function you are going to use.
   These two defines ('TIMES' and 'CLOCK') correspond to the usage of
   functions times() and clock() respectively.
        Which one is better? It depends on your needs:
                TIMES - returns the clock ticks since the system was up
              (you may use it if you want to measure a system
              delay for a task, like time spent to get input from keyboard)
                CLOCK - returns the clock ticks dedicated to the program
                        (this should be prefered in a multithread system and is
              the default choice)

   note: I guess TIMES don't work for win32
*/

#ifdef TIMES
        #include <sys/times.h>
        static struct tms t;
        #define times(t) times(t)
#else /* ifdef CLOCK */
        #define times(t) clock()
#endif


void lprofC_start_timer(clock_t *time_marker) {
    *time_marker = times(&t);
}

static clock_t get_clocks(clock_t time_marker) {
    return times(&t) - time_marker;
}

float lprofC_get_seconds(clock_t time_marker) {
	clock_t clocks;
    clocks = get_clocks(time_marker);
    return (float)clocks / (float)CLOCKS_PER_SEC;
}


time_t lprofC_get_current() {
	return time(NULL);
}

LARGE_INTEGER nFreq;

void lprofC_start_timer2(LARGE_INTEGER *nBeginTime)
{
	// 跨平台支持 2016-08-10 lennon.c
#ifdef _MSC_VER
	QueryPerformanceFrequency(&nFreq);
	QueryPerformanceCounter(nBeginTime);
#else
	gettimeofday(nBeginTime, NULL);
#endif
	
}

double lprofC_get_seconds2(LARGE_INTEGER *nBeginTime)
{
	LARGE_INTEGER nEndTime;
	double time = 0.0;
	// 跨平台支持 2016-08-10 lennon.c
#ifdef _MSC_VER
	QueryPerformanceCounter(&nEndTime);
	time = ((double)(nEndTime.QuadPart - nBeginTime->QuadPart) / (double)nFreq.QuadPart) * 1000;
#else
	gettimeofday(&nEndTime, NULL);
	time = (double)(nEndTime.tv_sec * 1000 - nBeginTime->tv_sec * 1000) + (double)(nEndTime.tv_usec - nBeginTime->tv_usec) / 1000;
#endif
	return time;
}

double lprofC_get_millisecond(LARGE_INTEGER *nTime)
{
	double time = 0.0;
	// 跨平台支持 2016-08-10 lennon.c
#ifdef _MSC_VER
	time = ((double)(nTime->QuadPart) / (double)nFreq.QuadPart) * 1000;
#else
	//time = nTime->tv_usec / 1000;
	time = (double)(nTime->tv_sec * 1000) + (double)(nTime->tv_usec) / 1000;
#endif
	return time;
}

double lprofC_get_interval(LARGE_INTEGER *nBeginTime, LARGE_INTEGER *nEndTime)
{
	double time = 0.0;
	// 跨平台支持 2016-08-10 lennon.c
#ifdef _MSC_VER
	time = ((double)(nEndTime->QuadPart - nBeginTime->QuadPart) / (double)nFreq.QuadPart) * 1000;
#else
	time = (double)(nEndTime->tv_sec * 1000 - nBeginTime->tv_sec * 1000) + (double)(nEndTime->tv_usec - nBeginTime->tv_usec) / 1000;
#endif
	return time;
}

#ifndef _TIME_MANAGER
#define _TIME_MANAGER

#include <time.h>

class MyTime
{
	struct timespec ts;
	MyTime(timespec && obj)
	{
		ts = obj;
	}
public:
	MyTime()
	{
		now();
	}
	MyTime(time_t sec, long nsec)
	{
		ts.tv_sec = sec;
		ts.tv_nsec = nsec;
	}
	MyTime operator-(const MyTime & obj) const
	{
		MyTime res;

		const timespec & time1 = ts;
		const timespec & time2 = obj.ts;
		timespec result;
		if ((time1.tv_sec < time2.tv_sec) ||
				((time1.tv_sec == time2.tv_sec) &&
				 (time1.tv_nsec <= time2.tv_nsec))) {		/* TIME1 <= TIME2? */
			result.tv_sec = result.tv_nsec = 0 ;
		} else {						/* TIME1 > TIME2 */
			result.tv_sec = time1.tv_sec - time2.tv_sec ;
			if (time1.tv_nsec < time2.tv_nsec) {
				result.tv_nsec = time1.tv_nsec + 1000000000L - time2.tv_nsec ;
				result.tv_sec-- ;				/* Borrow a second. */
			} else {
				result.tv_nsec = time1.tv_nsec - time2.tv_nsec ;
			}
		}
		return MyTime(std::move(result));
	}
	MyTime & operator+=(const MyTime & obj)
	{
		ts.tv_nsec += obj.ts.tv_nsec;
		if(ts.tv_nsec >= 1000000000L)
		{
			ts.tv_nsec -= 1000000000L;
			ts.tv_sec ++;
		}
		ts.tv_sec += obj.ts.tv_sec;
		return *this;
	}
	void now()
	{
		clock_gettime(CLOCK_MONOTONIC, &ts);
	}
	void sleepuntil()
	{
		//TODO:handle signal
		clock_nanosleep(CLOCK_MONOTONIC, TIMER_ABSTIME, &ts, NULL);
	}
};

#endif //_TIME_MANAGER

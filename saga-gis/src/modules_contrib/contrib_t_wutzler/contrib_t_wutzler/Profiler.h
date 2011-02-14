
#if !defined(AFX_PROFILER1_H__B9E65F37_8974_4887_809E_968156F6E347__INCLUDED_)
#define AFX_PROFILER1_H__B9E65F37_8974_4887_809E_968156F6E347__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000


//avoid compile complaining for long names in stl
#pragma warning(disable:4786)


#include <string>
#include <map>
#include <windows.h>
#include <iomanip>

#define clock_t LONGLONG


using namespace std;

class CProfiler  
{
public:
	clock_t clock();
	// writes all information to sout
	void output( ostream sout );

	// returns the average execution time in ms (millisecons)
	double avgMs(string id);
	// returns the average execution time in clock ticks
	double avg(string id );

	// returns the overall execution time in ms
	double sumMs(string id);
	// returns the overall execution time in clock-ticks
	clock_t sum( string id);

	// returns the number of execution
	long cnt(string id);

	// converts clock_ticks to ms 
	double ticks2ms( double ticks );
	
	// resets all execution cnts and times to zero
	void resetAll();
	void reset(string id);

	// returns the stdDeviation of call to get the time
	double getStdDev(){ return stdError; }
	// returns the timLag of call to get the time in clock-ticks
	double getLag(){ return lag; }
	double getLagMs(){ return ticks2ms(lag); }

	// starts the timer for given id
	void start( string id );
	// stops the timer for given id, return the execution time since last start
	clock_t stop( string id );

	CProfiler();
	virtual ~CProfiler();

private:

	typedef struct{
		long cnt;
		clock_t sum;
		clock_t beginClock;
	} profileInfoT;

double lag;
double stdError;
clock_t	ticksPerSecond;

typedef map<string,profileInfoT> cntMapT;

cntMapT cntMap;
cntMapT::iterator itCnt;

};

#endif // !defined(AFX_PROFILER1_H__B9E65F37_8974_4887_809E_968156F6E347__INCLUDED_)

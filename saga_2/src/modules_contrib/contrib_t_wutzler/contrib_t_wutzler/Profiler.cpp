// Profiler1.cpp: implementation of the CProfiler class.
//
//////////////////////////////////////////////////////////////////////

#include "Profiler.h"
#include <math.h>

//////////////////////////////////////////////////////////////////////
// Construction/Destruction
//////////////////////////////////////////////////////////////////////



CProfiler::CProfiler(){

	long i;
	static const long COUNTS = 1000;
	clock_t dif, difSingle;

	LARGE_INTEGER li;
	QueryPerformanceFrequency( &li );
	ticksPerSecond = li.QuadPart;

    //now we need to find out how long it actually takes
    //to make the function calls, so we can subtract by that amount.
    dif = 0;
    for( i = 0; i < COUNTS; i++ ){
		start("test");
        dif += stop("test");
    }
	lag = dif / COUNTS;

	// calculate std-error of the calls
    dif = 0;
    for( i = 0; i < COUNTS; i++ ){
		start("test");
        difSingle = stop("test");
        dif += (difSingle-(lag/2) ) * (difSingle-(lag/2) );
    }
    stdError = sqrt(dif / COUNTS);
	resetAll();
}

CProfiler::~CProfiler(){}

void CProfiler::start(string id){
	clock_t beginClock = clock();
	itCnt = cntMap.find(id);
	if( itCnt == cntMap.end() ){
		profileInfoT info;
		info.cnt = 0;
		info.sum = 0;
		info.beginClock = beginClock;
		cntMap.insert( cntMap.end(), cntMapT::value_type(id, info) );
	}else{
		(*itCnt).second.beginClock = beginClock;
	}
}

clock_t CProfiler::stop(string id){
	itCnt = cntMap.find(id);
	clock_t endClock = clock();
	if( itCnt == cntMap.end() ){
		return 0;
	}else{
		clock_t dif = clock() - (*itCnt).second.beginClock;
		(*itCnt).second.cnt++;
		(*itCnt).second.sum+=dif;
		return dif;
	}
}

long CProfiler::cnt(string id){
	itCnt = cntMap.find(id);
	if( itCnt == cntMap.end() ){
		return 0;
	}else{
		return (*itCnt).second.cnt;
	}
	return 0;
}

double CProfiler::avg(string id){
	itCnt = cntMap.find(id);
	if( itCnt == cntMap.end() ){
		return 0;
	}else{
		if( (*itCnt).second.cnt > 0 ){
			double ret = ((*itCnt).second.sum - (*itCnt).second.cnt * lag) / (double) (*itCnt).second.cnt;
			return ret;
		}else
			return 0;
	}
}

clock_t CProfiler::sum(string id){
	itCnt = cntMap.find(id);
	if( itCnt == cntMap.end() ){
		return 0;
	}else{
		return (*itCnt).second.sum - (*itCnt).second.cnt * lag;
	}
}


void CProfiler::reset(string id){
	itCnt = cntMap.find(id);
	if( itCnt != cntMap.end() ){
		(*itCnt).second.sum = 0;
		(*itCnt).second.cnt = 0;
	}
}

void CProfiler::resetAll(){
	cntMap.clear();
}

double CProfiler::ticks2ms(double ticks){
	return (1000 * ticks) / ticksPerSecond;
}

double CProfiler::sumMs(string id){
	clock_t sumt = sum(id);
	double ret = ticks2ms( sumt );
	return ret;
}

double CProfiler::avgMs(string id){
	return ticks2ms( avg(id) );
}

void CProfiler::output(ostream sout){
	string key;
	sout << endl;
	for( itCnt=cntMap.begin(); itCnt != cntMap.end(); ++itCnt ){
		key = (*itCnt).first; 
		setprecision(2);
		sout << "cnt=" << cnt(key) << "\tavg=" << fixed << avgMs(key) << "\tsum=" << fixed << sumMs(key) << "\t" << key << endl;
	}
}

clock_t CProfiler::clock(){
	LARGE_INTEGER li;
	QueryPerformanceCounter( &li );
	return li.QuadPart;
}

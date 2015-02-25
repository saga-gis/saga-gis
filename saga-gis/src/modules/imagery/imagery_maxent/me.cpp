/////////////////////////////////////////////////////////////////////////
//Copyright (C) 2003 Dekang Lin, lindek@cs.ualberta.ca
//
//Permission to use, copy, modify, and distribute this software for any
//purpose is hereby granted without fee, provided that the above
//copyright notice appear in all copies and that both that copyright
//notice and this permission notice appear in supporting documentation.
//No representations about the suitability of this software for any
//purpose is made. It is provided "as is" without express or implied
//warranty.
//
/////////////////////////////////////////////////////////////////////////

#include <iostream>
#include <cmath>
#include <float.h>

#include "me.h"

/////////////////////////////////////////////////////////////////////////
#ifdef _SAGA_MSW
#define isinf(x) (!_finite(x))
#else
#define isinf(x) (!finite(x))
#endif

/** The input array contains a set of log probabilities lp1, lp2, lp3
    ... The return value should be the log of the sum of the
    probabilities: log(e^lp1 + e^lp2 + e^lp3 + ...) */
double sumLogProb(vector<double>& logprobs)
{
  double max = 0;
  unsigned int i;
  for (i = 0; i<logprobs.size(); i++) {
    if (i==0 || logprobs[i]>max)
      max = logprobs[i];
  }
  if (isinf(max)) // the largest probability is 0 (log prob= -inf)
    return max;   // return log 0
  double p = 0;
  for (i = 0; i<logprobs.size(); i++) {
    p += exp(logprobs[i]-max);
  }
  return max + log(p);
}

/** returns log (e^logprob1 + e^logprob2). */
double sumLogProb(double logprob1, double logprob2)
{
  if (isinf(logprob1) && isinf(logprob2)) 
    return logprob1; // both prob1 and prob2 are 0, return log 0.
  if (logprob1>logprob2)
    return logprob1+log(1+exp(logprob2-logprob1));
  else
    return logprob2+log(1+exp(logprob1-logprob2));
}
/////////////////////////////////////////////////////////////////////////


void MaxEntModel::addFeature(unsigned long f)
{
  _index[f] = _lambda.size();
  for (unsigned int i = 0; i<_classes; i++) {
    _lambda.push_back(0);
  }
}

void MaxEntModel::print(ostream& ostrm, MaxEntTrainer& trainer)
{
  for (FtMap::iterator it = _index.begin(); it!=_index.end(); it++) {
    unsigned long i = it->second;
    for (unsigned long c = 0; c<_classes; c++) {
      ostrm << "lambda(" << trainer.className(c) << ", " 
	    << trainer.getStr(it->first) << ")=" 
	    << _lambda[i+c] << endl;
    }
  }
}

int MaxEntModel::getProbs(MaxEntEvent& event, vector<double>& probs)
{
  probs.clear();
  probs.assign(_classes, 0);
  int max = -1;
  for (unsigned int c = 0; c<_classes; c++) {
    double s = 0;
    for (unsigned int f = 0; f<event.size(); f++) {
      FtMap::iterator it = _index.find(event[f]);
      if (it!=_index.end()) 
	s += _lambda[it->second+c];
    }
    probs[c] = s;
    if (max<0 || probs[max]<s)
      max = c;
  }
  double sum = sumLogProb(probs);
  for (unsigned int i = 0; i<_classes; i++) {
    probs[i] = exp(probs[i])/exp(sum);
  }
  return max;
}

EventSet::~EventSet()
{
  for (EventSet::iterator it = begin(); it!=end(); it++) {
    delete *it;
  }
}

double MaxEntModel::getExpects(EventSet& events, vector<double>& expects)
{
  expects.clear();
  expects.assign(_lambda.size(), 0);
  double sumLogProb = 0;
  for (unsigned int i = 0; i<events.size(); i++) {
    MaxEntEvent& e = *events[i];
    vector<double> probs;
    getProbs(e, probs);
    for (unsigned long c = 0; c < _classes; ++c) {
      double count = probs[c] * e.count();
      for (unsigned long j = 0; j < e.size(); j++) {
	FtMap::iterator it = _index.find(e[j]);
	if (it!=_index.end())
	  expects[it->second + c] += count;
      }
    }
    sumLogProb += log(probs[e.classId()]);
  }
  return sumLogProb;
}

double MaxEntModel::getObsCounts(EventSet& events, vector<double>& obsCounts)
{
  vector<double> probs;
  obsCounts.clear();
  obsCounts.assign(_lambda.size(), 0);
  double maxFtSum = 0;
  for (unsigned int i = 0; i<events.size(); i++) {
    MaxEntEvent& e = *events[i];
    unsigned long c = e.classId();
    double count = e.count();
    double ftSum = 0;
    for (unsigned long j=0; j<e.size(); j++) {
      FtMap::iterator it = _index.find(e[j]);
      if (it!=_index.end()) 
	obsCounts[it->second+c] += count;
      else { // new feature, need to expand obsCounts and _lambda
	for (unsigned int k = 0; k<_classes; k++) 
	  obsCounts.push_back(0);
	obsCounts[_lambda.size()+c] += count;
	addFeature(e[j]);
      }
      ftSum++;
    }
    if (ftSum>maxFtSum)
      maxFtSum = ftSum;
  }
  return maxFtSum;
}

void GISTrainer::train(MaxEntModel& model, EventSet& events)
{
  vector<double> obsCounts, expects;
  double C = model.getObsCounts(events, obsCounts);
  double sumLogProb=0, prevSumLogProb=0;
  for (int i = 0; i<_maxIterations; i++) {
    sumLogProb = model.getExpects(events, expects);
    if (_printDetails)
      cerr << "Iteration " << i+1 <<  " logProb=" << sumLogProb << endl;
    if (i>0 && sumLogProb-prevSumLogProb<=_threshold)
      break;
    prevSumLogProb = sumLogProb;
    vector<double>& lambda = model.lambda();
    for (unsigned int i = 0; i<lambda.size(); i++) {
      double obs = (obsCounts[i]-_alpha);
      double newLambda = 0;
      if (obs>0)
	newLambda = lambda[i]+log(obs/expects[i])/C;
      if (newLambda>0)
	lambda[i] = newLambda;
      else
	lambda[i] = 0;
    }
  }
}

unsigned long MaxEntTrainer::getClassId(string c)
{
  for (unsigned long i = 0; i<_classes.size(); i++) {
    if (c==_classes[i])
      return i;
  }
  return _classes.size();
}

double MaxEntTrainer::Test_Event(MaxEntEvent &Event, MaxEntModel &Model)
{
	vector<double> probs;
	double total=0, error = 0;

	int c = Model.getProbs(Event, probs);

	if( c != (int)Event.classId() )
	{
		error++;
	}

	cerr << className(Event.classId()) << '\t';

	for (unsigned int cl = 0; cl<probs.size(); cl++)
	{
		cerr << className(cl) << ' ' << probs[cl] << '\t';
	}
	cerr << endl;

	return c;
}

double MaxEntTrainer::test(EventSet& events, MaxEntModel& model)
{
  vector<double> probs;
  double total=0, error = 0;
  for (unsigned int i = 0; i<events.size(); i++) {
    int c = model.getProbs(*events[i], probs);
    if (c!=(int)events[i]->classId()) {
      error++;
      if (_printDetails) cerr << '*';
    }
    if (_printDetails) {
      cerr << className(events[i]->classId()) << '\t';
      for (unsigned int cl = 0; cl<probs.size(); cl++) {
	cerr << className(cl) << ' ' << probs[cl] << '\t';
      }
      cerr << endl;
    }
    total++;
  }
  return error/total;
}

void MaxEntTrainer::Add_Event(EventSet &events, const char *name, const char *data)
{
	const string delims(" ");

	MaxEntEvent	*event	= new MaxEntEvent;
	getIds(data, *event, delims);
	event->classId(getClassId(name));
	event->count(1);
	events.push_back(event);
}

void MaxEntTrainer::readEvents(istream& istrm, EventSet& events)
{
  string line, cls;
  const string delims(" ");
  while ((istrm>>cls) && getline(istrm, line)) {
    MaxEntEvent* event = new MaxEntEvent;
    getIds(line, *event, delims);
    event->classId(getClassId(cls));
    event->count(1);
    events.push_back(event);
  }
}

void MaxEntTrainer::loadParams(istream& istrm)
{
  string name;
  int c;
  istrm >> _alpha >> _threshold >> _maxIterations;
  istrm >> c;
  for (int i = 0; i<c && (istrm >> name); i++) {
    addClass(name);
  }
}

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

#ifndef ME_H
#define ME_H

/////////////////////////////////////////////////////////////////////////
#ifndef STR2IDMAP_H
#define STR2IDMAP_H

#include <vector>
#include <string>
#include <map>
using namespace std;

/** Establish a mapping between strings and integers so that a string
 can be converted to an integer id as well as the other way
 around. The integer id 0 is always reserved for the empty string. The
 ids of other strings begin with 1. 
*/
class Str2IdMap {
  map<string, unsigned long> _toId; // mapping from string to id
  vector<string> _toStr;            // mapping from id to string
public:
  /** return the string corresponding to an id */
  string getStr(unsigned long id) {
    return _toStr[id];
  }
  
  Str2IdMap() { _toStr.push_back(""); }

  /** Return the id corresponding to the string. If it is not
      currently in the mapping, it will be added to the mapping and
      assigned an id. */
  unsigned long getId(string str) {
    map<string, unsigned long>::iterator f = _toId.find(str);
    unsigned long id;
    if (f==_toId.end()) {
      id = _toStr.size();
      _toId[str] = id;
      _toStr.push_back(str);
      return id;
    }
    else 
      return f->second;
  }

  /** Like getId() except that if the string is not currently in the
      mapping, the id 0 will be returned and it is NOT added to the
      mapping. */
  unsigned long getExistingId(string str) {
    map<string, unsigned long>::iterator f = _toId.find(str);
    if (f==_toId.end()) 
      return 0;
    else 
      return f->second;
  }

  /** Convert the sequence of tokens in line into a sequence of
      integer ids. delim is the separator between the tokens.  */
  void getIds(string line, vector<unsigned long>& seq, string delim) {
    string::size_type begIdx, endIdx;
    begIdx = line.find_first_not_of(delim);
    while (begIdx!=string::npos) {
      endIdx = line.find_first_of(delim, begIdx);
      if (endIdx==string::npos) {
	endIdx = line.length();
      }
      string word = line.substr(begIdx, endIdx-begIdx);
      seq.push_back(getId(word));
      begIdx = line.find_first_not_of(delim, endIdx);
    }
  }
};

#endif
/////////////////////////////////////////////////////////////////////////


class MaxEntTrainer;

/** A Event consists of a set of binary features (corresponding to the
    integers in the vector).
*/
class MaxEntEvent : public vector<unsigned long> {
  double _count; // the number of instances of this event (typicall 1).
  unsigned long _classId; // the class that this event belongs to.
public:
  double count() const { return _count;}
  void count(double c) { _count = c;}
  unsigned long classId() { return _classId;}
  void classId(unsigned long id) { _classId = id;}
  MaxEntEvent() {
    _count = 0;
    _classId = 0;
  }
};
  
/** A set of events. */
class EventSet : public vector<MaxEntEvent*> {
public:
  ~EventSet();
};

/**
   The parameters in a maximum entropy classifier. 
 */
class MaxEntModel {
  typedef map<unsigned long, unsigned long> FtMap;
  unsigned long _classes; // the number of possible output classes
  FtMap _index;           // mapping features to indices in the _lambda vector
  vector<double> _lambda; // _lambda[_index[f]+c] is the lambda value for
                          // feature f and class c;
public:
  MaxEntModel(unsigned long classes=0) { _classes = classes;}

  vector<double>& lambda() { return _lambda;}

  /** Compute the probability of all classes given the event. Return
      the class with the highest probability. */
  int getProbs(MaxEntEvent& event, vector<double>& probs);

  /** Compute the observed counts of all features. Return the maximum
      number of features in any event. */
  double getObsCounts(EventSet& events, vector<double>& obsCounts);  

  /** Compute the expected value of all features. Return the log
      likelihood of the events.*/
  double getExpects(EventSet& events, vector<double>& expects);

  void classes(unsigned long classes) { _classes = classes;}

  /** Add a feature to the model. */
  void addFeature(unsigned long f);

  /** print the parameters in the model. */
  void print(ostream& ostrm, MaxEntTrainer& trainer);
};

/** The super class of all trainers for Maximum Entropy Models. It is
    also responsible for converting string form of features and
    classes into integer ids.  */
class MaxEntTrainer : public Str2IdMap {
protected:
  vector<string> _classes;
  double _alpha; // used as exponential prior
  double _threshold; // stop running GIS if the log likelihood is
                     // smaller than this
  double _maxIterations;

  bool _printDetails; 
public:  
  MaxEntTrainer() { _alpha = 0.1; _threshold = 0; _maxIterations = 100; _printDetails = false;}

	void	Set_Alpha			(double alpha)		{	_alpha			= alpha;		}
	void	Set_Threshold		(double threshold)	{	_threshold		= threshold;	}
	void	Set_Iterations		(double Iterations)	{	_maxIterations	= Iterations;	}

	void	Add_Event			(EventSet &events, const char *name, const char *data);

	double	Test_Event			(MaxEntEvent &Event, MaxEntModel &Model);

  void printDetails(bool flag) { _printDetails = flag;}

  virtual void train(MaxEntModel& model, EventSet& events) = 0;

  vector<string>& classes() { return _classes;}

  void addClass(string c) { _classes.push_back(c); }

  //  unsigned long getClass(unsigned long c) { return _classes[c];}
  string className(unsigned long c) { return _classes[c];}

  unsigned long getClassId(string c);

  /** Test the classification of the events. Return the error rate. */
  double test(EventSet& events, MaxEntModel& model);

  /** Read a set of events from istrm. Each event occupies a line. The
      first token is the class. The rest of the line are the features. */
  void readEvents(istream& istrm, EventSet& events);

  void loadParams(istream& istrm);

  virtual ~MaxEntTrainer() {};
};

class GISTrainer : public MaxEntTrainer {
public:
  virtual void train(MaxEntModel& model, EventSet& events);
};

#endif

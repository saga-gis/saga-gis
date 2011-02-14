/**********************************************************
 * Version $Id$
 *********************************************************/
///////////////////////////////////////////////////////////
//                     model_tools.h                     //
//                                                       //
//                 Copyright (C) 2007 by                 //
//                     Stefan Liersch                    //
//-------------------------------------------------------//
//    e-mail:     stefan.liersch@ufz.de                  //
//                stefan.liersch@gmail.com               //
//                     2008-01-24                        //
//-------------------------------------------------------//

#ifndef HEADER_INCLUDED__model_tools_H
#define HEADER_INCLUDED__model_tools_H

#include <vector>
typedef std::vector<double> vector_d;

namespace model_tools
{
	///////////////////////////////////////////////////////////////////////
	//
	//		OBJECTIVE FUNCTIONS
	//
	///////////////////////////////////////////////////////////////////////

	//---------------------------------------------------------------------
	// Calculate Nash-Sutcliff efficiency
	//---------------------------------------------------------------------
	double			CalcEfficiency(double *obs, double *sim, int nvals);

	double			CalcEfficiency(vector_d &obs, vector_d &sim);

	// Calculate Nash-Sutcliff efficiency adapted to high flow
	// Reference:
	//	Liu and De Smedt, 2004. WetSpa Extension, 
	//		A GIS-based Hydrologic Model for Flood Prediction and Watershed Management – 
	//		Documentation and User Manual. Brussels. Vrije Universiteit Brussel.
	double			Calc_NSE_HighFlow(double *obs, double *sim, int nvals);
	double			Calc_NSE_LowFlow (double *obs, double *sim, int nvals);
	//---------------------------------------------------------------------

	//---------------------------------------------------------------------
	// PBIAS
	//---------------------------------------------------------------------
	double			Calc_PBIAS(double* obs, double* sim, int nvals);
	//---------------------------------------------------------------------



	///////////////////////////////////////////////////////////////////////
	//
	//		RUNOFF COEFFICIENT
	//
	///////////////////////////////////////////////////////////////////////

	//---------------------------------------------------------------------
	// Calculate rainfall-runoff coefficient
	double			CalcRunoffCoeff			(double *streamflow,
											 double *precipitation, int nvals);
	
	double			CalcRunoffCoeff			(vector_d &streamflow,
											 vector_d &precipitation);
	//---------------------------------------------------------------------



	///////////////////////////////////////////////////////////////////////
	//
	//		UNIT CONVERSION FUNCTIONS
	//
	///////////////////////////////////////////////////////////////////////

	//---------------------------------------------------------------------
	double			m3s_to_mmday(double val, double area);
	double*			m3s_to_mmday(double *m3s, double *mmday, int nvals, double area);
	vector_d		m3s_to_mmday(vector_d &m3s, vector_d &mmday, double area);

	double			mmday_to_m3s(double val, double area);
	double*			mmday_to_m3s(double *mmday, double *m3s, int nvals, double area);
	vector_d		mmday_to_m3s(vector_d &mmday, vector_d &m3s, double area);
	//---------------------------------------------------------------------


	
	///////////////////////////////////////////////////////////////////////
	//
	//		MISC
	//
	///////////////////////////////////////////////////////////////////////

	//---------------------------------------------------------------------
	// Produce a random number within the given lower and upper bound
	// Don't forget to initialize the random function before calling
	// this function with: srand((unsigned) time(NULL)); // using time.h
	// include 'stdlib.h' and 'time.h' in the calling program
	double			Random_double(double lb, double ub);
	//---------------------------------------------------------------------

	//---------------------------------------------------------------------
	// Returns true if the year is a leap year
	bool			LeapYear(int year);
	//---------------------------------------------------------------------

	//---------------------------------------------------------------------
	// This function receives an array of doubles and
	// returns the indices of the lowest values.
	// array	= array of doubles
	// nvals		= number of values in array
	// TopIndex = array of indices = return values
	// top		= number of lowest values which indices will be stored in TopIndex
	void			FindLowestIndices(double* array, int nvals, int* TopIndex, int top);
	//---------------------------------------------------------------------

	//---------------------------------------------------------------------
	// This function receives an array of doubles and
	// returns the indices of the highest values.
	// array	= array of doubles
	// nvals		= number of values in array
	// TopIndex = array of indices = return values
	// top		= number of highest values which indices will be stored in TopIndex
	// min		= minimum
	void			FindHighestIndices(double* array, int nvals, int* TopIndex, int top, double min);
	//---------------------------------------------------------------------


	//---------------------------------------------------------------------
	// summarize arrays
	//---------------------------------------------------------------------
	double			SumArray(double* array, unsigned int size);
};





///////////////////////////////////////////////////////////////////////
//
//	dynamic array template class
//
///////////////////////////////////////////////////////////////////////
// http://www.codeguru.com/forum/showthread.php?s=&threadid=231046
template <typename T>
class dynamic_array
{
public:
  dynamic_array(){};
  dynamic_array(int rows, int cols)
  {
    for(int i=0; i<rows; ++i)
    {
      data_.push_back(std::vector<T>(cols));
    }
  }
  
  // other ctors ....

  inline std::vector<T> & operator[](int i) { return data_[i]; }

  inline const std::vector<T> & operator[] (int i) const { return data_[i]; }

  // other accessors, like at() ...

  void resize(int rows, int cols)
  {
    data_.resize(rows);
    for(int i = 0; i < rows; ++i)
      data_[i].resize(cols);
  }

  // other member functions, like reserve()....

private:
  std::vector<std::vector<T> > data_;  
};

//---------------------------------------------------------------------
// An example to use the template class
//---------------------------------------------------------------------
//int main()
//{
//  dynamic_array<int> a(3, 3);
//  a[1][1] = 2;
//  int x = a[1][1];
//  return 0;
//}
///////////////////////////////////////////////////////////////////////


#endif /* HEADER_INCLUDED__model_tools_H */

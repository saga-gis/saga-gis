/**********************************************************
 * Version $Id$
 *********************************************************/
#include "model_tools.h"
#include <math.h>
#include <stdlib.h> // random numbers
//-------------------------------------------------------------------

///////////////////////////////////////////////////////////////////////
//
//		OBJECTIVE FUNCTIONS
//
///////////////////////////////////////////////////////////////////////

//---------------------------------------------------------------------
// Calculate Nash-Sutcliff efficiency
//---------------------------------------------------------------------
double model_tools::CalcEfficiency(double *obs, double *sim, int nvals)
{
	int		i;

	double sum_obsminsim_2	= 0.0;
	double sum_obsminmean_2 = 0.0;
	double mean_obs			= 0.0;
	//double sum_obs			= 0.0; // sum of observed discharge

	// calculate mean of observed time series
	for (i = 0; i < nvals; i++)
		mean_obs += obs[i] / nvals;
	
	for (i = 0; i < nvals; i++)
	{
		sum_obsminsim_2 += pow(obs[i] - sim[i], 2.0);
		sum_obsminmean_2 += pow(obs[i] - mean_obs, 2.0);
	}
	
	return (1 - sum_obsminsim_2 / sum_obsminmean_2);
}
//-------------------------------------------------------------------

double model_tools::CalcEfficiency(vector_d &obs, vector_d &sim)
{
	int		i;

	int nvals = obs.size();
	double sum_obsminsim_2	= 0.0;
	double sum_obsminmean_2 = 0.0;
	double mean_obs			= 0.0;
	//double sum_obs			= 0.0; // sum of observed discharge

	// calculate mean of observed time series
	for (i = 0; i < nvals; i++)
		mean_obs += obs[i] / nvals;
	
	for (i = 0; i < nvals; i++)
	{
		sum_obsminsim_2 += pow(obs[i] - sim[i], 2.0);
		sum_obsminmean_2 += pow(obs[i] - mean_obs, 2.0);
	}
	
	return (1 - sum_obsminsim_2 / sum_obsminmean_2);
}
//-------------------------------------------------------------------
// Calculate Nash-Sutcliff efficiency adapted to high flow
// Reference:
//	Liu and De Smedt, 2004. WetSpa Extension, 
//		A GIS-based Hydrologic Model for Flood Prediction and Watershed Management  
//		Documentation and User Manual. Brussels. Vrije Universiteit Brussel.

double model_tools::Calc_NSE_HighFlow(double *obs, double *sim, int nvals)
{
	int		i;

	double mean_obs				= 0.0;
	double sum_numerator		= 0.0;
	double sum_denominator		= 0.0;

	// calculate mean of observed time series
	for (i = 0; i < nvals; i++)
		mean_obs += obs[i] / nvals;
	
	for (i = 0; i < nvals; i++)
	{
		sum_numerator	+= ( (obs[i] + mean_obs) * pow(sim[i] - obs[i], 2.0));
		sum_denominator	+= ( (obs[i] + mean_obs) * pow(obs[i] - mean_obs, 2.0));
	}

	return ( 1 - sum_numerator / sum_denominator );
}
//-------------------------------------------------------------------

//-------------------------------------------------------------------
// Calculate Nash-Sutcliff efficiency adapted to high flow
// Reference:
//	Liu and De Smedt, 2004. WetSpa Extension, 
//		A GIS-based Hydrologic Model for Flood Prediction and Watershed Management  
//		Documentation and User Manual. Brussels. Vrije Universiteit Brussel.

double model_tools::Calc_NSE_LowFlow(double *obs, double *sim, int nvals)
{
	int		i;

	double mean_obs				= 0.0;
	double sum_log_obsminsim_2	= 0.0;
	double sum_log_obsminmean_2 = 0.0;

	// calculate mean of observed time series
	for (i = 0; i < nvals; i++)
		mean_obs += obs[i] / nvals;

	for (i = 0; i < nvals; i++)
	{
		sum_log_obsminsim_2  += pow( (log(obs[i])-log(sim[i])),2 );
		sum_log_obsminmean_2 += pow( (log(obs[i])-log(mean_obs)),2 );
	}

	return( 1 - sum_log_obsminsim_2 / sum_log_obsminmean_2);
}
//---------------------------------------------------------------------

//---------------------------------------------------------------------
//                    Calculate PBIAS
//---------------------------------------------------------------------
double model_tools::Calc_PBIAS(double* obs, double* sim, int nvals)
{
	double	sim_min_obs = 0.0;
	double	sum_obs		= 0.0;

	for (int i = 0; i < nvals; i++)
	{
		sim_min_obs += sim[i] - obs[i];
		sum_obs += obs[i];
	}

	return(sim_min_obs * 100 / sum_obs);
}
//---------------------------------------------------------------------




///////////////////////////////////////////////////////////////////////
//
//		RUNOFF COEFFICIENT
//
///////////////////////////////////////////////////////////////////////

//---------------------------------------------------------------------
//                    Calculate Runoff Coefficient
//---------------------------------------------------------------------
double model_tools::CalcRunoffCoeff(vector_d &streamflow, vector_d &precipitation)
{
	// calculate runoff coefficient as stated in:
	// Kokkonen, T. S. et al. (2003).
	double sum_flow = 0;
	double sum_pcp  = 0;

	for (unsigned int i = 0; i < streamflow.size(); i++)
	{
		sum_flow += streamflow[i];
		sum_pcp  += precipitation[i];
	}
	return (sum_flow / sum_pcp * 100);
}
//---------------------------------------------------------------------
double model_tools::CalcRunoffCoeff(double *streamflow, double *precipitation,
									int nvals)
{
	// calculate runoff coefficient as stated in:
	// Kokkonen, T. S. et al. (2003).
	double sum_flow = 0;
	double sum_pcp  = 0;

	for (int i = 0; i < nvals; i++)
	{
		sum_flow += streamflow[i];
		sum_pcp  += precipitation[i];
	}
	return (sum_flow / sum_pcp * 100);
}





///////////////////////////////////////////////////////////////////////
//
//		UNIT CONVERSION
//
///////////////////////////////////////////////////////////////////////

double model_tools::m3s_to_mmday(double val, double area)
{
	return(val * 86.4 / area);
}
//---------------------------------------------------------------------
double *model_tools::m3s_to_mmday(double *m3s, double *mmday, int nvals, double area)
{
	for (int i = 0; i < nvals; i++)
		mmday[i] = m3s[i] * 86.4 / area;
	return(mmday);
}
//---------------------------------------------------------------------
vector_d model_tools::m3s_to_mmday(vector_d &m3s, vector_d &mmday, double area)
{
	for (unsigned int i = 0; i < m3s.size(); i++)
		mmday[i] = m3s[i] * 86.4 / area;
	return(mmday);
}
//---------------------------------------------------------------------

double model_tools::mmday_to_m3s(double val, double area)
{
	return(val * area / 86.4);
}
//---------------------------------------------------------------------
double *model_tools::mmday_to_m3s(double *mmday, double *m3s, int nvals, double area)
{
	for (int i = 0; i < nvals; i++)
		m3s[i] = mmday[i] * area / 86.4;
	return(m3s);
}
//---------------------------------------------------------------------
vector_d model_tools::mmday_to_m3s(vector_d &mmday, vector_d &m3s, double area)
{
	for (unsigned int i = 0; i < m3s.size(); i++)
		m3s[i] = mmday[i] * area / 86.4;
	return(m3s);
}





///////////////////////////////////////////////////////////////////////
//
//		MISC
//
///////////////////////////////////////////////////////////////////////

//---------------------------------------------------------------------
// Create a random number
//---------------------------------------------------------------------
double model_tools::Random_double(double lb, double ub)
{
	// lb = lower bound, ub = upper bound
	double random;

	random = rand()/(1.+RAND_MAX);
	random = lb + (ub - lb) * random;

	return(random);
}
//-------------------------------------------------------------------

//---------------------------------------------------------------------
// Leap year function
//---------------------------------------------------------------------
bool model_tools::LeapYear(int year)
{
	if ( (year % 4   == 0) &&
		 (year % 100 != 0) ||
		 (year % 400 == 0) )
		 return(true);
	else 
		return(false);
}
//-------------------------------------------------------------------


//---------------------------------------------------------------------
// Find indices of lowest values
//---------------------------------------------------------------------
void model_tools::FindLowestIndices(double *array, int nvals, int *TopIndex, int top)
{
	double min_temp = 99999999.0;
	double x		= -99999999.0;
	int index		= 0;

	for (int k = 0; k < top; k++)
	{
		for (int j = 0; j < nvals; j++)
		{
			if (array[j] < min_temp && array[j] > x)
			{
				min_temp = array[j];
				index = j;
			}
		}
		TopIndex[k] = index;
		x = min_temp;
		min_temp = 99999999.0;
	}
}

//---------------------------------------------------------------------
// Find indices of highest values
//---------------------------------------------------------------------
void model_tools::FindHighestIndices(double *array, int nvals, int *TopIndex, int top, double min)
{
	double max_temp = -99999999.0;
	double x		= 99999999.0;
	int index		= 0;
	bool exist		= false;

	for (int k = 0; k < top; k++)
	{
		for (int j = 0; j < nvals; j++)
		{
			if (array[j] > max_temp && array[j] < x && array[j] > min)
			{
				max_temp = array[j];
				index = j;
				exist = true;
			}
		}
		if (exist)
		{
			TopIndex[k] = index;
		} else {
			TopIndex[k] = -1;
		}
		exist = false;
		x = max_temp;
		max_temp = -99999999.0;
	}
}
//---------------------------------------------------------------------


//---------------------------------------------------------------------
// summarize arrays
//---------------------------------------------------------------------
double model_tools::SumArray(double* array, unsigned int size)
{
	double sum = 0.0;
	for (unsigned int i = 0; i < size; i++) {
		sum += array[i];
	}
	return(sum);
}
//---------------------------------------------------------------------

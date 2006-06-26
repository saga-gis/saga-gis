
///////////////////////////////////////////////////////////
//                                                       //
//                         SAGA                          //
//                                                       //
//      System for Automated Geoscientific Analyses      //
//                                                       //
//                    Module Library:                    //
//                    Table_Calculus                     //
//                                                       //
//-------------------------------------------------------//
//                                                       //
//                        LMFit.h                        //
//                                                       //
//                 Copyright (C) 2003 by                 //
//                    Andre Ringeler                     //
//                                                       //
//-------------------------------------------------------//
//                                                       //
// This file is part of 'SAGA - System for Automated     //
// Geoscientific Analyses'. SAGA is free software; you   //
// can redistribute it and/or modify it under the terms  //
// of the GNU General Public License as published by the //
// Free Software Foundation; version 2 of the License.   //
//                                                       //
// SAGA is distributed in the hope that it will be       //
// useful, but WITHOUT ANY WARRANTY; without even the    //
// implied warranty of MERCHANTABILITY or FITNESS FOR A  //
// PARTICULAR PURPOSE. See the GNU General Public        //
// License for more details.                             //
//                                                       //
// You should have received a copy of the GNU General    //
// Public License along with this program; if not,       //
// write to the Free Software Foundation, Inc.,          //
// 59 Temple Place - Suite 330, Boston, MA 02111-1307,   //
// USA.                                                  //
//                                                       //
//-------------------------------------------------------//
//                                                       //
//    e-mail:     aringel@gwdg.de                        //
//                                                       //
//    contact:    Andre Ringeler                         //
//                Institute of Geography                 //
//                University of Goettingen               //
//                Goldschmidtstr. 5                      //
//                37077 Goettingen                       //
//                Germany                                //
//                                                       //
///////////////////////////////////////////////////////////

//---------------------------------------------------------

#ifndef LMFitH
#define LMFitH

#include <vector>
#include <math.h>
#define SWAP(a,b) {temp=(a);(a)=(b);(b)=temp;}

using namespace std;

class TLMFit{
public:


   TLMFit::TLMFit  (vector<double> Xdata,
                              vector<double> Ydata,
                              vector<double> Param,
                             
                              void (*CfuncP)(double x, vector<double> ca,
                              double &y, vector<double> &dyda, int na));

   TLMFit::~TLMFit (){};

  void Fit();

  // Accessors:
  vector< vector<double> >	Alpha() const {return alpha;};
  vector< vector<double> > 	Covar() const {return covar;};
  double Chisq() const {return chisq;};
  vector<double> Param() const {return a;};
  int Ndata() const {return ndata;};
  int Nparam() const {return nparam;};
  double Alamda() const {return alamda;};

private:
  void gaussj(vector< vector<double> > &a, int m, vector< vector<double> > &b, int n);
  void covsrt(int mfit);
  void mrqcof(vector<double> &a, vector< vector<double> > &alpha, vector<double> &beta);
  void mrqmin(void);

  double chisq, alamda;
  vector<double> x, y, a;                 //x-, y-Values, Fit-Parameters a, Sigmas
  vector<int> ia;                              //Use Fit-Parameter for fit (1) or not (0)
  vector< vector<double> > covar, alpha;       //Covarianz-Matrix
  int ndata, nparam;                           //Number of Points, Number of Parameters
  void (*funcP)(double x, vector<double> ca, double &y, vector<double> &dyda, int na);
};

class ESingularMatrix {// Error Handling
	public : int Type;
        	 ESingularMatrix (int i);
};
#endif



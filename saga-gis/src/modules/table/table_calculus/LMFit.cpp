
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
//                       LMFit.cpp                       //
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

#include "LMFit.h"

TLMFit::TLMFit(vector<double> Xdata, vector<double> Ydata, vector<double> Param,  void(*CfuncP)(double x, vector<double> ca, double &y, vector < double> &dyda, int na))
{
	int		i, mfit = 0;
	chisq = 0;
	alamda = -1;
	
	ndata  = Xdata.size();
	nparam = Param.size();
	
	x.resize(ndata);
	y.resize(ndata);
	for (i = 0; i < ndata; i++)
	{
		x[i]   = Xdata[i];
		y[i]   = Ydata[i];
	}
	
	a.resize(nparam);
	ia.resize(nparam);
	for (i = 0; i < nparam; i++)
	{
		a[i] = Param[i];
		ia[i] = 1;
		if (ia[i])
			mfit++;
	}
	
	alpha.resize(mfit);
	covar.resize(mfit);
	for (i = 0; i < mfit; i++)
	{
		covar[i].resize(mfit);
		alpha[i].resize(mfit);
	}
	
	funcP = CfuncP;
}
//----------------------------------------------------------------------------
void TLMFit::Fit(void)
{
	mrqmin();
}

void TLMFit::gaussj(vector< vector<double> > &aa, int n, vector< vector < double> > &b, int m)
{
	vector < int> indxc, indxr, ipiv;
	int i, icol, irow, j, k, l, ll;
	double big, dum, pivinv, temp, test;
	
	indxc.resize(n);
	indxr.resize(n);
	ipiv.resize(n);
	
	for (j = 0; j < n; j++)
		ipiv[j] = 0;
	for (i = 0; i < n; i++)
	{
		big = 0.0;
		for (j = 0; j < n; j++)
			if (ipiv[j] != 1)
				for (k = 0; k < n; k++)
				{
					if (ipiv[k] == 0)
					{
						if (fabs(aa[j][k]) >= big)
						{
							big = fabs(aa[j][k]);
							irow = j;
							icol = k;
						}
					}
					else if (ipiv[k] > 1)
					{
						throw ESingularMatrix(1);
					}
				}
				++(ipiv[icol]);
				
				if (irow != icol)
				{
					for (l = 0; l < n; l++)
						SWAP(aa[irow][l], aa[icol][l]);
					for (l = 0; l < m; l++)
						SWAP(b[irow][l], b[icol][l]);
				}
				indxr[i] = irow;
				indxc[i] = icol;
				if (fabs(aa[icol][icol]) < 1E-300)
				{
					throw ESingularMatrix(2);
				}
				//         else
				test = aa[icol][icol];
				
				pivinv = 1.0/test;
				aa[icol][icol] = 1.0;
				for (l = 0; l < n; l++)
					aa[icol][l] *= pivinv;
				for (l = 0; l < m; l++)
					b[icol][l] *= pivinv;
				
				for (ll = 0; ll < n; ll++)
					if (ll != icol)
					{
						dum = aa[ll][icol];
						aa[ll][icol] = 0.0;
						for (l = 0; l < n; l++)
							aa[ll][l] -= aa[icol][l]*dum;
						for (l = 0; l < m; l++)
							b[ll][l] -= b[icol][l]*dum;
					}
	}
	for (l =(n - 1); l > -1; l--)
	{
        if (indxr[l] != indxc[l])
			for (k = 0; k < n; k++)
				SWAP(aa[k][indxr[l]], aa[k][indxc[l]]);
	}
}
//-----------------------------------------------------------------------
void TLMFit::covsrt(int mfit)
{
	int i, j, k;
	double temp;
	
	for (i = mfit; i < nparam; i++)
		for (j = 0; j < i; j++)
			covar[i][j] = 0.0;
		k = mfit;
		for (j = (nparam - 1); j>-1; j--)
		{
			if (ia[j])
			{
				for (i = 0; i < nparam; i++)
					SWAP(covar[i][k], covar[i][j]);
				for (i = 0; i < nparam; i++)
					SWAP(covar[k][i], covar[j][i]);
				k--;
			}
		}
}
//-----------------------------------------------------------------------
void TLMFit::mrqcof(vector<double> &ba, vector< vector<double> > &balpha, vector < double> &bbeta)
{
	vector < double> dyda(nparam, 0);
	int i, j, k, l, m, mfit = 0;
	double ymod, wt,  dy;
	
	for (j = 0; j < nparam; j++)
		if (ia[j] > 0)
			mfit++;
		
		for (j = 0; j < mfit; j++)
		{
			for (k = 0; k <=j; k++)
				balpha[j][k] = 0.0;
			bbeta[j] = 0.0;
		}
		chisq = 0.0;
		
		for (i = 0; i < ndata; i++)
		{
			(*funcP)(x[i], ba, ymod, dyda, nparam);
			
			dy = y[i] - ymod;
			for (j=-1, l = 0; l < nparam; l++)
			{
				if (ia[l])
				{
					wt = dyda[l];
					for (j++, k=-1, m = 0; m <= l; m++)
					{
						if (ia[m])
							balpha[j][++k] += wt*dyda[m];
					}
					bbeta[j] += dy*wt;
				}
			}
			chisq += dy*dy;
		}
		
		for (j = 1; j < mfit; j++)
			for (k = 0; k < j; k++)
				balpha[k][j] = balpha[j][k];
}
//-----------------------------------------------------------------------
void TLMFit::mrqmin()
{
	static vector < double> atry, beta, da;
	int j, k, l;
	static int mfit;
	static double ochisq;
	static vector< vector < double> > oneda;
	
	if (alamda < 0.0)
	{
		atry.resize(nparam);
		beta.resize(nparam);
		da.resize(nparam);
		
		for (mfit = 0, j = 0; j < nparam; j++)
			if (ia[j])
				mfit++;
			oneda.resize(mfit);
			for (unsigned int i = 0; i < oneda.size(); i++)
				oneda[i].resize(1);
			alamda = 0.001;
			mrqcof(a, alpha, beta);
			ochisq = (chisq);
			for (j = 0; j < nparam; j++)
				atry[j] =(a[j]);
	}
	for (j = 0; j < mfit; j++)
	{
		for (k = 0; k < mfit; k++)
			covar[j][k] = alpha[j][k];
		covar[j][j] = alpha[j][j]*(1.0 + (alamda));
		oneda[j][0] = beta[j];
	}
	try {gaussj(covar, mfit, oneda, 1);}
	catch (ESingularMatrix &E)
	{
		throw;
	}
	for (j = 0; j < mfit; j++)
	{
		da[j] = oneda[j][0];
	}
	if (alamda == 0.0)
	{
		covsrt(mfit);
		return;
	}
	for (j = 0, l = 0; l < nparam; l++)
		if (ia[l])
			atry[l] = a[l] + da[j++];
		mrqcof(atry, covar, da);
		if (chisq < ochisq)
		{
			alamda *= 0.1;
			ochisq = (chisq);
			for (j = 0; j < mfit; j++)
			{
				for (k = 0; k < mfit; k++)
					alpha[j][k] = covar[j][k];
				beta[j] = da[j];
			}
			for (j = 0; j < nparam; j++)             // Achtung!! in aelteren Versionen war hier ein Fehler
				a[j] = atry[j];
		} else 
		{
			alamda *= 10.0;
			chisq = ochisq;
		}
}
//----------------------------------------------------------------------------
// Implementation ESinglularMatrix (error handling for singular matrix)
//----------------------------------------------------------------------------
ESingularMatrix::ESingularMatrix(int i)
{
	Type = i;
}
//----------------------------------------------------------------------------


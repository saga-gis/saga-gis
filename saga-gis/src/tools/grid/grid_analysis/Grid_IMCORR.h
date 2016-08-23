/**********************************************************
 * Version $Id$
 *********************************************************/

///////////////////////////////////////////////////////////
//                                                       //
//                         SAGA                          //
//                                                       //
//      System for Automated Geoscientific Analyses      //
//                                                       //
//                     Tool Library                      //
//                     grid analysis                     //
//                                                       //
//-------------------------------------------------------//
//                                                       //
//                     Grid_IMCORR.h                     //
//                                                       //
//                 Copyright (C) 2012 by                 //
//                     Magnus Bremer                     //
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
// 51 Franklin Street, 5th Floor, Boston, MA 02110-1301, //
// USA.                                                  //
//                                                       //
//-------------------------------------------------------//
//                                                       //
//    e-mail:     magnus.bremer@uibk.ac.at               //
//                                                       //
//    contact:    Magnus Bremer                          //
//                Institute of Geography                 //
//                University of Innsbruck                //
//                Innrain 52                             //
//                6020 Innsbruck                         //
//                Austria                                //
//                                                       //
///////////////////////////////////////////////////////////

//---------------------------------------------------------


///////////////////////////////////////////////////////////
//														 //
//														 //
//														 //
///////////////////////////////////////////////////////////

//---------------------------------------------------------
#ifndef HEADER_INCLUDED__Grid_IMCORR_H
#define HEADER_INCLUDED__Grid_IMCORR_H

//---------------------------------------------------------
#include "MLB_Interface.h"
#include <vector>


///////////////////////////////////////////////////////////
//														 //
//														 //
//														 //
///////////////////////////////////////////////////////////

//---------------------------------------------------------
class CGrid_IMCORR : public CSG_Tool_Grid
{
public:
	CGrid_IMCORR(void);


protected:

	virtual bool			On_Execute		(void);


private:
	void	Get_This_Chip(std::vector<std::vector<double> >& Chip, CSG_Grid *pGrid, int gx, int gy, int Ref_Chipsize);

	void	gcorr(std::vector<std::vector<double> >ChipSearch, std::vector<std::vector<double> >ChipRef, double csmin, int mfit, double ddmx, std::vector<double> ioffrq, std::vector<double> nomoff,    int& iacrej, double& streng, std::vector<double>& bfoffs,  std::vector<double>& tlerrs, double ddact);

	int		decimal(std::vector<int> Bin);
	void	binary(std::vector<int>& Bin, int number);
	void	cross(std::vector<double>& UNORMC , std::vector<std::vector<double> >ChipSearch, std::vector<std::vector<double> >ChipRef);
	void	fft2(std::vector<double>& data, std::vector<int>nel, int isign);
	void	gnorm(std::vector<double>& CCNORM , std::vector<double>& pkval, std::vector<int>& ipkcol, std::vector<int>& ipkrow, std::vector<double>& sums,       std::vector<std::vector<double> >ChipSearch, std::vector<std::vector<double> >ChipRef, std::vector<double> UNORMC );
	void	eval(int ncol, int nrow, std::vector<double> CCNORM , std::vector<double> pkval, std::vector<int> ipkcol, std::vector<int> ipkrow, std::vector<double> sums, double& csmin, double& streng, int& iacrej, std::vector<double>& cpval);
	void	fitreg(std::vector<double> cpval, int mfit, std::vector<double>& pkoffs, std::vector<double>& tlerrs);
	void	sums(std::vector<double> cpval, int mfit, std::vector<double>& z, std::vector<double>& wghts,std::vector<std::vector<float> > & b, std::vector<double>& vector);
	void	kvert(std::vector<std::vector<float> >& V);
	float	detrm( float a[ 25 ][ 25 ], float k );
	void	cofact( float num[ 25 ][ 25 ], float f, std::vector<std::vector<float> >& INV );
	void	trans( float num[ 25 ][ 25 ], float fac[ 25 ][ 25 ], float r , std::vector<std::vector<float> >& INV);
	void	esterr(std::vector<double> z, std::vector<double> wghts,std::vector<std::vector<float> > bnvrs, std::vector<double> coeffs, std::vector<double>& pkoffs, std::vector<double>& tlerrs);

};


///////////////////////////////////////////////////////////
//														 //
//														 //
//														 //
///////////////////////////////////////////////////////////

//---------------------------------------------------------
#endif // #ifndef HEADER_INCLUDED__Grid_IMCORR_H


///////////////////////////////////////////////////////////
//                                                       //
//                         SAGA                          //
//                                                       //
//      System for Automated Geoscientific Analyses      //
//                                                       //
//                    Module Library:                    //
//                     ta_hydrology                      //
//                                                       //
//-------------------------------------------------------//
//                                                       //
//                    Flow_Parallel.h                    //
//                                                       //
//                 Copyright (C) 2003 by                 //
//                      Olaf Conrad                      //
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
//    e-mail:     oconrad@saga-gis.org                   //
//                                                       //
//    contact:    Olaf Conrad                            //
//                Institute of Geography                 //
//                University of Goettingen               //
//                Goldschmidtstr. 5                      //
//                37077 Goettingen                       //
//                Germany                                //
//                                                       //
///////////////////////////////////////////////////////////

//---------------------------------------------------------


///////////////////////////////////////////////////////////
//														 //
//                                                       //
//														 //
///////////////////////////////////////////////////////////

//---------------------------------------------------------
#ifndef HEADER_INCLUDED__Flow_Parallel_H
#define HEADER_INCLUDED__Flow_Parallel_H


///////////////////////////////////////////////////////////
//														 //
//                                                       //
//														 //
///////////////////////////////////////////////////////////

//---------------------------------------------------------
#include "Flow.h"


///////////////////////////////////////////////////////////
//														 //
//                                                       //
//														 //
///////////////////////////////////////////////////////////

//---------------------------------------------------------
class ta_hydrology_EXPORT CFlow_Parallel : public CFlow  
{
public:
	CFlow_Parallel(void);
	virtual ~CFlow_Parallel(void);


protected:

	virtual void			On_Initialize	(void);

	virtual bool			Calculate		(void);
	virtual bool			Calculate		(int x, int y);


private:

	int						BRM_kgexp[8], BRM_idreh[8];

	double					TH_LinearFlow, BRM_sinus[361], BRM_cosin[361];


	bool					Set_Flow		(void);
	void					Check_Route		(int x, int y);

	void					Set_D8			(int x, int y);
	void					Set_Rho8		(int x, int y);
	void					Set_DInf		(int x, int y);
	void					Set_MFD			(int x, int y);
	void					Set_MDInf		(int x, int y);
	void					Set_BRM			(int x, int y);

	void					BRM_Init		(void);
	int						BRM_InitRZ		(int x, int y, int ix[3], int iy[3]);
	void					BRM_GetOrtho	(int Dir, int x, int y, int ix[3], int iy[3], double nnei[6], int nexp[6]);
	void					BRM_GetDiago	(int Dir, int x, int y, int ix[3], int iy[3], double nnei[6], int nexp[6]);
	void					BRM_QStreuung	(int i64, int g64, double nnei[6], int nexp[6], int &QBinaer, double &QLinks, double &QMitte, double &QRecht);
};


///////////////////////////////////////////////////////////
//														 //
//                                                       //
//														 //
///////////////////////////////////////////////////////////

//---------------------------------------------------------
#endif // #ifndef HEADER_INCLUDED__Flow_Parallel_H

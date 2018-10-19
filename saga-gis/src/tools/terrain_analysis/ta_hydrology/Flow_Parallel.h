
///////////////////////////////////////////////////////////
//                                                       //
//                         SAGA                          //
//                                                       //
//      System for Automated Geoscientific Analyses      //
//                                                       //
//                     Tool Library                      //
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
// Free Software Foundation, either version 2 of the     //
// License, or (at your option) any later version.       //
//                                                       //
// SAGA is distributed in the hope that it will be       //
// useful, but WITHOUT ANY WARRANTY; without even the    //
// implied warranty of MERCHANTABILITY or FITNESS FOR A  //
// PARTICULAR PURPOSE. See the GNU General Public        //
// License for more details.                             //
//                                                       //
// You should have received a copy of the GNU General    //
// Public License along with this program; if not, see   //
// <http://www.gnu.org/licenses/>.                       //
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
class CFlow_Parallel : public CFlow  
{
public:
	CFlow_Parallel(void);


protected:

	virtual int				On_Parameters_Enable	(CSG_Parameters *pParameters, CSG_Parameter *pParameter);

	virtual void			On_Initialize			(void);

	virtual bool			Calculate				(void);
	virtual bool			Calculate				(int x, int y);


private:

	bool					Set_Flow		(void);

	void					Check_Route		(int x, int y);

	void					Set_D8			(int x, int y, int Direction = -1);
	void					Set_Rho8		(int x, int y);
	void					Set_DInf		(int x, int y);
	void					Set_MFD			(int x, int y);
	void					Set_MMDGFD		(int x, int y);	
	void					Set_MDInf		(int x, int y);	
	void					Set_BRM			(int x, int y);

	//-----------------------------------------------------
	int						BRM_kgexp[8], BRM_idreh[8];

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

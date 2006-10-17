
///////////////////////////////////////////////////////////
//                                                       //
//                         SAGA                          //
//                                                       //
//      System for Automated Geoscientific Analyses      //
//                                                       //
//                    Module Library:                    //
//                      Grid_Filter                      //
//                                                       //
//-------------------------------------------------------//
//                                                       //
//                    Filter_Gauss.h                     //
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
//    e-mail:     aringel@saga-gis.org                   //
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


///////////////////////////////////////////////////////////
//														 //
//                                                       //
//														 //
///////////////////////////////////////////////////////////

//---------------------------------------------------------
#ifndef HEADER_INCLUDED__Filter_Gauss_H
#define HEADER_INCLUDED__Filter_Gauss_H


//---------------------------------------------------------
#include "MLB_Interface.h"

//---------------------------------------------------------
class CFilter_Gauss : public CModule_Grid
{
public:
	CFilter_Gauss(void);
	virtual ~CFilter_Gauss(void);
	

protected:

	virtual bool		On_Execute		(void);


private:

	CSG_Grid_Radius		m_Radius;

	CGrid				*pInput;

	CGrid				*pKernel;

	double				m_sigma;


	double				Gauss_Function	(double x, double y);
	void				Init_Kernel		(int Radius);
	
	double				Get_Mean_Square	(int x, int y, int Radius);
	double				Get_Mean_Circle	(int x, int y);

};

//---------------------------------------------------------
#endif // #ifndef HEADER_INCLUDED__Filter_Gauss_H

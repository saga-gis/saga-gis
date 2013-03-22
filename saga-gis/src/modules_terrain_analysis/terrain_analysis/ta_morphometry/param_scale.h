/**********************************************************
 * Version $Id: param_scale.cpp 911 2011-11-11 11:11:11Z oconrad $
 *********************************************************/

///////////////////////////////////////////////////////////
//                                                       //
//                         SAGA                          //
//                                                       //
//      System for Automated Geoscientific Analyses      //
//                                                       //
//                    Module Library:                    //
//                      morphometry                      //
//                                                       //
//-------------------------------------------------------//
//                                                       //
//                     param_scale.h                     //
//                                                       //
//                 Copyright (C) 2013 by                 //
//                     Olaf Conrad                       //
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
//    e-mail:     oconrad@saga-gis.de                    //
//                                                       //
//    contact:    Olaf Conrad                            //
//                Institute of Geography                 //
//                University of Hamburg                  //
//                Germany                                //
//                                                       //
///////////////////////////////////////////////////////////

//---------------------------------------------------------


///////////////////////////////////////////////////////////
//														 //
//														 //
//														 //
///////////////////////////////////////////////////////////

//---------------------------------------------------------
#ifndef HEADER_INCLUDED__param_scale_H
#define HEADER_INCLUDED__param_scale_H

//---------------------------------------------------------
#include "MLB_Interface.h"


///////////////////////////////////////////////////////////
//														 //
//														 //
//														 //
///////////////////////////////////////////////////////////

//---------------------------------------------------------
class CParam_Scale : public CSG_Module_Grid
{
public:
	CParam_Scale(void);

	virtual CSG_String		Get_MenuPath		(void)	{	return( _TL("A:Terrain Analysis|Terrain Classification" ));	}


protected:

	virtual bool			On_Execute		(void);


private:

	int						m_Radius;

	CSG_Matrix				m_Weights;

	CSG_Grid				*m_pDEM;


	bool					Get_Parameters	(double zScale, double coeff[6], double &elevation, double &slope, double &aspect, double &profc, double &planc, double &longc, double &crosc, double &minic, double &maxic);
	int						Get_Feature		(double slope, double minic, double maxic, double crosc, double Tol_Slope, double Tol_Curve);

	bool					Get_Weights		(void);
	bool					Get_Normal		(CSG_Matrix &Normal);
	bool					Get_Observed	(int x, int y, CSG_Vector &Observed, bool bConstrain);

};


///////////////////////////////////////////////////////////
//														 //
//														 //
//														 //
///////////////////////////////////////////////////////////

//---------------------------------------------------------
#endif // #ifndef HEADER_INCLUDED__param_scale_H

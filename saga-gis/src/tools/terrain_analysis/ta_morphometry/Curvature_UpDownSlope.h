/**********************************************************
* Version $Id: Curvature_UpDownSlope.cpp 911 2011-11-11 11:11:11Z oconrad $
*********************************************************/

///////////////////////////////////////////////////////////
//                                                       //
//                         SAGA                          //
//                                                       //
//      System for Automated Geoscientific Analyses      //
//                                                       //
//                     Tool Library                      //
//                    ta_morphometry                     //
//                                                       //
//-------------------------------------------------------//
//                                                       //
//                Curvature_UpDownSlope.h                //
//                                                       //
//                 Copyright (C) 2015 by                 //
//                    Paolo Gandelli                     //
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
//    e-mail:     gandelli.paolo@gmail.com               //
//                                                       //
//    contact:    Paolo Gandelli                         //
//                Engineering Geology and Geomorphology  //
//                Department of Earth Sciences           //
//                University of Firenze                  //
//                Italy                                  //
//                                                       //
///////////////////////////////////////////////////////////

//---------------------------------------------------------


///////////////////////////////////////////////////////////
//														 //
//														 //
//														 //
///////////////////////////////////////////////////////////

//---------------------------------------------------------
#ifndef HEADER_INCLUDED__Curvature_UpDownSlope_H
#define HEADER_INCLUDED__Curvature_UpDownSlope_H


///////////////////////////////////////////////////////////
//														 //
//														 //
//														 //
///////////////////////////////////////////////////////////

//---------------------------------------------------------
#include "MLB_Interface.h"


///////////////////////////////////////////////////////////
//														 //
//														 //
//														 //
///////////////////////////////////////////////////////////

//---------------------------------------------------------
class CCurvature_UpDownSlope : public CSG_Tool_Grid
{
public:
	CCurvature_UpDownSlope(void);


protected:

	virtual bool		On_Execute(void);


private:

	double				m_Weighting;

	CSG_Grid			*m_pDEM, *m_pC_Local, *m_pC_Up, *m_pC_Up_Local, *m_pC_Down, *m_pC_Down_Local, m_Weights;


	double				Get_Local				(int x, int y);

	bool				Get_Upslope				(int x, int y);
	bool				Get_Downslope			(int x, int y);

	bool				Get_Flow_Proportions	(int x, int y, double Proportion[8]);

};


///////////////////////////////////////////////////////////
//														 //
//														 //
//														 //
///////////////////////////////////////////////////////////

//---------------------------------------------------------
#endif // #ifndef HEADER_INCLUDED__Curvature_UpDownSlope_H

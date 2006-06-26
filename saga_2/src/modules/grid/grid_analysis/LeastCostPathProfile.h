
///////////////////////////////////////////////////////////
//                                                       //
//                         SAGA                          //
//                                                       //
//      System for Automated Geoscientific Analyses      //
//                                                       //
//                    Module Library:                    //
//                   Terrain_Analysis                    //
//                                                       //
//-------------------------------------------------------//
//                                                       //
//                 LeastCostPathProfile.h                //
//                                                       //
//                 Copyright (C) 2004 by                 //
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
//    contact:    Olaf Conrad                            //
//                Institute of Geography                 //
//                University of Goettingen               //
//                Goldschmidtstr. 5                      //
//                37077 Goettingen                       //
//                Germany                                //
//                                                       //
//    e-mail:     oconrad@saga-gis.org                   //
//                                                       //
///////////////////////////////////////////////////////////

//---------------------------------------------------------


///////////////////////////////////////////////////////////
//														 //
//														 //
//														 //
///////////////////////////////////////////////////////////

//---------------------------------------------------------
#ifndef HEADER_INCLUDED__LeastCostPathProfile_H
#define HEADER_INCLUDED__LeastCostPathProfile_H

//---------------------------------------------------------
#include "MLB_Interface.h"


///////////////////////////////////////////////////////////
//														 //
//														 //
//														 //
///////////////////////////////////////////////////////////

//---------------------------------------------------------
class CLeastCostPathProfile : public CModule_Grid_Interactive
{
public:
	CLeastCostPathProfile(void);
	virtual ~CLeastCostPathProfile(void);

	virtual const char *	Get_MenuPath	(void)	{	return( _TL("R:Cost Analysis") );	}

protected:

	virtual bool				On_Execute(void);
	virtual bool				On_Execute_Position(CGEO_Point ptWorld, TModule_Interactive_Mode Mode);


private:

	CShapes						*m_pPoints, *m_pLine;

	CGrid						*m_pDEM;

	CParameter_Grid_List		*m_pValues;


	bool						Set_Profile(TGEO_Point ptWorld);
	void						Set_Profile(int x, int y);

	bool						Add_Point(int x, int y);

	void						getNextCell(CGrid *g, int iX, int iY, int &iNextX, int &iNextY);
};


///////////////////////////////////////////////////////////
//														 //
//														 //
//														 //
///////////////////////////////////////////////////////////

//---------------------------------------------------------
#endif // #ifndef HEADER_INCLUDED__LeastCostPathProfile_H

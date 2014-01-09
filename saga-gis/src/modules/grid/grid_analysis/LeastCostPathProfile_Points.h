/**********************************************************
 * Version $Id$
 *********************************************************/

///////////////////////////////////////////////////////////
//                                                       //
//                         SAGA                          //
//                                                       //
//      System for Automated Geoscientific Analyses      //
//                                                       //
//                    Module Library:                    //
//                     grid_analysis                     //
//                                                       //
//-------------------------------------------------------//
//                                                       //
//             LeastCostPathProfile_Points.h             //
//                                                       //
//              Copyright (C) 2004-2010 by               //
//      Olaf Conrad, Victor Olaya & Volker Wichmann      //
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
//    e-mail:     wichmann@laserdata                     //
//                                                       //
//    contact:    Volker Wichmann                        //
//                LASERDATA GmbH                         //
//                Management and analysis of             //
//                laserscanning data                     //
//                Innsbruck, Austria                     //
//                                                       //
///////////////////////////////////////////////////////////

//---------------------------------------------------------


///////////////////////////////////////////////////////////
//														 //
//														 //
//														 //
///////////////////////////////////////////////////////////

//---------------------------------------------------------
#ifndef HEADER_INCLUDED__LeastCostPathProfile_Points_H
#define HEADER_INCLUDED__LeastCostPathProfile_Points_H

//---------------------------------------------------------
#include "MLB_Interface.h"


///////////////////////////////////////////////////////////
//														 //
//														 //
//														 //
///////////////////////////////////////////////////////////

//---------------------------------------------------------
class CLeastCostPathProfile_Points : public CSG_Module_Grid
{
public:
	CLeastCostPathProfile_Points(void);
	virtual ~CLeastCostPathProfile_Points(void);

	virtual CSG_String		Get_MenuPath	(void)	{	return( _TL("R:Cost Analysis") );	}

protected:

	virtual bool				On_Execute(void);


private:

	CSG_Grid					*m_pDEM;

	CSG_Parameter_Grid_List		*m_pValues;


	void						Set_Profile(int x, int y, CSG_Shapes *pPoints, CSG_Shapes *pLine);

	bool						Add_Point(int x, int y, CSG_Shapes *pPoints, CSG_Shapes *pLine);

	void						getNextCell(CSG_Grid *g, int iX, int iY, int &iNextX, int &iNextY);
};


///////////////////////////////////////////////////////////
//														 //
//														 //
//														 //
///////////////////////////////////////////////////////////

//---------------------------------------------------------
#endif // #ifndef HEADER_INCLUDED__LeastCostPathProfile_Points_H

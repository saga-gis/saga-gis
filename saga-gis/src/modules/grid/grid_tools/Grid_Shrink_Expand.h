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
//                      Grid_Tools                       //
//                                                       //
//-------------------------------------------------------//
//                                                       //
//                 Grid_Shrink_Expand.h                  //
//                                                       //
//                 Copyright (C) 2011 by                 //
//            Volker Wichmann and Olaf Conrad            //
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
//                                                       //
//														 //
///////////////////////////////////////////////////////////

//---------------------------------------------------------
#ifndef HEADER_INCLUDED__Grid_Shrink_Expand_H
#define HEADER_INCLUDED__Grid_Shrink_Expand_H


///////////////////////////////////////////////////////////
//														 //
//                                                       //
//														 //
///////////////////////////////////////////////////////////

//---------------------------------------------------------
#include "MLB_Interface.h"


///////////////////////////////////////////////////////////
//														 //
//                                                       //
//														 //
///////////////////////////////////////////////////////////

//---------------------------------------------------------
typedef enum ESG_Method_Key
{
	METHOD_KEY_MIN		= 0,
	METHOD_KEY_MAX,
	METHOD_KEY_MEAN,
	METHOD_KEY_MAJORITY,
	METHOD_KEY_Count
}
TSG_Method_Key;

//---------------------------------------------------------
const SG_Char	gSG_Method_Key_Names[METHOD_KEY_Count][32]	=
{
	SG_T("min"),
	SG_T("max"),
	SG_T("mean"),
	SG_T("majority")
};


//---------------------------------------------------------
class CGrid_Shrink_Expand : public CSG_Module_Grid
{
public:
	CGrid_Shrink_Expand(void);


protected:

	virtual bool			On_Execute		(void);

	virtual int				On_Parameters_Enable(CSG_Parameters *pParameters, CSG_Parameter *pParameter);

private:

	int						m_Radius, m_Threshold;

	CSG_Grid				m_Kernel, *m_pInput;

	CSG_Class_Statistics	m_Majority;


	double					Get_Majority	(int x, int y);

};


///////////////////////////////////////////////////////////
//														 //
//                                                       //
//														 //
///////////////////////////////////////////////////////////

//---------------------------------------------------------
#endif // #ifndef HEADER_INCLUDED__Grid_Shrink_Expand_H

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
//                   pointcloud_tools                    //
//                                                       //
//-------------------------------------------------------//
//                                                       //
//                       pc_cut.h                        //
//                                                       //
//                 Copyright (C) 2009 by                 //
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
#ifndef HEADER_INCLUDED__PC_Cut_H
#define HEADER_INCLUDED__PC_Cut_H

//---------------------------------------------------------
#include "MLB_Interface.h"


///////////////////////////////////////////////////////////
//														 //
//														 //
//														 //
///////////////////////////////////////////////////////////

//---------------------------------------------------------
class CPC_Cut : public CSG_Module
{
public:
	CPC_Cut(void);

	virtual CSG_String			Get_MenuPath			(void)	{	return( _TL("R:Tools") );	}

	static bool					Get_Cut					(CSG_PointCloud *pPoints, CSG_PointCloud *pCut, const CSG_Rect &Extent, bool bInverse);
	static bool					Get_Cut					(CSG_PointCloud *pPoints, CSG_PointCloud *pCut, CSG_Shapes *pPolygons, bool bInverse);


protected:

	virtual bool				On_Execute				(void);


private:

	static bool					Contains				(CSG_Shapes *pPolygons, double x, double y);

};


///////////////////////////////////////////////////////////
//														 //
//														 //
//														 //
///////////////////////////////////////////////////////////

//---------------------------------------------------------
class CPC_Cut_Interactive : public CSG_Module_Interactive
{
public:
	CPC_Cut_Interactive(void);

	virtual CSG_String			Get_MenuPath			(void)	{	return( _TL("R:Tools") );	}


protected:

	virtual bool				On_Execute				(void);
	virtual bool				On_Execute_Position		(CSG_Point ptWorld, TSG_Module_Interactive_Mode Mode);


private:

	CSG_Point					m_ptDown;

	CSG_PointCloud				*m_pPoints, *m_pCut;

	CSG_Shapes					*m_pAOI;

	bool						m_bAOIBox, m_bAdd, m_bInverse;

};


///////////////////////////////////////////////////////////
//														 //
//														 //
//														 //
///////////////////////////////////////////////////////////

//---------------------------------------------------------
#endif // #ifndef HEADER_INCLUDED__PC_Cut_H

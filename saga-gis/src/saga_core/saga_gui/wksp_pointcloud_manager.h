
///////////////////////////////////////////////////////////
//                                                       //
//                         SAGA                          //
//                                                       //
//      System for Automated Geoscientific Analyses      //
//                                                       //
//                    User Interface                     //
//                                                       //
//                    Program: SAGA                      //
//                                                       //
//-------------------------------------------------------//
//                                                       //
//               WKSP_PointCloud_Manager.h               //
//                                                       //
//          Copyright (C) 2005 by Olaf Conrad            //
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
//                University of Hamburg                  //
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
#ifndef _HEADER_INCLUDED__SAGA_GUI__WKSP_PointCloud_Manager_H
#define _HEADER_INCLUDED__SAGA_GUI__WKSP_PointCloud_Manager_H


///////////////////////////////////////////////////////////
//														 //
//														 //
//														 //
///////////////////////////////////////////////////////////

//---------------------------------------------------------
#include "wksp_base_manager.h"


///////////////////////////////////////////////////////////
//														 //
//														 //
//														 //
///////////////////////////////////////////////////////////

//---------------------------------------------------------
class CWKSP_PointCloud_Manager : public CWKSP_Base_Manager
{
public:
	CWKSP_PointCloud_Manager(void);
	virtual ~CWKSP_PointCloud_Manager(void);

	virtual TWKSP_Item			Get_Type		(void)		{	return( WKSP_ITEM_PointCloud_Manager );	}

	virtual wxString			Get_Name		(void);
	virtual wxString			Get_Description	(void);

	virtual wxMenu *			Get_Menu		(void);

	virtual bool				On_Command		(int Cmd_ID);

	class CWKSP_PointCloud *	Get_PointCloud	(int i)		{	return( (class CWKSP_PointCloud *)Get_Item(i) );	}
	class CWKSP_PointCloud *	Get_PointCloud	(class CSG_PointCloud *pPointCloud);

	bool						Exists			(class CSG_PointCloud *pPointCloud);
	class CWKSP_PointCloud *	Add				(class CSG_PointCloud *pPointCloud);
	class CSG_PointCloud *		Get_byFileName	(const wxChar *File_Name);

	bool						Update			(class CSG_PointCloud *pPointCloud, class CSG_Parameters *pParameters);
	bool						Update_Views	(class CSG_PointCloud *pPointCloud);
	bool						Show			(class CSG_PointCloud *pPointCloud, int Map_Mode);
	bool						asImage			(class CSG_PointCloud *pPointCloud, class CSG_Grid *pImage);

	bool						Get_Colors		(class CSG_PointCloud *pPointCloud, class CSG_Colors *pColors);
	bool						Set_Colors		(class CSG_PointCloud *pPointCloud, class CSG_Colors *pColors);


private:

};


///////////////////////////////////////////////////////////
//														 //
//														 //
//														 //
///////////////////////////////////////////////////////////

//---------------------------------------------------------
#endif // #ifndef _HEADER_INCLUDED__SAGA_GUI__WKSP_PointCloud_Manager_H

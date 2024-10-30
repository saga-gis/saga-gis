
///////////////////////////////////////////////////////////
//                                                       //
//                         SAGA                          //
//                                                       //
//      System for Automated Geoscientific Analyses      //
//                                                       //
//                     Tool Library                      //
//                       io_pdal                         //
//                                                       //
//-------------------------------------------------------//
//                                                       //
//                   pdal_to_grid.h                      //
//                                                       //
//                 Copyrights (c) 2024                   //
//                     Olaf Conrad                       //
//                   Volker Wichmann                     //
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
//                University of Hamburg                  //
//                Germany                                //
//                                                       //
///////////////////////////////////////////////////////////

//---------------------------------------------------------
#ifndef HEADER_INCLUDED__pdal_to_grid_H
#define HEADER_INCLUDED__pdal_to_grid_H


///////////////////////////////////////////////////////////
//                                                       //
//                                                       //
//                                                       //
///////////////////////////////////////////////////////////

//---------------------------------------------------------
#include <saga_api/saga_api.h>


///////////////////////////////////////////////////////////
//                                                       //
//                                                       //
//                                                       //
///////////////////////////////////////////////////////////

//---------------------------------------------------------
class CPDAL_to_Grid : public CSG_Tool
{
public:
	CPDAL_to_Grid(void);

	virtual CSG_String			Get_MenuPath			(void)  { return( _TL("A:File|Grid|Import") );  }

	virtual bool				do_Sync_Projections		(void)  const { return( false );  }


protected:

	virtual int					On_Parameter_Changed	(CSG_Parameters *pParameters, CSG_Parameter *pParameter);
	virtual int					On_Parameters_Enable	(CSG_Parameters *pParameters, CSG_Parameter *pParameter);

	virtual bool				On_Execute				(void);


private:

	int							m_Aggregation = 1; // last

	CSG_Grid					*m_pGrid = NULL, *m_pCount = NULL;

	CSG_Parameters_Grid_Target	m_Grid_Target;


	bool						_Find_Class				(const CSG_Array_Int &Classes, int ID);

	void						_Add_Point				(double x, double y, double z);
	bool						_Read_Points            (const CSG_String &File, const CSG_Array_Int &Classes, bool bStream);

	static bool					_Get_Extent				(const CSG_Strings &Files, CSG_Rect &Extent, bool bStream);
	static bool					_Get_Extent				(const CSG_String  &File , CSG_Rect &Extent, bool bStream);

};


///////////////////////////////////////////////////////////
//                                                       //
//                                                       //
//                                                       //
///////////////////////////////////////////////////////////

//---------------------------------------------------------
#endif // #ifndef HEADER_INCLUDED__pdal_to_grid_H

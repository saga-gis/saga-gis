
///////////////////////////////////////////////////////////
//                                                       //
//                         SAGA                          //
//                                                       //
//      System for Automated Geoscientific Analyses      //
//                                                       //
//                     Tool Library                      //
//                    io_webservices                     //
//                                                       //
//-------------------------------------------------------//
//                                                       //
//                  bayern_opendata.h                    //
//                                                       //
//                 Copyrights (C) 2025                   //
//                     Olaf Conrad                       //
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
#ifndef HEADER_INCLUDED__bayern_opendata_H
#define HEADER_INCLUDED__bayern_opendata_H


///////////////////////////////////////////////////////////
//														 //
//														 //
//														 //
///////////////////////////////////////////////////////////

//---------------------------------------------------------
#include "sg_curl.h"


///////////////////////////////////////////////////////////
//														 //
//														 //
//														 //
///////////////////////////////////////////////////////////

//---------------------------------------------------------
class CBayernOpenData_DGM1 : public CSG_Tool
{
public:
	CBayernOpenData_DGM1(void);

	virtual CSG_String			Get_MenuPath			(void)	{	return( "OpenData, Bayern" );	}

//	virtual bool				do_Sync_Projections		(void)	const	{	return( false  );	}

	virtual bool				On_Before_Execution		(void);
	virtual bool				On_After_Execution		(void);


protected:

	virtual int                 On_Parameter_Changed	(CSG_Parameters *pParameters, CSG_Parameter *pParameter);
	virtual int					On_Parameters_Enable	(CSG_Parameters *pParameters, CSG_Parameter *pParameter);

	virtual bool				On_Execute				(void);


private:

	CSG_String					m_ServerPath, m_VRT_Name;

	CSG_Parameters_CRSPicker	m_CRS;


	bool						Provide_Tiles			(const CSG_String &Directory, CSG_Rect Extent);
	int							Provide_Tile			(const CSG_String &Directory, int Col, int Row);

	bool						Update_VRT				(const CSG_String &Directory);

};


///////////////////////////////////////////////////////////
//														 //
//														 //
//														 //
///////////////////////////////////////////////////////////

//---------------------------------------------------------
#endif // #ifndef HEADER_INCLUDED__bayern_opendata_H

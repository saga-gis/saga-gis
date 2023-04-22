
///////////////////////////////////////////////////////////
//                                                       //
//                         SAGA                          //
//                                                       //
//      System for Automated Geoscientific Analyses      //
//                                                       //
//                     Tool Library                      //
//                       Tutorial                        //
//                                                       //
//-------------------------------------------------------//
//                                                       //
//                  Spectral_Profile.h                   //
//                                                       //
//                 Copyright (C) 2023 by                 //
//                      Olaf Conrad                      //
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
#ifndef HEADER_INCLUDED__Spectral_Profile_H
#define HEADER_INCLUDED__Spectral_Profile_H


///////////////////////////////////////////////////////////
//														 //
//														 //
//														 //
///////////////////////////////////////////////////////////

//---------------------------------------------------------
#include <saga_api/saga_api.h>


///////////////////////////////////////////////////////////
//														 //
//														 //
//														 //
///////////////////////////////////////////////////////////

//---------------------------------------------------------
class CSpectral_Profile : public CSG_Tool
{
public:
	friend class CSpectral_Profile_Interactive;

	CSpectral_Profile(void);

	virtual CSG_String		Get_MenuPath			(void)	{	return( _TL("Tools") );	}


protected:

	virtual int 			On_Parameter_Changed	(CSG_Parameters *pParameters, CSG_Parameter *pParameter);

	virtual bool			On_Execute				(void);

	bool					Initialize				(void);

	bool					Add_Profile				(const CSG_Point &Point, bool bMultiple);

	bool					Update_Profile			(bool bUpdate = false);

	const CSG_Rect &		Get_Extent				(void) const { return( m_Extent ); }


private:

	TSG_Grid_Resampling		m_Resampling{GRID_RESAMPLING_BSpline};

	CSG_Rect				m_Extent;

	CSG_Parameter_Grid_List	*m_pBands{NULL};

	CSG_Table				*m_pProfile{NULL};

};


///////////////////////////////////////////////////////////
//														 //
///////////////////////////////////////////////////////////

//---------------------------------------------------------
class CSpectral_Profile_Interactive : public CSG_Tool_Interactive
{
public:
	CSpectral_Profile_Interactive(void);

	virtual CSG_String		Get_MenuPath			(void)	{	return( _TL("Tools") );	}


protected:

	virtual int 			On_Parameter_Changed	(CSG_Parameters *pParameters, CSG_Parameter *pParameter);
	virtual int 			On_Parameters_Enable	(CSG_Parameters *pParameters, CSG_Parameter *pParameter);

	virtual bool			On_Execute				(void);

	virtual bool			On_Execute_Position		(CSG_Point ptWorld, TSG_Tool_Interactive_Mode Mode);


private:

	bool					m_bMultiple{false};

	CSG_Shapes				*m_pLocation{NULL}, *m_pSamples{NULL};

	CSpectral_Profile		m_Profile;

};


///////////////////////////////////////////////////////////
//														 //
//														 //
//														 //
///////////////////////////////////////////////////////////

//---------------------------------------------------------
#endif // #ifndef HEADER_INCLUDED__Spectral_Profile_H

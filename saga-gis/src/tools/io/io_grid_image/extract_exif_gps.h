
///////////////////////////////////////////////////////////
//                                                       //
//                         SAGA                          //
//                                                       //
//      System for Automated Geoscientific Analyses      //
//                                                       //
//                     Tool Library                      //
//                       image_io                        //
//                                                       //
//-------------------------------------------------------//
//                                                       //
//                  extract_exif_gps.h                   //
//                                                       //
//                 Copyright (C) 2025 by                 //
//                  Justus Spitzmueller                  //
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
//                                                       //
///////////////////////////////////////////////////////////

//---------------------------------------------------------
#pragma once

#if !HAVE_EXIV2
	#define new_CExtract_EXIF_GPS TLB_INTERFACE_SKIP_TOOL
#else
	#define new_CExtract_EXIF_GPS new CExtract_EXIF_GPS


///////////////////////////////////////////////////////////
//														 //
//														 //
//														 //
///////////////////////////////////////////////////////////

//---------------------------------------------------------
#include <saga_api/saga_api.h>
#include <exiv2/exiv2.hpp>


///////////////////////////////////////////////////////////
//														 //
//														 //
//														 //
///////////////////////////////////////////////////////////

//---------------------------------------------------------
class CExtract_EXIF_GPS : public CSG_Tool
{
public:
	CExtract_EXIF_GPS(void);

	virtual CSG_String			Get_MenuPath			(void)	{	return( _TL("Import") );	}
	
	virtual bool				On_Before_Execution		(void);
	virtual bool				On_After_Execution		(void);

protected:

	virtual int					On_Parameter_Changed	(CSG_Parameters *pParameters, CSG_Parameter *pParameter);
	virtual int					On_Parameters_Enable	(CSG_Parameters *pParameters, CSG_Parameter *pParameter);

	virtual bool				On_Execute				(void);

private:

	bool 						Get_Coordinate			( double& Coordinate, const Exiv2::Value& Value, const std::string& Ref );
	double 						Convert_Rational 		( const Exiv2::Value& Value, const size_t Position );
	
	CSG_Parameters_CRSPicker 	m_CRS;

};


///////////////////////////////////////////////////////////
//														 //
//														 //
//														 //
///////////////////////////////////////////////////////////

#endif // HAVE_EXIV2

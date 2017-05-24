/**********************************************************
 * Version $Id: bmp_export.h 1921 2014-01-09 10:24:11Z oconrad $
 *********************************************************/

///////////////////////////////////////////////////////////
//                                                       //
//                         SAGA                          //
//                                                       //
//    System for an Automated Geo-Scientific Analysis    //
//                                                       //
//                     Tool Library                      //
//                        Grid_IO                        //
//                                                       //
//-------------------------------------------------------//
//                                                       //
//                      bmp_export.h                     //
//                                                       //
//                 Copyright (C) 2005 by                 //
//                      Olaf Conrad                      //
//                                                       //
//-------------------------------------------------------//
//                                                       //
// This file is part of 'SAGA - System for an Automated  //
// Geo-Scientific Analysis'. SAGA is free software; you  //
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
//    e-mail:     oconrad@gwdg.de                        //
//                                                       //
//    contact:    Olaf Conrad                            //
//                Institute of Geography                 //
//                University of Goettingen               //
//                Goldschmidtstr. 5                      //
//                37077 Goettingen                       //
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
#ifndef HEADER_INCLUDED__bmp_export_H
#define HEADER_INCLUDED__bmp_export_H


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
class CBMP_Export : public CSG_Tool_Grid
{
public:
	CBMP_Export(void);
	virtual ~CBMP_Export(void);

	virtual CSG_String		Get_MenuPath	(void)		{	return( _TL("Export") );	}


protected:

	virtual bool			On_Execute		(void);


private:

	void					Write_WORD		(FILE *Stream, WORD  Value);
	void					Write_DWORD		(FILE *Stream, DWORD Value);
	void					Write_LONG		(FILE *Stream, long  Value);

};


///////////////////////////////////////////////////////////
//														 //
//														 //
//														 //
///////////////////////////////////////////////////////////

//---------------------------------------------------------
#endif // #ifndef HEADER_INCLUDED__bmp_export_H

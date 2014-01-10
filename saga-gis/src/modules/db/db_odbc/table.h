/**********************************************************
 * Version $Id: table.h 1922 2014-01-09 10:28:46Z oconrad $
 *********************************************************/

///////////////////////////////////////////////////////////
//                                                       //
//                         SAGA                          //
//                                                       //
//      System for Automated Geoscientific Analyses      //
//                                                       //
//                    Module Library:                    //
//                        db_odbc                        //
//                                                       //
//-------------------------------------------------------//
//                                                       //
//                       Table.h                         //
//                                                       //
//                 Copyright (C) 2005 by                 //
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
// 51 Franklin Street, 5th Floor, Boston, MA 02110-1301, //
// USA.                                                  //
//                                                       //
//-------------------------------------------------------//
//                                                       //
//    e-mail:     oconrad@saga-gis.org                   //
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
#ifndef HEADER_INCLUDED__Table_H
#define HEADER_INCLUDED__Table_H


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
class CTable_List : public CSG_ODBC_Module
{
public:
	CTable_List(void);

	virtual CSG_String			Get_MenuPath			(void)	{	return( SG_T("R:Tools") );	}


protected:

	virtual bool				On_Execute				(void);

};


///////////////////////////////////////////////////////////
//														 //
///////////////////////////////////////////////////////////

//---------------------------------------------------------
class CTable_Info : public CSG_ODBC_Module
{
public:
	CTable_Info(void);

	virtual CSG_String			Get_MenuPath			(void)	{	return( SG_T("R:Tools") );	}


protected:

	virtual void				On_Connection_Changed	(CSG_Parameters *pParameters);

	virtual bool				On_Execute				(void);

};


///////////////////////////////////////////////////////////
//														 //
///////////////////////////////////////////////////////////

//---------------------------------------------------------
class CTable_Load : public CSG_ODBC_Module
{
public:
	CTable_Load(void);

	virtual CSG_String			Get_MenuPath			(void)	{	return( SG_T("R:Tables") );	}


protected:

	virtual void				On_Connection_Changed	(CSG_Parameters *pParameters);

	virtual bool				On_Execute				(void);

};


///////////////////////////////////////////////////////////
//														 //
///////////////////////////////////////////////////////////

//---------------------------------------------------------
class CTable_Save : public CSG_ODBC_Module
{
public:
	CTable_Save(void);

	virtual CSG_String			Get_MenuPath			(void)	{	return( SG_T("R:Tables") );	}


protected:

	virtual int					On_Parameter_Changed	(CSG_Parameters *pParameters, CSG_Parameter *pParameter);

	virtual bool				On_Execute				(void);

};


///////////////////////////////////////////////////////////
//														 //
///////////////////////////////////////////////////////////

//---------------------------------------------------------
class CTable_Drop : public CSG_ODBC_Module
{
public:
	CTable_Drop(void);

	virtual CSG_String			Get_MenuPath			(void)	{	return( SG_T("R:Tables") );	}


protected:

	virtual void				On_Connection_Changed	(CSG_Parameters *pParameters);

	virtual bool				On_Execute				(void);

};


///////////////////////////////////////////////////////////
//														 //
///////////////////////////////////////////////////////////

//---------------------------------------------------------
class CTable_Query : public CSG_ODBC_Module
{
public:
	CTable_Query(void);

	virtual CSG_String			Get_MenuPath			(void)	{	return( SG_T("R:Tables") );	}


protected:

	virtual bool				On_Execute				(void);

};


///////////////////////////////////////////////////////////
//														 //
//														 //
//														 //
///////////////////////////////////////////////////////////

//---------------------------------------------------------
#endif // #ifndef HEADER_INCLUDED__Table_H

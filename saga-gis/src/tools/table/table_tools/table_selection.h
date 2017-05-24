/**********************************************************
 * Version $Id: table_selection.h 1246 2011-11-25 13:42:38Z oconrad $
 *********************************************************/

///////////////////////////////////////////////////////////
//                                                       //
//                         SAGA                          //
//                                                       //
//      System for Automated Geoscientific Analyses      //
//                                                       //
//                     Tool Library                      //
//                     table_tools                       //
//                                                       //
//-------------------------------------------------------//
//                                                       //
//                   table_selection.h                   //
//                                                       //
//                 Copyright (C) 2013 by                 //
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

///////////////////////////////////////////////////////////
//                                                       //												
//                                                       //												
//                                                       //
///////////////////////////////////////////////////////////

//---------------------------------------------------------
#ifndef HEADER_INCLUDED__table_selection_H
#define HEADER_INCLUDED__table_selection_H


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
class CSelection_Copy : public CSG_Tool
{
public:
	CSelection_Copy(void);

	virtual CSG_String			Get_MenuPath	(void)	{	return( _TL("A:Table|Selection") );	}


protected:

	virtual int					On_Parameters_Enable	(CSG_Parameters *pParameters, CSG_Parameter *pParameter);

	virtual bool				On_Execute		(void);

};


///////////////////////////////////////////////////////////
//														 //
///////////////////////////////////////////////////////////

//---------------------------------------------------------
class CSelection_Delete : public CSG_Tool
{
public:
	CSelection_Delete(void);

	virtual CSG_String			Get_MenuPath	(void)	{	return( _TL("A:Table|Selection") );	}


protected:

	virtual bool				On_Execute		(void);

};


///////////////////////////////////////////////////////////
//														 //
///////////////////////////////////////////////////////////

//---------------------------------------------------------
class CSelection_Invert : public CSG_Tool
{
public:
	CSelection_Invert(void);

	virtual CSG_String			Get_MenuPath	(void)	{	return( _TL("A:Table|Selection") );	}


protected:

	virtual bool				On_Execute		(void);

};


///////////////////////////////////////////////////////////
//														 //
///////////////////////////////////////////////////////////

//---------------------------------------------------------
class CSelect_Numeric : public CSG_Tool  
{
public:
	CSelect_Numeric(void);

	virtual CSG_String			Get_MenuPath	(void)	{	return( _TL("A:Table|Selection") );	}


protected:

	virtual bool				On_Execute		(void);

};


///////////////////////////////////////////////////////////
//														 //
///////////////////////////////////////////////////////////

//---------------------------------------------------------
class CSelect_String : public CSG_Tool
{
public:
	CSelect_String(void);

	virtual CSG_String			Get_MenuPath	(void)	{	return( _TL("A:Table|Selection") );	}


protected:

	virtual bool				On_Execute		(void);


private:

	bool						m_Case;

	int							m_Field, m_Compare;

	CSG_String					m_Expression;


	bool						Do_Compare		(const SG_Char *Value);

	bool						Do_Select		(CSG_Table_Record *pRecord);

};


///////////////////////////////////////////////////////////
//														 //
//														 //
//														 //
///////////////////////////////////////////////////////////

//---------------------------------------------------------
#endif // #ifndef HEADER_INCLUDED__table_selection_H

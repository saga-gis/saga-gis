/**********************************************************
 * Version $Id: Join_Tables.h 911 2011-02-14 16:38:15Z reklov_w $
 *********************************************************/

///////////////////////////////////////////////////////////
//                                                       //
//                         SAGA                          //
//                                                       //
//      System for Automated Geoscientific Analyses      //
//                                                       //
//                    Module Library:                    //
//                     Shapes_Tools                      //
//                                                       //
//-------------------------------------------------------//
//                                                       //
//                     Join_Tables.h                     //
//                                                       //
//                 Copyright (C) 2003 by                 //
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
//                University of Goettingen               //
//                Goldschmidtstr. 5                      //
//                37077 Goettingen                       //
//                Germany                                //
//                                                       //
///////////////////////////////////////////////////////////

//---------------------------------------------------------


///////////////////////////////////////////////////////////
//														 //
//                                                       //
//														 //
///////////////////////////////////////////////////////////

//---------------------------------------------------------
#ifndef HEADER_INCLUDED__Join_Tables_H
#define HEADER_INCLUDED__Join_Tables_H

//---------------------------------------------------------
#include "MLB_Interface.h"


///////////////////////////////////////////////////////////
//														 //
//														 //
//														 //
///////////////////////////////////////////////////////////

//---------------------------------------------------------
class CJoin_Tables_Base : public CSG_Module  
{
protected:

	void						Initialise				(void);

	virtual bool				On_Execute				(void);


private:

	bool						m_bCmpNumeric;

	int							m_id_A, m_id_B, m_off_Field;

	CSG_Table					*m_pTable_A, *m_pTable_B;

	int							Cmp_Keys				(CSG_Table_Record *pA, CSG_Table_Record *pB);

	void						Add_Attributes			(CSG_Table_Record *pA, CSG_Table_Record *pB);

};

//---------------------------------------------------------
class CJoin_Tables : public CJoin_Tables_Base  
{
public:
	CJoin_Tables(void);

};

//---------------------------------------------------------
class CJoin_Tables_Shapes : public CJoin_Tables_Base  
{
public:
	CJoin_Tables_Shapes(void);

	virtual CSG_String			Get_MenuPath			(void)	{	return( _TL("A:Shapes|Table") );	}

};


///////////////////////////////////////////////////////////
//														 //
///////////////////////////////////////////////////////////

//---------------------------------------------------------
class CTable_Append_Rows : public CSG_Module  
{
public:
	CTable_Append_Rows(void);


protected:

	virtual bool				On_Execute				(void);

};


///////////////////////////////////////////////////////////
//														 //
//														 //
//														 //
///////////////////////////////////////////////////////////

//---------------------------------------------------------
#endif // #ifndef HEADER_INCLUDED__Join_Tables_H


///////////////////////////////////////////////////////////
//                                                       //
//                         SAGA                          //
//                                                       //
//      System for Automated Geoscientific Analyses      //
//                                                       //
//           Application Programming Interface           //
//                                                       //
//                  Library: SAGA_API                    //
//                                                       //
//-------------------------------------------------------//
//                                                       //
//                  module_library.h                     //
//                                                       //
//          Copyright (C) 2006 by Olaf Conrad            //
//                                                       //
//-------------------------------------------------------//
//                                                       //
// This file is part of 'SAGA - System for Automated     //
// Geoscientific Analyses'.                              //
//                                                       //
// This library is free software; you can redistribute   //
// it and/or modify it under the terms of the GNU Lesser //
// General Public License as published by the Free       //
// Software Foundation, version 2.1 of the License.      //
//                                                       //
// This library is distributed in the hope that it will  //
// be useful, but WITHOUT ANY WARRANTY; without even the //
// implied warranty of MERCHANTABILITY or FITNESS FOR A  //
// PARTICULAR PURPOSE. See the GNU Lesser General Public //
// License for more details.                             //
//                                                       //
// You should have received a copy of the GNU Lesser     //
// General Public License along with this program; if    //
// not, write to the Free Software Foundation, Inc.,     //
// 59 Temple Place - Suite 330, Boston, MA 02111-1307,   //
// USA.                                                  //
//                                                       //
//-------------------------------------------------------//
//                                                       //
//    contact:    Olaf Conrad                            //
//                Institute of Geography                 //
//                University of Goettingen               //
//                Goldschmidtstr. 5                      //
//                37077 Goettingen                       //
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
#ifndef HEADER_INCLUDED__SAGA_API__module_library_H
#define HEADER_INCLUDED__SAGA_API__module_library_H


///////////////////////////////////////////////////////////
//														 //
//														 //
//														 //
///////////////////////////////////////////////////////////

//---------------------------------------------------------
#include "module.h"


///////////////////////////////////////////////////////////
//														 //
//														 //
//														 //
///////////////////////////////////////////////////////////

//---------------------------------------------------------
class SAGA_API_DLL_EXPORT CSG_Module_Library
{
public:
	CSG_Module_Library(void);
	virtual ~CSG_Module_Library(void);

									CSG_Module_Library	(const SG_Char *File_Name);
	bool							Create				(const SG_Char *File_Name);

	bool							Destroy				(void);

	bool							is_Valid			(void)		{	return( Get_Count() > 0 );	}

	const CSG_String &				Get_File_Name		(void)		{	return( m_File_Name );	}

	const SG_Char *					Get_Info			(int Type);

	CSG_String						Get_Name			(void);
	CSG_String						Get_Description		(void);
	CSG_String						Get_Author			(void);
	CSG_String						Get_Version			(void);
	CSG_String						Get_Menu			(void);
	CSG_String						Get_Summary			(bool bHTML = false);

	int								Get_Count			(void)		{	return( m_pInterface ? m_pInterface->Get_Count() : 0 );	}

	CSG_Module *					Get_Module			(int i)		{	return( i >= 0 && i < Get_Count() ? m_pInterface->Get_Module(i) : NULL );	}
	CSG_Module_Grid *				Get_Module_Grid		(int i);
	CSG_Module_Interactive *		Get_Module_I		(int i);
	CSG_Module_Grid_Interactive *	Get_Module_Grid_I	(int i);

	CSG_String						Get_Menu			(int i);


private:

	CSG_String						m_File_Name;

	CSG_Module_Library_Interface	*m_pInterface;

	class wxDynamicLibrary			*m_pLibrary;


	void							_On_Construction	(void);

};


///////////////////////////////////////////////////////////
//														 //
//														 //
//														 //
///////////////////////////////////////////////////////////

//---------------------------------------------------------
#endif // #ifndef HEADER_INCLUDED__SAGA_API__module_library_H

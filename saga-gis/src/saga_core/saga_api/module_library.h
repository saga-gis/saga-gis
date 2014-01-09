/**********************************************************
 * Version $Id$
 *********************************************************/

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
// 51 Franklin Street, 5th Floor, Boston, MA 02110-1301, //
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
enum
{
	SG_SUMMARY_FMT_FLAT	= 0,
	SG_SUMMARY_FMT_FLAT_NO_INTERACTIVE,
	SG_SUMMARY_FMT_HTML,
	SG_SUMMARY_FMT_XML,
	SG_SUMMARY_FMT_XML_NO_INTERACTIVE
};


///////////////////////////////////////////////////////////
//														 //
//														 //
//														 //
///////////////////////////////////////////////////////////

//---------------------------------------------------------
class SAGA_API_DLL_EXPORT CSG_Module_Library
{
	friend class CSG_Module_Library_Manager;

public:

	bool							is_Valid			(void)	const	{	return( Get_Count() > 0 );	}

	const CSG_String &				Get_File_Name		(void)	const	{	return( m_File_Name );		}
	const CSG_String &				Get_Library_Name	(void)	const	{	return( m_Library_Name );	}

	const SG_Char *					Get_Info			(int Type)	const;
	CSG_String						Get_Name			(void)	const	{	return( Get_Info(MLB_INFO_Name       ) );	}
	CSG_String						Get_Description		(void)	const	{	return( Get_Info(MLB_INFO_Description) );	}
	CSG_String						Get_Author			(void)	const	{	return( Get_Info(MLB_INFO_Author     ) );	}
	CSG_String						Get_Version			(void)	const	{	return( Get_Info(MLB_INFO_Version    ) );	}
	CSG_String						Get_Menu			(void)	const	{	return( Get_Info(MLB_INFO_Menu_Path  ) );	}
	CSG_String						Get_Summary			(int Format = SG_SUMMARY_FMT_HTML)	const;
	bool							Get_Summary			(const CSG_String &Path)			const;

	int								Get_Count			(void)	const	{	return( m_pInterface ? m_pInterface->Get_Count() : 0 );	}

	CSG_String						Get_Menu			(int i)	const;

	CSG_Module *					Get_Module			(int           Index, TSG_Module_Type Type = MODULE_TYPE_Base)	const;
	CSG_Module *					Get_Module			(const SG_Char *Name, TSG_Module_Type Type = MODULE_TYPE_Base)	const;

	CSG_Module_Grid *				Get_Module_Grid				(int           Index)	const;
	CSG_Module_Grid *				Get_Module_Grid				(const SG_Char *Name)	const;
	CSG_Module_Interactive *		Get_Module_Interactive		(int           Index)	const;
	CSG_Module_Interactive *		Get_Module_Interactive		(const SG_Char *Name)	const;
	CSG_Module_Grid_Interactive *	Get_Module_Grid_Interactive	(int           Index)	const;
	CSG_Module_Grid_Interactive *	Get_Module_Grid_Interactive	(const SG_Char *Name)	const;


private:

	CSG_Module_Library(const CSG_String &File_Name);
	virtual ~CSG_Module_Library(void);

	bool							_Destroy			(void);


	CSG_String						m_File_Name, m_Library_Name;

	CSG_Module_Library_Interface	*m_pInterface;

	class wxDynamicLibrary			*m_pLibrary;

};


///////////////////////////////////////////////////////////
//														 //
//														 //
//														 //
///////////////////////////////////////////////////////////

//---------------------------------------------------------
class SAGA_API_DLL_EXPORT CSG_Module_Library_Manager
{
public:
	CSG_Module_Library_Manager(void);
	virtual ~CSG_Module_Library_Manager(void);

	bool							Destroy				(void);

	int								Get_Count			(void)	const	{	return( m_nLibraries );	}

	CSG_Module_Library *			Add_Library			(const SG_Char *File_Name);
	int								Add_Directory		(const SG_Char *Directory, bool bOnlySubDirectories);

	bool							Del_Library			(int i);
	bool							Del_Library			(CSG_Module_Library *pLibrary);

	CSG_Module_Library *			Get_Library			(int i)	const	{	return( i >= 0 && i < Get_Count() ? m_pLibraries[i] : NULL );	}
	CSG_Module_Library *			Get_Library			(const SG_Char *Name, bool bLibrary)	const;

	CSG_Module *					Get_Module			(const SG_Char *Library, int            Module)	const;
	CSG_Module *					Get_Module			(const SG_Char *Library, const SG_Char *Module)	const;

	CSG_String						Get_Summary			(int Format = SG_SUMMARY_FMT_HTML)	const;
	bool							Get_Summary			(const CSG_String &Path)			const;


private:

	int								m_nLibraries;

	CSG_Module_Library				**m_pLibraries;

};

//---------------------------------------------------------
SAGA_API_DLL_EXPORT CSG_Module_Library_Manager &	SG_Get_Module_Library_Manager	(void);


///////////////////////////////////////////////////////////
//														 //
//														 //
//														 //
///////////////////////////////////////////////////////////

//---------------------------------------------------------
#define SG_RUN_MODULE(bRetVal, LIBRARY, MODULE, CONDITION)	{\
	\
	bRetVal	= false;\
	\
	CSG_Module	*pModule	= SG_Get_Module_Library_Manager().Get_Module(SG_T(LIBRARY), MODULE);\
	\
	if(	pModule == NULL )\
	{\
		Error_Set(CSG_String::Format(SG_T("%s: %s"), _TL("could not find module"), SG_T(LIBRARY)));\
	}\
	else\
	{\
		Process_Set_Text(pModule->Get_Name());\
		\
		CSG_Parameters	P; P.Assign(pModule->Get_Parameters());\
		\
		pModule->Set_Manager(NULL);\
		\
		if( !(CONDITION) )\
		{\
			Error_Set(CSG_String::Format(SG_T("%s: %s > %s"), _TL("could not initialize module"), SG_T(LIBRARY), pModule->Get_Name().c_str()));\
		}\
		else if( !pModule->Execute() )\
		{\
			Error_Set(CSG_String::Format(SG_T("%s: %s > %s"), _TL("could not execute module")   , SG_T(LIBRARY), pModule->Get_Name().c_str()));\
		}\
		else\
		{\
			bRetVal	= true;\
		}\
		\
		pModule->Get_Parameters()->Assign_Values(&P);\
		pModule->Set_Manager(P.Get_Manager());\
	}\
}


///////////////////////////////////////////////////////////
//														 //
//														 //
//														 //
///////////////////////////////////////////////////////////

//---------------------------------------------------------
#endif // #ifndef HEADER_INCLUDED__SAGA_API__module_library_H

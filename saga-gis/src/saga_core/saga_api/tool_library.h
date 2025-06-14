
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
//                    tool_library.h                     //
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
// Software Foundation, either version 2.1 of the        //
// License, or (at your option) any later version.       //
//                                                       //
// This library is distributed in the hope that it will  //
// be useful, but WITHOUT ANY WARRANTY; without even the //
// implied warranty of MERCHANTABILITY or FITNESS FOR A  //
// PARTICULAR PURPOSE. See the GNU Lesser General Public //
// License for more details.                             //
//                                                       //
// You should have received a copy of the GNU Lesser     //
// General Public License along with this program; if    //
// not, see <http://www.gnu.org/licenses/>.              //
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
#ifndef HEADER_INCLUDED__SAGA_API__tool_library_H
#define HEADER_INCLUDED__SAGA_API__tool_library_H


///////////////////////////////////////////////////////////
//                                                       //
//                                                       //
//                                                       //
///////////////////////////////////////////////////////////

//---------------------------------------------------------
/** \file tool_library.h
* The definitions needed for any SAGA tool library and the
* tool library management, including the SAGA API's default
* tool library manager.
* @see SG_Get_Tool_Library_Manager
* @see CSG_Tool_Library_Manager
* @see CSG_Tool_Library
*/


///////////////////////////////////////////////////////////
//                                                       //
//                                                       //
//                                                       //
///////////////////////////////////////////////////////////

//---------------------------------------------------------
#include "tool.h"


///////////////////////////////////////////////////////////
//                                                       //
//                                                       //
//                                                       //
///////////////////////////////////////////////////////////

//---------------------------------------------------------
enum class ESG_Library_Type
{
	Library, Chain, Undefined
};


///////////////////////////////////////////////////////////
//                                                       //
//                                                       //
//                                                       //
///////////////////////////////////////////////////////////

//---------------------------------------------------------
class SAGA_API_DLL_EXPORT CSG_Tool_Library
{
	friend class CSG_Tool_Library_Manager;

public:

	virtual ESG_Library_Type		Get_Type			(void)	const	{	return( ESG_Library_Type::Library );	}

	bool							is_Valid			(void)	const	{	return( Get_Count() > 0 );	}

	const CSG_String &				Get_File_Name		(void)	const	{	return( m_File_Name    );	}
	const CSG_String &				Get_Library_Name	(void)	const	{	return( m_Library_Name );	}

	virtual CSG_String				Get_Info			(int Type)	const;
	CSG_String						Get_Name			(void)	const	{	return( Get_Info(TLB_INFO_Name       ) );	}
	CSG_String						Get_Description		(void)	const	{	return( Get_Info(TLB_INFO_Description) );	}
	CSG_String						Get_Author			(void)	const	{	return( Get_Info(TLB_INFO_Author     ) );	}
	CSG_String						Get_Version			(void)	const	{	return( Get_Info(TLB_INFO_Version    ) );	}
	CSG_String						Get_Menu			(void)	const	{	return( Get_Info(TLB_INFO_Menu_Path  ) );	}
	CSG_String						Get_Category		(void)	const	{	return( Get_Info(TLB_INFO_Category   ) );	}

	CSG_String						Get_Summary			(int Format = SG_SUMMARY_FMT_HTML, bool bInteractive = true)	const;
	bool							Get_Summary			(const CSG_String &Path)	const;

	virtual int						Get_Count			(void)	const	{	return( m_pInterface ? m_pInterface->Get_Count() : 0 );	}

	virtual CSG_Tool *				Get_Tool			(int              Index, TSG_Tool_Type Type = TOOL_TYPE_Base)	const;
	virtual CSG_Tool *				Get_Tool			(const CSG_String &Name, TSG_Tool_Type Type = TOOL_TYPE_Base)	const;
	virtual CSG_Tool *				Get_Tool			(const char       *Name, TSG_Tool_Type Type = TOOL_TYPE_Base)	const;
	virtual CSG_Tool *				Get_Tool			(const wchar_t    *Name, TSG_Tool_Type Type = TOOL_TYPE_Base)	const;

	virtual CSG_Tool *				Create_Tool			(int              Index, bool bWithGUI = false, bool bWithCMD = true);
	virtual CSG_Tool *				Create_Tool			(const CSG_String &Name, bool bWithGUI = false, bool bWithCMD = true);
	virtual CSG_Tool *				Create_Tool			(const char       *Name, bool bWithGUI = false, bool bWithCMD = true);
	virtual CSG_Tool *				Create_Tool			(const wchar_t    *Name, bool bWithGUI = false, bool bWithCMD = true);

	virtual bool					Delete_Tool			(CSG_Tool *pTool);
	virtual bool					Delete_Tools		(void);

	virtual CSG_String				Get_File_Name		(int i)	const	{	return( "" );	}
	virtual CSG_String				Get_Menu			(int i)	const;

	void							Add_Reference		(const CSG_String &Authors, const CSG_String &Year, const CSG_String &Title, const CSG_String &Where, const SG_Char *Link = NULL, const SG_Char *Link_Text = NULL);
	void							Add_Reference		(const CSG_String &Link, const SG_Char *Link_Text = NULL);
	void							Del_References		(void);
	const CSG_Strings &				Get_References		(void)	const	{	return( m_References );		}


protected:

	CSG_Tool_Library(void);
	CSG_Tool_Library(const CSG_String &File);
	virtual ~CSG_Tool_Library(void);


	CSG_String						m_File_Name, m_Library_Name;


private:

	bool							_Destroy			(void);


	CSG_Strings						m_References;

	CSG_Tool_Library_Interface		*m_pInterface;

	class wxDynamicLibrary			*m_pLibrary;

};


///////////////////////////////////////////////////////////
//                                                       //
//                                                       //
//                                                       //
///////////////////////////////////////////////////////////

//---------------------------------------------------------
class SAGA_API_DLL_EXPORT CSG_Tool_Library_Manager
{
public:
	CSG_Tool_Library_Manager(void);
	virtual ~CSG_Tool_Library_Manager(void);

	bool						Destroy					(void);

	int							Get_Count				(void)	const	{	return( m_nLibraries );	}
	int							Get_Tool_Count			(void)	const;

	bool						Add_Default_Libraries	(bool bVerbose = false);

	CSG_Tool_Library *			Add_Library				(const CSG_String &File);
	CSG_Tool_Library *			Add_Library				(const char       *File);
	CSG_Tool_Library *			Add_Library				(const wchar_t    *File);

	int							Add_Directory			(const CSG_String &Directory, bool bOnlySubDirectories = false);
	int							Add_Directory			(const char       *Directory, bool bOnlySubDirectories = false);
	int							Add_Directory			(const wchar_t    *Directory, bool bOnlySubDirectories = false);

	bool						Del_Library				(int i);
	bool						Del_Library				(CSG_Tool_Library *pLibrary);

	CSG_Tool_Library *			Get_Library				(int i)	const	{	return( i >= 0 && i < Get_Count() ? m_pLibraries[i] : NULL );	}
	CSG_Tool_Library *			Get_Library				(const CSG_String &Name, bool bLibrary, ESG_Library_Type Type = ESG_Library_Type::Undefined)	const;
	CSG_Tool_Library *			Get_Library				(const char       *Name, bool bLibrary, ESG_Library_Type Type = ESG_Library_Type::Undefined)	const;
	CSG_Tool_Library *			Get_Library				(const wchar_t    *Name, bool bLibrary, ESG_Library_Type Type = ESG_Library_Type::Undefined)	const;

	bool						is_Loaded				(CSG_Tool_Library *pLibrary)	const;

	CSG_Tool *					Get_Tool				(const CSG_String &Library, int              Index)	const;
	CSG_Tool *					Get_Tool				(const char       *Library, int              Index)	const;
	CSG_Tool *					Get_Tool				(const wchar_t    *Library, int              Index)	const;
	CSG_Tool *					Get_Tool				(const CSG_String &Library, const CSG_String &Name)	const;
	CSG_Tool *					Get_Tool				(const char       *Library, const char       *Name)	const;
	CSG_Tool *					Get_Tool				(const wchar_t    *Library, const wchar_t    *Name)	const;

	CSG_Tool *					Create_Tool				(const CSG_String &Library, int              Index, bool bWithGUI = false, bool bWithCMD = true)	const;
	CSG_Tool *					Create_Tool				(const char       *Library, int              Index, bool bWithGUI = false, bool bWithCMD = true)	const;
	CSG_Tool *					Create_Tool				(const wchar_t    *Library, int              Index, bool bWithGUI = false, bool bWithCMD = true)	const;
	CSG_Tool *					Create_Tool				(const CSG_String &Library, const CSG_String &Name, bool bWithGUI = false, bool bWithCMD = true)	const;
	CSG_Tool *					Create_Tool				(const char       *Library, const char       *Name, bool bWithGUI = false, bool bWithCMD = true)	const;
	CSG_Tool *					Create_Tool				(const wchar_t    *Library, const wchar_t    *Name, bool bWithGUI = false, bool bWithCMD = true)	const;
	bool						Delete_Tool				(CSG_Tool *pTool)	const;

	CSG_String					Get_Summary				(int Format = SG_SUMMARY_FMT_HTML)	const;
	bool						Get_Summary				(const CSG_String &Path)			const;

	bool						Create_Python_ToolBox	(const CSG_String &Destination, bool bClean = true, bool bName = true, bool bSingleFile = false) const;


private:

	int							m_nLibraries;

	CSG_Tool_Library			**m_pLibraries;


	bool						_Add_Library			(const CSG_String &Library);
	bool						_Add_Library_Chains		(const CSG_String &Library, const CSG_String &Directory);

	CSG_Tool_Library *			_Add_Tool_Chain			(const CSG_String &File, bool bReload = true);

};

//---------------------------------------------------------
SAGA_API_DLL_EXPORT CSG_Tool_Library_Manager &	SG_Get_Tool_Library_Manager	(void);


///////////////////////////////////////////////////////////
//                                                       //
//                                                       //
//                                                       //
///////////////////////////////////////////////////////////

//---------------------------------------------------------
#define SG_RUN_TOOL(bRetVal, LIBRARY, TOOL, CONDITION)	{\
	\
	bRetVal = false;\
	\
	CSG_Tool *pTool = SG_Get_Tool_Library_Manager().Create_Tool(SG_T(LIBRARY), TOOL);\
	\
	if(	pTool == NULL )\
	{\
		SG_UI_Msg_Add_Error(CSG_String::Format("%s [%s]", _TL("could not find tool"), SG_T(LIBRARY)));\
	}\
	else\
	{\
		SG_UI_Process_Set_Text(pTool->Get_Name());\
		\
		pTool->Settings_Push();\
		\
		if( !pTool->On_Before_Execution() || !(CONDITION) )\
		{\
			SG_UI_Msg_Add_Error(CSG_String::Format("%s [%s].[%s]", _TL("could not initialize tool"), SG_T(LIBRARY), pTool->Get_Name().c_str()));\
		}\
		else if( !pTool->Execute(false) )\
		{\
			SG_UI_Msg_Add_Error(CSG_String::Format("%s [%s].[%s]", _TL("could not execute tool"   ), SG_T(LIBRARY), pTool->Get_Name().c_str()));\
		}\
		else\
		{\
			bRetVal = true;\
		}\
		\
		SG_Get_Tool_Library_Manager().Delete_Tool(pTool);\
	}\
}

#define SG_RUN_TOOL_ExitOnError(LIBRARY, TOOL, CONDITION)	{\
	\
	bool bResult;\
	\
	SG_RUN_TOOL(bResult, LIBRARY, TOOL, CONDITION)\
	\
	if( !bResult )\
	{\
		return( false );\
	}\
}

//---------------------------------------------------------
#define SG_RUN_TOOL_KEEP_PARMS(bRetVal, LIBRARY, TOOL, PARMS, CONDITION)	{\
	\
	bRetVal = false;\
	\
	CSG_Tool *pTool = SG_Get_Tool_Library_Manager().Create_Tool(SG_T(LIBRARY), TOOL);\
	\
	if(	pTool == NULL )\
	{\
		SG_UI_Msg_Add_Error(CSG_String::Format("%s [%s]", _TL("could not find tool"), SG_T(LIBRARY)));\
	}\
	else\
	{\
		SG_UI_Process_Set_Text(pTool->Get_Name());\
		\
		pTool->Settings_Push();\
		\
		if( !pTool->On_Before_Execution() || !(CONDITION) )\
		{\
			SG_UI_Msg_Add_Error(CSG_String::Format("%s [%s].[%s]", _TL("could not initialize tool"), SG_T(LIBRARY), pTool->Get_Name().c_str()));\
		}\
		else if( !pTool->Execute(false) )\
		{\
			SG_UI_Msg_Add_Error(CSG_String::Format("%s [%s].[%s]", _TL("could not execute tool"   ), SG_T(LIBRARY), pTool->Get_Name().c_str()));\
		}\
		else\
		{\
			bRetVal	= true;\
			\
			PARMS.Set_Manager(NULL); PARMS.Assign_Parameters(pTool->Get_Parameters());\
		}\
		\
		SG_Get_Tool_Library_Manager().Delete_Tool(pTool);\
	}\
}

#define SG_RUN_TOOL_KEEP_PARMS_ExitOnError(LIBRARY, TOOL, PARMS, CONDITION)	{\
	\
	bool bResult;\
	\
	SG_RUN_TOOL_KEEP_PARMS(bResult, LIBRARY, TOOL, PARMS, CONDITION)\
	\
	if( !bResult )\
	{\
		return( false );\
	}\
}

//---------------------------------------------------------
#define SG_TOOL_PARAMETER_SET(IDENTIFIER, VALUE)	(pTool->Get_Parameters()->Get_Parameter(IDENTIFIER, true) && pTool->Set_Parameter(IDENTIFIER, VALUE))

#define SG_TOOL_PARAMLIST_ADD(IDENTIFIER, VALUE)	(\
		pTool->Get_Parameters()->Get_Parameter(IDENTIFIER, true)\
	&&	pTool->Get_Parameters()->Get_Parameter(IDENTIFIER)->asList()\
	&&	pTool->Get_Parameters()->Get_Parameter(IDENTIFIER)->asList()->Add_Item(VALUE)\
)

#define SG_TOOL_SET_DATAOBJECT_LIST(IDENTIFIER, VALUE)	(\
		pTool->Get_Parameters()->Get_Parameter(IDENTIFIER, true)\
	&&	pTool->Get_Parameters()->Get_Parameter(IDENTIFIER)->asList()\
	&&	pTool->Get_Parameters()->Get_Parameter(IDENTIFIER)->asList()->Assign(VALUE)\
)


///////////////////////////////////////////////////////////
//                                                       //
//                                                       //
//                                                       //
///////////////////////////////////////////////////////////

//---------------------------------------------------------
#endif // #ifndef HEADER_INCLUDED__SAGA_API__tool_library_H

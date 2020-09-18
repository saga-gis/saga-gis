
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
//                     tool_chain.h                      //
//                                                       //
//          Copyright (C) 2014 by Olaf Conrad            //
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
//                University of Hamburg                  //
//                Germany                                //
//                                                       //
//    e-mail:     oconrad@saga-gis.org                   //
//                                                       //
///////////////////////////////////////////////////////////

//---------------------------------------------------------
#ifndef HEADER_INCLUDED__SAGA_API__tool_chain_H
#define HEADER_INCLUDED__SAGA_API__tool_chain_H


///////////////////////////////////////////////////////////
//														 //
//														 //
//														 //
///////////////////////////////////////////////////////////

//---------------------------------------------------------
#include "tool_library.h"
#include "data_manager.h"


///////////////////////////////////////////////////////////
//														 //
//					CSG_Tool_Chain						 //
//														 //
///////////////////////////////////////////////////////////

//---------------------------------------------------------
/**
  * CSG_Tool_Chain, the SAGA Model class.
*/
//---------------------------------------------------------
class SAGA_API_DLL_EXPORT CSG_Tool_Chain : public CSG_Tool
{
public:
	CSG_Tool_Chain(void);
	virtual ~CSG_Tool_Chain(void);

								CSG_Tool_Chain			(const CSG_Tool_Chain &Tool, bool bWithGUI);
	bool						Create					(const CSG_Tool_Chain &Tool, bool bWithGUI);

								CSG_Tool_Chain			(const CSG_String &File);
	bool						Create					(const CSG_String &File);

								CSG_Tool_Chain			(const CSG_MetaData &Chain);
	bool						Create					(const CSG_MetaData &Chain);

	virtual TSG_Tool_Type		Get_Type				(void)	const	{	return( TOOL_TYPE_Chain );	}

	virtual bool				do_Sync_Projections		(void)	const;

	bool						is_Okay					(void)	const	{	return( m_Chain.Get_Children_Count() > 0 );	}

	void						Set_Library_Menu		(const CSG_String &Menu);
	virtual CSG_String			Get_MenuPath			(void)			{	return( m_Menu );	}
	const CSG_String &			Get_Library_Name		(void)	const	{	return( m_Library_Name );	}

	static CSG_String			Get_Script				(CSG_Tool *pTool, bool bHeader, bool bAllParameters = true);

	static bool					Save_History_to_Model	(const CSG_MetaData &History, const CSG_String &File);


protected:

	virtual int					On_Parameters_Enable	(CSG_Parameters *pParameters, CSG_Parameter *pParameter);

	virtual bool				On_Execute				(void);


private:

	bool						m_bAddHistory;

	CSG_String					m_Library_Name, m_Menu;

	CSG_MetaData				m_Chain, m_Conditions;

	CSG_Parameters				m_Data;

	CSG_Data_Manager			m_Data_Manager;


	void						Reset					(void);

	void						Add_References			(void);

	bool						Data_Add				(const CSG_String &ID, CSG_Parameter *pData);
	bool						Data_Add_TempList		(const CSG_String &ID, const CSG_String &Type);
	bool						Data_Del_Temp			(const CSG_String &ID, bool bData);
	bool						Data_Exists				(CSG_Data_Object *pData);
	bool						Data_Initialize			(void);
	bool						Data_Finalize			(void);

	bool						Parameter_isCompatible	(TSG_Parameter_Type A, TSG_Parameter_Type B);

	bool						Check_Condition			(const CSG_MetaData &Condition, CSG_Parameters *pData);

	bool						ForEach					(const CSG_MetaData &Commands);
	bool						ForEach_Iterator		(const CSG_MetaData &Commands, const CSG_String &    VarName, bool bIgnoreErrors);
	bool						ForEach_Object			(const CSG_MetaData &Commands, const CSG_String &ListVarName, bool bIgnoreErrors);
	bool						ForEach_File			(const CSG_MetaData &Commands, const CSG_String &ListVarName, bool bIgnoreErrors);

	bool						Tool_Run				(const CSG_MetaData &Tool, bool bShowError = true);
	bool						Tool_Check_Condition	(const CSG_MetaData &Tool);
	bool						Tool_Get_Parameter		(const CSG_String ID, CSG_Parameters *pParameters, CSG_Parameter **ppParameter, CSG_Parameter **ppOwner = NULL);
	bool						Tool_Get_Parameter		(const CSG_MetaData &Parameter, CSG_Tool *pTool  , CSG_Parameter **ppParameter, CSG_Parameter **ppOwner = NULL);
	bool						Tool_Initialize			(const CSG_MetaData &Tool, CSG_Tool *pTool);
	bool						Tool_Finalize			(const CSG_MetaData &Tool, CSG_Tool *pTool);

	//-----------------------------------------------------
	static bool					_Get_Script_Tool		(CSG_MetaData &Tool      , CSG_Parameters *pParameters, bool bAllParameters, const CSG_String &Prefix, bool bVarNames);
	static bool					_Get_Script_Parameters	(CSG_MetaData &Parameters, CSG_Parameters *pParameters, const CSG_String &Prefix);

	static bool					_Save_History_Add_Tool	(const CSG_MetaData &History, CSG_MetaData &Parms, CSG_MetaData &Tools, bool bAddOutput = false);
	static bool					_Save_History_Add_Input	(const CSG_MetaData &History, CSG_MetaData &Parms, CSG_MetaData &Tool);

};


///////////////////////////////////////////////////////////
//														 //
//														 //
//														 //
///////////////////////////////////////////////////////////

//---------------------------------------------------------
class SAGA_API_DLL_EXPORT CSG_Tool_Chains : public CSG_Tool_Library
{
	friend class CSG_Tool_Library_Manager;

public:

	virtual TSG_Tool_Library_Type	Get_Type			(void)		const	{	return( TOOL_CHAINS );	}

	virtual CSG_String				Get_Info			(int Type)	const;

	bool							Add_Tool			(CSG_Tool_Chain *pTool);

	virtual int						Get_Count			(void)		const	{	return( (int)m_Tools.Get_Size() );	}

	virtual CSG_Tool *				Get_Tool			(int Index, TSG_Tool_Type Type = TOOL_TYPE_Base)	const;

	virtual CSG_Tool *				Create_Tool			(const CSG_String &Name, bool bWithGUI = false);
	virtual bool					Delete_Tool			(CSG_Tool *pTool);
	virtual bool					Delete_Tools		(void);

	virtual CSG_String				Get_File_Name		(int Index)	const	{	return( Index >= 0 && Index < Get_Count() ? ((CSG_Tool *)m_Tools[Index])->Get_File_Name() : "" );	}


protected:

	CSG_Tool_Chains(const CSG_String &Library_Name, const CSG_String &Path);
	virtual ~CSG_Tool_Chains(void);


private:

	CSG_Array_Pointer				m_Tools, m_xTools;

	CSG_String						m_Name, m_Description, m_Menu;

};


///////////////////////////////////////////////////////////
//														 //
//														 //
//														 //
///////////////////////////////////////////////////////////

//---------------------------------------------------------
#endif // #ifndef HEADER_INCLUDED__SAGA_API__tool_chain_H

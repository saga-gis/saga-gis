/**********************************************************
 * Version $Id: module_chain.h 2111 2014-05-07 09:58:48Z oconrad $
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
//                    module_chain.h                     //
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
//                University of Hamburg                  //
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
#ifndef HEADER_INCLUDED__SAGA_API__module_chain_H
#define HEADER_INCLUDED__SAGA_API__module_chain_H


///////////////////////////////////////////////////////////
//														 //
//														 //
//														 //
///////////////////////////////////////////////////////////

//---------------------------------------------------------
#include "module_library.h"


///////////////////////////////////////////////////////////
//														 //
//					CSG_Module_Chain					 //
//														 //
///////////////////////////////////////////////////////////

//---------------------------------------------------------
/**
  * CSG_Module_Chain, the SAGA Model class.
*/
//---------------------------------------------------------
class SAGA_API_DLL_EXPORT CSG_Module_Chain : public CSG_Module
{
public:
								CSG_Module_Chain		(void);
	bool						Create					(void);

								CSG_Module_Chain		(const CSG_String &File);
	bool						Create					(const CSG_String &File);

	virtual ~CSG_Module_Chain(void);

	virtual TSG_Module_Type		Get_Type				(void)			{	return( MODULE_TYPE_Chain );	}

	bool						is_Okay					(void)	const	{	return( m_Chain.is_Valid() );	}

	const CSG_String &			Get_File_Name			(void)	const	{	return( m_File_Name );	}

	static bool					Save_History_to_Model	(const CSG_MetaData &History, const CSG_String &File);


protected:

	virtual bool				On_Execute				(void);


private:

	CSG_String					m_File_Name;

	CSG_MetaData				m_Chain;

};


///////////////////////////////////////////////////////////
//														 //
//														 //
//														 //
///////////////////////////////////////////////////////////

//---------------------------------------------------------
class SAGA_API_DLL_EXPORT CSG_Module_Chains : public CSG_Module_Library
{
	friend class CSG_Module_Library_Manager;

public:

	virtual TSG_Module_Library_Type	Get_Type			(void)		const	{	return( MODULE_CHAINS );	}

	virtual CSG_String				Get_Info			(int Type)	const;

	bool							Add_Module			(CSG_Module_Chain *pModule);

	virtual int						Get_Count			(void)		const	{	return( m_nModules );	}

	virtual CSG_Module *			Get_Module			(int Index, TSG_Module_Type Type = MODULE_TYPE_Base)	const;

	virtual CSG_String				Get_File_Name		(int Index)	const	{	return( Index >= 0 && Index < m_nModules ? m_pModules[Index]->Get_File_Name() : "" );	}


protected:

	CSG_Module_Chains(const CSG_String &Library_Name, const CSG_String &Name, const CSG_String &Description, const CSG_String &Menu);
	virtual ~CSG_Module_Chains(void);


private:

	int								m_nModules;

	CSG_Module_Chain				**m_pModules;

	CSG_String						m_Name, m_Description, m_Menu;

};


///////////////////////////////////////////////////////////
//														 //
//														 //
//														 //
///////////////////////////////////////////////////////////

//---------------------------------------------------------
#endif // #ifndef HEADER_INCLUDED__SAGA_API__module_chain_H

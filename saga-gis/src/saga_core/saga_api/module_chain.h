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

	const CSG_String &			Get_File_Name			(void)	const	{	return( m_File_Name );	}

	static bool					Save_History_to_Model	(const CSG_MetaData &History, const CSG_String &File);


protected:

	virtual bool				On_Execute				(void);


private:

	CSG_String					m_File_Name;

};


///////////////////////////////////////////////////////////
//														 //
//														 //
//														 //
///////////////////////////////////////////////////////////

//---------------------------------------------------------
class SAGA_API_DLL_EXPORT CSG_Module_Chains
{
	friend class CSG_Module_Library_Manager;

public:

	virtual TSG_Module_Library_Type	Get_Type			(void)		const	{	return( MODULE_CHAINS );	}

	virtual int						Get_Count			(void)		const	{	return( m_nModules );	}

	virtual CSG_String				Get_File_Name		(int i)		const	{	return( i >= 0 && i < m_nModules ? m_pModules[i]->Get_File_Name() : "" );	}

	virtual CSG_String				Get_Info			(int Type)	const;

	bool							Add_Module			(CSG_Module_Chain *pModule);

	virtual CSG_String				Get_Menu			(int i)		const;


private:

	CSG_Module_Chains(void);
	virtual ~CSG_Module_Chains(void);

	int								m_nModules;

	CSG_Module_Chain				**m_pModules;

};


///////////////////////////////////////////////////////////
//														 //
//														 //
//														 //
///////////////////////////////////////////////////////////

//---------------------------------------------------------
#endif // #ifndef HEADER_INCLUDED__SAGA_API__module_chain_H


///////////////////////////////////////////////////////////
//                                                       //
//                         SAGA                          //
//                                                       //
//      System for Automated Geoscientific Analyses      //
//                                                       //
//           Application Programming Interface           //
//                                                       //
//                  Library: SAGA_ODBC                   //
//                                                       //
//-------------------------------------------------------//
//                                                       //
//                      saga_odbc.h                      //
//                                                       //
//                 Copyright (C) 2010 by                 //
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
//                University of Hamburg                  //
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
#ifndef HEADER_INCLUDED__SAGA_ODBC_saga_odbc_H
#define HEADER_INCLUDED__SAGA_ODBC_saga_odbc_H


///////////////////////////////////////////////////////////
//														 //
//														 //
//														 //
///////////////////////////////////////////////////////////

//---------------------------------------------------------
#include <saga_api/saga_api.h>

#ifdef _SAGA_ODBC_EXPORTS
	#define	SG_ODBC_API_DLL_EXPORT		_SAGA_DLL_EXPORT
#else
 	#define	SG_ODBC_API_DLL_EXPORT		_SAGA_DLL_IMPORT
#endif


///////////////////////////////////////////////////////////
//														 //
//														 //
//														 //
///////////////////////////////////////////////////////////

//---------------------------------------------------------
class SG_ODBC_API_DLL_EXPORT CSG_ODBC_Connection
{
	friend class CSG_ODBC_Connections;

public:

	bool						is_Connected			(void)	const	{	return( m_pConnection != NULL );	}

	int							Get_Size_Buffer			(void)	const	{	return( m_Size_Buffer );	}
	bool						Set_Size_Buffer			(int Size);

	int							Get_Size_LOB_Max		(void)	const;
	bool						Set_Size_LOB_Max		(int Size);

	CSG_String					Get_DBMS_Name			(void)	const;
	bool						is_Postgres				(void)	const;

	CSG_String					Get_Server				(void)	const	{	return( m_DSN );	}

	CSG_String					Get_Tables				(void)	const;
	CSG_String					Get_Columns				(const CSG_String &Table)	const;

	bool						Execute					(const CSG_String &SQL, bool bCommit = false);
	bool						Commit					(void);
	bool						Rollback				(void);

	bool						Table_Exists			(const CSG_String &Table_Name)	const;

	bool						Table_Create			(const CSG_String &Table_Name, const CSG_Table &Table, bool bCommit = true);
	bool						Table_Drop				(const CSG_String &Table_Name                        , bool bCommit = true);

	bool						Table_Load				(CSG_Table &Table, const CSG_String &Table_Name, bool bLOB = false);
	bool						Table_Load				(CSG_Table &Table, const CSG_String &Tables, const CSG_String &Fields, const CSG_String &Where = SG_T(""), const CSG_String &Order = SG_T(""), bool bLOB = false);

	bool						Table_Load_BLOBs		(CSG_Bytes_Array &BLOBs, const CSG_String &Table, const CSG_String &Field, const CSG_String &Where = SG_T(""), const CSG_String &Order = SG_T(""));

	bool						Table_Save				(const CSG_String &Table_Name, const CSG_Table &Table, bool bCommit = true);


private:

	CSG_ODBC_Connection(const CSG_String &Server, const CSG_String &User, const CSG_String &Password, const CSG_String &Directory);
	virtual ~CSG_ODBC_Connection(void);


	int							m_Size_Buffer;

	void						*m_pConnection;

	CSG_String					m_DSN;


	CSG_String					_Get_DBMS_Info			(int What)	const;

	int							_Get_Type_To_SQL		(TSG_Data_Type Type);
	TSG_Data_Type				_Get_Type_From_SQL		(int Type);

	bool						_Table_Load				(CSG_Table &Table, const CSG_String &Select, const CSG_String &Name, bool bLOB);

};


///////////////////////////////////////////////////////////
//														 //
//														 //
//														 //
///////////////////////////////////////////////////////////

//---------------------------------------------------------
class SG_ODBC_API_DLL_EXPORT CSG_ODBC_Connections
{
public:
	CSG_ODBC_Connections(void);
	virtual ~CSG_ODBC_Connections(void);

	bool						Create					(void);
	bool						Destroy					(void);

	int							Get_Servers				(CSG_String &Servers);
	CSG_Strings					Get_Servers				(void);

	int							Get_Connections			(CSG_String &Connections);
	CSG_Strings					Get_Connections			(void);

	int							Get_Count				(void)		{	return( m_nConnections );	}
	CSG_ODBC_Connection *		Get_Connection			(int Index)	{	return( Index >= 0 && Index < m_nConnections ? m_pConnections[Index] : NULL );	}
	CSG_ODBC_Connection *		Get_Connection			(const CSG_String &Server);
	CSG_ODBC_Connection *		Add_Connection			(const CSG_String &Server, const CSG_String &User, const CSG_String &Password, const CSG_String &Directory = SG_T(""));
	bool						Del_Connection			(int Index                       , bool bCommit);
	bool						Del_Connection			(const CSG_String &Server        , bool bCommit);
	bool						Del_Connection			(CSG_ODBC_Connection *pConnection, bool bCommit);


private:

	void						*m_hEnv;

	int							m_nConnections;

	CSG_ODBC_Connection			**m_pConnections;


};


///////////////////////////////////////////////////////////
//														 //
//														 //
//														 //
///////////////////////////////////////////////////////////

//---------------------------------------------------------
class SG_ODBC_API_DLL_EXPORT CSG_ODBC_Module : public CSG_Module
{
public:
	CSG_ODBC_Module(void);


protected:

	virtual bool				On_Before_Execution		(void);
	virtual bool				On_After_Execution		(void);

	CSG_ODBC_Connection *		Get_Connection			(void)	{	return( m_pConnection );	}


private:

	CSG_ODBC_Connection			*m_pConnection;

	CSG_Parameters				m_Connection_Choice;


};


///////////////////////////////////////////////////////////
//														 //
//														 //
//														 //
///////////////////////////////////////////////////////////

//---------------------------------------------------------
SG_ODBC_API_DLL_EXPORT CSG_ODBC_Connections &	SG_ODBC_Get_Connection_Manager	(void);

//---------------------------------------------------------
SG_ODBC_API_DLL_EXPORT bool						SG_ODBC_is_Supported			(void);


///////////////////////////////////////////////////////////
//														 //
//														 //
//														 //
///////////////////////////////////////////////////////////

//---------------------------------------------------------
#endif // #ifndef HEADER_INCLUDED__SAGA_ODBC_saga_odbc_H

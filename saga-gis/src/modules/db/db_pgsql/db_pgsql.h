/**********************************************************
 * Version $Id: db_pgsql.cpp 911 2011-11-11 11:11:11Z oconrad $
 *********************************************************/

///////////////////////////////////////////////////////////
//                                                       //
//                         SAGA                          //
//                                                       //
//      System for Automated Geoscientific Analyses      //
//                                                       //
//                    Module Library:                    //
//                       db_pgsql                        //
//                                                       //
//-------------------------------------------------------//
//                                                       //
//                      db_pgsql.h                       //
//                                                       //
//                 Copyright (C) 2013 by                 //
//                     Olaf Conrad                       //
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
//    e-mail:     oconrad@saga-gis.de                    //
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
#ifndef HEADER_INCLUDED__db_pgsql_H
#define HEADER_INCLUDED__db_pgsql_H

//---------------------------------------------------------
#include "MLB_Interface.h"


///////////////////////////////////////////////////////////
//														 //
//														 //
//														 //
///////////////////////////////////////////////////////////

//---------------------------------------------------------
#define SG_PG_PRIMARY_KEY	0x01
#define SG_PG_NOT_NULL		0x02
#define SG_PG_UNIQUE		0x04


///////////////////////////////////////////////////////////
//														 //
//														 //
//														 //
///////////////////////////////////////////////////////////

//---------------------------------------------------------
class db_pgsql_EXPORT CSG_PG_Connection
{
	friend class CSG_PG_Connections;

public:

	static CSG_String			Get_Type_To_SQL			(TSG_Data_Type Type, int Size = 0);
	static TSG_Data_Type		Get_Type_From_SQL		(int Type);
	static CSG_String			Get_Raster_Type_To_SQL	(TSG_Data_Type Type);

	void						GUI_Update				(void)	const;

	bool						is_Connected			(void)	const	{	return( m_pConnection != NULL );	}

	CSG_String					Get_Connection			(int Style = 0)	const;

	CSG_String					Get_Host				(void)	const;
	CSG_String					Get_Port				(void)	const;
	CSG_String					Get_User				(void)	const;
	CSG_String					Get_DBName				(void)	const;
	CSG_String					Get_Version				(void)	const;
	bool						has_Version				(int Major, int Minor = 0, int Revision = 0) const;

	CSG_String					Get_PostGIS				(void)	const;
	bool						has_PostGIS				(double minVersion = 0.0);

	int							Get_Tables				(CSG_Strings &Tables)			const;
	CSG_String					Get_Tables				(void)							const;
	CSG_String					Get_Field_Names			(const CSG_String &Table_Name)	const;
	CSG_Table					Get_Field_Desc			(const CSG_String &Table_Name)	const;

	bool						Execute					(const CSG_String &SQL, CSG_Table *pTable = NULL);

	bool						Begin					(const CSG_String &SavePoint = "");
	bool						Rollback				(const CSG_String &SavePoint = "");
	bool						Commit					(const CSG_String &SavePoint = "");
	bool						is_Transaction			(void)	const	{	return( is_Connected() && m_bTransaction );	}

	CSG_MetaData &				Add_MetaData			(CSG_Data_Object &Object, const CSG_String &Table, const CSG_String &Select = "");

	bool						Table_Exists			(const CSG_String &Table_Name)	const;

	bool						Table_Create			(const CSG_String &Table_Name, const CSG_Table &Table, const CSG_Buffer &Flags = 0, bool bCommit = true);
	bool						Table_Drop				(const CSG_String &Table_Name                                                     , bool bCommit = true);
	bool						Table_Insert			(const CSG_String &Table_Name, const CSG_Table &Table                             , bool bCommit = true);
	bool						Table_Save				(const CSG_String &Table_Name, const CSG_Table &Table, const CSG_Buffer &Flags = 0, bool bCommit = true);

	bool						Table_Load				(CSG_Table &Data, const CSG_String &Table );
	bool						Table_Load				(CSG_Table &Data, const CSG_String &Tables, const CSG_String &Fields, const CSG_String &Where = "", const CSG_String &Group = "", const CSG_String &Having = "", const CSG_String &Order = "", bool bDistinct = false);

	bool						Shapes_Load				(CSG_Shapes *pShapes, const CSG_String &Table);
	bool						Shapes_Load				(CSG_Shapes *pShapes, const CSG_String &Name, const CSG_String &Select, const CSG_String &Geometry_Field, bool bBinary, int SRID = -1);

	bool						Raster_Load				(CSG_Parameter_Grid_List *pGrids    , const CSG_String &Table, const CSG_String &Where = "", const CSG_String &Order = "");
	bool						Raster_Load				(CSG_Grid *pGrid                    , const CSG_String &Table, const CSG_String &Where = "");
	bool						Raster_Save				(CSG_Grid *pGrid, int SRID, const CSG_String &Table, const CSG_String &Field);
	

private:

								CSG_PG_Connection		(const CSG_String &Host, int Port, const CSG_String &Name, const CSG_String &User, const CSG_String &Password, bool bAutoCommit = false);
	bool						Create					(const CSG_String &Host, int Port, const CSG_String &Name, const CSG_String &User, const CSG_String &Password, bool bAutoCommit = false);

	virtual ~CSG_PG_Connection(void);

	bool						Destroy					(void);


private:

	void						*m_pConnection;

	bool						m_bTransaction;


	bool						_Table_Load				(CSG_Table &Data, const CSG_String &Select, const CSG_String &Name = "")	const;
	bool						_Table_Load				(CSG_Table &Data, void *pResult)	const;

	bool						_Raster_Open			(CSG_Table &Info, const CSG_String &Table, const CSG_String &Where = "", const CSG_String &Order = "", bool bBinary = true);
	bool						_Raster_Load			(CSG_Grid *pGrid, bool bFirst, bool bBinary = true);

};


///////////////////////////////////////////////////////////
//														 //
//														 //
//														 //
///////////////////////////////////////////////////////////

//---------------------------------------------------------
class db_pgsql_EXPORT CSG_PG_Connections
{
public:
	CSG_PG_Connections(void);
	virtual ~CSG_PG_Connections(void);

	bool						Create					(void);
	bool						Destroy					(void);

	int							Get_Servers				(CSG_String  &Servers);
	int							Get_Servers				(CSG_Strings &Servers);
	CSG_Strings					Get_Servers				(void);

	int							Get_Connections			(CSG_String &Connections);
	CSG_Strings					Get_Connections			(void);

	int							Get_Count				(void)		{	return( m_nConnections );	}
	CSG_PG_Connection *			Get_Connection			(int Index)	{	return( Index >= 0 && Index < m_nConnections ? m_pConnections[Index] : NULL );	}
	CSG_PG_Connection *			Get_Connection			(const CSG_String &Name);
	CSG_PG_Connection *			Add_Connection			(const CSG_String &Name, const CSG_String &User, const CSG_String &Password, const CSG_String &Host = "localhost", int Port = 5432);
	bool						Del_Connection			(int Index                     , bool bCommit);
	bool						Del_Connection			(const CSG_String &Name        , bool bCommit);
	bool						Del_Connection			(CSG_PG_Connection *pConnection, bool bCommit);


private:

	int							m_nConnections;

	CSG_PG_Connection			**m_pConnections;


};


///////////////////////////////////////////////////////////
//														 //
//														 //
//														 //
///////////////////////////////////////////////////////////

//---------------------------------------------------------
class db_pgsql_EXPORT CSG_PG_Module : public CSG_Module
{
public:
	CSG_PG_Module(void);


protected:

	virtual bool				On_Before_Execution		(void);
	virtual bool				On_After_Execution		(void);

	virtual int					On_Parameter_Changed	(CSG_Parameters *pParameters, CSG_Parameter *pParameter);
	virtual void				On_Connection_Changed	(CSG_Parameters *pParameters)	{}

	CSG_PG_Connection *			Get_Connection			(void)	{	return( m_pConnection );	}

	bool						Add_SRID_Picker			(CSG_Parameters *pParameters = NULL);
	bool						Set_SRID_Picker_Enabled	(CSG_Parameters *pParameters, bool bEnable = true);
	bool						Set_SRID				(CSG_Parameters *pParameters, int SRID);
	int							Get_SRID				(CSG_Parameters *pParameters = NULL);

	static bool					Set_Constraints			(CSG_Parameters *pParameters, const CSG_String &Identifier);
	static CSG_Buffer			Get_Constraints			(CSG_Parameters *pParameters, const CSG_String &Identifier);


private:

	CSG_PG_Connection			*m_pConnection;


};


///////////////////////////////////////////////////////////
//														 //
//														 //
//														 //
///////////////////////////////////////////////////////////

//---------------------------------------------------------
db_pgsql_EXPORT CSG_PG_Connections &	SG_PG_Get_Connection_Manager	(void);

//---------------------------------------------------------
db_pgsql_EXPORT bool					SG_PG_is_Supported				(void);


///////////////////////////////////////////////////////////
//														 //
//														 //
//														 //
///////////////////////////////////////////////////////////

//---------------------------------------------------------
#endif // #ifndef HEADER_INCLUDED__db_pgsql_H

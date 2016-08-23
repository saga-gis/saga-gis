/**********************************************************
 * Version $Id: Get_Table.cpp 1925 2014-01-09 12:15:18Z oconrad $
 *********************************************************/

///////////////////////////////////////////////////////////
//                                                       //
//                         SAGA                          //
//                                                       //
//      System for Automated Geoscientific Analyses      //
//                                                       //
//                     Tool Library                      //
//                      Table_ODBC                       //
//                                                       //
//-------------------------------------------------------//
//                                                       //
//                    TLB_Interface.h                    //
//                                                       //
//                 Copyright (C) 2004 by                 //
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
// 51 Franklin Street, 5th Floor, Boston, MA 02110-1301, //
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
//                                                       //
//    This code is something I did for SAGA GIS to       //
//    import data from MySQL Database.					 //
//														 //
//	  INPUT : Connection information and SQL statement	 //
//	  OUTPUT: Result data as Table						 //
//														 //
//		RELEASE : v0.1b (Lot more to do)				 //
//														 //
//	  The SAGA structure code is grapped from 	         //
//    Olaf Conrad's ODBC example and database			 //
//	  communication part is replaced with MySQL			 //
//	  native library (libmysql.dll for win32)			 //
//														 //
//	  The code is compiled under						 //
//	      Visual C++ 2008 Express Edition 	             //
//-------------------------------------------------------//
//                                                       //
//    e-mail:     sagamysql@ferhatbingol.com             //
//                                                       //
//    contact:    Ferhat Bingöl                          //
//                                                       //
///////////////////////////////////////////////////////////


#include <config-win.h> // Needed for Win32, change it for your system
#include <mysql.h>		// Needed for Win32, change it (if needed) for your system
#include "Get_Table.h"





///////////////////////////////////////////////////////////
//														 //
//														 //
//														 //
///////////////////////////////////////////////////////////

//---------------------------------------------------------
CGet_Table::CGet_Table(void)
{
	Set_Name	(_TL("Import DB Table from MySQL"));

	Set_Author		(SG_T("(c) 2009 by Ferhat Bingöl"));

	Set_Description(
		_TL("Get table from MySQL server.\n")
	);

	
	Parameters.Add_String(
		NULL	, "SERVER"		, _TL("Server"),
		_TL(""),
		_TL("localhost")
	);

	Parameters.Add_String(
		NULL	, "PORT"		, _TL("Port"),
		_TL(""),
		_TL("3306")
	);
	
	Parameters.Add_String(
		NULL	, "USERNAME"		, _TL("Username"),
		_TL(""),
		_TL("root")
	);

	Parameters.Add_String(
		NULL	, "PASSWORD"		, _TL("Password"),
		_TL(""),
		_TL("")
	);

	Parameters.Add_String(
		NULL	, "DATABASE"		, _TL("Database"),
		_TL(""),
		_TL("")
	);

	Parameters.Add_String(
		NULL	, "SQLStatement"		, _TL("SQL Statement"),
		_TL(""),
		_TL("Select * FROM test")
	);


	Parameters.Add_Table(
		NULL	, "SQLData"		, _TL("MySQL Data"),
		_TL(""),
		PARAMETER_OUTPUT
	);

}


CGet_Table::~CGet_Table(void)
{

}


//---------------------------------------------------------
bool CGet_Table::On_Execute(void)
{
	// Input variables
	CSG_String MyServer,MyUsername,MyPassword,MyDatabase,MySQLStatement,s,Types;
	CSG_Table *MyTable;
	int MyPort;

	// MySQL Variables
	char SQL[2000];
	MYSQL *conn;					// MySQL connection handle
	MYSQL_RES *res;					// MySQL Results 
	MYSQL_ROW row;					// MySQL Rows
	MYSQL_FIELD *fields;			// Storage for the field names
	int NR;							// Number of returned raws
	int NF;							// Number of returned fields

	// Variables used in transformation
	CSG_String				Name;
	int	iType,i,j;
	CSG_Parameters			P;
	TSG_Table_Field_Type	Type;

	


	// Get the intput variables
	MyServer		= Parameters("SERVER")			->asString();
	MyPort			= Parameters("PORT")			->asInt();
	MyUsername		= Parameters("USERNAME")		->asString();
	MyPassword		= Parameters("PASSWORD")		->asString();
	MyDatabase		= Parameters("DATABASE")		->asString();
	MySQLStatement	= Parameters("SQLStatement")	->asString();
	
	MyTable			= Parameters("SQLData")			->asTable();


	// print out what is going on to the execution screen
	sprintf(SQL,"Server connection will be made to  %s@%s:%i/%s", MyUsername,MyServer,MyPort,MyDatabase);
	Message_Add(_TL(SQL));
	sprintf(SQL,"Entered SQL statement -> %s)", MySQLStatement);
	Message_Add(_TL(SQL));

	// Init MySQL 
	conn = mysql_init(NULL);

   // Connect
   if (!mysql_real_connect(conn, MyServer,MyUsername, MyPassword, MyDatabase, MyPort, NULL, 0)) 
   {
	   sprintf(SQL,"Cannot connect to the database server -> %s)", mysql_error(conn));
	   Message_Add(_TL(SQL));
       return(false);
   }
   else 
   {
	   sprintf(SQL,"Connected to %s@%s:%i/%s",MyUsername,MyServer,MyPort,MyDatabase);
	   Message_Add(_TL(SQL)); 
   }

  // Send the SQL statement and get the data
  if (mysql_query(conn, MySQLStatement)) {
	   sprintf(SQL,"Cannot get the data you asked with %s -> (%s)",MySQLStatement, mysql_error(conn));
	   Message_Add(_TL(SQL)); 
      return(false);
   }

   // Store the results
   res = mysql_store_result(conn);

   // Get the number of rows and inform
   NR=(int)mysql_num_rows(res); 
   sprintf(SQL,"%i number of rows are avaliable", NR);
   Message_Add(_TL(SQL)); 
   
   // Get the number of fields and inform 
   NF=mysql_num_fields(res);
   sprintf(SQL,"%i number of fields are avaliable", NF);
   Message_Add(_TL(SQL)); 

	// Get the field types
   fields = mysql_fetch_fields(res);

	// Define the table field types and insert the names
   for(i=0; i<NF; i++)
	{
			
			Name	= fields[i].name;
			iType	= fields[i].type;
			switch( iType )
			{
			default:
			case 0:	Type	= TABLE_FIELDTYPE_None;		break; // Not supported
			case 1:	Type	= TABLE_FIELDTYPE_Int;		break;
			case 2:	Type	= TABLE_FIELDTYPE_Short;	break;
			case 3:	Type	= TABLE_FIELDTYPE_Long;		break;
			case 4:	Type	= TABLE_FIELDTYPE_Float;	break;
			case 5:	Type	= TABLE_FIELDTYPE_Double;	break;
			case 6:	Type	= TABLE_FIELDTYPE_None;		break;
			case 7:	Type	= TABLE_FIELDTYPE_String;	break;
			case 8:	Type	= TABLE_FIELDTYPE_Long;		break;
			case 9:	Type	= TABLE_FIELDTYPE_Long;		break;
			case 10:Type	= TABLE_FIELDTYPE_String;	break;
			case 11:Type	= TABLE_FIELDTYPE_String;	break;
			case 12:Type	= TABLE_FIELDTYPE_String;	break;
			case 13:Type	= TABLE_FIELDTYPE_String;	break;
			case 14:Type	= TABLE_FIELDTYPE_String;	break;
			case 15:Type	= TABLE_FIELDTYPE_String;	break;
			case 16:Type	= TABLE_FIELDTYPE_None;		break; // Not supported
			case 246:Type	= TABLE_FIELDTYPE_None;		break; // Not supported
			case 247:Type	= TABLE_FIELDTYPE_None;		break; // Not supported
			case 248:Type	= TABLE_FIELDTYPE_None;		break; // Not supported
			case 249:Type	= TABLE_FIELDTYPE_None;		break; // Not supported
			case 250:Type	= TABLE_FIELDTYPE_None;		break; // Not supported
			case 251:Type	= TABLE_FIELDTYPE_None;		break; // Not supported
			case 252:Type	= TABLE_FIELDTYPE_None;		break; // Not supported
			case 253:Type	= TABLE_FIELDTYPE_String;	break;
			case 254:Type	= TABLE_FIELDTYPE_String;	break;
			case 255:Type	= TABLE_FIELDTYPE_None;		break; // Not supported
			}

			MyTable->Add_Field(Name, Type);
	   }
	// Insert data to the ready table
	j=0;
	while ((row = mysql_fetch_row(res)) != NULL) 
	{
	   MyTable->Add_Record();
	   for(i=0;i<NF;i++)
	   {
		   	iType	= fields[i].type;
			switch( iType )
			{
			default:
			case 0:	  MyTable->Set_Value(j,i,0.0); 					break; // Not supported
			case 1:	  MyTable->Set_Value(j,i,atoi(row[i])); 		break;
			case 2:	  MyTable->Set_Value(j,i,atoi(row[i]));			break;
			case 3:	  MyTable->Set_Value(j,i,atoi(row[i])); 		break;
			case 4:	  MyTable->Set_Value(j,i,atof(row[i]));			break;
			case 5:	  MyTable->Set_Value(j,i,atof(row[i]));  		break;
			case 6:	  MyTable->Set_Value(j,i,0.0);  				break;
			case 7:	  MyTable->Set_Value(j,i,row[i]);				break;
			case 8:	  MyTable->Set_Value(j,i,atoi(row[i]));			break;
			case 9:	  MyTable->Set_Value(j,i,atoi(row[i]));			break;
			case 10:  MyTable->Set_Value(j,i,row[i]);				break;
			case 11:  MyTable->Set_Value(j,i,row[i]);  				break;
			case 12:  MyTable->Set_Value(j,i,row[i]); 				break;
			case 13:  MyTable->Set_Value(j,i,row[i]);  				break;
			case 14:  MyTable->Set_Value(j,i,row[i]);				break;
			case 15:  MyTable->Set_Value(j,i,row[i]);				break;
			case 16:  MyTable->Set_Value(j,i,0.0);  				break; // Not supported
			case 246:  MyTable->Set_Value(j,i,0.0); 				break; // Not supported
			case 247:  MyTable->Set_Value(j,i,0.0);					break; // Not supported
			case 248:  MyTable->Set_Value(j,i,0.0);					break; // Not supported
			case 249:  MyTable->Set_Value(j,i,0.0);					break; // Not supported
			case 250:  MyTable->Set_Value(j,i,0.0);					break; // Not supported
			case 251:  MyTable->Set_Value(j,i,0.0);					break; // Not supported
			case 252:  MyTable->Set_Value(j,i,0.0);					break; // Not supported
			case 253:  MyTable->Set_Value(j,i,row[i]); 				break;
			case 254:  MyTable->Set_Value(j,i,row[i]); 				break;
			case 255:  MyTable->Set_Value(j,i,0.0);					break; // Not supported
			}
	   }
	   j++;

	}
     mysql_close(conn);

	
	return(true);
}
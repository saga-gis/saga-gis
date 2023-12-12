
///////////////////////////////////////////////////////////
//                                                       //
//                         SAGA                          //
//                                                       //
//      System for Automated Geoscientific Analyses      //
//                                                       //
//                Command Line Interface                 //
//                                                       //
//                   Program: SAGA_CMD                   //
//                                                       //
//-------------------------------------------------------//
//                                                       //
//                        tool.h                         //
//                                                       //
//          Copyright (C) 2005 by Olaf Conrad            //
//                                                       //
//-------------------------------------------------------//
//                                                       //
// This file is part of 'SAGA - System for Automated     //
// Geoscientific Analyses'. SAGA is free software; you   //
// can redistribute it and/or modify it under the terms  //
// of the GNU General Public License as published by the //
// Free Software Foundation, either version 2 of the     //
// License, or (at your option) any later version.       //
//                                                       //
// SAGA is distributed in the hope that it will be       //
// useful, but WITHOUT ANY WARRANTY; without even the    //
// implied warranty of MERCHANTABILITY or FITNESS FOR A  //
// PARTICULAR PURPOSE. See the GNU General Public        //
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
#ifndef _HEADER_INCLUDED__SAGA_CMD__tool_H
#define _HEADER_INCLUDED__SAGA_CMD__tool_H


///////////////////////////////////////////////////////////
//														 //
//														 //
//														 //
///////////////////////////////////////////////////////////

//---------------------------------------------------------
#include <saga_api/saga_api.h>


///////////////////////////////////////////////////////////
//                                                       //
//                                                       //
//                                                       //
///////////////////////////////////////////////////////////

//---------------------------------------------------------
class CCMD_Tool
{
public:
	CCMD_Tool(void);
	CCMD_Tool(CSG_Tool *pTool);
	virtual ~CCMD_Tool(void);

	bool						Create					(CSG_Tool *pTool);
	void						Destroy					(void);

	void						Usage					(void);

	bool						Execute					(int argc, char *argv[]);

	bool						Get_Parameters			(CSG_Parameters *pParameters)	{	return( _Get_Parameters(pParameters, false) );	}


private:

	CSG_Tool					*m_pTool;

	CSG_String					m_Usage;

	CSG_Table					m_Arguments;


	bool						_Parse					(int argc, char *argv[]);

	bool						_has_Unused				(void);

	CSG_Table_Record *			_Found					(const CSG_String &Name);
	bool						_Found					(const CSG_String &Name, CSG_String   &Value);
	bool						_Found					(const CSG_String &Name, int          &Value);
	bool						_Found					(const CSG_String &Name, double       &Value);
	bool						_Found					(const CSG_String &Name, CSG_DateTime &Value);

	bool						_Get_Parameters			(CSG_Parameters *pParameters, bool bInitialize);
	bool						_Get_Options			(CSG_Parameters *pParameters, bool bInitialize);
	bool						_Get_Input				(CSG_Parameters *pParameters);

	bool						_Load_Input				(CSG_Parameter  *pParameter);
	bool						_Save_Output			(CSG_Parameters *pParameters);
	bool						_Save_Output			(CSG_Data_Object *pObject, const CSG_String &FileName);

};


///////////////////////////////////////////////////////////
//														 //
//														 //
//														 //
///////////////////////////////////////////////////////////

//---------------------------------------------------------
#endif // #ifndef _HEADER_INCLUDED__SAGA_CMD__tool_H

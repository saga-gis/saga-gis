
///////////////////////////////////////////////////////////
//                                                       //
//                         SAGA                          //
//                                                       //
//      System for Automated Geoscientific Analyses      //
//                                                       //
//                    Module Library:                    //
//                      Grid_Tools                       //
//                                                       //
//-------------------------------------------------------//
//                                                       //
//                  Grid_Value_Type.cpp                  //
//                                                       //
//                 Copyright (C) 2003 by                 //
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
//                University of Goettingen               //
//                Goldschmidtstr. 5                      //
//                37077 Goettingen                       //
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
#include "Grid_Value_Type.h"


///////////////////////////////////////////////////////////
//														 //
//														 //
//														 //
///////////////////////////////////////////////////////////

//---------------------------------------------------------
CGrid_Value_Type::CGrid_Value_Type(void)
{
	//-----------------------------------------------------
	Set_Name(_TL("Convert Data Storage Type"));

	Set_Author		(SG_T("O.Conrad (c) 2003"));

	Set_Description	(_TW(
		"Changes the storage data type of a grid "
		"(e.g. from 4 byte floating point to 2 byte signed integer). "
		"If the target is not set, the original grid's storage type will be changed. ")
	);

	//-----------------------------------------------------
	Parameters.Add_Grid(
		NULL	, "INPUT"	, _TL("Grid"),
		_TL(""),
		PARAMETER_INPUT
	);

	Parameters.Add_Grid(
		NULL	, "OUTPUT"	, _TL("Converted Grid"),
		_TL(""),
		PARAMETER_OUTPUT_OPTIONAL
	);

	Parameters.Add_Choice(
		NULL	, "TYPE"	, _TL("Data storage type"),
		_TL(""),
		CSG_String::Format(SG_T("%s|%s|%s|%s|%s|%s|%s|%s|%s|"),
			SG_Data_Type_Get_Name(SG_DATATYPE_Bit),
			SG_Data_Type_Get_Name(SG_DATATYPE_Byte),
			SG_Data_Type_Get_Name(SG_DATATYPE_Char),
			SG_Data_Type_Get_Name(SG_DATATYPE_Word),
			SG_Data_Type_Get_Name(SG_DATATYPE_Short),
			SG_Data_Type_Get_Name(SG_DATATYPE_DWord),
			SG_Data_Type_Get_Name(SG_DATATYPE_Int),
			SG_Data_Type_Get_Name(SG_DATATYPE_Float),
			SG_Data_Type_Get_Name(SG_DATATYPE_Double)
		)
	);
}


///////////////////////////////////////////////////////////
//														 //
//														 //
//														 //
///////////////////////////////////////////////////////////

//---------------------------------------------------------
bool CGrid_Value_Type::On_Execute(void)
{
	//-----------------------------------------------------
	CSG_Grid	*pOutput	= Parameters("OUTPUT")	->asGrid();
	CSG_Grid	*pInput		= Parameters("INPUT")	->asGrid();

	if( pOutput == NULL || pOutput == pInput )
	{
		pOutput	= pInput;
		pInput	= new CSG_Grid(*pOutput);
	}

	//-----------------------------------------------------
	switch( Parameters("TYPE")->asInt() )
	{
	default:
	case 0:	pOutput->Create(*Get_System(), SG_DATATYPE_Bit   );	break;
	case 1:	pOutput->Create(*Get_System(), SG_DATATYPE_Byte  );	break;
	case 2:	pOutput->Create(*Get_System(), SG_DATATYPE_Char  );	break;
	case 3:	pOutput->Create(*Get_System(), SG_DATATYPE_Word  );	break;
	case 4:	pOutput->Create(*Get_System(), SG_DATATYPE_Short );	break;
	case 5:	pOutput->Create(*Get_System(), SG_DATATYPE_DWord );	break;
	case 6:	pOutput->Create(*Get_System(), SG_DATATYPE_Int   );	break;
	case 7:	pOutput->Create(*Get_System(), SG_DATATYPE_Float );	break;
	case 8:	pOutput->Create(*Get_System(), SG_DATATYPE_Double);	break;
	}

	//-----------------------------------------------------
	pOutput->Assign(pInput);

	//-----------------------------------------------------
	if( pInput != Parameters("INPUT")->asGrid() )
	{
		delete(pInput);
	}

	return( true );
}


///////////////////////////////////////////////////////////
//														 //
//														 //
//														 //
///////////////////////////////////////////////////////////

//---------------------------------------------------------

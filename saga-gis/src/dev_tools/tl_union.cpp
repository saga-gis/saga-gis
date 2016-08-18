/**********************************************************
 * Version $Id$
 *********************************************************/

///////////////////////////////////////////////////////////
//                                                       //
//                         SAGA                          //
//                                                       //
//      System for Automated Geoscientific Analyses      //
//                                                       //
//                     Tool Library                      //
//                      dev_tools                        //
//                                                       //
//-------------------------------------------------------//
//                                                       //
//                     tl_union.cpp                      //
//                                                       //
//                 Copyright (C) 2010 by                 //
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
//    e-mail:     oconrad@saga-gis.org                   //
//                                                       //
//    contact:    Olaf Conrad                            //
//                Institute for Geography                //
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
#include "tl_union.h"


///////////////////////////////////////////////////////////
//														 //
//														 //
//														 //
///////////////////////////////////////////////////////////

//---------------------------------------------------------
CTL_Union::CTL_Union(void)
{
	//-----------------------------------------------------
	Set_Name		(SG_T("Import Translations"));

	Set_Author		(SG_T("O. Conrad (c) 2010"));

	Set_Description	(SG_T(""));

	//-----------------------------------------------------
	Parameters.Add_Table(
		NULL	, "MASTER"		, SG_T("Translations"),
		SG_T(""),
		PARAMETER_INPUT
	);

	Parameters.Add_Table(
		NULL	, "IMPORT"		, SG_T("Import Translations"),
		SG_T(""),
		PARAMETER_INPUT
	);

	Parameters.Add_Table(
		NULL	, "UNION"		, SG_T("Merged Translations"),
		SG_T(""),
		PARAMETER_OUTPUT
	);

	Parameters.Add_Choice(
		NULL	, "METHOD"		, SG_T("Import Options"),
		SG_T(""),
		CSG_String::Format(SG_T("%s|%s|"),
			SG_T("replace all"),
			SG_T("only add when empty")
		), 1
	);
}


///////////////////////////////////////////////////////////
//														 //
//														 //
//														 //
///////////////////////////////////////////////////////////

//---------------------------------------------------------
bool CTL_Union::On_Execute(void)
{
	//-----------------------------------------------------
	CSG_Translator	Translator;

	if( !Translator.Create(Parameters("IMPORT")->asTable(), false) )
	{
		Error_Set(SG_T("failed to load import translations"));

		return( false );
	}

	//-----------------------------------------------------
	CSG_Table	*pTable	= Parameters("UNION")->asTable();

	pTable->Create(*Parameters("MASTER")->asTable());

	if( pTable->Get_Count() < 1 || pTable->Get_Field_Count() < 2 )
	{
		Error_Set(SG_T("invalid master table"));

		return( false );
	}

	int	Method	= Parameters("METHOD")->asInt();

	//-----------------------------------------------------
	for(int i=0; i<pTable->Get_Count() && Set_Progress(i, pTable->Get_Count()); i++)
	{
		CSG_Table_Record	*pRecord	= pTable->Get_Record(i);

		CSG_String	Translation;

		switch( Method )
		{
		default:	// replace all
			if( Translator.Get_Translation(pRecord->asString(0), Translation) )
			{
				pRecord->Set_Value(1, Translation);
			}
			break;

		case  1:	// only add to empty entries
			Translation	= pRecord->asString(1);

			if( Translation.is_Empty() && Translator.Get_Translation(pRecord->asString(0), Translation) )
			{
				pRecord->Set_Value(1, Translation);
			}
			break;
		}
	}

	//-----------------------------------------------------
	return( true );
}


///////////////////////////////////////////////////////////
//														 //
//														 //
//														 //
///////////////////////////////////////////////////////////

//---------------------------------------------------------


///////////////////////////////////////////////////////////
//                                                       //
//                         SAGA                          //
//                                                       //
//      System for Automated Geoscientific Analyses      //
//                                                       //
//                    Module Library:                    //
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
// 59 Temple Place - Suite 330, Boston, MA 02111-1307,   //
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
	CSG_String	Filter;
	
	Filter.Printf(SG_T("%s|*.lng;*.txt|%s|*.lng|%s|*.txt|%s|*.*"),
		SG_T("All recognized Files"),
		SG_T("SAGA Translation File (*.lng)"),
		SG_T("Text Files (*.txt)"),
		SG_T("All Files")
	);

	//-----------------------------------------------------
	Parameters.Add_Table(
		NULL	, "TARGET"		, SG_T("Translation Table"),
		SG_T(""),
		PARAMETER_OUTPUT
	);

	Parameters.Add_FilePath(
		NULL	, "FILE"		, SG_T("Existing Translations"),
		SG_T(""),
		Filter, NULL
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
	int				Method;
	CSG_String		File, Translation;
	CSG_Translator	Translator;
	CSG_Table		*pTarget;

	//-----------------------------------------------------
	pTarget	= Parameters("TARGET")	->asTable();
	File	= Parameters("FILE")	->asString();
	Method	= Parameters("METHOD")	->asInt();

	//-----------------------------------------------------
	if( pTarget->Get_Count() < 1 || pTarget->Get_Field_Count() < 2 )
	{
		Error_Set(SG_T("invalid target table"));

		return( false );
	}

	//-----------------------------------------------------
	if( !Translator.Create(File, false) )
	{
		Error_Set(SG_T("failed to load translation file"));

		return( false );
	}

	//-----------------------------------------------------
	for(int i=0; i<pTarget->Get_Count() && Set_Progress(i, pTarget->Get_Count()); i++)
	{
		CSG_Table_Record	*pRecord	= pTarget->Get_Record(i);

		switch( Method )
		{
		case 0: default:	// replace all
			if( Translator.Get_Translation(pRecord->asString(0), Translation) )
			{
				pRecord->Set_Value(1, Translation);
			}
			break;

		case 1:				// only add to empty entries
			Translation	= pRecord->asString(1);

			if( Translation.Length() == 0 && Translator.Get_Translation(pRecord->asString(0), Translation) )
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

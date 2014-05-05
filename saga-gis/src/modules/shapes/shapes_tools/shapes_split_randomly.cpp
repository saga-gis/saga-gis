/**********************************************************
 * Version $Id$
 *********************************************************/

///////////////////////////////////////////////////////////
//                                                       //
//                         SAGA                          //
//                                                       //
//      System for Automated Geoscientific Analyses      //
//                                                       //
//                    Module Library:                    //
//                     shapes_tools                      //
//                                                       //
//-------------------------------------------------------//
//                                                       //
//               Shapes_Split_Randomly.cpp               //
//                                                       //
//                 Copyright (C) 2008 by                 //
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

///////////////////////////////////////////////////////////
//														 //
//														 //
//														 //
///////////////////////////////////////////////////////////

//---------------------------------------------------------
#include <time.h>

#include "shapes_split_randomly.h"


///////////////////////////////////////////////////////////
//														 //
//														 //
//														 //
///////////////////////////////////////////////////////////

//---------------------------------------------------------
CShapes_Split_Randomly::CShapes_Split_Randomly(void)
{
	Set_Name		(_TL("Split Shapes Layer Randomly"));

	Set_Author		(SG_T("(c) 2008 by O.Conrad"));

	Set_Description	(_TW(
		"Randomly splits one shapes layer into to new shapes layers. "
		"Useful to create a control group for model testing. "
	));

	//-----------------------------------------------------
	Parameters.Add_Shapes(
		NULL	, "SHAPES"		, _TL("Shapes"),
		_TL(""),
		PARAMETER_INPUT
	);

	Parameters.Add_Shapes(
		NULL	, "A"			, _TL("Group A"),
		_TL(""),
		PARAMETER_OUTPUT
	);

	Parameters.Add_Shapes(
		NULL	, "B"			, _TL("Group B"),
		_TL(""),
		PARAMETER_OUTPUT
	);

	Parameters.Add_Value(
		NULL	, "PERCENT"		, _TL("Relation B / A"),
		_TL(""),
		PARAMETER_TYPE_Double, 25.0, 0.0, true, 100.0, true
	);

	Parameters.Add_Value(
		NULL	, "EXACT"		, _TL("Exact"),
		_TL(""),
		PARAMETER_TYPE_Bool, true
	);
}


///////////////////////////////////////////////////////////
//														 //
//														 //
//														 //
///////////////////////////////////////////////////////////

//---------------------------------------------------------
bool CShapes_Split_Randomly::On_Execute(void)
{
	//-----------------------------------------------------
	CSG_Shapes	*pSplit[2], *pShapes	= Parameters("SHAPES")->asShapes();

	if( !pShapes->is_Valid() )
	{
		Error_Set(SG_T("invalid input"));

		return( false );
	}

	//-----------------------------------------------------
	double	Percent	= Parameters("PERCENT")->asDouble();

	pSplit[0]	= Parameters("A")->asShapes();
	pSplit[1]	= Parameters("B")->asShapes();

	pSplit[0]->Create(pShapes->Get_Type(), CSG_String::Format(SG_T("%s [%d%%]"), pShapes->Get_Name(), (int)(100.5 - Percent)), pShapes);
	pSplit[1]->Create(pShapes->Get_Type(), CSG_String::Format(SG_T("%s [%d%%]"), pShapes->Get_Name(), (int)(  0.5 + Percent)), pShapes);

	//-----------------------------------------------------
	if( !Parameters("EXACT")->asBool() )
	{
		for(int i=0; i<pShapes->Get_Count() && Set_Progress(i, pShapes->Get_Count()); i++)
		{
			pSplit[Percent >= CSG_Random::Get_Uniform(0, 100) ? 1 : 0]->Add_Shape(pShapes->Get_Shape(i));
		}
	}

	//-----------------------------------------------------
	else
	{
		int		i, n	= (int)(0.5 + pShapes->Get_Count() * Percent / 100.0);

		CSG_Table	Random;

		Random.Add_Field("INDEX"  , SG_DATATYPE_Int);
		Random.Add_Field("PERCENT", SG_DATATYPE_Double);

		for(i=0; i<pShapes->Get_Count() && Set_Progress(i, pShapes->Get_Count()); i++)
		{
			CSG_Table_Record	*pRecord	= Random.Add_Record();

			pRecord->Set_Value(0, i);
			pRecord->Set_Value(1, CSG_Random::Get_Uniform(0, 100));
		}

		Random.Set_Index(1, TABLE_INDEX_Ascending);

		//-------------------------------------------------
		for(i=0; i<pShapes->Get_Count() && Set_Progress(i, pShapes->Get_Count()); i++)
		{
			pSplit[i < n ? 1 : 0]->Add_Shape(pShapes->Get_Shape(Random[i].asInt(0)));
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

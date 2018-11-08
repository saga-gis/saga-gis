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
// Free Software Foundation, either version 2 of the     //
// License, or (at your option) any later version.       //
//                                                       //
// SAGA is distributed in the hope that it will be       //
// useful, but WITHOUT ANY WARRANTY; without even the    //
// implied warranty of MERCHANTABILITY or FITNESS FOR A  //
// PARTICULAR PURPOSE. See the GNU General Public        //
// License for more details.                             //
//                                                       //
// You should have received a copy of the GNU General    //
// Public License along with this program; if not, see   //
// <http://www.gnu.org/licenses/>.                       //
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

	Set_Author		("O.Conrad (c) 2008");

	Set_Description	(_TW(
		"Randomly splits one layer into to two new layers. "
		"Useful to create a control group for model testing. "
		"Optionally this can be done category-wise if a category "
		"field is specified. "
	));

	//-----------------------------------------------------
	Parameters.Add_Shapes("",
		"SHAPES"	, _TL("Shapes"),
		_TL(""),
		PARAMETER_INPUT
	);

	Parameters.Add_Table_Field("SHAPES",
		"FIELD"		, _TL("Categories"),
		_TL(""),
		true
	);

	Parameters.Add_Shapes("",
		"A"			, _TL("Group A"),
		_TL(""),
		PARAMETER_OUTPUT
	);

	Parameters.Add_Shapes("",
		"B"			, _TL("Group B"),
		_TL(""),
		PARAMETER_OUTPUT
	);

	Parameters.Add_Double("",
		"PERCENT"	, _TL("Relation B / A"),
		_TL(""),
		25.0, 0.0, true, 100.0, true
	);

	Parameters.Add_Bool("",
		"EXACT"		, _TL("Exact"),
		_TL(""),
		true
	);
}


///////////////////////////////////////////////////////////
//														 //
///////////////////////////////////////////////////////////

//---------------------------------------------------------
bool CShapes_Split_Randomly::On_Execute(void)
{
	//-----------------------------------------------------
	CSG_Shapes	*pSplit[2], *pShapes	= Parameters("SHAPES")->asShapes();

	if( !pShapes->is_Valid() )
	{
		Error_Set("invalid input");

		return( false );
	}

	//-----------------------------------------------------
	double	Percent	= Parameters("PERCENT")->asDouble();

	pSplit[0]	= Parameters("A")->asShapes();
	pSplit[1]	= Parameters("B")->asShapes();

	pSplit[0]->Create(pShapes->Get_Type(), CSG_String::Format("%s [%d%%]", pShapes->Get_Name(), (int)(100.5 - Percent)), pShapes);
	pSplit[1]->Create(pShapes->Get_Type(), CSG_String::Format("%s [%d%%]", pShapes->Get_Name(), (int)(  0.5 + Percent)), pShapes);

	CSG_Random::Initialize();	// initialize with current time

	//-----------------------------------------------------
	int	Field	= Parameters("FIELD")->asInt();

	if( Field < 0 )
	{
		Split(pShapes, pSplit, Percent);
	}
	else	// stratified
	{
		CSG_String	Category;
		CSG_Shapes	Shapes(pShapes->Get_Type(), pShapes->Get_Name(), pShapes);

		pShapes->Set_Index(Field, TABLE_INDEX_Ascending);

		for(int i=0; i<pShapes->Get_Count() && Set_Progress(i, pShapes->Get_Count()); i++)
		{
			CSG_Shape	*pShape	= pShapes->Get_Shape_byIndex(i);

			if( Shapes.Get_Count() == 0 )
			{
				Category	= pShape->asString(Field);
			}
			else if( Category.Cmp(pShape->asString(Field)) )	// category changed
			{
				Category	= pShape->asString(Field);

				Split(&Shapes, pSplit, Percent);

				Shapes.Del_Shapes();
			}

			Shapes.Add_Shape(pShape);
		}

		Split(&Shapes, pSplit, Percent);	// don't forget to split last category
	}

	//-----------------------------------------------------
	return( true );
}


///////////////////////////////////////////////////////////
//														 //
///////////////////////////////////////////////////////////

//---------------------------------------------------------
void CShapes_Split_Randomly::Split(CSG_Shapes *pShapes, CSG_Shapes *pSplit[2], double Percent)
{
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
}


///////////////////////////////////////////////////////////
//														 //
//														 //
//														 //
///////////////////////////////////////////////////////////

//---------------------------------------------------------

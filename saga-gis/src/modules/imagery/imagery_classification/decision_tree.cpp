/**********************************************************
 * Version $Id: decision_tree.cpp 1081 2011-06-08 08:05:26Z reklov_w $
 *********************************************************/

///////////////////////////////////////////////////////////
//                                                       //
//                         SAGA                          //
//                                                       //
//      System for Automated Geoscientific Analyses      //
//                                                       //
//                    Module Library:                    //
//                imagery_classification                 //
//                                                       //
//-------------------------------------------------------//
//                                                       //
//                   decision_tree.cpp                   //
//                                                       //
//                 Copyright (C) 2011 by                 //
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
#include "decision_tree.h"


///////////////////////////////////////////////////////////
//														 //
//														 //
//														 //
///////////////////////////////////////////////////////////

//---------------------------------------------------------
CDecision_Tree::CDecision_Tree(void)
{
	//-----------------------------------------------------
	Set_Name		(_TL("Decision Tree"));

	Set_Author		(SG_T("O.Conrad (c) 2011"));

	Set_Description	(_TW(
		"Decision Tree"
	));

	//-----------------------------------------------------
	Parameters.Add_Grid(
		NULL	, "CLASSES"		, _TL("Decision Tree"),
		_TL(""),
		PARAMETER_OUTPUT, true, SG_DATATYPE_Short
	);

	//-----------------------------------------------------
	CSG_Parameter	*pRoot	= Parameters.Add_Parameters(
		NULL	, "ROOT"		, _TL("Decision"),
		_TL("")
	);

	Add_Decision(pRoot->asParameters());
}


///////////////////////////////////////////////////////////
//														 //
//														 //
//														 //
///////////////////////////////////////////////////////////

//---------------------------------------------------------
bool CDecision_Tree::Add_Decision(CSG_Parameters *pDecision)
{
	if( !pDecision || pDecision->Get_Count() > 0 )
	{
		return( false );
	}

	CSG_String		ID, ID_Root	= pDecision->Get_Identifier(); if( !ID_Root.Cmp(SG_T("ROOT")) ) ID_Root.Clear();
	CSG_Parameter	*pNode;

	pNode	= pDecision->Add_Grid(
		NULL	, "GRID"			, _TL("Grid"),
		_TL(""),
		PARAMETER_INPUT, false
	);

	pDecision->Add_Value(
		pNode	, "THRESHOLD"		, _TL("Threshold"),
		_TL(""),
		PARAMETER_TYPE_Double
	);

	//-----------------------------------------------------
	ID	= ID_Root + SG_T("A");

	pNode	= pDecision->Add_Node(
		NULL	, ID + SG_T("|A")	, _TL("Lower"),
		_TL("")
	);

	pDecision->Add_String(
		pNode	, ID + SG_T("|NAME"), _TL("Name"),
		_TL(""),
		ID
	);

	pDecision->Add_Value(
		pNode	, ID + SG_T("|NODE"), _TL("Children"),
		_TL(""),
		PARAMETER_TYPE_Bool, false
	);

	pDecision->Add_Parameters(
		pNode	, ID				, _TL("Decision"),
		_TL("")
	)->asParameters()->Set_Name(ID);

	//-----------------------------------------------------
	ID	= ID_Root + SG_T("B");

	pNode	= pDecision->Add_Node(
		NULL	, ID + SG_T("|B")	, _TL("Higher"),
		_TL("")
	);

	pDecision->Add_String(
		pNode	, ID + SG_T("|NAME"), _TL("Name"),
		_TL(""),
		ID
	);

	pDecision->Add_Value(
		pNode	, ID + SG_T("|NODE"), _TL("Children"),
		_TL(""),
		PARAMETER_TYPE_Bool, false
	);

	pDecision->Add_Parameters(
		pNode	, ID				, _TL("Decision"),
		_TL("")
	);

	return( true );
}


///////////////////////////////////////////////////////////
//														 //
//														 //
//														 //
///////////////////////////////////////////////////////////

//---------------------------------------------------------
int CDecision_Tree::On_Parameter_Changed(CSG_Parameters *pParameters, CSG_Parameter *pParameter)
{
	if( CSG_String(pParameter->Get_Identifier()).Find(SG_T("NODE")) >= 0 )
	{
		CSG_String	ID	= CSG_String(pParameter->Get_Identifier()).BeforeFirst(SG_T('|'));

		if( pParameter->asBool() )
		{
			CSG_Parameters	*pDecision	= pParameters->Get_Parameter(ID)->asParameters();

			pDecision->Set_Name(CSG_String::Format(SG_T("%s|%s"), pParameters->Get_Name().c_str(), pParameters->Get_Parameter(ID + SG_T("|NAME"))->asString()));

			Add_Decision(pDecision);
		}
		else
		{
			pParameters->Get_Parameter(ID)->asParameters()->Del_Parameters();
		}
	}

	if( !SG_STR_CMP(pParameter->Get_Identifier(), SG_T("GRID")) )
	{
		if( pParameter->asGrid() )
		{
			pParameters->Get_Parameter("THRESHOLD")->Set_Value(pParameter->asGrid()->Get_ArithMean());
		}
	}

	return( 0 );
}

//---------------------------------------------------------
int CDecision_Tree::On_Parameters_Enable(CSG_Parameters *pParameters, CSG_Parameter *pParameter)
{
	if( CSG_String(pParameter->Get_Identifier()).Find(SG_T("NODE")) >= 0 )
	{
		CSG_String	ID	= CSG_String(pParameter->Get_Identifier()).BeforeFirst(SG_T('|'));

		pParameters->Get_Parameter(ID)->Set_Enabled(pParameter->asBool());
	}

	return( 0 );
}


///////////////////////////////////////////////////////////
//														 //
//														 //
//														 //
///////////////////////////////////////////////////////////

//---------------------------------------------------------
bool CDecision_Tree::On_Execute(void)
{
	CSG_Grid	*pClasses;

	//-----------------------------------------------------
	pClasses	= Parameters("CLASSES")	->asGrid();
	pClasses	->Set_NoData_Value(-1);

	//-----------------------------------------------------
	for(int y=0; y<Get_NY() && Set_Progress(y); y++)
	{
		for(int x=0; x<Get_NX(); x++)
		{
			pClasses->Set_Value(x, y, Get_Class(Parameters("ROOT")->asParameters(), Get_System()->Get_Grid_to_World(x, y)));
		}
	}

	//-----------------------------------------------------
	CSG_Parameters	P;

	if( DataObject_Get_Parameters(pClasses, P) && P("COLORS_TYPE") && P("LUT") )
	{
		CSG_Table	*pTable	= P("LUT")->asTable();

		pTable->Del_Records();

		Get_Class(Parameters("ROOT")->asParameters(), pTable);

		P("COLORS_TYPE")->Set_Value(1);	// Color Classification Type: Lookup Table

		DataObject_Set_Parameters(pClasses, P);
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
int CDecision_Tree::Get_Class(const CSG_String &ID)
{
	int		Class	= 0;

	for(int i=0, j=1; i<ID.Length(); i++, j*=2)
	{
		if( ID[i] == SG_T('B') )
		{
			Class	+= j;
		}
	}

	return( Class );
}

//---------------------------------------------------------
int CDecision_Tree::Get_Class(CSG_Parameters *pDecision, const TSG_Point &Point)
{
	double		Value;
	CSG_Grid	*pGrid	= pDecision->Get_Parameter("GRID")->asGrid();

	if( pGrid && pGrid->Get_Value(Point, Value) )
	{
		CSG_String	ID	= pDecision->Get_Identifier(); if( !ID.Cmp(SG_T("ROOT")) ) ID.Clear();

		ID	+= Value < pDecision->Get_Parameter("THRESHOLD")->asDouble() ? SG_T("A") : SG_T("B");

		if( pDecision->Get_Parameter(ID + SG_T("|NODE"))->asBool() )
		{
			return( Get_Class(pDecision->Get_Parameter(ID)->asParameters(), Point) );
		}
		else
		{
			return( Get_Class(ID) );
		}
	}

	return( -1 );
}

//---------------------------------------------------------
int CDecision_Tree::Get_Class(CSG_Parameters *pDecision, CSG_Table *pTable)
{
	CSG_String	ID_Root	= pDecision->Get_Identifier(); if( !ID_Root.Cmp(SG_T("ROOT")) ) ID_Root.Clear();

	for(int Child=0; Child<=1; Child++)
	{
		CSG_String	ID	= ID_Root + (Child == 0 ? SG_T("A") : SG_T("B"));

		if( pDecision->Get_Parameter(ID + SG_T("|NODE"))->asBool() )
		{
			Get_Class(pDecision->Get_Parameter(ID)->asParameters(), pTable);
		}
		else
		{
			CSG_Table_Record	*pRecord	= pTable->Add_Record();

			pRecord->Set_Value(0, SG_GET_RGB(rand() * 255.0 / RAND_MAX, rand() * 255.0 / RAND_MAX, rand() * 255.0 / RAND_MAX));
			pRecord->Set_Value(1, pDecision->Get_Parameter(ID + SG_T("|NAME"))->asString());
			pRecord->Set_Value(2, pDecision->Get_Parameter(ID + SG_T("|NAME"))->asString());
			pRecord->Set_Value(3, Get_Class(ID));
			pRecord->Set_Value(4, Get_Class(ID));
		}
	}

	return( pTable->Get_Count() );
}


///////////////////////////////////////////////////////////
//														 //
//														 //
//														 //
///////////////////////////////////////////////////////////

//---------------------------------------------------------

/*******************************************************************************
    NewLayerFromSelectedShapes.cpp
    Copyright (C) Victor Olaya
    
    This program is free software; you can redistribute it and/or modify
    it under the terms of the GNU General Public License as published by
    the Free Software Foundation; either version 2 of the License, or
    (at your option) any later version.

    This program is distributed in the hope that it will be useful,
    but WITHOUT ANY WARRANTY; without even the implied warranty of
    MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
    GNU General Public License for more details.

    You should have received a copy of the GNU General Public License
    along with this program; if not, write to the Free Software
    Foundation, Inc., 59 Temple Place, Suite 330, Boston, MA  02111-1307  USA
*******************************************************************************/ 


///////////////////////////////////////////////////////////
//														 //
//														 //
//														 //
///////////////////////////////////////////////////////////

//---------------------------------------------------------
#include "NewLayerFromSelectedShapes.h"


///////////////////////////////////////////////////////////
//														 //
//														 //
//														 //
///////////////////////////////////////////////////////////

//---------------------------------------------------------
CSelection_Copy::CSelection_Copy(void)
{	
	Set_Name		(_TL("Copy Selection to New Shapes Layer"));

	Set_Author		(SG_T("Victor Olaya (c) 2004"));

	Set_Description	(_TW(
		"Copies selected shapes to new shapes layer."
	));

	Parameters.Add_Shapes(
		NULL	, "INPUT"	, _TL("Input"), 
		_TL(""),
		PARAMETER_INPUT
	);

	Parameters.Add_Shapes(
		NULL	, "OUTPUT"	, _TL("Output"),
		_TL(""),
		PARAMETER_OUTPUT
	);
}

//---------------------------------------------------------
bool CSelection_Copy::On_Execute(void)
{
	CSG_Shapes	*pInput, *pOutput;

	pInput	= Parameters("INPUT") ->asShapes();
	pOutput	= Parameters("OUTPUT")->asShapes();

	if( pInput->Get_Selection_Count() <= 0 )
	{
		Error_Set(_TL("no shapes in selection"));

		return( false );
	}

	if( pOutput->Get_Type() != SHAPE_TYPE_Undefined && pOutput->Get_Type() != pInput->Get_Type() )
	{
		Parameters("OUTPUT")->Set_Value(pOutput	= SG_Create_Shapes());
	}

	pOutput->Create(pInput->Get_Type(), CSG_String::Format(SG_T("%s [%s]"), pInput->Get_Name(), _TL("Selection")), pInput);

	for(int i=0; i<pInput->Get_Selection_Count() && Set_Progress(i, pInput->Get_Selection_Count()); i++)
	{
		pOutput->Add_Shape(pInput->Get_Selection(i));
	}

	return( true );
}


///////////////////////////////////////////////////////////
//														 //
//														 //
//														 //
///////////////////////////////////////////////////////////

//---------------------------------------------------------
CSelection_Delete::CSelection_Delete(void)
{	
	Set_Name		(_TL("Delete Selection from Shapes Layer"));

	Set_Author		(SG_T("O.Conrad (c) 2011"));

	Set_Description	(_TW(
		"Deletes selected shapes from shapes layer."
	));

	Parameters.Add_Shapes(
		NULL	, "INPUT"	, _TL("Input"), 
		_TL(""),
		PARAMETER_INPUT
	);
}

//---------------------------------------------------------
bool CSelection_Delete::On_Execute(void)
{
	CSG_Shapes	*pInput;

	pInput	= Parameters("INPUT") ->asShapes();

	if( pInput->Get_Selection_Count() <= 0 )
	{
		Error_Set(_TL("no shapes in selection"));

		return( false );
	}

	pInput->Del_Selection();

	DataObject_Update(pInput);

	return( true );
}


///////////////////////////////////////////////////////////
//														 //
//														 //
//														 //
///////////////////////////////////////////////////////////

//---------------------------------------------------------
CSelection_Invert::CSelection_Invert(void)
{	
	Set_Name		(_TL("Invert Selection of Shapes Layer"));

	Set_Author		(SG_T("O.Conrad (c) 2011"));

	Set_Description	(_TW(
		"Deselects selected and selects unselected shapes of given shapes layer."
	));

	Parameters.Add_Shapes(
		NULL	, "INPUT"	, _TL("Input"), 
		_TL(""),
		PARAMETER_INPUT
	);
}

//---------------------------------------------------------
bool CSelection_Invert::On_Execute(void)
{
	CSG_Shapes	*pInput;

	pInput	= Parameters("INPUT") ->asShapes();

	pInput->Inv_Selection();

	DataObject_Update(pInput);

	return( true );
}


///////////////////////////////////////////////////////////
//														 //
//														 //
//														 //
///////////////////////////////////////////////////////////

//---------------------------------------------------------

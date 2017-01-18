
///////////////////////////////////////////////////////////
//                                                       //
//                         SAGA                          //
//                                                       //
//      System for Automated Geoscientific Analyses      //
//                                                       //
//                     Tool Library                      //
//                      Grid_Tools                       //
//                                                       //
//-------------------------------------------------------//
//                                                       //
//                Grid_Combine_Classes.cpp               //
//                                                       //
//                 Copyright (C) 2017 by                 //
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
#include "Grid_Combine_Classes.h"


///////////////////////////////////////////////////////////
//														 //
//														 //
//														 //
///////////////////////////////////////////////////////////

//---------------------------------------------------------
CGrid_Combine_Classes::CGrid_Combine_Classes(void)
{
	//-----------------------------------------------------
	Set_Name		(_TL("Combine Classes"));

	Set_Author		("O.Conrad (c) 2017");

	Set_Description	(_TW(
		"Based on the look-up table classfication of a grid, this tool "
		"allows to change and combine class belongings of the cells. "
	));

	//-----------------------------------------------------
	Parameters.Add_Grid(NULL,
		"GRID"		, _TL("Grid"),
		_TL(""),
		PARAMETER_INPUT
	);

	Parameters.Add_Grid(NULL,
		"OUTPUT"	, _TL("Output"),
		_TL(""),
		PARAMETER_OUTPUT_OPTIONAL
	);

	Parameters.Add_Parameters(NULL,
		"CLASSES"	, _TL("Classes"),
		_TL("")
	);
}


///////////////////////////////////////////////////////////
//														 //
///////////////////////////////////////////////////////////

//---------------------------------------------------------
int CGrid_Combine_Classes::On_Parameter_Changed(CSG_Parameters *pParameters, CSG_Parameter *pParameter)
{
	if(	!SG_STR_CMP(pParameter->Get_Identifier(), "GRID") )
	{
		Set_Classes(pParameters);
	}

	//-----------------------------------------------------
	return( CSG_Tool_Grid::On_Parameter_Changed(pParameters, pParameter) );
}

//---------------------------------------------------------
int CGrid_Combine_Classes::On_Parameters_Enable(CSG_Parameters *pParameters, CSG_Parameter *pParameter)
{
	if(	!SG_STR_CMP(pParameter->Get_Identifier(), "GRID") )
	{
		pParameters->Set_Enabled("OUTPUT" , pParameter->asGrid() != NULL);
		pParameters->Set_Enabled("CLASSES", pParameter->asGrid() != NULL);
	}

	//-----------------------------------------------------
	return( CSG_Tool_Grid::On_Parameters_Enable(pParameters, pParameter) );
}


///////////////////////////////////////////////////////////
//														 //
///////////////////////////////////////////////////////////

//---------------------------------------------------------
bool CGrid_Combine_Classes::Set_Classes(CSG_Parameters *pParameters)
{
	CSG_Parameter	*pLUT	= DataObject_Get_Parameter(pParameters->Get_Parameter("GRID")->asGrid(), "LUT");

	if( !pLUT || !pLUT->asTable() )
	{
		return( false );
	}

	CSG_Table	&LUT	= *pLUT->asTable();

	//-----------------------------------------------------
	int		i;

	CSG_String	Classes;

	for(i=0; i<LUT.Get_Count(); i++)
	{
		if( i > 0 )
		{
			Classes	+= '|';
		}

		Classes	+= LUT[i].asString(1);
	}

	//-----------------------------------------------------
	CSG_Parameters	*pClasses	= pParameters->Get_Parameter("CLASSES")->asParameters();

	pClasses->Del_Parameters();

	for(i=0; i<LUT.Get_Count(); i++)
	{
		pClasses->Add_Choice(NULL,
			CSG_String::Format("CLASS%d", i), LUT[i].asString(1), "", Classes, i
		);
	}

	//-----------------------------------------------------
	return( true );
}


///////////////////////////////////////////////////////////
//														 //
///////////////////////////////////////////////////////////

//---------------------------------------------------------
bool CGrid_Combine_Classes::On_Execute(void)
{
	//-----------------------------------------------------
	CSG_Parameter	*pLUT	= DataObject_Get_Parameter(Parameters("GRID")->asGrid(), "LUT");

	if( !pLUT || !pLUT->asTable() )
	{
		Message_Add(_TL("unexpected error"));

		return( false );
	}

	CSG_Table	LUT	= *pLUT->asTable();

	//-----------------------------------------------------
	CSG_Parameters	&Classes	= *Parameters("CLASSES")->asParameters();

	if( Classes.Get_Count() != LUT.Get_Count() || LUT.Get_Count() == 0 )
	{
		Message_Add(_TL("unexpected error"));

		return( false );
	}
	else
	{
		bool	bCombine	= false;

		for(int i=0; !bCombine && i<Classes.Get_Count(); i++)
		{
			bCombine	= i != Classes[i].asInt();
		}

		if( !bCombine )
		{
			Error_Set(_TL("no classes to combine"));

			return( false );
		}
	}

	//-----------------------------------------------------
	CSG_Grid	*pGrid	= Parameters("OUTPUT")->asGrid();

	if( !pGrid )
	{
		pGrid	= Parameters("GRID")->asGrid();
	}
	else if( pGrid != Parameters("GRID")->asGrid() )
	{
		pGrid->Create(*Parameters("GRID")->asGrid());
	}

	//-----------------------------------------------------
	for(int y=0; y<Get_NY() && Set_Progress(y); y++)
	{
		#ifndef _DEBUG
		#pragma omp parallel for
		#endif
		for(int x=0; x<Get_NX(); x++)
		{
			int	i	= Get_Class(LUT, pGrid->asDouble(x, y));

			if( i >= 0 && i != Classes[i].asInt() )
			{
				pGrid->Set_Value(x, y, LUT[Classes[i].asInt()].asDouble(3));
			}
		}
	}

	//-----------------------------------------------------
	pLUT	= DataObject_Get_Parameter(pGrid, "LUT");

	pLUT->asTable()->Del_Records();

	for(int i=0; i<LUT.Get_Count(); i++)
	{
		bool	bAdd	= false;

		for(int j=0; !bAdd && j<Classes.Get_Count(); j++)
		{
			bAdd	= i == Classes[j].asInt();
		}

		if( bAdd )
		{
			pLUT->asTable()->Add_Record(LUT.Get_Record(i));
		}
	}

	DataObject_Set_Parameter(pGrid, pLUT);
	DataObject_Set_Parameter(pGrid, "COLORS_TYPE", 1);	// Color Classification Type: Lookup Table

	//-----------------------------------------------------
	if( pGrid != Parameters("GRID")->asGrid() )
	{
		pGrid->Set_Name(CSG_String::Format("%s [%s]", Parameters("GRID")->asGrid()->Get_Name(), _TL("Combine Classes")));
	}
	else
	{
		Set_Classes(&Parameters);
	}

	//-----------------------------------------------------
	return( true );
}


///////////////////////////////////////////////////////////
//														 //
///////////////////////////////////////////////////////////

//---------------------------------------------------------
int CGrid_Combine_Classes::Get_Class(const CSG_Table &LUT, double Value)
{
	for(int i=0; i<LUT.Get_Count(); i++)
	{
		if( LUT[i].asDouble(3) <= Value && Value <= LUT[i].asDouble(4) )
		{
			return( i );
		}
	}

	return( -1 );
}


///////////////////////////////////////////////////////////
//														 //
//														 //
//														 //
///////////////////////////////////////////////////////////

//---------------------------------------------------------

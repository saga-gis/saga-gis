/**********************************************************
 * Version $Id: MBASpline_for_Categories.cpp 1921 2014-01-09 10:24:11Z oconrad $
 *********************************************************/

///////////////////////////////////////////////////////////
//                                                       //
//                         SAGA                          //
//                                                       //
//      System for Automated Geoscientific Analyses      //
//                                                       //
//                    Module Library:                    //
//                     grid_spline                       //
//                                                       //
//-------------------------------------------------------//
//                                                       //
//               MBASpline_for_Categories.cpp            //
//                                                       //
//                 Copyright (C) 2015 by                 //
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
#include "MBASpline_for_Categories.h"


///////////////////////////////////////////////////////////
//														 //
//														 //
//														 //
///////////////////////////////////////////////////////////

//---------------------------------------------------------
CMBASpline_for_Categories::CMBASpline_for_Categories(void)
{
	Set_Name		(_TL("Multilevel B-Spline Interpolation for Categories"));

	Set_Author		("O.Conrad (c) 2015");

	Set_Description	(_TW(
		""
	));

	//-----------------------------------------------------
	CSG_Parameter	*pNode	= Parameters.Add_Shapes(
		NULL	, "POINTS"	, _TL("Points"),
		_TL(""),
		PARAMETER_INPUT
	);

	Parameters.Add_Table_Field(
		pNode	, "FIELD"	, _TL("Attribute"),
		_TL("")
	);

	//-----------------------------------------------------
	m_Grid_Target.Create(&Parameters, false, NULL, "TARGET_");

	m_Grid_Target.Add_Grid("CATEGORIES" , _TL("Categories" ), false);
	m_Grid_Target.Add_Grid("PROPABILITY", _TL("Propability"), false);
}


///////////////////////////////////////////////////////////
//														 //
///////////////////////////////////////////////////////////

//---------------------------------------------------------
int CMBASpline_for_Categories::On_Parameter_Changed(CSG_Parameters *pParameters, CSG_Parameter *pParameter)
{
	if( !SG_STR_CMP(pParameter->Get_Identifier(), "POINTS") )
	{
		m_Grid_Target.Set_User_Defined(pParameters, pParameter->asShapes());
	}

	return( m_Grid_Target.On_Parameter_Changed(pParameters, pParameter) ? 1 : 0 );
}

//---------------------------------------------------------
int CMBASpline_for_Categories::On_Parameters_Enable(CSG_Parameters *pParameters, CSG_Parameter *pParameter)
{
	return( m_Grid_Target.On_Parameters_Enable(pParameters, pParameter) ? 1 : 0 );
}


///////////////////////////////////////////////////////////
//														 //
///////////////////////////////////////////////////////////

//---------------------------------------------------------
bool CMBASpline_for_Categories::On_Execute(void)
{
	CSG_Shapes	Points;

	//-----------------------------------------------------
	SG_RUN_MODULE_ExitOnError("table_tools", 20,	// Add Indicator Fields for Categories
			SG_MODULE_PARAMETER_SET("TABLE"     , Parameters("POINTS")->asShapes())
		&&	SG_MODULE_PARAMETER_SET("FIELD"     , Parameters("FIELD"))
		&&	SG_MODULE_PARAMETER_SET("OUT_SHAPES", &Points)	// >> Indicators
	)

	int	nCategories	= Points.Get_Field_Count() - 1;

	if( nCategories < 2 )
	{
		Error_Set(_TL("found less than two categories, nothing to do"));

		return( false );
	}

	//-----------------------------------------------------
	CSG_Grid	*pGrid, *pProp, Prop;

	m_Grid_Target.Cmd_Update(&Points);	// if called from saga_cmd

	if( !(pGrid = m_Grid_Target.Get_Grid("CATEGORIES", nCategories < 128 ? SG_DATATYPE_Char : SG_DATATYPE_Int))
	||  !(pProp = m_Grid_Target.Get_Grid("PROPABILITY")) )
	{
		return( false );
	}

	//-----------------------------------------------------
	CSG_Parameter	*pLUT	= DataObject_Get_Parameter(pGrid, "LUT");

	if( pLUT && pLUT->asTable() )
	{
		CSG_Colors	Colors(nCategories);	Colors.Random();

		CSG_Table	*pClasses	= pLUT->asTable();

		pClasses->Set_Record_Count(nCategories);

		for(int iClass=0; iClass<nCategories; iClass++)
		{
			CSG_Table_Record	*pClass	= pClasses->Get_Record(iClass);

			pClass->Set_Value(0, Colors[iClass]);
			pClass->Set_Value(1, Points.Get_Field_Name(1 + iClass));
			pClass->Set_Value(2, "");
			pClass->Set_Value(3, iClass);
			pClass->Set_Value(4, iClass);
		}

		DataObject_Set_Parameter(pGrid, pLUT);	// Lookup Table
		DataObject_Set_Parameter(pGrid, "COLORS_TYPE", 1);	// Color Classification Type: Lookup Table
	}

	//-----------------------------------------------------
	pGrid->Set_Name(CSG_String::Format("%s"     , Points.Get_Field_Name(0)));
	pProp->Set_Name(CSG_String::Format("%s [%s]", Points.Get_Field_Name(0), _TL("Propability")));

	pProp->Assign(0.0);
	pProp->Set_NoData_Value(0.0);

	Prop.Create(pGrid->Get_System());

	//-----------------------------------------------------
	for(int i=0; i<nCategories; i++)
	{
		Process_Set_Text(CSG_String::Format("%s: %s", _TL("processing"), Points.Get_Field_Name(1 + i)));

		SG_UI_Progress_Lock(true);

		SG_RUN_MODULE_ExitOnError("grid_spline", 4,	// Multilevel B-Spline Interpolation
				SG_MODULE_PARAMETER_SET("SHAPES"           , &Points)
			&&	SG_MODULE_PARAMETER_SET("FIELD"            , 1 + i)	// indicator field
			&&	SG_MODULE_PARAMETER_SET("TARGET_DEFINITION", 1)		// grid or grid system
			&&	SG_MODULE_PARAMETER_SET("TARGET_OUT_GRID"  , &Prop)	// target grid
		)

		SG_UI_Progress_Lock(false);

		#pragma omp parallel for
		for(int y=0; y<pGrid->Get_NY(); y++)
		{
			for(int x=0; x<pGrid->Get_NX(); x++)
			{
				if( pProp->asDouble(x, y) < Prop.asDouble(x, y) )
				{
					pProp->Set_Value(x, y, Prop.asDouble(x, y));
					pGrid->Set_Value(x, y, i);
				}
			}
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

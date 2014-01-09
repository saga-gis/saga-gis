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
//                      Grid_Tools                       //
//                                                       //
//-------------------------------------------------------//
//                                                       //
//                     Grid_Cut.cpp                      //
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

//---------------------------------------------------------


///////////////////////////////////////////////////////////
//														 //
//														 //
//														 //
///////////////////////////////////////////////////////////

//---------------------------------------------------------
#include "Grid_Cut.h"


///////////////////////////////////////////////////////////
//														 //
//														 //
//														 //
///////////////////////////////////////////////////////////

//---------------------------------------------------------
CSG_Grid_System Fit_Extent(const CSG_Grid_System &System, const CSG_Rect &Extent)
{
	CSG_Rect	r(
		System.Fit_xto_Grid_System(Extent.Get_XMin()),
		System.Fit_yto_Grid_System(Extent.Get_YMin()),
		System.Fit_xto_Grid_System(Extent.Get_XMax()),
		System.Fit_yto_Grid_System(Extent.Get_YMax())
	);

	r.Intersect(System.Get_Extent(true));

	return( CSG_Grid_System(System.Get_Cellsize(), r) );
}

//---------------------------------------------------------
void Fit_Extent(CSG_Parameters *pParameters, CSG_Parameter *pParameter, CSG_Grid_System *pSystem)
{
	if( pSystem && pSystem->is_Valid() )
	{
		CSG_Grid_System	s(Fit_Extent(*pSystem, CSG_Rect(
			pParameters->Get_Parameter("XMIN")->asDouble(),
			pParameters->Get_Parameter("YMIN")->asDouble(),
			pParameters->Get_Parameter("XMAX")->asDouble(),
			pParameters->Get_Parameter("YMAX")->asDouble()
		)));

		if( !SG_STR_CMP(pParameter->Get_Identifier(), SG_T("NX")) )
		{
			s.Assign(s.Get_Cellsize(), s.Get_XMin(), s.Get_YMin(), pParameters->Get_Parameter("NX")->asInt(), s.Get_NY());
		}

		if( !SG_STR_CMP(pParameter->Get_Identifier(), SG_T("NY")) )
		{
			s.Assign(s.Get_Cellsize(), s.Get_XMin(), s.Get_YMin(), s.Get_NX(), pParameters->Get_Parameter("NY")->asInt());
		}

		if( s.is_Valid() )
		{
			pParameters->Get_Parameter("XMIN")->Set_Value(s.Get_XMin());
			pParameters->Get_Parameter("XMAX")->Set_Value(s.Get_XMax());
			pParameters->Get_Parameter("YMIN")->Set_Value(s.Get_YMin());
			pParameters->Get_Parameter("YMAX")->Set_Value(s.Get_YMax());
			pParameters->Get_Parameter("NX"  )->Set_Value(s.Get_NX  ());
			pParameters->Get_Parameter("NY"  )->Set_Value(s.Get_NY  ());
		}
	}
}


///////////////////////////////////////////////////////////
//														 //
//														 //
//														 //
///////////////////////////////////////////////////////////

//---------------------------------------------------------
CGrid_Clip_Interactive::CGrid_Clip_Interactive(void)
{
	//-----------------------------------------------------
	Set_Name		(_TL("Clip Grids"));

	Set_Author		(SG_T("O.Conrad (c) 2003"));

	Set_Description	(_TW(
		"Clip selected grids to interactively defined extent."
	));

	//-----------------------------------------------------
	Parameters.Add_Grid_List(
		NULL	, "GRIDS"	, _TL("Grids"),
		_TL(""),
		PARAMETER_INPUT
	);

	Parameters.Add_Grid_List(
		NULL	, "CLIPPED"	, _TL("Clipped Grids"),
		_TL(""),
		PARAMETER_OUTPUT, false
	);

	//-----------------------------------------------------
	CSG_Parameters	*pParameters	= Add_Parameters("EXTENT", _TL("Extent"), _TL(""));

	pParameters->Add_Value(NULL, "XMIN", _TL("Left"   ), _TL(""), PARAMETER_TYPE_Double);
	pParameters->Add_Value(NULL, "XMAX", _TL("Right"  ), _TL(""), PARAMETER_TYPE_Double);
	pParameters->Add_Value(NULL, "YMIN", _TL("Bottom" ), _TL(""), PARAMETER_TYPE_Double);
	pParameters->Add_Value(NULL, "YMAX", _TL("Top"    ), _TL(""), PARAMETER_TYPE_Double);
	pParameters->Add_Value(NULL, "NX"  , _TL("Columns"), _TL(""), PARAMETER_TYPE_Int, 1, 1, true);
	pParameters->Add_Value(NULL, "NY"  , _TL("Rows"   ), _TL(""), PARAMETER_TYPE_Int, 1, 1, true);
}


///////////////////////////////////////////////////////////
//														 //
///////////////////////////////////////////////////////////

//---------------------------------------------------------
int CGrid_Clip_Interactive::On_Parameter_Changed(CSG_Parameters *pParameters, CSG_Parameter *pParameter)
{
	if( is_Executing() && !SG_STR_CMP(pParameters->Get_Identifier(), SG_T("EXTENT")) )
	{
		Fit_Extent(pParameters, pParameter, Get_System());
	}

	return( 1 );
}


///////////////////////////////////////////////////////////
//														 //
///////////////////////////////////////////////////////////

//---------------------------------------------------------
bool CGrid_Clip_Interactive::On_Execute(void)
{
	m_bDown		= false;

	return( true );
}


///////////////////////////////////////////////////////////
//														 //
///////////////////////////////////////////////////////////

//---------------------------------------------------------
TSG_Point CGrid_Clip_Interactive::Fit_to_Grid(TSG_Point Point)
{
	Point.x	= Get_XMin() + Get_Cellsize() * (int)(0.5 + (Point.x - Get_XMin()) / Get_Cellsize());
	Point.y	= Get_YMin() + Get_Cellsize() * (int)(0.5 + (Point.y - Get_YMin()) / Get_Cellsize());

	return( Point );
}

//---------------------------------------------------------
bool CGrid_Clip_Interactive::On_Execute_Position(CSG_Point ptWorld, TSG_Module_Interactive_Mode Mode)
{
	switch( Mode )
	{
	//-----------------------------------------------------
	case MODULE_INTERACTIVE_LDOWN:
		if( m_bDown == false )
		{
			m_bDown		= true;
			m_ptDown	= Fit_to_Grid(ptWorld);
		}

		return( true );

	//-----------------------------------------------------
	case MODULE_INTERACTIVE_LUP:
		if( m_bDown == true )
		{
			m_bDown		= false;
			ptWorld		= Fit_to_Grid(ptWorld);

			//---------------------------------------------
			CSG_Grid_System	System	= Fit_Extent(*Get_System(), CSG_Rect(
				m_ptDown.Get_X(), m_ptDown.Get_Y(), ptWorld.Get_X(), ptWorld.Get_Y()
			));

			CSG_Parameters	*pParameters	= Get_Parameters("EXTENT");

			pParameters->Get_Parameter("XMIN")->Set_Value(System.Get_XMin());
			pParameters->Get_Parameter("XMAX")->Set_Value(System.Get_XMax());
			pParameters->Get_Parameter("YMIN")->Set_Value(System.Get_YMin());
			pParameters->Get_Parameter("YMAX")->Set_Value(System.Get_YMax());
			pParameters->Get_Parameter("NX"  )->Set_Value(System.Get_NX  ());
			pParameters->Get_Parameter("NY"  )->Set_Value(System.Get_NY  ());

			if( Dlg_Parameters(pParameters, _TL("Clip to Extent")) )
			{
				System	= Fit_Extent(*Get_System(), CSG_Rect(
					pParameters->Get_Parameter("XMIN")->asDouble(),
					pParameters->Get_Parameter("YMIN")->asDouble(),
					pParameters->Get_Parameter("XMAX")->asDouble(),
					pParameters->Get_Parameter("YMAX")->asDouble()
				));

				//-----------------------------------------
				if( System.is_Valid() )
				{
					CSG_Parameter_Grid_List	*pInput		= Parameters("GRIDS"  )->asGridList();
					CSG_Parameter_Grid_List	*pOutput	= Parameters("CLIPPED")->asGridList();

					for(int i=0; i<pInput->Get_Count(); i++)
					{
						CSG_Grid	*pClip	= SG_Create_Grid(System, pInput->asGrid(i)->Get_Type());

						pClip->Assign  (pInput->asGrid(i), GRID_INTERPOLATION_NearestNeighbour);
						pClip->Set_Name(pInput->asGrid(i)->Get_Name());

						pOutput->Add_Item(pClip);
						DataObject_Add   (pClip);
					}
				}
			}
		}

		return( true );

	//-----------------------------------------------------
	}

	return( false );
}


///////////////////////////////////////////////////////////
//														 //
//														 //
//														 //
///////////////////////////////////////////////////////////

//---------------------------------------------------------
CGrid_Clip::CGrid_Clip(void)
{
	CSG_Parameter	*pNode;

	//-----------------------------------------------------
	Set_Name		(_TL("Clip Grids"));

	Set_Author		(SG_T("O.Conrad (c) 2003"));

	Set_Description	(_TW(
		"Clip selected grids to specified extent."
	));

	//-----------------------------------------------------
	Parameters.Add_Grid_List(
		NULL	, "GRIDS"		, _TL("Grids"),
		_TL(""),
		PARAMETER_INPUT
	);

	Parameters.Add_Grid_List(
		NULL	, "CLIPPED"		, _TL("Clipped Grids"),
		_TL(""),
		PARAMETER_OUTPUT, false
	);

	//-----------------------------------------------------
	pNode	= Parameters.Add_Choice(
		NULL	, "EXTENT"		, _TL("Extent"),
		_TL(""),
		CSG_String::Format(SG_T("%s|%s|%s|"),
			_TL("user defined"),
			_TL("grid system"),
			_TL("shapes extent")
		), 0
	);

	Parameters.Add_Grid_System(
		pNode	, "GRIDSYSTEM"	, _TL("Grid System"),
		_TL("")
	);

	Parameters.Add_Shapes(
		pNode	, "SHAPES"		, _TL("Shapes Extent"),
		_TL(""),
		PARAMETER_INPUT
	);

	Parameters.Add_Value(pNode, "XMIN", _TL("Left"   ), _TL(""), PARAMETER_TYPE_Double);
	Parameters.Add_Value(pNode, "XMAX", _TL("Right"  ), _TL(""), PARAMETER_TYPE_Double);
	Parameters.Add_Value(pNode, "YMIN", _TL("Bottom" ), _TL(""), PARAMETER_TYPE_Double);
	Parameters.Add_Value(pNode, "YMAX", _TL("Top"    ), _TL(""), PARAMETER_TYPE_Double);
	Parameters.Add_Value(pNode, "NX"  , _TL("Columns"), _TL(""), PARAMETER_TYPE_Int, 1, 1, true);
	Parameters.Add_Value(pNode, "NY"  , _TL("Rows"   ), _TL(""), PARAMETER_TYPE_Int, 1, 1, true);
}


///////////////////////////////////////////////////////////
//														 //
///////////////////////////////////////////////////////////

//---------------------------------------------------------
int CGrid_Clip::On_Parameter_Changed(CSG_Parameters *pParameters, CSG_Parameter *pParameter)
{
	CSG_Grid_System	*pSystem	= pParameters->Get_Parameter("PARAMETERS_GRID_SYSTEM")->asGrid_System();

	if( !SG_STR_CMP(pParameter->Get_Identifier(), "PARAMETERS_GRID_SYSTEM") && pSystem && pSystem->is_Valid() )
	{
		pParameters->Get_Parameter("XMIN")->Set_Value(pSystem->Get_XMin());
		pParameters->Get_Parameter("XMAX")->Set_Value(pSystem->Get_XMax());
		pParameters->Get_Parameter("YMIN")->Set_Value(pSystem->Get_YMin());
		pParameters->Get_Parameter("YMAX")->Set_Value(pSystem->Get_YMax());
	}

	Fit_Extent(pParameters, pParameter, pSystem);

	return( 1 );
}

//---------------------------------------------------------
int CGrid_Clip::On_Parameters_Enable(CSG_Parameters *pParameters, CSG_Parameter *pParameter)
{
	if( !SG_STR_CMP(pParameter->Get_Identifier(), "EXTENT") )
	{
		pParameters->Get_Parameter("XMIN"      )->Set_Enabled(pParameter->asInt() == 0);
		pParameters->Get_Parameter("XMAX"      )->Set_Enabled(pParameter->asInt() == 0);
		pParameters->Get_Parameter("YMIN"      )->Set_Enabled(pParameter->asInt() == 0);
		pParameters->Get_Parameter("YMAX"      )->Set_Enabled(pParameter->asInt() == 0);
		pParameters->Get_Parameter("NX"        )->Set_Enabled(pParameter->asInt() == 0);
		pParameters->Get_Parameter("NY"        )->Set_Enabled(pParameter->asInt() == 0);
		pParameters->Get_Parameter("GRIDSYSTEM")->Set_Enabled(pParameter->asInt() == 1);
		pParameters->Get_Parameter("SHAPES"    )->Set_Enabled(pParameter->asInt() == 2);
	}

	return( 1 );
}


///////////////////////////////////////////////////////////
//														 //
///////////////////////////////////////////////////////////

//---------------------------------------------------------
bool CGrid_Clip::On_Execute(void)
{
	CSG_Grid_System	System;
	
	switch( Parameters("EXTENT")->asInt() )
	{
	case 0:	// user defined
		System	= Fit_Extent(*Get_System(), CSG_Rect(
			Parameters("XMIN")->asDouble(),
			Parameters("YMIN")->asDouble(),
			Parameters("XMAX")->asDouble(),
			Parameters("YMAX")->asDouble()
		));
		break;

	case 1: // grid system
		System	= Fit_Extent(*Get_System(), Parameters("GRIDSYSTEM")->asGrid_System()->Get_Extent());
		break;

	case 2:	// shapes extent
		System	= Fit_Extent(*Get_System(), Parameters("SHAPES")->asShapes()->Get_Extent());
		break;
	}

	if( !System.is_Valid() )
	{
		Error_Set(_TL("clip extent does not match grid's extent"));

		return( false );
	}

	//--------------------------------------------------------
	CSG_Parameter_Grid_List	*pInput		= Parameters("GRIDS"  )->asGridList();
	CSG_Parameter_Grid_List	*pOutput	= Parameters("CLIPPED")->asGridList();

	for(int i=0; i<pInput->Get_Count(); i++)
	{
		CSG_Grid	*pClip	= SG_Create_Grid(System, pInput->asGrid(i)->Get_Type());

		pClip->Assign  (pInput->asGrid(i), GRID_INTERPOLATION_NearestNeighbour);
		pClip->Set_Name(pInput->asGrid(i)->Get_Name());

		pOutput->Add_Item(pClip);
		DataObject_Add   (pClip);
	}

	return( true );
}


///////////////////////////////////////////////////////////
//														 //
//														 //
//														 //
///////////////////////////////////////////////////////////

//---------------------------------------------------------

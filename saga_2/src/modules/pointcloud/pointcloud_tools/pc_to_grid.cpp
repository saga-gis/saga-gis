
///////////////////////////////////////////////////////////
//                                                       //
//                         SAGA                          //
//                                                       //
//      System for Automated Geoscientific Analyses      //
//                                                       //
//                    Module Library:                    //
//                   pointcloud_tools                    //
//                                                       //
//-------------------------------------------------------//
//                                                       //
//                    pc_to_grid.cpp                     //
//                                                       //
//                 Copyright (C) 2009 by                 //
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
#include "pc_to_grid.h"


///////////////////////////////////////////////////////////
//														 //
//														 //
//														 //
///////////////////////////////////////////////////////////

//---------------------------------------------------------
CPC_To_Grid::CPC_To_Grid(void)
{
	//-----------------------------------------------------
	Set_Name		(_TL("Point Cloud to Grid"));

	Set_Author		(SG_T("O.Conrad (c) 2009"));

	Set_Description	(_TW(
		""
	));


	//-----------------------------------------------------
	Parameters.Add_PointCloud(
		NULL	, "POINTS"		, _TL("Points"),
		_TL(""),
		PARAMETER_INPUT
	);

	Parameters.Add_Grid_Output(
		NULL	, "GRID"		, _TL("Z Value"),
		_TL("")
	);

	Parameters.Add_Grid_Output(
		NULL	, "COUNT"		, _TL("Number of Points in Cell"),
		_TL("")
	);

	Parameters.Add_Grid_List(
		NULL	, "GRIDS"		, _TL("Attributes"),
		_TL(""),
		PARAMETER_OUTPUT_OPTIONAL
	);

	Parameters.Add_Choice(
		NULL	, "OUTPUT"		, _TL("Output"),
		_TL(""),
		CSG_String::Format(SG_T("%s|%s|"),
			_TL("only z"),
			_TL("all attributes")
		), 0
	);

	Parameters.Add_Choice(
		NULL	, "AGGREGATION"	, _TL("Aggregation"),
		_TL(""),
		CSG_String::Format(SG_T("%s|%s|%s|%s|%s|"),
			_TL("first value"),
			_TL("last value"),
			_TL("mean value"),
			_TL("lowest z"),
			_TL("highest z")
		), 0
	);

	Parameters.Add_Value(
		NULL	, "CELLSIZE"	, _TL("Cellsize"),
		_TL(""),
		PARAMETER_TYPE_Double, 1.0, 0.0, true
	);
}


///////////////////////////////////////////////////////////
//														 //
//														 //
//														 //
///////////////////////////////////////////////////////////

//---------------------------------------------------------
bool CPC_To_Grid::On_Execute(void)
{
	int						x, y, iField, iGrid;
	CSG_Grid_System			System;
	CSG_Parameter_Grid_List	*pGrids;
	CSG_PointCloud			*pPoints;

	pPoints			= Parameters("POINTS")		->asPointCloud();
	pGrids			= Parameters("GRIDS")		->asGridList();
	m_Aggregation	= Parameters("AGGREGATION")	->asInt();

	//-----------------------------------------------------
	System.Assign(Parameters("CELLSIZE")->asDouble(), pPoints->Get_Extent());

	//-----------------------------------------------------
	pGrids->Del_Items();

	if( Parameters("OUTPUT")->asInt() != 0 )
	{
		for(iField=3; iField<pPoints->Get_Field_Count(); iField++)
		{
			pGrids->Add_Item(SG_Create_Grid(System, SG_DATATYPE_Float));
			pGrids->asGrid(iField - 3)->Set_Name(CSG_String::Format(SG_T("%s - %s"), pPoints->Get_Name(), pPoints->Get_Field_Name(iField)));
		}
	}

	Parameters("GRID")	->Set_Value(m_pGrid  = SG_Create_Grid(System, SG_DATATYPE_Float));
	Parameters("COUNT")	->Set_Value(m_pCount = SG_Create_Grid(System, SG_DATATYPE_Int));

	m_pGrid		->Set_Name(CSG_String::Format(SG_T("%s [%s]"), pPoints->Get_Name(), pPoints->Get_Field_Name(2)));
	m_pCount	->Set_Name(CSG_String::Format(SG_T("%s [%s]"), pPoints->Get_Name(), _TL("Points per Cell")));

	m_pCount	->Set_NoData_Value(0.0);

	//-----------------------------------------------------
	for(int iPoint=0; iPoint<pPoints->Get_Count() && Set_Progress(iPoint, pPoints->Get_Count()); iPoint++)
	{
		pPoints->Set_Cursor(iPoint);

		if( System.Get_World_to_Grid(x, y, pPoints->Get_X(), pPoints->Get_Y()) )
		{
			int		n	= m_pCount->asInt(x, y);
			double	z	= pPoints->Get_Z();

			for(iGrid=0; iGrid<pGrids->Get_Count(); iGrid++)
			{
				Set_Value(x, y, z, n, pPoints->Get_Value(iGrid + 3), pGrids->asGrid(iGrid));
			}

			Set_Value(x, y, z, n, z, m_pGrid);

			m_pCount->Add_Value(x, y, 1);
		}
	}

	//-----------------------------------------------------
	for(y=0; y<System.Get_NY() && Set_Progress(y, System.Get_NY()); y++)
	{
		for(x=0; x<System.Get_NX(); x++)
		{
			int		n	= m_pCount->asInt(x, y);

			switch( n )
			{
			case 0:
				m_pGrid->Set_NoData(x, y);

				for(iGrid=0; iGrid<pGrids->Get_Count(); iGrid++)
				{
					pGrids->asGrid(iGrid)->Set_NoData(x, y);
				}
				break;

			case 1:
				break;

			default:
				if( m_Aggregation == 2 )	// mean value
				{
					m_pGrid->Mul_Value(x, y, 1.0 / n);

					for(iGrid=0; iGrid<pGrids->Get_Count(); iGrid++)
					{
						pGrids->asGrid(iGrid)->Mul_Value(x, y, 1.0 / n);
					}
				}
				break;
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
inline void CPC_To_Grid::Set_Value(int x, int y, double z, int Count, double value, CSG_Grid *pGrid)
{
	switch( m_Aggregation )
	{
	case 0:	// first value
		if( Count == 0 )
		{
			pGrid->Set_Value(x, y, value);
		}
		break;

	case 1:	// last value
		pGrid->Set_Value(x, y, value);
		break;

	case 2:	// mean value
		pGrid->Add_Value(x, y, value);
		break;

	case 3:	// lowest z
		if( Count == 0 || z < m_pGrid->asDouble(x, y) )
		{
			pGrid->Set_Value(x, y, value);
		}
		break;

	case 4:	// highest z
		if( Count == 0 || z > m_pGrid->asDouble(x, y) )
		{
			pGrid->Set_Value(x, y, value);
		}
		break;
	}
}


///////////////////////////////////////////////////////////
//														 //
//														 //
//														 //
///////////////////////////////////////////////////////////

//---------------------------------------------------------

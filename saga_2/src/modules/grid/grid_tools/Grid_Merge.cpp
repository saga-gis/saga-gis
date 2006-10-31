
///////////////////////////////////////////////////////////
//                                                       //
//                         SAGA                          //
//                                                       //
//      System for Automated Geoscientific Analyses      //
//                                                       //
//                    Module Library:                    //
//                       Tutorial                        //
//                                                       //
//-------------------------------------------------------//
//                                                       //
//                    Grid_Merge.cpp                     //
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
// 59 Temple Place - Suite 330, Boston, MA 02111-1307,   //
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
#include "Grid_Merge.h"


///////////////////////////////////////////////////////////
//														 //
//														 //
//														 //
///////////////////////////////////////////////////////////

//---------------------------------------------------------
CGrid_Merge::CGrid_Merge(void)
{
	//-----------------------------------------------------
	// 1. Info...

	Set_Name(_TL("Merging"));

	Set_Author(_TL("Copyrights (c) 2003 by Olaf Conrad"));

	Set_Description(_TL("Merge Grids.\n")
	);


	//-----------------------------------------------------
	// 2. Standard in- and output...

	Parameters.Add_Grid_List(
		NULL	, "GRID_LIST"	, _TL("Grids to merge"),
		"",
		PARAMETER_INPUT
	);

	Parameters.Add_Grid(
		NULL	, "MERGED"		, _TL("Merged Grid"),
		"",
		PARAMETER_OUTPUT_OPTIONAL
	);

	Parameters.Add_Grid_Output(
		NULL	, "GRID"		, _TL("Merged Grid (unknown grid system)"),
		""
	);

	Parameters.Add_Choice(
		NULL	, "TYPE"		, _TL("Preferred data storage type"),
		"",
		CSG_String::Format("%s|%s|%s|%s|%s|%s|%s|%s|%s|",
			_TL("1 bit"),
			_TL("1 byte unsigned"),
			_TL("1 byte signed"),
			_TL("2 byte unsigned"),
			_TL("2 byte signed"),
			_TL("4 byte unsigned"),
			_TL("4 byte signed"),
			_TL("4 byte floating point"),
			_TL("8 byte floating point")
		), 7
	);


	//-----------------------------------------------------
	CSG_Parameters	*pParameters;

	pParameters	= Add_Parameters(
		"MERGE_INFO"	, _TL("Mesh Size Specification"),
		_TL("The grid selection contains grids with different mesh sizes."
		"Please specify the mesh size that you desire for the merged grid."
		"All grids with other mesh sizes will then be resampled according to your specification.")
	);

	pParameters->Add_Value(
		NULL	, "MESH_SIZE_X"	, _TL("Mesh Width"),
		"",
		PARAMETER_TYPE_Double, 1
	);

	pParameters->Add_Value(
		NULL	, "MESH_SIZE_Y"	, _TL("Mesh Height"),
		"",
		PARAMETER_TYPE_Double, 1
	);

	Parameters.Add_Choice(
		NULL	, "INTERPOL"	, _TL("Interpolation"),
		"",
		CSG_String::Format("%s|%s|%s|%s|%s|",
			_TL("Nearest Neighbor"),
			_TL("Bilinear Interpolation"),
			_TL("Inverse Distance Interpolation"),
			_TL("Bicubic Spline Interpolation"),
			_TL("B-Spline Interpolation")
		), 4
	);
}

//---------------------------------------------------------
CGrid_Merge::~CGrid_Merge(void)
{}


///////////////////////////////////////////////////////////
//														 //
//	Run													 //
//														 //
///////////////////////////////////////////////////////////

//---------------------------------------------------------
bool CGrid_Merge::On_Execute(void)
{
	int						iGrid, nGrids, nx, ny, x, y, Interpol;
	double					d, Cellsize, xPos, yPos;
	TSG_Grid_Type				Type;
	CSG_Rect				Extent;
	CSG_Grid					*pGrid, *pMerge;
	CSG_Parameter_Grid_List	*pParm_Grids;

	pParm_Grids	= Parameters("GRID_LIST")	->asGridList();
	Interpol	= Parameters("INTERPOL")	->asInt();

	if( (nGrids = pParm_Grids->Get_Count()) > 1 )
	{
		if( (pMerge = Parameters("MERGED")->asGrid()) == NULL )
		{
			//---------------------------------------------
			// 1. Check the mesh sizes...

			pGrid		= pParm_Grids->asGrid(0);

			Cellsize	= pGrid->Get_Cellsize();

			for(iGrid=1; iGrid<nGrids; iGrid++)
			{
				pGrid	= pParm_Grids->asGrid(iGrid);

				if( Cellsize != pGrid->Get_Cellsize() )
				{
					Get_Parameters("MERGE_INFO")->Get_Parameter("MESH_SIZE_X")->Set_Value(Cellsize);
					Get_Parameters("MERGE_INFO")->Get_Parameter("MESH_SIZE_Y")->Set_Value(Cellsize);

					if( !Dlg_Parameters("MERGE_INFO") )
					{
						return( false );
					}

					Cellsize	= Get_Parameters("MERGE_INFO")->Get_Parameter("MESH_SIZE_X")->asDouble();

					break;
				}
			}

			//---------------------------------------------
			// 2. Calculate the dimensions of the merged grid and create it...

			switch( Parameters("TYPE")->asInt() )
			{
			case 0:				Type	= GRID_TYPE_Bit;	break;
			case 1:				Type	= GRID_TYPE_Byte;	break;
			case 2:				Type	= GRID_TYPE_Char;	break;
			case 3:				Type	= GRID_TYPE_Word;	break;
			case 4:				Type	= GRID_TYPE_Short;	break;
			case 5:				Type	= GRID_TYPE_DWord;	break;
			case 6:				Type	= GRID_TYPE_Int;	break;
			case 7: default:	Type	= GRID_TYPE_Float;	break;
			case 8:				Type	= GRID_TYPE_Double;	break;
			}

			pGrid	= pParm_Grids->asGrid(0);

			Extent.Assign(pGrid->Get_Extent());

			for(iGrid=1; iGrid<nGrids; iGrid++)
			{
				pGrid	= pParm_Grids->asGrid(iGrid);

				Extent.Union(pGrid->Get_Extent());
			}

			nx		= 1 + (int)((Extent.m_rect.xMax - Extent.m_rect.xMin) / Cellsize);
			ny		= 1 + (int)((Extent.m_rect.yMax - Extent.m_rect.yMin) / Cellsize);

			pMerge	= SG_Create_Grid(Type, nx, ny, Cellsize, Extent.m_rect.xMin, Extent.m_rect.yMin);

			Parameters("GRID")->Set_Value(pMerge);
		}


		//-------------------------------------------------
		// 3. Merge grids...

		pMerge->Assign_NoData();
		pMerge->Set_Name(_TL("Merged Grid"));

		for(iGrid=0; iGrid<nGrids; iGrid++)
		{
			Process_Set_Text(CSG_String::Format(_TL("Merging grid %d of %d..."), iGrid + 1, nGrids));

			pGrid	= pParm_Grids->asGrid(iGrid);

			for(y=0, yPos=pMerge->Get_YMin(); y<pMerge->Get_NY() && yPos<=pGrid->Get_YMax() && Set_Progress(y, pMerge->Get_NY()); y++, yPos+=pMerge->Get_Cellsize())
			{
				if( yPos >= pGrid->Get_YMin() )
				{
					for(x=0, xPos=pMerge->Get_XMin(); x<pMerge->Get_NX() && xPos<=pGrid->Get_XMax(); x++, xPos+=pMerge->Get_Cellsize())
					{
						if( xPos >= pGrid->Get_XMin() )
						{
							if( (d = pGrid->Get_Value(xPos, yPos, Interpol)) != pGrid->Get_NoData_Value() )
							{
								pMerge->Set_Value(x, y, pMerge->is_NoData(x, y) ? d : (d + pMerge->asDouble(x, y)) / 2.0);
							}
						}
					}
				}
			}
		}

		//-------------------------------------------------
		DataObject_Update(pMerge);

		return( true );
	}

	Message_Dlg(_TL("There is nothing to merge, because less than 2 grids have been selected."), Get_Name());

	return( false );
}

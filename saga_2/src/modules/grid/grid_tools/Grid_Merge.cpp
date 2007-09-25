
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

	Set_Name		(_TL("Merging"));

	Set_Author		(_TL("Copyrights (c) 2003 by Olaf Conrad"));

	Set_Description	(_TW(
		"Merge Grids."
	));


	//-----------------------------------------------------
	// 2. Standard in- and output...

	Parameters.Add_Grid_List(
		NULL	, "GRIDS"		, _TL("Grids to Merge"),
		_TL(""),
		PARAMETER_INPUT
	);

	Parameters.Add_Grid(
		NULL	, "GRID_TARGET"	, _TL("Target Grid"),
		_TL(""),
		PARAMETER_OUTPUT_OPTIONAL
	);

	Parameters.Add_Grid_Output(
		NULL	, "GRID"		, _TL("Merged Grid"),
		_TL("")
	);

	Parameters.Add_Choice(
		NULL	, "TYPE"		, _TL("Preferred data storage type"),
		_TL(""),
		CSG_String::Format(SG_T("%s|%s|%s|%s|%s|%s|%s|%s|%s|"),
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
		"MERGE_INFO"	, _TL("Cell Size Specification"),
		_TW(	"Your grid selection contains grids with different cell sizes. "
				"Please specify the cell size that you desire for the merged "
				"grid. All grids with other cell sizes will then be resampled "
				"according to your specification.")
	);

	pParameters->Add_Value(
		NULL	, "MESH_SIZE"	, _TL("Cell Size"),
		_TL(""),
		PARAMETER_TYPE_Double, 1.0, 0.0, true
	);

	Parameters.Add_Choice(
		NULL	, "INTERPOL"	, _TL("Interpolation"),
		_TL(""),
		CSG_String::Format(SG_T("%s|%s|%s|%s|%s|"),
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
//														 //
//														 //
///////////////////////////////////////////////////////////

//---------------------------------------------------------
bool CGrid_Merge::On_Execute(void)
{
	int						Interpolation;
	CSG_Parameter_Grid_List	*pGrids;
	CSG_Grid				*pGrid;

	//-----------------------------------------------------
	Interpolation	= Parameters("INTERPOL")	->asInt();
	pGrids			= Parameters("GRIDS")		->asGridList();
	pGrid			= Parameters("GRID_TARGET")	->asGrid();

	//-----------------------------------------------------
	if( pGrids->Get_Count() > 1 )
	{
		bool	bResampling;
		int		i, x, y;
		double	Cellsize;

		//-------------------------------------------------
		Cellsize	= pGrids->asGrid(0)->Get_Cellsize();

		for(i=1, bResampling=false; i<pGrids->Get_Count(); i++)
		{
			if( Cellsize != pGrids->asGrid(i)->Get_Cellsize() )
			{
				bResampling	= true;

				if( Cellsize > pGrids->asGrid(i)->Get_Cellsize() )
				{
					Cellsize	= pGrids->asGrid(i)->Get_Cellsize();
				}

				if( !bResampling )
				{
					if(	(pGrids->asGrid(0)->Get_XMin() - pGrids->asGrid(i)->Get_XMin()) / Cellsize != 0.0
					||	(pGrids->asGrid(0)->Get_YMin() - pGrids->asGrid(i)->Get_YMin()) / Cellsize != 0.0 )
						bResampling	= true;
				}
			}
		}

		//-------------------------------------------------
		if( pGrid == NULL )
		{
			TSG_Grid_Type	Type;
			CSG_Rect		Extent;

			//---------------------------------------------
			// Type...

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

			//---------------------------------------------
			// Cell Size...

			if( bResampling )
			{
				Get_Parameters("MERGE_INFO")->Get_Parameter("MESH_SIZE_X")->Set_Value(Cellsize);

				if( !Dlg_Parameters("MERGE_INFO") )
				{
					return( false );
				}

				Cellsize	= Get_Parameters("MERGE_INFO")->Get_Parameter("MESH_SIZE_X")->asDouble();
			}

			//---------------------------------------------
			// Extent...

			Extent.Assign(pGrids->asGrid(0)->Get_Extent());

			for(i=1; i<pGrids->Get_Count(); i++)
			{
				Extent.Union(pGrids->asGrid(i)->Get_Extent());
			}

			//---------------------------------------------
			// Create Grid...

			Parameters("GRID")->Set_Value(pGrid	= SG_Create_Grid(
				Type,
				1 + (int)(0.5 + Extent.Get_XRange() / Cellsize),
				1 + (int)(0.5 + Extent.Get_YRange() / Cellsize),
				Cellsize,
				Extent.Get_XMin(),
				Extent.Get_YMin()
			));
		}


		//-------------------------------------------------
		// Merge grids...

		pGrid->Set_Name(_TL("Merged Grid"));

		//-------------------------------------------------
		if( !bResampling )	// without resampling...
		{
			pGrid->Assign_NoData();

			for(i=0; i<pGrids->Get_Count(); i++)
			{
				int			ix, iy;
				CSG_Grid	*g	= pGrids->asGrid(i);

				iy	= (int)((g->Get_YMin() - pGrid->Get_YMin()) / Cellsize);

				for(y=0; y<g->Get_NY() && Set_Progress(y, g->Get_NY()); y++, iy++)
				{
					if( iy >= 0 && iy < pGrid->Get_NY() )
					{
						ix	= (int)((g->Get_XMin() - pGrid->Get_XMin()) / Cellsize);

						for(x=0; x<g->Get_NX(); x++, ix++)
						{
							if( ix >= 0 && ix < pGrid->Get_NX() )
							{
								if( pGrid->is_NoData(ix, iy) )
									pGrid->Set_Value(ix, iy,  g->asDouble(x, y));
								else
									pGrid->Set_Value(ix, iy, (g->asDouble(x, y) + pGrid->asDouble(ix, iy)) / 2.0);
							}
						}
					}
				}
			}
		}

		//-------------------------------------------------
		else				// with resampling...
		{
			int			zCount;
			double		z, zSum;
			TSG_Point	p;

			for(y=0, p.y=pGrid->Get_YMin(); y<pGrid->Get_NY() && Set_Progress(y, pGrid->Get_NY()); y++, p.y+=pGrid->Get_Cellsize())
			{
				for(x=0, p.x=pGrid->Get_XMin(); x<pGrid->Get_NX(); x++, p.x+=pGrid->Get_Cellsize())
				{
					for(i=0, zCount=0, zSum=0.0; i<pGrids->Get_Count(); i++)
					{
						if( pGrids->asGrid(i)->Get_Value(p, z, Interpolation) )
						{
							zSum	+= z;
							zCount	++;
						}
					}

					if( zCount > 0 )
						pGrid->Set_Value (x, y, zSum / zCount);
					else
						pGrid->Set_NoData(x, y);
				}
			}
		}


		//-------------------------------------------------
		DataObject_Update(pGrid);

		return( true );
	}

	Message_Dlg(_TL("There is nothing to merge, because less than 2 grids have been selected."), Get_Name());

	return( false );
}


///////////////////////////////////////////////////////////
//														 //
//														 //
//														 //
///////////////////////////////////////////////////////////

//---------------------------------------------------------

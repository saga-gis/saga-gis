
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

	Set_Author		(SG_T("(c) 2003 by O.Conrad"));

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
		NULL	, "MERGED"		, _TL("Merged Grid"),
		_TL("")
	);

	Parameters.Add_Choice(
		NULL	, "TYPE"		, _TL("Preferred data storage type"),
		_TL(""),
		CSG_String::Format(SG_T("%s|%s|%s|%s|%s|%s|%s|%s|%s|"),
			_TL("1 bit"),
			_TL("1 byte unsigned integer"),
			_TL("1 byte signed integer"),
			_TL("2 byte unsigned integer"),
			_TL("2 byte signed integer"),
			_TL("4 byte unsigned integer"),
			_TL("4 byte signed integer"),
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

	Parameters.Add_Choice(
		NULL	, "OVERLAP"		, _TL("Overlapping Cells"),
		_TL(""),
		CSG_String::Format(SG_T("%s|%s|"),
			_TL("mean value"),
			_TL("first value in order of grid list")
		), 0
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
	bool					bMean;
	int						x, y, i, ix, iy, ax, ay, Interpolation;
	double					z;
	TSG_Point				p;
	CSG_Parameter_Grid_List	*pGrids;
	CSG_Grid				*pMerged, *pGrid;

	//-----------------------------------------------------
	pGrids			= Parameters("GRIDS")		->asGridList();
	pMerged			= Parameters("GRID_TARGET")	->asGrid();
	Interpolation	= Parameters("INTERPOL")	->asInt();
	bMean			= Parameters("OVERLAP")		->asInt() == 0;

	//-----------------------------------------------------
	if( pGrids->Get_Count() < 2 )
	{
		Message_Dlg(_TL("There is nothing to merge, because less than 2 grids have been selected."), Get_Name());
	}

	//-----------------------------------------------------
	else
	{
		if( pMerged == NULL )
		{
			bool			bResampling;
			double			Cellsize;
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

			for(i=1, Cellsize=pGrids->asGrid(0)->Get_Cellsize(), bResampling=false; i<pGrids->Get_Count(); i++)
			{
				if( Cellsize != pGrids->asGrid(i)->Get_Cellsize() )
				{
					bResampling	= true;

					if( Cellsize > pGrids->asGrid(i)->Get_Cellsize() )
					{
						Cellsize	= pGrids->asGrid(i)->Get_Cellsize();
					}
				}
			}

			if( bResampling )
			{
				Get_Parameters("MERGE_INFO")->Get_Parameter("MESH_SIZE")->Set_Value(Cellsize);

				if( !Dlg_Parameters("MERGE_INFO") )
				{
					return( false );
				}

				Cellsize	= Get_Parameters("MERGE_INFO")->Get_Parameter("MESH_SIZE")->asDouble();
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

			Parameters("MERGED")->Set_Value(pMerged	= SG_Create_Grid(
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

		pMerged->Set_Name(_TL("Merged Grid"));

		pMerged->Assign_NoData();

		//-------------------------------------------------
		for(i=0; i<pGrids->Get_Count(); i++)
		{
			Process_Set_Text(CSG_String::Format(SG_T("%s: %d (%d)"), _TL("merging grid"), i + 1, pGrids->Get_Count()));

			pGrid	= pGrids->asGrid(i);

			ax		= (int)((pGrid->Get_XMin() - pMerged->Get_XMin()) / pMerged->Get_Cellsize());
			ay		= (int)((pGrid->Get_YMin() - pMerged->Get_YMin()) / pMerged->Get_Cellsize());

			//---------------------------------------------
			if(	pGrid->Get_Cellsize() == pMerged->Get_Cellsize()
			&&	fabs(fmod(pGrid->Get_XMin() - pMerged->Get_XMin(), pMerged->Get_Cellsize())) <= 0.001 * pMerged->Get_Cellsize()
			&&	fabs(fmod(pGrid->Get_YMin() - pMerged->Get_YMin(), pMerged->Get_Cellsize())) <= 0.001 * pMerged->Get_Cellsize() )
			{
				for(y=ay, iy=0; iy<pGrid->Get_NY() && Set_Progress(iy, pGrid->Get_NY()); y++, iy++)
				{
					if( y >= 0 && y < pMerged->Get_NY() )
					{
						for(x=ax, ix=0; ix<pGrid->Get_NX(); x++, ix++)
						{
							if( x >= 0 && x < pMerged->Get_NX() && !pGrid->is_NoData(ix, iy) )
							{
								if( pMerged->is_NoData(x, y) )
								{
									pMerged->Set_Value(x, y, pGrid->asDouble(ix, iy));
								}
								else if( bMean )
								{
									pMerged->Set_Value(x, y, 0.5 * (pMerged->asDouble(x, y) + pGrid->asDouble(ix, iy)));
								}
							}
						}
					}
				}
			}

			//---------------------------------------------
			else
			{
				Message_Add(_TL("interpolating grid: "));	Message_Add(pGrid->Get_Name(), false);

				p.y	= pMerged->Get_YMin() + ay * pMerged->Get_Cellsize();

				for(y=ay; p.y<=pGrid->Get_YMax() && Set_Progress(p.y-pGrid->Get_YMin(), pGrid->Get_System().Get_YRange()); y++, p.y+=pMerged->Get_Cellsize())
				{
					if( y >= 0 && y < pMerged->Get_NY() )
					{
						p.x	= pMerged->Get_XMin() + ax * pMerged->Get_Cellsize();

						for(x=ax; p.x<pGrid->Get_XMax(); x++, p.x+=pMerged->Get_Cellsize())
						{
							if( x >= 0 && x < pMerged->Get_NX() && pGrid->Get_Value(p, z, Interpolation) )
							{
								if( pMerged->is_NoData(x, y) )
								{
									pMerged->Set_Value(x, y, z);
								}
								else if( bMean )
								{
									pMerged->Set_Value(x, y, 0.5 * (pMerged->asDouble(x, y) + z));
								}
							}
						}
					}
				}
			}
		}


		//-------------------------------------------------
		DataObject_Update(pMerged);

		return( true );
	}

	return( false );
}


///////////////////////////////////////////////////////////
//														 //
//														 //
//														 //
///////////////////////////////////////////////////////////

//---------------------------------------------------------

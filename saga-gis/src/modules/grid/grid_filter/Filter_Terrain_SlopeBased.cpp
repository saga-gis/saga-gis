
///////////////////////////////////////////////////////////
//                                                       //
//                         SAGA                          //
//                                                       //
//      System for Automated Geoscientific Analyses      //
//                                                       //
//                    Module Library:                    //
//                      Grid_Filter                      //
//                                                       //
//-------------------------------------------------------//
//                                                       //
//              Filter_Terrain_SlopeBased.cpp            //
//                                                       //
//                 Copyright (C) 2010 by                 //
//                    Volker Wichmann                    //
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
//    e-mail:     wichmann@laserdata                     //
//                                                       //
//    contact:    Volker Wichmann                        //
//                LASERDATA GmbH                         //
//                Management and analysis of             //
//                laserscanning data                     //
//                Innsbruck, Austria                     //
//                                                       //
///////////////////////////////////////////////////////////

//---------------------------------------------------------


///////////////////////////////////////////////////////////
//														 //
//														 //
//														 //
///////////////////////////////////////////////////////////

//---------------------------------------------------------
#include "Filter_Terrain_SlopeBased.h"
#include "./../grid_tools/Grid_Gaps.h"


///////////////////////////////////////////////////////////
//														 //
//				Construction/Destruction				 //
//														 //
///////////////////////////////////////////////////////////

//---------------------------------------------------------
CFilter_Terrain_SlopeBased::CFilter_Terrain_SlopeBased(void)
{

	//-----------------------------------------------------
	Set_Name(_TL("DTM Filter (slope-based)"));

	Set_Author(_TL("Volker Wichmann (c) 2010, LASERDATA GmbH"));

	Set_Description	(_TW(
		"The module can be used to filter a digital surface model (DSM), i.e. to classify "
		"its cells into bare earth and object cells (ground and nonground cells).\n\n"
		"The module uses concepts described by VOSSELMAN (2000) and is based on the "
		"assumption that a large height difference between two nearby cells is unlikely "
		"to be caused by a steep slope in the terrain. The probability that the higher cell "
		"could be a ground point decreases if the distance between the two cells decreases. "
		"Therefore the filter defines the acceptable height difference between two cells as "
		"a function of the distance between the cells. A cell is classified as terrain if "
		"there is no other cell within the kernel search radius such that the height difference "
		"between these cells is larger than the allowed maximum height difference at the distance "
		"between these cells.\n\n"
		"The approximate terrain slope parameter is used to modify the filter function to match "
		"the overall slope in the study area. A confidence interval may be used to reject outliers.\n\n"
		"Reference:\n"
		"VOSSELMAN, G. (2000): Slope based filtering of laser altimetry data. IAPRS, Vol. XXXIII, "
		"Part B3, Amsterdam, The Netherlands. pp. 935-942\n\n")
	);


	//-----------------------------------------------------
	Parameters.Add_Grid(
		NULL, "INPUT", _TL("Grid to filter"),
		_TL("The grid to filter."),
		PARAMETER_INPUT
	);

	Parameters.Add_Value(
		NULL, "RADIUS", _TL("Search Radius"),
		_TL("Search radius of kernel in cells."),
		PARAMETER_TYPE_Int, 2, 1, true
	);

	/*Parameters.Add_Value(
		NULL, "ITERATIONS", _TL("Iterations"),
		_TL("Number of iterations, starts with search radius and increments with one cell by iteration."),
		PARAMETER_TYPE_Int, 1, 1, true
	);*/

	Parameters.Add_Value(
		NULL, "TERRAINSLOPE", _TL("Approx. Terrain Slope"),
		_TL("Approximate Terrain Slope [%]"),
		PARAMETER_TYPE_Double, 30.0, 0.0, true
	);

	Parameters.Add_Value(
		NULL, "STDDEV", _TL("Use Confidence Interval"),
		_TL("Use 5 percent confidence interval"),
		PARAMETER_TYPE_Bool, false
	);

	Parameters.Add_Grid(
		NULL, "GROUND", _TL("Bare Earth"),
		_TL("Filtered DTM"),
		PARAMETER_OUTPUT
	);

	Parameters.Add_Grid(
		NULL, "NONGROUND", _TL("Removed Objects"),
		_TL("Objects removed from input grid"),
		PARAMETER_OUTPUT
	);

}

//---------------------------------------------------------
CFilter_Terrain_SlopeBased::~CFilter_Terrain_SlopeBased(void)
{}


///////////////////////////////////////////////////////////
//														 //
//														 //
//														 //
///////////////////////////////////////////////////////////

//---------------------------------------------------------
bool CFilter_Terrain_SlopeBased::On_Execute(void)
{
	CSG_Grid			*pInput, *pGround, *pNonGround;
	CSG_Grid_Radius		gridRadius;
	int					sradius;
	int					iterations;
	double				tslope;
	bool				bStdDev;

	CSG_Grid			*pFilter;
	std::vector<double>	dzKernel;
	int					gradius;
	int					x, y, ix, iy, iPoint;
	double				dz;
	int					n;
	double				sum, sumsq, stddev, confInter;
	double				min_z, ie;
	
	//---------------------------------------------------------

    pInput		= Parameters("INPUT")->asGrid();
	pGround		= Parameters("GROUND")->asGrid();
	pNonGround	= Parameters("NONGROUND")->asGrid();
	sradius		= Parameters("RADIUS")->asInt() + 1;
	iterations	= 1; //Parameters("ITERATIONS")->asInt();
	tslope		= Parameters("TERRAINSLOPE")->asDouble() / 100.0;
    bStdDev     = Parameters("STDDEV")->asBool();

	
	//---------------------------------------------------------
	pFilter    = SG_Create_Grid(SG_DATATYPE_Double, pInput->Get_NX(), pInput->Get_NY(), pInput->Get_Cellsize(), pInput->Get_XMin(), pInput->Get_YMin());
	
	pGround		->Assign(pInput);
	pFilter		->Assign_NoData();
	pNonGround	->Assign_NoData();



	for( int iter=0; iter<iterations; iter++ )
	{
		//---------------------------------------------------------
		//Create dzKernel
		
		gradius = sradius + iter;

		gridRadius.Create(gradius);

		x = gradius + 1;
		y = gradius + 1;

		for( iPoint=0; iPoint<gridRadius.Get_nPoints(); iPoint++ )
		{
			dz = gridRadius.Get_Point(iPoint, x, y, ix, iy) * tslope;
			dzKernel.push_back(dz);
		}
		
		for( y=0; y<pGround->Get_NY() && Set_Progress(y); y++ )
		{
			for( x=0; x<pGround->Get_NX(); x++ )
			{
				if( !pGround->is_NoData(x, y) )
				{
					if( bStdDev )
					{
						// calc stddev
						sum	= sumsq	= 0.0;
						n	= 0;

						for( iPoint=1; iPoint<gridRadius.Get_nPoints(); iPoint++ )
						{
							gridRadius.Get_Point(iPoint, x, y, ix, iy);

							if( pGround->is_InGrid(ix, iy, true) )
							{
								n++;
								sum		+= pGround->asDouble(ix, iy);
								sumsq	+= pGround->asDouble(ix, iy) * pGround->asDouble(ix, iy);
							}
						}
						
						stddev = sqrt(sumsq - n * pow(sum/n, 2)) / (n - 1);
						//stddev = sqrt((sumsq - sum * sum / n) / n);

						confInter = 1.65 * sqrt(2 * stddev);
					}
					else
						confInter = 0.0;


					// calc erosion
					min_z	= 999999.0;

					for( iPoint=1; iPoint<gridRadius.Get_nPoints(); iPoint++ )
					{
						gridRadius.Get_Point(iPoint, x, y, ix, iy);

						if( pGround->is_InGrid(ix, iy, true) )
						{
							ie = pGround->asDouble(ix, iy) + dzKernel[iPoint] + confInter;

							if( ie < min_z )
								min_z = ie;
						}
					}

					// classify
					if( pGround->asDouble(x, y) <= min_z )
						pFilter->Set_Value(x, y, pGround->asDouble(x, y));
					else
						pNonGround->Set_Value(x, y, pGround->asDouble(x, y));
				}// if not NoData
			}// for x
		}// for y

		pGround->Assign(pFilter);
		pFilter->Assign_NoData();

		gridRadius.Destroy();

		/*CGrid_Gaps	CloseGaps;

		if(	!CloseGaps.Get_Parameters()->Set_Parameter(SG_T("INPUT")	, PARAMETER_TYPE_Grid, pGround)
		||	!CloseGaps.Execute() )
		{
			return( false );
		}*/

	}// for iter


	//-----------------------------------------------------
	return( true );
}



///////////////////////////////////////////////////////////
//														 //
//														 //
//														 //
///////////////////////////////////////////////////////////

//---------------------------------------------------------

/**********************************************************
 * Version $Id: tpi.cpp 911 2011-02-14 16:38:15Z reklov_w $
 *********************************************************/

///////////////////////////////////////////////////////////
//                                                       //
//                         SAGA                          //
//                                                       //
//      System for Automated Geoscientific Analyses      //
//                                                       //
//                    Module Library:                    //
//                    ta_morphometry                     //
//                                                       //
//-------------------------------------------------------//
//                                                       //
//                        tpi.cpp                        //
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
#include "tpi.h"


///////////////////////////////////////////////////////////
//														 //
//														 //
//														 //
///////////////////////////////////////////////////////////

//---------------------------------------------------------
CTPI::CTPI(void)
{
	CSG_Parameter	*pNode;

	//-----------------------------------------------------
	// 1. Info...

	Set_Name		(_TL("Topographic Position Index (TPI)"));

	Set_Author		(SG_T("O.Conrad (c) 2011"));

	Set_Description	(_TW(
		"Topographic Position Index (TPI) calculation as proposed by Guisan et al. (1999). "
		"This is literally the same as the difference to the mean calculation (residual analysis) "
		"proposed by Wilson & Gallant (2000).\n"
		"\n"
		"References:\n"
		"- Guisan, A., Weiss, S.B., Weiss, A.D. (1999): GLM versus CCA spatial modeling of plant species distribution. Plant Ecology 143: 107-122.\n"
		"- Weiss, A.D. (2000): Topographic Position and Landforms Analysis. <a target=\"_blank\" href=\"http://www.jennessent.com/downloads/tpi-poster-tnc_18x22.pdf\">poster</a>.\n"
		"- Wilson, J.P. & Gallant, J.C. (2000): Terrain Analysis - Principles and Applications.\n"
	));


	//-----------------------------------------------------
	// 2. Parameters...

	Parameters.Add_Grid(
		NULL	, "DEM"			, _TL("Elevation"),
		_TL(""),
		PARAMETER_INPUT
	);

	pNode	= Parameters.Add_Grid(
		NULL	, "TPI"			, _TL("Topographic Position Index"),
		_TL(""),
		PARAMETER_OUTPUT
	);

	Parameters.Add_Value(
		NULL	, "STANDARD"	, _TL("Standardize"),
		_TL(""),
		PARAMETER_TYPE_Bool, false
	);

	Parameters.Add_Range(
		NULL	, "RADIUS"		, _TL("Radius"),
		_TL("radius in map units"),
		0.0, 100.0, 0.0, true
	);

	m_Cells.Get_Weighting().Set_BandWidth(75.0);	// 75%
	Parameters.Add_Parameters(
		NULL	, "WEIGHTING"	, _TL("Weighting"),
		_TL("bandwidth has to be given as percentage of the (outer) radius")
	)->asParameters()->Assign(m_Cells.Get_Weighting().Get_Parameters());
}


///////////////////////////////////////////////////////////
//														 //
///////////////////////////////////////////////////////////

//---------------------------------------------------------
bool CTPI::On_Execute(void)
{
	m_pDEM		= Parameters("DEM")	->asGrid();
	m_pTPI		= Parameters("TPI")	->asGrid();

	DataObject_Set_Colors(m_pTPI, 100, SG_COLORS_RED_GREY_BLUE, true);

	//-----------------------------------------------------
	double	r_inner	= Parameters("RADIUS")->asRange()->Get_LoVal() / Get_Cellsize();
	double	r_outer	= Parameters("RADIUS")->asRange()->Get_HiVal() / Get_Cellsize();

	m_Cells.Get_Weighting().Set_Parameters(Parameters("WEIGHTING")->asParameters());
	m_Cells.Get_Weighting().Set_BandWidth(r_outer * m_Cells.Get_Weighting().Get_BandWidth() / 100.0);

	if( !m_Cells.Set_Annulus(r_inner, r_outer) )
	{
		return( false );
	}

	//-----------------------------------------------------
	for(int y=0; y<Get_NY() && Set_Progress(y); y++)
	{
		for(int x=0; x<Get_NX(); x++)
		{
			Get_Statistics(x, y);
		}
	}

	//-----------------------------------------------------
	m_Cells.Destroy();

	if( Parameters("STANDARD")->asBool() )
	{
		m_pTPI->Standardise();
	}

	return( true );
}


///////////////////////////////////////////////////////////
//														 //
///////////////////////////////////////////////////////////

//---------------------------------------------------------
bool CTPI::Get_Statistics(int x, int y)
{
	if( m_pDEM->is_InGrid(x, y) )
	{
		int		i, ix, iy;
		double	z, id, iw;

		CSG_Simple_Statistics	Statistics;

		for(i=0, z=m_pDEM->asDouble(x, y); i<m_Cells.Get_Count(); i++)
		{
			if( m_Cells.Get_Values(i, ix = x, iy = y, id, iw, true) && id >= 0.0 && m_pDEM->is_InGrid(ix, iy) )
			{
				Statistics.Add_Value(m_pDEM->asDouble(ix, iy), iw);
			}
		}

		//-------------------------------------------------
		if( Statistics.Get_Weights() > 0.0 )
		{
			m_pTPI->Set_Value(x, y, z - Statistics.Get_Mean());

			return( true );
		}
	}

	//-----------------------------------------------------
	m_pTPI->Set_NoData(x, y);

	return( false );
}


///////////////////////////////////////////////////////////
//														 //
//														 //
//														 //
///////////////////////////////////////////////////////////

//---------------------------------------------------------
CTPI_Classification::CTPI_Classification(void)
{
	CSG_Parameter	*pNode;

	//-----------------------------------------------------
	// 1. Info...

	Set_Name		(_TL("TPI Based Landform Classification"));

	Set_Author		(SG_T("O.Conrad (c) 2011"));

	Set_Description	(_TW(
		"Topographic Position Index (TPI) calculation as proposed by Guisan et al. (1999). "
		"This is literally the same as the difference to the mean calculation (residual analysis) "
		"proposed by Wilson & Gallant (2000).\n"
		"\n"
		"References:\n"
		"- Guisan, A., Weiss, S.B., Weiss, A.D. (1999): GLM versus CCA spatial modeling of plant species distribution. Plant Ecology 143: 107-122.\n"
		"- Weiss, A.D. (2000): Topographic Position and Landforms Analysis. <a target=\"_blank\" href=\"http://www.jennessent.com/downloads/tpi-poster-tnc_18x22.pdf\">poster</a>.\n"
		"- Wilson, J.P. & Gallant, J.C. (2000): Terrain Analysis - Principles and Applications.\n"
	));


	//-----------------------------------------------------
	// 2. Parameters...

	Parameters.Add_Grid(
		NULL	, "DEM"			, _TL("Elevation"),
		_TL(""),
		PARAMETER_INPUT
	);

	pNode	= Parameters.Add_Grid(
		NULL	, "LANDFORMS"	, _TL("Landforms"),
		_TL(""),
		PARAMETER_OUTPUT
	);

	Parameters.Add_Range(
		NULL	, "RADIUS_A"	, _TL("Radius"),
		_TL("radius in map units"),
		0.0, 100.0, 0.0, true
	);

	Parameters.Add_Range(
		NULL	, "RADIUS_B"	, _TL("Radius"),
		_TL("radius in map units"),
		0.0, 1000.0, 0.0, true
	);

	m_Weighting.Set_BandWidth(75.0);	// 75%
	Parameters.Add_Parameters(
		NULL	, "WEIGHTING"	, _TL("Weighting"),
		_TL("bandwidth has to be given as percentage of the (outer) radius")
	)->asParameters()->Assign(m_Weighting.Get_Parameters());
}


///////////////////////////////////////////////////////////
//														 //
///////////////////////////////////////////////////////////

//---------------------------------------------------------
enum
{
	LF_CANYON		= 0,
	LF_MID_SLOPE,
	LF_UPLAND,
	LF_VALLEY,
	LF_PLAIN,
	LF_OPEN_SLOPE,
	LF_UPPER_SLOPE,
	LF_LOCAL_RIDGE,
	LF_MIDSLOPE_RIDGE,
	LF_HIGH_RIDGE,
	LF_COUNT
};


///////////////////////////////////////////////////////////
//														 //
///////////////////////////////////////////////////////////

//---------------------------------------------------------
bool CTPI_Classification::On_Execute(void)
{
	double		Slope, Aspect;
	CSG_Grid	*pDEM, *pLandforms, TPI_A, TPI_B;
	CTPI		TPI;

	//-----------------------------------------------------
	pDEM		= Parameters("DEM")			->asGrid();
	pLandforms	= Parameters("LANDFORMS")	->asGrid();

	TPI.Get_Parameters()->Set_Parameter("DEM"      , Parameters("DEM"));
	TPI.Get_Parameters()->Set_Parameter("WEIGHTING", Parameters("WEIGHTING"));
	TPI.Get_Parameters()->Get_Parameter("STANDARD")->Set_Value(true);

	//-----------------------------------------------------
	TPI_A.Create(*Get_System());
	TPI.Get_Parameters()->Get_Parameter("TPI")->Set_Value(&TPI_A);
	TPI.Get_Parameters()->Set_Parameter("RADIUS", Parameters("RADIUS_A"));

	if( !TPI.Execute() )
	{
		return( false );
	}

	//-----------------------------------------------------
	TPI_B.Create(*Get_System());
	TPI.Get_Parameters()->Get_Parameter("TPI")->Set_Value(&TPI_B);
	TPI.Get_Parameters()->Set_Parameter("RADIUS", Parameters("RADIUS_B"));

	if( !TPI.Execute() )
	{
		return( false );
	}

	//-----------------------------------------------------
	for(int y=0; y<Get_NY() && Set_Progress(y); y++)
	{
		for(int x=0; x<Get_NX(); x++)
		{
			if( pDEM->is_NoData(x, y) )
			{
				pLandforms->Set_Value(x, y, -1.0);
			}
			else
			{
				double	A	= TPI_A.asDouble(x, y);
				double	B	= TPI_B.asDouble(x, y);

				if( A <= -1.0 )
				{
					if( B <= -1.0 )
					{	// Canyons, Deeply Incised Streams
						pLandforms->Set_Value(x, y, LF_CANYON);
					}
					else if( B < 1.0 )
					{	// Midslope Drainages, Shallow Valleys
						pLandforms->Set_Value(x, y, LF_MID_SLOPE);
					}
					else // if( B >= 1.0 )
					{	// Upland Drainages, Headwaters
						pLandforms->Set_Value(x, y, LF_UPLAND);
					}
				}
				else if( A < 1.0 )
				{
					if( B <= -1.0 )
					{	// U-shaped Valleys
						pLandforms->Set_Value(x, y, LF_VALLEY);
					}
					else if( B < 1.0 )
					{
						pDEM->Get_Gradient(x, y, Slope, Aspect);

						if( Slope <= 5.0 * M_DEG_TO_RAD )
						{	// Plains
							pLandforms->Set_Value(x, y, LF_PLAIN);
						}
						else
						{	// Open Slopes
							pLandforms->Set_Value(x, y, LF_OPEN_SLOPE);
						}
					}
					else // if( B >= 1.0 )
					{	// Upper Slopes, Mesas
						pLandforms->Set_Value(x, y, LF_UPPER_SLOPE);
					}
				}
				else // if( A >= 1.0 )
				{
					if( B <= -1.0 )
					{	// Local Ridges, Hills in Valleys
						pLandforms->Set_Value(x, y, LF_LOCAL_RIDGE);
					}
					else if( B < 1.0 )
					{	// Midslope Ridges, Small Hills in Plains
						pLandforms->Set_Value(x, y, LF_MIDSLOPE_RIDGE);
					}
					else // if( B >= 1.0 )
					{	// Mountain Tops, High Ridges
						pLandforms->Set_Value(x, y, LF_HIGH_RIDGE);
					}
				}
			}
		}
	}

	//-----------------------------------------------------
	CSG_Parameters	P;

	if( DataObject_Get_Parameters(pLandforms, P) && P("COLORS_TYPE") && P("LUT") )
	{
		int LF_Colors[LF_COUNT]	=
		{
			SG_GET_RGB(  0,   0, 127),	// LF_CANYON
			SG_GET_RGB(200, 200, 255),	// LF_MID_SLOPE
			SG_GET_RGB(  0, 200, 255),	// LF_UPLAND
			SG_GET_RGB(127, 127, 255),	// LF_VALLEY
			SG_GET_RGB(255, 255, 128),	// LF_PLAIN
			SG_GET_RGB(128, 255,   0),	// LF_OPEN_SLOPE
			SG_GET_RGB(  0, 255,   0),	// LF_UPPER_SLOPE
			SG_GET_RGB(255, 200, 127),	// LF_LOCAL_RIDGE
			SG_GET_RGB(255, 127,   0),	// LF_MIDSLOPE_RIDGE
			SG_GET_RGB(255,   0,   0)	// LF_HIGH_RIDGE
		};

		//-------------------------------------------------
		CSG_Strings	Name, Desc;

		Name	+= _TL("Streams");				Desc	+= _TL("Canyons, Deeply Incised Streams");
		Name	+= _TL("Midslope Drainages");	Desc	+= _TL("Midslope Drainages, Shallow Valleys");
		Name	+= _TL("Upland Drainages");		Desc	+= _TL("Upland Drainages, Headwaters");
		Name	+= _TL("Valleys");				Desc	+= _TL("U-shaped Valleys");
		Name	+= _TL("Plains");				Desc	+= _TL("Plains");
		Name	+= _TL("Open Slopes");			Desc	+= _TL("Open Slopes");
		Name	+= _TL("Upper Slopes");			Desc	+= _TL("Upper Slopes, Mesas");
		Name	+= _TL("Local Ridges");			Desc	+= _TL("Local Ridges, Hills in Valleys");
		Name	+= _TL("Midslope Ridges");		Desc	+= _TL("Midslope Ridges, Small Hills in Plains");
		Name	+= _TL("High Ridges");			Desc	+= _TL("Mountain Tops, High Ridges");

		//-------------------------------------------------
		CSG_Table	*pTable	= P("LUT")->asTable();

		pTable->Del_Records();

		for(int i=0; i<LF_COUNT; i++)
		{
			CSG_Table_Record	*pRecord	= pTable->Add_Record();

			pRecord->Set_Value(0, LF_Colors[i]);
			pRecord->Set_Value(1, Name[i].c_str());
			pRecord->Set_Value(2, Desc[i].c_str());
			pRecord->Set_Value(3, i);
			pRecord->Set_Value(4, i);
		}

		P("COLORS_TYPE")->Set_Value(1);	// Color Classification Type: Lookup Table

		DataObject_Set_Parameters(pLandforms, P);
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

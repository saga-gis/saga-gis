/**********************************************************
 * Version $Id: tpi.cpp 911 2011-02-14 16:38:15Z reklov_w $
 *********************************************************/

///////////////////////////////////////////////////////////
//                                                       //
//                         SAGA                          //
//                                                       //
//      System for Automated Geoscientific Analyses      //
//                                                       //
//                     Tool Library                      //
//                    ta_morphometry                     //
//                                                       //
//-------------------------------------------------------//
//                                                       //
//                  tc_iwahashi_pike.cpp                 //
//                                                       //
//                 Copyright (C) 2012 by                 //
//                      Olaf Conrad                      //
//                                                       //
//-------------------------------------------------------//
//                                                       //
// This file is part of 'SAGA - System for Automated     //
// Geoscientific Analyses'. SAGA is free software; you   //
// can redistribute it and/or modify it under the terms  //
// of the GNU General Public License as published by the //
// Free Software Foundation, either version 2 of the     //
// License, or (at your option) any later version.       //
//                                                       //
// SAGA is distributed in the hope that it will be       //
// useful, but WITHOUT ANY WARRANTY; without even the    //
// implied warranty of MERCHANTABILITY or FITNESS FOR A  //
// PARTICULAR PURPOSE. See the GNU General Public        //
// License for more details.                             //
//                                                       //
// You should have received a copy of the GNU General    //
// Public License along with this program; if not, see   //
// <http://www.gnu.org/licenses/>.                       //
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
#include "tc_iwahashi_pike.h"


///////////////////////////////////////////////////////////
//														 //
//														 //
//														 //
///////////////////////////////////////////////////////////

//---------------------------------------------------------
CTC_Parameter_Base::CTC_Parameter_Base(void)
{}

//---------------------------------------------------------
void CTC_Parameter_Base::On_Construction(void)
{
	Parameters.Add_Value(
		NULL	, "SCALE"		, _TL("Scale (Cells)"),
		_TL(""),
		PARAMETER_TYPE_Int, 10, 1, true
	);

	Parameters.Add_Choice(
		NULL	, "METHOD"		, _TL("Method"),
		_TL(""),
		CSG_String::Format("%s|%s|",
			_TL("counting cells"),
			_TL("resampling")
		), 1
	);

	m_Kernel.Get_Weighting().Set_Weighting(SG_DISTWGHT_GAUSS);
	m_Kernel.Get_Weighting().Set_BandWidth(0.7);
	m_Kernel.Get_Weighting().Create_Parameters(&Parameters, false);
}


///////////////////////////////////////////////////////////
//														 //
///////////////////////////////////////////////////////////

//---------------------------------------------------------
int CTC_Parameter_Base::On_Parameters_Enable(CSG_Parameters *pParameters, CSG_Parameter *pParameter)
{
	if( !SG_STR_CMP(pParameter->Get_Identifier(), "METHOD") )
	{
		pParameters->Set_Enabled("DISTANCE_WEIGHTING", pParameter->asInt() == 0);
	}

	m_Kernel.Get_Weighting().Enable_Parameters(pParameters);

	return( CSG_Tool_Grid::On_Parameters_Enable(pParameters, pParameter) );
}


///////////////////////////////////////////////////////////
//														 //
///////////////////////////////////////////////////////////

//---------------------------------------------------------
bool CTC_Parameter_Base::Get_Parameter(CSG_Grid *pValues, CSG_Grid *pParameter)
{
	DataObject_Set_Colors(pParameter, 10, SG_COLORS_RED_GREY_BLUE, true);

	//-----------------------------------------------------
	if( Parameters("METHOD")->asInt() == 0 )
	{
		m_Kernel.Get_Weighting().Set_Parameters(&Parameters);
		m_Kernel.Get_Weighting().Set_BandWidth(Parameters("SCALE")->asDouble() * m_Kernel.Get_Weighting().Get_BandWidth());
		m_Kernel.Set_Radius(Parameters("SCALE")->asDouble());

		for(int y=0; y<Get_NY() && Set_Progress(y); y++)
		{
			#pragma omp parallel for
			for(int x=0; x<Get_NX(); x++)
			{
				if( pValues->is_NoData(x, y) )
				{
					pParameter->Set_NoData(x, y);
				}
				else
				{
					double	d, w, nTotal = 0.0, nValid = 0.0;

					for(int i=0, ix, iy; i<m_Kernel.Get_Count(); i++)
					{
						if( m_Kernel.Get_Values(i, ix = x, iy = y, d, w, true) && pValues->is_InGrid(ix, iy) )
						{
							nTotal	+= w;

							if( pValues->asInt(ix, iy) != 0 )
							{
								nValid	+= w;
							}
						}
					}

					pParameter->Set_Value(x, y, nTotal > 0.0 ? 100.0 * nValid / nTotal : 0.0);	// make percentage
				}
			}
		}

		m_Kernel.Destroy();
	}

	//-----------------------------------------------------
	else
	{
		double		Cellsize	= Parameters("SCALE")->asInt() * Get_Cellsize();

		if( Cellsize > 0.5 * SG_Get_Length(Get_System()->Get_XRange(), Get_System()->Get_YRange()) )
		{
			Error_Set(_TL("resampling cell size is too large"));

			return( false );
		}

		CSG_Grid	Values(CSG_Grid_System(Cellsize, Get_XMin(), Get_YMin(), Get_XMax(), Get_YMax()), SG_DATATYPE_Float);

		Values.Assign(pValues, GRID_RESAMPLING_Mean_Cells);

		for(int y=0; y<Get_NY() && Set_Progress(y); y++)
		{
			double	py	= Get_YMin() + y * Get_Cellsize();

			#pragma omp parallel for
			for(int x=0; x<Get_NX(); x++)
			{
				double	z, px	= Get_XMin() + x * Get_Cellsize();

				if( pValues->is_NoData(x, y) || !Values.Get_Value(px, py, z) )
				{
					pParameter->Set_NoData(x, y);
				}
				else
				{
					pParameter->Set_Value(x, y, 100.0 * z);	// make percentage
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
CTC_Texture::CTC_Texture(void)
{
	//-----------------------------------------------------
	Set_Name		(_TL("Terrain Surface Texture"));

	Set_Author		("O.Conrad (c) 2012");

	Set_Description	(_TW(
		"Terrain surface texture as proposed by Iwahashi & Pike (2007) for subsequent terrain classification.\n"
		"\n"
		"Reference:\n"
		"Iwahashi, J. & Pike, R.J. (2007): "
		"Automated classifications of topography from DEMs by an unsupervised nested-means algorithm and a three-part geometric signature. "
		"Geomorphology, Vol. 86, pp. 409–440\n"
	));

	//-----------------------------------------------------
	Parameters.Add_Grid(
		NULL	, "DEM"			, _TL("Elevation"),
		_TL(""),
		PARAMETER_INPUT
	);

	Parameters.Add_Grid(
		NULL	, "TEXTURE"		, _TL("Texture"),
		_TL(""),
		PARAMETER_OUTPUT
	);

	Parameters.Add_Value(
		NULL	, "EPSILON"		, _TL("Flat Area Threshold"),
		_TL("maximum difference between original and median filtered elevation (3x3 moving window) that still is recognized flat"),
		PARAMETER_TYPE_Double, 1.0, 0.0, true
	);

	//-----------------------------------------------------
	On_Construction();
}


///////////////////////////////////////////////////////////
//														 //
///////////////////////////////////////////////////////////

//---------------------------------------------------------
bool CTC_Texture::On_Execute(void)
{
	//-----------------------------------------------------
	CSG_Grid	Noise(*Get_System(), SG_DATATYPE_Char);

	double	Epsilon	= Parameters("EPSILON")->asDouble();

	m_pDEM	= Parameters("DEM")->asGrid();

	//-----------------------------------------------------
	for(int y=0; y<Get_NY() && Set_Progress(y); y++)
	{
		#pragma omp parallel for
		for(int x=0; x<Get_NX(); x++)
		{
			if( m_pDEM->is_NoData(x, y) )
			{
				Noise.Set_NoData(x, y);
			}
			else
			{
				Noise.Set_Value(x, y, Get_Noise(x, y, Epsilon));
			}
		}
	}

	//-----------------------------------------------------
	return( Get_Parameter(&Noise, Parameters("TEXTURE")->asGrid()) );
}


///////////////////////////////////////////////////////////
//														 //
///////////////////////////////////////////////////////////

//---------------------------------------------------------
// returns 1 if the cell represents a 'peak/crest' or
// if the cell is a 'pit/trough' based on the difference to
// median of a 3x3 neighbourhood
//---------------------------------------------------------
int CTC_Texture::Get_Noise(int x, int y, double Epsilon)
{
	CSG_Simple_Statistics	s(true);

	for(int iy=y-1; iy<=y+1; iy++)
	{
		for(int ix=x-1; ix<=x+1; ix++)
		{
			if( m_pDEM->is_InGrid(ix, iy) )
			{
				s	+= m_pDEM->asDouble(ix, iy);
			}
		}
	}

	return( fabs(m_pDEM->asDouble(x, y) - s.Get_Median()) > Epsilon ? 1 : 0 );
}


///////////////////////////////////////////////////////////
//														 //
//														 //
//														 //
///////////////////////////////////////////////////////////

//---------------------------------------------------------
CTC_Convexity::CTC_Convexity(void)
{
	//-----------------------------------------------------
	Set_Name		(_TL("Terrain Surface Convexity"));

	Set_Author		("O.Conrad (c) 2012");

	Set_Description	(_TW(
		"Terrain surface convexity as proposed by Iwahashi & Pike (2007) for subsequent terrain classification.\n"
		"\n"
		"Reference:\n"
		"Iwahashi, J. & Pike, R.J. (2007): "
		"Automated classifications of topography from DEMs by an unsupervised nested-means algorithm and a three-part geometric signature. "
		"Geomorphology, Vol. 86, pp. 409–440\n"
	));

	//-----------------------------------------------------
	Parameters.Add_Grid(
		NULL	, "DEM"			, _TL("Elevation"),
		_TL(""),
		PARAMETER_INPUT
	);

	Parameters.Add_Grid(
		NULL	, "CONVEXITY"	, _TL("Convexity"),
		_TL(""),
		PARAMETER_OUTPUT
	);

	Parameters.Add_Choice(
		NULL	, "KERNEL"		, _TL("Laplacian Filter Kernel"),
		_TL(""),
		CSG_String::Format("%s|%s|%s|",
			_TL("conventional four-neighbourhood"),
			_TL("conventional eight-neihbourhood"),
			_TL("eight-neihbourhood (distance based weighting)")
		)
	);

	Parameters.Add_Choice(
		NULL	, "TYPE"		, _TL("Type"),
		_TL(""),
		CSG_String::Format("%s|%s|",
			_TL("convexity"),
			_TL("concavity")
		)
	);

	Parameters.Add_Value(
		NULL	, "EPSILON"		, _TL("Flat Area Threshold"),
		_TL(""),
		PARAMETER_TYPE_Double, 0.0, 0.0, true
	);

	//-----------------------------------------------------
	On_Construction();
}


///////////////////////////////////////////////////////////
//														 //
///////////////////////////////////////////////////////////

//---------------------------------------------------------
bool CTC_Convexity::On_Execute(void)
{
	//-----------------------------------------------------
	const double	Kernels[3][2]	= { { 1, 0 }, { 1, 1 }, { 1, 1 / sqrt(2.0) } };

	int	Kernel	= Parameters("KERNEL")->asInt();

	//-----------------------------------------------------
	CSG_Grid	Laplace(*Get_System(), SG_DATATYPE_Char);

	double	Epsilon	= Parameters("EPSILON")->asDouble();
	int		Type	= Parameters("TYPE"   )->asInt   ();

	m_pDEM	= Parameters("DEM")->asGrid();

	//-----------------------------------------------------
	for(int y=0; y<Get_NY() && Set_Progress(y); y++)
	{
		#pragma omp parallel for
		for(int x=0; x<Get_NX(); x++)
		{
			if( m_pDEM->is_NoData(x, y) )
			{
				Laplace.Set_NoData(x, y);
			}
			else
			{
				Laplace.Set_Value(x, y, Get_Laplace(x, y, Kernels[Kernel], Type, Epsilon));
			}
		}
	}

	//-----------------------------------------------------
	return( Get_Parameter(&Laplace, Parameters("CONVEXITY")->asGrid()) );
}


///////////////////////////////////////////////////////////
//														 //
///////////////////////////////////////////////////////////

//---------------------------------------------------------
int CTC_Convexity::Get_Laplace(int x, int y, const double Kernel[2], int Type, double Epsilon)
{
	double	Sum	= 4 * (Kernel[0] + Kernel[1]) * m_pDEM->asDouble(x, y);

	for(int i=0; i<8; i++)
	{
		int		ix	= Get_xTo(i, x);
		int		iy	= Get_yTo(i, y);

		double	iz	= m_pDEM->is_InGrid(ix, iy) ? m_pDEM->asDouble(ix, iy) : m_pDEM->asDouble(x, y);

		Sum		-= Kernel[i % 2] * iz;
	}

	return( Type == 0
		? Sum >  Epsilon ? 1 : 0
		: Sum < -Epsilon ? 1 : 0
	);
}


///////////////////////////////////////////////////////////
//														 //
//														 //
//														 //
///////////////////////////////////////////////////////////

//---------------------------------------------------------
#define CLASS_FLAG_NODATA		0xFF	// 11111111
#define CLASS_FLAG_SLOPE		0x40	// 01000000
#define CLASS_FLAG_CONVEXITY	0x20	// 00100000
#define CLASS_FLAG_TEXTURE		0x10	// 00010000
#define CLASS_FLAG_LEVEL		0x07	// 00000111


///////////////////////////////////////////////////////////
//														 //
///////////////////////////////////////////////////////////

//---------------------------------------------------------
CTC_Classification::CTC_Classification(void)
{
	CSG_Parameter	*pNode;

	//-----------------------------------------------------
	Set_Name		(_TL("Terrain Surface Classification (Iwahashi and Pike)"));

	Set_Author		("O.Conrad (c) 2012");

	Set_Description	(_TW(
		"Terrain surface classification as proposed by Iwahashi & Pike (2007).\n"
		"\n"
		"Reference:\n"
		"Iwahashi, J. & Pike, R.J. (2007): "
		"Automated classifications of topography from DEMs by an unsupervised nested-means algorithm and a three-part geometric signature. "
		"Geomorphology, Vol. 86, pp. 409–440\n"
	));

	//-----------------------------------------------------
	Parameters.Add_Grid(
		NULL	, "DEM"			, _TL("Elevation"),
		_TL(""),
		PARAMETER_INPUT
	);

	Parameters.Add_Grid(
		NULL	, "SLOPE"		, _TL("Slope"),
		_TL(""),
		PARAMETER_INPUT_OPTIONAL
	);

	pNode	= Parameters.Add_Grid(
		NULL	, "CONVEXITY"	, _TL("Convexity"),
		_TL(""),
		PARAMETER_INPUT_OPTIONAL
	);

	Parameters.Add_Value(
		pNode	, "CONV_RECALC"	, _TL("Recalculate"),
		_TL(""),
		PARAMETER_TYPE_Bool, false
	);

	pNode	= Parameters.Add_Grid(
		NULL	, "TEXTURE"		, _TL("Texture"),
		_TL(""),
		PARAMETER_INPUT_OPTIONAL
	);

	Parameters.Add_Value(
		pNode	, "TEXT_RECALC"	, _TL("Recalculate"),
		_TL(""),
		PARAMETER_TYPE_Bool, false
	);

	Parameters.Add_Grid(
		NULL	, "LANDFORMS"	, _TL("Landforms"),
		_TL(""),
		PARAMETER_OUTPUT, true, SG_DATATYPE_Byte
	);

	Parameters.Add_Choice(
		NULL	, "TYPE"		, _TL("Number of Classes"),
		_TL(""),
		SG_T("8|12|16|"), 2
	);

	//-----------------------------------------------------
	pNode	= Parameters.Add_Node(
		NULL	, "CONV_NODE"	, _TL("Convexity"),
		_TL("")
	);

	Parameters.Add_Value(
		pNode	, "CONV_SCALE"	, _TL("Scale (Cells)"),
		_TL(""),
		PARAMETER_TYPE_Int, 10, 1, true
	);

	Parameters.Add_Choice(
		pNode	, "CONV_KERNEL"	, _TL("Laplacian Filter Kernel"),
		_TL(""),
		CSG_String::Format("%s|%s|%s|",
			_TL("four-neighbourhood"),
			_TL("eight-neihbourhood"),
			_TL("eight-neihbourhood (distance based weighting)")
		)
	);

	Parameters.Add_Choice(
		pNode	, "CONV_TYPE"	, _TL("Type"),
		_TL(""),
		CSG_String::Format("%s|%s|",
			_TL("convexity"),
			_TL("concavity")
		)
	);

	Parameters.Add_Value(
		pNode	, "CONV_EPSILON", _TL("Flat Area Threshold"),
		_TL(""),
		PARAMETER_TYPE_Double, 0.0, 0.0, true
	);

	//-----------------------------------------------------
	pNode	= Parameters.Add_Node(
		NULL	, "TEXT_NODE"	, _TL("Texture"),
		_TL("")
	);

	Parameters.Add_Value(
		pNode	, "TEXT_SCALE"	, _TL("Scale (Cells)"),
		_TL(""),
		PARAMETER_TYPE_Int, 10, 1, true
	);

	Parameters.Add_Value(
		pNode	, "TEXT_EPSILON", _TL("Flat Area Threshold"),
		_TL("maximum difference between original and median filtered elevation (3x3 moving window) that still is recognized flat"),
		PARAMETER_TYPE_Double, 1.0, 0.0, true
	);
}


///////////////////////////////////////////////////////////
//														 //
///////////////////////////////////////////////////////////

//---------------------------------------------------------
int CTC_Classification::On_Parameters_Enable(CSG_Parameters *pParameters, CSG_Parameter *pParameter)
{
	pParameters->Set_Enabled("DEM",
		pParameters->Get_Parameter("SLOPE"      )->asGrid() == NULL
	||	pParameters->Get_Parameter("CONVEXITY"  )->asGrid() == NULL
	||	pParameters->Get_Parameter("TEXTURE"    )->asGrid() == NULL
	);

	if( !SG_STR_CMP(pParameter->Get_Identifier(), "CONVEXITY") )
	{
		pParameters->Set_Enabled("CONV_RECALC", pParameter->asGrid() != NULL);
	}

	pParameters->Set_Enabled("CONV_NODE",
		pParameters->Get_Parameter("CONVEXITY"  )->asGrid() == NULL
	||	pParameters->Get_Parameter("CONV_RECALC")->asBool()
	);

	if( !SG_STR_CMP(pParameter->Get_Identifier(), "TEXTURE") )
	{
		pParameters->Set_Enabled("TEXT_RECALC" , pParameter->asGrid() != NULL);
	}

	pParameters->Set_Enabled("TEXT_NODE",
		pParameters->Get_Parameter("TEXTURE"    )->asGrid() == NULL
	||	pParameters->Get_Parameter("TEXT_RECALC")->asBool()
	);

	return( CSG_Tool_Grid::On_Parameters_Enable(pParameters, pParameter) );
}


///////////////////////////////////////////////////////////
//														 //
///////////////////////////////////////////////////////////

//---------------------------------------------------------
bool CTC_Classification::On_Execute(void)
{
	//-----------------------------------------------------
	m_pSlope		= Parameters("SLOPE"    )->asGrid();
	m_pConvexity	= Parameters("CONVEXITY")->asGrid();
	m_pTexture		= Parameters("TEXTURE"  )->asGrid();

	if( (!m_pSlope || !m_pConvexity || !m_pTexture) && !Parameters("DEM")->asGrid() )
	{
		return( false );
	}

	//-----------------------------------------------------
	CSG_Grid	Slope;

	if( !m_pSlope )
	{
		Slope.Create(*Get_System());	m_pSlope	= &Slope;

		CSG_Grid	*pDEM	= Parameters("DEM")->asGrid();

		for(int y=0; y<Get_NY() && Set_Progress(y); y++)
		{
			#pragma omp parallel for
			for(int x=0; x<Get_NX(); x++)
			{
				double	s, a;

				if( pDEM->Get_Gradient(x, y, s, a) )
				{
					Slope.Set_Value(x, y, s);
				}
				else
				{
					Slope.Set_NoData(x, y);
				}
			}
		}
	}

	//-----------------------------------------------------
	if( !m_pConvexity || Parameters("CONV_RECALC")->asBool() )
	{
		CTC_Convexity	c;

		c.Set_Parameter(      "DEM", Parameters("DEM"));
		c.Set_Parameter(    "SCALE", Parameters("CONV_SCALE"));
		c.Set_Parameter(   "KERNEL", Parameters("CONV_KERNEL"));
		c.Set_Parameter(     "TYPE", Parameters("CONV_TYPE"));
		c.Set_Parameter(  "EPSILON", Parameters("CONV_EPSILON"));
		c.Set_Parameter("CONVEXITY", m_pConvexity);

		if( !c.Execute() )
		{
			return( false );
		}

		Parameters("CONVEXITY")->Set_Value(m_pConvexity = c.Get_Parameters()->Get_Parameter("CONVEXITY")->asGrid());
	}

	//-----------------------------------------------------
	if( !m_pTexture || Parameters("TEXT_RECALC")->asBool() )
	{
		CTC_Texture	c;

		c.Set_Parameter(    "DEM", Parameters("DEM"));
		c.Set_Parameter(  "SCALE", Parameters("TEXT_SCALE"));
		c.Set_Parameter("EPSILON", Parameters("TEXT_EPSILON"));
		c.Set_Parameter("TEXTURE", m_pTexture);

		if( !c.Execute() )
		{
			return( false );
		}

		Parameters("TEXTURE")->Set_Value(m_pTexture = c.Get_Parameters()->Get_Parameter("TEXTURE")->asGrid());
	}

	//-----------------------------------------------------
	return( Get_Classes() );
}


///////////////////////////////////////////////////////////
//														 //
///////////////////////////////////////////////////////////

//---------------------------------------------------------
bool CTC_Classification::Get_Classes(void)
{
	//-----------------------------------------------------
	int	Level, nLevels	= 1 + Parameters("TYPE")->asInt();

	CSG_Grid	*pLandforms	= Parameters("LANDFORMS")->asGrid();

	pLandforms->Assign(0.0);
	pLandforms->Set_NoData_Value(CLASS_FLAG_NODATA);

	Set_LUT(pLandforms, nLevels);

	//-----------------------------------------------------
	for(Level=1; Level<=nLevels && Process_Get_Okay(); Level++)
	{
		Process_Set_Text(CSG_String::Format("%s: %d", _TL("Level"), Level));

		m_Mean_Slope		= Level == 1 ? m_pSlope    ->Get_Mean() : m_Stat_Slope    .Get_Mean();
		m_Mean_Convexity	= Level == 1 ? m_pConvexity->Get_Mean() : m_Stat_Convexity.Get_Mean();
		m_Mean_Texture  	= Level == 1 ? m_pTexture  ->Get_Mean() : m_Stat_Texture  .Get_Mean();

		m_Stat_Slope    .Invalidate();
		m_Stat_Convexity.Invalidate();
		m_Stat_Texture  .Invalidate();

		for(int y=0; y<Get_NY() && Set_Progress(y); y++)
		{
			for(int x=0; x<Get_NX(); x++)
			{
				if( pLandforms->asInt(x, y) == 0 )
				{
					pLandforms->Set_Value(x, y, Get_Class(Level, x, y, Level == nLevels));
				}
			}
		}
	}

	//-----------------------------------------------------
	return( true );
}

//---------------------------------------------------------
int CTC_Classification::Get_Class(int Level, int x, int y, bool bLastLevel)
{
	if( Level == 1 && (m_pSlope->is_NoData(x, y) || m_pConvexity->is_NoData(x, y) || m_pTexture->is_NoData(x, y)) )
	{
		return( CLASS_FLAG_NODATA );
	}

	//-----------------------------------------------------
	if( m_pSlope->asDouble(x, y) > m_Mean_Slope )
	{
		Level	|= CLASS_FLAG_SLOPE;
	}
	else if( !bLastLevel )
	{
		m_Stat_Slope		+= m_pSlope    ->asDouble(x, y);
		m_Stat_Convexity	+= m_pConvexity->asDouble(x, y);
		m_Stat_Texture		+= m_pTexture  ->asDouble(x, y);

		return( 0 );	// check again on next level
	}

	if( m_pConvexity->asDouble(x, y) > m_Mean_Convexity )
	{
		Level	|= CLASS_FLAG_CONVEXITY;
	}

	if( m_pTexture->asDouble(x, y) > m_Mean_Texture )
	{
		Level	|= CLASS_FLAG_TEXTURE;
	}

	return( Level );
}


///////////////////////////////////////////////////////////
//														 //
///////////////////////////////////////////////////////////

//---------------------------------------------------------
#define ADD_CLASS(Level, Slope, Convexity, Texture, Color, sSlope)	{\
	CSG_Table_Record	*pClass	= pLUT->asTable()->Add_Record();\
	pClass->Set_Value(0, SG_GET_RGB(SG_GET_B(Color), SG_GET_G(Color), SG_GET_R(Color)));\
	pClass->Set_Value(1, CSG_String::Format("%d) %s slope, %s texture, %s convexity", pLUT->asTable()->Get_Count(), SG_T(sSlope),\
		Texture   ? SG_T("fine") : SG_T("coarse"),\
		Convexity ? SG_T("high") : SG_T("low")));\
	pClass->Set_Value(2, pClass->asString(1));\
	pClass->Set_Value(3, Level|(Slope ? CLASS_FLAG_SLOPE : 0)|(Convexity ? CLASS_FLAG_CONVEXITY : 0)|(Texture ? CLASS_FLAG_TEXTURE : 0));\
	pClass->Set_Value(4, pClass->asInt(3));\
}

//---------------------------------------------------------
void CTC_Classification::Set_LUT(CSG_Grid *pLandforms, int nLevels)
{
	CSG_Parameter	*pLUT	= DataObject_Get_Parameter(pLandforms, "LUT");

	if( !pLUT || !pLUT->asTable() )
	{
		return;
	}

	pLUT->asTable()->Del_Records();

	switch( nLevels )
	{
	case 1:		// 8 classes
		//        l  s  c  t
		ADD_CLASS(1, 1, 1, 1, 0x7F4633, "steep"     );	// class  1
		ADD_CLASS(1, 1, 1, 0, 0xFF00FF, "steep"     );	// class  2
		ADD_CLASS(1, 1, 0, 1, 0xC16E2E, "steep"     );	// class  3
		ADD_CLASS(1, 1, 0, 0, 0xFC97CF, "steep"     );	// class  4

		ADD_CLASS(1, 0, 1, 1, 0x66A726, "gentle"    );	// class  5
		ADD_CLASS(1, 0, 1, 0, 0xFFFF40, "gentle"    );	// class  6
		ADD_CLASS(1, 0, 0, 1, 0xAEFF7B, "gentle"    );	// class  7
		ADD_CLASS(1, 0, 0, 0, 0xFFFFBF, "gentle"    );	// class  8
		break;

	case 2:		// 12 classes
		ADD_CLASS(1, 1, 1, 1, 0x7F4633, "steep"     );	// class  1
		ADD_CLASS(1, 1, 1, 0, 0xFF00FF, "steep"     );	// class  2
		ADD_CLASS(1, 1, 0, 1, 0xC16E2E, "steep"     );	// class  3
		ADD_CLASS(1, 1, 0, 0, 0xFC97CF, "steep"     );	// class  4

		ADD_CLASS(2, 1, 1, 1, 0xFA9600, "moderate"  );	// class  5
		ADD_CLASS(2, 1, 1, 0, 0xFD43A8, "moderate"  );	// class  6
		ADD_CLASS(2, 1, 0, 1, 0xFBCF66, "moderate"  );	// class  7
		ADD_CLASS(2, 1, 0, 0, 0xF9C4D4, "moderate"  );	// class  8

		ADD_CLASS(2, 0, 1, 1, 0x66A726, "gentle"    );	// class  9
		ADD_CLASS(2, 0, 1, 0, 0xFFFF40, "gentle"    );	// class 10
		ADD_CLASS(2, 0, 0, 1, 0xAEFF7B, "gentle"    );	// class 11
		ADD_CLASS(2, 0, 0, 0, 0xFFFFBF, "gentle"    );	// class 12
		break;

	default:	// 16 classes
		ADD_CLASS(1, 1, 1, 1, 0x7F4633, "very steep");	// class  1
		ADD_CLASS(1, 1, 1, 0, 0xFF00FF, "very steep");	// class  2
		ADD_CLASS(1, 1, 0, 1, 0xC16E2E, "very steep");	// class  3
		ADD_CLASS(1, 1, 0, 0, 0xFC97CF, "very steep");	// class  4

		ADD_CLASS(2, 1, 1, 1, 0xFA9600, "steep"     );	// class  5
		ADD_CLASS(2, 1, 1, 0, 0xFD43A8, "steep"     );	// class  6
		ADD_CLASS(2, 1, 0, 1, 0xFBCF66, "steep"     );	// class  7
		ADD_CLASS(2, 1, 0, 0, 0xF9C4D4, "steep"     );	// class  8

		ADD_CLASS(3, 1, 1, 1, 0x019E73, "moderate"  );	// class  9
		ADD_CLASS(3, 1, 1, 0, 0xC4B71D, "moderate"  );	// class 10
		ADD_CLASS(3, 1, 0, 1, 0x0072B1, "moderate"  );	// class 11
		ADD_CLASS(3, 1, 0, 0, 0xD7D700, "moderate"  );	// class 12

		ADD_CLASS(3, 0, 1, 1, 0xA0FD8E, "gentle"    );	// class 13
		ADD_CLASS(3, 0, 1, 0, 0xE9E915, "gentle"    );	// class 14
		ADD_CLASS(3, 0, 0, 1, 0x003B5B, "gentle"    );	// class 15
		ADD_CLASS(3, 0, 0, 0, 0xF2FEC0, "gentle"    );	// class 16
		break;
	}

	DataObject_Set_Parameter(pLandforms, pLUT);
	DataObject_Set_Parameter(pLandforms, "COLORS_TYPE", 1);	// Color Classification Type: Lookup Table
}


///////////////////////////////////////////////////////////
//														 //
//														 //
//														 //
///////////////////////////////////////////////////////////

//---------------------------------------------------------

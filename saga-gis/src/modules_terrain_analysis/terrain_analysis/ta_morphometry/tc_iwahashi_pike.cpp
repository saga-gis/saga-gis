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
#include "tc_iwahashi_pike.h"


///////////////////////////////////////////////////////////
//														 //
//														 //
//														 //
///////////////////////////////////////////////////////////

//---------------------------------------------------------
CTC_Texture::CTC_Texture(void)
{
	//-----------------------------------------------------
	// 1. Info...

	Set_Name		(_TL("Terrain Surface Texture"));

	Set_Author		(SG_T("O.Conrad (c) 2012"));

	Set_Description	(_TW(
		"Terrain surface texture as proposed by Iwahashi & Pike (2007) for subsequent terrain classification.\n"
		"\n"
		"Reference:\n"
		"Iwahashi, J. & Pike, R.J. (2007): "
		"Automated classifications of topography from DEMs by an unsupervised nested-means algorithm and a three-part geometric signature. "
		"Geomorphology, Vol. 86, pp. 409–440\n"
	));


	//-----------------------------------------------------
	// 2. Parameters...

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
		NULL	, "RADIUS"		, _TL("Radius"),
		_TL(""),
		PARAMETER_TYPE_Int, 10, 1, true
	);
}

///////////////////////////////////////////////////////////
//														 //
///////////////////////////////////////////////////////////

//---------------------------------------------------------
bool CTC_Texture::On_Execute(void)
{
	//-----------------------------------------------------
	int			y;

	CSG_Grid	Noise(*Get_System(), SG_DATATYPE_Char);
	m_pNoise	= &Noise;

	m_pDEM		= Parameters("DEM"    )->asGrid();
	m_pTexture	= Parameters("TEXTURE")->asGrid();

	DataObject_Set_Colors(m_pTexture, 100, SG_COLORS_RED_GREY_BLUE, true);

	//-----------------------------------------------------
	for(y=0; y<Get_NY() && Set_Progress(y); y++)
	{
		#pragma omp parallel for
		for(int x=0; x<Get_NX(); x++)
		{
			Noise.Set_Value(x, y, Get_MedianDiff(x, y));
		}
	}

	//-----------------------------------------------------
	m_Kernel.Set_Radius(Parameters("RADIUS")->asInt());

	for(y=0; y<Get_NY() && Set_Progress(y); y++)
	{
		#pragma omp parallel for
		for(int x=0; x<Get_NX(); x++)
		{
			Get_Texture(x, y);
		}
	}

	m_Kernel.Destroy();

	//-----------------------------------------------------
	return( true );
}

///////////////////////////////////////////////////////////
//														 //
///////////////////////////////////////////////////////////

//---------------------------------------------------------
int CTC_Texture::Get_MedianDiff(int x, int y)
{
	int		n	= 0;

	if( m_pDEM->is_InGrid(x, y) )
	{
		double	z	= m_pDEM->asDouble(x, y);

		for(int iy=y-1; iy<=y+1; iy++)
		{
			for(int ix=x-1; ix<=x+1; ix++)
			{
				if( (ix != x || iy != y) && m_pDEM->is_InGrid(ix, iy) )
				{
					double	iz	= m_pDEM->asDouble(ix, iy);

					n	= z < iz ? n + 1
						: z > iz ? n - 1 : n;
				}
			}
		}
	}

	return( n );
}

//---------------------------------------------------------
int CTC_Texture::Get_Texture(int x, int y)
{
	if( m_pDEM->is_InGrid(x, y) )
	{
		int		nNoise	= 0;
		int		nTotal	= 0;

		for(int i=0; i<m_Kernel.Get_Count(); i++)
		{
			int	ix	= m_Kernel.Get_X(i, x);
			int	iy	= m_Kernel.Get_Y(i, y);

			if( m_pNoise->is_InGrid(ix, iy) )
			{
				nTotal++;

				if( m_pNoise->asDouble(ix, iy) != 0.0 )
				{
					nNoise++;
				}
			}
		}

		m_pTexture->Set_Value(x, y, nNoise / (double)nTotal);

		return( true );
	}

	//-----------------------------------------------------
	m_pTexture->Set_NoData(x, y);

	return( false );
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
	// 1. Info...

	Set_Name		(_TL("Terrain Surface Convexity"));

	Set_Author		(SG_T("O.Conrad (c) 2012"));

	Set_Description	(_TW(
		"Terrain surface convexity as proposed by Iwahashi & Pike (2007) for subsequent terrain classification.\n"
		"\n"
		"Reference:\n"
		"Iwahashi, J. & Pike, R.J. (2007): "
		"Automated classifications of topography from DEMs by an unsupervised nested-means algorithm and a three-part geometric signature. "
		"Geomorphology, Vol. 86, pp. 409–440\n"
	));


	//-----------------------------------------------------
	// 2. Parameters...

	Parameters.Add_Grid(
		NULL	, "DEM"			, _TL("Elevation"),
		_TL(""),
		PARAMETER_INPUT
	);

	Parameters.Add_Grid(
		NULL	, "CONVEX"		, _TL("Convexity"),
		_TL(""),
		PARAMETER_OUTPUT
	);

	Parameters.Add_Value(
		NULL	, "RADIUS"		, _TL("Radius"),
		_TL(""),
		PARAMETER_TYPE_Int, 10, 1, true
	);
}


///////////////////////////////////////////////////////////
//														 //
///////////////////////////////////////////////////////////

//---------------------------------------------------------
bool CTC_Convexity::On_Execute(void)
{
	//-----------------------------------------------------
	int			y;

	CSG_Grid	Laplace(*Get_System(), SG_DATATYPE_Char);
	m_pLaplace	= &Laplace;

	m_pDEM			= Parameters("DEM"   )->asGrid();
	m_pConvexity	= Parameters("CONVEX")->asGrid();

	DataObject_Set_Colors(m_pConvexity, 100, SG_COLORS_RED_GREY_BLUE, true);

	switch( 0 )
	{
	case 0:	m_dLaplace[0]	= 1;	m_dLaplace[1]	= 0;	break;
	case 1:	m_dLaplace[0]	= 1;	m_dLaplace[1]	= 1;	break;
	case 2:	m_dLaplace[0]	= 2;	m_dLaplace[1]	= 1;	break;
	}

	//-----------------------------------------------------
	for(y=0; y<Get_NY() && Set_Progress(y); y++)
	{
		#pragma omp parallel for
		for(int x=0; x<Get_NX(); x++)
		{
			Laplace.Set_Value(x, y, Get_Laplace(x, y));
		}
	}

	//-----------------------------------------------------
	m_Kernel.Set_Radius(Parameters("RADIUS")->asInt());

	for(y=0; y<Get_NY() && Set_Progress(y); y++)
	{
		#pragma omp parallel for
		for(int x=0; x<Get_NX(); x++)
		{
			Get_Convexity(x, y);
		}
	}

	m_Kernel.Destroy();

	//-----------------------------------------------------
	return( true );
}

///////////////////////////////////////////////////////////
//														 //
///////////////////////////////////////////////////////////

//---------------------------------------------------------
int CTC_Convexity::Get_Laplace(int x, int y)
{
	if( m_pDEM->is_InGrid(x, y) )
	{
		double	Sum		= 4 * (m_dLaplace[0] + m_dLaplace[1]) * m_pDEM->asDouble(x, y);

		for(int i=0; i<8; i++)
		{
			int		ix	= Get_xTo(i, x);
			int		iy	= Get_yTo(i, y);
			double	iz	= m_pDEM->is_InGrid(ix, iy) ? m_pDEM->asDouble(ix, iy) : m_pDEM->asDouble(x, y);

			Sum		-= m_dLaplace[i % 2] * iz;
		}

		return( Sum > 0.0 ? 1 : 0 );
	}

	return( 0 );
}

//---------------------------------------------------------
int CTC_Convexity::Get_Convexity(int x, int y)
{
	if( m_pDEM->is_InGrid(x, y) )
	{
		int		nConvex	= 0;
		int		nTotal	= 0;

		for(int i=0; i<m_Kernel.Get_Count(); i++)
		{
			int	ix	= m_Kernel.Get_X(i, x);
			int	iy	= m_Kernel.Get_Y(i, y);

			if( m_pLaplace->is_InGrid(ix, iy) )
			{
				nTotal++;

				if( m_pLaplace->asDouble(ix, iy) > 0.0 )
				{
					nConvex++;
				}
			}
		}

		m_pConvexity->Set_Value(x, y, nConvex / (double)nTotal);

		return( true );
	}

	//-----------------------------------------------------
	m_pConvexity->Set_NoData(x, y);

	return( false );
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
	//-----------------------------------------------------
	// 1. Info...

	Set_Name		(_TL("Terrain Surface Classification"));

	Set_Author		(SG_T("O.Conrad (c) 2012"));

	Set_Description	(_TW(
		"Terrain surface classification as proposed by Iwahashi & Pike (2007).\n"
		"\n"
		"Reference:\n"
		"Iwahashi, J. & Pike, R.J. (2007): "
		"Automated classifications of topography from DEMs by an unsupervised nested-means algorithm and a three-part geometric signature. "
		"Geomorphology, Vol. 86, pp. 409–440\n"
	));


	//-----------------------------------------------------
	// 2. Parameters...

	Parameters.Add_Grid(
		NULL	, "DEM"			, _TL("Elevation"),
		_TL(""),
		PARAMETER_INPUT
	);

	Parameters.Add_Grid(
		NULL	, "CONVEX"		, _TL("Convexity"),
		_TL(""),
		PARAMETER_INPUT_OPTIONAL
	);

	Parameters.Add_Grid(
		NULL	, "TEXTURE"		, _TL("Texture"),
		_TL(""),
		PARAMETER_INPUT_OPTIONAL
	);

	Parameters.Add_Grid(
		NULL	, "LANDFORMS"	, _TL("Landforms"),
		_TL(""),
		PARAMETER_OUTPUT, true, SG_DATATYPE_Byte
	);

	Parameters.Add_Choice(
		NULL	, "TYPE"		, _TL("Number of Classes"),
		_TL(""),
		SG_T("8|12|16|"), 0
	);
}


///////////////////////////////////////////////////////////
//														 //
///////////////////////////////////////////////////////////

//---------------------------------------------------------
bool CTC_Classification::On_Execute(void)
{
	CSG_Grid	Slope, Convexity, Texture, *pDEM;

	//-----------------------------------------------------
	pDEM			= Parameters("DEM"      )->asGrid();
	m_pConvexity	= Parameters("CONVEX"   )->asGrid();
	m_pTexture		= Parameters("TEXTURE"  )->asGrid();
	m_pLandforms	= Parameters("LANDFORMS")->asGrid();

	//-----------------------------------------------------
	Slope.Create(*Get_System());
	m_pSlope	= &Slope;

	for(int y=0; y<Get_NY() && Set_Progress(y); y++)
	{
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

	//-----------------------------------------------------
	if( !m_pConvexity )
	{
		Convexity.Create(*Get_System());
		m_pConvexity	= &Convexity;

		CTC_Convexity	c;

		c.Get_Parameters()->Set_Parameter("DEM"   , pDEM);
		c.Get_Parameters()->Set_Parameter("CONVEX", m_pConvexity);

		if( !c.Execute() )
		{
			return( false );
		}
	}

	//-----------------------------------------------------
	if( !m_pTexture )
	{
		Texture.Create(*Get_System());
		m_pTexture	= &Texture;

		CTC_Texture	c;

		c.Get_Parameters()->Set_Parameter("DEM"    , pDEM);
		c.Get_Parameters()->Set_Parameter("TEXTURE", m_pTexture);

		if( !c.Execute() )
		{
			return( false );
		}
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

	m_pLandforms->Assign(0.0);
	m_pLandforms->Set_NoData_Value(CLASS_FLAG_NODATA);

	//-----------------------------------------------------
	for(Level=1; Level<=nLevels && Process_Get_Okay(); Level++)
	{
		Process_Set_Text(CSG_String::Format(SG_T("%s: %d"), _TL("Level"), Level));

		m_Mean_Slope		= Level == 1 ? m_pSlope    ->Get_ArithMean() : stat_Slope    .Get_Mean();
		m_Mean_Convexity	= Level == 1 ? m_pConvexity->Get_ArithMean() : stat_Convexity.Get_Mean();
		m_Mean_Texture  	= Level == 1 ? m_pTexture  ->Get_ArithMean() : stat_Texture  .Get_Mean();

		stat_Slope    .Invalidate();
		stat_Convexity.Invalidate();
		stat_Texture  .Invalidate();

		for(int y=0; y<Get_NY() && Set_Progress(y); y++)
		{
			for(int x=0; x<Get_NX(); x++)
			{
				Get_Class(Level, x, y, Level == nLevels);
			}
		}
	}

	//-----------------------------------------------------
	Set_LUT(nLevels);

	return( true );
}


///////////////////////////////////////////////////////////
//														 //
///////////////////////////////////////////////////////////

//---------------------------------------------------------
#define ADD_CLASS(Level, Slope, Convexity, Texture, Color, sSlope)	{\
	CSG_Table_Record	*pClass	= pLUT->asTable()->Add_Record();\
	pClass->Set_Value(0, Color);\
	pClass->Set_Value(1, CSG_String::Format(SG_T("%d) %s, %s convexity, %s texture"), pLUT->asTable()->Get_Count(), sSlope,\
		Convexity ? SG_T("high") : SG_T("low"),\
		Texture   ? SG_T("fine") : SG_T("coarse")));\
	pClass->Set_Value(2, pClass->asString(1));\
	pClass->Set_Value(3, Level|(Slope ? CLASS_FLAG_SLOPE : 0)|(Convexity ? CLASS_FLAG_CONVEXITY : 0)|(Texture ? CLASS_FLAG_TEXTURE : 0));\
	pClass->Set_Value(4, Level|(Slope ? CLASS_FLAG_SLOPE : 0)|(Convexity ? CLASS_FLAG_CONVEXITY : 0)|(Texture ? CLASS_FLAG_TEXTURE : 0));\
}

//---------------------------------------------------------
void CTC_Classification::Set_LUT(int nLevels)
{
	CSG_Parameter	*pLUT	= DataObject_Get_Parameter(m_pLandforms, "LUT");

	if( !pLUT || !pLUT->asTable() )
	{
		return;
	}

	pLUT->asTable()->Del_Records();

	switch( nLevels )
	{
	case 1:		// 8 classes
		ADD_CLASS(1, 1, 1, 1, SG_GET_RGB(124,  70,  44), SG_T("steep"));
		ADD_CLASS(1, 1, 1, 0, SG_GET_RGB(255,   0, 255), SG_T("steep"));
		ADD_CLASS(1, 1, 0, 1, SG_GET_RGB(190, 108,  40), SG_T("steep"));
		ADD_CLASS(1, 1, 0, 0, SG_GET_RGB(255, 146, 205), SG_T("steep"));

		ADD_CLASS(1, 0, 1, 1, SG_GET_RGB(248, 148,   0), SG_T("gentle"));
		ADD_CLASS(1, 0, 1, 0, SG_GET_RGB(255,  67, 162), SG_T("gentle"));
		ADD_CLASS(1, 0, 0, 1, SG_GET_RGB(248, 206,  97), SG_T("gentle"));
		ADD_CLASS(1, 0, 0, 0, SG_GET_RGB(248, 194, 208), SG_T("gentle"));
		break;

	case 2:		// 12 classes
		ADD_CLASS(1, 1, 1, 1, SG_GET_RGB(124,  70,  44), SG_T("steep"));
		ADD_CLASS(1, 1, 1, 0, SG_GET_RGB(255,   0, 255), SG_T("steep"));
		ADD_CLASS(1, 1, 0, 1, SG_GET_RGB(190, 108,  40), SG_T("steep"));
		ADD_CLASS(1, 1, 0, 0, SG_GET_RGB(255, 146, 205), SG_T("steep"));

		ADD_CLASS(2, 1, 1, 1, SG_GET_RGB(248, 148,   0), SG_T("moderate"));
		ADD_CLASS(2, 1, 1, 0, SG_GET_RGB(255,  67, 162), SG_T("moderate"));
		ADD_CLASS(2, 1, 0, 1, SG_GET_RGB(248, 206,  97), SG_T("moderate"));
		ADD_CLASS(2, 1, 0, 0, SG_GET_RGB(248, 194, 208), SG_T("moderate"));

		ADD_CLASS(2, 0, 1, 1, SG_GET_RGB(  0, 154, 111), SG_T("gentle"));
		ADD_CLASS(2, 0, 1, 0, SG_GET_RGB(192, 182,  20), SG_T("gentle"));
		ADD_CLASS(2, 0, 0, 1, SG_GET_RGB( 12, 106, 172), SG_T("gentle"));
		ADD_CLASS(2, 0, 0, 0, SG_GET_RGB(207, 214,   5), SG_T("gentle"));
		break;

	default:	// 16 classes
		ADD_CLASS(1, 1, 1, 1, SG_GET_RGB(124,  70,  44), SG_T("very steep"));
		ADD_CLASS(1, 1, 1, 0, SG_GET_RGB(255,   0, 255), SG_T("very steep"));
		ADD_CLASS(1, 1, 0, 1, SG_GET_RGB(190, 108,  40), SG_T("very steep"));
		ADD_CLASS(1, 1, 0, 0, SG_GET_RGB(255, 146, 205), SG_T("very steep"));

		ADD_CLASS(2, 1, 1, 1, SG_GET_RGB(248, 148,   0), SG_T("steep"));
		ADD_CLASS(2, 1, 1, 0, SG_GET_RGB(255,  67, 162), SG_T("steep"));
		ADD_CLASS(2, 1, 0, 1, SG_GET_RGB(248, 206,  97), SG_T("steep"));
		ADD_CLASS(2, 1, 0, 0, SG_GET_RGB(248, 194, 208), SG_T("steep"));

		ADD_CLASS(3, 1, 1, 1, SG_GET_RGB(  0, 154, 111), SG_T("moderate"));
		ADD_CLASS(3, 1, 1, 0, SG_GET_RGB(192, 182,  20), SG_T("moderate"));
		ADD_CLASS(3, 1, 0, 1, SG_GET_RGB( 12, 106, 172), SG_T("moderate"));
		ADD_CLASS(3, 1, 0, 0, SG_GET_RGB(207, 214,   5), SG_T("moderate"));

		ADD_CLASS(3, 0, 1, 1, SG_GET_RGB(149, 254, 139), SG_T("gentle"));
		ADD_CLASS(3, 0, 1, 0, SG_GET_RGB(226, 233,  16), SG_T("gentle"));
		ADD_CLASS(3, 0, 0, 1, SG_GET_RGB(  3,  56,  85), SG_T("gentle"));
		ADD_CLASS(3, 0, 0, 0, SG_GET_RGB(237, 255, 191), SG_T("gentle"));
		break;
	}

	DataObject_Set_Parameter(m_pLandforms, pLUT);
	DataObject_Set_Parameter(m_pLandforms, "COLORS_TYPE", 1);	// Color Classification Type: Lookup Table
}


///////////////////////////////////////////////////////////
//														 //
///////////////////////////////////////////////////////////

//---------------------------------------------------------
inline void CTC_Classification::Get_Class(int Level, int x, int y, bool bLastLevel)
{
	if( Level == 1 && (m_pSlope->is_NoData(x, y) || m_pConvexity->is_NoData(x, y) || m_pTexture->is_NoData(x, y)) )
	{
		m_pLandforms->Set_Value(x, y, CLASS_FLAG_NODATA);

		return;
	}

	if( m_pLandforms->asInt(x, y) != 0 )
	{
		return;
	}

	//-----------------------------------------------------
	if( m_pSlope->asDouble(x, y) <= m_Mean_Slope )
	{
		if( !bLastLevel )
		{
			stat_Slope		+= m_pSlope    ->asDouble(x, y);
			stat_Convexity	+= m_pConvexity->asDouble(x, y);
			stat_Texture	+= m_pTexture  ->asDouble(x, y);

			return;
		}
	}
	else
	{
		Level	|= CLASS_FLAG_SLOPE;
	}

	if( m_pConvexity->asDouble(x, y) > m_Mean_Convexity )
	{
		Level	|= CLASS_FLAG_CONVEXITY;
	}

	if( m_pTexture->asDouble(x, y) > m_Mean_Texture )
	{
		Level	|= CLASS_FLAG_TEXTURE;
	}

	m_pLandforms->Set_Value(x, y, Level);
}


///////////////////////////////////////////////////////////
//														 //
//														 //
//														 //
///////////////////////////////////////////////////////////

//---------------------------------------------------------

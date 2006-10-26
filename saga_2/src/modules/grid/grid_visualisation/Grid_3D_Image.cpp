
///////////////////////////////////////////////////////////
//                                                       //
//                         SAGA                          //
//                                                       //
//      System for Automated Geoscientific Analyses      //
//                                                       //
//                    Module Library:                    //
//                  Grid_Visualisation                   //
//                                                       //
//-------------------------------------------------------//
//                                                       //
//                   Grid_3D_Image.cpp                   //
//                                                       //
//                 Copyright (C) 2005 by                 //
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
#include "Grid_3D_Image.h"


///////////////////////////////////////////////////////////
//														 //
//														 //
//														 //
///////////////////////////////////////////////////////////

//---------------------------------------------------------
CGrid_3D_Image::CGrid_3D_Image(void)
{
	Set_Name(_TL("Create 3D Image"));

	Set_Author(_TL("Copyrights (c) 2005 by Olaf Conrad"));

	Set_Description(
		""
	);

	//-----------------------------------------------------
	Parameters.Add_Grid(
		NULL	, "DEM"				, _TL("Elevation"),
		"",
		PARAMETER_INPUT
	);

	Parameters.Add_Grid(
		NULL	, "IMAGE"			, _TL("Overlay Image"),
		"",
		PARAMETER_INPUT
	);

	Parameters.Add_Shapes_List(
		NULL	, "SHAPES"			, _TL("Shapes to project"),
		"",
		PARAMETER_INPUT_OPTIONAL
	);


	//-----------------------------------------------------
	Parameters.Add_Value(
		NULL	, "ZEXAGG"			, _TL("Exaggeration"),
		"",
		PARAMETER_TYPE_Double, 1.0
	);

	Parameters.Add_Value(
		NULL	, "ZEXAGG_MIN"		, _TL("Minimum Exaggeration [%]"),
		"",
		PARAMETER_TYPE_Double, 10.0, 0.0, true, 100.0, true
	);

	Parameters.Add_Value(
		NULL	, "Z_ROTATE"		, _TL("Image Rotation [Degree]"),
		"",
		PARAMETER_TYPE_Double, 0.0
	);

	Parameters.Add_Value(
		NULL	, "X_ROTATE"		, _TL("Local Rotation [Degree]"),
		"",
		PARAMETER_TYPE_Double, 1.0
	);

	Parameters.Add_Choice(
		NULL	, "X_ROTATE_LEVEL"	, _TL("Local Rotation Base Level"),
		"",

		"Zero|"
		"Mean Elevation|", 1
	);

	Parameters.Add_Value(
		NULL	, "PANBREAK"		, _TL("Panorama Break [%]"),
		"",
		PARAMETER_TYPE_Double, 70.0, 0.0, true, 100.0, true
	);

	Parameters.Add_Value(
		NULL	, "BKCOLOR"			, _TL("Background Color"),
		"",
		PARAMETER_TYPE_Color, SG_COLOR_BLACK
	);


	//-----------------------------------------------------
	Parameters.Add_Choice(
		NULL	, "PROJECTION"		, _TL("Projection"),
		"",
		_TL(
		"Panorama|"
		"Circular|"), 0
	);


	//-----------------------------------------------------
	Parameters.Add_Value(
		NULL	, "NX"				, _TL("3D Image Width"),
		"",
		PARAMETER_TYPE_Int, 100, 1, true
	);

	Parameters.Add_Value(
		NULL	, "NY"				, _TL("3D Image Height"),
		"",
		PARAMETER_TYPE_Int, 100, 1, true
	);

	Parameters.Add_Grid(
		NULL	, "RGB"				, _TL("3D Image"),
		"",
//		PARAMETER_OUTPUT_OPTIONAL, false, GRID_TYPE_Int
		PARAMETER_OUTPUT_OPTIONAL, true, GRID_TYPE_Int
	);

	Parameters.Add_Grid(
		NULL	, "RGB_Z"			, _TL("Projected Height"),
		"",
//		PARAMETER_OUTPUT_OPTIONAL, false, GRID_TYPE_Float
		PARAMETER_OUTPUT_OPTIONAL, true, GRID_TYPE_Float
	);
}

//---------------------------------------------------------
CGrid_3D_Image::~CGrid_3D_Image(void)
{}


///////////////////////////////////////////////////////////
//														 //
//														 //
//														 //
///////////////////////////////////////////////////////////

//---------------------------------------------------------
bool CGrid_3D_Image::On_Execute(void)
{
	//-----------------------------------------------------
	m_pDEM			= Parameters("DEM")				->asGrid();
	m_pImage		= Parameters("IMAGE")			->asGrid();

	m_Projection	= Parameters("PROJECTION")		->asInt();

	m_ZExagg		= Parameters("ZEXAGG")			->asDouble();
	m_ZExagg_Min	= Parameters("ZEXAGG_MIN")		->asDouble() / 100.0;
	m_ZMean			= Parameters("X_ROTATE_LEVEL")	->asInt() == 0 ? 0.0 : m_pDEM->Get_ZMin() + m_pDEM->Get_ZRange() / 2.0;
	m_XRotate		= Parameters("X_ROTATE")		->asDouble() * M_DEG_TO_RAD;
	m_ZRotate		= Parameters("Z_ROTATE")		->asDouble() * M_DEG_TO_RAD;

	m_PanoramaBreak	= Parameters("PANBREAK")		->asDouble() / 100.0;

	//-----------------------------------------------------
	m_pRGB			= Parameters("RGB")				->asGrid();
	m_pRGB_Z		= Parameters("RGB_Z")			->asGrid();

	if( !m_pRGB )
	{
		int		nx, ny;

		nx			= Parameters("NX")->asInt();
		ny			= Parameters("NY")->asInt();

		m_pRGB		= SG_Create_Grid(GRID_TYPE_Int		, nx, ny, 1.0);
	}

	if( !m_pRGB_Z || !m_pRGB_Z->is_Compatible(m_pRGB->Get_System()) )
	{
		m_pRGB_Z	= SG_Create_Grid(m_pRGB, GRID_TYPE_Float);
	}

	m_pRGB			->Set_Name(_TL("3D Image"));
	m_pRGB			->Assign(Parameters("BKCOLOR")->asDouble());

	m_pRGB_Z		->Set_Name(_TL("3D Image Height"));
	m_pRGB_Z		->Set_NoData_Value_Range(-999999, -999999);
	m_pRGB_Z		->Assign_NoData();

	//-----------------------------------------------------
	m_XScale		= (double)m_pRGB->Get_NX() / (double)Get_NX();
	m_YScale		= (double)m_pRGB->Get_NY() / (double)Get_NY();

	//-----------------------------------------------------
	_Set_Grid();

	CSG_Parameter_Shapes_List	*pShapes	= Parameters("SHAPES")->asShapesList();

	for(int i=0; i<pShapes->Get_Count(); i++)
	{
		_Set_Shapes(pShapes->asShapes(i));
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
#define DRAW_TRIANGLE(pa, pb, pc)	p[0] = pa; p[1] = pb; p[2] = pc; _Draw_Triangle(p);

//---------------------------------------------------------
void CGrid_3D_Image::_Set_Grid(void)
{
	T3DPoint	*a, *b, *c, *d, p[3];

	//-----------------------------------------------------
	a	= (T3DPoint *)SG_Malloc(sizeof(T3DPoint) *  Get_NX());
	b	= (T3DPoint *)SG_Malloc(sizeof(T3DPoint) *  Get_NX());
	c	= (T3DPoint *)SG_Malloc(sizeof(T3DPoint) * (Get_NX() - 1));

	//-----------------------------------------------------
	_Get_Line(0, b);

	for(int y=1; y<Get_NY() && Set_Progress(y); y++)
	{
		d	= a;
		a	= b;
		b	= d;

		_Get_Line(y, b);
		_Get_Line(a, b, c);

		for(int ax=0, bx=1; bx<Get_NX(); ax++, bx++)
		{
			DRAW_TRIANGLE(a[ax], a[bx], c[ax]);
			DRAW_TRIANGLE(b[ax], b[bx], c[ax]);
			DRAW_TRIANGLE(a[ax], b[ax], c[ax]);
			DRAW_TRIANGLE(a[bx], b[bx], c[ax]);
		}
	}

	//-----------------------------------------------------
	SG_Free(a);
	SG_Free(b);
	SG_Free(c);

	//-----------------------------------------------------
	DataObject_Add(m_pRGB_Z);
	DataObject_Add(m_pRGB);
	CSG_Colors	Colors(100, SG_COLORS_BLACK_WHITE);
	DataObject_Set_Colors(m_pRGB, Colors);
}

//---------------------------------------------------------
void CGrid_3D_Image::_Set_Shapes(CSG_Shapes *pInput)
{
	int			iShape, iPart, iPoint;
	double		x, y, z, dx, dy;
	T3DPoint	p;
	TSG_Point	Point;
	CSG_Shape		*pShape;
	CSG_Shapes		*pOutput;

	if( pInput && pInput->is_Valid() )
	{
		Process_Set_Text(CSG_String::Format(_TL("Project \'%s\'"), pInput->Get_Name()));

		pOutput	= SG_Create_Shapes(*pInput);
		dx		= (double)Get_NX() / Get_System()->Get_XRange();
		dy		= (double)Get_NY() / Get_System()->Get_YRange();

		for(iShape=0; iShape<pOutput->Get_Count() && Set_Progress(iShape, pOutput->Get_Count()); iShape++)
		{
			pShape	= pOutput->Get_Shape(iShape);

			for(iPart=0; iPart<pShape->Get_Part_Count(); iPart++)
			{
				for(iPoint=0; iPoint<pShape->Get_Point_Count(iPart); iPoint++)
				{
					Point	= pShape->Get_Point(iPoint, iPart);

					x		= dx * (Point.x - Get_XMin());
					y		= dy * (Point.y - Get_YMin());
					z		= m_pDEM->is_InGrid((int)x, (int)y, true) ? m_pDEM->asDouble((int)x, (int)y) : 0.0;

					_Get_Position(x, y, z, p);

					pShape->Set_Point(p.x, p.y, iPoint, iPart);
				}
			}
		}

		DataObject_Add(pOutput);
	}
}


///////////////////////////////////////////////////////////
//														 //
//														 //
//														 //
///////////////////////////////////////////////////////////

//---------------------------------------------------------
void CGrid_3D_Image::_Get_Line(int y, T3DPoint *p)
{
	for(int x=0; x<Get_NX(); x++)
	{
		if( m_pDEM->is_NoData(x, y) || m_pImage->is_NoData(x, y) )
		{
			p[x].bOk	= false;
		}
		else
		{
			p[x].r		= SG_GET_R(m_pImage->asInt(x, y));
			p[x].g		= SG_GET_G(m_pImage->asInt(x, y));
			p[x].b		= SG_GET_B(m_pImage->asInt(x, y));

			_Get_Position(x, y, m_pDEM->asDouble(x, y), p[x]);
		}
	}
}

//---------------------------------------------------------
void CGrid_3D_Image::_Get_Line(T3DPoint *a, T3DPoint *b, T3DPoint *c)
{
	for(int ax=0, bx=1; bx<Get_NX(); ax++, bx++)
	{
		if( (c[ax].bOk = a[ax].bOk && a[bx].bOk && b[ax].bOk && b[bx].bOk) == true )
		{
			c[ax].x	= (int)(0.5 +	(a[ax].x + a[bx].x + b[ax].x + b[bx].x) / 4.0);
			c[ax].y	= (int)(0.5 +	(a[ax].y + a[bx].y + b[ax].y + b[bx].y) / 4.0);
			c[ax].z	=				(a[ax].z + a[bx].z + b[ax].z + b[bx].z) / 4.0;

			c[ax].r	=				(a[ax].r + a[bx].r + b[ax].r + b[bx].r) / 4;
			c[ax].g	=				(a[ax].g + a[bx].g + b[ax].g + b[bx].g) / 4;
			c[ax].b	=				(a[ax].b + a[bx].b + b[ax].b + b[bx].b) / 4;
		}
	}
}


///////////////////////////////////////////////////////////
//														 //
//														 //
//														 //
///////////////////////////////////////////////////////////

//---------------------------------------------------------
inline void CGrid_3D_Image::_Get_Position(double x, double y, double z, T3DPoint &p)
{
	bool	bResult;

	//-----------------------------------------------------
	if( m_ZRotate != 0.0 )
	{
		_Get_Rotated(0.5 * Get_NX(), x, 0.5 * Get_NY(), y, m_ZRotate);
	}

	x	= m_XScale *  x;
	y	= m_YScale *  y;
	z	= m_ZExagg * ((z - m_ZMean) / Get_Cellsize());

	//-----------------------------------------------------
	switch( m_Projection )
	{
	case 0: default:
		bResult	= _Get_Panorama	(y, z);
		break;

	case 1:
		bResult	= _Get_Circle	(y, z);
		break;

	case 2:
		bResult	= _Get_Sinus	(y, z);
		break;

	case 3:
		bResult	= _Get_Hyperbel	(y, z);
		break;
	}

	//-----------------------------------------------------
	if( bResult )
	{
		p.bOk	= true;
		p.x		= (int)x;
		p.y		= (int)y;
		p.z		=      z;
	}
	else
	{
		p.bOk	= false;
	}
}

//---------------------------------------------------------
inline void CGrid_3D_Image::_Get_Rotated(double xAxis, double &x, double yAxis, double &y, double angle)
{
	double	dsin, dcos, dx, dy;

	dsin	= sin(-angle);
	dcos	= cos(-angle);

	dx		= x - xAxis;
	dy		= y - yAxis;

	x		= xAxis + dcos * dx - dsin * dy;
	y		= yAxis + dsin * dx + dcos * dy;
}


///////////////////////////////////////////////////////////
//														 //
//			now follow the projection specials			 //
//														 //
///////////////////////////////////////////////////////////

//---------------------------------------------------------
inline bool CGrid_3D_Image::_Get_Panorama(double &y, double &z)
{
	int		yOffset, yRange;
	double	angle;

	if( y < (yOffset = (int)(m_PanoramaBreak * m_pRGB->Get_NY())) )
	{
		_Get_Rotated(y, y, 0.0, z, m_XRotate);
	}
	else
	{
		_Get_Rotated(y, y, 0.0, z, m_XRotate);

		yRange	= m_pRGB->Get_NY() - yOffset;
		angle	= M_PI_090 * ((y - yOffset) / (double)yRange);

		z		= z * (m_ZExagg_Min + (1.0 - m_ZExagg_Min) * (angle > M_PI_090 ? 0.0 : 0.5 + 0.5 * cos(2.0 * angle)));

	//	if( angle < m_XRotate )
	//	{
	//		_Get_Rotated(y, y, 0.0, z, m_XRotate - angle);
	//	}

		y		= yOffset;

		_Get_Rotated(y, y, -(1.0 / M_PI_090) * yRange, z, angle);
	}

	return( true );
}

//---------------------------------------------------------
inline bool CGrid_3D_Image::_Get_Circle(double &y, double &z)
{
	double	angle;

	angle	= M_PI_090 * (y / (double)m_pRGB->Get_NY());

	z		= z * (m_ZExagg_Min + (1.0 - m_ZExagg_Min) * (angle > M_PI_090 ? 0.0 : 0.5 + 0.5 * cos(2.0 * angle)));

	y		= 0.0;

	_Get_Rotated(y, y, -(1.0 / M_PI_090) * m_pRGB->Get_NY(), z, angle);

	return( true );
}

//---------------------------------------------------------
inline bool CGrid_3D_Image::_Get_Sinus(double &y, double &z)
{
	if( y >= 0 && y < m_pRGB->Get_NY() )
	{
		return( true );
	}

	return( false );
}

//---------------------------------------------------------
inline bool CGrid_3D_Image::_Get_Hyperbel(double &y, double &z)
{
	if( y >= 0 && y < m_pRGB->Get_NY() )
	{
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
#define SORT_POINTS_Y(a, b)	if( p[a].y < p[b].y ) {	pp = p[a]; p[a] = p[b]; p[b] = pp;	}
#define SORT_POINTS_X(a, b)	if( p[a].x < p[b].x ) {	pp = p[a]; p[a] = p[b]; p[b] = pp;	}

//---------------------------------------------------------
void CGrid_3D_Image::_Draw_Triangle(T3DPoint p[3])
{
	int		i, j, k, y, y_j;
	double	x[2], dx[2], dy, z[2], dz[2], r[2], dr[2], g[2], dg[2], b[2], db[2];
	T3DPoint	pp;

	//-----------------------------------------------------
	if( !p[0].bOk || !p[1].bOk || !p[2].bOk )
	{
		return;
	}

	//-----------------------------------------------------
	SORT_POINTS_Y(1, 0);
	SORT_POINTS_Y(2, 0);
	SORT_POINTS_Y(2, 1);

	//-----------------------------------------------------
	if( p[2].y == p[0].y )
	{
		if( p[0].y >= 0 && p[0].y < m_pRGB->Get_NY() )
		{
			SORT_POINTS_X(1, 0);
			SORT_POINTS_X(2, 0);
			SORT_POINTS_X(2, 1);

			//---------------------------------------------
			if( p[2].x == p[0].x )
			{
				if(	p[0].x >= 0 && p[0].x < m_pRGB->Get_NX() )
				{
					i	= p[0].z > p[1].z ? (p[0].z > p[2].z ? 0 : 2) : (p[1].z > p[2].z ? 1 : 2);
					_Draw_Pixel(p[0].x, p[0].y, p[i].z, p[i].r, p[i].g, p[i].b);
				}
			}

			//---------------------------------------------
			else
			{
				_Draw_Line(p[0].x, p[1].x, p[0].y, p[0].z, p[1].z, p[0].r, p[1].r, p[0].g, p[1].g, p[0].b, p[1].b);
				_Draw_Line(p[1].x, p[2].x, p[0].y, p[1].z, p[2].z, p[1].r, p[2].r, p[1].g, p[2].g, p[1].b, p[2].b);
			}
		}
	}

	//-----------------------------------------------------
	else if( !((p[0].y < 0 && p[2].y < 0) || (p[0].y >= m_pRGB->Get_NY() && p[2].y >= m_pRGB->Get_NY())) )
	{
		dy		=  p[2].y - p[0].y;
		dx[0]	= (p[2].x - p[0].x) / dy;
		dz[0]	= (p[2].z - p[0].z) / dy;
		dr[0]	= (p[2].r - p[0].r) / dy;
		dg[0]	= (p[2].g - p[0].g) / dy;
		db[0]	= (p[2].b - p[0].b) / dy;
		x [0]	=  p[0].x;
		z [0]	=  p[0].z;
		r [0]	=  p[0].r;
		g [0]	=  p[0].g;
		b [0]	=  p[0].b;

		for(i=0, j=1; i<2; i++, j++)
		{
			if( (dy	=  p[j].y - p[i].y) > 0.0 )
			{
				dx[1]	= (p[j].x - p[i].x) / dy;
				dz[1]	= (p[j].z - p[i].z) / dy;
				dr[1]	= (p[j].r - p[i].r) / dy;
				dg[1]	= (p[j].g - p[i].g) / dy;
				db[1]	= (p[j].b - p[i].b) / dy;
				x [1]	=  p[i].x;
				z [1]	=  p[i].z;
				r [1]	=  p[i].r;
				g [1]	=  p[i].g;
				b [1]	=  p[i].b;

				if( (y = p[i].y) < 0 )
				{
					x[1]	-= y * dx[1];
					z[1]	-= y * dz[1];
					r[1]	-= y * dr[1];
					g[1]	-= y * dg[1];
					b[1]	-= y * db[1];
					y		 = 0;
					x[0]	 = p[0].x - p[0].y * dx[0];
					z[0]	 = p[0].z - p[0].y * dz[0];
					r[0]	 = p[0].r - p[0].y * dr[0];
					g[0]	 = p[0].g - p[0].y * dg[0];
					b[0]	 = p[0].b - p[0].y * db[0];
				}

				if( (y_j = p[j].y) > m_pRGB->Get_NY() )
				{
					y_j		= m_pRGB->Get_NY();
				}

				for( ; y<y_j; y++)
				{
					if( x[1] < x[0] )
					{
						_Draw_Line((int)x[1], (int)x[0], y, z[1], z[0], r[1], r[0], g[1], g[0], b[1], b[0]);
					}
					else
					{
						_Draw_Line((int)x[0], (int)x[1], y, z[0], z[1], r[0], r[1], g[0], g[1], b[0], b[1]);
					}

					for(k=0; k<=1; k++)
					{
						x[k]	+= dx[k];
						z[k]	+= dz[k];
						r[k]	+= dr[k];
						g[k]	+= dg[k];
						b[k]	+= db[k];
					}
				}
			}
		}
	}
}

//---------------------------------------------------------
inline void CGrid_3D_Image::_Draw_Line(int xa, int xb, int y, double za, double zb, double ra, double rb, double ga, double gb, double ba, double bb)
{
	double	d, dz, dr, dg, db;

	if( (d = xb - xa) > 0.0 )
	{
		dz	= (zb - za) / d;
		dr	= (rb - ra) / d;
		dg	= (gb - ga) / d;
		db	= (bb - ba) / d;

		if( xa < 0 )
		{
			za	-= dz * xa;
			ra	-= dr * xa;
			ga	-= dg * xa;
			ba	-= db * xa;
			xa	 = 0;
		}

		if( xb >= m_pRGB->Get_NX() )
		{
			xb	= m_pRGB->Get_NX() - 1;
		}

		for(int x=xa; x<=xb; x++, za+=dz, ra+=dr, ga+=dg, ba+=db)
		{
			_Draw_Pixel(x, y, za, (int)ra, (int)ga, (int)ba);
		}
	}
	else if( xa >= 0 && xa < m_pRGB->Get_NX() )
	{
		_Draw_Pixel(xa, y, za, (int)ra, (int)ga, (int)ba);
	}
}

//---------------------------------------------------------
inline void CGrid_3D_Image::_Draw_Pixel(int x, int y, double z, BYTE r, BYTE g, BYTE b)
{
	if(	m_pRGB->is_InGrid(x, y, false) && (m_pRGB_Z->is_NoData(x, y) || z > m_pRGB_Z->asDouble(x, y)) )
	{
		m_pRGB_Z	->Set_Value(x, y, z);
		m_pRGB		->Set_Value(x, y, SG_GET_RGB(r, g, b));
	}
}


///////////////////////////////////////////////////////////
//														 //
//														 //
//														 //
///////////////////////////////////////////////////////////

//---------------------------------------------------------

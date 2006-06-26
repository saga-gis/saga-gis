
///////////////////////////////////////////////////////////
//                                                       //
//                         SAGA                          //
//                                                       //
//      System for Automated Geoscientific Analyses      //
//                                                       //
//                    User Interface                     //
//                                                       //
//                    Program: SAGA                      //
//                                                       //
//-------------------------------------------------------//
//                                                       //
//                VIEW_Map_3D_Image.cpp                  //
//                                                       //
//          Copyright (C) 2005 by Olaf Conrad            //
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
//    contact:    Olaf Conrad                            //
//                Institute of Geography                 //
//                University of Goettingen               //
//                Goldschmidtstr. 5                      //
//                37077 Goettingen                       //
//                Germany                                //
//                                                       //
//    e-mail:     oconrad@saga-gis.org                   //
//                                                       //
///////////////////////////////////////////////////////////

//---------------------------------------------------------


///////////////////////////////////////////////////////////
//														 //
//														 //
//														 //
///////////////////////////////////////////////////////////

//---------------------------------------------------------
#include <wx/wx.h>
#include <wx/window.h>

#include "res_dialogs.h"

#include "helper.h"

#include "wksp_map.h"

#include "view_map_3d.h"
#include "view_map_3d_image.h"


///////////////////////////////////////////////////////////
//														 //
//														 //
//														 //
///////////////////////////////////////////////////////////

//---------------------------------------------------------
#define FLAG_COLOR		0x01
#define FLAG_DATA		0x02
#define FLAG_PROJECTION	0x04
#define FLAG_ALL		(FLAG_COLOR|FLAG_DATA|FLAG_PROJECTION)


///////////////////////////////////////////////////////////
//														 //
//														 //
//														 //
///////////////////////////////////////////////////////////

//---------------------------------------------------------
CVIEW_Map_3D_Image::CVIEW_Map_3D_Image(CVIEW_Map_3D *pParent, CWKSP_Map *pMap)
{
	m_pParent	= pParent;
	m_pMap		= pMap;

	m_pDEM		= NULL;

	m_img_z		= NULL;
	m_img_nx	= 0;
	m_img_ny	= 0;

	m_Points	= NULL;
	m_nxPoints	= 0;
	m_nyPoints	= 0;

	m_bInterpol	= false;

	//-----------------------------------------------------
	((BYTE *)&m_Missing)[3]	= 0xff;
	((BYTE *)&m_Missing)[2]	= 0x7f;
	((BYTE *)&m_Missing)[1]	= 0xff;
	((BYTE *)&m_Missing)[0]	= 0xff;
}

//---------------------------------------------------------
CVIEW_Map_3D_Image::~CVIEW_Map_3D_Image(void)
{
	if( m_img_z )
	{
		API_Free(m_img_z[0]);
		API_Free(m_img_z);
	}

	if( m_Points )
	{
		API_Free(m_Points[0]);
		API_Free(m_Points);
	}
}


///////////////////////////////////////////////////////////
//														 //
//														 //
//														 //
///////////////////////////////////////////////////////////


///////////////////////////////////////////////////////////
//														 //
//														 //
//														 //
///////////////////////////////////////////////////////////

//---------------------------------------------------------
void CVIEW_Map_3D_Image::Save(void)
{
	int			type;
	wxString	file;
	CParameters	Parameters;

	//-----------------------------------------------------
	Parameters.Create(NULL, LNG("Save 3D Image Options"), LNG(""));

	Parameters.Add_Node(NULL, "NODE_RES", LNG("Image Resolution"), LNG(""));

	Parameters.Add_Value(
		Parameters("NODE_RES")	, "WIDTH"	, LNG("Width"),
		"",
		PARAMETER_TYPE_Int		, m_img_nx, 1, true
	);

	Parameters.Add_Value(
		Parameters("NODE_RES")	, "HEIGHT"	, LNG("Height"),
		"",
		PARAMETER_TYPE_Int		, m_img_ny, 1, true
	);

	//-----------------------------------------------------
	if( m_img.Ok() && DLG_Image_Save(file, type) && DLG_Parameters(&Parameters) )
	{
		Set_Buisy_Cursor(true);

		if( m_pParent && (m_img_nx != Parameters("WIDTH")->asInt() || m_img_ny != Parameters("HEIGHT")->asInt()) )
		{
			CVIEW_Map_3D_Image	Image(NULL, m_pMap);

			m_pParent->_Parms_Update(true, &Image);

			Image.Set_Image(Parameters("WIDTH")->asInt(), Parameters("HEIGHT")->asInt());

			Image.Save(file, type);
		}
		else
		{
			Save(file, type);
		}

		Set_Buisy_Cursor(false);
	}
}

//---------------------------------------------------------
void CVIEW_Map_3D_Image::Save(const char *file, int type)
{
	if( m_img.Ok() && file != NULL )
	{
		m_img.SaveFile(file, type);
	}
}


///////////////////////////////////////////////////////////
//														 //
//														 //
//														 //
///////////////////////////////////////////////////////////

//---------------------------------------------------------
void CVIEW_Map_3D_Image::Set_Source(int NX, int NY)
{
	int		n;
	TPoint	*pPoint;

	if( !m_Points || NX != m_nxPoints || NY != m_nyPoints )
	{
		m_nxPoints	= NX;
		m_nyPoints	= NY;
		n			= NX * NY;
		pPoint		= m_Points ? m_Points[0] : NULL;
		pPoint		= (TPoint  *)API_Realloc( pPoint ,  n * sizeof(TPoint  ));
		m_Points	= (TPoint **)API_Realloc(m_Points, NY * sizeof(TPoint *));

		for(n=0; n<NY; n++, pPoint+=NX)
		{
			m_Points[n]	= pPoint;
		}

		m_Src_bUpdate	= true;
	}

	Set_Source();
}

//---------------------------------------------------------
void CVIEW_Map_3D_Image::Set_Source(void)
{
	BYTE		*Color, Mask[3];
	int			x, y;
	double		z, xPos, yPos, dx, dy, zMin, zMax;
	CGEO_Rect	rSource;
	wxImage		img;

	//-----------------------------------------------------
	if( m_Src_bUpdate && m_Points && m_pDEM )
	{
		m_Src_bUpdate	= false;

		if( m_bInterpol )
			img.Create(m_nxPoints    , m_nyPoints);
		else
			img.Create(m_nxPoints - 1, m_nyPoints - 1);
		m_pMap->Get_Image(img, rSource);

		m_Range	= rSource.Get_XRange() > rSource.Get_YRange() ? rSource.Get_XRange() : rSource.Get_YRange();

		//-------------------------------------------------
		dx		= rSource.Get_XRange() / (double)m_nxPoints;
		dy		= rSource.Get_YRange() / (double)m_nyPoints;
		zMin	= 1.0;
		zMax	= 0.0;

		for(y=0, yPos=rSource.Get_YMin(); y<m_nyPoints && PROGRESSBAR_Set_Position(y, m_nyPoints); y++, yPos+=dy)
		{
			for(x=0, xPos=rSource.Get_XMin(); x<m_nxPoints; x++, xPos+=dx)
			{
				if( m_pDEM->Get_Value(xPos, yPos, z) )
				{
					m_Points[y][x].zDEM		= z;
					m_Points[y][x].Flags	= FLAG_DATA;

					if( zMin > zMax )
						zMin	= zMax	= z;
					else if( z < zMin )
						zMin	= z;
					else if( z > zMax )
						zMax	= z;
				}
				else
				{
					m_Points[y][x].Flags	= 0;
				}
			}
		}

		//-------------------------------------------------
		z		= zMin + (zMax - zMin) / 2.0;

		for(y=0; y<m_nyPoints && PROGRESSBAR_Set_Position(y, m_nyPoints); y++)
		{
			for(x=0; x<m_nxPoints; x++)
			{
				if( (m_Points[y][x].Flags & FLAG_DATA) != 0 )
				{
					m_Points[y][x].zDEM	-= z;
				}
			}
		}

		//-----------------------------------------------------
		Mask[0]	= img.GetMaskRed();
		Mask[1]	= img.GetMaskGreen();
		Mask[2]	= img.GetMaskBlue();

		for(y=0; y<img.GetHeight(); y++)
		{
			Color	= img.GetData() + (img.GetHeight() - 1 - y) * 3 * img.GetWidth();

			for(x=0; x<img.GetWidth(); x++, Color+=3)
			{
				if( memcmp(Color, Mask, 3 * sizeof(BYTE)) )
				{
					m_Points[y][x].r		 = Color[0];
					m_Points[y][x].g		 = Color[1];
					m_Points[y][x].b		 = Color[2];
					m_Points[y][x].Flags	|= FLAG_COLOR;
				}
			}
		}

		//-------------------------------------------------
		PROCESS_Set_Okay(true);

		Set_Image();
	}
}


///////////////////////////////////////////////////////////
//														 //
//														 //
//														 //
///////////////////////////////////////////////////////////

//---------------------------------------------------------
void CVIEW_Map_3D_Image::Set_Image(int NX, int NY)
{
	int		y;
	float	*pz;

	if( !m_img_z || NX != m_img_nx || NY != m_img_ny )
	{
		m_img.Create(NX, NY);
		m_img_nx	= m_img.GetWidth();
		m_img_ny	= m_img.GetHeight();

		pz		= m_img_z ? m_img_z[0] : NULL;
		pz		= (float  *)API_Realloc(pz     , NY * NX * sizeof(float  ));
		m_img_z	= (float **)API_Realloc(m_img_z, NY      * sizeof(float *));

		for(y=0; y<NY; y++, pz+=NX)
		{
			m_img_z[y]	= pz;
		}

		Set_Image();
	}
}

//---------------------------------------------------------
void CVIEW_Map_3D_Image::Set_Image(void)
{
	BYTE	BkStereo, *r, *p;
	int		i, n;

	//-----------------------------------------------------
	if( m_Points && m_img_z )
	{
		n	= m_img_nx * m_img_ny;

		if( !m_bStereo )
		{
			for(i=0, p=m_img.GetData(); i<n; i++, p+=3)
			{
				memcpy(p, &m_BkColor, 3);
			}

			_Rotate_Matrix(m_xRotate, m_yRotate, m_zRotate);
			_Draw_Image();
		}

		//-------------------------------------------------
		else
		{
			BkStereo	= (BYTE)((COLOR_GET_R(m_BkColor) + COLOR_GET_G(m_BkColor) + COLOR_GET_B(m_BkColor)) / 3.0);

			//---------------------------------------------
			// 1. Right View...
			memset(m_img.GetData(), BkStereo, 3 * n);

			_Rotate_Matrix(m_xRotate, m_yRotate + 0.5 * m_Stereo, m_zRotate);
			_Draw_Image();

			r			= (BYTE *)API_Malloc(n * sizeof(BYTE));

			for(i=0, p=m_img.GetData(); i<n; i++, p+=3)
			{
				r[i]	= (p[0] + p[1] + p[2]) / 3;
			}

			//---------------------------------------------
			// 2. Left View...
			memset(m_img.GetData(), BkStereo, 3 * n);

			_Rotate_Matrix(m_xRotate, m_yRotate - 0.5 * m_Stereo, m_zRotate);
			_Draw_Image();

			for(i=0, p=m_img.GetData(); i<n; i++, p+=3)
			{
				p[0]	= (p[0] + p[1] + p[2]) / 3;
				p[1]	= r[i];
				p[2]	= BkStereo;
			}

			API_Free(r);
		}

		if( m_pParent )
		{
			m_pParent->_Paint();
		}
	}
}


///////////////////////////////////////////////////////////
//														 //
//														 //
//														 //
///////////////////////////////////////////////////////////

//---------------------------------------------------------
#define RANGE	200.0

//---------------------------------------------------------
void CVIEW_Map_3D_Image::_Rotate_Matrix(double xRotate, double yRotate, double zRotate)
{
	int		x, y;
	double	px, py, pz, ix, iy, dx, dy, dz;
	TPoint	*p;

	//-----------------------------------------------------
	r_fig	= sqrt(2.0) * m_Figure_Weight * RANGE;

	dx		=  RANGE / m_nxPoints;
	dy		=  RANGE / m_nyPoints;
	dz		= -RANGE * m_Exaggeration / m_Range;

	r_sin_x	= sin(xRotate);
	r_sin_y	= sin(yRotate);
	r_sin_z	= sin(zRotate);

	r_cos_x	= cos(xRotate);
	r_cos_y	= cos(yRotate);
	r_cos_z	= cos(zRotate);

	r_ext	= RANGE;

	if( m_img_ny > m_img_nx )
	{
		r_m		= m_img_nx / r_ext;
		r_kx	= 0.0f;
		r_ky	= m_img_ny - (m_img_ny - r_m * r_ext) / 2.0;
	}
	else
	{
		r_m		= m_img_ny / r_ext;
		r_kx	= (m_img_nx - r_m * r_ext) / 2.0;
		r_ky	= m_img_ny;
	}

	r_ext	= -r_ext / 2.0;

	//-----------------------------------------------------
	for(y=0, iy=-RANGE/2.0; y<m_nyPoints; y++, iy+=dy)
	{
		for(x=0, ix=-RANGE/2.0, p=m_Points[y]; x<m_nxPoints; x++, ix+=dx, p++)
		{
			if( (p->Flags & FLAG_DATA) != 0 && _Rotate_Point(ix, iy, dz * p->zDEM, px, py, pz) )
			{
				p->x		= (int)px;
				p->y		= (int)py;
				p->z		=      pz;
				p->Flags	|=  FLAG_PROJECTION;
			}
			else
			{
				p->Flags	&= ~FLAG_PROJECTION;
			}
		}
	}
}

//---------------------------------------------------------
inline bool CVIEW_Map_3D_Image::_Rotate_Point(double x, double y, double z, double &px, double &py, double &pz)
{
	//-----------------------------------------------------
	px	= r_cos_z * x - r_sin_z *  y;
	py	= r_sin_z * x + r_cos_z *  y;

	switch( m_Figure )
	{
	default:
		break;

	case 1:	// cylinder around x-axis
		x	 = r_fig*r_fig - py*py;
		z	+= r_fig - (x > 0.0 ? sqrt(x) : 0.0);
		break;

	case 2:	// ball
		x	 = r_fig*r_fig - py*py - px*px;
		z	+= r_fig - (x > 0.0 ? sqrt(x) : 0.0);
		break;

	case 3:	// panaroma
		x	 = m_Figure_Weight * 20.0 * (0.5 * RANGE - py) / RANGE;
		z	+= x*x;
		break;
	}

	pz	= r_cos_x * z - r_sin_x * py;
	py	= r_sin_x * z + r_cos_x * py;

	z	= pz;
	pz	= r_cos_y * z - r_sin_y * px;
	px	= r_sin_y * z + r_cos_y * px;

	px	+= m_xShift;
	py	+= m_yShift;
	pz	+= m_zShift;

	//-----------------------------------------------------
	if( m_bCentral )
	{
		if( pz > 0.0 )
		{
			pz	 = m_Central / pz;
			px	*= pz;
			py	*= pz;
		}
		else
		{
			return( false );
		}
	}
	else
	{
		if( m_zShift > 0.0 )
		{
			z	 = m_Central / m_zShift;
			px	*= z;
			py	*= z;
			pz	 = -pz;
		}
		else
		{
			return( false );
		}
	}

	//-----------------------------------------------------
	px	= r_kx + r_m * (px - r_ext);
	py	= r_ky - r_m * (py - r_ext);

	return( true );
}


///////////////////////////////////////////////////////////
//														 //
//														 //
//														 //
///////////////////////////////////////////////////////////

//---------------------------------------------------------
void CVIEW_Map_3D_Image::_Draw_Image(void)
{
	int		x, y;
	TPoint	p[3];

	//-----------------------------------------------------
	for(y=0; y<m_img_ny; y++)
	{
		for(x=0; x<m_img_nx; x++)
		{
			m_img_z[y][x]	= m_Missing;
		}
	}

	//-----------------------------------------------------
	for(y=0; y<m_nyPoints-1; y++)
	{
		for(x=0; x<m_nxPoints-1; x++)
		{
			p[0]	= m_Points[y    ][x    ];
			p[1]	= m_Points[y + 1][x + 1];
			p[2]	= m_Points[y    ][x + 1];

			if( m_bInterpol )
				_Draw_Triangle_i(p);
			else
				_Draw_Triangle  (p);

			//---------------------------------------------
			p[0]	= m_Points[y    ][x    ];
			p[1]	= m_Points[y + 1][x + 1];
			p[2]	= m_Points[y + 1][x    ];

			if( m_bInterpol )
				_Draw_Triangle_i(p);
			else
				_Draw_Triangle  (p);
		}
	}
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
#define _Draw_Pixel(x, y, z, r, g, b)	if(	z > m_img_z[y][x] ) { m_img_z[y][x] = z; m_img.SetRGB(x, y, r, g, b); }


///////////////////////////////////////////////////////////
//														 //
//														 //
//														 //
///////////////////////////////////////////////////////////

//---------------------------------------------------------
void CVIEW_Map_3D_Image::_Draw_Triangle(TPoint p[3])
{
	BYTE	r, g, b;
	int		i, j, k, y, y_j;
	double	dy, x[2], dx[2], z[2], dz[2];
	TPoint	pp;

	//-----------------------------------------------------
	if(	(p[0].Flags & FLAG_COLOR)      == 0
	||	(p[0].Flags & FLAG_PROJECTION) == 0
	||	(p[1].Flags & FLAG_PROJECTION) == 0
	||	(p[2].Flags & FLAG_PROJECTION) == 0 )
	{
		return;
	}

	//-----------------------------------------------------
	r	= p[0].r;
	g	= p[0].g;
	b	= p[0].b;

	SORT_POINTS_Y(1, 0);
	SORT_POINTS_Y(2, 0);
	SORT_POINTS_Y(2, 1);

	//-----------------------------------------------------
	if( p[2].y == p[0].y )
	{
		if( p[0].y >= 0 && p[0].y < m_img_ny )
		{
			SORT_POINTS_X(1, 0);
			SORT_POINTS_X(2, 0);
			SORT_POINTS_X(2, 1);

			//---------------------------------------------
			if( p[2].x == p[0].x )
			{
				if(	p[0].x >= 0 && p[0].x < m_img_nx )
				{
					i	= p[0].z > p[1].z ? (p[0].z > p[2].z ? 0 : 2) : (p[1].z > p[2].z ? 1 : 2);
					_Draw_Pixel(p[0].x, p[0].y, p[i].z, r, g, b);
				}
			}

			//---------------------------------------------
			else
			{
				_Draw_Line(p[0].x, p[1].x, p[0].y, p[0].z, p[1].z, r, g, b);
				_Draw_Line(p[1].x, p[2].x, p[0].y, p[1].z, p[2].z, r, g, b);
			}
		}
	}

	//-----------------------------------------------------
	else if( !((p[0].y < 0 && p[2].y < 0) || (p[0].y >= m_img_ny && p[2].y >= m_img_ny)) )
	{
		dy		=  p[2].y - p[0].y;
		dx[0]	= (p[2].x - p[0].x) / dy;
		dz[0]	= (p[2].z - p[0].z) / dy;
		x [0]	=  p[0].x;
		z [0]	=  p[0].z;

		for(i=0, j=1; i<2; i++, j++)
		{
			if( (dy	=  p[j].y - p[i].y) > 0.0 )
			{
				dx[1]	= (p[j].x - p[i].x) / dy;
				dz[1]	= (p[j].z - p[i].z) / dy;
				x [1]	=  p[i].x;
				z [1]	=  p[i].z;

				if( (y = p[i].y) < 0 )
				{
					x[1]	-= y * dx[1];
					z[1]	-= y * dz[1];
					y		 = 0;
					x[0]	 = p[0].x - p[0].y * dx[0];
					z[0]	 = p[0].z - p[0].y * dz[0];
				}

				if( (y_j = p[j].y) > m_img_ny )
				{
					y_j		= m_img_ny;
				}

				for( ; y<y_j; y++)
				{
					if( x[1] < x[0] )
					{
						_Draw_Line((int)x[1], (int)x[0], y, z[1], z[0], r, g, b);
					}
					else
					{
						_Draw_Line((int)x[0], (int)x[1], y, z[0], z[1], r, g, b);
					}

					for(k=0; k<=1; k++)
					{
						x[k]	+= dx[k];
						z[k]	+= dz[k];
					}
				}
			}
		}
	}
}

//---------------------------------------------------------
inline void CVIEW_Map_3D_Image::_Draw_Line(int xa, int xb, int y, double za, double zb, BYTE r, BYTE g, BYTE b)
{
	double	dz;

	if( (dz = xb - xa) > 0.0 )
	{
		dz	= (zb - za) / dz;

		if( xa < 0 )
		{
			za	-= dz * xa;
			xa	 = 0;
		}

		if( xb >= m_img_nx )
		{
			xb	= m_img_nx - 1;
		}

		for(int x=xa; x<=xb; x++, za+=dz)
		{
			_Draw_Pixel(x, y, za, r, g, b);
		}
	}
	else if( xa >= 0 && xa < m_img_nx )
	{
		_Draw_Pixel(xa, y, za, r, g, b);
	}
}


///////////////////////////////////////////////////////////
//														 //
//														 //
//														 //
///////////////////////////////////////////////////////////

//---------------------------------------------------------
void CVIEW_Map_3D_Image::_Draw_Triangle_i(TPoint p[3])
{
	int		i, j, k, y, y_j;
	double	x[2], dx[2], dy, z[2], dz[2], r[2], dr[2], g[2], dg[2], b[2], db[2];
	TPoint	pp;

	//-----------------------------------------------------
	i	= FLAG_ALL;
	if(	(p[0].Flags != i) || (p[1].Flags != i) || (p[2].Flags != i) )
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
		if( p[0].y >= 0 && p[0].y < m_img_ny )
		{
			SORT_POINTS_X(1, 0);
			SORT_POINTS_X(2, 0);
			SORT_POINTS_X(2, 1);

			//---------------------------------------------
			if( p[2].x == p[0].x )
			{
				if(	p[0].x >= 0 && p[0].x < m_img_nx )
				{
					i	= p[0].z > p[1].z ? (p[0].z > p[2].z ? 0 : 2) : (p[1].z > p[2].z ? 1 : 2);
					_Draw_Pixel(p[0].x, p[0].y, p[i].z, p[i].r, p[i].g, p[i].b);
				}
			}

			//---------------------------------------------
			else
			{
				_Draw_Line_i(p[0].x, p[1].x, p[0].y, p[0].z, p[1].z, p[0].r, p[1].r, p[0].g, p[1].g, p[0].b, p[1].b);
				_Draw_Line_i(p[1].x, p[2].x, p[0].y, p[1].z, p[2].z, p[1].r, p[2].r, p[1].g, p[2].g, p[1].b, p[2].b);
			}
		}
	}

	//-----------------------------------------------------
	else if( !((p[0].y < 0 && p[2].y < 0) || (p[0].y >= m_img_ny && p[2].y >= m_img_ny)) )
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

				if( (y_j = p[j].y) > m_img_ny )
				{
					y_j		= m_img_ny;
				}

				for( ; y<y_j; y++)
				{
					if( x[1] < x[0] )
					{
						_Draw_Line_i((int)x[1], (int)x[0], y, z[1], z[0], r[1], r[0], g[1], g[0], b[1], b[0]);
					}
					else
					{
						_Draw_Line_i((int)x[0], (int)x[1], y, z[0], z[1], r[0], r[1], g[0], g[1], b[0], b[1]);
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
inline void CVIEW_Map_3D_Image::_Draw_Line_i(int xa, int xb, int y, double za, double zb, double ra, double rb, double ga, double gb, double ba, double bb)
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

		if( xb >= m_img_nx )
		{
			xb	= m_img_nx - 1;
		}

		for(int x=xa; x<=xb; x++, za+=dz, ra+=dr, ga+=dg, ba+=db)
		{
			_Draw_Pixel(x, y, za, (int)ra, (int)ga, (int)ba);
		}
	}
	else if( xa >= 0 && xa < m_img_nx )
	{
		_Draw_Pixel(xa, y, za, (int)ra, (int)ga, (int)ba);
	}
}


///////////////////////////////////////////////////////////
//														 //
//														 //
//														 //
///////////////////////////////////////////////////////////

//---------------------------------------------------------

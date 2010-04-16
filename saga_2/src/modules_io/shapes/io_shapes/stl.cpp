
///////////////////////////////////////////////////////////
//                                                       //
//                         SAGA                          //
//                                                       //
//      System for Automated Geoscientific Analyses      //
//                                                       //
//                    Module Library:                    //
//                       io_shapes                       //
//                                                       //
//-------------------------------------------------------//
//                                                       //
//					      stl.cpp                        //
//                                                       //
//                 Copyright (C) 2008 by                 //
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
#include "stl.h"


///////////////////////////////////////////////////////////
//														 //
//														 //
//														 //
///////////////////////////////////////////////////////////

//---------------------------------------------------------
CSTL_Import::CSTL_Import(void)
{
	CSG_Parameter	*pNode;

	//-----------------------------------------------------
	Set_Name		(_TL("Import Stereo Lithography File (STL)"));

	Set_Author		(_TL("O. Conrad (c) 2008"));

	Set_Description	(_TW(
		"<a href=\"http://www.ennex.com/~fabbers/StL.asp\">The StL Format</a>"
	));

	//-----------------------------------------------------
	Parameters.Add_PointCloud_Output(
		NULL	, "POINTS"		, _TL("Point Cloud"),
		_TL("")
	);

	Parameters.Add_Shapes_Output(
		NULL	, "SHAPES"		, _TL("Shapes"),
		_TL("")
	);

	Parameters.Add_Grid_Output(
		NULL	, "GRID"		, _TL("Grid"),
		_TL("")
	);

	Parameters.Add_FilePath(
		NULL	, "FILE"		, _TL("File"),
		_TL(""),
		CSG_String::Format(SG_T("%s|%s|%s|%s"),
			_TL("STL Files")	, SG_T("*.stl"),
			_TL("All Files")	, SG_T("*.*")
		)
	);

	Parameters.Add_Choice(
		NULL	, "METHOD"		, _TL("Target"),
		_TL(""),
		CSG_String::Format(SG_T("%s|%s|%s|%s|"),
			_TL("point cloud"),
			_TL("point cloud (centered)"),
			_TL("points"),
			_TL("raster")
		), 3
	);

	Parameters.Add_Value(
		NULL	, "GRID_RES"	, _TL("Raster Resolution (Pixels X)"),
		_TL(""),
		PARAMETER_TYPE_Int		, 2000, 10, true
	);

	pNode	= Parameters.Add_Node(
		NULL	, "NODE_ROTATE"	, _TL("Rotation"),
		_TL("")
	);

	Parameters.Add_Value(
		pNode	, "ROT_X"		, _TL("X Axis"),
		_TL(""),
		PARAMETER_TYPE_Double	, 0.0
	);

	Parameters.Add_Value(
		pNode	, "ROT_Y"		, _TL("Y Axis"),
		_TL(""),
		PARAMETER_TYPE_Double	, 0.0
	);

	Parameters.Add_Value(
		pNode	, "ROT_Z"		, _TL("Z Axis"),
		_TL(""),
		PARAMETER_TYPE_Double	, 0.0
	);
}


///////////////////////////////////////////////////////////
//														 //
///////////////////////////////////////////////////////////

//---------------------------------------------------------
bool CSTL_Import::On_Execute(void)
{
	int			Method;
	DWORD		iFacette, nFacettes;
	TSTL_Point	p[3];
	CSG_String	sFile, sHeader;
	CSG_File	Stream;

	//-----------------------------------------------------
	sFile		= Parameters("FILE")		->asString();
	Method		= Parameters("METHOD")		->asInt();

	r_sin_x	= sin(Parameters("ROT_X")->asDouble() * M_DEG_TO_RAD);
	r_sin_y	= sin(Parameters("ROT_Y")->asDouble() * M_DEG_TO_RAD);
	r_sin_z	= sin(Parameters("ROT_Z")->asDouble() * M_DEG_TO_RAD);
	r_cos_x	= cos(Parameters("ROT_X")->asDouble() * M_DEG_TO_RAD);
	r_cos_y	= cos(Parameters("ROT_Y")->asDouble() * M_DEG_TO_RAD);
	r_cos_z	= cos(Parameters("ROT_Z")->asDouble() * M_DEG_TO_RAD);

	//-----------------------------------------------------
	if( !Stream.Open(sFile) )
	{
		return( false );
	}

	if( !Stream.Read(sHeader, 80) )
	{
		return( false );
	}

	Message_Add(sHeader);

	if( !Stream.Read(&nFacettes, sizeof(nFacettes)) )
	{
		return( false );
	}

	Message_Add(CSG_String::Format(SG_T("%s: %d"), _TL("Number of Facettes"), nFacettes));

	//-----------------------------------------------------
	switch( Method )
	{

	//-----------------------------------------------------
	case 0:	{	// Point Cloud
		CSG_Rect	Extent;

		if( Get_Extent(Stream, Extent, nFacettes) )
		{
			CSG_PRQuadTree	Points(Extent);
			CSG_PointCloud	*pPoints	= SG_Create_PointCloud();
			Parameters("POINTS")->Set_Value(pPoints);
			pPoints->Set_Name(SG_File_Get_Name(sFile, false));
			pPoints->Add_Field((const char *)NULL, SG_DATATYPE_Undefined);

			for(iFacette=0; iFacette<nFacettes && !Stream.is_EOF() && Set_Progress(iFacette, nFacettes); iFacette++)
			{
				if( Read_Facette(Stream, p) )
				{
					for(int i=0; i<3; i++)
					{
						if( Points.Add_Point(p[i].x, p[i].y, p[i].z) )
						{
							pPoints->Add_Point(p[i].x, p[i].y, p[i].z);
						}
					}
				}
			}
		}

	break;	}

	//-----------------------------------------------------
	case 1:	{	// Point Cloud (centered)
		CSG_PointCloud	*pPoints	= SG_Create_PointCloud();
		Parameters("POINTS")->Set_Value(pPoints);
		pPoints->Set_Name(SG_File_Get_Name(sFile, false));
		pPoints->Add_Field((const char *)NULL, SG_DATATYPE_Undefined);

		for(iFacette=0; iFacette<nFacettes && !Stream.is_EOF() && Set_Progress(iFacette, nFacettes); iFacette++)
		{
			if( Read_Facette(Stream, p) )
			{
				pPoints->Add_Point(
					(p[0].x + p[1].x + p[2].x) / 3.0,
					(p[0].y + p[1].y + p[2].y) / 3.0,
					(p[0].z + p[1].z + p[2].z) / 3.0
				);
			}
		}

	break;	}

	//-----------------------------------------------------
	case 2:	{	// Points
		CSG_Shapes	*pPoints	= SG_Create_Shapes(SHAPE_TYPE_Point, SG_File_Get_Name(sFile, false));
		pPoints->Add_Field(SG_T("Z"), SG_DATATYPE_Float);
		Parameters("SHAPES")->Set_Value(pPoints);

		for(iFacette=0; iFacette<nFacettes && !Stream.is_EOF() && Set_Progress(iFacette, nFacettes); iFacette++)
		{
			if( Read_Facette(Stream, p) )
			{
				CSG_Shape	*pPoint	= pPoints->Add_Shape();

				pPoint->Add_Point(
					(p[0].x + p[1].x + p[2].x) / 3.0,
					(p[0].y + p[1].y + p[2].y) / 3.0
				);

				pPoint->Set_Value(0,
					(p[0].z + p[1].z + p[2].z) / 3.0
				);
			}
		}

	break;	}

	//-----------------------------------------------------
	case 3:	{	// Raster
		CSG_Rect	Extent;

		if( Get_Extent(Stream, Extent, nFacettes) )
		{
			int		nx, ny;
			double	d;

			nx		= Parameters("GRID_RES")->asInt();
			d		= Extent.Get_XRange() / nx;
			ny		= 1 + (int)(Extent.Get_YRange() / d);

			m_pGrid	= SG_Create_Grid(SG_DATATYPE_Float, nx, ny, d, Extent.Get_XMin(), Extent.Get_YMin());
			m_pGrid->Set_Name(SG_File_Get_Name(sFile, false));
			m_pGrid->Set_NoData_Value(-99999);
			m_pGrid->Assign_NoData();

			Parameters("GRID")->Set_Value(m_pGrid);

			//---------------------------------------------
			for(iFacette=0; iFacette<nFacettes && !Stream.is_EOF() && Set_Progress(iFacette, nFacettes); iFacette++)
			{
				if( Read_Facette(Stream, p) )
				{
					TGRD_Point	Point[3];

					for(int i=0; i<3; i++)
					{
						Point[i].x	= m_pGrid->Get_System().Get_xWorld_to_Grid(p[i].x);
						Point[i].y	= m_pGrid->Get_System().Get_yWorld_to_Grid(p[i].y);
						Point[i].z	= p[i].z;
					}

					Set_Triangle(Point);
				}
			}
		}

	break;	}
	}

	//-----------------------------------------------------
	return( true );
}


///////////////////////////////////////////////////////////
//														 //
///////////////////////////////////////////////////////////

//---------------------------------------------------------
inline bool CSTL_Import::Read_Facette(CSG_File &Stream, TSTL_Point p[3])
{
	WORD	Attribute;

	if( Stream.Read(p + 0, sizeof(TSTL_Point))
	&&	Stream.Read(p + 0, sizeof(TSTL_Point))
	&&	Stream.Read(p + 1, sizeof(TSTL_Point))
	&&	Stream.Read(p + 2, sizeof(TSTL_Point))
	&&	Stream.Read(&Attribute, sizeof(Attribute)) )
	{
		Rotate(p[0]);
		Rotate(p[1]);
		Rotate(p[2]);

		return( true );
	}

	return( false );
}

//---------------------------------------------------------
inline void CSTL_Import::Rotate(TSTL_Point &p)
{
	float	d;

	d	= (float)(r_cos_z * p.x - r_sin_z * p.y);
	p.y	= (float)(r_sin_z * p.x + r_cos_z * p.y);
	p.x	= d;

	d	= (float)(r_cos_y * p.z - r_sin_y * p.x);
	p.x	= (float)(r_sin_y * p.z + r_cos_y * p.x);
	p.z	= d;

	d	= (float)(r_cos_x * p.z - r_sin_x * p.y);
	p.y	= (float)(r_sin_x * p.z + r_cos_x * p.y);
	p.z	= d;
}


///////////////////////////////////////////////////////////
//														 //
///////////////////////////////////////////////////////////

//---------------------------------------------------------
bool CSTL_Import::Get_Extent(CSG_File &Stream, CSG_Rect &Extent, int nFacettes)
{
	float	xMin = 1, xMax = 0, yMin, yMax;

	for(int iFacette=0; iFacette<nFacettes && !Stream.is_EOF() && Set_Progress(iFacette, nFacettes); iFacette++)
	{
		TSTL_Point	p[3];

		if( Read_Facette(Stream, p) )
		{
			if( iFacette == 0 )
			{
				xMin	= xMax	= p[0].x;
				yMin	= yMax	= p[0].y;
			}

			for(int i=0; i<3; i++)
			{
				if( xMin > p[i].x )	{	xMin	= p[i].x;	}	else if( xMax < p[i].x )	{	xMax	= p[i].x;	}
				if( yMin > p[i].y )	{	yMin	= p[i].y;	}	else if( yMax < p[i].y )	{	yMax	= p[i].y;	}
			}
		}
	}

	Extent.Assign(xMin, yMin, xMax, yMax);

	return( xMin < xMax && yMin < yMax && Stream.Seek(80 + sizeof(nFacettes)) );
}


///////////////////////////////////////////////////////////
//														 //
///////////////////////////////////////////////////////////

//---------------------------------------------------------
#define SORT_POINTS_Y(a, b)	if( p[a].y < p[b].y ) {	pp = p[a]; p[a] = p[b]; p[b] = pp;	}
#define SORT_POINTS_X(a, b)	if( p[a].x < p[b].x ) {	pp = p[a]; p[a] = p[b]; p[b] = pp;	}

//---------------------------------------------------------
void CSTL_Import::Set_Triangle(TGRD_Point p[3])
{
	int		i, j, y, y_j;
	double	x, x_a, dx, dx_a, dy, z, z_a, dz, dz_a;
	TGRD_Point	pp;

	//-----------------------------------------------------
	SORT_POINTS_Y(1, 0);
	SORT_POINTS_Y(2, 0);
	SORT_POINTS_Y(2, 1);

	//-----------------------------------------------------
	if( p[2].y == p[0].y )
	{
		if( p[0].y >= 0 && p[0].y < m_pGrid->Get_NY() )
		{
			SORT_POINTS_X(1, 0);
			SORT_POINTS_X(2, 0);
			SORT_POINTS_X(2, 1);

			//---------------------------------------------
			if( p[2].x == p[0].x )
			{
				if(	p[0].x >= 0 && p[0].x < m_pGrid->Get_NX() )
				{
					Set_Triangle_Point(p[0].x, p[0].y, p[0].z > p[1].z
						? (p[0].z > p[2].z ? p[0].z : p[2].z)
						: (p[1].z > p[2].z ? p[1].z : p[2].z)
					);
				}
			}

			//---------------------------------------------
			else
			{
				Set_Triangle_Line(p[0].x, p[1].x, p[0].y, p[0].z, p[1].z);
				Set_Triangle_Line(p[1].x, p[2].x, p[0].y, p[1].z, p[2].z);
			}
		}
	}

	//-----------------------------------------------------
	else if( !((p[0].y < 0 && p[2].y < 0) || (p[0].y >= m_pGrid->Get_NY() && p[2].y >= m_pGrid->Get_NY())) )
	{
		dy		=  p[2].y - p[0].y;
		dx_a	= (p[2].x - p[0].x) / dy;
		dz_a	= (p[2].z - p[0].z) / dy;
		x_a		=  p[0].x;
		z_a		=  p[0].z;

		for(i=0, j=1; i<2; i++, j++)
		{
			if( (dy	=  p[j].y - p[i].y) > 0.0 )
			{
				dx		= (p[j].x - p[i].x) / dy;
				dz		= (p[j].z - p[i].z) / dy;
				x		=  p[i].x;
				z		=  p[i].z;

				if( (y = p[i].y) < 0 )
				{
					x		-= y * dx;
					z		-= y * dz;
					y		 = 0;
					x_a		 = p[0].x - p[0].y * dx_a;
					z_a		 = p[0].z - p[0].y * dz_a;
				}

				if( (y_j = p[j].y) > m_pGrid->Get_NY() )
				{
					y_j		= m_pGrid->Get_NY();
				}

				for( ; y<y_j; y++, x+=dx, z+=dz, x_a+=dx_a, z_a+=dz_a)
				{
					if( x < x_a )
					{
						Set_Triangle_Line((int)x, (int)x_a, y, z, z_a);
					}
					else
					{
						Set_Triangle_Line((int)x_a, (int)x, y, z_a, z);
					}
				}
			}
		}
	}
}

//---------------------------------------------------------
inline void CSTL_Import::Set_Triangle_Line(int xa, int xb, int y, double za, double zb)
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

		if( xb >= m_pGrid->Get_NX() )
		{
			xb	= m_pGrid->Get_NX() - 1;
		}

		for(int x=xa; x<=xb; x++, za+=dz)
		{
			Set_Triangle_Point(x, y, za);
		}
	}
	else if( xa >= 0 && xa < m_pGrid->Get_NX() )
	{
		Set_Triangle_Point(xa, y, za);
	}
}

//---------------------------------------------------------
inline void CSTL_Import::Set_Triangle_Point(int x, int y, double z)
{
	if( m_pGrid->is_NoData(x, y) || m_pGrid->asDouble(x, y) < z )
	{
		m_pGrid->Set_Value(x, y, z);
	}
}


///////////////////////////////////////////////////////////
//														 //
//														 //
//														 //
///////////////////////////////////////////////////////////

//---------------------------------------------------------
CSTL_Export::CSTL_Export(void)
{
	CSG_Parameter	*pNode;

	//-----------------------------------------------------
	Set_Name		(_TL("Export TIN to Stereo Lithography File (STL)"));

	Set_Author		(_TL("Navaladi, Schoeller, Conrad (c) 2009"));

	Set_Description	(_TW(
		"<a href=\"http://www.ennex.com/~fabbers/StL.asp\">The StL Format</a>"
	));

	//-----------------------------------------------------
	pNode	= Parameters.Add_TIN(
		NULL	, "TIN"			, _TL("TIN"),
		_TL(""),
		PARAMETER_INPUT
	);

	Parameters.Add_Table_Field(
		pNode	, "ZFIELD"		, _TL("Z Attribute"),
		_TL("")
	);

	Parameters.Add_FilePath(
		NULL	, "FILE"		, _TL("File"),
		_TL(""),
		CSG_String::Format(SG_T("%s|%s|%s|%s"),
			_TL("STL Files")	, SG_T("*.stl"),
			_TL("All Files")	, SG_T("*.*")
		), NULL, true
	);

	Parameters.Add_Choice(
		pNode	, "BINARY"		, _TL("Output Type"),
		_TL(""),
		CSG_String::Format(SG_T("%s|%s|"),
			_TL("ASCII"),
			_TL("binary")
		), 1
	);
}

//---------------------------------------------------------
bool CSTL_Export::On_Execute(void)
{
	bool		bBinary;
	int			zField;
	float		v[3];
	CSG_String	File;
	CSG_File	Stream;
	CSG_TIN		*pTIN;

	pTIN	= Parameters("TIN")		->asTIN();
	File	= Parameters("FILE")	->asString();
	zField	= Parameters("ZFIELD")	->asInt(); 
	bBinary	= Parameters("BINARY")	->asInt() == 1; 

	if( !Stream.Open(File, SG_FILE_W, bBinary) )
	{
		return( false );
	}

	//-----------------------------------------------------
	if( bBinary )
	{
		char	*sHeader	= (char *)SG_Calloc(80, sizeof(char));
		DWORD	nFacets		= pTIN->Get_Triangle_Count();
		WORD	nBytes		= 0;

		Stream.Write(sHeader , sizeof(char), 80);
		Stream.Write(&nFacets, sizeof(DWORD));

		SG_Free(sHeader);

		//-------------------------------------------------
		for(int iTriangle=0; iTriangle<pTIN->Get_Triangle_Count(); iTriangle++)
		{
			CSG_TIN_Triangle	*pTriangle	= pTIN->Get_Triangle(iTriangle);

			Get_Normal(pTriangle, zField, v);

			Stream.Write(v, sizeof(float), 3);	// facet normal

			for(int iNode=0; iNode<3; iNode++)
			{
				CSG_TIN_Node	*pNode	= pTriangle->Get_Node(iNode);

				v[0]	= (float)pNode->Get_X();
				v[1]	= (float)pNode->Get_Y();
				v[2]	= (float)pNode->asDouble(zField);

				Stream.Write(v, sizeof(float), 3);
			}

			Stream.Write(&nBytes, sizeof(WORD));
		}
	}

	//-----------------------------------------------------
	else	// ASCII
	{
		Stream.Printf(SG_T("solid %s\n"), SG_File_Get_Name(File, false).c_str());

		for(int iTriangle=0; iTriangle<pTIN->Get_Triangle_Count(); iTriangle++)
		{
			CSG_TIN_Triangle	*pTriangle	= pTIN->Get_Triangle(iTriangle);

			Get_Normal(pTriangle, zField, v);

			Stream.Printf(SG_T(" facet normal %.4f %.4f %.4f\n"), v[0], v[1], v[2]);
			Stream.Printf(SG_T("  outer loop\n"));

			for(int iNode=0; iNode<3; iNode++)
			{
				CSG_TIN_Node	*pNode	= pTriangle->Get_Node(iNode);

				v[0]	= (float)pNode->Get_X();
				v[1]	= (float)pNode->Get_Y();
				v[2]	= (float)pNode->asDouble(zField);

				Stream.Printf(SG_T("   vertex %.4f %.4f %.4f\n"), v[0], v[1], v[2]);
			}

			Stream.Printf(SG_T("  endloop\n"));
			Stream.Printf(SG_T(" endfacet\n"));		
		}

		Stream.Printf(SG_T("endsolid %s\n"), SG_File_Get_Name(File, false).c_str());
	}

	return( true );
}

//---------------------------------------------------------
inline bool CSTL_Export::Get_Normal(CSG_TIN_Triangle *pTriangle, int zField, float Normal[3])
{
	double			u[3], v[3];
	CSG_TIN_Node	*pA, *pB;

	pA			= pTriangle->Get_Node(0);

	pB			= pTriangle->Get_Node(1);
	u[0]		= pB->Get_X()          - pA->Get_X();
	u[1]		= pB->Get_Y()          - pA->Get_Y();
	u[2]		= pB->asDouble(zField) - pA->asDouble(zField);

	pB			= pTriangle->Get_Node(2);
	v[0]		= pB->Get_X()          - pA->Get_X();
	v[1]		= pB->Get_Y()          - pA->Get_Y();
	v[2]		= pB->asDouble(zField) - pA->asDouble(zField);

	Normal[0]	= (float)(u[1] * v[2] - u[2] * v[2]);
	Normal[1]	= (float)(u[2] * v[0] - u[0] * v[1]);
	Normal[2]	= (float)(u[0] * v[1] - u[1] * v[0]);

	return( true );
}


///////////////////////////////////////////////////////////
//														 //
//														 //
//														 //
///////////////////////////////////////////////////////////

//---------------------------------------------------------

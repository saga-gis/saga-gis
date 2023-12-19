
///////////////////////////////////////////////////////////
//                                                       //
//                         SAGA                          //
//                                                       //
//      System for Automated Geoscientific Analyses      //
//                                                       //
//                     Tool Library                      //
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
//                University of Goettingen               //
//                Goldschmidtstr. 5                      //
//                37077 Goettingen                       //
//                Germany                                //
//                                                       //
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
	Set_Name		(_TL("Import Stereo Lithography File (STL)"));

	Set_Author		("O.Conrad (c) 2008");

	Set_Description	(_TW(
		"An StL (\"StereoLithography\") file is a triangular representation "
		"of a 3-dimensional surface geometry. The surface is tessellated or "
		"broken down logically into a series of small triangles (facets). "
		"Each facet is described by a perpendicular direction and three points "
		"representing the vertices (corners) of the triangle (Ennex Research Corporation). "
		"The StL file format is commonly used for 3D printing."
	));

	Add_Reference("http://www.fabbers.com/tech/STL_Format", SG_T("Ennex Research Corporation - The StL Format"));

	//-----------------------------------------------------
	Parameters.Add_PointCloud ("", "POINTS"  , _TL("Points"  ), _TL(""), PARAMETER_OUTPUT);
	Parameters.Add_Shapes     ("", "POLYGONS", _TL("Polygons"), _TL(""), PARAMETER_OUTPUT, SHAPE_TYPE_Polygon);
	Parameters.Add_Grid_Output("", "GRID"    , _TL("Grid"    ), _TL(""));
	Parameters.Add_TIN        ("", "TIN"     , _TL("TIN"     ), _TL(""), PARAMETER_OUTPUT);

	Parameters.Add_FilePath("", "FILE", _TL("File"), _TL(""), CSG_String::Format("%s|*.stl|%s|*.*",
		_TL("STL Files"),
		_TL("All Files"))
	);

	Parameters.Add_Choice("", "METHOD", _TL("Target"), _TL(""), CSG_String::Format("%s|%s|%s|%s",
		_TL("Points"  ),
		_TL("Polygons"),
		_TL("TIN"     ),
		_TL("Grid"    )), 2
	);

	Parameters.Add_Bool("POINTS", "CENTROIDS" , _TL("Centroids"        ), _TL(""), false);
	Parameters.Add_Bool("POINTS", "DUPLICATES", _TL("Remove Duplicates"), _TL(""),  true);

	Parameters.Add_Choice("", "GRID_DIM", _TL("Grid Dimension"), _TL(""), CSG_String::Format("%s|%s",
		_TL("Width"   ),
		_TL("Cellsize")), 0
	);

	Parameters.Add_Int   ("GRID_DIM", "GRID_WIDTH", _TL("Width"   ), _TL("Number of cells."), 2000 , 10 , true);
	Parameters.Add_Double("GRID_DIM", "GRID_SIZE" , _TL("Cellsize"), _TL(""), 1., 0., true);

	Parameters.Add_Bool  ("", "ROTATE", _TL("Rotation"), _TL(""), false);
	Parameters.Add_Double("ROTATE", "ROT_X", _TL("X Axis"), _TL(""), 0.);
	Parameters.Add_Double("ROTATE", "ROT_Y", _TL("Y Axis"), _TL(""), 0.);
	Parameters.Add_Double("ROTATE", "ROT_Z", _TL("Z Axis"), _TL(""), 0.);
}


///////////////////////////////////////////////////////////
//														 //
///////////////////////////////////////////////////////////

//---------------------------------------------------------
int CSTL_Import::On_Parameters_Enable(CSG_Parameters *pParameters, CSG_Parameter *pParameter)
{
	if( pParameter->Cmp_Identifier("METHOD") )
	{
		pParameters->Set_Enabled("POINTS"  , pParameter->asInt() == 0);
		pParameters->Set_Enabled("POLYGONS", pParameter->asInt() == 1);
		pParameters->Set_Enabled("TIN"     , pParameter->asInt() == 2);
		pParameters->Set_Enabled("GRID_DIM", pParameter->asInt() == 3);
	}

	if( pParameter->Cmp_Identifier("GRID_DIM") )
	{
		pParameters->Set_Enabled("GRID_WIDTH", pParameter->asInt() == 0);
		pParameters->Set_Enabled("GRID_SIZE" , pParameter->asInt() == 1);
	}

	if( pParameter->Cmp_Identifier("CENTROIDS") )
	{
		pParameters->Set_Enabled("DUPLICATES", pParameter->asBool() == false);
	}

	if( pParameter->Cmp_Identifier("ROTATE") )
	{
		pParameter->Set_Children_Enabled(pParameter->asBool());
	}

	return( CSG_Tool::On_Parameters_Enable(pParameters, pParameter) );
}


///////////////////////////////////////////////////////////
//														 //
///////////////////////////////////////////////////////////

//---------------------------------------------------------
bool CSTL_Import::On_Execute(void)
{
	CSG_File Stream;

	if( !Stream.Open(Parameters("FILE")->asString()) )
	{
		return( false );
	}

	//-----------------------------------------------------
	CSG_String Header;

	if( !Stream.Read(Header, 80) )
	{
		return( false );
	}

	Message_Add(Header);

	DWORD _nFacettes;

	if( !Stream.Read(&_nFacettes, sizeof(_nFacettes)) )
	{
		return( false );
	}

	int nFacettes = (int)_nFacettes;

	Message_Fmt("\n%s: %d", _TL("Number of Facettes"), nFacettes);

	//-----------------------------------------------------
	m_bRotate = Parameters("ROTATE")->asBool();

	m_sin.x	= sin(Parameters("ROT_X")->asDouble() * M_DEG_TO_RAD);
	m_sin.y	= sin(Parameters("ROT_Y")->asDouble() * M_DEG_TO_RAD);
	m_sin.z	= sin(Parameters("ROT_Z")->asDouble() * M_DEG_TO_RAD);
	m_cos.x	= cos(Parameters("ROT_X")->asDouble() * M_DEG_TO_RAD);
	m_cos.y	= cos(Parameters("ROT_Y")->asDouble() * M_DEG_TO_RAD);
	m_cos.z	= cos(Parameters("ROT_Z")->asDouble() * M_DEG_TO_RAD);

	switch( Parameters("METHOD")->asInt() )
	{

	//-----------------------------------------------------
	case 0:	{	// Points
		CSG_PointCloud *pPoints = Parameters("POINTS")->asPointCloud();
		pPoints->Create();
		pPoints->Set_Name(SG_File_Get_Name(Parameters("FILE")->asString(), false));
		pPoints->Add_Field("Attribute", SG_DATATYPE_Word);

		bool bCentroid = Parameters("CENTROIDS")->asBool();

		for(int iFacette=0; iFacette<nFacettes && !Stream.is_EOF() && Set_Progress(iFacette, nFacettes); iFacette++)
		{
			TSG_Point_3D Point[4]; WORD Attribute;

			if( Read_Facette(Stream, Point, Attribute) )
			{
				if( bCentroid )
				{
					pPoints->Add_Point(
						(Point[1].x + Point[2].x + Point[3].x) / 3.,
						(Point[1].y + Point[2].y + Point[3].y) / 3.,
						(Point[1].z + Point[2].z + Point[3].z) / 3.
					);

					pPoints->Set_Attribute(0, Attribute);
				}
				else for(int i=1; i<=3; i++)
				{
					pPoints->Add_Point(Point[i]);

					pPoints->Set_Attribute(0, Attribute);
				}
			}
		}

		if( !bCentroid && Parameters("DUPLICATES")->asBool() ) // remove duplicates
		{
			CSG_Array_Int Duplicates(pPoints->Get_Count()); CSG_KDTree_3D Search(pPoints);

			for(sLong i=0; i<pPoints->Get_Count() && Set_Progress(i, pPoints->Get_Count()); i++)
			{
				if( pPoints->Get_Attribute(i, 0) == 0. )
				{
					size_t n = Search.Get_Duplicates(pPoints->Get_X(i), pPoints->Get_Y(i), pPoints->Get_Z(i));

					for(size_t j=1; j<n; j++)
					{
						Duplicates[Search.Get_Match_Index(j)] = (int)j;
					}
				}
			}

			for(sLong i=pPoints->Get_Count()-1; i>=0; i--)
			{
				if( Duplicates[i] > 0 )
				{
					pPoints->Del_Point(i);
				}
			}
		}
		break; }

	//-----------------------------------------------------
	case 1:	{	// Polygons
		CSG_Shapes *pPolygons = Parameters("POLYGONS")->asShapes();
		pPolygons->Create(SHAPE_TYPE_Polygon, SG_File_Get_Name(Parameters("FILE")->asString(), false), NULL, SG_VERTEX_TYPE_XYZ);
		pPolygons->Add_Field("Attribute", SG_DATATYPE_Word );
		pPolygons->Add_Field("Mean"     , SG_DATATYPE_Float);
		Parameters("POLYGONS")->Set_Value(pPolygons);

		for(int iFacette=0; iFacette<nFacettes && !Stream.is_EOF() && Set_Progress(iFacette, nFacettes); iFacette++)
		{
			TSG_Point_3D Point[4]; WORD Attribute;

			if( Read_Facette(Stream, Point, Attribute) )
			{
				CSG_Shape *pPolygon = pPolygons->Add_Shape();

				pPolygon->Add_Point(Point[1]);
				pPolygon->Add_Point(Point[2]);
				pPolygon->Add_Point(Point[3]);
				pPolygon->Add_Point(Point[1]);

				pPolygon->Set_Value(0, Attribute);
				pPolygon->Set_Value(1, (Point[1].z + Point[2].z + Point[3].z) / 3.);
			}
		}

		break; }

	//-----------------------------------------------------
	case 2:	{	// TIN
		CSG_TIN *pTIN = Parameters("TIN")->asTIN();
		pTIN->Destroy();
		pTIN->Set_Name(SG_File_Get_Name(Parameters("FILE")->asString(), false));
		pTIN->Add_Field("Z"        , SG_DATATYPE_Float);
		pTIN->Add_Field("Attribute", SG_DATATYPE_Word );

		for(int iFacette=0; iFacette<nFacettes && !Stream.is_EOF() && Set_Progress(iFacette, nFacettes); iFacette++)
		{
			TSG_Point_3D Point[4]; WORD Attribute;

			if( Read_Facette(Stream, Point, Attribute) )
			{
				CSG_TIN_Node *Node[3];

				for(int i=0; i<3; i++)
				{
					Node[i] = pTIN->Add_Node(CSG_Point(Point[i + 1].x, Point[i + 1].y));
					Node[i]->Set_Value(0, Point[i + 1].z);
					Node[i]->Set_Value(1, Attribute);
				}

				pTIN->Add_Triangle(Node);
			}
		}

		pTIN->Triangulate(false, true);

		break; }

	//-----------------------------------------------------
	case 3:	{	// Grid
		CSG_Rect Extent;

		if( Get_Extent(Stream, Extent, nFacettes) )
		{
			CSG_Grid_System System;

			switch( Parameters("GRID_DIM")->asInt() )
			{
			default: // Width (number of cells)
				System.Create(Extent.Get_XRange() / Parameters("GRID_WIDTH")->asInt(),
					Extent
				);
				break;

			case  1: // Cellsize
				System.Create(Parameters("GRID_SIZE")->asDouble(),
					Extent
				);
			}

			m_pGrid	= SG_Create_Grid(System);
			m_pGrid->Set_Name(SG_File_Get_Name(Parameters("FILE")->asString(), false));
			m_pGrid->Set_NoData_Value(-99999.);
			m_pGrid->Assign_NoData();

			Parameters("GRID")->Set_Value(m_pGrid);

			//---------------------------------------------
			for(int iFacette=0; iFacette<nFacettes && !Stream.is_EOF() && Set_Progress(iFacette, nFacettes); iFacette++)
			{
				TSG_Point_3D Point[4]; WORD Attribute;

				if( Read_Facette(Stream, Point, Attribute) )
				{
					for(int i=1; i<=3; i++)
					{
						Point[i].x = (Point[i].x - m_pGrid->Get_XMin()) / m_pGrid->Get_Cellsize();
						Point[i].y = (Point[i].y - m_pGrid->Get_YMin()) / m_pGrid->Get_Cellsize();
					}

					Set_Triangle(Point + 1);
				}
			}
		}

		break; }
	}

	//-----------------------------------------------------
	return( true );
}


///////////////////////////////////////////////////////////
//														 //
///////////////////////////////////////////////////////////

//---------------------------------------------------------
inline bool CSTL_Import::Read_Facette(CSG_File &Stream, TSG_Point_3D Point[4], WORD &Attribute)
{
	float p[3];

	for(int i=0; i<4; i++)
	{
		if( !Stream.Read(p, sizeof(float), 3) )
		{
			return( false );
		}

		Point[i].x = (double)p[0];
		Point[i].y = (double)p[1];
		Point[i].z = (double)p[2];

		if( m_bRotate )
		{
			Rotate(Point[i]);
		}
	}

	if( !Stream.Read(&Attribute, sizeof(Attribute)) )
	{
		return( false );
	}

	return( true );
}

//---------------------------------------------------------
inline void CSTL_Import::Rotate(TSG_Point_3D &p)
{
	double d;

	d   = (m_cos.z * p.x - m_sin.z * p.y);
	p.y = (m_sin.z * p.x + m_cos.z * p.y);
	p.x = d;

	d   = (m_cos.y * p.z - m_sin.y * p.x);
	p.x = (m_sin.y * p.z + m_cos.y * p.x);
	p.z = d;

	d   = (m_cos.x * p.z - m_sin.x * p.y);
	p.y = (m_sin.x * p.z + m_cos.x * p.y);
	p.z = d;
}


///////////////////////////////////////////////////////////
//														 //
///////////////////////////////////////////////////////////

//---------------------------------------------------------
bool CSTL_Import::Get_Extent(CSG_File &Stream, CSG_Rect &Extent, int nFacettes)
{
	double xMin = 0., xMax = 0., yMin = 0., yMax = 0.;

	for(int iFacette=0; iFacette<nFacettes && !Stream.is_EOF() && Set_Progress(iFacette, nFacettes); iFacette++)
	{
		TSG_Point_3D Point[4]; WORD Attribute;

		if( Read_Facette(Stream, Point, Attribute) )
		{
			if( iFacette == 0 )
			{
				xMin = xMax = Point[1].x;
				yMin = yMax = Point[1].y;
			}
			else for(int i=1; i<=3; i++)
			{
				if( xMin > Point[i].x ) { xMin = Point[i].x; } else if( xMax < Point[i].x ) { xMax = Point[i].x; }
				if( yMin > Point[i].y ) { yMin = Point[i].y; } else if( yMax < Point[i].y ) { yMax = Point[i].y; }
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
void CSTL_Import::Set_Triangle(TSG_Point_3D p[3])
{
	if( p[1].y < p[0].y ) {	TSG_Point_3D pp = p[1]; p[1] = p[0]; p[0] = pp;	}
	if( p[2].y < p[0].y ) {	TSG_Point_3D pp = p[2]; p[2] = p[0]; p[0] = pp;	}
	if( p[2].y < p[1].y ) {	TSG_Point_3D pp = p[2]; p[2] = p[1]; p[1] = pp;	}

	//-----------------------------------------------------
	TSG_Rect	r;

	r.yMin	= p[0].y;
	r.yMax	= p[2].y;
	r.xMin	= p[0].x < p[1].x ? (p[0].x < p[2].x ? p[0].x : p[2].x) : (p[1].x < p[2].x ? p[1].x : p[2].x);
	r.xMax	= p[0].x > p[1].x ? (p[0].x > p[2].x ? p[0].x : p[2].x) : (p[1].x > p[2].x ? p[1].x : p[2].x);

	if( r.yMin >= r.yMax || r.xMin >= r.xMax )
	{
		return;	// no area
	}

	if( (r.yMin < 0.0 && r.yMax < 0.0) || (r.yMin >= m_pGrid->Get_NY() && r.yMax >= m_pGrid->Get_NY())
	||	(r.xMin < 0.0 && r.xMax < 0.0) || (r.xMin >= m_pGrid->Get_NX() && r.xMax >= m_pGrid->Get_NX()) )
	{
		return;	// completely outside grid
	}

	//-----------------------------------------------------
	TSG_Point_3D	d[3];

	if( (d[0].y	= p[2].y - p[0].y) != 0.0 )
	{
		d[0].x	= (p[2].x - p[0].x) / d[0].y;
		d[0].z	= (p[2].z - p[0].z) / d[0].y;
	}

	if( (d[1].y	= p[1].y - p[0].y) != 0.0 )
	{
		d[1].x	= (p[1].x - p[0].x) / d[1].y;
		d[1].z	= (p[1].z - p[0].z) / d[1].y;
	}

	if( (d[2].y	= p[2].y - p[1].y) != 0.0 )
	{
		d[2].x	= (p[2].x - p[1].x) / d[2].y;
		d[2].z	= (p[2].z - p[1].z) / d[2].y;
	}

	//-----------------------------------------------------
	int	ay	= (int)r.yMin;	if( ay < 0 )	ay	= 0;	if( ay < r.yMin )	ay++;
	int	by	= (int)r.yMax;	if( by >= m_pGrid->Get_NY() )	by	= m_pGrid->Get_NY() - 1;

	for(int y=ay; y<=by; y++)
	{
		if( y <= p[1].y && d[1].y > 0.0 )
		{
			Set_Triangle_Line(y,
				p[0].x + (y - p[0].y) * d[0].x,
				p[0].z + (y - p[0].y) * d[0].z,
				p[0].x + (y - p[0].y) * d[1].x,
				p[0].z + (y - p[0].y) * d[1].z
			);
		}
		else if( d[2].y > 0.0 )
		{
			Set_Triangle_Line(y,
				p[0].x + (y - p[0].y) * d[0].x,
				p[0].z + (y - p[0].y) * d[0].z,
				p[1].x + (y - p[1].y) * d[2].x,
				p[1].z + (y - p[1].y) * d[2].z
			);
		}
	}
}

//---------------------------------------------------------
inline void CSTL_Import::Set_Triangle_Line(int y, double xa, double za, double xb, double zb)
{
	if( xb < xa )
	{
		double	d;

		d	= xa;	xa	= xb;	xb	= d;
		d	= za;	za	= zb;	zb	= d;
	}

	if( xb > xa )
	{
		double	dz	= (zb - za) / (xb - xa);
		int		ax	= (int)xa;	if( ax < 0 )	ax	= 0;	if( ax < xa )	ax++;
		int		bx	= (int)xb;	if( bx >= m_pGrid->Get_NX() )	bx	= m_pGrid->Get_NX() - 1;

		for(int x=ax; x<=bx; x++)
		{
			double	z	= za + dz * (x - xa);

			if( m_pGrid->is_NoData(x, y) || m_pGrid->asDouble(x, y) < z )
			{
				m_pGrid->Set_Value(x, y, z);
			}
		}
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
	Set_Name		(_TL("Export TIN to Stereo Lithography File (STL)"));

	Set_Author		("Navaladi, Schoeller, Conrad (c) 2009");

	Set_Description	(_TW(
		"An StL (\"StereoLithography\") file is a triangular representation "
		"of a 3-dimensional surface geometry. The surface is tessellated or "
		"broken down logically into a series of small triangles (facets). "
		"Each facet is described by a perpendicular direction and three points "
		"representing the vertices (corners) of the triangle (Ennex Research Corporation). "
		"The StL file format is commonly used for 3D printing."
	));

	Add_Reference("http://www.fabbers.com/tech/STL_Format", SG_T("Ennex Research Corporation - The StL Format"));

	//-----------------------------------------------------
	Parameters.Add_TIN("",
		"TIN"   , _TL("TIN"        ), _TL(""), PARAMETER_INPUT
	);

	Parameters.Add_Table_Field("TIN",
		"ZFIELD", _TL("Attribute"  ), _TL("")
	);

	Parameters.Add_FilePath("",
		"FILE"  , _TL("File"       ), _TL(""), CSG_String::Format("%s|*.stl|%s|*.*",
			_TL("STL Files"),
			_TL("All Files")), NULL, true
	);

	Parameters.Add_Choice("",
		"BINARY", _TL("Output Type"), _TL(""), CSG_String::Format("%s|%s",
			_TL("ASCII"),
			_TL("binary")
		), 1
	);
}


///////////////////////////////////////////////////////////
//														 //
///////////////////////////////////////////////////////////

//---------------------------------------------------------
bool CSTL_Export::On_Execute(void)
{
	CSG_TIN *pTIN = Parameters("TIN")->asTIN();
	int    zField = Parameters("ZFIELD")->asInt(); 

	CSG_File Stream;

	if( !Stream.Open(Parameters("FILE")->asString(), SG_FILE_W, Parameters("BINARY")->asInt() == 1) )
	{
		Error_Fmt("%s\n'%s'", _TL("failed to create target file"), Parameters("FILE")->asString());

		return( false );
	}

	//-----------------------------------------------------
	if( Parameters("BINARY")->asInt() == 1 )
	{
		char *sHeader = (char *)SG_Calloc(80, sizeof(char));
		DWORD nFacets = (DWORD)pTIN->Get_Triangle_Count();
		WORD   nBytes = 0;

		Stream.Write(sHeader , sizeof(char), 80);
		Stream.Write(&nFacets, sizeof(DWORD));

		SG_Free(sHeader);

		//-------------------------------------------------
		for(sLong iTriangle=0; iTriangle<pTIN->Get_Triangle_Count(); iTriangle++)
		{
			CSG_TIN_Triangle *pTriangle = pTIN->Get_Triangle(iTriangle);

			float v[3]; Get_Normal(pTriangle, zField, v);

			Stream.Write(v, sizeof(float), 3);	// facet normal

			for(int iNode=0; iNode<3; iNode++)
			{
				CSG_TIN_Node *pNode = pTriangle->Get_Node(iNode);

				v[0] = (float)pNode->Get_X();
				v[1] = (float)pNode->Get_Y();
				v[2] = (float)pNode->asDouble(zField);

				Stream.Write(v, sizeof(float), 3);
			}

			Stream.Write(&nBytes, sizeof(WORD));
		}
	}

	//-----------------------------------------------------
	else	// ASCII
	{
		Stream.Printf("solid %s\n", SG_File_Get_Name(Parameters("FILE")->asString(), false).c_str());

		for(sLong iTriangle=0; iTriangle<pTIN->Get_Triangle_Count(); iTriangle++)
		{
			CSG_TIN_Triangle *pTriangle = pTIN->Get_Triangle(iTriangle);

			float v[3]; Get_Normal(pTriangle, zField, v);

			Stream.Printf(" facet normal %.4f %.4f %.4f\n", v[0], v[1], v[2]);
			Stream.Printf("  outer loop\n");

			for(int iNode=0; iNode<3; iNode++)
			{
				CSG_TIN_Node *pNode = pTriangle->Get_Node(iNode);

				v[0] = (float)pNode->Get_X();
				v[1] = (float)pNode->Get_Y();
				v[2] = (float)pNode->asDouble(zField);

				Stream.Printf("   vertex %.4f %.4f %.4f\n", v[0], v[1], v[2]);
			}

			Stream.Printf("  endloop\n");
			Stream.Printf(" endfacet\n");		
		}

		Stream.Printf("endsolid %s\n", SG_File_Get_Name(Parameters("FILE")->asString(), false).c_str());
	}

	return( true );
}

//---------------------------------------------------------
inline bool CSTL_Export::Get_Normal(CSG_TIN_Triangle *pTriangle, int zField, float Normal[3])
{
	double u[3], v[3]; CSG_TIN_Node *pB, *pA = pTriangle->Get_Node(0);

	pB   = pTriangle->Get_Node(1);
	u[0] = pB->Get_X()          - pA->Get_X();
	u[1] = pB->Get_Y()          - pA->Get_Y();
	u[2] = pB->asDouble(zField) - pA->asDouble(zField);

	pB   = pTriangle->Get_Node(2);
	v[0] = pB->Get_X()          - pA->Get_X();
	v[1] = pB->Get_Y()          - pA->Get_Y();
	v[2] = pB->asDouble(zField) - pA->asDouble(zField);

	Normal[0] = (float)(u[1] * v[2] - u[2] * v[2]);
	Normal[1] = (float)(u[2] * v[0] - u[0] * v[1]);
	Normal[2] = (float)(u[0] * v[1] - u[1] * v[0]);

	return( true );
}


///////////////////////////////////////////////////////////
//														 //
//														 //
//														 //
///////////////////////////////////////////////////////////

//---------------------------------------------------------

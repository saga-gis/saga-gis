
///////////////////////////////////////////////////////////
//                                                       //
//                         SAGA                          //
//                                                       //
//      System for Automated Geoscientific Analyses      //
//                                                       //
//                     Tool Library                      //
//                        Grid_IO                        //
//                                                       //
//-------------------------------------------------------//
//                                                       //
//                     ESRI_E00.cpp                      //
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
//                Germany                                //
//                                                       //
///////////////////////////////////////////////////////////

//---------------------------------------------------------
#include "ESRI_E00_Import.h"


///////////////////////////////////////////////////////////
//														 //
//						Import							 //
//														 //
///////////////////////////////////////////////////////////

//---------------------------------------------------------
CESRI_E00_Import::CESRI_E00_Import(void)
{
	Set_Name		(_TL("Import ESRI E00 File"));

	Set_Author		("O.Conrad (c) 2004");

	Set_Description	(_TW(
		"Import data sets from ESRI's E00 interchange format.\n\n"

		"This import filter is based on the E00 format analysis of the GRASS GIS tool "
		"\'m.in.e00\' written by Michel J. Wurtz. Go to the "
		"<a target=\"_blank\" href=\"http://grass.itc.it/\">GRASS GIS Hompage</a> "
		"for more information.\n"

		"The <a target=\"_blank\" href=\"http://avce00.maptools.org/e00compr/index.html\">\'E00Compr\' library</a> "
		"written by Daniel Morissette has been used for e00 file access, so that "
		"compressed e00 files also can be read.\n"
	));

	//-----------------------------------------------------
	Parameters.Add_Table_List(
		"", "TABLES"	, _TL("Tables"),
		_TL(""),
		PARAMETER_OUTPUT_OPTIONAL
	);

	Parameters.Add_Shapes_List(
		"", "SHAPES"	, _TL("Shapes"),
		_TL(""),
		PARAMETER_OUTPUT_OPTIONAL
	);

	Parameters.Add_Grid_List(
		"", "GRIDS"		, _TL("Grids"),
		_TL(""),
		PARAMETER_OUTPUT_OPTIONAL
	);

	Parameters.Add_Table_Output(
		"", "TABLE"		, _TL("Table"),
		_TL("")
	);

	Parameters.Add_FilePath(
		"", "FILE"		, _TL("File"),
		_TL(""),
		CSG_String::Format("ESRI E00 %s|*.e00|%s|*.*",
			_TL("Files"),
			_TL("All Files")
		), NULL, false, false, true
	);

	Parameters.Add_Bool(
		"", "BBND"		, _TL("Import Extents"),
		_TL(""),
		false
	);

	Parameters.Add_Bool(
		"", "BTIC"		, _TL("Import Tick Points"),
		_TL(""),
		false
	);

	Parameters.Add_Bool(
		"", "BTABLES"	, _TL("Import Tables"),
		_TL(""),
		false
	);
}


///////////////////////////////////////////////////////////
//														 //
///////////////////////////////////////////////////////////

//---------------------------------------------------------
bool CESRI_E00_Import::On_Execute(void)
{
	int	nLoaded	= 0;

	m_bBnd		= Parameters("BBND")	->asBool();
	m_bTic		= Parameters("BTIC")	->asBool();
	m_bTables	= Parameters("BTABLES")	->asBool();

	m_pTables	= Parameters("TABLES")	->asTableList();
	m_pShapes	= Parameters("SHAPES")	->asShapesList();
	m_pGrids	= Parameters("GRIDS")	->asGridList();

	m_pTables	->Del_Items();
	m_pShapes	->Del_Items();
	m_pGrids	->Del_Items();

	CSG_Strings	fNames;

	if( Parameters("FILE")->asFilePath()->Get_FilePaths(fNames) && fNames.Get_Count() > 0 )
	{
		for(int i=0; i<fNames.Get_Count(); i++)
		{
			if( Load(fNames[i]) )
			{
				nLoaded++;
			}
		}
	}

	return( nLoaded > 0 );
}


///////////////////////////////////////////////////////////
//														 //
///////////////////////////////////////////////////////////

//---------------------------------------------------------
const char * CESRI_E00_Import::E00_Read_Line(void)
{
	const char	*line	= E00ReadNextLine(m_hReadPtr);

	if( line == NULL )
	{
		FILE	*fp	= fopen(SG_File_Make_Path("", m_e00_Name, CSG_String::Format("e%02d", m_iFile + 1)), "rb");

		if( fp )
		{
			m_iFile++;

			int	nInputLineNo	= m_hReadPtr->nInputLineNo;

			E00ReadRewind(m_hReadPtr);

			fclose(m_hReadPtr->fp);

			m_hReadPtr->fp				= fp;
			m_hReadPtr->nInputLineNo	= nInputLineNo - 1;

			line	= E00ReadNextLine(m_hReadPtr);
		}
	}

	return( line );
}

//---------------------------------------------------------
bool CESRI_E00_Import::E00_Goto_Line(int iLine)
{
	if( m_hReadPtr )
	{
		if( m_iFile == 0 )
		{
			E00ReadRewind(m_hReadPtr);
		}
		else
		{
			E00ReadClose(m_hReadPtr);
			m_hReadPtr	= E00ReadOpen(m_e00_Name);
			m_iFile		= 0;
		}

		while( E00_Read_Line() && m_hReadPtr->nInputLineNo < iLine );

		return( m_hReadPtr->nInputLineNo == iLine );
	}

	return( false );
}


///////////////////////////////////////////////////////////
//														 //
///////////////////////////////////////////////////////////

//---------------------------------------------------------
bool CESRI_E00_Import::Load(const CSG_String &FileName)
{
	bool	bResult	= false;

	const char	*Line;

	m_hReadPtr	= NULL;
	m_e00_Name	= FileName;
	m_iFile		= 0;

	//-----------------------------------------------------
	if( (m_hReadPtr = E00ReadOpen(m_e00_Name)) == NULL )
	{
		Error_Set(CSG_String::Format(SG_T("%s: %s"), _TL("file not found")  , FileName.c_str()));
	}
	else if( (Line = E00_Read_Line()) == NULL )
	{
		Error_Set(CSG_String::Format(SG_T("%s: %s"), _TL("invalid E00 file"), FileName.c_str()));
	}
	else if( strncmp(Line, "EXP", 3) )
	{
		Error_Set(CSG_String::Format(SG_T("%s: %d"), _TL("invalid E00 file"), FileName.c_str()));
	}

	//-----------------------------------------------------
	else
	{
		bResult	= Load();
	}

	//-----------------------------------------------------
	if( m_hReadPtr )
	{
		E00ReadClose(m_hReadPtr);
	}

	return( bResult );
}


///////////////////////////////////////////////////////////
//														 //
///////////////////////////////////////////////////////////

//---------------------------------------------------------
bool CESRI_E00_Import::Load(void)
{
	const char	*line;

	int			prec_grd, prec_arc, prec_lab, prec_pal;

	long		current_line,
				offset_grd	= 0,
				offset_arc	= 0,
				offset_lab	= 0,
				offset_pal	= 0;

	double		scale		= 1.0;

	TSG_Shape_Type	shape_type;

	CSG_Grid		*pGrid;

	CSG_Shapes		*pShapes;

	//-----------------------------------------------------
	m_pPAT	= NULL;
	m_pAAT	= NULL;

	//-----------------------------------------------------
	while( (line = E00_Read_Line()) != NULL && strncmp(line, "EOS", 3) )
	{
		current_line = m_hReadPtr->nInputLineNo;

		// GRID SECTION
		if( !strncmp(line, "GRD  ", 5) )
		{
			offset_grd	= current_line;
			prec_grd	= line[5] - '2';
			skip("EOG");
			continue;
		}

		// ARC SECTION
		if( !strncmp(line, "ARC  ", 5) )
		{
			offset_arc	= current_line;
			prec_arc	= line[5] - '2';
			skip_arc(prec_arc);
			continue;
		}

		// POLYGON TOPOLOGY
		if(	!strncmp(line, "PAL  ", 5)
		||	!strncmp(line, "PFF  ", 5)	)
		{
			offset_pal	= current_line;
			prec_pal	= line[5] - '2';
			skip_pal(prec_pal);
			continue;
		}

		// CENTROID SECTION
		if( !strncmp(line, "CNT  ", 5) )
		{
			skip_dat();
			continue;
		}

		// LABEL SECTION
		if( !strncmp(line, "LAB  ", 5))
		{
			offset_lab	= current_line;
			prec_lab	= line[5] - '2';
			skip_lab(prec_lab);
			continue;
		}

		// INFO SECTION
		if( !strncmp(line, "IFO  ", 5) )
		{
			info_Get_Tables();
			continue;
		}

		// PROJECTION INFOS
		if( !strncmp(line, "PRJ  ", 5) )
		{
			scale		= getproj();
			continue;
		}

		// Annotations (text). To be imported ? Does anybody have an idea ?
		if( !strncmp(line, "TXT  ", 5) )
		{
			skip_txt(line[5] - '2');
			continue;
		}

		// Mask description ? Noting to do with it
		if( !strncmp(line, "MSK  ", 5) )
		{
			skip_msk();
			continue;
		}

		// TOLERANCE SECTION. Should we really use it ?
		if( !strncmp(line, "TOL  ", 5) )
		{
			skip_dat();
			continue;
		}

		// UNKNOW KEYWORD SECTION. Don't know what to do with. Does anybody have an idea?
		if( !strncmp(line, "LNK  ", 5) )
		{
			skip("END OF LINK DATA");
			continue;
		}

		// SPATIAL INDEX SECTION. Noting to do with it
		if( !strncmp(line, "SIN  ", 5) )
		{
			skip("EOX");
			continue;
		}

		// Line pattern and palette. Shade pattern and palette end same as e00 archive !
		if(	!strncmp(line, "CLN  ", 5)
		||	!strncmp(line, "CSH  ", 5)	)
		{
			skip("EOS");
			continue;
		}

		// Font description ? Noting to do with it
		if( !strncmp(line, "FNT  ", 5) )
		{
			skip("EOF");
			continue;
		}

		// PLOT SECTION. Why should we import it ?
		if( !strncmp(line, "PLT  ", 5) )
		{
			skip("EOP");
			continue;
		}

		// LOG SECTION. Nothing to do with it
		if( !strncmp(line, "LOG  ", 5) )
		{
			skip("EOL");
			continue;
		}

		if(	!strncmp(line, "RPL  ", 5)		// Specific to regions. Contains PAL formated data for each subclass
		||	!strncmp(line, "RXP  ", 5)		// Specific to regions. Seems to link regions IDs to PAL polygons IDs
		||	!strncmp(line, "TX6  ", 5)		// Other kind of annotations not same termination. Other differences ?
		||	!strncmp(line, "TX7  ", 5)	)	// Very close from TX6. So same questions and same rules...
		{
			skip("JABBERWOCKY");
			continue;
		}
	}

	//-----------------------------------------------------
	switch( m_pPAT ? (m_pAAT ? 3 : 2) : (m_pAAT ? 1 : 0) )
	{
	case 0: default:
		shape_type	= offset_arc != 0 ? SHAPE_TYPE_Line : SHAPE_TYPE_Point;
		break;

	case 1:	// m_pAAT
		shape_type	= SHAPE_TYPE_Line;
		break;

	case 2:	// m_pPAT
		shape_type	= offset_arc != 0 ? SHAPE_TYPE_Polygon : SHAPE_TYPE_Point;
		break;

	case 3:	// m_pAAT && m_pPAT
		shape_type	= offset_pal != 0 || offset_lab != 0 ? SHAPE_TYPE_Polygon : SHAPE_TYPE_Line;
		break;
	}

	//-----------------------------------------------------
	// Extracting useful information as noted before...

	//-----------------------------------------------------
	if( offset_grd > 0 )
	{
		E00_Goto_Line(offset_grd);

		if( (pGrid = getraster	(prec_grd, scale)) != NULL )
		{
			pGrid->Set_Name(SG_File_Get_Name(m_e00_Name, false));

			m_pGrids->Add_Item(pGrid);
		}
	}

	//-----------------------------------------------------
	if( offset_arc != 0 )
	{
		E00_Goto_Line(offset_arc);

		if( (pShapes = getarcs	(prec_arc, scale, shape_type)) != NULL )
		{
			pShapes->Set_Name(SG_File_Get_Name(m_e00_Name, false));

			m_pShapes->Add_Item(pShapes);
		}
	}

	//-----------------------------------------------------
	if( offset_lab != 0 && shape_type == SHAPE_TYPE_Point )
	{
		E00_Goto_Line(offset_lab);

		if( (pShapes = getsites	(prec_lab, scale)) != NULL )
		{
			pShapes->Set_Name(SG_File_Get_Name(m_e00_Name, false));

			m_pShapes->Add_Item(pShapes);
		}
	}

	//-----------------------------------------------------
	if( offset_lab != 0 && shape_type != SHAPE_TYPE_Point )
	{
		E00_Goto_Line(offset_lab);

		if( (pShapes = getlabels(prec_lab, scale)) != NULL )
		{
			pShapes->Set_Name(SG_File_Get_Name(m_e00_Name, false));

			m_pShapes->Add_Item(pShapes);
		}
	}

	//-----------------------------------------------------
	if( !m_bTables )
	{
		if( m_pPAT )	delete(m_pPAT);
		if( m_pAAT )	delete(m_pAAT);
	}

	return( true );
}


///////////////////////////////////////////////////////////
//														 //
///////////////////////////////////////////////////////////

//---------------------------------------------------------
CSG_Grid * CESRI_E00_Import::getraster(int prec, double scale)
{
	const char	*line;

	int		x, y, ix;
	long	rows, cols, depth, p[5];
	float	f[5];
	double	xres, yres, xmin, ymin, xmax, ymax, nul_val, d[3];
	CSG_Grid	*pGrid;

	//-----------------------------------------------------
	if( (line = E00_Read_Line()) == NULL )
		return( NULL );
//	sscanf(line, "%ld%ld%ld", &cols, &rows, &depth, &nul_val);
	sscanf(line, "%ld%ld%ld%lf", &cols, &rows, &depth, &nul_val);

	if( (line = E00_Read_Line()) == NULL )
		return( NULL );
	sscanf(line, "%lf%lf", &xres, &yres);

	if( (line = E00_Read_Line()) == NULL )
		return( NULL );
	sscanf(line, "%lf%lf", &xmin, &ymin);

	if( (line = E00_Read_Line()) == NULL )
		return( NULL );
	sscanf(line, "%lf%lf", &xmax, &ymax);

	xmax	= xmax * scale;
	xmin	= xmin * scale;
	ymax	= ymax * scale;
	ymin	= ymin * scale;
	xres	= xres * scale;
	yres	= yres * scale;

	xmin	+= xres / 2.0;	// SAGA treats xmin/ymin as "pixel-as-point" and not as "pixel-as-area"
	ymin	+= yres / 2.0;

	if( depth == 2 && prec )
	{
		depth	= 3;
	}

	//-----------------------------------------------------
	switch( depth )
	{
	default:
		pGrid	= NULL;
		break;

	//-----------------------------------------------------
	case 1:
		pGrid	= SG_Create_Grid(SG_DATATYPE_Int, cols, rows, xres, xmin, ymin);
		pGrid->Set_NoData_Value(nul_val);

		for(y=0; y<rows && line && Set_Progress(y, rows); y++)
		{
			for(x=0; x<cols; x+= 5)
			{
				if( (line = E00_Read_Line()) != NULL )
				{
					sscanf(line, "%ld%ld%ld%ld%ld", p, p+1, p+2, p+3, p+4);

					for(ix=0; ix<5 && x+ix<cols; ix++)
					{
						pGrid->Set_Value(x + ix, y, p[ix]);
					}
				}
			}
		}
		break;

	//-----------------------------------------------------
	case 2:
		pGrid	= SG_Create_Grid(SG_DATATYPE_Float, cols, rows, xres, xmin, ymin);
		pGrid->Set_NoData_Value(nul_val);

		for(y=0; y<rows && line && Set_Progress(y, rows); y++)
		{
			for(x=0; x<cols; x+= 5)
			{
				if( (line = E00_Read_Line()) != NULL )
				{
					sscanf(line, "%f%f%f%f%f", f, f+1, f+2, f+3, f+4);

					for(ix=0; ix<5 && x+ix<cols; ix++)
					{
						pGrid->Set_Value(x + ix, y, f[ix]);
					}
				}
			}
		}
		break;

	//-----------------------------------------------------
	case 3:
		pGrid	= SG_Create_Grid(SG_DATATYPE_Double, cols, rows, xres, xmin, ymin);
		pGrid->Set_NoData_Value(nul_val);

		for(y=0; y<rows && line && Set_Progress(y, rows); y++)
		{
			for(x=0; x<cols; x+= 3)
			{
				if( (line = E00_Read_Line()) != NULL )
				{
					sscanf(line, "%lf%lf%lf", d, d+1, d+2);

					for(ix=0; ix<3 && x+ix<cols; ix++)
					{
						pGrid->Set_Value(x + ix, y, d[ix]);
					}
				}
			}
		}
		break;
	}

	//-----------------------------------------------------
	skip("EOG");

	return( pGrid );
}


///////////////////////////////////////////////////////////
//														 //
//														 //
//														 //
///////////////////////////////////////////////////////////

//---------------------------------------------------------
#define ARC_FNODE	2
#define ARC_TNODE	3
#define ARC_LPOL	4
#define ARC_RPOL	5

//---------------------------------------------------------
CSG_Shapes * CESRI_E00_Import::getarcs(int prec, double scale, TSG_Shape_Type &shape_type)
{
	const char	*line;

	int		covnum, cov_id, fnode, tnode, lpol, rpol, nPoints, iPoint;
	double	x_buf[2], y_buf[2];
	CSG_Shape	*pShape;
	CSG_Shapes	*pShapes;

	//-----------------------------------------------------
	pShapes	= SG_Create_Shapes(shape_type);
	pShapes->Add_Field("ID"	, SG_DATATYPE_Int);
	pShapes->Add_Field("ID#"	, SG_DATATYPE_Int);
	pShapes->Add_Field("FNODE"	, SG_DATATYPE_Int);
	pShapes->Add_Field("TNODE"	, SG_DATATYPE_Int);
	pShapes->Add_Field("LPOL"	, SG_DATATYPE_Int);
	pShapes->Add_Field("RPOL"	, SG_DATATYPE_Int);

	Set_Progress(0, 100);

	//-----------------------------------------------------
	do
	{
		Process_Set_Text("%s: %d", _TL("loaded arcs"), pShapes->Get_Count());

		if( (line = E00_Read_Line()) == NULL )
		{
			covnum	= -1;
		}
		else
		{
			sscanf(line, "%d %d %d %d %d %d %d", &covnum, &cov_id, &fnode, &tnode, &lpol, &rpol, &nPoints);
		}

		if( covnum != -1 )
		{
			pShape	= pShapes->Add_Shape();

			pShape->Set_Value(0			, covnum);
			pShape->Set_Value(1			, cov_id);
			pShape->Set_Value(ARC_FNODE	, fnode);
			pShape->Set_Value(ARC_TNODE	, tnode);
			pShape->Set_Value(ARC_LPOL	, lpol);
			pShape->Set_Value(ARC_RPOL	, rpol);

			//---------------------------------------------
			if( prec )	// double precision : 1 coord pair / line
			{
				for(iPoint=0; iPoint<nPoints && line; iPoint++)
				{
					if( (line = E00_Read_Line()) != NULL )
					{
						sscanf(line, "%lf %lf", x_buf, y_buf);

						pShape->Add_Point(x_buf[0] * scale, y_buf[0] * scale);
					}
				}
			}

			//---------------------------------------------
			else		// single precision : 2 x,y pairs / line
			{
				for(iPoint=0; iPoint<nPoints && line; iPoint+=2)
				{
					if( (line = E00_Read_Line()) != NULL )
					{
						sscanf(line, "%lf %lf %lf %lf", x_buf, y_buf, x_buf + 1, y_buf + 1);

						pShape->Add_Point(x_buf[0] * scale, y_buf[0] * scale);

						if( iPoint + 1 < nPoints )
						{
							pShape->Add_Point(x_buf[1] * scale, y_buf[1] * scale);
						}
					}
				}
			}
		}
	}
	while( covnum != -1 && line && Process_Get_Okay(false) );

	//-----------------------------------------------------
	if( pShapes->Get_Count() == 0 )
	{
		delete(pShapes);

		shape_type	= SHAPE_TYPE_Point;

		return( NULL );
	}

	if( shape_type == SHAPE_TYPE_Polygon )
	{
		pShapes	= Arcs2Polygons(pShapes);

		Assign_Attributes(pShapes);
	}

	return( pShapes );
}

//---------------------------------------------------------
CSG_Shapes * CESRI_E00_Import::Arcs2Polygons(CSG_Shapes *pArcs)
{
	//-----------------------------------------------------
	Process_Set_Text(_TL("Arcs to polygons"));

	CSG_Shapes	*pPolygons	= SG_Create_Shapes(SHAPE_TYPE_Polygon);

	pPolygons->Add_Field("ID", SG_DATATYPE_Int);

	//-----------------------------------------------------
	for(int iArc=0; iArc<pArcs->Get_Count() && Set_Progress(iArc, pArcs->Get_Count()); iArc++)
	{
		Arcs2Polygon(pArcs, pPolygons, pArcs->Get_Shape(iArc)->asInt(ARC_LPOL));
		Arcs2Polygon(pArcs, pPolygons, pArcs->Get_Shape(iArc)->asInt(ARC_RPOL));
	}

	//-----------------------------------------------------
	delete(pArcs);

	pPolygons->Make_Clean();

	return( pPolygons );
}

//---------------------------------------------------------
void CESRI_E00_Import::Arcs2Polygon(CSG_Shapes *pArcs, CSG_Shapes *pPolygons, int id)
{
	//-----------------------------------------------------
	if( id <= 1 )
	{
		return;
	}

	//-----------------------------------------------------
	CSG_Shapes	Segments(SHAPE_TYPE_Line);

	Segments.Add_Field("FNODE", SG_DATATYPE_Int);
	Segments.Add_Field("TNODE", SG_DATATYPE_Int);

	//-----------------------------------------------------
	for(int iArc=0; iArc<pArcs->Get_Count(); iArc++)
	{
		CSG_Shape	*pArc	= pArcs->Get_Shape(iArc);

		if( id == pArc->asInt(ARC_LPOL) )
		{
			CSG_Shape	*pSegment	= Segments.Add_Shape();

			pSegment->Set_Value(0, pArc->asInt(ARC_FNODE));
			pSegment->Set_Value(1, pArc->asInt(ARC_TNODE));

			for(int iVertex=0; iVertex<pArc->Get_Point_Count(0); iVertex++)
			{
				pSegment->Add_Point(pArc->Get_Point(iVertex, 0, false), 0);
			}

			pArc->Set_Value(ARC_LPOL, -1);
		}
		else if( id == pArc->asInt(ARC_RPOL) )
		{
			CSG_Shape	*pSegment	= Segments.Add_Shape();

			pSegment->Set_Value(1, pArc->asInt(ARC_FNODE));
			pSegment->Set_Value(0, pArc->asInt(ARC_TNODE));

			for(int iVertex=0; iVertex<pArc->Get_Point_Count(0); iVertex++)
			{
				pSegment->Add_Point(pArc->Get_Point(iVertex, 0,  true), 0);
			}

			pArc->Set_Value(ARC_RPOL, -1);
		}
	}

	if( Segments.Get_Count() <= 0 )
	{
		return;
	}

	//-----------------------------------------------------
	CSG_Shape	*pPolygon	= pPolygons->Add_Shape();

	pPolygon->Set_Value(0, id);

	while( Segments.Get_Count() > 0 )
	{
		CSG_Shape	*pSegment	= Segments.Get_Shape(0);

		int	iPart	= pPolygon->Get_Part_Count();

		do
		{
			for(int iVertex=0; iVertex<pSegment->Get_Point_Count(0); iVertex++)
			{
				pPolygon->Add_Point(pSegment->Get_Point(iVertex), iPart);
			}

			int		fNode	= pSegment->asInt(0);
			int		tNode	= pSegment->asInt(1);

			Segments.Del_Shape(pSegment);	pSegment	= NULL;

			if( fNode != tNode )
			{
				for(int iSegment=0; iSegment<Segments.Get_Count() && !pSegment; iSegment++)
				{
					if( fNode == Segments.Get_Shape(iSegment)->asInt(1) )
					{
						pSegment	= Segments.Get_Shape(iSegment);
					}
				}
			}
		}
		while( pSegment );
	}
}


///////////////////////////////////////////////////////////
//														 //
///////////////////////////////////////////////////////////

//---------------------------------------------------------
CSG_Shapes * CESRI_E00_Import::getlabels(int prec, double scale)	// shape_type: LINE or AREA
{
	const char	*line;

	int		num, id;	// coverage-# and coverage-ID
	double	x, y;
	CSG_Shapes	*pShapes;
	CSG_Shape	*pShape;

	pShapes	= SG_Create_Shapes(SHAPE_TYPE_Point);

	pShapes->Add_Field("ID#"	, SG_DATATYPE_Int);
	pShapes->Add_Field("ID"	, SG_DATATYPE_Int);

	while( (line = E00_Read_Line()) != NULL )
	{
		sscanf(line, "%d %d %lf %lf", &id, &num, &x, &y);

		if( id == -1 )
		{
			break;
		}
		else
		{
			pShape	= pShapes->Add_Shape();

			pShape->Add_Point(x * scale, y * scale);

			pShape->Set_Value(0, num);
			pShape->Set_Value(1, id);

			//---------------------------------------------
			E00_Read_Line();		// 4 values to skip

			if( prec )
			{
				E00_Read_Line();	// on 2nd line when double precision
			}
		}
	}

	if( pShapes->Get_Count() <= 0 )
	{
		delete( pShapes );
		pShapes	= NULL;
	}

	return( pShapes );
}


///////////////////////////////////////////////////////////
//														 //
///////////////////////////////////////////////////////////

//---------------------------------------------------------
CSG_Shapes * CESRI_E00_Import::getsites(int prec, double scale)
{
	const char	*line;

	int		id;
	double	x, y;
	CSG_Shape	*pShape;
	CSG_Shapes	*pShapes;

	pShapes	= SG_Create_Shapes(SHAPE_TYPE_Point);
	pShapes->Add_Field("ID", SG_DATATYPE_Int);

	while( (line = E00_Read_Line()) != NULL )
	{
		sscanf(line, "%d %*d %lf %lf", &id, &x, &y);

		if( id == -1 )
		{
			break;
		}

		pShape	= pShapes->Add_Shape();

		pShape->Add_Point(x * scale, y * scale);
		pShape->Set_Value(0, id);

		//-------------------------------------------------
		E00_Read_Line();		// 4 values to skip

		if( prec )
		{
			E00_Read_Line();	// on 2nd line when double precision
		}
	}

	if( pShapes->Get_Count() <= 0 )
	{
		delete( pShapes );
		pShapes	= NULL;
	}
	else
	{
		Assign_Attributes(pShapes);
	}

	return( pShapes );
}


///////////////////////////////////////////////////////////
//														 //
///////////////////////////////////////////////////////////

//---------------------------------------------------------
double CESRI_E00_Import::getproj(void)
{
	const char	*line;

	double	scale	= 1.0;

	while( (line = E00_Read_Line()) != NULL && strncmp(line, "EOP", 3) )
	{
		if( !strncmp(line, "Units", 5) )
		{
			sscanf(line + 6, "%lf", &scale);
		}
	}

	scale	= 1.0 / scale;

	return( scale );
}


///////////////////////////////////////////////////////////
//														 //
//						info section					 //
//														 //
///////////////////////////////////////////////////////////

//---------------------------------------------------------
struct info_Field
{
	char				Name[18];	// name of item

	int					Position,	// position in data line
						Size,		// size for reading
						Type;		// type of data
};

//---------------------------------------------------------
struct info_Table
{
	char				Name[34],
						AI[4];		// XX if Arc/info file, spaces otherwise

	int					uFields,	// number of usable items in this table
						nFields,	// number of items in this table
						ldr;		// length of data record

	long				nRecords,	// number of data records
						length;		// total length for one data line

	struct info_Field	*Field;		// One per field...
};

//---------------------------------------------------------
// [06.06.2006] ESRI E00 Import crash fix, James Flemer
int CESRI_E00_Import::info_Get_Tables(void)
{
	const char *line;

	char				tmp[12], *p;
	int					i;
	CSG_String			s;
	CSG_Table				*pTable;
	struct info_Table	info;

	//-----------------------------------------------------
	while( (line = E00_Read_Line()) != NULL && strncmp(line, "EOI", 3) )
	{
		strncpy(info.Name, line, 32);
		info.Name[32]	= 0;
		p	= strchr(info.Name, ' ');
		if( p != NULL )
			*p	= 0;
		p	= strchr(info.Name, '.');
		if( p == 0 )
			p	= info.Name;
		else
			p++;
		s	= p;

		strncpy(info.AI	, line + 32,  2);	info.AI[2]	= 0;
		strncpy(tmp		, line + 34,  4);	tmp[ 4]		= 0;	info.uFields	= atoi(tmp);
		strncpy(tmp		, line + 38,  4);	tmp[ 4]		= 0;	info.nFields	= atoi(tmp);
		strncpy(tmp		, line + 42,  4);	tmp[ 4]		= 0;	info.ldr		= atoi(tmp);
		strncpy(tmp		, line + 46, 11);	tmp[11]		= 0;	info.nRecords	= atol(tmp);

		info.length	= 0;
		info.Field	= (struct info_Field *)malloc(info.nFields * sizeof(struct info_Field));

		//---------------------------------------------
		for(i=0; i<info.nFields; i++)
		{
			if( (line = E00_Read_Line()) != NULL )
			{
				sscanf(line, "%16s", info.Field[i].Name);
				info.Field[i].Size	= atoi(&line[16]);
				info.Field[i].Type	= atoi(&line[34]);
			}

			//---------------------------------------------
			switch( info.Field[i].Type )
			{
			case 60:	// float / double
				info.Field[i].Size = info.Field[i].Size == 4 ? 14 : 24;
				break;

			case 50:	// short / long
				info.Field[i].Size = info.Field[i].Size == 2 ?  6 : 11;
				break;

			case 40:	// float
				info.Field[i].Size = 14;
				break;

			case 10:
				info.Field[i].Size = 8;
				break;

			default:	// string
				break;
			}

			if( i < info.uFields )
			{
				info.length			+= info.Field[i].Size;
			}

			if( i == 0 )
			{
				info.Field[i].Position	= 0;
			}
			else
			{
				info.Field[i].Position	= info.Field[i-1].Position + info.Field[i-1].Size;
			}
		}

		//---------------------------------------------
		pTable	= NULL;

		if     ( !s.CmpNoCase(SG_T("aat")) && m_pAAT == NULL )
		{
			pTable	= m_pAAT	= info_Get_Table(info);
		}
		else if( !s.CmpNoCase(SG_T("pat")) && m_pPAT == NULL )
		{
			pTable	= m_pPAT	= info_Get_Table(info);
		}
	//	else if( !s.CmpNoCase("vat") )		// value table (grid)
	//	else if( !s.CmpNoCase("bnd") )		// coverage boundaries
	//	else if( !s.CmpNoCase("tic") )		// tick marks
	//	else if( !s.CmpNoCase("sta") )		// stats on grid
	//	else if( !s.CmpNoCase("lut") )		// look-up table
	//	else if( !s.CmpNoCase("acode") )	// arc attributes
	//	else if( !s.CmpNoCase("pcode") )	// polygon attributes
	//	else								// non graphic tables
		else	// come on, let's get every table we can get...
		{
			pTable	= info_Get_Table(info);
		}

		//-------------------------------------------------
		free(info.Field);

		if( pTable )
		{
			//---------------------------------------------
			if     ( !s.CmpNoCase(SG_T("bnd")) )	// coverage boundaries
			{
				if( m_bBnd )
				{
					CSG_Shapes	*pShapes	= SG_Create_Shapes(SHAPE_TYPE_Polygon, SG_T("Boundary"));

					pShapes->Add_Field("XMIN", SG_DATATYPE_Double);
					pShapes->Add_Field("YMIN", SG_DATATYPE_Double);
					pShapes->Add_Field("XMAX", SG_DATATYPE_Double);
					pShapes->Add_Field("YMAX", SG_DATATYPE_Double);

					CSG_Table_Record	*pRecord	= pTable->Get_Record(0);
					CSG_Shape			*pShape		= pShapes->Add_Shape();

					pShape->Set_Value(0, pRecord->asDouble(0));
					pShape->Set_Value(1, pRecord->asDouble(1));
					pShape->Set_Value(2, pRecord->asDouble(2));
					pShape->Set_Value(3, pRecord->asDouble(3));

					pShape->Add_Point(pRecord->asDouble(0), pRecord->asDouble(1));
					pShape->Add_Point(pRecord->asDouble(0), pRecord->asDouble(3));
					pShape->Add_Point(pRecord->asDouble(2), pRecord->asDouble(3));
					pShape->Add_Point(pRecord->asDouble(2), pRecord->asDouble(1));

					m_pShapes->Add_Item(pShapes);
				}

				delete(pTable);
			}

			//---------------------------------------------
			else if( !s.CmpNoCase(SG_T("tic")) )	// tick marks
			{
				if( m_bTic )
				{
					CSG_Shapes	*pShapes	= SG_Create_Shapes(SHAPE_TYPE_Point, SG_T("Tick Points"));

					pShapes->Add_Field("ID", SG_DATATYPE_Int);
					pShapes->Add_Field("X" , SG_DATATYPE_Double);
					pShapes->Add_Field("Y" , SG_DATATYPE_Double);

					for(i=0; i<pTable->Get_Record_Count(); i++)
					{
						CSG_Table_Record	*pRecord	= pTable->Get_Record(i);
						CSG_Shape			*pShape		= pShapes->Add_Shape();
						pShape->Set_Value(0, pRecord->asInt   (0));
						pShape->Set_Value(1, pRecord->asDouble(1));
						pShape->Set_Value(2, pRecord->asDouble(2));
						pShape->Add_Point(pRecord->asDouble(1), pRecord->asDouble(2));
					}

					m_pShapes->Add_Item(pShapes);
				}

				delete(pTable);
			}

			//---------------------------------------------
			else if( m_bTables )
			{
				m_pTables->Add_Item(pTable);
			}

			//---------------------------------------------
			else if( pTable != m_pPAT && pTable != m_pAAT )
			{
				delete(pTable);
			}
		}
	}

	//-----------------------------------------------------
	// 0 if none, 1 if AAT, 2 if PAT, 3 if both
	return( m_pPAT ? (m_pAAT ? 3 : 2) : (m_pAAT ? 1 : 0) );
}

//---------------------------------------------------------
CSG_Table * CESRI_E00_Import::info_Get_Table(struct info_Table info)
{
	char				*buffer_record, *buffer_item;
	int					iRecord, iField;
	CSG_Table			*pTable;
	CSG_Table_Record	*pRecord;

	//-----------------------------------------------------
	Process_Set_Text(CSG_String(info.Name));

	buffer_record	= (char *)malloc(info.length + 3);
	buffer_item		= (char *)malloc(info.length + 3);

	pTable			= SG_Create_Table();
	pTable->Set_Name(CSG_String(info.Name));

	//-----------------------------------------------------
	for(iField=0; iField<info.uFields; iField++)
	{
		switch( info.Field[iField].Type )
		{
		case 60:	// float / double
			pTable->Add_Field(info.Field[iField].Name, SG_DATATYPE_Double);
			break;

		case 50:	// short / long
			pTable->Add_Field(info.Field[iField].Name, SG_DATATYPE_Int);
			break;

		case 40:	// float
			pTable->Add_Field(info.Field[iField].Name, SG_DATATYPE_Double);
			break;

		case 10:	// short
			pTable->Add_Field(info.Field[iField].Name, SG_DATATYPE_Int);
			break;

		default:	// string
			pTable->Add_Field(info.Field[iField].Name, SG_DATATYPE_String);
			break;
		}
	}

	//-----------------------------------------------------
	for(iRecord=0; iRecord<info.nRecords && Set_Progress(iRecord, info.nRecords); iRecord++)
	{
		info_Get_Record(buffer_record, info.length);

		pRecord	= pTable->Add_Record();

		for(iField=0; iField<info.uFields; iField++)
		{
			strncpy(buffer_item, &buffer_record[info.Field[iField].Position], info.Field[iField].Size);
			buffer_item[info.Field[iField].Size] = 0;

			switch( pTable->Get_Field_Type(iField) )
			{
			default:
				pRecord->Set_Value(iField, atof(buffer_item));
				break;

			case SG_DATATYPE_Int:
				pRecord->Set_Value(iField, atoi(buffer_item));
				break;

			case SG_DATATYPE_String:
				pRecord->Set_Value(iField, CSG_String(buffer_item));
				break;
			}
		}
	}

	//-----------------------------------------------------
	free(buffer_record);
	free(buffer_item);

	return( pTable );
}

//---------------------------------------------------------
void CESRI_E00_Import::info_Skip_Table(struct info_Table info)
{
	char	*buffer_record;
	int		iRecord;

	buffer_record	= (char *)malloc(info.length + 3);

	for(iRecord=0; iRecord<info.nRecords; iRecord++)
	{
		info_Get_Record(buffer_record, info.length);
	}

	free(buffer_record);
}

//---------------------------------------------------------
void CESRI_E00_Import::info_Get_Record(char *buffer, int buffer_length)
{
	const char *line;

	char	*p;
	int		l;

	//-----------------------------------------------------
	p	= buffer;
	l	= 0;

	//-----------------------------------------------------
	if( (line = E00_Read_Line()) != NULL )
	{
		strncpy(buffer, line, buffer_length < 84 ? buffer_length : 84);

		while( l < buffer_length )
		{
			if( *p =='\r' || *p == '\n' || *p == 0 )
			{
				while( (l % 80 || p == buffer) && l < buffer_length )
				{
					l++;
					*p++	= ' ';
				}

				if( l == buffer_length )
				{
					break;
				}
				else if( (line = E00_Read_Line()) != NULL )
				{
					strncpy(p, line, buffer_length - l < 84 ? buffer_length - l : 84);

					if( *p =='\r' || *p == '\n' || *p == 0 )	// if empty line
					{
						l++;
						*p++	= ' ';
						*p		= 0;
					}
				}
			}
			else
			{
				l++;
				p++;
			}
		}

		*p	= 0;
	}
}


///////////////////////////////////////////////////////////
//														 //
//						Skips							 //
//														 //
///////////////////////////////////////////////////////////

//---------------------------------------------------------
bool CESRI_E00_Import::Assign_Attributes(CSG_Shapes *pShapes)
{
	if( !pShapes || pShapes->Get_Field_Count() <= 0 || !m_pPAT || m_pPAT->Get_Field_Count() <= 2 )
	{
		return( false );
	}

	Process_Set_Text(_TL("Assign attributes to shapes..."));

	int	off_Field	= pShapes->Get_Field_Count();

	for(int iField=0; iField<m_pPAT->Get_Field_Count(); iField++)
	{
		pShapes->Add_Field(m_pPAT->Get_Field_Name(iField), m_pPAT->Get_Field_Type(iField));
	}

	for(int i=0; i<m_pPAT->Get_Count() && Set_Progress(i, m_pPAT->Get_Count()); i++)
	{
		CSG_Shape	*pShape	= pShapes->Get_Shape(i);

		if( pShape )
		{
			CSG_Table_Record	*pRecord	= m_pPAT->Get_Record(pShape->asInt(0) - 1);

			if( pRecord )
			{
				for(int iField=0; iField<m_pPAT->Get_Field_Count(); iField++)
				{
					if( SG_Data_Type_is_Numeric(m_pPAT->Get_Field_Type(iField)) )
					{
						pShape->Set_Value(off_Field + iField, pRecord->asDouble(iField));
					}
					else
					{
						pShape->Set_Value(off_Field + iField, pRecord->asString(iField));
					}
				}
			}
		}
	}

	return( true );

/*	pShapes->Set_Index(0, TABLE_INDEX_Ascending);
	m_pPAT ->Set_Index(3, TABLE_INDEX_Ascending);

	for(int iShape=0, off_Record=0; iShape<pShapes->Get_Count() && Set_Progress(iShape, pShapes->Get_Count()); iShape++)
	{
		CSG_Shape	*pShape	= pShapes->Get_Shape_byIndex(iShape);
		int			id		= pShape->asInt(0);

		for(int iRecord=off_Record; iRecord<m_pPAT->Get_Record_Count(); iRecord++)
		{
			CSG_Table_Record	*pRecord	= m_pPAT->Get_Record_byIndex(iRecord);

			if( id == pRecord->asInt(3) )
			{
				for(iField=0; iField<m_pPAT->Get_Field_Count(); iField++)
				{
					if( SG_Data_Type_is_Numeric(m_pPAT->Get_Field_Type(iField)) )
						pShape->Set_Value(off_Field + iField, pRecord->asDouble(iField));
					else
						pShape->Set_Value(off_Field + iField, pRecord->asString(iField));
				}

				off_Record++;

				break;
			}
		}
	}

	return( true );/**/
}


///////////////////////////////////////////////////////////
//														 //
///////////////////////////////////////////////////////////

//---------------------------------------------------------
void CESRI_E00_Import::skip(char *end)
{
	const char	*line;

	int		l	= strlen(end);

	while( (line = E00_Read_Line()) != NULL && strncmp(line, end, l) );
}

//---------------------------------------------------------
void CESRI_E00_Import::skip_dat(void)
{
	const char	*line;

	int		i	= 0;
	
	while( (line = E00_Read_Line()) != NULL && i != -1 )
	{
		sscanf(line, "%d", &i);
	}
}

//---------------------------------------------------------
void CESRI_E00_Import::skip_msk(void)
{
	const char	*line;

	double	xmin, ymin, xmax, ymax, res, sk;
	long	xsize, ysize, nskip;

	if( (line = E00_Read_Line()) != NULL )
	{
		sscanf(line, "%lf %lf %lf", &xmin, &ymin, &xmax);

		if( (line = E00_Read_Line()) != NULL )
		{
			sscanf(line, "%lf %lf %ld %ld", &ymax, &res, &xsize, &ysize);

			sk		= ((ymax - ymin) / res) * ((xmax - xmin) / res) / 32.0;
			nskip	= (long)ceil(sk / 7.0);

			while( nskip-- )
			{
				E00_Read_Line();
			}
		}
	}
}

//---------------------------------------------------------
void CESRI_E00_Import::skip_arc(int prec)
{
	const char	*line;

	int		i, covnum, nPoints;

	while( (line = E00_Read_Line()) != NULL )
	{
		sscanf(line, "%d %*d %*d %*d %*d %*d %d", &covnum, &nPoints);

		if( covnum == -1 )
			break;

		if( prec == 0 )
			nPoints	= (nPoints + 1) / 2;	// number of coordinate lines

		for(i=0; i<nPoints; i++)
		{
			E00_Read_Line();
		}
	}
}

//---------------------------------------------------------
void CESRI_E00_Import::skip_lab(int prec)
{
	const char	*line;

	long	covid;

	while( (line = E00_Read_Line()) != NULL )
	{
		sscanf(line, "%ld", &covid);

		if( covid == -1 )
			break;

		E00_Read_Line();

		if( prec )	// two lines of coordinates in double precision
			E00_Read_Line();
	}
}

//---------------------------------------------------------
void CESRI_E00_Import::skip_pal(int prec)
{
	const char	*line;

	int		i, narcs;

	while( (line = E00_Read_Line()) != NULL )
	{
		sscanf(line, "%d", &narcs);

		if( prec )	// two lines of coordinates in double precision
			E00_Read_Line();

		if( narcs == -1 )
			break;

		for(i=(narcs+1)/2; i; i--)
			E00_Read_Line();
	}
}

//---------------------------------------------------------
void CESRI_E00_Import::skip_txt(int prec)
{
	const char	*line;

	int		i, n, nskip;

	nskip	= prec ? 7 : 5;

	while( (line = E00_Read_Line()) != NULL )
	{
		sscanf( line, "%d", &n);

		if( n == -1 )
			break;

		for(i=0; i<nskip; i++)
			E00_Read_Line();
	}
}


///////////////////////////////////////////////////////////
//														 //
//														 //
//														 //
///////////////////////////////////////////////////////////

//---------------------------------------------------------

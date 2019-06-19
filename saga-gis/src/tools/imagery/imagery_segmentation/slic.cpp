
///////////////////////////////////////////////////////////
//                                                       //
//                         SAGA                          //
//                                                       //
//      System for Automated Geoscientific Analyses      //
//                                                       //
//                     Tool Library:                     //
//                 imagery_segmentation                  //
//                                                       //
//-------------------------------------------------------//
//                                                       //
//                       slic.cpp                        //
//                                                       //
//                  Olaf Conrad (C) 2019                 //
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
#include "slic.h"


///////////////////////////////////////////////////////////
//														 //
//														 //
//														 //
///////////////////////////////////////////////////////////

//---------------------------------------------------------
CSLIC::CSLIC(void)
{
	Set_Name		(_TL("Superpixel Segmentation"));

	Set_Author		("O.Conrad (c) 2019");

	Set_Description	(_TW(
		"The Superpixel Segmentation tool implements the "
		"'Simple Linear Iterative Clustering' (SLIC) algorithm, "
		"an image segmentation method described in Achanta et al. (2010). "
		"\n\n"
		"SLIC is a simple and efficient method to decompose an image in "
		"visually homogeneous regions. It is based on a spatially "
		"localized version of k-means clustering. Similar to mean shift or "
		"quick shift, each pixel is associated to a feature vector. "
		"\n\n"
		"This tool is follows the SLIC implementation created by "
		"Vedaldi and Fulkerson as part of the VLFeat library. "
	));

	Add_Reference("Achanta, R., Shaji, A., Smith, K., Lucchi, A., Fua, P., & Süsstrunk, S.", "2010",
		"Slic Superpixels",
		"EPFL Technical Report no. 149300, June 2010.",
		SG_T("https://infoscience.epfl.ch/record/149300/files/SLIC_Superpixels_TR_2.pdf"), SG_T("epfl.ch")
	);

	Add_Reference("Achanta, R., Shaji, A., Smith, K., Lucchi, A., Fua, P., & Süsstrunk, S.", "2012",
		"SLIC Superpixels compared to state-of-the-art superpixel methods",
		"IEEE transactions on pattern analysis and machine intelligence, 34(11), 2274-2282.",
		SG_T("https://ieeexplore.ieee.org/iel5/34/4359286/06205760.pdf"), SG_T("ieee.org")
	);

	Add_Reference(
		SG_T("http://www.vlfeat.org/overview/slic.html"), SG_T("SLIC at VLFeat.org")
	);

	//-----------------------------------------------------
	Parameters.Add_Grid_List("",
		"FEATURES"		, _TL("Features"),
		_TL(""),
		PARAMETER_INPUT
	);

	Parameters.Add_Bool("FEATURES",
		"NORMALIZE"		, _TL("Normalize"),
		_TL(""),
		false
	);

	Parameters.Add_Shapes("",
		"POLYGONS"		, _TL("Segments"),
		_TL(""),
		PARAMETER_OUTPUT, SHAPE_TYPE_Polygon
	);

	//-----------------------------------------------------
	Parameters.Add_Int("",
		"MAX_ITERATIONS", _TL("Maximum Iterations"),
		_TL(""),
		100, 1, true
	);

	Parameters.Add_Double("",
		"REGULARIZATION", _TL("Regularization"),
		_TL(""),
		1., 0., true
	);

	Parameters.Add_Int("",
		"SIZE"			, _TL("Region Size"),
		_TL("Starting 'cell size' of the superpixels given as number of cells."),
		10, 1, true
	);

	Parameters.Add_Int("",
		"SIZE_MIN"		, _TL("Minimum Region Size"),
		_TL("In postprocessing join segments, which cover less cells than specified here, to a larger neighbour segment."),
		1, 1, true
	);

	//-----------------------------------------------------
	Parameters.Add_Bool("",
		"SUPERPIXELS_DO", _TL("Create Superpixel Grids"),
		_TL(""),
		false
	);

	Parameters.Add_Grid_List("",
		"SUPERPIXELS"	, _TL("Superpixels"),
		_TL(""),
		PARAMETER_OUTPUT_OPTIONAL
	);

	//-----------------------------------------------------
	Parameters.Add_Choice("",
		"POSTPROCESSING", _TL("Post-Processing"),
		_TL(""),
		CSG_String::Format("%s|%s",
			_TL("none"),
			_TL("unsupervised classification")
		), 0
	);

	Parameters.Add_Int("POSTPROCESSING",
		"NCLUSTER"		, _TL("Number of Clusters"),
		_TL(""),
		12, 2, true
	);

	Parameters.Add_Bool("POSTPROCESSING",
		"SPLIT_CLUSTERS", _TL("Split Clusters"),
		_TL(""),
		true
	);

	//-----------------------------------------------------
	m_Centroid	= NULL;
}


///////////////////////////////////////////////////////////
//														 //
///////////////////////////////////////////////////////////

//---------------------------------------------------------
int CSLIC::On_Parameter_Changed(CSG_Parameters *pParameters, CSG_Parameter *pParameter)
{
	return( CSG_Tool_Grid::On_Parameter_Changed(pParameters, pParameter) );
}

//---------------------------------------------------------
int CSLIC::On_Parameters_Enable(CSG_Parameters *pParameters, CSG_Parameter *pParameter)
{
	if( pParameter->Cmp_Identifier("POSTPROCESSING") )
	{
		pParameter->Set_Children_Enabled(pParameter->asInt() == 1);
	}

	return( CSG_Tool_Grid::On_Parameters_Enable(pParameters, pParameter) );
}


///////////////////////////////////////////////////////////
//														 //
///////////////////////////////////////////////////////////

//---------------------------------------------------------
bool CSLIC::On_Execute(void)
{
	m_pGrids	= Parameters("FEATURES")->asGridList();

	m_bNormalize	= Parameters("NORMALIZE")->asBool();

	//-----------------------------------------------------
	CSG_Grid	Segments;

	if( !Get_Segments(Segments) )
	{
		Del_Centroids();

		return( false );
	}

	Get_Generalized(Segments);

	//-----------------------------------------------------
	Get_Grids(Segments);

	bool	bResult	= Get_Polygons(Segments);

	Parameters("POLYGONS")->asShapes()->Set_Name(_TL("Segments"));

	Del_Centroids();

	return( bResult );
}


///////////////////////////////////////////////////////////
//														 //
///////////////////////////////////////////////////////////

//---------------------------------------------------------
inline int CSLIC::Get_Feature_Count(void)
{
	return( m_pGrids->Get_Grid_Count() );
}

//---------------------------------------------------------
inline double CSLIC::Get_Feature(int k, int x, int y)
{
	CSG_Grid	*pGrid	= m_pGrids->Get_Grid(k);

	double	Value	= pGrid->asDouble(x, y);

	if( m_bNormalize && pGrid->Get_StdDev() > 0. )
	{
		Value	= (Value - pGrid->Get_Min()) / pGrid->Get_StdDev();
	}

	return( Value );
}

//---------------------------------------------------------
inline int CSLIC::Fit_To_Grid_System(double Value, int Coordinate)
{
	int	i	= (int)floor(Value + 0.5);

	switch( Coordinate )
	{
	default: return( M_GET_MAX(M_GET_MIN(i, Get_NX() - 1), 0) );
	case  1: return( M_GET_MAX(M_GET_MIN(i, Get_NY() - 1), 0) );
	}
}


///////////////////////////////////////////////////////////
//														 //
///////////////////////////////////////////////////////////

//---------------------------------------------------------
bool CSLIC::Get_Polygons(CSG_Grid &Segments)
{
	CSG_Shapes	*pPolygons	= Parameters("POLYGONS")->asShapes();

	CSG_Tool	*pTool	= SG_Get_Tool_Library_Manager().Create_Tool("shapes_grid", 6);	// Vectorising Grid Classes

	if( (pPolygons && pTool && pTool->Set_Manager(NULL)
		&&  pTool->Set_Parameter("CLASS_ALL"  , 1        )
		&&  pTool->Set_Parameter("SPLIT"      , 0        )
		&&  pTool->Set_Parameter("ALLVERTICES", false    )
		&&  pTool->Set_Parameter("GRID"       , &Segments)
		&&  pTool->Set_Parameter("POLYGONS"   , pPolygons)
		&&  pTool->Execute()) == false )
	{
		SG_Get_Tool_Library_Manager().Delete_Tool(pTool);

		return( false );
	}

	SG_Get_Tool_Library_Manager().Delete_Tool(pTool);

	//-----------------------------------------------------
	pPolygons->Del_Field(pPolygons->Get_Field("NAME"));
	pPolygons->Del_Field(pPolygons->Get_Field("ID"  ));

	for(int k=0; k<Get_Feature_Count(); k++)
	{
		pPolygons->Add_Field(m_pGrids->Get_Grid(k)->Get_Name(), SG_DATATYPE_Double);
	}

	for(int i=0; i<pPolygons->Get_Count(); i++)
	{
		CSG_Shape	*pPolygon	= pPolygons->Get_Shape(i);

		sLong	ID	= pPolygon->asInt(0);

		for(int k=0; k<Get_Feature_Count(); k++)
		{
			if( ID >= 0 && ID < m_Centroid->Get_NCells() )
			{
				pPolygon->Set_Value(1 + k, m_Centroid[2 + k].asDouble(ID));
			}
			else
			{
				pPolygon->Set_NoData(1 + k);
			}
		}
	}

	//-----------------------------------------------------
	if( Parameters("POSTPROCESSING")->asInt() == 0 )
	{
		return( true );
	}

	CSG_Table	Statistics;

	if( ((pTool = SG_Get_Tool_Library_Manager().Create_Tool("table_calculus", 14)) && pTool->Set_Manager(NULL) // Cluster Analysis (Shapes)
		&&  pTool->Set_Parameter("NCLUSTER"  , Parameters("NCLUSTER" ))
		&&  pTool->Set_Parameter("NORMALISE" , Parameters("NORMALIZE"))
		&&  pTool->Set_Parameter("STATISTICS", &Statistics)
		&&  pTool->Set_Parameter("INPUT"     , pPolygons)
		&&  pTool->Set_Parameter("FIELDS"    , "1,2,3,4,5,6,7,8,9,10,11,12,13,14,15,16,17,18,19,20,21,22,23,24,25,26,27,28,29,30,31,32")
		&&  pTool->Execute()) == false )
	{
		SG_Get_Tool_Library_Manager().Delete_Tool(pTool);

		return( false );
	}

	SG_Get_Tool_Library_Manager().Delete_Tool(pTool);

	//-----------------------------------------------------
	CSG_Shapes	Polygons(SHAPE_TYPE_Polygon);

	if( ((pTool = SG_Get_Tool_Library_Manager().Create_Tool("shapes_polygons", 5)) && pTool->Set_Manager(NULL) // Polygon Dissolve
		&&  pTool->Set_Parameter("POLYGONS"  , pPolygons)
		&&  pTool->Set_Parameter("DISSOLVED" , &Polygons)
		&&  pTool->Set_Parameter("FIELDS"    , "CLUSTER")
		&&  pTool->Execute()) == false )
	{
		SG_Get_Tool_Library_Manager().Delete_Tool(pTool);

		return( false );
	}

	SG_Get_Tool_Library_Manager().Delete_Tool(pTool);

	//-----------------------------------------------------
	if( Parameters("SPLIT_CLUSTERS")->asBool() == false )
	{
		return( pPolygons->Create(Polygons) );
	}

	if( ((pTool = SG_Get_Tool_Library_Manager().Create_Tool("shapes_polygons", 10)) && pTool->Set_Manager(NULL) // Polygon Parts to Separate Polygons
		&&  pTool->Set_Parameter("POLYGONS"  , &Polygons)
		&&  pTool->Set_Parameter("PARTS"     , pPolygons)
		&&  pTool->Execute()) == false )
	{
		SG_Get_Tool_Library_Manager().Delete_Tool(pTool);

		return( false );
	}

	SG_Get_Tool_Library_Manager().Delete_Tool(pTool);

	return( true );
}

//---------------------------------------------------------
bool CSLIC::Get_Grids(CSG_Grid &Segments)
{
	if( Parameters("SUPERPIXELS_DO")->asBool() == false )
	{
		return( true ); // nothing to do !
	}

	//-----------------------------------------------------
	CSG_Parameter_Grid_List	*pGrids	= Parameters("SUPERPIXELS")->asGridList();

	pGrids->Del_Items();

	for(int i=0; i<m_pGrids->Get_Item_Count(); i++)
	{
		CSG_Data_Object	*pItem = m_pGrids->Get_Item(i), *pNew = NULL;

		switch( pItem->Get_ObjectType() )
		{
		case SG_DATAOBJECT_TYPE_Grid : pNew = SG_Create_Grid (*pItem->asGrid ()); break;
		case SG_DATAOBJECT_TYPE_Grids: pNew = SG_Create_Grids(*pItem->asGrids()); break;
		default                      : pNew = NULL;
		}

		if( !pNew )
		{
			return( false );
		}

		pNew->Fmt_Name("%s [%s]", pItem->Get_Name(), _TL("SLIC"));

		pGrids->Add_Item(pNew);
	}

	//-----------------------------------------------------
	for(int k=0; k<pGrids->Get_Grid_Count(); k++)
	{
		CSG_Grid	*pGrid = pGrids->Get_Grid(k), *pCentroid = m_Centroid + 2 + k;

		for(sLong Cell=0; Cell<Get_NCells(); Cell++)
		{
			sLong	ID	= Segments.asLong(Cell);

			for(int k=0; k<Get_Feature_Count(); k++)
			{
				if( ID >= 0 && ID < m_Centroid->Get_NCells() )
				{
					pGrid->Set_Value(Cell, pCentroid->asDouble(ID));
				}
				else
				{
					pGrid->Set_NoData(Cell);
				}
			}
		}
	}

	//-----------------------------------------------------
	for(int i=0; i<m_pGrids->Get_Item_Count(); i++)
	{
		DataObject_Add           (pGrids->Get_Item(i));
		DataObject_Set_Parameters(pGrids->Get_Item(i), m_pGrids->Get_Item(i));
	}

	return( true );
}


///////////////////////////////////////////////////////////
//														 //
///////////////////////////////////////////////////////////

//---------------------------------------------------------
bool CSLIC::Get_Segments(CSG_Grid &Segments)
{
	int	Size	= Parameters("SIZE")->asInt();

	if( Size < 1 || !Get_Centroids(Size) )
	{
		Error_Set(_TL("failed to initialize centroids"));

		return( false );
	}

	//-----------------------------------------------------
	Process_Set_Text(_TL("running k-means iterations"));

	Segments.Create(Get_System(), SG_DATATYPE_Word);

	CSG_Grid	Masses(Get_System(), SG_DATATYPE_Word);

	double	Energy_0, Energy_Last = -1.;

	double	factor	= Parameters("REGULARIZATION")->asDouble() / (Size*Size);

	int	max_Iterations	= Parameters("MAX_ITERATIONS")->asInt();

	//-----------------------------------------------------
	for(int Iteration=0; Iteration<max_Iterations && Process_Get_Okay(); Iteration++)
	{
		double	Energy	= 0.;

		for(int y=0; y<Get_NY(); y++)	// assign pixels to centers
		{
			for(int x=0; x<Get_NX(); x++)
			{
				int	cx	= (int)floor((double)x / Size - 0.5);
				int	cy	= (int)floor((double)y / Size - 0.5);

				double	min_Distance	= -1.;

				for(int iy=M_GET_MAX(0, cy); iy<=M_GET_MIN(m_Centroid->Get_NY()-1, cy+1); iy++)
				{
					for(int ix=M_GET_MAX(0, cx); ix<=M_GET_MIN(m_Centroid->Get_NX()-1, cx+1); ix++)
					{
						int	min_x	= m_Centroid[0].asInt(ix, iy);
						int	min_y	= m_Centroid[1].asInt(ix, iy);	//	double min_y	= centers[(2 + Get_Feature_Count()) * region + 1];

						double	appearance	= 0., spatial = (x - min_x)*(x - min_x) + (y - min_y)*(y - min_y);

						for(int k=0; k<Get_Feature_Count(); k++)
						{
							double	cz	= m_Centroid[2 + k].asDouble(ix, iy);
							double	z	= Get_Feature(k, x, y);
							appearance	+= (z - cz)*(z - cz);
						}

						double	Distance	= appearance + factor * spatial;

						if( min_Distance > Distance || min_Distance < 0. )
						{
							min_Distance	= Distance;

							Segments.Set_Value(x, y, ix + iy * m_Centroid->Get_NX());
						}
					}
				}

				Energy	+= min_Distance;
			}
		}

		//-------------------------------------------------
		// check energy termination conditions

		Process_Set_Text(CSG_String::Format("%s %d, %s: %f", _TL("iteration"), 1 + Iteration, _TL("energy"), Energy));

		if( Iteration < 1 )
		{
			Energy_0	= Energy;
		}
		else if( (Energy_Last - Energy) < 1e-5 * (Energy_0 - Energy) )
		{
			break;
		}

		Energy_Last	= Energy;

		//-------------------------------------------------
		// recompute centers

		Masses.Assign(0.);

		for(int i=0; i<Get_Feature_Count()+2; i++)
		{
			m_Centroid[i].Assign(0.);
		}

		for(int y=0; y<Get_NY(); y++)
		{
			for(int x=0; x<Get_NX(); x++)
			{
				sLong	i	= Segments.asInt(x, y);

				Masses.Add_Value(i, 1.);

				m_Centroid[0].Add_Value(i, x);
				m_Centroid[1].Add_Value(i, y);

				for(int k=0; k<Get_Feature_Count(); k++)
				{
					m_Centroid[2 + k].Add_Value(i, Get_Feature(k, x, y));
				}
			}
		}

		#pragma omp parallel for
		for(sLong i=0; i<m_Centroid->Get_NCells(); i++)
		{
			double	Mass	= 1. / M_GET_MAX(Masses.asDouble(i), 1e-8);

			for(int k=0; k<Get_Feature_Count()+2; k++)
			{
				m_Centroid[k].Mul_Value(i, Mass);
			}
		}
	}

	return( true );
}


///////////////////////////////////////////////////////////
//														 //
///////////////////////////////////////////////////////////

//---------------------------------------------------------
bool CSLIC::Get_Edge(CSG_Grid &Edge)
{
	if( !Edge.Create(Get_System(), SG_DATATYPE_Float) )
	{
		Error_Set(_TL("failed to create edge map"));

		return( false );
	}

	//-----------------------------------------------------
	Process_Set_Text(_TL("computing edge map"));	// compute edge map (gradient strength)

	#pragma omp parallel for
	for(int y=1; y<Get_NY()-1; y++)
	{
		for(int x=1; x<Get_NX()-1; x++)
		{
			for(int k=0; k<Get_Feature_Count(); k++)
			{
				double	a	= Get_Feature(k, x - 1, y    );
				double	b	= Get_Feature(k, x + 1, y    );
				double	c	= Get_Feature(k, x    , y + 1);
				double	d	= Get_Feature(k, x    , y - 1);

				Edge.Add_Value(x, y, (a - b)*(a - b) + (c - d)*(c - d));
			}
		}
	}

	return( true );
}

//---------------------------------------------------------
bool CSLIC::Get_Centroids(int Size)
{
	CSG_Grid	Edge;

	if( !Get_Edge(Edge) )
	{
		return( false );
	}

	//-----------------------------------------------------
	Process_Set_Text(_TL("initializing k-means centroids"));	// initialize k-means centroids

	m_Centroid	= new CSG_Grid[2 + Get_Feature_Count()];

	if( !m_Centroid )
	{
		return( false );
	}

	CSG_Grid_System	System(Get_Cellsize() / Size, Get_XMin(), Get_YMin(),
		(int)ceil((double)Get_NX() / Size),
		(int)ceil((double)Get_NY() / Size)
	);

	if( !m_Centroid[0].Create(System, SG_DATATYPE_Word)
	||  !m_Centroid[1].Create(System, SG_DATATYPE_Word) )
	{
		return( false );
	}

	for(int k=0; k<Get_Feature_Count(); k++)
	{
		if( !m_Centroid[2 + k].Create(System, SG_DATATYPE_Float) )
		{
			return( false );
		}
	}

	//-----------------------------------------------------
	#pragma omp parallel for
	for(int cy=0; cy<System.Get_NY(); cy++)
	{
		for(int cx=0; cx<System.Get_NX(); cx++)
		{
			int	x	= Fit_To_Grid_System(Size * (cx + 0.5), 0);
			int	y	= Fit_To_Grid_System(Size * (cy + 0.5), 1);

			double	min_e	= -1.;
			int		min_x	= 0;
			int		min_y	= 0;

			for(int iy=M_GET_MAX(0, y-1); iy<=M_GET_MIN(Get_NY()-1, y+1); iy++)	// search in a 3x3 neighbourhood the smallest edge response
			{
				for(int ix=M_GET_MAX(0, x-1); ix<=M_GET_MIN(Get_NX()-1, x+1); ix++)
				{
					double	ie	= Edge.asDouble(ix, iy);

					if( min_e > ie || min_e < 0. )
					{
						min_e	= ie;
						min_x	= ix;
						min_y	= iy;
					}
				}
			}

			m_Centroid[0].Set_Value(cx, cy, min_x);	// initialize the new center at this location
			m_Centroid[1].Set_Value(cx, cy, min_y);

			for(int k=0; k<Get_Feature_Count(); k++)
			{
				m_Centroid[2 + k].Set_Value(cx, cy, Get_Feature(k, min_x, min_y));
			}
		}
	}

	//-----------------------------------------------------
	return( true );
}

//---------------------------------------------------------
bool CSLIC::Del_Centroids(void)
{
	if( m_Centroid )
	{
		for(int i=0; i<Get_Feature_Count()+2; i++)
		{
			m_Centroid[i].Destroy();
		}

		delete[](m_Centroid);

		m_Centroid	= NULL;

		return( true );
	}

	return( false );
}


///////////////////////////////////////////////////////////
//														 //
///////////////////////////////////////////////////////////

//---------------------------------------------------------
bool CSLIC::Get_Generalized(CSG_Grid &Segments)
{
	int	min_Size	= Parameters("SIZE_MIN")->asInt();

	if( min_Size <= 1 )
	{
		return( true );	// nothing to do !
	}

	//-----------------------------------------------------
	Process_Set_Text(_TL("eliminating small regions"));

	CSG_Grid	Cleaned(Get_System(), SG_DATATYPE_Int);
	CSG_Grid	Segment(Get_System(), SG_DATATYPE_Int);

	//-----------------------------------------------------
	for(sLong Cell=0; Cell<Get_NCells(); Cell++)
	{
		if( Cleaned.asInt(Cell) )
		{
			continue;
		}

		//-------------------------------------------------
		int	ID	= Segments.asInt(Cell), Size = 0;

		int	ID_Cleaned	= ID + 1;

		Segment.Set_Value(Size++, (int)Cell);

		Cleaned.Set_Value(Cell, ID + 1);

		for(int i=0; i<8; i+=2)	// find ID_Cleaned as the ID of an already cleaned region neihbour of this pixel
		{
			int	ix	= Get_xTo(i, (int)(Cell % Get_NX()));
			int	iy	= Get_yTo(i, (int)(Cell / Get_NX()));

			if( is_InGrid(ix, iy) && Cleaned.asInt(ix, iy) != 0 )
			{
				ID_Cleaned	= Cleaned.asInt(ix, iy);
			}
		}

		//-------------------------------------------------
		for(int	Expanded=0; Expanded<Size; Expanded++)	// expand the segment
		{
			sLong	open	= Segment.asLong(Expanded);

			for(int i=0; i<8; i+=2)
			{
				int	ix	= Get_xTo(i, (int)(open % Get_NX()));
				int	iy	= Get_yTo(i, (int)(open / Get_NX()));

				if( is_InGrid(ix, iy) && Cleaned.asInt(ix, iy) == 0 && Segments.asInt(ix, iy) == ID )
				{
					Cleaned.Set_Value(ix, iy, ID + 1);

					Segment.Set_Value(Size++, ix + iy * Get_NX());
				}
			}
		}

		//-------------------------------------------------
		if( Size < min_Size )	// change label to ID_Cleaned if the segment is too small
		{
			while( Size > 0 )
			{
				Cleaned.Set_Value(Segment.asInt(--Size), ID_Cleaned);
			}
		}
	}

	//-----------------------------------------------------
	for(sLong i=0; i<Get_NCells(); i++)	// restore base 0 indexing of the regions
	{
		Segments.Set_Value(i, Cleaned.asDouble(i) - 1);
	}

	return( true );
}


///////////////////////////////////////////////////////////
//														 //
//														 //
//														 //
///////////////////////////////////////////////////////////

//---------------------------------------------------------

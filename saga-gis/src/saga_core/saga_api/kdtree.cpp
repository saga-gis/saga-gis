
///////////////////////////////////////////////////////////
//                                                       //
//                         SAGA                          //
//                                                       //
//      System for Automated Geoscientific Analyses      //
//                                                       //
//           Application Programming Interface           //
//                                                       //
//                  Library: SAGA_API                    //
//                                                       //
//-------------------------------------------------------//
//                                                       //
//                     kdtree.cpp                        //
//                                                       //
//          Copyright (C) 2019 by Olaf Conrad            //
//                                                       //
//-------------------------------------------------------//
//                                                       //
// This file is part of 'SAGA - System for Automated     //
// Geoscientific Analyses'.                              //
//                                                       //
// This library is free software; you can redistribute   //
// it and/or modify it under the terms of the GNU Lesser //
// General Public License as published by the Free       //
// Software Foundation, either version 2.1 of the        //
// License, or (at your option) any later version.       //
//                                                       //
// This library is distributed in the hope that it will  //
// be useful, but WITHOUT ANY WARRANTY; without even the //
// implied warranty of MERCHANTABILITY or FITNESS FOR A  //
// PARTICULAR PURPOSE. See the GNU Lesser General Public //
// License for more details.                             //
//                                                       //
// You should have received a copy of the GNU Lesser     //
// General Public License along with this program; if    //
// not, see <http://www.gnu.org/licenses/>.              //
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
#include "pointcloud.h"

#include "nanoflann.hpp"


///////////////////////////////////////////////////////////
//														 //
//														 //
//														 //
///////////////////////////////////////////////////////////

//---------------------------------------------------------
class CSG_KDTree_Adaptor
{
public:

	typedef nanoflann::KDTreeSingleIndexAdaptor<nanoflann::L2_Simple_Adaptor<double, CSG_KDTree_Adaptor>,
		CSG_KDTree_Adaptor, 2> kd_tree_2d;

	typedef nanoflann::KDTreeSingleIndexAdaptor<nanoflann::L2_Simple_Adaptor<double, CSG_KDTree_Adaptor>,
		CSG_KDTree_Adaptor, 3> kd_tree_3d;

	//-----------------------------------------------------
	inline virtual size_t		kdtree_get_point_count	(void)								const	= 0;
	inline virtual double		kdtree_get_pt			(const size_t Index, int Dimension)	const	= 0;

	template <class BBOX> bool	kdtree_get_bbox			(BBOX &bb)	const
	{
		double Extent[3][2];

		if( !Get_Extent(Extent) )
		{
			return( false );
		}

		bb[0].low	= Extent[0][0];	bb[0].high	= Extent[0][1];
		bb[1].low	= Extent[1][0];	bb[1].high	= Extent[1][1];

		if( bb.size() > 2 )
		{
			bb[2].low	= Extent[2][0];	bb[2].high	= Extent[2][1];
		}

		return( true );
	}

	//-----------------------------------------------------
	CSG_Data_Object *			Get_Data_Object	(void)	{	return( m_pData );	}


protected:

	double						m_zScale;

	CSG_Data_Object				*m_pData;


	virtual bool				Get_Extent				(double Extent[3][2])	const	{	return( false );	}

};

//---------------------------------------------------------
class CSG_KDTree_Adaptor_Points : public CSG_KDTree_Adaptor
{
public:

	CSG_KDTree_Adaptor_Points(CSG_Shapes *pPoints, int zField = -1, double zScale = 1.)
	{
		m_pData		= m_pPoints	= pPoints;
		m_zField	= m_pPoints && zField < m_pPoints->Get_Count() ? zField : -1;
		m_zScale	= zScale;
	}

	inline virtual size_t	kdtree_get_point_count	(void)	const
	{
		return( m_pPoints->Get_Count() );
	}

	inline virtual double	kdtree_get_pt			(const size_t Index, int Dimension)	const
	{
		if( Dimension == 0 ) { return( m_pPoints->Get_Shape(Index)->Get_Point(0).x ); }
		if( Dimension == 1 ) { return( m_pPoints->Get_Shape(Index)->Get_Point(0).y ); }
		if( Dimension == 2 ) { return( m_zScale * (m_zField >= 0
			? m_pPoints->Get_Shape(Index)->asDouble(m_zField)
			: m_pPoints->Get_Shape(Index)->Get_Z(0)	));
		}

		return( 0. );
	}


protected:

	int						m_zField;

	CSG_Shapes				*m_pPoints;


	virtual bool			Get_Extent				(double Extent[3][2])	const
	{
		Extent[0][0]	= m_pPoints->Get_Extent().Get_XMin();
		Extent[0][1]	= m_pPoints->Get_Extent().Get_XMax();
		Extent[1][0]	= m_pPoints->Get_Extent().Get_YMin();
		Extent[1][1]	= m_pPoints->Get_Extent().Get_YMax();
		Extent[2][0]	= m_zField < 0 ? 0. : m_pPoints->Get_Minimum(m_zField);
		Extent[2][1]	= m_zField < 0 ? 0. : m_pPoints->Get_Maximum(m_zField);

		return( true );
	}

};

//---------------------------------------------------------
class CSG_KDTree_Adaptor_PointCloud : public CSG_KDTree_Adaptor
{
public:

	CSG_KDTree_Adaptor_PointCloud(CSG_PointCloud *pPoints, double zScale = 1.)
	{
		m_pData		= m_pPoints	= pPoints;
		m_zScale	= zScale;
	}

	inline virtual size_t	kdtree_get_point_count	(void)	const
	{
		return( m_pPoints->Get_Count() );
	}

	inline virtual double	kdtree_get_pt			(const size_t Index, int Dimension)	const
	{
		if( Dimension == 0 ) { return( m_pPoints->Get_X(Index)            ); }
		if( Dimension == 1 ) { return( m_pPoints->Get_Y(Index)            ); }
		if( Dimension == 2 ) { return( m_pPoints->Get_Z(Index) * m_zScale ); }

		return( 0. );
	}


protected:

	CSG_PointCloud			*m_pPoints;


	virtual bool			Get_Extent				(double Extent[3][2])	const
	{
		Extent[0][0]	= m_pPoints->Get_Extent().Get_XMin();
		Extent[0][1]	= m_pPoints->Get_Extent().Get_XMax();
		Extent[1][0]	= m_pPoints->Get_Extent().Get_YMin();
		Extent[1][1]	= m_pPoints->Get_Extent().Get_YMax();
		Extent[2][0]	= m_pPoints->             Get_ZMin();
		Extent[2][1]	= m_pPoints->             Get_ZMax();

		return( true );
	}

};


///////////////////////////////////////////////////////////
//														 //
//														 //
//														 //
///////////////////////////////////////////////////////////

//---------------------------------------------------------
CSG_KDTree::CSG_KDTree(void)
{
	_On_Construction();
}

//---------------------------------------------------------
CSG_KDTree::~CSG_KDTree(void)
{}

//---------------------------------------------------------
void CSG_KDTree::_On_Construction(void)
{
	m_pKDTree	= NULL;
	m_pAdaptor	= NULL;
}

//---------------------------------------------------------
bool CSG_KDTree::Destroy(void)
{
	SG_DELETE_SAFE(m_pAdaptor);

	m_Matches.clear();

	return( true );
}

//---------------------------------------------------------
CSG_Shape * CSG_KDTree::Get_Match_Shape(size_t i) const
{
	if( i < Get_Match_Count() )
	{
		CSG_Shapes	*pShapes	= m_pAdaptor && m_pAdaptor->Get_Data_Object() ? m_pAdaptor->Get_Data_Object()->asShapes() : NULL;

		if( pShapes )
		{
			return( pShapes->Get_Shape((int)Get_Match_Index(i)) );
		}
	}

	return( NULL );
}


///////////////////////////////////////////////////////////
//														 //
//														 //
//														 //
///////////////////////////////////////////////////////////

//---------------------------------------------------------
CSG_KDTree_2D::CSG_KDTree_2D(void)
{
	_On_Construction();
}

//---------------------------------------------------------
CSG_KDTree_2D::~CSG_KDTree_2D(void)
{
	Destroy();
}

//---------------------------------------------------------
CSG_KDTree_2D::CSG_KDTree_2D(CSG_Shapes *pPoints)
{
	_On_Construction();

	Create(pPoints);
}

bool CSG_KDTree_2D::Create(CSG_Shapes *pPoints)
{
	Destroy();

	m_pAdaptor	= new CSG_KDTree_Adaptor_Points(pPoints);
	m_pKDTree	= new CSG_KDTree_Adaptor::kd_tree_2d(2, *m_pAdaptor, nanoflann::KDTreeSingleIndexAdaptorParams(10));

	((CSG_KDTree_Adaptor::kd_tree_2d *)m_pKDTree)->buildIndex();

	return( true );
}

//---------------------------------------------------------
bool CSG_KDTree_2D::Destroy(void)
{
	if( m_pKDTree  )
	{
		delete((CSG_KDTree_Adaptor::kd_tree_2d *)m_pKDTree);

		m_pKDTree	= NULL;
	}

	return( CSG_KDTree::Destroy() );
}


///////////////////////////////////////////////////////////
//														 //
///////////////////////////////////////////////////////////

//---------------------------------------------------------
size_t CSG_KDTree_2D::Get_Nearest_Points(double Coordinate[2], size_t Count, double Radius)
{
	nanoflann::SearchParams	SearchParams;

	SearchParams.sorted	= Count > 0;

	((CSG_KDTree_Adaptor::kd_tree_2d *)m_pKDTree)->radiusSearch(Coordinate, Radius, m_Matches, SearchParams);

	if( Count > 0 && m_Matches.size() > Count )
	{
		m_Matches.resize(Count);
	}

	return( m_Matches.size() );
}

//---------------------------------------------------------
bool CSG_KDTree_2D::Get_Nearest_Point(double Coordinate[2], size_t &Index, double &Distance)
{
	return( Get_Nearest_Points(Coordinate, 1, &Index, &Distance) == 1 );
}

//---------------------------------------------------------
size_t CSG_KDTree_2D::Get_Nearest_Points(double Coordinate[2], size_t Count, size_t *Indices, double *Distances)
{
	return( ((CSG_KDTree_Adaptor::kd_tree_2d *)m_pKDTree)->knnSearch(Coordinate, Count, Indices, Distances) );
}

//---------------------------------------------------------
bool CSG_KDTree_2D::Get_Nearest_Point(double Coordinate[2], size_t &Index)
{
	double	Distance;

	return( Get_Nearest_Points(Coordinate, 1, &Index, &Distance) == 1 );
}

//---------------------------------------------------------
CSG_Shape * CSG_KDTree_2D::Get_Nearest_Shape(double Coordinate[2])
{
	size_t	Index;

	CSG_Shapes	*pShapes	= m_pAdaptor && m_pAdaptor->Get_Data_Object() ? m_pAdaptor->Get_Data_Object()->asShapes() : NULL;

	return( pShapes && Get_Nearest_Point(Coordinate, Index) ? pShapes->Get_Shape((int)Index) : NULL );
}

//---------------------------------------------------------
size_t      CSG_KDTree_2D::Get_Nearest_Points(double x, double y, size_t Count, double Radius)
{
	double	c[2]; c[0] = x; c[1] = y;

	return( Get_Nearest_Points(c, Count, Radius) );
}

size_t      CSG_KDTree_2D::Get_Nearest_Points(double x, double y, size_t Count, size_t *Indices, double *Distances)
{
	double	c[2]; c[0] = x; c[1] = y;

	return( Get_Nearest_Points(c, Count, Indices, Distances) );
}

bool        CSG_KDTree_2D::Get_Nearest_Point(double x, double y, size_t &Index, double &Distance)
{
	double	c[2]; c[0] = x; c[1] = y;

	return( Get_Nearest_Point(c, Index, Distance) );
}

bool        CSG_KDTree_2D::Get_Nearest_Point(double x, double y, size_t &Index)
{
	double	c[2]; c[0] = x; c[1] = y;

	return( Get_Nearest_Point(c, Index) );
}

CSG_Shape * CSG_KDTree_2D::Get_Nearest_Shape(double x, double y)
{
	double	c[2]; c[0] = x; c[1] = y;

	return( Get_Nearest_Shape(c) );
}


///////////////////////////////////////////////////////////
//														 //
//														 //
//														 //
///////////////////////////////////////////////////////////

//---------------------------------------------------------
CSG_KDTree_3D::CSG_KDTree_3D(void)
{
	_On_Construction();
}

//---------------------------------------------------------
CSG_KDTree_3D::~CSG_KDTree_3D(void)
{
	Destroy();
}

//---------------------------------------------------------
CSG_KDTree_3D::CSG_KDTree_3D(CSG_Shapes *pPoints, int zField, double zScale)
{
	_On_Construction();

	Create(pPoints, zField, zScale);
}

bool CSG_KDTree_3D::Create(CSG_Shapes *pPoints, int zField, double zScale)
{
	Destroy();

	m_pAdaptor	= new CSG_KDTree_Adaptor_Points(pPoints, zField, zScale);
	m_pKDTree	= new CSG_KDTree_Adaptor::kd_tree_3d(3, *m_pAdaptor, nanoflann::KDTreeSingleIndexAdaptorParams(10));

	((CSG_KDTree_Adaptor::kd_tree_3d *)m_pKDTree)->buildIndex();

	return( true );
}

//---------------------------------------------------------
CSG_KDTree_3D::CSG_KDTree_3D(CSG_PointCloud *pPoints)
{
	_On_Construction();

	Create(pPoints);
}

bool CSG_KDTree_3D::Create(CSG_PointCloud *pPoints)
{
	Destroy();

	m_pAdaptor	= new CSG_KDTree_Adaptor_PointCloud(pPoints);
	m_pKDTree	= new CSG_KDTree_Adaptor::kd_tree_3d(3, *m_pAdaptor, nanoflann::KDTreeSingleIndexAdaptorParams(10));

	((CSG_KDTree_Adaptor::kd_tree_3d *)m_pKDTree)->buildIndex();

	return( true );
}

//---------------------------------------------------------
bool CSG_KDTree_3D::Destroy(void)
{
	if( m_pKDTree )
	{
		delete((CSG_KDTree_Adaptor::kd_tree_3d *)m_pKDTree);
		
		m_pKDTree	= NULL;
	}

	return( CSG_KDTree::Destroy() );
}


///////////////////////////////////////////////////////////
//														 //
///////////////////////////////////////////////////////////

//---------------------------------------------------------
size_t CSG_KDTree_3D::Get_Nearest_Points(double Coordinate[3], size_t Count, double Radius)
{
	nanoflann::SearchParams	SearchParams;

	SearchParams.sorted	= Count > 0;

	const size_t nMatches	=
	((CSG_KDTree_Adaptor::kd_tree_3d *)m_pKDTree)->radiusSearch(Coordinate, Radius, m_Matches, SearchParams);

	if( Count > 0 && m_Matches.size() > Count )
	{
		m_Matches.resize(Count);
	}

	return( m_Matches.size() );
}

//---------------------------------------------------------
size_t CSG_KDTree_3D::Get_Nearest_Points(double Coordinate[3], size_t Count, size_t *Indices, double *Distances)
{
	return( ((CSG_KDTree_Adaptor::kd_tree_3d *)m_pKDTree)->knnSearch(Coordinate, Count, Indices, Distances) );
}

//---------------------------------------------------------
bool CSG_KDTree_3D::Get_Nearest_Point(double Coordinate[3], size_t &Index, double &Distance)
{
	return( Get_Nearest_Points(Coordinate, 1, &Index, &Distance) == 1 );
}

//---------------------------------------------------------
bool CSG_KDTree_3D::Get_Nearest_Point(double Coordinate[3], size_t &Index)
{
	double	Distance;

	return( Get_Nearest_Points(Coordinate, 1, &Index, &Distance) == 1 );
}

//---------------------------------------------------------
CSG_Shape * CSG_KDTree_3D::Get_Nearest_Shape(double Coordinate[3])
{
	size_t	Index;

	CSG_Shapes	*pShapes	= m_pAdaptor && m_pAdaptor->Get_Data_Object() ? m_pAdaptor->Get_Data_Object()->asShapes() : NULL;

	return( pShapes && Get_Nearest_Point(Coordinate, Index) ? pShapes->Get_Shape((int)Index) : NULL );
}

//---------------------------------------------------------
size_t      CSG_KDTree_3D::Get_Nearest_Points(double x, double y, double z, size_t Count, double Radius)
{
	double	c[2]; c[0] = x; c[1] = y;

	return( Get_Nearest_Points(c, Count, Radius) );
}

size_t      CSG_KDTree_3D::Get_Nearest_Points(double x, double y, double z, size_t Count, size_t *Indices, double *Distances)
{
	double	c[2]; c[0] = x; c[1] = y;

	return( Get_Nearest_Points(c, Count, Indices, Distances) );
}

bool        CSG_KDTree_3D::Get_Nearest_Point(double x, double y, double z, size_t &Index, double &Distance)
{
	double	c[2]; c[0] = x; c[1] = y;

	return( Get_Nearest_Point(c, Index, Distance) );
}

bool        CSG_KDTree_3D::Get_Nearest_Point(double x, double y, double z, size_t &Index)
{
	double	c[2]; c[0] = x; c[1] = y;

	return( Get_Nearest_Point(c, Index) );
}

CSG_Shape * CSG_KDTree_3D::Get_Nearest_Shape(double x, double y, double z)
{
	double	c[2]; c[0] = x; c[1] = y;

	return( Get_Nearest_Shape(c) );
}


///////////////////////////////////////////////////////////
//														 //
//														 //
//														 //
///////////////////////////////////////////////////////////

//---------------------------------------------------------

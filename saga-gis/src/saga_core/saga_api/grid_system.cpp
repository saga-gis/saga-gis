/**********************************************************
 * Version $Id$
 *********************************************************/

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
//                   grid_system.cpp                     //
//                                                       //
//          Copyright (C) 2005 by Olaf Conrad            //
//                                                       //
//-------------------------------------------------------//
//                                                       //
// This file is part of 'SAGA - System for Automated     //
// Geoscientific Analyses'.                              //
//                                                       //
// This library is free software; you can redistribute   //
// it and/or modify it under the terms of the GNU Lesser //
// General Public License as published by the Free       //
// Software Foundation, version 2.1 of the License.      //
//                                                       //
// This library is distributed in the hope that it will  //
// be useful, but WITHOUT ANY WARRANTY; without even the //
// implied warranty of MERCHANTABILITY or FITNESS FOR A  //
// PARTICULAR PURPOSE. See the GNU Lesser General Public //
// License for more details.                             //
//                                                       //
// You should have received a copy of the GNU Lesser     //
// General Public License along with this program; if    //
// not, write to the Free Software Foundation, Inc.,     //
// 51 Franklin Street, 5th Floor, Boston, MA 02110-1301, //
// USA.                                                  //
//                                                       //
//-------------------------------------------------------//
//                                                       //
//    contact:    Olaf Conrad                            //
//                Institute of Geography                 //
//                University Hamburg                     //
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
#include "grid.h"
#include "shapes.h"


///////////////////////////////////////////////////////////
//														 //
//														 //
//														 //
///////////////////////////////////////////////////////////

//---------------------------------------------------------
CSG_Grid_System::CSG_Grid_System(void)
{
	m_Cellsize		= -1.0;

	m_NX	= m_NY	= 0;
}

//---------------------------------------------------------
CSG_Grid_System::CSG_Grid_System(const CSG_Grid_System &System)
{
	m_Cellsize	= -1.0;

	Assign(System);
}

//---------------------------------------------------------
CSG_Grid_System::CSG_Grid_System(double Cellsize, const CSG_Rect &Extent)
{
	m_Cellsize	= -1.0;

	Assign(Cellsize, Extent);
}

//---------------------------------------------------------
CSG_Grid_System::CSG_Grid_System(double Cellsize, double xMin, double yMin, double xMax, double yMax)
{
	m_Cellsize	= -1.0;

	Assign(Cellsize, xMin, yMin, xMax, yMax);
}

//---------------------------------------------------------
CSG_Grid_System::CSG_Grid_System(double Cellsize, double xMin, double yMin, int NX, int NY)
{
	m_Cellsize	= -1.0;

	Assign(Cellsize, xMin, yMin, NX, NY);
}

//---------------------------------------------------------
CSG_Grid_System::~CSG_Grid_System(void)
{
}


///////////////////////////////////////////////////////////
//														 //
//														 //
//														 //
///////////////////////////////////////////////////////////

//---------------------------------------------------------
bool CSG_Grid_System::is_Valid(void) const
{
	return( m_Cellsize > 0.0 );
}

//---------------------------------------------------------
const SG_Char * CSG_Grid_System::Get_Name(bool bShort)
{
	if( is_Valid() )
	{
		if( bShort )
		{
			m_Name.Printf(SG_T("%.*f; %dx %dy; %.*fx %.*fy"),
				SG_Get_Significant_Decimals(Get_Cellsize()),
				Get_Cellsize(),
				Get_NX(),
				Get_NY(),
				SG_Get_Significant_Decimals(Get_XMin()), Get_XMin(),
				SG_Get_Significant_Decimals(Get_YMin()), Get_YMin()
			);
		}
		else
		{
			m_Name.Printf(SG_T("%s: %f, %s: %dx/%dy, %s: %fx/%fy"),
				_TL("Cell size"),
				Get_Cellsize(),
				_TL("Number of cells"),
				Get_NX(),
				Get_NY(),
				_TL("Lower left corner"),
				Get_XMin(),
				Get_YMin()
			);
		}
	}
	else
	{
		m_Name	= _TL("<not set>");
	}

	return( m_Name );
}


///////////////////////////////////////////////////////////
//														 //
//														 //
//														 //
///////////////////////////////////////////////////////////

//---------------------------------------------------------
bool CSG_Grid_System::operator == (const CSG_Grid_System &System) const
{
	return( is_Equal(System) );
}

//---------------------------------------------------------
void CSG_Grid_System::operator = (const CSG_Grid_System &System)
{
	Assign(System);
}


///////////////////////////////////////////////////////////
//														 //
//														 //
//														 //
///////////////////////////////////////////////////////////

//---------------------------------------------------------
bool CSG_Grid_System::Assign(const CSG_Grid_System &System)
{
	return( Assign(System.m_Cellsize, System.m_Extent) );
}

//---------------------------------------------------------
bool CSG_Grid_System::Assign(double Cellsize, const CSG_Rect &Extent)
{
	return( Assign(Cellsize, Extent.m_rect.xMin, Extent.m_rect.yMin, Extent.m_rect.xMax, Extent.m_rect.yMax) );
}

//---------------------------------------------------------
bool CSG_Grid_System::Assign(double Cellsize, double xMin, double yMin, double xMax, double yMax)
{
	if( Cellsize > 0.0 && xMin < xMax && yMin < yMax )
	{
		return( Assign(Cellsize, xMin, yMin, 1 + (int)(0.5 + (xMax - xMin) / Cellsize), 1 + (int)(0.5 + (yMax - yMin) / Cellsize)) );
	}

	return( Assign(0.0, 0.0, 0.0, 0, 0) );
}

//---------------------------------------------------------
bool CSG_Grid_System::Assign(double Cellsize, double xMin, double yMin, int NX, int NY)
{
	if( Cellsize > 0.0 && NX > 0 && NY > 0 )
	{
		m_NX		= NX;
		m_NY		= NY;
		m_NCells	= (sLong)NY * NX;

		m_Cellsize	= Cellsize;
		m_Cellarea	= Cellsize * Cellsize;

		m_Extent		.Assign(
			xMin,
			yMin,
			xMin + (NX - 1.0) * Cellsize,
			yMin + (NY - 1.0) * Cellsize
		);

		m_Extent_Cells	.Assign(
			xMin - 0.5 * Cellsize,
			yMin - 0.5 * Cellsize,
			xMin + (NX - 0.5) * Cellsize,
			yMin + (NY - 0.5) * Cellsize
		);

		m_Diagonal	= Cellsize * sqrt(2.0);

		return( true );
	}

	m_Cellsize		= -1.0;
	m_NX			= 0;
	m_NY			= 0;
	m_NCells		= 0;
	m_Cellsize		= 0.0;
	m_Cellarea		= 0.0;
	m_Diagonal		= 0.0;
	m_Extent		.Assign(0.0, 0.0, 0.0, 0.0);
	m_Extent_Cells	.Assign(0.0, 0.0, 0.0, 0.0);

	return( false );
}


///////////////////////////////////////////////////////////
//														 //
//														 //
//														 //
///////////////////////////////////////////////////////////

//---------------------------------------------------------
bool CSG_Grid_System::is_Equal(const CSG_Grid_System &System) const
{
	return( is_Equal(System.m_Cellsize, System.m_Extent.m_rect) );
}

//---------------------------------------------------------
bool CSG_Grid_System::is_Equal(double Cellsize, const TSG_Rect &Extent) const
{
	return( m_Cellsize == Cellsize && m_Extent == Extent );
}


///////////////////////////////////////////////////////////
//														 //
//														 //
//														 //
///////////////////////////////////////////////////////////

//---------------------------------------------------------
CSG_Grid_Cell_Addressor::CSG_Grid_Cell_Addressor(void)
{
	m_Cells.Add_Field(SG_T("X"), SG_DATATYPE_Int);
	m_Cells.Add_Field(SG_T("Y"), SG_DATATYPE_Int);
	m_Cells.Add_Field(SG_T("D"), SG_DATATYPE_Double);
	m_Cells.Add_Field(SG_T("W"), SG_DATATYPE_Double);
}

//---------------------------------------------------------
bool CSG_Grid_Cell_Addressor::Destroy(void)
{
	m_Cells.Del_Records();

	return( true );
}


///////////////////////////////////////////////////////////
//														 //
///////////////////////////////////////////////////////////

//---------------------------------------------------------
#define	ADD_CELL(x, y, d)	{\
	CSG_Table_Record	*pRecord	= m_Cells.Add_Record();\
	pRecord->Set_Value(0, x);\
	pRecord->Set_Value(1, y);\
	pRecord->Set_Value(2, d);\
	pRecord->Set_Value(3, m_Weighting.Get_Weight(d));\
}

//---------------------------------------------------------
bool CSG_Grid_Cell_Addressor::Set_Radius(double Radius, bool bSquare)
{
	Destroy();

	//-----------------------------------------------------
	if( Radius > 0.0 )
	{
		ADD_CELL(0.0, 0.0, 0.0);

		for(double y=1.0; y<=Radius; y++)
		{
			for(double x=0.0; x<=Radius; x++)
			{
				double	d	= SG_Get_Length(x, y);

				if( bSquare || d <= Radius )
				{
					ADD_CELL( x,  y, d);
					ADD_CELL( y, -x, d);
					ADD_CELL(-x, -y, d);
					ADD_CELL(-y,  x, d);
				}
			}
		}

		//-------------------------------------------------
		if( m_Cells.Get_Count() > 0 )
		{
			m_Cells.Set_Index(2, TABLE_INDEX_Ascending);

			return( true );
		}
	}

	return( false );
}

//---------------------------------------------------------
bool CSG_Grid_Cell_Addressor::Set_Annulus(double inner_Radius, double outer_Radius)
{
	Destroy();

	//-----------------------------------------------------
	if( inner_Radius <= outer_Radius )
	{
		if( inner_Radius <= 0.0 )
		{
			ADD_CELL(0.0, 0.0, 0.0);
		}

		for(double y=1.0; y<=outer_Radius; y++)
		{
			for(double x=0.0; x<=outer_Radius; x++)
			{
				double	d	= SG_Get_Length(x, y);

				if( inner_Radius <= d && d <= outer_Radius )
				{
					ADD_CELL( x,  y, d);
					ADD_CELL( y, -x, d);
					ADD_CELL(-x, -y, d);
					ADD_CELL(-y,  x, d);
				}
			}
		}

		//-------------------------------------------------
		if( m_Cells.Get_Count() > 0 )
		{
			m_Cells.Set_Index(2, TABLE_INDEX_Ascending);

			return( true );
		}
	}

	return( false );
}

//---------------------------------------------------------
bool CSG_Grid_Cell_Addressor::Set_Sector(double Radius, double Direction, double Tolerance)
{
	Destroy();

	//-----------------------------------------------------
	if( Radius > 0.0 )
	{
		TSG_Point			a, b;
		CSG_Shapes			Polygons(SHAPE_TYPE_Polygon);	// Polygons.Add_Field(SG_T("ID"), SG_DATATYPE_Int);
		CSG_Shape_Polygon	*pPolygon	= (CSG_Shape_Polygon *)Polygons.Add_Shape();

		Direction	= fmod(Direction, M_PI_360);	if( Direction < 0.0 )	Direction	+= M_PI_360;

		if( Direction < M_PI_090 )
		{
			a.x	= -0.5;	b.x	=  0.5;
			a.y	=  0.5;	b.y	= -0.5;
		}
		else if( Direction < M_PI_180 )
		{
			a.x	=  0.5;	b.x	= -0.5;
			a.y	=  0.5;	b.y	= -0.5;
		}
		else if( Direction < M_PI_270 )
		{
			a.x	=  0.5;	b.x	= -0.5;
			a.y	= -0.5;	b.y	=  0.5;
		}
		else // if( Direction < M_PI_360 )
		{
			a.x	= -0.5;	b.x	=  0.5;
			a.y	= -0.5;	b.y	=  0.5;
		}

		double	d	= 10.0 * SG_Get_Length(Radius, Radius);

		pPolygon->Add_Point(b.x, b.y);
		pPolygon->Add_Point(a.x, a.y);
		pPolygon->Add_Point(a.x + d * sin(Direction - Tolerance), a.y + d * cos(Direction - Tolerance));
		pPolygon->Add_Point(      d * sin(Direction)            ,       d * cos(Direction));
		pPolygon->Add_Point(b.x + d * sin(Direction + Tolerance), a.y + d * cos(Direction + Tolerance));

		//-------------------------------------------------
		for(double y=1.0; y<=Radius; y++)
		{
			for(double x=0.0; x<=Radius; x++)
			{
				if( (d = SG_Get_Length(x, y)) <= Radius )
				{
					if( pPolygon->Contains( x,  y) )	ADD_CELL( x,  y, d);
					if( pPolygon->Contains( y, -x) )	ADD_CELL( y, -x, d);
					if( pPolygon->Contains(-x, -y) )	ADD_CELL(-x, -y, d);
					if( pPolygon->Contains(-y,  x) )	ADD_CELL(-y,  x, d);
				}
			}
		}

		//-------------------------------------------------
		if( m_Cells.Get_Count() > 0 )
		{
			m_Cells.Set_Index(2, TABLE_INDEX_Ascending);

			return( true );
		}
	}

	return( false );
}


///////////////////////////////////////////////////////////
//														 //
//														 //
//														 //
///////////////////////////////////////////////////////////

//---------------------------------------------------------

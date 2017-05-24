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
//                  grid_operation.cpp                   //
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
#include <memory.h>

#include "grid.h"


///////////////////////////////////////////////////////////
//														 //
//				Data Assignments - Value				 //
//														 //
///////////////////////////////////////////////////////////

//---------------------------------------------------------
void CSG_Grid::Assign_NoData(void)
{
	double	Value	= Get_NoData_Value();

	#pragma omp parallel for
	for(sLong i=0; i<Get_NCells(); i++)
	{
		Set_Value(i, Value, false);
	}
}

//---------------------------------------------------------
bool CSG_Grid::Assign(double Value)
{
	if( !is_Valid() )
	{
		return( false );
	}

	if( Value == 0.0 && m_Memory_Type == GRID_MEMORY_Normal )
	{
		int	n	= _Get_nLineBytes();

		#pragma omp parallel for
		for(int y=0; y<Get_NY(); y++)
		{
			memset(m_Values[y], 0, n);
		}
	}
	else
	{
		#pragma omp parallel for
		for(int y=0; y<Get_NY(); y++)
		{
			for(int x=0; x<Get_NX(); x++)
			{
				Set_Value(x, y, Value);
			}
		}
	}

	//-----------------------------------------------------
	Get_History().Destroy();
	Get_History().Add_Child(SG_T("GRID_OPERATION"), Value)->Add_Property(SG_T("NAME"), _TL("Assign"));

	m_Statistics.Invalidate();

	Set_Update_Flag(false);

	return( true );
}


///////////////////////////////////////////////////////////
//														 //
//				Data Assignments - Grid					 //
//														 //
///////////////////////////////////////////////////////////

//---------------------------------------------------------
bool CSG_Grid::Assign(CSG_Data_Object *pObject)
{
	return( pObject && pObject->is_Valid() && pObject->Get_ObjectType() == Get_ObjectType()
		&&  Assign((CSG_Grid *)pObject, GRID_RESAMPLING_Undefined) );
}

bool CSG_Grid::Assign(CSG_Grid *pGrid, TSG_Grid_Resampling Interpolation)
{
	//-----------------------------------------------------
	if(	!is_Valid() || !pGrid || !pGrid->is_Valid() || is_Intersecting(pGrid->Get_Extent()) == INTERSECTION_None )
	{
		return( false );
	}

	bool	bResult	= false;

	//---------------------------------------------------------
	if( m_System == pGrid->m_System )
	{
		for(int y=0; y<Get_NY() && SG_UI_Process_Set_Progress(y, Get_NY()); y++)
		{
			#pragma omp parallel for
			for(int x=0; x<Get_NX(); x++)
			{
				if( pGrid->is_NoData(x, y) )
				{
					Set_NoData(x, y);
				}
				else
				{
					Set_Value(x, y, pGrid->asDouble(x, y));
				}
			}
		}

		bResult	= true;
	}

	else if(  Get_Cellsize() == pGrid->Get_Cellsize()	// No-Scaling...
	&&	fmod(Get_XMin() - pGrid->Get_XMin(), Get_Cellsize()) == 0.0
	&&	fmod(Get_YMin() - pGrid->Get_YMin(), Get_Cellsize()) == 0.0	)
	{
		bResult	= _Assign_Interpolated(pGrid, GRID_RESAMPLING_NearestNeighbour);
	}

	//---------------------------------------------------------
	else switch( Interpolation )
	{
	case GRID_RESAMPLING_NearestNeighbour:
	case GRID_RESAMPLING_Bilinear:
	case GRID_RESAMPLING_BicubicSpline:
	case GRID_RESAMPLING_BSpline:
		bResult	= _Assign_Interpolated(pGrid, Interpolation);
		break;

	case GRID_RESAMPLING_Mean_Nodes:
	case GRID_RESAMPLING_Mean_Cells:
		bResult	= _Assign_MeanValue   (pGrid, Interpolation != GRID_RESAMPLING_Mean_Nodes);
		break;

	case GRID_RESAMPLING_Minimum:
	case GRID_RESAMPLING_Maximum:
		bResult	= _Assign_ExtremeValue(pGrid, Interpolation == GRID_RESAMPLING_Maximum);
		break;

	case GRID_RESAMPLING_Majority:
		bResult	= _Assign_Majority    (pGrid);
		break;

	default:
		if( Get_Cellsize() < pGrid->Get_Cellsize() )	// Down-Scaling...
		{
			bResult	= _Assign_Interpolated(pGrid, GRID_RESAMPLING_BSpline);
		}
		else											// Up-Scaling...
		{
			bResult	= _Assign_MeanValue(pGrid, Interpolation != GRID_RESAMPLING_Mean_Nodes);
		}
		break;
	}

	//---------------------------------------------------------
	if( bResult )
	{
		Set_Unit(pGrid->Get_Unit());

		if( pGrid->Get_Projection().is_Okay() )
		{
			Get_Projection()	= pGrid->Get_Projection();
		}

		Get_History()	= pGrid->Get_History();
	}

	//---------------------------------------------------------
	SG_UI_Process_Set_Ready();

	return( bResult );
}

//---------------------------------------------------------
bool CSG_Grid::_Assign_Interpolated(CSG_Grid *pGrid, TSG_Grid_Resampling Interpolation)
{
	double	py	= Get_YMin();

	for(int y=0; y<Get_NY() && SG_UI_Process_Set_Progress(y, Get_NY()); y++, py+=Get_Cellsize())
	{
		#pragma omp parallel for
		for(int x=0; x<Get_NX(); x++)
		{
			double	z;

			if( pGrid->Get_Value(Get_XMin() + x * Get_Cellsize(), py, z, Interpolation) )
			{
				Set_Value(x, y, z);
			}
			else
			{
				Set_NoData(x, y);
			}
		}
	}

	return( true );
}

//---------------------------------------------------------
bool CSG_Grid::_Assign_ExtremeValue(CSG_Grid *pGrid, bool bMaximum)
{
	if( Get_Cellsize() < pGrid->Get_Cellsize() )
	{
		return( false );
	}

	//-----------------------------------------------------
	Assign_NoData();

	double	ax	= 0.5 + (pGrid->Get_XMin() - Get_XMin()) / Get_Cellsize();
	double	py	= 0.5 + (pGrid->Get_YMin() - Get_YMin()) / Get_Cellsize();

	double	d	= pGrid->Get_Cellsize() / Get_Cellsize();

	for(int y=0; y<pGrid->Get_NY() && SG_UI_Process_Set_Progress(y, pGrid->Get_NY()); y++, py+=d)
	{
		int	iy	= (int)floor(py);

		if( iy >= 0 && iy < Get_NY() )
		{
			#pragma omp parallel for
			for(int x=0; x<pGrid->Get_NX(); x++)
			{
				if( !pGrid->is_NoData(x, y) )
				{
					int	ix	= (int)floor(ax + x * d);
					
					if( ix >= 0 && ix < Get_NX() )
					{
						double	z	= pGrid->asDouble(x, y);

						if( is_NoData(ix, iy)
						||	(bMaximum ==  true && z > asDouble(ix, iy))
						||	(bMaximum == false && z < asDouble(ix, iy)) )
						{
							Set_Value(ix, iy, z);
						}
					}
				}
			}
		}
	}

	//-----------------------------------------------------
	return( true );
}

//---------------------------------------------------------
bool CSG_Grid::_Assign_MeanValue(CSG_Grid *pGrid, bool bAreaProportional)
{
	if( Get_Cellsize() < pGrid->Get_Cellsize() )
	{
		return( false );
	}

	//-----------------------------------------------------
	double	d	= Get_Cellsize() / pGrid->Get_Cellsize();

	double	ox	= (Get_XMin(true) - pGrid->Get_XMin()) / pGrid->Get_Cellsize();
	double	py	= (Get_YMin(true) - pGrid->Get_YMin()) / pGrid->Get_Cellsize();

	for(int y=0; y<Get_NY() && SG_UI_Process_Set_Progress(y, Get_NY()); y++, py+=d)
	{
		int		ay	= (int)(bAreaProportional ? floor(py    ) : ceil (py    ));
		int		by	= (int)(bAreaProportional ? ceil (py + d) : floor(py + d));

		//-------------------------------------------------
		#pragma omp parallel for
		for(int x=0; x<Get_NX(); x++)
		{
			double	px	= ox + x * d;

			int		ax	= (int)(bAreaProportional ? floor(px    ) : ceil (px    ));
			int		bx	= (int)(bAreaProportional ? ceil (px + d) : floor(px + d));

			CSG_Rect	rMean(px, py, px + d, py + d);

			CSG_Simple_Statistics	s;

			for(int iy=ay; iy<=by; iy++)
			{
				if( iy >= 0 && iy < pGrid->Get_NY() )
				{
					for(int ix=ax; ix<=bx; ix++)
					{
						if( ix >= 0 && ix < pGrid->Get_NX() && !pGrid->is_NoData(ix, iy) )
						{
							if( bAreaProportional )
							{
								CSG_Rect	r(ix - 0.5, iy - 0.5, ix + 0.5, iy + 0.5);

								if( r.Intersect(rMean) )
								{
									s.Add_Value(pGrid->asDouble(ix, iy), r.Get_Area());
								}
							}
							else
							{
								s.Add_Value(pGrid->asDouble(ix, iy));
							}
						}
					}
				}
			}

			//---------------------------------------------
			if( s.Get_Count() > 0 )
			{
				Set_Value(x, y, s.Get_Mean());
			}
			else
			{
				Set_NoData(x, y);
			}
		}
	}

	//-----------------------------------------------------
	return( true );
}


///////////////////////////////////////////////////////////
//														 //
///////////////////////////////////////////////////////////

//---------------------------------------------------------
bool CSG_Grid::_Assign_Majority(CSG_Grid *pGrid)
{
	if( Get_Cellsize() < pGrid->Get_Cellsize() )
	{
		return( false );
	}

	//-----------------------------------------------------
	CSG_Class_Statistics	m;

	Set_NoData_Value(pGrid->Get_NoData_Value());

	Assign_NoData();

	//-----------------------------------------------------
	int	ay, by	= (int)(1.0 + (((0 - 0.5) * Get_Cellsize() + Get_YMin()) - pGrid->Get_YMin()) / pGrid->Get_Cellsize());

	for(int y=0; y<Get_NY() && SG_UI_Process_Set_Progress(y, Get_NY()); y++)
	{
		ay	= by;
		by	= (int)(1.0 + (((y + 0.5) * Get_Cellsize() + Get_YMin()) - pGrid->Get_YMin()) / pGrid->Get_Cellsize());

		if( ay < pGrid->Get_NY() && by > 0 )
		{
			if( ay < 0 )
			{
				ay	= 0;
			}

			if( by > pGrid->Get_NY() )
			{
				by	= pGrid->Get_NY();
			}

			int	ax, bx	= (int)(1.0 + (((0 - 0.5) * Get_Cellsize() + Get_XMin()) - pGrid->Get_XMin()) / pGrid->Get_Cellsize());

			for(int x=0; x<Get_NX(); x++)
			{
				ax	= bx;
				bx	= (int)(1.0 + (((x + 0.5) * Get_Cellsize() + Get_XMin()) - pGrid->Get_XMin()) / pGrid->Get_Cellsize());

				if( ax < pGrid->Get_NX() && bx > 0 )
				{
					m.Reset();

					if( ax < 0 )
					{
						ax	= 0;
					}

					if( bx > pGrid->Get_NX() )
					{
						bx	= pGrid->Get_NX();
					}

					for(int iy=ay; iy<by; iy++)
					{
						for(int ix=ax; ix<bx; ix++)
						{
							if( !pGrid->is_NoData(ix, iy) )
							{
								m.Add_Value(pGrid->asDouble(ix, iy));
							}
						}
					}

					int		n;
					double	z;

					if( m.Get_Majority(z, n) )//&& n > 1 )
					{
						Set_Value(x, y, z);
					}
				}
			}
		}
	}

	//-----------------------------------------------------
	return( true );
}


///////////////////////////////////////////////////////////
//														 //
//					Operatoren							 //
//														 //
///////////////////////////////////////////////////////////

//---------------------------------------------------------
CSG_Grid & CSG_Grid::operator = (const CSG_Grid &Grid)
{
	Assign((CSG_Grid *)&Grid, GRID_RESAMPLING_Undefined);

	return( *this );
}

CSG_Grid & CSG_Grid::operator =	(double Value)
{
	Assign(Value);

	return( *this );
}

//---------------------------------------------------------
CSG_Grid CSG_Grid::operator +		(const CSG_Grid &Grid) const
{
	CSG_Grid	g(*this);

	return( g._Operation_Arithmetic(Grid, GRID_OPERATION_Addition) );
}

CSG_Grid CSG_Grid::operator +		(double Value) const
{
	CSG_Grid	g(*this);

	return( g._Operation_Arithmetic(Value, GRID_OPERATION_Addition) );
}

CSG_Grid & CSG_Grid::operator +=	(const CSG_Grid &Grid)
{
	return( _Operation_Arithmetic(Grid, GRID_OPERATION_Addition) );
}

CSG_Grid & CSG_Grid::operator +=	(double Value)
{
	return( _Operation_Arithmetic(Value, GRID_OPERATION_Addition) );
}

CSG_Grid & CSG_Grid::Add			(const CSG_Grid &Grid)
{
	return( _Operation_Arithmetic(Grid, GRID_OPERATION_Addition) );
}

CSG_Grid & CSG_Grid::Add			(double Value)
{
	return( _Operation_Arithmetic(Value, GRID_OPERATION_Addition) );
}

//---------------------------------------------------------
CSG_Grid CSG_Grid::operator -		(const CSG_Grid &Grid) const
{
	CSG_Grid	g(*this);

	return( g._Operation_Arithmetic(Grid, GRID_OPERATION_Subtraction) );
}

CSG_Grid CSG_Grid::operator -		(double Value) const
{
	CSG_Grid	g(*this);

	return( g._Operation_Arithmetic(Value, GRID_OPERATION_Subtraction) );
}

CSG_Grid & CSG_Grid::operator -=	(const CSG_Grid &Grid)
{
	return( _Operation_Arithmetic(Grid, GRID_OPERATION_Subtraction) );
}

CSG_Grid & CSG_Grid::operator -=	(double Value)
{
	return( _Operation_Arithmetic(Value, GRID_OPERATION_Subtraction) );
}

CSG_Grid & CSG_Grid::Subtract		(const CSG_Grid &Grid)
{
	return( _Operation_Arithmetic(Grid, GRID_OPERATION_Subtraction) );
}

CSG_Grid & CSG_Grid::Subtract		(double Value)
{
	return( _Operation_Arithmetic(Value, GRID_OPERATION_Subtraction) );
}

//---------------------------------------------------------
CSG_Grid CSG_Grid::operator *		(const CSG_Grid &Grid) const
{
	CSG_Grid	g(*this);

	return( g._Operation_Arithmetic(Grid, GRID_OPERATION_Multiplication) );
}

CSG_Grid CSG_Grid::operator *		(double Value) const
{
	CSG_Grid	g(*this);

	return( g._Operation_Arithmetic(Value, GRID_OPERATION_Multiplication) );
}

CSG_Grid & CSG_Grid::operator *=	(const CSG_Grid &Grid)
{
	return( _Operation_Arithmetic(Grid, GRID_OPERATION_Multiplication) );
}

CSG_Grid & CSG_Grid::operator *=	(double Value)
{
	return( _Operation_Arithmetic(Value, GRID_OPERATION_Multiplication) );
}

CSG_Grid & CSG_Grid::Multiply		(const CSG_Grid &Grid)
{
	return( _Operation_Arithmetic(Grid, GRID_OPERATION_Multiplication) );
}

CSG_Grid & CSG_Grid::Multiply		(double Value)
{
	return( _Operation_Arithmetic(Value, GRID_OPERATION_Multiplication) );
}

//---------------------------------------------------------
CSG_Grid CSG_Grid::operator /		(const CSG_Grid &Grid) const
{
	CSG_Grid	g(*this);

	return( g._Operation_Arithmetic(Grid, GRID_OPERATION_Division) );
}

CSG_Grid CSG_Grid::operator /		(double Value) const
{
	CSG_Grid	g(*this);

	return( g._Operation_Arithmetic(Value, GRID_OPERATION_Division) );
}

CSG_Grid & CSG_Grid::operator /=	(const CSG_Grid &Grid)
{
	return( _Operation_Arithmetic(Grid, GRID_OPERATION_Division) );
}

CSG_Grid & CSG_Grid::operator /=	(double Value)
{
	return( _Operation_Arithmetic(Value, GRID_OPERATION_Division) );
}

CSG_Grid & CSG_Grid::Divide			(const CSG_Grid &Grid)
{
	return( _Operation_Arithmetic(Grid, GRID_OPERATION_Division) );
}

CSG_Grid & CSG_Grid::Divide			(double Value)
{
	return( _Operation_Arithmetic(Value, GRID_OPERATION_Division) );
}


///////////////////////////////////////////////////////////
//														 //
//					Operatoren							 //
//														 //
///////////////////////////////////////////////////////////

//---------------------------------------------------------
CSG_Grid & CSG_Grid::_Operation_Arithmetic(const CSG_Grid &Grid, TSG_Grid_Operation Operation)
{
	if( is_Intersecting(Grid.Get_Extent()) )
	{
		TSG_Grid_Resampling	Interpolation	=
			Get_Cellsize() == Grid.Get_Cellsize() && fmod(Get_XMin() - Grid.Get_XMin(), Get_Cellsize()) == 0.0
		&&	Get_Cellsize() == Grid.Get_Cellsize() && fmod(Get_YMin() - Grid.Get_YMin(), Get_Cellsize()) == 0.0
		?	GRID_RESAMPLING_NearestNeighbour
		:	GRID_RESAMPLING_BSpline;

		for(int y=0; y<Get_NY() && SG_UI_Process_Set_Progress(y, Get_NY()); y++)
		{
			double	yWorld	= Get_YMin() + y * Get_Cellsize();

			#pragma omp parallel for
			for(int x=0; x<Get_NX(); x++)
			{
				if( !is_NoData(x, y) )
				{
					double	xWorld	= Get_XMin() + x * Get_Cellsize(), Value;

					if( Grid.Get_Value(xWorld, yWorld, Value, Interpolation) )
					{
						switch( Operation )
						{
						case GRID_OPERATION_Addition      :	Add_Value(x, y,  Value);	break;
						case GRID_OPERATION_Subtraction   :	Add_Value(x, y, -Value);	break;
						case GRID_OPERATION_Multiplication:	Mul_Value(x, y,  Value);	break;
						case GRID_OPERATION_Division      :
							if( Value != 0.0 )
							{
								Mul_Value(x, y, 1.0 / Value);
							}
							else
							{
								Set_NoData(x, y);
							}
							break;
						}
					}
				}
			}
		}

		SG_UI_Process_Set_Ready();

		//-------------------------------------------------
		CSG_String	Name;

		switch( Operation )
		{
		case GRID_OPERATION_Addition      :	Name	= _TL("Addition"      );	break;
		case GRID_OPERATION_Subtraction   :	Name	= _TL("Subtraction"   );	break;
		case GRID_OPERATION_Multiplication:	Name	= _TL("Multiplication");	break;
		case GRID_OPERATION_Division      :	Name	= _TL("Division"      );	break;
		}

		Get_History().Add_Child("GRID_OPERATION", Grid.Get_Name())->Add_Property("NAME", Name);

		Get_History().Add_Children(((CSG_Grid *)&Grid)->Get_History());
	}

	return( *this );
}

//---------------------------------------------------------
CSG_Grid & CSG_Grid::_Operation_Arithmetic(double Value, TSG_Grid_Operation Operation)
{
	//-----------------------------------------------------
	CSG_String	Name;

	switch( Operation )
	{
	case GRID_OPERATION_Addition      :
		Name	=  _TL("Addition");
		if( Value == 0.0 )
			return( *this );
		break;

	case GRID_OPERATION_Subtraction   :
		Name	=  _TL("Subtraction");
		if( Value == 0.0 )
			return( *this );
		Value	= -Value;
		break;

	case GRID_OPERATION_Multiplication:
		Name	=  _TL("Multiplication");
		if( Value == 1.0 )
			return( *this );
		break;

	case GRID_OPERATION_Division      :
		Name	=  _TL("Division");
		if( Value == 0.0 )
			return( *this );
		Value	= 1.0 / Value;
		break;
	}

	Get_History().Add_Child("GRID_OPERATION", Value)->Add_Property("NAME", Name);

	//-----------------------------------------------------
	#pragma omp parallel for
	for(int y=0; y<Get_NY(); y++)
	{
		for(int x=0; x<Get_NX(); x++)
		{
			if( !is_NoData(x, y) )
			{
				switch( Operation )
				{
				case GRID_OPERATION_Addition      :
				case GRID_OPERATION_Subtraction   :	Add_Value(x, y, Value);	break;

				case GRID_OPERATION_Multiplication:
				case GRID_OPERATION_Division      :	Mul_Value(x, y, Value);	break;
				}
			}
		}
	}

	//-----------------------------------------------------
	return( *this );
}


///////////////////////////////////////////////////////////
//														 //
//					Grid-Operations - A					 //
//														 //
///////////////////////////////////////////////////////////

//---------------------------------------------------------
void CSG_Grid::Invert(void)
{
	if( is_Valid() && Get_Range() > 0.0 )
	{
		double	Min	= Get_Min();
		double	Max	= Get_Max();

		#pragma omp parallel for
		for(int y=0; y<Get_NY(); y++)
		{
			for(int x=0; x<Get_NX(); x++)
			{
				if( !is_NoData(x, y) )
				{
					Set_Value(x, y, Max - (asDouble(x, y) - Min));
				}
			}
		}

		Get_History().Add_Child("GRID_OPERATION", _TL("Inverted"));
	}
}

//---------------------------------------------------------
void CSG_Grid::Flip(void)
{
	if( is_Valid() )
	{
		#pragma omp parallel for
		for(int x=0; x<Get_NX(); x++)
		{
			for(int yA=0, yB=Get_NY()-1; yA<yB; yA++, yB--)
			{
				double	d	   = asDouble(x, yA);
				Set_Value(x, yA, asDouble(x, yB));
				Set_Value(x, yB, d);
			}
		}

		Get_History().Add_Child("GRID_OPERATION", _TL("Mirrored vertically"));
	}
}

//---------------------------------------------------------
void CSG_Grid::Mirror(void)
{
	if( is_Valid() )
	{
		#pragma omp parallel for
		for(int y=0; y<Get_NY(); y++)
		{
			for(int xA=0, xB=Get_NX()-1; xA<xB; xA++, xB--)
			{
				double	d	   = asDouble(xA, y);
				Set_Value(xA, y, asDouble(xB, y));
				Set_Value(xB, y, d);
			}
		}

		Get_History().Add_Child("GRID_OPERATION", _TL("Mirrored horizontally"));
	}
}


///////////////////////////////////////////////////////////
//														 //
//					Grid-Operations - B					 //
//														 //
///////////////////////////////////////////////////////////

//---------------------------------------------------------
bool CSG_Grid::Normalise(void)
{
	if( is_Valid() && Get_Range() > 0.0 )
	{
		double	Min		= Get_Min  ();
		double	Range	= Get_Range();

		#pragma omp parallel for
		for(int y=0; y<Get_NY(); y++)
		{
			for(int x=0; x<Get_NX(); x++)
			{
				if( !is_NoData(x, y) )
				{
					Set_Value(x, y, (asDouble(x, y) - Min) / Range);
				}
			}
		}

		Get_History().Add_Child("GRID_OPERATION", _TL("Normalisation"));

		return( true );
	}

	return( false );
}

//---------------------------------------------------------
bool CSG_Grid::DeNormalise(double Minimum, double Maximum)
{
	if( is_Valid() && Minimum < Maximum )
	{
		#pragma omp parallel for
		for(int y=0; y<Get_NY(); y++)
		{
			for(int x=0; x<Get_NX(); x++)
			{
				if( !is_NoData(x, y) )
				{
					Set_Value(x, y, Minimum + asDouble(x, y) * (Maximum - Minimum));
				}
			}
		}

		Get_History().Add_Child("GRID_OPERATION", _TL("Denormalisation"));

		return( true );
	}

	return( false );
}

//---------------------------------------------------------
bool CSG_Grid::Standardise(void)
{
	if( is_Valid() && Get_StdDev() > 0.0 )
	{
		double	Mean	= Get_Mean  ();
		double	StdDev	= Get_StdDev();

		#pragma omp parallel for
		for(int y=0; y<Get_NY(); y++)
		{
			for(int x=0; x<Get_NX(); x++)
			{
				if( !is_NoData(x, y) )
				{
					Set_Value(x, y, (asDouble(x, y) - Mean) / StdDev);
				}
			}
		}

		Get_History().Add_Child("GRID_OPERATION", _TL("Standardisation"));

		return( true );
	}

	return( false );
}

//---------------------------------------------------------
bool CSG_Grid::DeStandardise(double Mean, double StdDev)
{
	if( is_Valid() && StdDev > 0.0 )
	{
		#pragma omp parallel for
		for(int y=0; y<Get_NY(); y++)
		{
			for(int x=0; x<Get_NX(); x++)
			{
				if( !is_NoData(x, y) )
				{
					Set_Value(x, y, Mean + asDouble(x, y) * StdDev);
				}
			}
		}

		Get_History().Add_Child("GRID_OPERATION", _TL("Destandardisation"));

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
/**
  * Returns the direction to which the downward gradient is
  * steepest. This implements the Deterministic 8 (D8) algorithm
  * for identifying a single flow direction based on elevation
  * data. Direction is numbered clock-wise beginning with 0 for
  * the North. If no direction can be identified result will be a -1.
  * If 'bDown' is not true the cell that the direction is pointing
  * to might have a higher value than the center cell. If 'bNoEdges'
  * is true a -1 will be returned for all cells that are at the
  * edge of the data area.
*/
//---------------------------------------------------------
int CSG_Grid::Get_Gradient_NeighborDir(int x, int y, bool bDown, bool bNoEdges)	const
{
	int	Direction	= -1;

	if( is_InGrid(x, y) )
	{
		double	z	= asDouble(x, y), dzMax	= 0.0;

		for(int i=0; i<8; i++)
		{
			int	ix	= m_System.Get_xTo(i, x);
			int	iy	= m_System.Get_yTo(i, y);

			if( is_InGrid(ix, iy) )
			{
				double	dz	= (z - asDouble(ix, iy)) / m_System.Get_Length(i);

				if( (!bDown || dz > 0.0) && (Direction < 0 || dzMax < dz) )
				{
					dzMax		= dz;
					Direction	= i;
				}
			}
			else if( bNoEdges )
			{
				return( -1 );
			}
		}
	}

	return( Direction );
}

//---------------------------------------------------------
/**
  * Calculates the gradient of a cell interpreting all grid cell values
  * as elevation surface. Calculation uses the formulas proposed
  * by Zevenbergen & Thorne (1986). Slope and aspect values are calulated
  * in radians. Aspect is zero for the North direction and increases
  * clockwise.
*/
//---------------------------------------------------------
bool CSG_Grid::Get_Gradient(int x, int y, double &Slope, double &Aspect) const
{
	if( is_InGrid(x, y) )
	{
		double	z	= asDouble(x, y), dz[4];

		for(int i=0, iDir=0, ix, iy; i<4; i++, iDir+=2)
		{
			if( is_InGrid(
				ix = m_System.Get_xTo  (iDir, x),
				iy = m_System.Get_yTo  (iDir, y)) )
			{
				dz[i]	= asDouble(ix, iy) - z;
			}
			else if( is_InGrid(
				ix = m_System.Get_xFrom(iDir, x),
				iy = m_System.Get_yFrom(iDir, y)) )
			{
				dz[i]	= z - asDouble(ix, iy);
			}
			else
			{
				dz[i]	= 0.0;
			}
		}

		double G	= (dz[0] - dz[2]) / (2.0 * Get_Cellsize());
        double H	= (dz[1] - dz[3]) / (2.0 * Get_Cellsize());

		Slope	= atan(sqrt(G*G + H*H));
		Aspect	= G != 0.0 ? M_PI_180 + atan2(H, G) : H > 0.0 ? M_PI_270 : H < 0.0 ? M_PI_090 : -1.0;

		return( true );
	}

	Slope	=  0.0;
	Aspect	= -1.0;

	return( false );
}

//---------------------------------------------------------
/**
  * Calculates the gradient for the given world coordinate.
  * Calculation uses the formulas proposed by Zevenbergen & Thorne (1986).
  * Slope and aspect values are calulated in radians.
  * Aspect is zero for the North direction and increases clockwise.
*/
//---------------------------------------------------------
bool CSG_Grid::Get_Gradient(double x, double y, double &Slope, double &Aspect, TSG_Grid_Resampling Interpolation) const
{
	double	z, iz, dz[4];

	if( Get_Value(x, y, z, Interpolation) )
	{
		for(int i=0, iDir=0; i<4; i++, iDir+=2)
		{
			if( Get_Value(
				x + Get_Cellsize() * m_System.Get_xTo  (iDir),
				y + Get_Cellsize() * m_System.Get_yTo  (iDir), iz, Interpolation) )
			{
				dz[i]	= iz - z;
			}
			else if( Get_Value(
				x + Get_Cellsize() * m_System.Get_xFrom(iDir),
				y + Get_Cellsize() * m_System.Get_yFrom(iDir), iz, Interpolation) )
			{
				dz[i]	= z - iz;
			}
			else
			{
				dz[i]	= 0.0;
			}
		}

		double G	= (dz[0] - dz[2]) / (2.0 * Get_Cellsize());
        double H	= (dz[1] - dz[3]) / (2.0 * Get_Cellsize());

		Slope	= atan(sqrt(G*G + H*H));
		Aspect	= G != 0.0 ? M_PI_180 + atan2(H, G) : H > 0.0 ? M_PI_270 : H < 0.0 ? M_PI_090 : -1.0;

		return( true );
	}

	Slope	=  0.0;
	Aspect	= -1.0;

	return( false );
}

//---------------------------------------------------------
/**
  * Calculates the gradient for the given world coordinate.
  * Calculation uses the formulas proposed by Zevenbergen & Thorne (1986).
  * Slope and aspect values are calulated in radians.
  * Aspect is zero for the North direction and increases clockwise.
*/
//---------------------------------------------------------
bool CSG_Grid::Get_Gradient(const TSG_Point &p, double &Incline, double &Azimuth, TSG_Grid_Resampling Interpolation) const
{
	return( Get_Gradient(p.x, p.y, Incline, Azimuth, Interpolation) );
}


///////////////////////////////////////////////////////////
//														 //
//														 //
//														 //
///////////////////////////////////////////////////////////

//---------------------------------------------------------

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
	Assign(Get_NoData_Value());
}

//---------------------------------------------------------
bool CSG_Grid::Assign(double Value)
{
	if( is_Valid() )
	{
		if( Value == 0.0 && m_Memory_Type == GRID_MEMORY_Normal )
		{
			for(int n=0, m=_Get_nLineBytes(); n<Get_NY(); n++)
			{
				memset(m_Values[n], 0, m);
			}
		}
		else
		{
			for(long n=0; n<Get_NCells(); n++)
			{
				Set_Value(n, Value);
			}
		}

		//-------------------------------------------------
		Get_History().Destroy();
		Get_History().Add_Child(SG_T("GRID_OPERATION"), Value)->Add_Property(SG_T("NAME"), _TL("Assign"));

		//-------------------------------------------------
		m_zStats.Invalidate();

		Set_Update_Flag(false);

		return( true );
	}

	return( false );
}


///////////////////////////////////////////////////////////
//														 //
//				Data Assignments - Grid					 //
//														 //
///////////////////////////////////////////////////////////

//---------------------------------------------------------
bool CSG_Grid::Assign(CSG_Data_Object *pObject)
{
	if( pObject && pObject->is_Valid() && pObject->Get_ObjectType() == Get_ObjectType() && Assign((CSG_Grid *)pObject, GRID_INTERPOLATION_Undefined) )
	{
		if( pObject->Get_Projection().is_Okay() )
		{
			Get_Projection()	= pObject->Get_Projection();
		}

		return( true );
	}

	return( false );
}

bool CSG_Grid::Assign(CSG_Grid *pGrid, TSG_Grid_Interpolation Interpolation)
{
	bool	bResult	= false;

	//-----------------------------------------------------
	if(	is_Valid() && pGrid && pGrid->is_Valid() && is_Intersecting(pGrid->Get_Extent()) != INTERSECTION_None )
	{
		if(	Get_Cellsize() == pGrid->Get_Cellsize()			// No-Scaling...
		&&	fmod(Get_XMin() - pGrid->Get_XMin(), Get_Cellsize()) == 0.0
		&&	fmod(Get_YMin() - pGrid->Get_YMin(), Get_Cellsize()) == 0.0	)
		{
			bResult	= _Assign_Interpolated	(pGrid, GRID_INTERPOLATION_NearestNeighbour);
		}
		else switch( Interpolation )
		{
			case GRID_INTERPOLATION_NearestNeighbour:
			case GRID_INTERPOLATION_Bilinear:
			case GRID_INTERPOLATION_InverseDistance:
			case GRID_INTERPOLATION_BicubicSpline:
			case GRID_INTERPOLATION_BSpline:
				bResult	= _Assign_Interpolated	(pGrid, Interpolation);
				break;

			case GRID_INTERPOLATION_Mean_Nodes:
			case GRID_INTERPOLATION_Mean_Cells:
				bResult	= _Assign_MeanValue		(pGrid, Interpolation != GRID_INTERPOLATION_Mean_Nodes);
				break;

			case GRID_INTERPOLATION_Minimum:
			case GRID_INTERPOLATION_Maximum:
				bResult	= _Assign_ExtremeValue	(pGrid, Interpolation == GRID_INTERPOLATION_Maximum);
				break;

			case GRID_INTERPOLATION_Majority:
				bResult	= _Assign_Majority		(pGrid);
				break;

			default:
				if( Get_Cellsize() < pGrid->Get_Cellsize() )	// Down-Scaling...
				{
					bResult	= _Assign_Interpolated	(pGrid, GRID_INTERPOLATION_BSpline);
				}
				else											// Up-Scaling...
				{
					bResult	= _Assign_MeanValue		(pGrid, Interpolation != GRID_INTERPOLATION_Mean_Nodes);
				}
				break;
		}

		//-------------------------------------------------
		if( bResult )
		{
//			Set_Name				(pGrid->Get_Name());
			Set_Description			(pGrid->Get_Description());
			Set_Unit				(pGrid->Get_Unit());
			Set_ZFactor				(pGrid->Get_ZFactor());
			Set_NoData_Value_Range	(pGrid->Get_NoData_Value(), pGrid->Get_NoData_hiValue());
		}
	}

	//-----------------------------------------------------
	return( bResult );
}

//---------------------------------------------------------
bool CSG_Grid::_Assign_Interpolated(CSG_Grid *pGrid, TSG_Grid_Interpolation Interpolation)
{
	int		x, y;
	double	yPosition;

	Set_NoData_Value_Range(pGrid->Get_NoData_Value(), pGrid->Get_NoData_hiValue());

	for(y=0, yPosition=Get_YMin(); y<Get_NY() && SG_UI_Process_Set_Progress(y, Get_NY()); y++, yPosition+=Get_Cellsize())
	{
		#pragma omp parallel for private(x)
		for(x=0; x<Get_NX(); x++)
		{
			double	z;

			if( pGrid->Get_Value(Get_XMin() + x * Get_Cellsize(), yPosition, z, Interpolation) )
			{
				Set_Value (x, y, z);
			}
			else
			{
				Set_NoData(x, y);
			}
		}
	}

	Get_History()	= pGrid->Get_History();
	Get_History().Add_Child(SG_T("GRID_OPERATION"), CSG_String::Format(SG_T("%f -> %f"), pGrid->Get_Cellsize(), Get_Cellsize()))->Add_Property(SG_T("NAME"), _TL("Resampling"));

	SG_UI_Process_Set_Ready();

	return( true );
}

//---------------------------------------------------------
bool CSG_Grid::_Assign_ExtremeValue(CSG_Grid *pGrid, bool bMaximum)
{
	if( Get_Cellsize() < pGrid->Get_Cellsize() || is_Intersecting(pGrid->Get_Extent()) == INTERSECTION_None )
	{
		return( false );
	}

	//-----------------------------------------------------
	int			x, y, ix, iy;
	double		px, py, ax, ay, d, z;
	CSG_Matrix	S(Get_NY(), Get_NX()), N(Get_NY(), Get_NX());

	d	= pGrid->Get_Cellsize() / Get_Cellsize();

	Set_NoData_Value(pGrid->Get_NoData_Value());

	Assign_NoData();

	//-----------------------------------------------------
	ax	= 0.5 + (pGrid->Get_XMin() - Get_XMin()) / Get_Cellsize();
	ay	= 0.5 + (pGrid->Get_YMin() - Get_YMin()) / Get_Cellsize();

	for(y=0, py=ay; y<pGrid->Get_NY() && SG_UI_Process_Set_Progress(y, pGrid->Get_NY()); y++, py+=d)
	{
		if( (iy = (int)floor(py)) >= 0 && iy < Get_NY() )
		{
			for(x=0, px=ax; x<pGrid->Get_NX(); x++, px+=d)
			{
				if( !pGrid->is_NoData(x, y) && (ix = (int)floor(px)) >= 0 && ix < Get_NX() )
				{
					z	= pGrid->asDouble(x, y);

					if( is_NoData(ix, iy)
					||	(bMaximum == true  && z > asDouble(ix, iy))
					||	(bMaximum == false && z < asDouble(ix, iy)) )
					{
						Set_Value(ix, iy, z);
					}
				}
			}
		}
	}

	//-----------------------------------------------------
	Get_History()	= pGrid->Get_History();
	Get_History().Add_Child(SG_T("GRID_OPERATION"), CSG_String::Format(SG_T("%f -> %f"), pGrid->Get_Cellsize(), Get_Cellsize()))->Add_Property(SG_T("NAME"), _TL("Resampling"));

	SG_UI_Process_Set_Ready();

	return( true );
}

//---------------------------------------------------------
bool CSG_Grid::_Assign_MeanValue(CSG_Grid *pGrid, bool bAreaProportional)
{
	if( Get_Cellsize() < pGrid->Get_Cellsize() || is_Intersecting(pGrid->Get_Extent()) == INTERSECTION_None )
	{
		return( false );
	}

	//-----------------------------------------------------
	int			x, y, ix, iy, jx, jy;
	double		px, py, ax, ay, d, w, wx, wy, z;
	CSG_Matrix	S(Get_NY(), Get_NX()), N(Get_NY(), Get_NX());

	d	= pGrid->Get_Cellsize() / Get_Cellsize();

	Set_NoData_Value(pGrid->Get_NoData_Value());

	Assign_NoData();

	//-----------------------------------------------------
	if( bAreaProportional == false )
	{
		ax	= 0.5 + (pGrid->Get_XMin() - Get_XMin()) / Get_Cellsize();
		ay	= 0.5 + (pGrid->Get_YMin() - Get_YMin()) / Get_Cellsize();

		for(y=0, py=ay; y<pGrid->Get_NY() && SG_UI_Process_Set_Progress(y, pGrid->Get_NY()); y++, py+=d)
		{
			if( (iy = (int)floor(py)) >= 0 && iy < Get_NY() )
			{
				for(x=0, px=ax; x<pGrid->Get_NX(); x++, px+=d)
				{
					if( !pGrid->is_NoData(x, y) && (ix = (int)floor(px)) >= 0 && ix < Get_NX() )
					{
						S[ix][iy]	+= pGrid->asDouble(x, y);
						N[ix][iy]	++;
					}
				}
			}
		}
	}

	//-----------------------------------------------------
	else // if( bAreaProportional == true )
	{
		ax	= ((pGrid->Get_XMin() - 0.5 * pGrid->Get_Cellsize()) - (Get_XMin() - 0.5 * Get_Cellsize())) / Get_Cellsize();
		ay	= ((pGrid->Get_YMin() - 0.5 * pGrid->Get_Cellsize()) - (Get_YMin() - 0.5 * Get_Cellsize())) / Get_Cellsize();

		for(y=0, py=ay; y<pGrid->Get_NY() && SG_UI_Process_Set_Progress(y, pGrid->Get_NY()); y++, py+=d)
		{
			if( py > -d || py < Get_NY() )
			{
				iy	= (int)floor(py);
				wy	= (py + d) - iy;
				wy	= wy < 1.0 ? 1.0 : wy - 1.0; 

				for(x=0, px=ax; x<pGrid->Get_NX(); x++, px+=d)
				{
					if( !pGrid->is_NoData(x, y) && (px > -d && px < Get_NX()) )
					{
						ix	= (int)floor(px);
						wx	= (px + d) - ix;
						wx	= wx < 1.0 ? 1.0 : wx - 1.0; 

						z	= pGrid->asDouble(x, y);

						if( iy >= 0 && iy < Get_NY() )		// wy > 0.0 is always true
						{
							if( ix >= 0 && ix < Get_NX() )	// wx > 0.0 is always true
							{
								w	= wx * wy;
								S[ix][iy]	+= w * z;
								N[ix][iy]	+= w;
							}

							if( wx < 1.0 && (jx = ix + 1) >= 0 && jx < Get_NX() )
							{
								w	= (1.0 - wx) * wy;
								S[jx][iy]	+= w * z;
								N[jx][iy]	+= w;
							}
						}

						if( wy < 1.0 && (jy = iy + 1) >= 0 && jy < Get_NY() )
						{
							if( ix >= 0 && ix < Get_NX() )
							{
								w	= wx * (1.0 - wy);
								S[ix][jy]	+= w * z;
								N[ix][jy]	+= w;
							}

							if( wx < 1.0 && (jx = ix + 1) >= 0 && jx < Get_NX() )
							{
								w	= (1.0 - wx) * (1.0 - wy);
								S[jx][jy]	+= w * z;
								N[jx][jy]	+= w;
							}
						}
					}
				}
			}
		}
	}

	//-----------------------------------------------------
	for(y=0; y<Get_NY() && SG_UI_Process_Set_Progress(y, Get_NY()); y++)
	{
		for(x=0; x<Get_NX(); x++)
		{
			if( N[x][y] )
			{
				Set_Value(x, y, S[x][y] / N[x][y]);
			}
			else
			{
				Set_NoData(x, y);
			}
		}
	}

	//-----------------------------------------------------
	Get_History()	= pGrid->Get_History();
	Get_History().Add_Child(SG_T("GRID_OPERATION"), CSG_String::Format(SG_T("%f -> %f"), pGrid->Get_Cellsize(), Get_Cellsize()))->Add_Property(SG_T("NAME"), _TL("Resampling"));

	SG_UI_Process_Set_Ready();

	return( true );
}


///////////////////////////////////////////////////////////
//														 //
///////////////////////////////////////////////////////////

//---------------------------------------------------------
bool CSG_Grid::_Assign_Majority(CSG_Grid *pGrid)
{
	if( Get_Cellsize() < pGrid->Get_Cellsize() || is_Intersecting(pGrid->Get_Extent()) == INTERSECTION_None )
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
	Get_History()	= pGrid->Get_History();
	Get_History().Add_Child(SG_T("GRID_OPERATION"), CSG_String::Format(SG_T("%f -> %f"), pGrid->Get_Cellsize(), Get_Cellsize()))->Add_Property(SG_T("NAME"), _TL("Resampling"));

	SG_UI_Process_Set_Ready();

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
	Assign((CSG_Grid *)&Grid, GRID_INTERPOLATION_Undefined);

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
		int						x, y;
		double					xWorld, yWorld, Value;
		TSG_Grid_Interpolation	Interpolation;

		Interpolation	=	Get_Cellsize() == Grid.Get_Cellsize() && fmod(Get_XMin() - Grid.Get_XMin(), Get_Cellsize()) == 0.0
						&&	Get_Cellsize() == Grid.Get_Cellsize() && fmod(Get_YMin() - Grid.Get_YMin(), Get_Cellsize()) == 0.0
						?	GRID_INTERPOLATION_NearestNeighbour
						:	GRID_INTERPOLATION_BSpline;

		for(y=0, yWorld=Get_YMin(); y<Get_NY() && SG_UI_Process_Set_Progress(y, Get_NY()); y++, yWorld+=Get_Cellsize())
		{
			for(x=0, xWorld=Get_XMin(); x<Get_NX(); x++, xWorld+=Get_Cellsize())
			{
				if( !Grid.Get_Value(xWorld, yWorld, Value, Interpolation, true) )
				{
					Set_NoData(x, y);
				}
				else switch( Operation )
				{
				case GRID_OPERATION_Addition:
					Add_Value(x, y,  Value);
					break;

				case GRID_OPERATION_Subtraction:
					Add_Value(x, y, -Value);
					break;

				case GRID_OPERATION_Multiplication:
					Mul_Value(x, y,  Value);
					break;

				case GRID_OPERATION_Division:
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

		SG_UI_Process_Set_Ready();

		//-------------------------------------------------
		switch( Operation )
		{
		case GRID_OPERATION_Addition:
			Get_History().Add_Child(SG_T("GRID_OPERATION"), Grid.Get_Name())->Add_Property(SG_T("NAME"), _TL("Addition"));
			break;

		case GRID_OPERATION_Subtraction:
			Get_History().Add_Child(SG_T("GRID_OPERATION"), Grid.Get_Name())->Add_Property(SG_T("NAME"), _TL("Subtraction"));
			break;

		case GRID_OPERATION_Multiplication:
			Get_History().Add_Child(SG_T("GRID_OPERATION"), Grid.Get_Name())->Add_Property(SG_T("NAME"), _TL("Multiplication"));
			break;

		case GRID_OPERATION_Division:
			Get_History().Add_Child(SG_T("GRID_OPERATION"), Grid.Get_Name())->Add_Property(SG_T("NAME"), _TL("Division"));
			break;
		}

		Get_History()	+= ((CSG_Grid *)&Grid)->Get_History();
	}

	return( *this );
}

//---------------------------------------------------------
CSG_Grid & CSG_Grid::_Operation_Arithmetic(double Value, TSG_Grid_Operation Operation)
{
	//-----------------------------------------------------
	switch( Operation )
	{
	case GRID_OPERATION_Addition:
		if( Value == 0.0 )
			return( *this );

		Get_History().Add_Child(SG_T("GRID_OPERATION"), Value)->Add_Property(SG_T("NAME"), _TL("Addition"));
		break;

	case GRID_OPERATION_Subtraction:
		if( Value == 0.0 )
			return( *this );

		Get_History().Add_Child(SG_T("GRID_OPERATION"), Value)->Add_Property(SG_T("NAME"), _TL("Subtraction"));
		Value	= -Value;
		break;

	case GRID_OPERATION_Multiplication:
		if( Value == 1.0 )
			return( *this );

		Get_History().Add_Child(SG_T("GRID_OPERATION"), Value)->Add_Property(SG_T("NAME"), _TL("Multiplication"));
		break;

	case GRID_OPERATION_Division:
		if( Value == 0.0 )
			return( *this );

		Get_History().Add_Child(SG_T("GRID_OPERATION"), Value)->Add_Property(SG_T("NAME"), _TL("Division"));
		Value	= 1.0 / Value;
		break;
	}

	//-----------------------------------------------------
	for(int y=0; y<Get_NY() && SG_UI_Process_Set_Progress(y, Get_NY()); y++)
	{
		for(int x=0; x<Get_NX(); x++)
		{
			if( !is_NoData(x, y) )
			{
				switch( Operation )
				{
				case GRID_OPERATION_Addition:
				case GRID_OPERATION_Subtraction:
					Add_Value(x, y, Value);
					break;

				case GRID_OPERATION_Multiplication:
				case GRID_OPERATION_Division:
					Mul_Value(x, y, Value);
					break;
				}
			}
		}
	}

	SG_UI_Process_Set_Ready();

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
	int		x, y;
	double	zMin, zMax;

	if( is_Valid() && Get_ZRange() > 0.0 )
	{
		zMin	= Get_ZMin();
		zMax	= Get_ZMax();

		for(y=0; y<Get_NY() && SG_UI_Process_Set_Progress(y, Get_NY()); y++)
		{
			for(x=0; x<Get_NX(); x++)
			{
				if( !is_NoData(x, y) )
				{
					Set_Value(x, y, zMax - (asDouble(x, y) - zMin));
				}
			}
		}

		SG_UI_Process_Set_Ready();

		Get_History().Add_Child(SG_T("GRID_OPERATION"), _TL("Inversion"));
	}
}

//---------------------------------------------------------
void CSG_Grid::Flip(void)
{
	int		x, yA, yB;
	double	*Line, d;

	if( is_Valid() )
	{
		Line	= (double *)SG_Malloc(Get_NX() * sizeof(double));

		for(yA=0, yB=Get_NY()-1; yA<yB && SG_UI_Process_Set_Progress(2 * yA, Get_NY()); yA++, yB--)
		{
			for(x=0; x<Get_NX(); x++)
			{
				Line[x]	= asDouble(x, yA);
			}

			for(x=0; x<Get_NX(); x++)
			{
				d		= Line[x];
				Line[x]	= asDouble(x, yB);
				Set_Value(x, yB, d);
			}

			for(x=0; x<Get_NX(); x++)
			{
				Set_Value(x, yA, Line[x]);
			}
		}

		SG_UI_Process_Set_Ready();

		SG_Free(Line);

		Get_History().Add_Child(SG_T("GRID_OPERATION"), _TL("Vertically mirrored"));
	}
}

//---------------------------------------------------------
void CSG_Grid::Mirror(void)
{
	int		xA, xB, y;
	double	d;

	if( is_Valid() )
	{
		for(y=0; y<Get_NY() && SG_UI_Process_Set_Progress(y, Get_NY()); y++)
		{
			for(xA=0, xB=Get_NX()-1; xA<xB; xA++, xB--)
			{
				d			=    asDouble(xA, y);
				Set_Value(xA, y, asDouble(xB, y));
				Set_Value(xB, y, d);
			}
		}

		SG_UI_Process_Set_Ready();

		Get_History().Add_Child(SG_T("GRID_OPERATION"), _TL("Horizontally mirrored"));
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
	if( !is_Valid() || Get_ZRange() <= 0.0 )
	{
		return( false );
	}

	SG_UI_Process_Set_Text(_TL("Normalisation"));

	double	zMin	= Get_ZMin();
	double	zRange	= Get_ZRange();

	for(int y=0; y<Get_NY() && SG_UI_Process_Set_Progress(y, Get_NY()); y++)
	{
		#pragma omp parallel for
		for(int x=0; x<Get_NX(); x++)
		{
			if( !is_NoData(x, y) )
			{
				Set_Value(x, y, (asDouble(x, y) - zMin) / zRange);
			}
		}
	}

	SG_UI_Process_Set_Ready();

	Get_History().Add_Child(SG_T("GRID_OPERATION"), _TL("Normalisation"));

	return( true );
}

//---------------------------------------------------------
bool CSG_Grid::DeNormalise(double Minimum, double Maximum)
{
	if( !is_Valid() || Minimum > Maximum )
	{
		return( false );
	}

	SG_UI_Process_Set_Text(_TL("Denormalisation"));

	for(int y=0; y<Get_NY() && SG_UI_Process_Set_Progress(y, Get_NY()); y++)
	{
		#pragma omp parallel for
		for(int x=0; x<Get_NX(); x++)
		{
			if( !is_NoData(x, y) )
			{
				Set_Value(x, y, Minimum + asDouble(x, y) * (Maximum - Minimum));
			}
		}
	}

	SG_UI_Process_Set_Ready();

	Get_History().Add_Child(SG_T("GRID_OPERATION"), _TL("Denormalisation"));

	return( true );
}

//---------------------------------------------------------
bool CSG_Grid::Standardise(void)
{
	if( !is_Valid() || Get_StdDev() <= 0.0 )
	{
		return( false );
	}

	SG_UI_Process_Set_Text(_TL("Standardisation"));

	double	Mean	= Get_ArithMean();
	double	StdDev	= Get_StdDev();

	for(int y=0; y<Get_NY() && SG_UI_Process_Set_Progress(y, Get_NY()); y++)
	{
		#pragma omp parallel for
		for(int x=0; x<Get_NX(); x++)
		{
			if( !is_NoData(x, y) )
			{
				Set_Value(x, y, (asDouble(x, y) - Mean) / StdDev);
			}
		}
	}

	SG_UI_Process_Set_Ready();

	Get_History().Add_Child(SG_T("GRID_OPERATION"), _TL("Standardisation"));

	return( false );
}

//---------------------------------------------------------
bool CSG_Grid::DeStandardise(double Mean, double StdDev)
{
	if( !is_Valid() || StdDev <= 0.0 )
	{
		return( false );
	}

	SG_UI_Process_Set_Text(_TL("Destandardisation"));

	for(int y=0; y<Get_NY() && SG_UI_Process_Set_Progress(y, Get_NY()); y++)
	{
		#pragma omp parallel for
		for(int x=0; x<Get_NX(); x++)
		{
			if( !is_NoData(x, y) )
			{
				Set_Value(x, y, Mean + asDouble(x, y) * StdDev);
			}
		}
	}

	SG_UI_Process_Set_Ready();

	Get_History().Add_Child(SG_T("GRID_OPERATION"), _TL("Destandardisation"));

	return( true );
}


///////////////////////////////////////////////////////////
//														 //
//														 //
//														 //
///////////////////////////////////////////////////////////

//---------------------------------------------------------
int CSG_Grid::Get_Gradient_NeighborDir(int x, int y, bool bMustBeLower)	const
{
	int		i, ix, iy, Direction;
	double	z, dz, dzMax;

	Direction	= -1;

	if( is_InGrid(x, y) )
	{
		z		= asDouble(x, y);
		dzMax	= 0.0;

		for(i=0; i<8; i++)
		{
			ix	= m_System.Get_xTo(i, x);
			iy	= m_System.Get_yTo(i, y);

			if( !is_InGrid(ix, iy) )
			{
				if( 1 )	// flag 'bStopOnNoData'
				{
					return( -1 );
				}
			}
			else
			{
				dz	= (z - asDouble(ix, iy)) / m_System.Get_Length(i);

				if( (bMustBeLower && dz > 0.0) || !bMustBeLower )
				{
					if( Direction < 0 || (dz > dzMax) )
					{
						Direction	= i;
						dzMax		= dz;
					}
				}
			}
		}
	}

	return( Direction );
}

//---------------------------------------------------------
bool CSG_Grid::Get_Gradient(int x, int y, double &Decline, double &Azimuth) const
{
	int		i, ix, iy, iDir;
	double	z, zm[4], G, H;

	if( is_InGrid(x, y) )
	{
		z		= asDouble(x, y);

		for(i=0, iDir=0; i<4; i++, iDir+=2)
		{
			ix		= m_System.Get_xTo(iDir, x);
			iy		= m_System.Get_yTo(iDir, y);

			if( is_InGrid(ix, iy) )
			{
				zm[i]	= asDouble(ix, iy) - z;
			}
			else
			{
				ix		= m_System.Get_xFrom(iDir, x);
				iy		= m_System.Get_yFrom(iDir, y);

				if( is_InGrid(ix, iy) )
				{
					zm[i]	= z - asDouble(ix, iy);
				}
				else
				{
					zm[i]	= 0.0;
				}
			}
		}

		G		= (zm[0] - zm[2]) / (2.0 * Get_Cellsize());
        H		= (zm[1] - zm[3]) / (2.0 * Get_Cellsize());

		Decline	= atan(sqrt(G*G + H*H));

		if( G != 0.0 )
			Azimuth	= M_PI_180 + atan2(H, G);
		else
			Azimuth	= H > 0.0 ? M_PI_270 : (H < 0.0 ? M_PI_090 : -1.0);

		return( true );
	}

	Decline	= 0.0;
	Azimuth	= -1.0;

	return( false );
}


///////////////////////////////////////////////////////////
//														 //
//														 //
//														 //
///////////////////////////////////////////////////////////

//---------------------------------------------------------

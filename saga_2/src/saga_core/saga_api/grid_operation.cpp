
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
	int		n, m;

	if( is_Valid() )
	{
		if( Value == 0.0 && m_Memory_Type == GRID_MEMORY_Normal )
		{
			for(n=0, m=_Get_nLineBytes(); n<Get_NY(); n++)
			{
				memset(m_Values[n], 0, m);
			}
		}
		else
		{
			for(n=0; n<Get_NCells(); n++)
			{
				Set_Value(n, Value);
			}
		}

		//-------------------------------------------------
		Get_History().Destroy();
		Get_History().Add_Entry(LNG("[HST] Value assigned to grid"), CSG_String::Format(SG_T("%f"), Value));

		//-------------------------------------------------
		m_bUpdate	= false;
		m_ArithMean	= m_zMin	= m_zMax	= Value;
		m_Variance	= 0.0;

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
	if( pObject && pObject->is_Valid() && pObject->Get_ObjectType() == Get_ObjectType() )
	{
		return( Assign((CSG_Grid *)pObject, GRID_INTERPOLATION_Undefined) );
	}

	return( false );
}

bool CSG_Grid::Assign(CSG_Grid *pGrid, TSG_Grid_Interpolation Interpolation)
{
	bool	bResult	= false;

	//-----------------------------------------------------
	if(	is_Valid() && pGrid && pGrid->is_Valid() && is_Intersecting(pGrid->Get_Extent()) != INTERSECTION_None )
	{
		if(	Get_Cellsize() == pGrid->Get_Cellsize()
		&&	fmod(Get_XMin() - pGrid->Get_XMin(), Get_Cellsize()) == 0.0
		&&	fmod(Get_YMin() - pGrid->Get_YMin(), Get_Cellsize()) == 0.0	)
		{	// in this case 'Nearest Neighbor' is always the best choice...
			bResult	= _Assign_Interpolated(pGrid, GRID_INTERPOLATION_NearestNeighbour);
		}
		else switch( Interpolation )
		{
		case GRID_INTERPOLATION_NearestNeighbour:
		case GRID_INTERPOLATION_Bilinear:
		case GRID_INTERPOLATION_InverseDistance:
		case GRID_INTERPOLATION_BicubicSpline:
		case GRID_INTERPOLATION_BSpline:
			bResult	= _Assign_Interpolated(pGrid, Interpolation);
			break;

		default:
			if( Get_Cellsize() < pGrid->Get_Cellsize() )	// Down-Scaling...
			{
				bResult	= _Assign_Interpolated(pGrid, GRID_INTERPOLATION_BSpline);
			}
			else											// Up-Scaling...
			{
				bResult	= _Assign_MeanValue(pGrid);
			}
			break;
		}

		//-------------------------------------------------
		if( bResult )
		{
			m_zFactor			= pGrid->m_zFactor;

			m_NoData_Value		= pGrid->m_NoData_Value;
			m_NoData_hiValue	= pGrid->m_NoData_hiValue;

//			Set_Name			(pGrid->Get_Name());
			Set_Description		(pGrid->Get_Description());
			Set_Unit			(pGrid->Get_Unit());
		}
	}

	//-----------------------------------------------------
	return( bResult );
}

//---------------------------------------------------------
bool CSG_Grid::_Assign_Interpolated(CSG_Grid *pGrid, TSG_Grid_Interpolation Interpolation)
{
	int		x, y;
	double	xPosition, yPosition, z;

	Set_NoData_Value_Range(pGrid->Get_NoData_Value(), pGrid->Get_NoData_hiValue());

	for(y=0, yPosition=Get_YMin(); y<Get_NY() && SG_UI_Process_Set_Progress(y, Get_NY()); y++, yPosition+=Get_Cellsize())
	{
		for(x=0, xPosition=Get_XMin(); x<Get_NX(); x++, xPosition+=Get_Cellsize())
		{
			if( pGrid->Get_Value(xPosition, yPosition, z, Interpolation) )
			{
				Set_Value (x, y, z);
			}
			else
			{
				Set_NoData(x, y);
			}
		}
	}

	Get_History().Assign(pGrid->Get_History());
	Get_History().Add_Entry(LNG("[DAT] Resampling"), CSG_String::Format(SG_T("%f -> %f"), pGrid->Get_Cellsize(), Get_Cellsize()));

	SG_UI_Process_Set_Ready();

	return( true );
}

//---------------------------------------------------------
bool CSG_Grid::_Assign_MeanValue(CSG_Grid *pGrid)
{
	int		o_x, o_y, o_ax, o_ay,
			ix, iy, ix_A, iy_A, ix_B, iy_B,
			n;

	double	i_x, i_y, i_ax, i_ay, i_dx, i_dy,
			Sum;

	//-----------------------------------------------------
	i_dx	= Get_Cellsize() / pGrid->Get_Cellsize();
	i_dy	= Get_Cellsize() / pGrid->Get_Cellsize();

	i_ax	=	(Get_XMin() - pGrid->Get_XMin()) / pGrid->Get_Cellsize();
	if( i_ax < 0 )
		i_ax	= 0;

	o_ax	= (int)((pGrid->Get_XMin() - Get_XMin()) / Get_Cellsize());
	if( o_ax < 0 )
		o_ax	= 0;

	i_ay	=		(Get_YMin() - pGrid->Get_YMin()) / pGrid->Get_Cellsize();
	if( i_ay < 0 )
		i_ay	= 0;

	o_ay	= (int)((pGrid->Get_YMin() - Get_YMin()) / Get_Cellsize());
	if( o_ay < 0 )
		o_ay	= 0;

	if(	i_ax < pGrid->Get_NX() && o_ax < Get_NX()
	&&	i_ay < pGrid->Get_NY() && o_ay < Get_NY()	)
	{
		Assign_NoData();

		for(o_y=o_ay, i_y=i_ay; o_y<Get_NY() && i_y<pGrid->Get_NY() && SG_UI_Process_Set_Progress(o_y, Get_NY()); o_y++, i_y+=i_dy)
		{
			iy_A	= (int)(0.5 + i_y - i_dy / 2.0);
			if( iy_A < 0 )
				iy_A	= 0;

			iy_B	= (int)(0.5 + i_y + i_dy / 2.0);
			if( iy_B  >= pGrid->Get_NY() )
				iy_B	= pGrid->Get_NY() - 1;

			for(o_x=o_ax, i_x=i_ax; o_x<Get_NX() && i_x<pGrid->Get_NX(); o_x++, i_x+=i_dx)
			{
				ix_A	= (int)(0.5 + i_x - i_dx / 2.0);
				if( ix_A < 0 )
					ix_A	= 0;

				ix_B	= (int)(0.5 + i_x + i_dx / 2.0);
				if( ix_B  >= pGrid->Get_NX() )
					ix_B	= pGrid->Get_NX() - 1;

				for(iy=iy_A, n=0, Sum=0.0; iy<=iy_B; iy++)
				{
					for(ix=ix_A; ix<=ix_B; ix++)
					{
						if( pGrid->is_InGrid(ix, iy) )
						{
							Sum	+= pGrid->asDouble(ix, iy);
							n++;
						}
					}
				}

				if( n > 0 )
				{
					Set_Value(o_x, o_y, Sum / (double)n);
				}
			}
		}

		Get_History().Assign(pGrid->Get_History());
		Get_History().Add_Entry(LNG("[DAT] Resampling"), CSG_String::Format(SG_T("%f -> %f"), pGrid->Get_Cellsize(), Get_Cellsize()));

		SG_UI_Process_Set_Ready();

		return( true );
	}

	return( false );
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
			Get_History().Add_Entry(LNG("[HST] Grid addition")		, Grid.Get_Name());
			break;

		case GRID_OPERATION_Subtraction:
			Get_History().Add_Entry(LNG("[HST] Grid subtraction")	, Grid.Get_Name());
			break;

		case GRID_OPERATION_Multiplication:
			Get_History().Add_Entry(LNG("[HST] Grid multiplication"), Grid.Get_Name());
			break;

		case GRID_OPERATION_Division:
			Get_History().Add_Entry(LNG("[HST] Grid division")		, Grid.Get_Name());
			break;
		}

		Get_History().Assign(Grid.m_History, true);
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
		Get_History().Add_Entry(LNG("[HST] Value addition")			, CSG_String::Format(SG_T("%f"), Value));
		break;

	case GRID_OPERATION_Subtraction:
		Get_History().Add_Entry(LNG("[HST] Value subtraction")		, CSG_String::Format(SG_T("%f"), Value));
		Value	= -Value;
		break;

	case GRID_OPERATION_Multiplication:
		Get_History().Add_Entry(LNG("[HST] Value multiplication")	, CSG_String::Format(SG_T("%f"), Value));
		break;

	case GRID_OPERATION_Division:
		if( Value == 0.0 )
			return( *this );

		Get_History().Add_Entry(LNG("[HST] Value division")			, CSG_String::Format(SG_T("%f"), Value));
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

		Get_History().Add_Entry(LNG("[HST] Grid operation"), LNG("[HST] Inversion"));
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

		Get_History().Add_Entry(LNG("[HST] Grid operation"), LNG("[HST] Vertically mirrored"));
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

		Get_History().Add_Entry(LNG("[HST] Grid operation"), LNG("[HST] Horizontally mirrored"));
	}
}


///////////////////////////////////////////////////////////
//														 //
//					Grid-Operations - B					 //
//														 //
///////////////////////////////////////////////////////////

//---------------------------------------------------------
#define NORMALISED_NODATA	9.0

//---------------------------------------------------------
void CSG_Grid::Normalise(void)
{
	int		x, y;
	double	d;

	if( is_Valid() )
	{
		Update_Statistics();

		if( m_Variance > 0.0 )
		{
			if(	(Get_NoData_hiValue() > -NORMALISED_NODATA && Get_NoData_hiValue() < NORMALISED_NODATA)
			||	(Get_NoData_Value  () > -NORMALISED_NODATA && Get_NoData_Value  () < NORMALISED_NODATA) )
			{
				for(y=0; y<Get_NY() && SG_UI_Process_Set_Progress(y, Get_NY()); y++)
				{
					for(x=0; x<Get_NX(); x++)
					{
						if( is_NoData(x, y) )
						{
							Set_Value(x, y, -NORMALISED_NODATA);
						}
					}
				}

				Set_NoData_Value(-NORMALISED_NODATA);
			}

			d	= sqrt(m_Variance);

			for(y=0; y<Get_NY() && SG_UI_Process_Set_Progress(y, Get_NY()); y++)
			{
				for(x=0; x<Get_NX(); x++)
				{
					if( !is_NoData(x, y) )
					{
						Set_Value(x, y, (asDouble(x, y) - m_ArithMean) / d );
					}
				}
			}

			SG_UI_Process_Set_Ready();

			Get_History().Add_Entry(LNG("[HST] Grid normalisation"), CSG_String::Format(SG_T("%f / %f"), m_ArithMean, m_Variance));
		}
	}
}

//---------------------------------------------------------
void CSG_Grid::DeNormalise(double ArithMean, double Variance)
{
	int		x, y;

	if( is_Valid() )
	{
		Variance	= sqrt(Variance);

		for(y=0; y<Get_NY() && SG_UI_Process_Set_Progress(y, Get_NY()); y++)
		{
			for(x=0; x<Get_NX(); x++)
			{
				if( !is_NoData(x, y) )
				{
					Set_Value(x, y, Variance * asDouble(x, y) + ArithMean);
				}
			}
		}

		SG_UI_Process_Set_Ready();

		Get_History().Add_Entry(LNG("[HST] Grid denormalisation"), CSG_String::Format(SG_T("%f / %f"), ArithMean, Variance));
	}
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

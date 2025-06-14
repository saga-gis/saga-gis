
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
//                University Hamburg                     //
//                Germany                                //
//                                                       //
//    e-mail:     oconrad@saga-gis.org                   //
//                                                       //
///////////////////////////////////////////////////////////

//---------------------------------------------------------
#include "grid.h"
#include "shapes.h"
#include "parameters.h"


///////////////////////////////////////////////////////////
//                                                       //
//                                                       //
//                                                       //
///////////////////////////////////////////////////////////

//---------------------------------------------------------
/**
* decimal digits, default precision used for storing cellsize and extent
**/
// 
int CSG_Grid_System::m_Precision = 16;

//---------------------------------------------------------
/**
* Set the default precision given as number of decimals used
* to store coordinates and cell sizes. Ignored if set to a
* negative value.
**/
int CSG_Grid_System::Set_Precision(int Decimals)
{
	m_Precision = Decimals;

	return( m_Precision );
}

//---------------------------------------------------------
/**
* Get the default precision (number of decimals) used
* to store coordinates and cell sizes.
* A negative value indicates that this is ignored.
**/
int CSG_Grid_System::Get_Precision(void)
{
	return( m_Precision );
}


///////////////////////////////////////////////////////////
//                                                       //
///////////////////////////////////////////////////////////

//---------------------------------------------------------
CSG_Grid_System::CSG_Grid_System(void)
{
	Destroy();
}

//---------------------------------------------------------
CSG_Grid_System::CSG_Grid_System(const CSG_Grid_System &System, int Precision)
{
	Create(System, Precision);
}

//---------------------------------------------------------
CSG_Grid_System::CSG_Grid_System(double Cellsize, const CSG_Rect &Extent, int Precision)
{
	Create(Cellsize, Extent, Precision);
}

//---------------------------------------------------------
CSG_Grid_System::CSG_Grid_System(double Cellsize, double xMin, double yMin, double xMax, double yMax, int Precision)
{
	Create(Cellsize, xMin, yMin, xMax, yMax, Precision);
}

//---------------------------------------------------------
CSG_Grid_System::CSG_Grid_System(double Cellsize, double xMin, double yMin, int NX, int NY, int Precision)
{
	Create(Cellsize, xMin, yMin, NX, NY, Precision);
}

//---------------------------------------------------------
CSG_Grid_System::CSG_Grid_System(const CSG_String &System, int Precision)
{
	Create(System, Precision);
}

//---------------------------------------------------------
CSG_Grid_System::~CSG_Grid_System(void)
{
	Destroy();
}


///////////////////////////////////////////////////////////
//                                                       //
///////////////////////////////////////////////////////////

//---------------------------------------------------------
bool CSG_Grid_System::Create(const CSG_Grid_System &System, int Precision)
{
	if( Precision >= 0 )
	{
		return( Create(System.Get_Cellsize(), System.Get_XMin(), System.Get_YMin(), System.Get_NX(), System.Get_NY(), Precision) );
	}

	m_NX           = System.m_NX;
	m_NY           = System.m_NY;
	m_NCells       = System.m_NCells;

	m_Cellsize     = System.m_Cellsize;
	m_Cellarea     = System.m_Cellarea;
	m_Diagonal     = System.m_Diagonal;

	m_Extent       = System.m_Extent;
	m_Extent_Cells = System.m_Extent_Cells;

	return( is_Valid() );
}

//---------------------------------------------------------
bool CSG_Grid_System::Create(double Cellsize, const CSG_Rect &Extent, int Precision)
{
	if( Cellsize > 0. && Extent.Get_XRange() >= 0. && Extent.Get_YRange() >= 0. )
	{
		int nx = 1 + (int)(0.5 + Extent.Get_XRange() / Cellsize);
		int ny = 1 + (int)(0.5 + Extent.Get_YRange() / Cellsize);

		double x = fabs(Cellsize - Extent.Get_XRange() / (nx - 1.)) <= 0. ? Extent.Get_XMin() : Extent.Get_Center().x - Cellsize * (nx - 1.) / 2.;
		double y = fabs(Cellsize - Extent.Get_YRange() / (ny - 1.)) <= 0. ? Extent.Get_YMin() : Extent.Get_Center().y - Cellsize * (ny - 1.) / 2.;

		return( Create(Cellsize, x, y, nx, ny, Precision) );
	}

	Destroy();

	return( false );
}

//---------------------------------------------------------
bool CSG_Grid_System::Create(double Cellsize, double xMin, double yMin, double xMax, double yMax, int Precision)
{
	return( Create(Cellsize, CSG_Rect(xMin, yMin, xMax, yMax), Precision) );
}

//---------------------------------------------------------
bool CSG_Grid_System::Create(double Cellsize, double xMin, double yMin, int NX, int NY, int Precision)
{
	if( Cellsize > 0. && NX > 0 && NY > 0 )
	{
		Cellsize = SG_Get_Rounded(Cellsize, Precision == -1 ? m_Precision : Precision);
		xMin     = SG_Get_Rounded(xMin    , Precision == -1 ? m_Precision : Precision);
		yMin     = SG_Get_Rounded(yMin    , Precision == -1 ? m_Precision : Precision);

		if( Cellsize > 0. )
		{
			m_NX           = NX;
			m_NY           = NY;
			m_NCells       = (sLong)NY * NX;

			m_Cellsize     = Cellsize;
			m_Cellarea     = Cellsize * Cellsize;
			m_Diagonal     = Cellsize * sqrt(2.);

			m_Extent.xMin  = xMin;
			m_Extent.yMin  = yMin;
			m_Extent.xMax  = xMin + (NX - 1.) * Cellsize;
			m_Extent.yMax  = yMin + (NY - 1.) * Cellsize;

			m_Extent_Cells = m_Extent;
			m_Extent_Cells.Inflate(0.5 * Cellsize, false);

			return( true );
		}
	}

	Destroy();

	return( false );
}

//---------------------------------------------------------
/**
* Tries to construct a grid system from a string that uses
* one of the formats provided by the Get_Name() function,
* i.e. "<double>; <int>x <int>y; <double>x <double>y"
* or "Cell size: <double>, Number of cells: <int>x/<int>y, Lower left corner: <double>x/<double>y"
*/
bool CSG_Grid_System::Create(const CSG_String &System, int Precision)
{
	double Cellsize, xMin, yMin; int NX, NY; CSG_Strings Values;

	//      1   2   3     4
	// "%.*f; %d; %d; %.*f; %.*fy"
	Values = SG_String_Tokenize(System, ";");

	if( Values.Get_Count() == 5 && Values[0].asDouble(Cellsize) && Values[1].asInt(NX) && Values[2].asInt(NY) && Values[3].asDouble(xMin) && Values[4].asDouble(yMin) )
	{
		return( Create(Cellsize, xMin, yMin, NX, NY, Precision) );
	}

	//      1   2    3     4
	// "%.*f; %dx %dy; %.*fx %.*fy"
	Values = SG_String_Tokenize(System, ";x");

	if( Values.Get_Count() == 5 && Values[0].asDouble(Cellsize) && Values[1].asInt(NX) && Values[2].asInt(NY) && Values[3].asDouble(xMin) && Values[4].asDouble(yMin) )
	{
		return( Create(Cellsize, xMin, yMin, NX, NY, Precision) );
	}

	//    1   2   3    4   5   6    7
	// "%s: %f, %s: %dx/%dy, %s: %fx/%fy"
	Values = SG_String_Tokenize(System, ":,/");

	if( Values.Get_Count() == 8 && Values[1].asDouble(Cellsize) && Values[3].asInt(NX) && Values[4].asInt(NY) && Values[6].asDouble(xMin) && Values[7].asDouble(yMin) )
	{
		return( Create(Cellsize, xMin, yMin, NX, NY, Precision) );
	}

	return( false );
}

//---------------------------------------------------------
bool CSG_Grid_System::Destroy(void)
{
	m_NX       = 0;
	m_NY       = 0;
	m_NCells   = 0;

	m_Cellsize = 0.;
	m_Cellarea = 0.;
	m_Diagonal = 0.;

	m_Extent      .Create(0., 0., 0., 0.);
	m_Extent_Cells.Create(0., 0., 0., 0.);

	return( true );
}

//---------------------------------------------------------
bool CSG_Grid_System::Assign(const CSG_Grid_System &System)
{	return( Create(System) );	}

bool CSG_Grid_System::Assign(double Cellsize, const CSG_Rect &Extent)
{	return( Create(Cellsize, Extent) );	}

bool CSG_Grid_System::Assign(double Cellsize, double xMin, double yMin, double xMax, double yMax)
{	return( Create(Cellsize, xMin, yMin, xMax, yMax) );	}

bool CSG_Grid_System::Assign(double Cellsize, double xMin, double yMin, int NX, int NY)
{	return( Create(Cellsize, xMin, yMin, NX, NY) );	}


///////////////////////////////////////////////////////////
//                                                       //
///////////////////////////////////////////////////////////

//---------------------------------------------------------
bool CSG_Grid_System::is_Valid(void) const
{
	return( m_Cellsize > 0. && m_NX > 0 && m_NY > 0 );
}

//---------------------------------------------------------
const SG_Char * CSG_Grid_System::Get_Name(bool bShort)
{
	if( is_Valid() )
	{
		if( bShort )
		{
			m_Name.Printf("%.*f; %dx %dy; %.*fx %.*fy",
				SG_Get_Significant_Decimals(m_Cellsize   ), m_Cellsize, m_NX, m_NY,
				SG_Get_Significant_Decimals(m_Extent.xMin), m_Extent.xMin,
				SG_Get_Significant_Decimals(m_Extent.yMin), m_Extent.yMin
			);
		}
		else
		{
			m_Name.Printf("%s: %f, %s: %dx/%dy, %s: %fx/%fy",
				_TL("Cell size"             ), m_Cellsize,
				_TL("Number of cells"       ), m_NX, m_NY,
				_TL("Lower left cell center"), m_Extent.xMin, m_Extent.yMin
			);
		}
	}
	else
	{
		m_Name = _TL("<not set>");
	}

	return( m_Name );
}

//---------------------------------------------------------
const SG_Char * CSG_Grid_System::asString(void)
{
	m_Name.Printf("%.*f; %d; %d; %.*f; %.*f",
		SG_Get_Significant_Decimals(m_Cellsize   ), m_Cellsize, m_NX, m_NY,
		SG_Get_Significant_Decimals(m_Extent.xMin), m_Extent.xMin,
		SG_Get_Significant_Decimals(m_Extent.yMin), m_Extent.yMin
	);

	return( m_Name );
}


///////////////////////////////////////////////////////////
//                                                       //
///////////////////////////////////////////////////////////

//---------------------------------------------------------
CSG_Grid_System & CSG_Grid_System::operator = (const CSG_Grid_System &System)
{
	Create(System);

	return( *this );
}

//---------------------------------------------------------
bool CSG_Grid_System::operator == (const CSG_Grid_System *pSystem) const
{
	return( pSystem != NULL && is_Equal(*pSystem) ==  true );
}

bool CSG_Grid_System::operator != (const CSG_Grid_System *pSystem) const
{
	return( pSystem == NULL || is_Equal(*pSystem) == false );
}

//---------------------------------------------------------
bool CSG_Grid_System::operator == (const CSG_Grid_System &System) const
{
	return( is_Equal(System) ==  true );
}

bool CSG_Grid_System::operator != (const CSG_Grid_System &System) const
{
	return( is_Equal(System) == false );
}


///////////////////////////////////////////////////////////
//                                                       //
///////////////////////////////////////////////////////////

//---------------------------------------------------------
bool CSG_Grid_System::is_Equal(const CSG_Grid_System &System) const
{
	return( m_Cellsize           == System.m_Cellsize
		&&  m_NX                 == System.m_NX
		&&  m_NY                 == System.m_NY
		&&  m_Extent.xMin == System.m_Extent.xMin
		&&  m_Extent.yMin == System.m_Extent.yMin
	);
}

//---------------------------------------------------------
bool CSG_Grid_System::is_Equal(double Cellsize, const TSG_Rect &Extent) const
{
	return( m_Cellsize == Cellsize && m_Extent == Extent );
}


///////////////////////////////////////////////////////////
//                                                       //
//                                                       //
//                                                       //
///////////////////////////////////////////////////////////

//---------------------------------------------------------
CSG_Grid_Cell_Addressor::CSG_Grid_Cell_Addressor(void)
{
	m_Kernel.Add_Field("X", SG_DATATYPE_Int   );
	m_Kernel.Add_Field("Y", SG_DATATYPE_Int   );
	m_Kernel.Add_Field("D", SG_DATATYPE_Double);
	m_Kernel.Add_Field("W", SG_DATATYPE_Double);
}

//---------------------------------------------------------
bool CSG_Grid_Cell_Addressor::Destroy(void)
{
	m_Kernel.Del_Records();

	m_Radius	= 1.;
	m_Radius_0	= 0.;
	m_Direction	= 0.;
	m_Tolerance	= 0.;

	return( true );
}


///////////////////////////////////////////////////////////
//                                                       //
///////////////////////////////////////////////////////////

//---------------------------------------------------------
bool CSG_Grid_Cell_Addressor::Add_Parameters(CSG_Parameters &Parameters, const CSG_String &Parent, int Style)
{
	CSG_String	Types;

	if( (Style & SG_GRIDCELLADDR_PARM_SQUARE ) != 0 )
	{
		Types	+= CSG_String::Format("{%d}%s|", SG_GRIDCELLADDR_PARM_SQUARE , _TL("Square" ));
	}

	if( (Style & SG_GRIDCELLADDR_PARM_CIRCLE ) != 0 )
	{
		Types	+= CSG_String::Format("{%d}%s|", SG_GRIDCELLADDR_PARM_CIRCLE , _TL("Circle" ));
	}

	if( (Style & SG_GRIDCELLADDR_PARM_ANNULUS) != 0 )
	{
		Types	+= CSG_String::Format("{%d}%s|", SG_GRIDCELLADDR_PARM_ANNULUS, _TL("Annulus"));
	}

	if( (Style & SG_GRIDCELLADDR_PARM_SECTOR ) != 0 )
	{
		Types	+= CSG_String::Format("{%d}%s|", SG_GRIDCELLADDR_PARM_SECTOR , _TL("Sector" ));
	}

	Parameters.Add_Choice(Parent, "KERNEL_TYPE", _TL("Kernel Type"),
		_TL("The kernel's shape."),
		Types, 1
	);

	Parameters.Set_Enabled("KERNEL_TYPE", Parameters("KERNEL_TYPE")->asChoice()->Get_Count() > 1);

	//-----------------------------------------------------
	CSG_String	Unit_Radius((Style & SG_GRIDCELLADDR_PARM_MAPUNIT) == 0 ? _TL("cells") : _TL("map units"));

	if( (Style & SG_GRIDCELLADDR_PARM_SIZEDBL) != 0 )
	{
		if( (Style & SG_GRIDCELLADDR_PARM_ANNULUS) != 0 )
		{
			Parameters.Add_Double("KERNEL_TYPE", "KERNEL_INNER" , _TL("Inner Radius"), Unit_Radius, 0., 0., true);
		}

		Parameters    .Add_Double("KERNEL_TYPE", "KERNEL_RADIUS", _TL(      "Radius"), Unit_Radius, 1., 0., true);
	}
	else
	{
		if( (Style & SG_GRIDCELLADDR_PARM_ANNULUS) != 0 )
		{
			Parameters.Add_Int   ("KERNEL_TYPE", "KERNEL_INNER" , _TL("Inner Radius"), Unit_Radius, 0 , 0 , true);
		}

		Parameters    .Add_Int   ("KERNEL_TYPE", "KERNEL_RADIUS", _TL(      "Radius"), Unit_Radius, 2 , 0 , true);
	}

	//-----------------------------------------------------
	if( (Style & SG_GRIDCELLADDR_PARM_SECTOR) != 0 )
	{
		Parameters.Add_Double("KERNEL_TYPE", "KERNEL_DIRECTION", _TL("Direction"), _TL("degree"), 0., -360., true, 360., true);
		Parameters.Add_Double("KERNEL_TYPE", "KERNEL_TOLERANCE", _TL("Tolerance"), _TL("degree"), 5.,    0., true, 180., true);
	}

	if( (Style & SG_GRIDCELLADDR_PARM_WEIGHTING) != 0 )
	{
		CSG_Distance_Weighting::Add_Parameters(Parameters,
			Parameters("KERNEL_TYPE")->is_Enabled() ? CSG_String("KERNEL_TYPE") : Parent
		);
	}

	return( true );
}

//---------------------------------------------------------
bool CSG_Grid_Cell_Addressor::Set_Parameters(CSG_Parameters &Parameters, int Type)
{
	if( Type == 0 && Parameters("KERNEL_TYPE") )
	{
		Parameters("KERNEL_TYPE")->asChoice()->Get_Data(Type);
	}

	switch( Type )
	{
	case SG_GRIDCELLADDR_PARM_SQUARE:
		return( Set_Radius(
			Parameters("KERNEL_RADIUS"   )->asDouble(),
			true
		));

	case SG_GRIDCELLADDR_PARM_CIRCLE:
		return( Set_Radius(
			Parameters("KERNEL_RADIUS"   )->asDouble(),
			false
		));

	case SG_GRIDCELLADDR_PARM_ANNULUS:
		return( Set_Annulus(
			Parameters("KERNEL_INNER"    )->asDouble(),
			Parameters("KERNEL_RADIUS"   )->asDouble()
		));

	case SG_GRIDCELLADDR_PARM_SECTOR:
		return( Set_Sector(
			Parameters("KERNEL_RADIUS"   )->asDouble(),
			Parameters("KERNEL_DIRECTION")->asDouble() * M_DEG_TO_RAD,
			Parameters("KERNEL_TOLERANCE")->asDouble() * M_DEG_TO_RAD
		));
	}

	return( false );
}

//---------------------------------------------------------
bool CSG_Grid_Cell_Addressor::Set_Square (CSG_Parameters &Parameters)	{	return( Set_Parameters(Parameters, SG_GRIDCELLADDR_PARM_SQUARE ) );	}
bool CSG_Grid_Cell_Addressor::Set_Circle (CSG_Parameters &Parameters)	{	return( Set_Parameters(Parameters, SG_GRIDCELLADDR_PARM_CIRCLE ) );	}
bool CSG_Grid_Cell_Addressor::Set_Annulus(CSG_Parameters &Parameters)	{	return( Set_Parameters(Parameters, SG_GRIDCELLADDR_PARM_ANNULUS) );	}
bool CSG_Grid_Cell_Addressor::Set_Sector (CSG_Parameters &Parameters)	{	return( Set_Parameters(Parameters, SG_GRIDCELLADDR_PARM_SECTOR ) );	}


///////////////////////////////////////////////////////////
//                                                       //
///////////////////////////////////////////////////////////

//---------------------------------------------------------
bool CSG_Grid_Cell_Addressor::Enable_Parameters(CSG_Parameters &Parameters)
{
	if( Parameters("KERNEL_TYPE") )
	{
		int	Type	= Parameters("KERNEL_TYPE")->asChoice()->Get_Item_Data(Parameters("KERNEL_TYPE")->asInt()).asInt();

		Parameters.Set_Enabled("KERNEL_INNER"    , Type == SG_GRIDCELLADDR_PARM_ANNULUS);
		Parameters.Set_Enabled("KERNEL_DIRECTION", Type == SG_GRIDCELLADDR_PARM_SECTOR );
		Parameters.Set_Enabled("KERNEL_TOLERANCE", Type == SG_GRIDCELLADDR_PARM_SECTOR );
	}

	CSG_Distance_Weighting::Enable_Parameters(Parameters);

	return( true );
}


///////////////////////////////////////////////////////////
//                                                       //
///////////////////////////////////////////////////////////

//---------------------------------------------------------
bool CSG_Grid_Cell_Addressor::_Set_Kernel(int Type, double Radius, double Radius_Inner, double Direction, double Tolerance)
{
	Destroy();

	m_Type		= Type;
	m_Radius	= Radius;
	m_Radius_0	= Radius_Inner;
	m_Direction	= fmod(Direction, M_PI_360); if( Direction < 0. ) Direction += M_PI_360;
	m_Tolerance	= fmod(Tolerance, M_PI_180); if( Tolerance < 0. ) Tolerance += M_PI_180;

	if( m_Radius < 0. || m_Radius < m_Radius_0 )
	{
		return( false );
	}

	CSG_Vector	Sector(2);

	if( m_Type == 3 )	// sector
	{
		Sector[0]	= fmod(Direction - Tolerance, M_PI_360); if( Sector[0] < 0. ) Sector[0] += M_PI_360;
		Sector[1]	= fmod(Direction + Tolerance, M_PI_360); if( Sector[1] < 0. ) Sector[1] += M_PI_360;
	}

	//-----------------------------------------------------
	#define	ADD_CELL(x, y, Distance)	{\
		CSG_Table_Record	&Cell	= *Kernel.Add_Record();\
		Cell.Set_Value(0, x);\
		Cell.Set_Value(1, y);\
		Cell.Set_Value(2, Distance);\
		Cell.Set_Value(3, m_Weighting.Get_Weight(d));\
	}

	CSG_Table	Kernel(&m_Kernel);

	int	Size	= (int)ceil(m_Radius);

	//-----------------------------------------------------
	for(int y=-Size; y<=Size; y++)
	{
		if( abs(y) > m_Radius )
			continue;

		for(int x=-Size; x<=Size; x++)
		{
			if( abs(x) > m_Radius )
				continue;

			double	d	= SG_Get_Length(x, y);

			switch( m_Type )
			{
			default:	// square
				ADD_CELL(x, y, d);
				break;

			case  1:	// circle
				if( d <= m_Radius )
				{
					ADD_CELL(x, y, d);
				}
				break;

			case  2:	// annulus
				if( d <= m_Radius && d >= m_Radius_0 )
				{
					ADD_CELL(x, y, d);
				}
				break;

			case  3:	// sector
				if( d <= m_Radius && d >= m_Radius_0 && ((x == 0 && y == 0) || SG_is_Angle_Between(SG_Get_Angle_Of_Direction(x, y), Sector[0], Sector[1], false)) )
				{
					ADD_CELL(x, y, d);
				}
				break;
			}
		}
	}

	//-----------------------------------------------------
	if( Kernel.Get_Count() < 1 )
	{
		return( false );
	}

	Kernel.Set_Index(2, TABLE_INDEX_Ascending);

	for(int i=0; i<Kernel.Get_Count(); i++)
	{
		m_Kernel.Add_Record(Kernel.Get_Record_byIndex(i));
	}

	return( true );
}

//---------------------------------------------------------
bool CSG_Grid_Cell_Addressor::Set_Radius(double Radius, bool bSquare)
{
	return( bSquare ? Set_Square(Radius) : Set_Circle(Radius) );
}

//---------------------------------------------------------
bool CSG_Grid_Cell_Addressor::Set_Square(double Radius)
{
	return( _Set_Kernel(0, Radius, 0., 0., 0.) );
}

//---------------------------------------------------------
bool CSG_Grid_Cell_Addressor::Set_Circle(double Radius)
{
	return( _Set_Kernel(1, Radius, 0., 0., 0.) );
}

//---------------------------------------------------------
bool CSG_Grid_Cell_Addressor::Set_Annulus(double Radius_Inner, double Radius_Outer)
{
	return( _Set_Kernel(2, Radius_Outer, Radius_Inner, 0., 0.) );
}

//---------------------------------------------------------
bool CSG_Grid_Cell_Addressor::Set_Sector(double Radius, double Direction, double Tolerance)
{
	return( _Set_Kernel(3, Radius, 0., Direction, Tolerance) );
}


///////////////////////////////////////////////////////////
//                                                       //
//                                                       //
//                                                       //
///////////////////////////////////////////////////////////

//---------------------------------------------------------

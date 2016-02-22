/**********************************************************
 * Version $Id$
 *********************************************************/

///////////////////////////////////////////////////////////
//                                                       //
//                         SAGA                          //
//                                                       //
//      System for Automated Geoscientific Analyses      //
//                                                       //
//                    Module Library:                    //
//                      Grid_Filter                      //
//                                                       //
//-------------------------------------------------------//
//                                                       //
//                      Filter.cpp                       //
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
// 51 Franklin Street, 5th Floor, Boston, MA 02110-1301, //
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
#include "Filter_in_Polygon.h"


///////////////////////////////////////////////////////////
//														 //
//														 //
//														 //
///////////////////////////////////////////////////////////

//---------------------------------------------------------
CFilter_in_Polygon::CFilter_in_Polygon(void)
{
	//-----------------------------------------------------
	// 1. Info...

	Set_Name		(_TL("Simple Filter within shapes"));

	Set_Author		(SG_T("Johan Van de Wauw, 2015"));

	Set_Description	(_TW(
		"Simple standard filters for grids, evaluation within shapes."
	));


	//-----------------------------------------------------
	// 2. Parameters...

	Parameters.Add_Grid(
		NULL, "INPUT"		, _TL("Grid"),
		_TL(""),
		PARAMETER_INPUT
	);

	Parameters.Add_Grid(
		NULL, "RESULT"		, _TL("Filtered Grid"),
		_TL(""),
		PARAMETER_OUTPUT_OPTIONAL
	);

	Parameters.Add_Shapes(
		NULL, "SHAPES",_TL("Boundaries"),
		_TL("Boundaries: the simple filter will only operate on grid cells which fall in the same shape"),
		PARAMETER_INPUT,
		SHAPE_TYPE_Polygon
		);

	Parameters.Add_Choice(
		NULL, "MODE"		, _TL("Search Mode"),
		_TL(""),
		CSG_String::Format(SG_T("%s|%s|"),
			_TL("Square"),
			_TL("Circle")
		), 1
	);

	Parameters.Add_Choice(
		NULL, "METHOD"		, _TL("Filter"),
		_TL(""),
		CSG_String::Format(SG_T("%s|%s|%s|"),
			_TL("Smooth"),
			_TL("Sharpen"),
			_TL("Edge")
		), 0
	);

	Parameters.Add_Value(
		NULL, "RADIUS"		, _TL("Radius"),
		_TL(""),
		PARAMETER_TYPE_Int, 1, 1, true
	);
}


///////////////////////////////////////////////////////////
//														 //
//														 //
//														 //
///////////////////////////////////////////////////////////

//---------------------------------------------------------
bool CFilter_in_Polygon::On_Execute(void)
{
	int			Mode, Radius, Method;
	double		Mean;
	CSG_Grid	*pResult;
	

	//-----------------------------------------------------
	m_pInput	= Parameters("INPUT")	->asGrid();
	pResult		= Parameters("RESULT")	->asGrid();
	Radius		= Parameters("RADIUS")	->asInt();
	Mode		= Parameters("MODE")	->asInt();
	Method		= Parameters("METHOD")	->asInt();
	Boundaries  = Parameters("SHAPES")  ->asShapes();




	switch( Mode )
	{
	case 0:								break;
	case 1:	m_Radius.Create(Radius);	break;
	}

	//-----------------------------------------------------
	if( !pResult || pResult == m_pInput )
	{
		pResult	= SG_Create_Grid(m_pInput);
	}
	else
	{
		pResult->Set_Name(CSG_String::Format(SG_T("%s [%s]"), m_pInput->Get_Name(), _TL("Filter")));

		pResult->Set_NoData_Value(m_pInput->Get_NoData_Value());
	}



		Process_Set_Text(_TL("Initializing Fields"));

	int m_nFields	= Boundaries->Get_Count();

	m_Fields.Create(*Get_System(), m_nFields < pow(2.0, 16.0) - 1.0 ? SG_DATATYPE_Word : SG_DATATYPE_DWord);
	m_Fields.Set_NoData_Value(m_nFields);
	m_Fields.Assign_NoData();

	//-----------------------------------------------------
	for(int iField=0; iField<Boundaries->Get_Count() && Set_Progress(iField, Boundaries->Get_Count()); iField++)
	{
		CSG_Shape_Polygon	*pField	= (CSG_Shape_Polygon *)Boundaries->Get_Shape(iField);

		int	xMin	= Get_System()->Get_xWorld_to_Grid(pField->Get_Extent().Get_XMin()) - 1; if( xMin <  0        ) xMin = 0;
		int	xMax	= Get_System()->Get_xWorld_to_Grid(pField->Get_Extent().Get_XMax()) + 1; if( xMax >= Get_NX() ) xMax = Get_NX() - 1;
		int	yMin	= Get_System()->Get_yWorld_to_Grid(pField->Get_Extent().Get_YMin()) - 1; if( yMin <  0        ) yMin = 0;
		int	yMax	= Get_System()->Get_yWorld_to_Grid(pField->Get_Extent().Get_YMax()) + 1; if( yMax >= Get_NY() ) yMax = Get_NY() - 1;

		for(int y=yMin; y<=yMax; y++)
		{
			for(int x=xMin; x<=xMax; x++)
			{
				if( m_pInput->is_InGrid(x, y) && pField->Contains(Get_System()->Get_Grid_to_World(x, y)) )
				{
					m_Fields.Set_Value(x, y, iField);
				}
			}
		}
	}

	//-----------------------------------------------------
	for(int y=0; y<Get_NY() && Set_Progress(y); y++)
	{
		for(int x=0; x<Get_NX(); x++)
		{
			if( m_pInput->is_InGrid(x, y) )
			{
				switch( Mode )
				{
				case 0:		Mean	= Get_Mean_Square(x, y, Radius);	break;
				case 1:		Mean	= Get_Mean_Circle(x, y);			break;
				}

				switch( Method )
				{
				case 0:	default:	// Smooth...
					pResult->Set_Value(x, y, Mean);
					break;

				case 1:				// Sharpen...
					pResult->Set_Value(x, y, m_pInput->asDouble(x, y) + (m_pInput->asDouble(x, y) - Mean));
					break;

				case 2:				// Edge...
					pResult->Set_Value(x, y, m_pInput->asDouble(x, y) - Mean);
					break;
				}
			}
			else
			{
				pResult->Set_NoData(x, y);
			}
		}
	}

	//-----------------------------------------------------
	if( !Parameters("RESULT")->asGrid() || Parameters("RESULT")->asGrid() == m_pInput )
	{
		m_pInput->Assign(pResult);

		delete(pResult);

		DataObject_Update(m_pInput);
	}

	m_Radius.Destroy();

	return( true );
}


///////////////////////////////////////////////////////////
//														 //
//														 //
//														 //
///////////////////////////////////////////////////////////

//---------------------------------------------------------
double CFilter_in_Polygon::Get_Mean_Square(int x, int y, int Radius)
{
	int		ix, iy, n;
	double	s;

	int shapeID = m_Fields.asInt(x,y);
	for(n=0, s=0.0, iy=y-Radius; iy<=y+Radius; iy++)
	{
		for(ix=x-Radius; ix<=x+Radius; ix++)
		{
			if( m_pInput->is_InGrid(ix, iy) && shapeID==m_Fields.asInt(ix,iy))
			{
				s	+= m_pInput->asDouble(ix, iy);
				n	++;
			}
		}
	}

	return( n > 0 ? s / n : m_pInput->Get_NoData_Value() );
}

//---------------------------------------------------------
double CFilter_in_Polygon::Get_Mean_Circle(int x, int y)
{
	int		i, ix, iy, n;
	double	s;

	int shapeID = m_Fields.asInt(x,y);
	for(n=0, s=0.0, i=0; i<m_Radius.Get_nPoints(); i++)
	{
		m_Radius.Get_Point(i, x, y, ix, iy);

		if( m_pInput->is_InGrid(ix, iy)  && shapeID==m_Fields.asInt(ix,iy))
		{
			s	+= m_pInput->asDouble(ix, iy);
			n	++;
		}
	}

	return( n > 0 ? s / n : m_pInput->Get_NoData_Value() );
}


///////////////////////////////////////////////////////////
//														 //
//														 //
//														 //
///////////////////////////////////////////////////////////

//---------------------------------------------------------

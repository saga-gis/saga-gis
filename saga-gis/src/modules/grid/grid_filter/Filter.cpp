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
#include "Filter.h"


///////////////////////////////////////////////////////////
//														 //
//														 //
//														 //
///////////////////////////////////////////////////////////

//---------------------------------------------------------
CFilter::CFilter(void)
{
	//-----------------------------------------------------
	Set_Name		(_TL("Simple Filter"));

	Set_Author		("O.Conrad (c) 2003");

	Set_Description	(_TW(
		"Simple standard filters for grids."
	));

	//-----------------------------------------------------
	Parameters.Add_Grid(
		NULL	, "INPUT"		, _TL("Grid"),
		_TL(""),
		PARAMETER_INPUT
	);

	Parameters.Add_Grid(
		NULL	, "RESULT"		, _TL("Filtered Grid"),
		_TL(""),
		PARAMETER_OUTPUT_OPTIONAL
	);

	Parameters.Add_Choice(
		NULL	, "METHOD"		, _TL("Filter"),
		_TL(""),
		CSG_String::Format("%s|%s|%s|",
			_TL("Smooth"),
			_TL("Sharpen"),
			_TL("Edge")
		), 0
	);

	Parameters.Add_Choice(
		NULL	, "MODE"		, _TL("Search Mode"),
		_TL(""),
		CSG_String::Format("%s|%s|",
			_TL("Square"),
			_TL("Circle")
		), 1
	);

	Parameters.Add_Value(
		NULL	, "RADIUS"		, _TL("Radius"),
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
bool CFilter::On_Execute(void)
{
	//-----------------------------------------------------
	m_pInput	= Parameters("INPUT")->asGrid();

	CSG_Grid	Input, *pResult	= Parameters("RESULT")->asGrid();

	if( !pResult || pResult == m_pInput )
	{
		Parameters("RESULT")->Set_Value(pResult = m_pInput);
		Input.Create(*m_pInput);
		m_pInput	= &Input;
	}
	else
	{
		pResult->Set_Name(CSG_String::Format("%s [%s]", m_pInput->Get_Name(), _TL("Filter")));
		pResult->Set_NoData_Value(m_pInput->Get_NoData_Value());
	}

	//-----------------------------------------------------
	int	Method	= Parameters("METHOD")->asInt();

	m_Kernel.Set_Radius(Parameters("RADIUS")->asInt(), Parameters("MODE")->asInt() == 0);

	for(int y=0; y<Get_NY() && Set_Progress(y); y++)
	{
		#pragma omp parallel for
		for(int x=0; x<Get_NX(); x++)
		{
			double	Mean;

			if( Get_Mean(x, y, Mean) )
			{
				switch( Method )
				{
				default:	// Smooth...
					pResult->Set_Value(x, y, Mean);
					break;

				case  1:	// Sharpen...
					pResult->Set_Value(x, y, m_pInput->asDouble(x, y) + (m_pInput->asDouble(x, y) - Mean));
					break;

				case  2:	// Edge...
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

	m_Kernel.Destroy();

	//-----------------------------------------------------
	return( true );
}


///////////////////////////////////////////////////////////
//														 //
//														 //
//														 //
///////////////////////////////////////////////////////////

//---------------------------------------------------------
bool CFilter::Get_Mean(int x, int y, double &Value)
{
	CSG_Simple_Statistics	s;

	if( m_pInput->is_InGrid(x, y) )
	{
		for(int i=0; i<m_Kernel.Get_Count(); i++)
		{
			int	ix	= m_Kernel.Get_X(i, x);
			int	iy	= m_Kernel.Get_Y(i, y);

			if( m_pInput->is_InGrid(ix, iy) )
			{
				s	+= m_pInput->asDouble(ix, iy);
			}
		}
	}

	if( s.Get_Count() > 0 )
	{
		Value	= s.Get_Mean();

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

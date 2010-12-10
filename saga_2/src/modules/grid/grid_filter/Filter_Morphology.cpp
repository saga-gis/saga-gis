
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
//                 Filter_Morphology.cpp                 //
//                                                       //
//                 Copyright (C) 2010 by                 //
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
// 59 Temple Place - Suite 330, Boston, MA 02111-1307,   //
// USA.                                                  //
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


///////////////////////////////////////////////////////////
//														 //
//														 //
//														 //
///////////////////////////////////////////////////////////

//---------------------------------------------------------
#include "Filter_Morphology.h"


///////////////////////////////////////////////////////////
//														 //
//														 //
//														 //
///////////////////////////////////////////////////////////

//---------------------------------------------------------
CFilter_Morphology::CFilter_Morphology(void)
{
	//-----------------------------------------------------
	// 1. Info...

	Set_Name		(_TL("Morphological Filter"));

	Set_Author		(SG_T("O.Conrad (c) 2010"));

	Set_Description	(_TW(
		"Morphological filter for grids."
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

	Parameters.Add_Choice(
		NULL, "MODE"		, _TL("Search Mode"),
		_TL(""),
		CSG_String::Format(SG_T("%s|%s|"),
			_TL("Square"),
			_TL("Circle")
		), 1
	);

	Parameters.Add_Value(
		NULL, "RADIUS"		, _TL("Radius"),
		_TL(""),
		PARAMETER_TYPE_Int, 1, 1, true
	);

	Parameters.Add_Choice(
		NULL, "METHOD"		, _TL("Method"),
		_TL(""),
		CSG_String::Format(SG_T("%s|%s|%s|%s|"),
			_TL("Dilation"),
			_TL("Erosion"),
			_TL("Opening"),
			_TL("Closing")
		), 0
	);
}


///////////////////////////////////////////////////////////
//														 //
//														 //
//														 //
///////////////////////////////////////////////////////////

//---------------------------------------------------------
bool CFilter_Morphology::On_Execute(void)
{
	int			x, y, ix, iy, Method;
	double		Minimum, Maximum;
	CSG_Grid	*pResult, Result;

	//-----------------------------------------------------
	m_pInput	= Parameters("INPUT")	->asGrid();
	pResult		= Parameters("RESULT")	->asGrid();
	m_Radius	= Parameters("RADIUS")	->asInt();
	Method		= Parameters("METHOD")	->asInt();

	//-----------------------------------------------------
	m_Kernel.Create(SG_DATATYPE_Byte, 1 + 2 * m_Radius, 1 + 2 * m_Radius);
	m_Kernel.Set_NoData_Value(0.0);
	m_Kernel.Assign(1.0);
	m_Kernel.Set_Value(m_Radius, m_Radius, 0.0);

	if( Parameters("MODE")->asInt() == 1 )
	{
		for(y=-m_Radius, iy=0; y<=m_Radius; y++, iy++)
		{
			for(x=-m_Radius, ix=0; x<=m_Radius; x++, ix++)
			{
				if( x*x + y*y > m_Radius*m_Radius )
				{
					m_Kernel.Set_Value(ix, iy, 0.0);
				}
			}
		}
	}

	//-----------------------------------------------------
	if( !pResult || pResult == m_pInput )
	{
		pResult	= SG_Create_Grid(m_pInput);
	}
	else
	{
		pResult->Set_Name(CSG_String::Format(SG_T("%s [%s]"), m_pInput->Get_Name(), Parameters("METHOD")->asString()));

		pResult->Set_NoData_Value(m_pInput->Get_NoData_Value());
	}

	//-----------------------------------------------------
	if( Method == 2 || Method == 3 )
	{
		Result.Create(*Get_System());

		for(y=0; y<Get_NY() && Set_Progress(y); y++)
		{
			for(x=0; x<Get_NX(); x++)
			{
				if( Get_Range(x, y, Minimum, Maximum) )
				{
					switch( Method )
					{
					case 2:	Result.Set_Value(x, y, Minimum);	break;	// Opening = Erosion + Dilation
					case 3:	Result.Set_Value(x, y, Maximum);	break;	// Closing = Dilation + Erosion
					}
				}
				else
				{
					Result.Set_NoData(x, y);
				}
			}
		}

		m_pInput	= &Result;
	}

	for(y=0; y<Get_NY() && Set_Progress(y); y++)
	{
		for(x=0; x<Get_NX(); x++)
		{
			if( Get_Range(x, y, Minimum, Maximum) )
			{
				switch( Method )
				{
				case 0: case 2:	pResult->Set_Value(x, y, Maximum);	break;	// Dilation
				case 1: case 3:	pResult->Set_Value(x, y, Minimum);	break;	// Erosion
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
	}

	m_Kernel.Destroy();

	return( true );
}


///////////////////////////////////////////////////////////
//														 //
//														 //
//														 //
///////////////////////////////////////////////////////////

//---------------------------------------------------------
bool CFilter_Morphology::Get_Range(int x, int y, double &Minimum, double &Maximum)
{
	if( !m_pInput->is_InGrid(x, y) )
	{
		return( false );
	}

	Minimum	= Maximum	= m_pInput->asDouble(x, y);

	for(int iy=0, jy=y-m_Radius; iy<m_Kernel.Get_NY(); iy++, jy++)
	{
		for(int ix=0, jx=x-m_Radius; ix<m_Kernel.Get_NX(); ix++, jx++)
		{
			if( m_Kernel.asByte(ix, iy) && m_pInput->is_InGrid(jx, jy) )
			{
				double	z	= m_pInput->asDouble(jx, jy);

				if( Minimum > z )
				{
					Minimum	= z;
				}
				else if( Maximum < z )
				{
					Maximum	= z;
				}
			}
		}
	}

	return( true );
}


///////////////////////////////////////////////////////////
//														 //
//														 //
//														 //
///////////////////////////////////////////////////////////

//---------------------------------------------------------

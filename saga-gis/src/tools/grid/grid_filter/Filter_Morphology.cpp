/**********************************************************
 * Version $Id$
 *********************************************************/

///////////////////////////////////////////////////////////
//                                                       //
//                         SAGA                          //
//                                                       //
//      System for Automated Geoscientific Analyses      //
//                                                       //
//                     Tool Library                      //
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
// 51 Franklin Street, 5th Floor, Boston, MA 02110-1301, //
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
	Set_Name		(_TL("Morphological Filter"));

	Set_Author		("O.Conrad (c) 2010");

	Set_Description	(_TW(
		"Morphological filter for grids. "
		"Dilation returns the maximum and erosion the minimum value "
		"found in a cell's neighbourhood as defined by the kernel. "
		"Opening applies first an erosion followed by a dilation and "
		"closing is a dilation followed by an erosion. "
	));

	//-----------------------------------------------------
	Parameters.Add_Grid(NULL,
		"INPUT"			, _TL("Grid"),
		_TL(""),
		PARAMETER_INPUT
	);

	Parameters.Add_Grid(NULL,
		"RESULT"		, _TL("Filtered Grid"),
		_TL(""),
		PARAMETER_OUTPUT_OPTIONAL
	);

	Parameters.Add_Choice(NULL,
		"METHOD"		, _TL("Method"),
		_TL("Choose the operation to perform."),
		CSG_String::Format("%s|%s|%s|%s|",
			_TL("Dilation"),
			_TL("Erosion" ),
			_TL("Opening" ),
			_TL("Closing" )
		), 0
	);

	CSG_Grid_Cell_Addressor::Add_Parameters(Parameters);
}


///////////////////////////////////////////////////////////
//														 //
///////////////////////////////////////////////////////////

//---------------------------------------------------------
bool CFilter_Morphology::On_After_Execution(void)
{
	if( Parameters("RESULT")->asGrid() == Parameters("INPUT")->asGrid() )
	{
		Parameters("RESULT")->Set_Value(DATAOBJECT_NOTSET);
	}

	return( true );
}


///////////////////////////////////////////////////////////
//														 //
///////////////////////////////////////////////////////////

//---------------------------------------------------------
bool CFilter_Morphology::On_Execute(void)
{
	//-----------------------------------------------------
	if( !m_Kernel.Set_Parameters(Parameters) )
	{
		Error_Set(_TL("could not initialize kernel"));

		return( false );
	}

	CSG_Grid	*pInput 	= Parameters("INPUT" )->asGrid(), Tmp;
	CSG_Grid	*pResult	= Parameters("RESULT")->asGrid();

	if( !pResult )
	{
		pResult	= pInput;
	}

	//-----------------------------------------------------
	switch( Parameters("METHOD")->asInt() )
	{
	case 2:	// Opening (Erosion + Dilation)
		Get_Extreme(true , pInput, &Tmp);	pInput	= &Tmp;
		break;

	case 3:	// Closing (Dilation + Erosion)
		Get_Extreme(false, pInput, &Tmp);	pInput	= &Tmp;
		break;
	}

	//-----------------------------------------------------
	if( pResult == pInput )
	{
		Tmp.Create(*pInput);
		pResult	= pInput;
		pInput	= &Tmp;
	}

	switch( Parameters("METHOD")->asInt() )
	{
	case 0: case 2:	// Dilation, Opening (Erosion + Dilation)
		Get_Extreme(false, pInput, pResult);
		break;

	case 1: case 3:	// Erosion, Closing (Dilation + Erosion)
		Get_Extreme(true , pInput, pResult);
		break;
	}

	//-------------------------------------------------
	if( pResult == Parameters("INPUT")->asGrid() )
	{
		DataObject_Update(pResult);

		Parameters("RESULT")->Set_Value(pResult);
	}
	else
	{
		pResult->Set_Name(CSG_String::Format("%s [%s]",
			Parameters("INPUT")->asGrid()->Get_Name(),
			Parameters("METHOD")->asString()
		));
	}

	m_Kernel.Destroy();

	return( true );
}


///////////////////////////////////////////////////////////
//														 //
///////////////////////////////////////////////////////////

//---------------------------------------------------------
bool CFilter_Morphology::Get_Extreme(bool bMinimum, CSG_Grid *pInput, CSG_Grid *pResult)
{
	if( !Get_System()->is_Equal(pResult->Get_System()) )
	{
		pResult->Create(*Get_System());
	}

	for(int y=0; y<Get_NY() && Set_Progress(y); y++)
	{
		#pragma omp parallel for
		for(int x=0; x<Get_NX(); x++)
		{
			double	Value;

			if( Get_Extreme(bMinimum, pInput, x, y, Value) )
			{
				pResult->Set_Value(x, y, Value);
			}
			else
			{
				pResult->Set_NoData(x, y);
			}
		}
	}

	return( true );
}

//---------------------------------------------------------
bool CFilter_Morphology::Get_Extreme(bool bMinimum, CSG_Grid *pInput, int x, int y, double &Value)
{
	if( pInput->is_InGrid(x, y) )
	{
		CSG_Simple_Statistics	s;

		for(int i=0; i<m_Kernel.Get_Count(); i++)
		{
			int	ix	= m_Kernel.Get_X(i, x);
			int	iy	= m_Kernel.Get_Y(i, y);

			if( pInput->is_InGrid(ix, iy) )
			{
				s	+= pInput->asDouble(ix, iy);
			}
		}

		if( s.Get_Count() > 0 )
		{
			Value	= bMinimum ? s.Get_Minimum() : s.Get_Maximum();

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

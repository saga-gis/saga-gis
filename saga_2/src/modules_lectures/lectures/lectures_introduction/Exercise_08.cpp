
///////////////////////////////////////////////////////////
//                                                       //
//                         SAGA                          //
//                                                       //
//      System for Automated Geoscientific Analyses      //
//                                                       //
//                    Module Library:                    //
//                       Lectures                        //
//                                                       //
//-------------------------------------------------------//
//                                                       //
//                   Exercise_08.cpp                     //
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
// 59 Temple Place - Suite 330, Boston, MA 02111-1307,   //
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
#include "Exercise_08.h"


///////////////////////////////////////////////////////////
//														 //
//														 //
//														 //
///////////////////////////////////////////////////////////

//---------------------------------------------------------
CExercise_08::CExercise_08(void)
{
	//-----------------------------------------------------
	// Give some information about your module...

	Set_Name	(_TL("08: Extended neighbourhoods - catchment areas (parallel)"));

	Set_Author	(_TL("Copyrights (c) 2003 by Olaf Conrad"));

	Set_Description(_TL(
		"Extended Neighbourhoods - Catchment areas.\n"
		"(c) 2003 by Olaf Conrad, Goettingen\n"
		"email: oconrad@gwdg.de")
	);


	//-----------------------------------------------------
	// Define your parameters list...

	Parameters.Add_Grid(
		NULL, "ELEVATION"	, _TL("Elevation grid"),
		_TL("This must be your input data of type grid."),
		PARAMETER_INPUT
	);

	Parameters.Add_Grid(
		NULL, "AREA"		, _TL("Catchment area"),
		_TL("This will contain your output data of type grid."),
		PARAMETER_OUTPUT
	);

	Parameters.Add_Choice(
		NULL, "METHOD"		, _TL("Method"),
		_TL("Choose a method"),
		_TL(
		"D8|"
		"MFD|")
	);
}

//---------------------------------------------------------
CExercise_08::~CExercise_08(void)
{}


///////////////////////////////////////////////////////////
//														 //
//														 //
//														 //
///////////////////////////////////////////////////////////

//---------------------------------------------------------
bool CExercise_08::On_Execute(void)
{
	bool	bResult;
	int		Method;
	CSG_Colors	Colors;

	//-----------------------------------------------------
	// Get parameter settings...

	m_pDTM		= Parameters("ELEVATION")->asGrid();

	m_pArea		= Parameters("AREA"     )->asGrid();

	Method		= Parameters("METHOD")->asInt();


	m_pArea		->Assign(0.0);
	m_pArea		->Set_Unit("m²");
	Colors.Set_Ramp(SG_GET_RGB(255, 255, 255), SG_GET_RGB(0, 0, 127));
	DataObject_Set_Colors(m_pArea, Colors);


	//-----------------------------------------------------
	// Execute calculation...

	switch( Method )
	{
	case 0:
		bResult	= Method_01();
		break;

	case 1:
		bResult	= Method_02();
		break;

	default:
		bResult	= false;
	}


	//-----------------------------------------------------
	// Return 'true' if everything went okay...

	return( bResult );
}


///////////////////////////////////////////////////////////
//														 //
//														 //
//														 //
///////////////////////////////////////////////////////////

//---------------------------------------------------------
bool CExercise_08::Method_01(void)
{
	int		n, x, y, i, ix, iy, iMax;
	double	z, dz, dzMax,
			Area_of_Cell	= Get_Cellsize() * Get_Cellsize();

	//-----------------------------------------------------
	for(n=0; n<Get_NCells() && Set_Progress_NCells(n); n++)
	{
		m_pDTM->Get_Sorted(n, x, y);

		if( m_pDTM->is_NoData(x, y) )
		{
			m_pArea->Set_NoData(x, y);
		}
		else
		{
			m_pArea->Add_Value(x, y, Area_of_Cell);

			z		= m_pDTM->asDouble(x, y);
			iMax	= -1;

			for(i=0; i<8; i++)
			{
				ix		= Get_xTo(i, x);
				iy		= Get_yTo(i, y);

				if( is_InGrid(ix, iy) && !m_pDTM->is_NoData(ix, iy) )
				{
					dz	= (z - m_pDTM->asDouble(ix, iy)) / Get_Length(i);

					if( dz > 0.0 && (iMax < 0 || (iMax >= 0 && dzMax < dz)) )
					{
						iMax	= i;
						dzMax	= dz;
					}
				}
			}

			if( iMax >= 0 )
			{
				ix		= Get_xTo(iMax, x);
				iy		= Get_yTo(iMax, y);

				m_pArea->Add_Value(ix, iy, m_pArea->asDouble(x, y));
			}
		}
	}

	//-----------------------------------------------------
	return( true );
}

//---------------------------------------------------------
bool CExercise_08::Method_02(void)
{
	int		n, x, y, i, ix, iy;
	double	z, d, dz[8], dzSum,
			Area_of_Cell	= Get_Cellsize() * Get_Cellsize(),
			MFD_Converge	= 1.1;

	//-----------------------------------------------------
	for(n=0; n<Get_NCells() && Set_Progress_NCells(n); n++)
	{
		m_pDTM->Get_Sorted(n, x, y);

		if( m_pDTM->is_NoData(x, y) )
		{
			m_pArea->Set_NoData(x, y);
		}
		else
		{
			m_pArea->Add_Value(x, y, Area_of_Cell);

			z		= m_pDTM->asDouble(x, y);
			dzSum	= 0.0;

			for(i=0; i<8; i++)
			{
				ix		= Get_xTo(i, x);
				iy		= Get_yTo(i, y);

				if( is_InGrid(ix, iy) && !m_pDTM->is_NoData(ix, iy) && (d = z - m_pDTM->asDouble(ix, iy)) > 0.0 )
				{
					dz[i]	= pow(d / Get_Length(i), MFD_Converge);
					dzSum	+= dz[i];
				}
				else
				{
					dz[i]	= 0.0;
				}
			}

			if( dzSum > 0.0 )
			{
				d		= m_pArea->asDouble(x, y) / dzSum;

				for(i=0; i<8; i++)
				{
					if( dz[i] > 0.0 )
					{
						ix		= Get_xTo(i, x);
						iy		= Get_yTo(i, y);

						m_pArea->Add_Value(ix, iy, dz[i] * d);
					}
				}
			}
		}
	}

	//-----------------------------------------------------
	return( true );
}

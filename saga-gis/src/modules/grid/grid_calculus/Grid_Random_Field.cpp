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
//                     Grid_Calculus                     //
//                                                       //
//-------------------------------------------------------//
//                                                       //
//                 Grid_Random_Field.cpp                 //
//                                                       //
//                 Copyright (C) 2005 by                 //
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
#include "Grid_Random_Field.h"


///////////////////////////////////////////////////////////
//														 //
//														 //
//														 //
///////////////////////////////////////////////////////////

//---------------------------------------------------------
CGrid_Random_Field::CGrid_Random_Field(void)
{
	CSG_Parameter	*pNode;

	//-----------------------------------------------------
	Set_Name		(_TL("Random Field"));

	Set_Author		(SG_T("O.Conrad (c) 2005"));

	Set_Description	(_TW(
		"Create a grid with pseudo-random numbers as grid cell values. "
	));

	//-----------------------------------------------------
	Parameters.Add_Choice(
		NULL	, "TARGET"		, _TL("Target Grid"),
		_TL(""),
		CSG_String::Format(SG_T("%s|%s|"),
			_TL("user defined"),
			_TL("grid")
		), 0
	);

	m_Grid_Target.Add_Parameters_User(Add_Parameters("USER", _TL("User Defined Grid")	, _TL("")));
	m_Grid_Target.Add_Parameters_Grid(Add_Parameters("GRID", _TL("Choose Grid")			, _TL("")));

	//-----------------------------------------------------
	Parameters.Add_Choice(
		NULL	, "METHOD"		, _TL("Method"),
		_TL(""),
		CSG_String::Format(SG_T("%s|%s|"),
			_TL("Uniform"),
			_TL("Gaussian")
		), 1
	);

	//-----------------------------------------------------
	pNode	= Parameters.Add_Node(
		NULL	, "NODE_UNIFORM", _TL("Uniform"),
		_TL("")
	);

	Parameters.Add_Range(
		pNode	, "RANGE"		, _TL("Range"),
		_TL(""),
		0.0, 1.0
	);

	//-----------------------------------------------------
	pNode	= Parameters.Add_Node(
		NULL	, "NODE_GAUSS"	, _TL("Gaussian"),
		_TL("")
	);

	Parameters.Add_Value(
		pNode	, "MEAN"		, _TL("Arithmetic Mean"),
		_TL(""),
		PARAMETER_TYPE_Double, 0.0
	);

	Parameters.Add_Value(
		pNode	, "STDDEV"		, _TL("Standard Deviation"),
		_TL(""),
		PARAMETER_TYPE_Double, 1.0
	);
}


///////////////////////////////////////////////////////////
//														 //
///////////////////////////////////////////////////////////

//---------------------------------------------------------
int CGrid_Random_Field::On_Parameter_Changed(CSG_Parameters *pParameters, CSG_Parameter *pParameter)
{
	return( m_Grid_Target.On_User_Changed(pParameters, pParameter) ? 1 : 0 );
}

//---------------------------------------------------------
int CGrid_Random_Field::On_Parameters_Enable(CSG_Parameters *pParameters, CSG_Parameter *pParameter)
{
	if( !SG_STR_CMP(pParameter->Get_Identifier(), SG_T("METHOD")) )
	{
		pParameters->Get_Parameter("NODE_UNIFORM")->Set_Enabled(pParameter->asInt() == 0);
		pParameters->Get_Parameter("NODE_GAUSS"  )->Set_Enabled(pParameter->asInt() == 1);
	}

	return( 0 );
}


///////////////////////////////////////////////////////////
//														 //
///////////////////////////////////////////////////////////

//---------------------------------------------------------
bool CGrid_Random_Field::On_Execute(void)
{
	//-----------------------------------------------------
	CSG_Grid	*pGrid	= NULL;

	switch( Parameters("TARGET")->asInt() )
	{
	case 0:	// user defined...
		if( m_Grid_Target.Init_User(0.0, 0.0, 1.0, 100, 100) && Dlg_Parameters("USER") )
		{
			pGrid	= m_Grid_Target.Get_User();
		}
		break;

	case 1:	// grid...
		if( Dlg_Parameters("GRID") )
		{
			pGrid	= m_Grid_Target.Get_Grid();
		}
		break;
	}

	if( !pGrid )
	{
		return( false );
	}

	pGrid->Set_Name(_TL("Random Field"));

	//-----------------------------------------------------
	int		Method	= Parameters("METHOD")->asInt();

	double	a	= Method == 0
		? Parameters("RANGE" )->asRange()->Get_LoVal()
		: Parameters("MEAN"  )->asDouble();

	double	b	= Method == 0
		? Parameters("RANGE" )->asRange()->Get_HiVal()
		: Parameters("STDDEV")->asDouble();

	//-----------------------------------------------------
	for(int y=0; y<pGrid->Get_NY() && Set_Progress(y, pGrid->Get_NY()); y++)
	{
		#pragma omp parallel for
		for(int x=0; x<pGrid->Get_NX(); x++)
		{
			switch( Method )
			{
			case 0:	// uniform...
				pGrid->Set_Value(x, y, CSG_Random::Get_Uniform (a, b));
				break;

			case 1:	// gaussian...
				pGrid->Set_Value(x, y, CSG_Random::Get_Gaussian(a, b));
				break;
			}
		}
	}

	//-----------------------------------------------------
	return( true );
}


///////////////////////////////////////////////////////////
//														 //
//														 //
//														 //
///////////////////////////////////////////////////////////

//---------------------------------------------------------

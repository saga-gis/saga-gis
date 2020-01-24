
/*******************************************************************************
    Grid_CVA.cpp
    Copyright (C) Victor Olaya
    
    This program is free software; you can redistribute it and/or modify
    it under the terms of the GNU General Public License as published by
    the Free Software Foundation; either version 2 of the License, or
    (at your option) any later version.

    This program is distributed in the hope that it will be useful,
    but WITHOUT ANY WARRANTY; without even the implied warranty of
    MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
    GNU General Public License for more details.

    You should have received a copy of the GNU General Public License
    along with this program; if not, write to the Free Software
    Foundation, Inc., 51 Franklin Street, 5th Floor, Boston, MA 02110-1301, USA
*******************************************************************************/ 

///////////////////////////////////////////////////////////
//                                                       //
//                                                       //
//                                                       //
///////////////////////////////////////////////////////////

//---------------------------------------------------------
#include "Grid_CVA.h"


///////////////////////////////////////////////////////////
//                                                       //
//                                                       //
//                                                       //
///////////////////////////////////////////////////////////

//---------------------------------------------------------
CGrid_CVA::CGrid_CVA(void)
{
	Set_Name		(_TL("Change Vector Analysis"));

	Set_Author		("V.Olaya (c) 2004, O.Conrad (c) 2012");

	Set_Description	(_TW(
		"This tool performs a change vector analysis (CVA) for the given input features. "
		"Input features are supplied as grid lists for initial and final state. "
		"In both lists features have to be given in the same order. "
		"Distance is measured as Euclidean distance in features space. When analyzing two features "
		"direction is calculated as angle (radians) by default. Otherwise direction is coded as "
		"the quadrant it points to in terms of feature space. "
	));

	Parameters.Add_Grid_List("",
		"A"		, _TL("Initial State"), 
		_TL(""),
		PARAMETER_INPUT
	);

	Parameters.Add_Grid_List("",
		"B"		, _TL("Final State"),
		_TL(""), 
		PARAMETER_INPUT
	);

	Parameters.Add_Grid("",
		"DIST"	, _TL("Distance"), 
		_TL(""), 
		PARAMETER_OUTPUT
	);

	Parameters.Add_Grid("",
		"DIR"	, _TL("Angle"), 
		_TL(""), 
		PARAMETER_OUTPUT_OPTIONAL
	);

	Parameters.Add_Grids("",
		"C"		, _TL("Change Vector"),
		_TL(""), 
		PARAMETER_OUTPUT_OPTIONAL
	);
}


///////////////////////////////////////////////////////////
//                                                       //
///////////////////////////////////////////////////////////

//---------------------------------------------------------
int CGrid_CVA::On_Parameters_Enable(CSG_Parameters *pParameters, CSG_Parameter *pParameter)
{
	if(	pParameter->Cmp_Identifier("A")
	||	pParameter->Cmp_Identifier("B") )
	{
		pParameters->Set_Enabled("ANGLE",
				(*pParameters)("A")->asInt() == 2
			&&	(*pParameters)("B")->asInt() == 2
		);
	}

	return( CSG_Tool_Grid::On_Parameters_Enable(pParameters, pParameter) );
}


///////////////////////////////////////////////////////////
//                                                       //
///////////////////////////////////////////////////////////

//---------------------------------------------------------
bool CGrid_CVA::On_Execute(void)
{	
	CSG_Parameter_Grid_List	*pA	= Parameters("A")->asGridList();
	CSG_Parameter_Grid_List	*pB	= Parameters("B")->asGridList();

	int	nFeatures	= pA->Get_Grid_Count();

	if( nFeatures != pB->Get_Grid_Count() )
	{
		Error_Set(_TL("number of initial and final state grids differs"));

		return( false );
	}

	if( nFeatures == 0 )
	{
		Error_Set(_TL("no grids in list"));

		return( false );
	}

	//-----------------------------------------------------
	CSG_Grids	*pC	= Parameters("C")->asGrids();

	if( pC )
	{
		if( !pC->Create(Get_System(), nFeatures) || !pC->is_Valid() || pC->Get_NZ() < nFeatures )
		{
			pC	= NULL;
		}
		else
		{
			pC->Set_Name(_TL("Change Vector"));

			pC->Add_Attribute("A", SG_DATATYPE_String);
			pC->Add_Attribute("B", SG_DATATYPE_String);

			for(int i=0; i<nFeatures; i++)
			{
				pC->Set_Z(i, i + 1);

				pC->Get_Attributes()[i].Set_Value(1, pA->Get_Grid(i)->Get_Name());
				pC->Get_Attributes()[i].Set_Value(2, pB->Get_Grid(i)->Get_Name());
			}

			pC->Get_Attributes_Ptr()->Set_Field_Type(0, SG_DATATYPE_Word);
		}
	}

	//-----------------------------------------------------
	CSG_Colors	Colors;
	Colors.Set_Ramp(SG_GET_RGB(255, 255, 255), SG_GET_RGB(  0, 127, 127), 0, Colors.Get_Count() / 2);
	Colors.Set_Ramp(SG_GET_RGB(  0, 127, 127), SG_GET_RGB(255,   0,   0),    Colors.Get_Count() / 2, Colors.Get_Count());

	CSG_Grid	*pLength = Parameters("DIST")->asGrid();
	CSG_Grid	*pAngle  = Parameters("DIR" )->asGrid();

	DataObject_Set_Colors(pLength, Colors);
	DataObject_Set_Colors(pAngle , Colors);

	//-----------------------------------------------------
    for(int y=0; y<Get_NY() && Set_Progress(y); y++)
	{
		#pragma omp parallel for
		for(int x=0; x<Get_NX(); x++)
		{
			bool	bOkay	= true;	CSG_Vector	a(nFeatures), b(nFeatures);

			for(int i=0; bOkay && i<nFeatures; i++)
			{
				if( (bOkay = !pA->Get_Grid(i)->is_NoData(x, y) && !pB->Get_Grid(i)->is_NoData(x, y)) == true )
				{
					a[i]	= pA->Get_Grid(i)->asDouble(x, y);
					b[i]	= pB->Get_Grid(i)->asDouble(x, y);
				}
			}

			//---------------------------------------------
			if( bOkay == false )
			{
				if( pLength ) pLength->Set_NoData(x, y);
				if( pAngle  ) pAngle ->Set_NoData(x, y);

				for(int i=0; pC && i<nFeatures; i++)
				{
					pC->Set_NoData(x, y, i);
				}
			}
			else
			{
				CSG_Vector	c	= b - a;

				if( pLength ) pLength->Set_Value(x, y, c.Get_Length());
				if( pAngle  ) pAngle ->Set_Value(x, y, a.Get_Angle(b) * M_RAD_TO_DEG);

				for(int i=0; pC && i<nFeatures; i++)
				{
					pC->Set_Value(x, y, i, c[i]);
				}
			}
        }
    }

	//-----------------------------------------------------
	return( true );
}


///////////////////////////////////////////////////////////
//                                                       //
//                                                       //
//                                                       //
///////////////////////////////////////////////////////////

//---------------------------------------------------------

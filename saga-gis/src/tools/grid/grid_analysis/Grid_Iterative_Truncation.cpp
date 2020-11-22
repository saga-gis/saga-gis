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
//                     grid_analysis                     //
//                                                       //
//-------------------------------------------------------//
//                                                       //
//              Grid_Iterative_Truncation.cpp            //
//                                                       //
//                 Copyright (C) 2020 by                 //
//                    Volker Wichmann                    //
//                                                       //
//-------------------------------------------------------//
//                                                       //
// This file is part of 'SAGA - System for Automated     //
// Geoscientific Analyses'. SAGA is free software; you   //
// can redistribute it and/or modify it under the terms  //
// of the GNU General Public License as published by the //
// Free Software Foundation, either version 2 of the     //
// License, or (at your option) any later version.       //
//                                                       //
// SAGA is distributed in the hope that it will be       //
// useful, but WITHOUT ANY WARRANTY; without even the    //
// implied warranty of MERCHANTABILITY or FITNESS FOR A  //
// PARTICULAR PURPOSE. See the GNU General Public        //
// License for more details.                             //
//                                                       //
// You should have received a copy of the GNU General    //
// Public License along with this program; if not, see   //
// <http://www.gnu.org/licenses/>.                       //
//                                                       //
//-------------------------------------------------------//
//                                                       //
//    e-mail:     wichmann@laserdata                     //
//                                                       //
//    contact:    Volker Wichmann                        //
//                LASERDATA GmbH                         //
//                Management and analysis of             //
//                laserscanning data                     //
//                Innsbruck, Austria                     //
//                                                       //
///////////////////////////////////////////////////////////

//---------------------------------------------------------


///////////////////////////////////////////////////////////
//														 //
//														 //
//														 //
///////////////////////////////////////////////////////////

//---------------------------------------------------------
#include "Grid_Iterative_Truncation.h"


///////////////////////////////////////////////////////////
//														 //
//														 //
//														 //
///////////////////////////////////////////////////////////

//-----------------------------------------------------------
CGrid_Iterative_Truncation::CGrid_Iterative_Truncation(void)
{
	Set_Name		(_TL("Iterative Truncation"));

	Set_Author		("V. Wichmann (c) 2020");

	Parameters.Set_Description(_TW(
		"The tool allows one to perform an iterative truncation to a target average. This operation "
        "iteratively removes the highest values from the input grid until the average of all grid values "
        "matches the user-specified target average. Instead of simply removing the highest cell values, "
        "these values can also be replaced by a substitute value.\n"
		"An example application is surface soil cleanup, where the highest soil contaminant concentrations "
        "are removed until targeted post-remediation concentrations are reached. In this case, the "
        "substitute value would be set to the concentration of clean fill.\n\n"
	));


    Parameters.Add_Grid("",
		"INPUT"		, _TL("Input"), 
		_TL("Grid to analyse."), 
		PARAMETER_INPUT
	);

	Parameters.Add_Grid("",
		"REMOVED"	, _TL("Removed Cells"), 
		_TL("Output grid showing the removed cells (1/NoData)."), 
		PARAMETER_OUTPUT, true, SG_DATATYPE_Char
	);

	Parameters.Add_Grid("",
		"OUTPUT"	, _TL("Output"), 
		_TL("The modified input grid."), 
		PARAMETER_OUTPUT_OPTIONAL
	);

    Parameters.Add_Double("",
        "TARGET", _TL("Target Average"), 
        _TL("The target average."), 
        100.0
    );

	Parameters.Add_Choice("",
		"METHOD"	, _TL("Method"),
		_TL("Choose a mode of operation."),
		CSG_String::Format("%s|%s",
			_TL("remove cell values"),
			_TL("replace cell values")
		),	0
	);

	Parameters.Add_Double("METHOD",
		"SUBSTITUTE", _TL("Substitute Value"), 
		_TL("The value with which the removed cell values are replaced."), 
		0.0
	);
}


///////////////////////////////////////////////////////////
//														 //
///////////////////////////////////////////////////////////

//---------------------------------------------------------
int CGrid_Iterative_Truncation::On_Parameters_Enable(CSG_Parameters *pParameters, CSG_Parameter *pParameter)
{
	if(	pParameter->Cmp_Identifier("METHOD") )
	{
		pParameters->Set_Enabled("SUBSTITUTE", pParameter->asInt() == 1);
	}

	return( 1 );
}


///////////////////////////////////////////////////////////
//														 //
///////////////////////////////////////////////////////////

//---------------------------------------------------------
bool CGrid_Iterative_Truncation::On_Execute(void)
{

    CSG_Grid    *pInput		= Parameters("INPUT")->asGrid();
    CSG_Grid    *pRemoved	= Parameters("REMOVED")->asGrid();
    CSG_Grid    *pOutput	= Parameters("OUTPUT")->asGrid();
    double      dTarget     = Parameters("TARGET")->asDouble();
	int         iMethod		= Parameters("METHOD")->asInt();
    double      dSubstitute  = Parameters("SUBSTITUTE")->asDouble();
    double      dSum        = 0.0;
    sLong       iCells      = 0;

    
    //-------------------------------------------------
    pRemoved->Assign_NoData();

    if( pOutput != NULL )
    {
        pOutput->Assign(pInput);
    }

    //-------------------------------------------------
    for(int y=0; y<Get_NY() && Set_Progress(y); y++)
    {
        for(int x=0; x<Get_NX(); x++)
        {
            if( !pInput->is_NoData(x, y) )
            {
                dSum += pInput->asDouble(x, y);
                iCells++;
            }
        }
    }


    //-------------------------------------------------
    if( !pInput->Set_Index() )
    {
        Error_Set(_TL("index creation failed"));

        return( false );
    }

    int     x, y;
    double  dAverage = dSum / iCells;
    bool    bReached = false;

    Message_Fmt("\n%s: %.2f", _TL("Average of input grid"), dAverage);


    //-------------------------------------------------
	for(sLong n=0; n<Get_NCells() && Set_Progress_NCells(n); n++)
	{
        if( dAverage <= dTarget )
        {
            Message_Fmt("\n%s: %.2f", _TL("Target average reached"), dAverage);
            bReached = true;
            break;
        }

		if( pInput->Get_Sorted(n, x, y) )
        {
            pRemoved->Set_Value(x, y, 1.0);

            switch (iMethod)
            {
                default:
                case 0:
                {
                    if( pOutput != NULL )
                    {
                        pOutput->Set_NoData(x, y);
                    }

                    dSum -= pInput->asDouble(x, y);
                    iCells--;
                    break;
                }
                case 1:
                {
                    if( pOutput != NULL )
                    {
                        pOutput->Set_Value(x, y, dSubstitute);
                    }

                    dSum = dSum - pInput->asDouble(x, y) + dSubstitute;
                    break;
                }
            }

            if( iCells > 0 )
            {
                dAverage = dSum / iCells;
            }
        }
    }


    //-----------------------------------------------------
    if( !bReached )
    {
        Message_Fmt("\n%s: %.2f", _TL("Target average NOT reached"), dAverage);
        Error_Fmt("\n%s: %.2f", _TL("Target average NOT reached"), dAverage);

        return( false );
    }
	
	return( true );
}


///////////////////////////////////////////////////////////
//														 //
//														 //
//														 //
///////////////////////////////////////////////////////////

//---------------------------------------------------------

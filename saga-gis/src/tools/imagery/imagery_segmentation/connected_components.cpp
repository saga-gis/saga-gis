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
//                 imagery_segmentation                  //
//                                                       //
//-------------------------------------------------------//
//                                                       //
//                connected_components.cpp               //
//                                                       //
//                 Copyright (C) 2021 by                 //
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
#include "connected_components.h"


///////////////////////////////////////////////////////////
//														 //
//														 //
//														 //
///////////////////////////////////////////////////////////

//-----------------------------------------------------------
CConnected_Components::CConnected_Components(void)
{
	Set_Name		(_TL("Connected Component Labeling"));

	Set_Author		("V. Wichmann (c) 2021");

	Parameters.Set_Description(_TW(
		"The tool allows one to label subsets of connected components with a unique identifier. "
        "Connected component labeling (CCL) is an operator which turns a binary image into "
        "a symbolic image in which the label assigned to each pixel is an integer uniquely identifying "
        "the connected component to which that pixel belongs (Shapiro 1996).\n"
        "The tool takes a grid as input and treats it as a binary image. The foreground is defined by "
        "all cell values greater than zero, the background by NoData cells and all cell values less than one. "
		"Connectivity can be determined by analysing either a 4-connected or a 8-connected neighborhood.\n\n"
	));

    Add_Reference(
        "Shapiro, L.G.", "1996",
        "Connected Component Labeling and Adjacency Graph Construction",
        "Machine Intelligence and Pattern Recognition, Vol. 19, p.1-30.",
        SG_T("https://doi.org/10.1016/S0923-0459(96)80011-5"), _TL("Link")
    );

    Parameters.Add_Grid("",
		"INPUT"		, _TL("Input"), 
		_TL("Grid to analyse."), 
		PARAMETER_INPUT
	);

	Parameters.Add_Grid("",
		"OUTPUT"	, _TL("Output"), 
		_TL("Output grid with labeled components."), 
		PARAMETER_OUTPUT, true, SG_DATATYPE_Long
	);

    Parameters.Add_Choice("",
        "NEIGHBOUR"	, _TL("Neighbourhood"),
        _TL(""),
        CSG_String::Format("%s|%s",
            _TL("4 (von Neumann)"),
            _TL("8 (Moore)")
        ), 1
    );
}


///////////////////////////////////////////////////////////
//														 //
///////////////////////////////////////////////////////////

//---------------------------------------------------------
bool CConnected_Components::On_Execute(void)
{

    CSG_Grid    *pInput		= Parameters("INPUT")->asGrid();
    CSG_Grid    *pOutput	= Parameters("OUTPUT")->asGrid();
    int         iNeighbour	= Parameters("NEIGHBOUR" )->asInt() == 0 ? 2 : 1;


    //-------------------------------------------------
    pOutput->Fmt_Name("%s [%s]", pInput->Get_Name(), SG_T("CCL"));
    pOutput->Assign_NoData();

    sLong               iIdentifier = 0;
    std::queue<sLong>   qFIFO;

    //-------------------------------------------------
	for(sLong n=0; n<Get_NCells() && Set_Progress_Cells(n); n++)
	{
        if( pInput->is_NoData(n) || pInput->asDouble(n) < 1.0 || !pOutput->is_NoData(n) )
        {
            continue;
        }

        iIdentifier++;

        pOutput->Set_Value(n, iIdentifier);

        qFIFO.push(n);

        while (qFIFO.size() > 0)
        {
            sLong nc = qFIFO.front();

            int x = (int)(nc % Get_NX());
            int y = (int)(nc / Get_NX());

            for(int i=0, ix, iy; i<8; i+=iNeighbour)
            {
                if( Get_System().Get_Neighbor_Pos(i, x, y, ix, iy) && !pInput->is_NoData(ix, iy)
                    && pInput->asDouble(ix, iy) > 0.0 && pOutput->is_NoData(ix, iy) )
                {
                    pOutput->Set_Value(ix, iy, iIdentifier);

                    qFIFO.push(ix + Get_NX() * iy);
                }
            }

            qFIFO.pop();
        }
    }
    

    //-----------------------------------------------------
    SG_UI_Msg_Add(CSG_String::Format("%s: %lld", _TL("Number of connected components"), iIdentifier), true);
	
    DataObject_Update(pOutput);


	return( true );
}


///////////////////////////////////////////////////////////
//														 //
//														 //
//														 //
///////////////////////////////////////////////////////////

//---------------------------------------------------------

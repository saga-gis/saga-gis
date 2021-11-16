
///////////////////////////////////////////////////////////
//                                                       //
//                         SAGA                          //
//                                                       //
//      System for Automated Geoscientific Analyses      //
//                                                       //
//                     Tool Library                      //
//                     shapes_lines                      //
//                                                       //
//-------------------------------------------------------//
//                                                       //
//                     cut_lines.cpp                     //
//                                                       //
//                 Copyright (C) 2021 by                 //
//                   Justus Spitzmüller                  //
//                     scilands GmbH                     //
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
//    e-mail:     jspitzmueller@scilands.de              //
//                                                       //
//    contact:    Justus Spitzmüller                     //
//                scilands GmbH                          //
//                Goethe-Allee 11                        //
//                37073 Göttingen                        //
//                Germany                                //
//                                                       //
///////////////////////////////////////////////////////////
#include "cut_lines.h"

CCut_Lines::CCut_Lines(void)
{
	Set_Name(_TL("Cut Lines"));

	Set_Author(_TL("Justus Spitzmüller, scilands GmbH \u00a9 2021"));

	Set_Version("1.1");

	Set_Description (_TW(
		"Tool cuts lines in a regular distance and assigns shape- and segment-numbers along with an ongoing identifier."
		"The Tool only works with projected datasets and will terminate by other projection-units than meter. In this case set the coordinate reference system if it is missing or re-project it."
		"The Output will named after the input with a \"_cut_xx\" suffix where xx is the length with truncated decimals (to prevent filesystem complications)"
		"\n\n"
		"&#8226 ")
		

	);

	Parameters.Add_Shapes(
		NULL, "INPUT", _TL("Input Lines"), _TL("Input Line Shapefile"),
		PARAMETER_INPUT, SHAPE_TYPE_Line
	);

	Parameters.Add_Shapes(
		NULL, "OUTPUT", _TL("Output Lines"), _TL("Output Line Shapefile"),
		PARAMETER_OUTPUT, SHAPE_TYPE_Line
	);

	// TODO Better better description
	Parameters.Add_Choice(
		NULL, "DISTRIBUTION", _TL("Distribution"), _TL(""),
		CSG_String::Format("%s|%s|", _TL("By Length"), _TL("By Number")), 0
	);

	Parameters.Add_Double(
		"DISTRIBUTION", "LENGTH", _TL("Length"), _TL("Length where the lines will be cutted in Meter"), 5.0, 0.0, true
	);

	Parameters.Add_Int(
		"DISTRIBUTION", "NUMBER", _TL("Number"), _TL("Number of cuts per line"), 5, 0, true
	);


}

CCut_Lines::~CCut_Lines(void)
{}


int CCut_Lines::On_Parameters_Enable(CSG_Parameters *pParameters, CSG_Parameter *pParameter)
{
	if( pParameter->Cmp_Identifier("DISTRIBUTION") )
	{
		pParameters->Set_Enabled("LENGTH", pParameter->asInt() == 0);
		pParameters->Set_Enabled("NUMBER", pParameter->asInt() == 1);
	}

	return( CSG_Tool::On_Parameters_Enable( pParameters, pParameter ) );
}

bool CCut_Lines::On_Execute(void)
{
	CSG_Shapes	*pInputLines 	= Parameters("INPUT")->asShapes();
	CSG_Shapes	*pOutputLines 	= Parameters("OUTPUT")->asShapes();

	double No_Data_Value = pOutputLines->Get_NoData_Value();
	double Cut_Length = Parameters("LENGTH")->asDouble();

	// Check for projection unit. This tool only works with projected
	// Coordinate Reference Systems and assumes meter. Could be extended
	if( pInputLines->Get_Projection().Get_Unit() != SG_PROJ_UNIT_Meter )
	{
		Error_Set(_TL("The input line feature has an other projection unit then meter."));
		return false;
	}

	// Remove everything. Set the name (input + '_cut_xx' xx=length) and
	// add the line and segment number and the id-field. Remember the field indexes
	pOutputLines->Destroy();
	pOutputLines->Set_Name(CSG_String::Format("%s_cut_%d", pInputLines->Get_Name(), (int) Cut_Length ));
	pOutputLines->Add_Field("NR_LINE", SG_DATATYPE_Int);
	pOutputLines->Add_Field("NR_SEGM", SG_DATATYPE_Int);
	pOutputLines->Add_Field("ID_SEGM", SG_DATATYPE_Int);

	int Field_Nr_Line = pOutputLines->Get_Field("NR_LINE");
	int Field_Nr_Segm = pOutputLines->Get_Field("NR_SEGM");
	int Field_ID_Segm = pOutputLines->Get_Field("ID_SEGM");

	// Start the ongoing id-index and set the overhang to 0.
	// Note: The Overhang is one of the crucial parts. This accumulates 
	// the distance walked since the last point.
	int ID_Segm = 1;
	double Distance_Overhang = 0.;

	for( int i=0; i<pInputLines->Get_Count(); i++ )
	{
		CSG_Shape_Line *pLine = pInputLines->Get_Shape(i)->asLine();

		for( int j=0; j<pLine->Get_Part_Count(); j++ )
		{
			// Get the current part and reset the overhang.
			CSG_Shape_Part *pPart = pLine->Get_Part(j);	
			Distance_Overhang = 0.0;

			// Only cut lines ( a line has > 2 points )
			if( pPart->Get_Count() > 1 )
			{
				// Reset the Segment Number and write the meta-data
				// in the new shape. Increment ongoing counter
				// TODO Is the Segment Number at this place smart???
				int Nr_Segm = 1;
				CSG_Shape *pSegment = pOutputLines->Add_Shape();
				pSegment->Set_Value( Field_Nr_Line, i );
				pSegment->Set_Value( Field_Nr_Segm, Nr_Segm );
				pSegment->Set_Value( Field_ID_Segm, ID_Segm );

				Nr_Segm++;
				ID_Segm++;

				// Write the first Point to the new shape
				pSegment->Add_Point( pPart->Get_Point(0) ); 

				// This loop investigates all segments. A segment is a
				// line between two points so n-1 operations (k=1)
				for( int k=1; k<pPart->Get_Count(); k++ )
				{
					// Investigate the next segment, get the length and reductor
					TSG_Point Front = pPart->Get_Point(k-1);
					TSG_Point Back 	= pPart->Get_Point(k);
					double Length_Seg 	= SG_Get_Distance( Front, Back );
					double Seg_Reductor = Length_Seg;

					// Check if there is still space left in the segment to fit 
					// a cut considering the overhang
					//	
					// ~-Distance_Overhang-|---Seg_Reductor-------|
					// --o--------------------------x-------------o--------
					// --------------Cut_Length-----|\_position of cut
					//
					while( Seg_Reductor + Distance_Overhang > Cut_Length )
					{
						// Decrement the reductor and reset the overhang
						Seg_Reductor -= ( Cut_Length - Distance_Overhang );
						Distance_Overhang = 0.0;


						// Construct the intermediate point with the angel and the
						// length.
						double Construct_Angle 	= SG_Get_Angle_Of_Direction( Front, Back );
						double Construct_Dist 	= Length_Seg - Seg_Reductor; 
						TSG_Point Intermediate_Point;
						Intermediate_Point.x = Front.x + ( Construct_Dist * sin(Construct_Angle) );
						Intermediate_Point.y = Front.y + ( Construct_Dist * cos(Construct_Angle) );

						// Add the intermediate point as the last point, add a new 
						// shape and add the intermediate as the first point and
						// set the metadata and also increment the counter
						pSegment->Add_Point( Intermediate_Point );
						pSegment = pOutputLines->Add_Shape();
						pSegment->Set_Value( Field_Nr_Line, i );
						pSegment->Set_Value( Field_Nr_Segm, Nr_Segm );
						pSegment->Set_Value( Field_ID_Segm, ID_Segm );
						pSegment->Add_Point( Intermediate_Point );

						Nr_Segm++;
						ID_Segm++;

					}

					// Close the shape with the last point
					pSegment->Add_Point( Back );

					// Accumulate the overhang
					// Note: this happens inside the loop over the points (Index k)
					Distance_Overhang += Seg_Reductor;
				}
			}
		}
	}
	return true;
}

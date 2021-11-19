
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
#include "line_split.h"

CCut_Lines::CCut_Lines(void)
{
	Set_Name(_TL("Split Lines"));

	Set_Author(_TL("Justus Spitzm\u00fcller, scilands GmbH \u00a9 2021"));

	Set_Version("1.3");

	Set_Description (_TW(
		"The Tool provides methods to split lines in multiple lines based on a regular distance or number"
		)
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
		CSG_String::Format("%s|%s|",
			_TL("By Length"), 
			_TL("By Number")
		), 0
	);

	Parameters.Add_Double(
		"DISTRIBUTION", "LENGTH", _TL("Length"), _TL("Length where the lines will be cutted in map-units"), 5.0, 0.0, true
	);

	Parameters.Add_Choice(
		"DISTRIBUTION", "CAPS_LENGTH", _TL("Caps"), _TL(""),
		CSG_String::Format("%s|%s|%s|",
			_TL("Start Full Length"), 
			_TL("Start Remaining Length"), 
			_TL("Even Ends")
		), 0
	);

	Parameters.Add_Int(
		"DISTRIBUTION", "NUMBER", _TL("Number Of Splits"), _TL("Number of splits per line"), 5, 0, true
	);

	Parameters.Add_Choice(
		"DISTRIBUTION", "CAPS_NUMBER", _TL("Caps"), _TL(""),
		CSG_String::Format("%s|%s|",
			_TL("Full Segment"),
			_TL("Half Segment")
		),0
	);

//	Parameters.Add_Choice(
//		NULL, "FEATURE", _TL("Cut Into"), _TL("Cut and put every piece in a individual Shape or organize the pieces of one Shape in Parts"),
//		CSG_String::Format("%s|%s|", _TL("Shapes"), _TL("Parts")),0
//	);



}

CCut_Lines::~CCut_Lines(void)
{}


int CCut_Lines::On_Parameters_Enable(CSG_Parameters *pParameters, CSG_Parameter *pParameter)
{
	if( pParameter->Cmp_Identifier("DISTRIBUTION") )
	{
		pParameters->Set_Enabled("LENGTH", 		pParameter->asInt() == 0);
		pParameters->Set_Enabled("CAPS_LENGTH", pParameter->asInt() == 0);
		pParameters->Set_Enabled("NUMBER", 		pParameter->asInt() == 1);
		pParameters->Set_Enabled("CAPS_NUMBER", pParameter->asInt() == 1);
	}

	return( CSG_Tool::On_Parameters_Enable( pParameters, pParameter ) );
}

bool CCut_Lines::On_Execute(void)
{
	CSG_Shapes	*pInputLines 		= Parameters("INPUT")->asShapes();
	CSG_Shapes	*pOutputLines 		= Parameters("OUTPUT")->asShapes();
	int			Distribution_Option = Parameters("DISTRIBUTION")->asInt();
	int			Caps_Length_Option	= Parameters("CAPS_LENGTH")->asInt();
	int			Caps_Number_Option  = Parameters("CAPS_NUMBER")->asInt();
	double		Length				= Parameters("LENGTH")->asDouble();
	int 		Number				= Parameters("NUMBER")->asInt();

	if( pInputLines == pOutputLines )
	{
		Error_Set(_TL("You selected the input also as the output. This would destroy your input! Abort!"));
		return false;
	}

	// Check for projection unit. This tool only works with projected
	// Coordinate Reference Systems and assumes meter. Could be extended
	//if( pInputLines->Get_Projection().Get_Unit() != SG_PROJ_UNIT_Meter )
	//{
	//	Error_Set(_TL("The input line feature has an other projection unit then meter. Abort!"));
	//	return false;
	//}

	// Remove everything. Set the name (input + '_cut_xx' xx=length) and
	// add the line and segment number and the id-field. Remember the field indexes
	pOutputLines->Assign(pInputLines);
	pOutputLines->Del_Shapes();
	pOutputLines->Set_Name(CSG_String::Format("%s_cut", pInputLines->Get_Name()));
	//pOutputLines->Set_Name(CSG_String::Format("%s_cut_%d", pInputLines->Get_Name(), (int) Cut_Length ));

	// Note: The Overhang is one of the crucial parts. This accumulates 
	// the distance walked since the last point. 
	// Switch_To_Default (Length) will happen after the first cut.
	double 	Distance_Overhang = 0.;
	bool 	Switch_To_Default = false;

	for( int i=0; i<pInputLines->Get_Count(); i++ )
	{
		CSG_Shape_Line *pLine = pInputLines->Get_Shape(i)->asLine();

		for( int j=0; j<pLine->Get_Part_Count(); j++ )
		{
			CSG_Shape_Part *pPart = pLine->Get_Part(j);	

			// This tool supports different cap styles. 			
			// The default length is the length used for every mid part. Initialize 
			// the target length witch the cap if needed. This will be set to default 
			// after the fist cut. The end-cap is inherent.
			double Default_Length 	 = 0.0;
			double Target_Cut_Length = 0.0;

			// Length distribution options
			// 0: Start Full Length 		= Reset the overhang and set length to parameter
			// 1: Start Remaining Length 	= Don't reset the overhang, set length to parameter
			// 2: Even Caps					= Calculate the cap: a half of the rest (modulo) 
			if( Distribution_Option == 0 )
			{
				if( Caps_Length_Option == 0 )
				{
					Distance_Overhang = 0.0;
					Target_Cut_Length = Length;
				}

				if( Caps_Length_Option == 1 )
				{
					Target_Cut_Length = Length;
				}

				if( Caps_Length_Option == 2 )
				{
					Switch_To_Default 	= true;
					Distance_Overhang 	= 0.0;
					Default_Length 		= Length;
					Target_Cut_Length 	= fmod(pLine->Get_Length(j), Default_Length )/2;
				}
			}

			// Number distribution options
			// 0: Full Segment	= n cuts mean n+1 parts
			// 1: Half Segment	= half caps are (l/n)2 .
			if( Distribution_Option == 1 )
			{
				if( Caps_Number_Option == 0 )
				{
					Distance_Overhang = 0.0;
					Target_Cut_Length = pLine->Get_Length(j) / (Number + 1);
				}

				if( Caps_Number_Option == 1 )
				{
					Switch_To_Default	= true;
					Distance_Overhang 	= 0.0;
					Default_Length 		= pLine->Get_Length(j) / Number;	
					Target_Cut_Length 	= Default_Length / 2.0;
				}
			}	

			// Only cut lines ( a line has > 2 points )
			if( pPart->Get_Count() > 1 )
			{
				// Write the first Point to the new shape
				CSG_Shape *pSegment = pOutputLines->Add_Shape(pLine, SHAPE_COPY_ATTR);
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

					if( Target_Cut_Length <= 0.0 )
					{
						Error_Set(  CSG_String::Format("%s %s %s\n%s %d, %d, %d. Abort!",
									_TL("Cut Length is set to or below zero. This will likely result in a infinite loop."),
									_TL("Ether your input parameter caused this or some weird edge-case with the input-data "),
									_TL("or it is a Bug"), //newline
									_TL("Verbose: Shape-, Part-, Point-Indices are:"), i, j, k)
						);
						return false;
					}

					// Check if there is still space left in the segment to fit 
					// a cut considering the overhang
					//	
					// ~-Distance_Overhang-|---Seg_Reductor-------|
					// --o--------------------------x-------------o--------
					// ------Target_Cut_Length------|\_position of cut
					//
					while( Seg_Reductor + Distance_Overhang > Target_Cut_Length )
					{
						// Decrement the reductor and reset the overhang
						Seg_Reductor -= ( Target_Cut_Length - Distance_Overhang );
						Distance_Overhang = 0.0;

						// Construct the intermediate point with the angel and the
						// length.
						double Construct_Angle 	= SG_Get_Angle_Of_Direction( Front, Back );
						double Construct_Dist 	= Length_Seg - Seg_Reductor; 
						TSG_Point Intermediate_Point;
						Intermediate_Point.x = Front.x + ( Construct_Dist * sin(Construct_Angle) );
						Intermediate_Point.y = Front.y + ( Construct_Dist * cos(Construct_Angle) );

						// Prevent edge-case caused by Seg_Reductor == Target_Cut_Length 
						// This checks if the constructed intermediate point is actually Back.
						// Break out will just add back at the end of the loop
						if( k == pPart->Get_Count()-1
						&& Intermediate_Point.x == Back.x
						&& Intermediate_Point.y == Back.y )
							break;

						// Use the intermediate point as last and also as the first point in new shape
						pSegment->Add_Point( Intermediate_Point );
						pSegment = pOutputLines->Add_Shape(pLine, SHAPE_COPY_ATTR);
						pSegment->Add_Point( Intermediate_Point );

						// Switch to the default length after the first cut is placed
						if( Switch_To_Default == true )
						{
							Switch_To_Default = false;
							Target_Cut_Length = Default_Length;
						}
					}

					// Set existing points
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

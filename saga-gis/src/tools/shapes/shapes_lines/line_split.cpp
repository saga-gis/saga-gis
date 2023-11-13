
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
		"The tool allows one to split lines into multiple lines. The lines can be split based on a user-defined line length or the given number of divisions of each input line. "
		"Optionally, a point shapes layer with the points at which the lines were split can be created. This can be used, e.g., to create the midpoint of each polyline "
		"by dividing each line into two halves."
		)
	);

	Parameters.Add_Shapes(
		NULL, "INPUT", _TL("Input Lines"), _TL("Input line shapefile."),
		PARAMETER_INPUT, SHAPE_TYPE_Line
	);

	Parameters.Add_Shapes(
		NULL, "OUTPUT", _TL("Output Lines"), _TL("Output line shapefile."),
		PARAMETER_OUTPUT, SHAPE_TYPE_Line
	);

	Parameters.Add_Shapes("",
		"INS_POINTS", _TL("Inserted Points"), _TL("The points at which the lines were split."),
		PARAMETER_OUTPUT_OPTIONAL, SHAPE_TYPE_Point
	);

	Parameters.Add_Choice(
		NULL, "DISTRIBUTION", _TL("Distribution"), _TL("Choose the method how to split the lines, either by a given line length or by the given number of divisions."),
		CSG_String::Format("%s|%s|",
			_TL("by length"), 
			_TL("by number")
		), 0
	);

	Parameters.Add_Double(
		"DISTRIBUTION", "LENGTH", _TL("Length"), _TL("Output line length used to split the lines [map units]."), 5.0, 0.0, true
	);

	Parameters.Add_Choice(
		"DISTRIBUTION", "CAPS_LENGTH", _TL("Caps"), _TL(""),
		CSG_String::Format("%s|%s|%s|",
			_TL("start full length"), 
			_TL("start remaining length"), 
			_TL("even ends")
		), 0
	);

	Parameters.Add_Int(
		"DISTRIBUTION", "NUMBER", _TL("Number Of Splits"), _TL("The number of divisions per line [-]."), 5, 0, true
	);

	Parameters.Add_Choice(
		"DISTRIBUTION", "CAPS_NUMBER", _TL("Caps"), _TL(""),
		CSG_String::Format("%s|%s|",
			_TL("full segment"),
			_TL("half segment")
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
	CSG_Shapes	*pInsPoints			= Parameters("INS_POINTS")->asShapes();
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

	if( pInsPoints != NULL )
	{
		pInsPoints->Create(SHAPE_TYPE_Point, CSG_String::Format("%s_inserted_pts", pInputLines->Get_Name()), pInputLines, pInputLines->Get_Vertex_Type());
	}
	
	// Check for projection unit. This tool only works with projected
	// Coordinate Reference Systems and assumes meter. Could be extended
	//if( pInputLines->Get_Projection().Get_Unit() != SG_PROJ_UNIT_Meter )
	//{
	//	Error_Set(_TL("The input line feature has an other projection unit then meter. Abort!"));
	//	return false;
	//}

	pOutputLines->Create(SHAPE_TYPE_Line, CSG_String::Format("%s_cut", pInputLines->Get_Name()), pInputLines, pInputLines->Get_Vertex_Type());

	bool	HasZ = pInputLines->Get_Vertex_Type() != SG_VERTEX_TYPE_XY;
	bool	HasM = pInputLines->Get_Vertex_Type() == SG_VERTEX_TYPE_XYZM;

	// Note: The Overhang is one of the crucial parts. This accumulates 
	// the distance walked since the last point. 
	// Switch_To_Default (Length) will happen after the first cut.
	double 	Distance_Overhang = 0.;
	bool 	Switch_To_Default = false;

	for( sLong i=0; i<pInputLines->Get_Count(); i++ )
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
				TSG_Point A, B = pPart->Get_Point(0);
				double Bz, Bm, Az, Am;

				pSegment->Add_Point( B ); 
				if( HasZ )
				{
					Bz = pPart->Get_Z(0);
					pSegment->Set_Z(Bz, pSegment->Get_Point_Count() - 1);
				}
				if( HasM )
				{
					Bm = pPart->Get_M(0);
					pSegment->Set_M(Bm, pSegment->Get_Point_Count() - 1);
				}

				// This loop investigates all segments. A segment is a
				// line between two points so n-1 operations (k=1)
				for(int k=1; k<pPart->Get_Count(); k++ )
				{
					// Investigate the next segment, get the length and reductor
					A = B; B = pPart->Get_Point(k);
					if( HasZ )	{ Az = Bz; Bz = pPart->Get_Z(k); }
					if( HasM )	{ Am = Bm; Bm = pPart->Get_M(k); }

					double Length_Seg 	= SG_Get_Distance( A, B );
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
						double Construct_Angle 	= SG_Get_Angle_Of_Direction( A, B );
						double Construct_Dist 	= Length_Seg - Seg_Reductor; 
						TSG_Point Intermediate_Point;
						double Iz, Im;

						Intermediate_Point.x = A.x + ( Construct_Dist * sin(Construct_Angle) );
						Intermediate_Point.y = A.y + ( Construct_Dist * cos(Construct_Angle) );
										
						if( HasZ )
						{
							double dShift = SG_Get_Distance(A, Intermediate_Point);

							Iz = Az + dShift * (Bz - Az) / Length_Seg;

							if( HasM )
							{
								Im	= Am + dShift * (Bm - Am) / Length_Seg;
							}
						}

						// Prevent edge-case caused by Seg_Reductor == Target_Cut_Length 
						// This checks if the constructed intermediate point is actually B.
						// Break out will just add back at the end of the loop
						if( k == pPart->Get_Count()-1
						&& Intermediate_Point.x == B.x
						&& Intermediate_Point.y == B.y )
							break;

						// Use the intermediate point as last and also as the first point in new shape
						pSegment->Add_Point( Intermediate_Point );
						if( HasZ ) { pSegment->Set_Z(Iz, pSegment->Get_Point_Count() - 1); }
						if( HasM ) { pSegment->Set_M(Im, pSegment->Get_Point_Count() - 1); }

						pSegment = pOutputLines->Add_Shape(pLine, SHAPE_COPY_ATTR);
						pSegment->Add_Point( Intermediate_Point );
						if( HasZ ) { pSegment->Set_Z(Iz, pSegment->Get_Point_Count() - 1); }
						if( HasM ) { pSegment->Set_M(Im, pSegment->Get_Point_Count() - 1); }

						if( pInsPoints != NULL )
						{
							CSG_Shape *pShape = pInsPoints->Add_Shape(pLine, SHAPE_COPY_ATTR);
							pShape->Add_Point( Intermediate_Point );
							if( HasZ ) { pShape->Set_Z(Iz, pShape->Get_Point_Count() - 1); }
							if( HasM ) { pShape->Set_M(Im, pShape->Get_Point_Count() - 1); }
						}
						
						// Switch to the default length after the first cut is placed
						if( Switch_To_Default == true )
						{
							Switch_To_Default = false;
							Target_Cut_Length = Default_Length;
						}
					}

					// Set existing points
					pSegment->Add_Point( B );
					if( HasZ ) { pSegment->Set_Z(Bz, pSegment->Get_Point_Count() - 1); }
					if( HasM ) { pSegment->Set_M(Bm, pSegment->Get_Point_Count() - 1); }

					// Accumulate the overhang
					// Note: this happens inside the loop over the points (Index k)
					Distance_Overhang += Seg_Reductor;
				}
			}
		}
	}
	return true;
}

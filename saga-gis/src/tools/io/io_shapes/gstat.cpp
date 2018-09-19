/**********************************************************
 * Version $Id: gstat.cpp 1921 2014-01-09 10:24:11Z oconrad $
 *********************************************************/

///////////////////////////////////////////////////////////
//                                                       //
//                         SAGA                          //
//                                                       //
//      System for Automated Geoscientific Analyses      //
//                                                       //
//                     Tool Library                      //
//                       Shapes_IO                       //
//                                                       //
//-------------------------------------------------------//
//                                                       //
//                       Gstat.cpp                       //
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
#include "gstat.h"


///////////////////////////////////////////////////////////
//														 //
//						Export							 //
//														 //
///////////////////////////////////////////////////////////

//---------------------------------------------------------
CGStat_Export::CGStat_Export(void)
{
	Set_Name		(_TL("Export GStat Shapes"));

	Set_Author		("O.Conrad (c) 2003");

	Set_Description(_TW(
		"GStat shapes format export."
	));

	Parameters.Add_Shapes("",
		"SHAPES"	, _TL("Shapes"),
		_TL(""),
		PARAMETER_INPUT
	);

	Parameters.Add_FilePath("",
		"FILENAME"	, _TL("File"),
		_TL(""),
		CSG_String::Format("%s (*.gstat)|*.gstat|%s (*.txt)|*.txt|%s|*.*",
			_TL("GStat Files"),
			_TL("Text Files"),
			_TL("All Files")
		), NULL, true
	);
}

//---------------------------------------------------------
bool CGStat_Export::On_Execute(void)
{
	CSG_File	Stream;

	if( !Stream.Open(Parameters("FILENAME")->asString(), SG_FILE_W, false) )
	{
		return( false );
	}

	int	iShape, iPart, iPoint, iField;

	CSG_Shapes	*pShapes	= Parameters("SHAPES")->asShapes();

	switch( pShapes->Get_Type() )
	{
	//-----------------------------------------------------
	case SHAPE_TYPE_Point:
		Stream.Printf("%s (created by DiGeM 2.0)\n%d\nX-Coordinate\nY-Coordinate",
			Parameters("FILENAME")->asString(),
			pShapes->Get_Field_Count() + 2
		);

		for(iField=0; iField<pShapes->Get_Field_Count(); iField++)
		{
			if( pShapes->Get_Field_Type(iField) == SG_DATATYPE_String )
			{
				Stream.Printf("\n%%%s", pShapes->Get_Field_Name(iField));
			}
			else
			{
				Stream.Printf("\n%s"  , pShapes->Get_Field_Name(iField));
			}
		}

		for(iShape=0; iShape<pShapes->Get_Count() && Set_Progress(iShape, pShapes->Get_Count()); iShape++)
		{
			CSG_Shape	*pShape	= pShapes->Get_Shape(iShape);

			for(iPart=0; iPart<pShape->Get_Part_Count(); iPart++)
			{
				for(iPoint=0; iPoint<pShape->Get_Point_Count(iPart); iPoint++)
				{
					TSG_Point	Point	= pShape->Get_Point(iPoint, iPart);

					Stream.Printf("\n%f\t%f", Point.x, Point.y);

					for(iField=0; iField<pShapes->Get_Field_Count(); iField++)
					{
						if( pShapes->Get_Field_Type(iField) == SG_DATATYPE_String )
						{
							Stream.Printf("\t\"%s\"", pShape->asString(iField));
						}
						else
						{
							Stream.Printf("\t%f"    , pShape->asDouble(iField));
						}
					}
				}
			}
		}
		break;

	//-----------------------------------------------------
	case SHAPE_TYPE_Line:
		Stream.Printf("EXP %s\nARC ", pShapes->Get_Name());

		for(iShape=0; iShape<pShapes->Get_Count() && Set_Progress(iShape,pShapes->Get_Count()); iShape++)
		{
			CSG_Shape	*pShape	= pShapes->Get_Shape(iShape);

			for(iPart=0; iPart<pShape->Get_Part_Count(); iPart++)
			{
				//I_ok...
				Stream.Printf("%d ", iShape + 1);
				// dummy_I dummy_I dummy_I dummy_I dummy_I...
				Stream.Printf("1 2 3 4 5 ");
				// I_np...
				Stream.Printf("%d ", pShape->Get_Point_Count(iPart));

				for(iPoint=0; iPoint<pShape->Get_Point_Count(iPart); iPoint++)
				{
					TSG_Point	Point	= pShape->Get_Point(iPoint, iPart);

					Stream.Printf("%f %f ", Point.x, Point.y);
				}
			}
		}
		break;

	//-----------------------------------------------------
	case SHAPE_TYPE_Polygon:
		Stream.Printf("EXP %s\nARC ", pShapes->Get_Name());

		for(iShape=0; iShape<pShapes->Get_Count() && Set_Progress(iShape, pShapes->Get_Count()); iShape++)
		{
			CSG_Shape	*pShape	= pShapes->Get_Shape(iShape);

			for(iPart=0; iPart<pShape->Get_Part_Count(); iPart++)
			{
				//I_ok...
				Stream.Printf("%d ", iShape + 1);
				// dummy_I dummy_I dummy_I dummy_I dummy_I...
				Stream.Printf("1 2 3 4 5 ");
				// I_np...
				Stream.Printf("%d ", pShape->Get_Point_Count(iPart));

				for(iPoint=0; iPoint<pShape->Get_Point_Count(iPart); iPoint++)
				{
					TSG_Point	Point	= pShape->Get_Point(iPoint, iPart);

					Stream.Printf("%f %f ", Point.x, Point.y);
				}
			}
		}
		break;
	}

	//-----------------------------------------------------
	return( true );
}


///////////////////////////////////////////////////////////
//														 //
//						Import							 //
//														 //
///////////////////////////////////////////////////////////

//---------------------------------------------------------
CGStat_Import::CGStat_Import(void)
{
	Set_Name		(_TL("Import GStat Shapes"));

	Set_Author		("O.Conrad (c) 2003");

	Set_Description(_TW(
		"GStat shapes format import."
	));

	Parameters.Add_Shapes("",
		"SHAPES"	, _TL("Shapes"),
		_TL(""),
		PARAMETER_OUTPUT
	);

	Parameters.Add_FilePath("",
		"FILENAME"	, _TL("File"),
		_TL(""),
		CSG_String::Format("%s (*.gstat)|*.gstat|%s (*.txt)|*.txt|%s|*.*",
			_TL("GStat Files"),
			_TL("Text Files"),
			_TL("All Files")
		), NULL, false
	);
}

//---------------------------------------------------------
bool CGStat_Import::On_Execute(void)
{
	CSG_File	Stream;

	if( !Stream.Open(Parameters("FILENAME")->asString(), SG_FILE_R, false) )
	{
		return( false );
	}

	CSG_String	s;

	sLong	fLength	= Stream.Length();

	if( fLength < 1 || !Stream.Read_Line(s) )
	{
		return( false );
	}

	char		c[3];

	//-----------------------------------------------------
	CSG_Shapes	*pShapes	= Parameters("SHAPES")->asShapes();

	//-----------------------------------------------------
	if( s.CmpNoCase("EXP") )	// Point...
	{
		pShapes->Create(SHAPE_TYPE_Point, SG_File_Get_Name(Parameters("FILENAME")->asString(), false));

		int	i, nFields	= Stream.Scan_Int();	Stream.Read_Line(s);	// next line...

		for(i=0; i<nFields; i++)
		{
			if( Stream.Read_Line(s) )
			{
				if( !s.CmpNoCase("[ignore]") || s[0] == '%' )
				{
					pShapes->Add_Field(s, SG_DATATYPE_String);
				}
				else
				{
					pShapes->Add_Field(s, SG_DATATYPE_Double);
				}
			}
		}

		if( nFields < 2 )
		{
			Error_Set(_TL("Invalid file format."));

			return( false );
		}

		//-------------------------------------------------
		while( !Stream.is_EOF() && Set_Progress((int)Stream.Tell(), fLength) )
		{
			double	x	= Stream.Scan_Double();
			double	y	= Stream.Scan_Double();

			if( !Stream.is_EOF() )
			{
				CSG_Shape	*pShape	= pShapes->Add_Shape();

				pShape->Add_Point(x, y);
				pShape->Set_Value(0, x);
				pShape->Set_Value(1, y);

				for(i=2; i<nFields && !Stream.is_EOF(); i++)
				{
					if( SG_STR_CMP(pShapes->Get_Field_Name(i), "[ignore]") )
					{
						Stream_Find_NextWhiteChar(Stream);

						pShape->Set_Value(i, "NA");
					}
					else if( pShapes->Get_Field_Name(i)[0] == '%' )
					{
						Stream_Get_StringInQuota(Stream, s);

						pShape->Set_Value(i, s);
					}
					else
					{
						pShape->Set_Value(i, Stream.Scan_Double());
					}
				}

				Stream.Read_Line(s);
			}
		}
	}

	//-------------------------------------------------
	// Line, Polygon...
	else
	{
		Stream.Read(c, 3, sizeof(char));

		if( !strncmp(c, "ARC", 3) )
		{
			pShapes->Create(SHAPE_TYPE_Line, Parameters("FILENAME")->asString());
			pShapes->Add_Field("VALUE", SG_DATATYPE_Double);

			//-----------------------------------------
			while( !Stream.is_EOF() && Set_Progress((int)Stream.Tell(), fLength) )
			{
				double	Value	= Stream.Scan_Double();	// i_ok...
				Stream.Scan_Int();	// dummy 1..5
				Stream.Scan_Int();	// dummy 2..5
				Stream.Scan_Int();	// dummy 3..5
				Stream.Scan_Int();	// dummy 4..5
				Stream.Scan_Int();	// dummy 5..5
				int	nFields	= Stream.Scan_Int();

				if( nFields > 0 )
				{
					CSG_Shape	*pShape	= NULL;

					for(int i=0; i<nFields; i++)
					{
						double	x	= Stream.Scan_Double();
						double	y	= Stream.Scan_Double();

						if( !Stream.is_EOF() )
						{
							if( !pShape )
							{
								pShape	= pShapes->Add_Shape();

								pShape->Set_Value(0, Value);
							}

							pShape->Add_Point(x, y);
						}
						else
						{
							break;
						}
					}
				}
			}
		}
	}

	//-----------------------------------------------------
	return( pShapes->Get_Count() > 0 );
}


///////////////////////////////////////////////////////////
//														 //
///////////////////////////////////////////////////////////

//---------------------------------------------------------
bool CGStat_Import::Stream_Find_NextWhiteChar(CSG_File &Stream)
{
	while( !Stream.is_EOF() && Stream.Read_Char() > 32 );

	return( true );
}

//---------------------------------------------------------
bool CGStat_Import::Stream_Get_StringInQuota(CSG_File &Stream, CSG_String &String)
{
	String.Clear();

	while( !Stream.is_EOF() && Stream.Read_Char() != '\"' );

	if( !Stream.is_EOF() )
	{
		char	c;

		while( !Stream.is_EOF() && (c = (char)Stream.Read_Char()) != '\"' )
		{
			String	+= c;
		}

		return( c == '\"' );
	}

	return( false );
}


///////////////////////////////////////////////////////////
//														 //
//														 //
//														 //
///////////////////////////////////////////////////////////

//---------------------------------------------------------

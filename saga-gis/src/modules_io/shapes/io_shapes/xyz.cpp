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
//                       Shapes_IO                       //
//                                                       //
//-------------------------------------------------------//
//                                                       //
//                        Xyz.cpp                        //
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
#include "xyz.h"


///////////////////////////////////////////////////////////
//														 //
//						Export							 //
//														 //
///////////////////////////////////////////////////////////

//---------------------------------------------------------
CXYZ_Export::CXYZ_Export(void)
{
	CSG_Parameter	*pNode;

	//-----------------------------------------------------
	Set_Name		(_TL("Export Shapes to XYZ"));

	Set_Author		(SG_T("O.Conrad (c) 2003"));

	Set_Description	(_TW(
		"XYZ export filter for shapes. "
	));

	//-----------------------------------------------------
	pNode	= Parameters.Add_Shapes(
		NULL	, "SHAPES"	, _TL("Shapes"),
		_TL(""),
		PARAMETER_INPUT
	);

	Parameters.Add_Table_Field(
		pNode	, "FIELD"	, _TL("Attribute"),
		_TL(""),
		true
	);

	Parameters.Add_Value(
		NULL	, "HEADER"	, _TL("Save Table Header"),
		_TL(""),
		PARAMETER_TYPE_Bool	, true
	);

	Parameters.Add_Choice(
		NULL	, "SEPARATE", _TL("Separate Line/Polygon Points"),
		_TL(""),
		CSG_String::Format(SG_T("%s|%s|%s|"),
			_TL("none"),
			_TL("*"),
			_TL("number of points")
		), 0
	);

	Parameters.Add_FilePath(
		NULL	, "FILENAME", _TL("File"),
		_TL(""),
		CSG_String::Format(SG_T("%s|%s|%s|%s|%s|%s"),
			_TL("XYZ Files (*.xyz)")	, SG_T("*.xyz"),
			_TL("Text Files (*.txt)")	, SG_T("*.txt"),
			_TL("All Files")			, SG_T("*.*")
		), NULL, true
	);
}

//---------------------------------------------------------
bool CXYZ_Export::On_Execute(void)
{
	bool		bHeader;
	int			Field, off_Field, Separate;
	CSG_File	Stream;
	CSG_Shapes	*pShapes;

	//-----------------------------------------------------
	pShapes		= Parameters("SHAPES"  )->asShapes();
	bHeader		= Parameters("HEADER"  )->asBool();
	Field		= Parameters("FIELD"   )->asInt();
	Separate	= pShapes->Get_Type() == SHAPE_TYPE_Point ? 0
				: Parameters("SEPARATE")->asInt();
	off_Field	= pShapes->Get_ObjectType() == DATAOBJECT_TYPE_PointCloud ? 2 : 0;

	//-----------------------------------------------------
	if( pShapes->Get_Field_Count() == 0 )
	{
		Error_Set(_TL("data set has no attributes"));

		return( false );
	}

	//-----------------------------------------------------
	if( !Stream.Open(Parameters("FILENAME")->asString(), SG_FILE_W, false) )
	{
		Error_Set(_TL("could not open file"));

		return( false );
	}

	//-----------------------------------------------------
	if( bHeader )
	{
		Stream.Printf(SG_T("X\tY"));

		if( Field < 0 )
		{
			for(int iField=off_Field; iField<pShapes->Get_Field_Count(); iField++)
			{
				Stream.Printf(SG_T("\t%s"), pShapes->Get_Field_Name(iField));
			}
		}
		else
		{
			Stream.Printf(SG_T("\tZ"));
		}

		Stream.Printf(SG_T("\n"));
	}

	//-------------------------------------------------
	for(int iShape=0; iShape<pShapes->Get_Count() && Set_Progress(iShape, pShapes->Get_Count()); iShape++)
	{
		CSG_Shape	*pShape	= pShapes->Get_Shape(iShape);

		for(int iPart=0; iPart<pShape->Get_Part_Count(); iPart++)
		{
			switch( Separate )
			{
			case 1:	// *
				Stream.Printf(SG_T("*\n"));
				break;

			case 2:	// number of points
				Stream.Printf(SG_T("%d\n"), pShape->Get_Point_Count(iPart));
				break;
			}

			for(int iPoint=0; iPoint<pShape->Get_Point_Count(iPart); iPoint++)
			{
				TSG_Point	Point	= pShape->Get_Point(iPoint, iPart);

				Stream.Printf(SG_T("%f\t%f"), Point.x, Point.y);

				if( Field < 0 )
				{
					for(int iField=off_Field; iField<pShapes->Get_Field_Count(); iField++)
					{
						switch( pShapes->Get_Field_Type(iField) )
						{
						case SG_DATATYPE_String:
						case SG_DATATYPE_Date:
							Stream.Printf(SG_T("\t\"%s\""), pShape->asString(iField));
							break;

						default:
							Stream.Printf(SG_T("\t%f")    , pShape->asDouble(iField));
							break;
						}
					}
				}
				else switch( pShapes->Get_Field_Type(Field) )
				{
				case SG_DATATYPE_String:
				case SG_DATATYPE_Date:
					Stream.Printf(SG_T("\t\"%s\""), pShape->asString(Field));
					break;

				default:
					Stream.Printf(SG_T("\t%f")    , pShape->asDouble(Field));
					break;
				}

				Stream.Printf(SG_T("\n"));
			}
		}
	}

	//-------------------------------------------------
	return( true );
}


///////////////////////////////////////////////////////////
//														 //
//						Import							 //
//														 //
///////////////////////////////////////////////////////////

//---------------------------------------------------------
CXYZ_Import::CXYZ_Import(void)
{
	CSG_Parameter	*pNode;

	//-----------------------------------------------------
	Set_Name		(_TL("Import Shapes from XYZ"));

	Set_Author		("O.Conrad (c) 2003");

	Set_Description	(_TW(
		"Point shapes import from text formated XYZ-table."
	));

	//-----------------------------------------------------
	pNode	= Parameters.Add_Shapes(
		NULL	, "SHAPES"		, _TL("Points"),
		_TL(""),
		PARAMETER_OUTPUT, SHAPE_TYPE_Point
	);

//	Parameters.Add_Value(
//		NULL	, "HEADLINE"	, "File contains headline",
//		_TL(""),
//		PARAMETER_TYPE_Bool		, true
//	);

	Parameters.Add_Value(
		pNode	, "X_FIELD"		, _TL("X Column"),
		_TL(""),
		PARAMETER_TYPE_Int		, 1, 1, true
	);

	Parameters.Add_Value(
		pNode	, "Y_FIELD"		, _TL("Y Column"),
		_TL(""),
		PARAMETER_TYPE_Int		, 2, 1, true
	);

	Parameters.Add_FilePath(
		NULL	, "FILENAME"	, _TL("File"),
		_TL(""),
		CSG_String::Format(SG_T("%s|%s|%s|%s|%s|%s"),
			_TL("XYZ Files (*.xyz)")	, SG_T("*.xyz"),
			_TL("Text Files (*.txt)")	, SG_T("*.txt"),
			_TL("All Files")			, SG_T("*.*")
		), NULL, false
	);
}

//---------------------------------------------------------
bool CXYZ_Import::On_Execute(void)
{
	CSG_Table	Table;

	//-----------------------------------------------------
	if( !Table.Create(Parameters("FILENAME")->asString()) )	// Parameters("HEADLINE")->asBool()
	{
		Error_Set(_TL("Table could not be opened."));

		return( false );
	}

	//-----------------------------------------------------
	if( Table.Get_Record_Count() <= 0 )
	{
		Error_Set(_TL("Table does not contain any data."));

		return( false );
	}

	//-----------------------------------------------------
	int	xField	= Parameters("X_FIELD")->asInt() - 1;
	int	yField	= Parameters("Y_FIELD")->asInt() - 1;

	if( xField == yField
	||  xField < 0 || xField >= Table.Get_Field_Count()
	||  yField < 0 || yField >= Table.Get_Field_Count() )
	{
		Error_Set(_TL("Invalid X/Y fields."));

		return( false );
	}

	//-----------------------------------------------------
	CSG_Shapes	*pShapes	= Parameters("SHAPES" )->asShapes();

	pShapes->Create(SHAPE_TYPE_Point, Table.Get_Name(), &Table);

	for(int iRecord=0; iRecord<Table.Get_Record_Count(); iRecord++)
	{
		CSG_Table_Record	*pRecord	= Table.Get_Record(iRecord);

		CSG_Shape	*pShape	= pShapes->Add_Shape(pRecord);

		pShape->Add_Point(
			pRecord->asDouble(xField),
			pRecord->asDouble(yField)
		);
	}

	return( true );
}


///////////////////////////////////////////////////////////
//														 //
//														 //
//														 //
///////////////////////////////////////////////////////////

//---------------------------------------------------------

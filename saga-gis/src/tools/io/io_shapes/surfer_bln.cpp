/**********************************************************
 * Version $Id: surfer_bln.cpp 1921 2014-01-09 10:24:11Z oconrad $
 *********************************************************/

///////////////////////////////////////////////////////////
//                                                       //
//                         SAGA                          //
//                                                       //
//      System for Automated Geoscientific Analyses      //
//                                                       //
//                     Tool Library                      //
//                       io_shapes                       //
//                                                       //
//-------------------------------------------------------//
//                                                       //
//                    Surfer_BLN.cpp                     //
//                                                       //
//                 Copyright (C) 2006 by                 //
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
#include "surfer_bln.h"


///////////////////////////////////////////////////////////
//														 //
//						Import							 //
//														 //
///////////////////////////////////////////////////////////

//---------------------------------------------------------
CSurfer_BLN_Import::CSurfer_BLN_Import(void)
{
	//-----------------------------------------------------
	Set_Name		(_TL("Import Surfer Blanking Files"));

	Set_Author		("O.Conrad (c) 2006");

	Set_Description	(_TW(
		"Import polygons/polylines from Golden Software's Surfer Blanking File format.\n"
	));

	//-----------------------------------------------------
	Parameters.Add_Shapes("",
		"SHAPES"	, _TL("Shapes"),
		_TL(""),
		PARAMETER_OUTPUT
	);

	Parameters.Add_Table("",
		"TABLE"	, _TL("Look up table (Points)"),
		_TL(""),
		PARAMETER_OUTPUT_OPTIONAL
	);

	Parameters.Add_FilePath("",
		"FILE"	, _TL("File"),
		_TL(""),
		_TL("Surfer Blanking Files (*.bln)|*.bln|All Files|*.*")
	);

	Parameters.Add_Choice("",
		"TYPE"	, _TL("Shape Type"),
		_TL(""),
		CSG_String::Format("%s|%s|%s|",
			_TL("points"),
			_TL("lines"),
			_TL("polygons")
		), 1
	);
}

//---------------------------------------------------------
bool CSurfer_BLN_Import::On_Execute(void)
{
	bool				bOk;
	int					ID, Flag, iPoint, nPoints;
	double				x, y;
	CSG_File			Stream;
	TSG_Shape_Type		Type;
	CSG_String			FileName, sLine, sName, sDesc, sTemp;
	CSG_Table_Record	*pRecord;
	CSG_Table			*pTable;
	CSG_Shape			*pShape;
	CSG_Shapes			*pShapes;

	//-----------------------------------------------------
	pShapes		= Parameters("SHAPES")->asShapes();
	pTable		= Parameters("TABLE" )->asTable();
	FileName	= Parameters("FILE"  )->asString();

	switch( Parameters("TYPE")->asInt() )
	{
	case  0: Type = SHAPE_TYPE_Point  ; break;
	default: Type = SHAPE_TYPE_Line   ; break;
	case  2: Type = SHAPE_TYPE_Polygon; break;
	}

	//-----------------------------------------------------
	if( Stream.Open(FileName, SG_FILE_R) )
	{
		bOk		= true;
		ID		= 0;

		if(	pShapes->Get_Type() != SHAPE_TYPE_Undefined
		&&	pShapes->Get_Type() != Type )
		{
			pShapes	= SG_Create_Shapes(Type, SG_File_Get_Name(FileName, false));
			Parameters("SHAPES")->Set_Value(pShapes);
			DataObject_Add(pShapes);
		}
		else
		{
			pShapes->Create(Type, SG_File_Get_Name(FileName, false));
		}

		if( Type == SHAPE_TYPE_Point )
		{
			if( pTable == NULL )
			{
				pTable	= SG_Create_Table();
				Parameters("TABLE")->Set_Value(pTable);
			}
			else
			{
				pTable->Destroy();
			}

			pTable ->Add_Field("ID"    , SG_DATATYPE_Int   );
			pTable ->Add_Field("FLAG"  , SG_DATATYPE_Int   );
			pTable ->Add_Field("NAME"  , SG_DATATYPE_String);
			pTable ->Add_Field("DESC"  , SG_DATATYPE_String);

			pShapes->Add_Field("ID"    , SG_DATATYPE_Int   );
			pShapes->Add_Field("ID_LUT", SG_DATATYPE_Int   );
			pShapes->Add_Field("Z"     , SG_DATATYPE_Double);
		}
		else
		{
			pShapes->Add_Field("ID"		, SG_DATATYPE_Int   );
			pShapes->Add_Field("FLAG"	, SG_DATATYPE_Int   );
			pShapes->Add_Field("NAME"	, SG_DATATYPE_String);
			pShapes->Add_Field("DESC"	, SG_DATATYPE_String);
		}

		//-------------------------------------------------
		while( bOk && Stream.Read_Line(sLine) && sLine.BeforeFirst(',').asInt(nPoints) && nPoints > 0 && Process_Get_Okay(true) )
		{
			Process_Set_Text(CSG_String::Format(SG_T("%d. %s"), ++ID, _TL("shape in process")));

			sTemp	= sLine.AfterFirst (',');	sLine	= sTemp;
			Flag	= sLine.BeforeFirst(',').asInt();

			sTemp	= sLine.AfterFirst (',');	sLine	= sTemp;
			sTemp	= sLine.BeforeFirst(',');
			sName	= sTemp.AfterFirst('\"').BeforeLast('\"');

			sTemp	= sLine.AfterFirst (',');	sLine	= sTemp;
			sTemp	= sLine.BeforeFirst(',');
			sDesc	= sTemp.AfterFirst('\"').BeforeLast('\"');

			if( Type == SHAPE_TYPE_Point )
			{
				pRecord	= pTable->Add_Record();
				pRecord->Set_Value(0, ID);
				pRecord->Set_Value(1, Flag);
				pRecord->Set_Value(2, sName);
				pRecord->Set_Value(3, sDesc);

				for(iPoint=0; iPoint<nPoints && bOk; iPoint++)
				{
					if( (bOk = Stream.Read_Line(sLine)) == true )
					{
						pShape	= pShapes->Add_Shape();
						pShape->Set_Value(0, iPoint + 1);
						pShape->Set_Value(1, ID);
						pShape->Set_Value(2, sLine.AfterLast (',').asDouble());

						x	= sLine.BeforeFirst(',').asDouble();
						y	= sLine.AfterFirst (',').asDouble();
						pShape->Add_Point(x, y);
					}
				}
			}
			else
			{
				pShape	= pShapes->Add_Shape();
				pShape->Set_Value(0, ID);
				pShape->Set_Value(1, Flag);
				pShape->Set_Value(2, sName);
				pShape->Set_Value(3, sDesc);

				for(iPoint=0; iPoint<nPoints && bOk; iPoint++)
				{
					if( (bOk = Stream.Read_Line(sLine)) == true )
					{
						x	= sLine.BeforeFirst(',').asDouble();
						y	= sLine.AfterFirst (',').asDouble();
						pShape->Add_Point(x, y);
					}
				}
			}
		}
	}

	//-----------------------------------------------------
	if( pShapes->is_Valid() && pShapes->Get_Count() > 0 )
	{
		return( true );
	}

	return( false );
}


///////////////////////////////////////////////////////////
//														 //
//						Export							 //
//														 //
///////////////////////////////////////////////////////////

//---------------------------------------------------------
CSurfer_BLN_Export::CSurfer_BLN_Export(void)
{
	//-----------------------------------------------------
	Set_Name		(_TL("Export Surfer Blanking File"));

	Set_Author		("O.Conrad (c) 2006");

	Set_Description	(_TW(
		"Export shapes to Golden Software's Surfer Blanking File format.\n"
	));

	//-----------------------------------------------------
	Parameters.Add_Shapes("",
		"SHAPES", _TL("Shapes"),
		_TL(""),
		PARAMETER_INPUT
	);

	Parameters.Add_Table_Field("SHAPES", "NAME", _TL("Name"       ), _TL(""), true);
	Parameters.Add_Table_Field("SHAPES", "DESC", _TL("Description"), _TL(""), true);
	Parameters.Add_Table_Field("SHAPES", "ZVAL", _TL("z values"   ), _TL(""), true);

	Parameters.Add_FilePath("",
		"FILE"	, _TL("File"),
		_TL(""),
		CSG_String::Format("%s|*bln|%s|*.*",
			_TL("Surfer Blanking Files (*.bln)"),
			_TL("All Files")
		), NULL, true
	);
}

//---------------------------------------------------------
bool CSurfer_BLN_Export::On_Execute(void)
{
	//-----------------------------------------------------
	CSG_File	Stream;

	if( !Stream.Open(Parameters("FILE")->asString(), SG_FILE_W) )
	{
		return( false );
	}

	CSG_Shapes	*pShapes	= Parameters("SHAPES")->asShapes();

	if( !pShapes->is_Valid() || pShapes->Get_Count() <= 0 )
	{
		return( false );
	}

	//-----------------------------------------------------
	int iName	= Parameters("BNAME")->asBool() ? Parameters("NAME")->asInt() : -1;
	int iDesc	= Parameters("BDESC")->asBool() ? Parameters("DESC")->asInt() : -1;
	int iZVal	= Parameters("BZVAL")->asBool() ? Parameters("ZVAL")->asInt() : -1;

	int Flag	= 1;

	//-----------------------------------------------------
	for(int iShape=0; iShape<pShapes->Get_Count() && Set_Progress(iShape, pShapes->Get_Count()); iShape++)
	{
		CSG_Shape	*pShape	= pShapes->Get_Shape(iShape);

		for(int iPart=0; iPart<pShape->Get_Part_Count(); iPart++)
		{
			Stream.Printf(SG_T("%d,%d"), pShape->Get_Point_Count(iPart), Flag);

			if( iName >= 0 )	{	Stream.Printf(",\"%s\"", pShape->asString(iName));	}
			if( iDesc >= 0 )	{	Stream.Printf(",\"%s\"", pShape->asString(iDesc));	}

			Stream.Printf("\n");

			for(int iPoint=0; iPoint<pShape->Get_Point_Count(iPart); iPoint++)
			{
				TSG_Point	p	= pShape->Get_Point(iPoint, iPart);

				if( iZVal >= 0 )
				{
					Stream.Printf("%f,%f,%f\n", p.x, p.y, pShape->asDouble(iZVal));
				}
				else
				{
					Stream.Printf("%f,%f\n"   , p.x, p.y);
				}
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

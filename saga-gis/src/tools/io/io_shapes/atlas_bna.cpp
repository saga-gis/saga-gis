/**********************************************************
 * Version $Id: atlas_bna.cpp 1921 2014-01-09 10:24:11Z oconrad $
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
//                     Atlas_BNA.cpp                     //
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
// 51 Franklin Street, 5th Floor, Boston, MA 02110-1301, //
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
#include "atlas_bna.h"


///////////////////////////////////////////////////////////
//														 //
//						Import							 //
//														 //
///////////////////////////////////////////////////////////

//---------------------------------------------------------
CAtlas_BNA_Import::CAtlas_BNA_Import(void)
{
	//-----------------------------------------------------
	// 1. Info...

	Set_Name		(_TL("Import Atlas Boundary File"));

	Set_Author		("O.Conrad (c) 2006");

	Set_Description	(_TW(
		""
	));


	//-----------------------------------------------------
	// 2. Parameters...

	Parameters.Add_FilePath(
		NULL	, "FILE"	, _TL("File"),
		_TL(""),
		CSG_String::Format("%s (*.bna)|*.bna|%s|*.*",
			_TL("Atlas Boundary Files"),
			_TL("All Files")
		)
	);
}

//---------------------------------------------------------
bool CAtlas_BNA_Import::On_Execute(void)
{
	bool		bOk;
	int			iPoint, nPoints;
	double		x, y;
	FILE		*Stream;
	CSG_String	FileName, sLine, sName1, sName2;
	CSG_Shape	*pShape;
	CSG_Shapes	*pPoints, *pLines, *pPolygons;

	//-----------------------------------------------------
	FileName	= Parameters("FILE")	->asString();

	//-----------------------------------------------------
	if( (Stream = fopen(FileName.b_str(), "r")) != NULL )
	{
		pPoints		= SG_Create_Shapes(SHAPE_TYPE_Point  , SG_File_Get_Name(FileName, false));
		pPoints		->Add_Field("NAME1"	, SG_DATATYPE_String);
		pPoints		->Add_Field("NAME2"	, SG_DATATYPE_String);

		pLines		= SG_Create_Shapes(SHAPE_TYPE_Line   , SG_File_Get_Name(FileName, false));
		pLines		->Add_Field("NAME1"	, SG_DATATYPE_String);
		pLines		->Add_Field("NAME2"	, SG_DATATYPE_String);

		pPolygons	= SG_Create_Shapes(SHAPE_TYPE_Polygon, SG_File_Get_Name(FileName, false));
		pPolygons	->Add_Field("NAME1"	, SG_DATATYPE_String);
		pPolygons	->Add_Field("NAME2"	, SG_DATATYPE_String);

		//-------------------------------------------------
		bOk		= true;

		while( bOk && SG_Read_Line(Stream, sLine) && Process_Get_Okay(true) )
		{
			sName1	= sLine.AfterFirst('\"').BeforeFirst('\"');
			sName2	= sLine.BeforeLast('\"').AfterLast('\"');
			sLine	= sLine.AfterLast('\"');	if( sLine.Find(',', true) >= 0 )	sLine	= sLine.AfterLast(',');

			nPoints	= sLine.asInt();

			if( nPoints == 1 )
			{
				pShape	= pPoints	->Add_Shape();
			}
			else if( nPoints < 0 )
			{
				pShape	= pLines	->Add_Shape();
				nPoints	= -nPoints;
			}
			else if( nPoints > 2 )
			{
				pShape	= pPolygons	->Add_Shape();
			}
			else
			{
				bOk		= false;
			}

			if( bOk )
			{
				pShape->Set_Value(0, sName1);
				pShape->Set_Value(1, sName2);

				for(iPoint=0; iPoint<nPoints && bOk; iPoint++)
				{
					if( (bOk = SG_Read_Line(Stream, sLine)) == true )
					{
						SG_SSCANF(sLine, SG_T("%lf %lf"), &x, &y);
						pShape->Add_Point(x, y);
					}
				}
			}
		}

		fclose(Stream);

		//-------------------------------------------------
		bOk		= false;

		if( pPoints->is_Valid() && pPoints->Get_Count() > 0 )
		{
			bOk		= true;
			DataObject_Add(pPoints);
		}
		else
		{
			delete(pPoints);
		}

		if( pLines->is_Valid() && pLines->Get_Count() > 0 )
		{
			bOk		= true;
			DataObject_Add(pLines);
		}
		else
		{
			delete(pLines);
		}

		if( pPolygons->is_Valid() && pPolygons->Get_Count() > 0 )
		{
			bOk		= true;
			DataObject_Add(pPolygons);
		}
		else
		{
			delete(pPolygons);
		}

		return( bOk );
	}

	//-----------------------------------------------------
	return( false );
}


///////////////////////////////////////////////////////////
//														 //
//						Export							 //
//														 //
///////////////////////////////////////////////////////////

//---------------------------------------------------------
CAtlas_BNA_Export::CAtlas_BNA_Export(void)
{
	//-----------------------------------------------------
	// 1. Info...

	Set_Name		(_TL("Export Atlas Boundary File"));

	Set_Author		("O.Conrad (c) 2006");

	Set_Description	(_TW(
		"\n"
	));


	//-----------------------------------------------------
	// 2. Parameters...

	CSG_Parameter	*pNode;

	pNode	= Parameters.Add_Shapes(
		NULL	, "SHAPES"	, _TL("Shapes"),
		_TL(""),
		PARAMETER_INPUT
	);

	Parameters.Add_Table_Field(
		pNode	, "PNAME"	, _TL("Primary Name"),
		_TL("")
	);

	Parameters.Add_Table_Field(
		pNode	, "SNAME"	, _TL("Secondary Name"),
		_TL("")
	);

	Parameters.Add_FilePath(
		NULL	, "FILE"	, _TL("File"),
		_TL(""),
		CSG_String::Format(SG_T("%s|*.bna|%s|*.*"),
			_TL("Atlas Boundary Files (*.bna)"),
			_TL("All Files")
		), NULL, true
	);
}

//---------------------------------------------------------
bool CAtlas_BNA_Export::On_Execute(void)
{
	//-----------------------------------------------------
	CSG_File	Stream;

	if( !Stream.Open(Parameters("FILE")->asString(), SG_FILE_W, false) )
	{
		return( false );
	}

	CSG_Shapes	*pShapes	= Parameters("SHAPES")->asShapes();

	if( !pShapes->is_Valid() || pShapes->Get_Count() <= 0 )
	{
		return( false );
	}

	//-----------------------------------------------------
	int	iName1	= Parameters("PNAME")->asInt();
	int	iName2	= Parameters("SNAME")->asInt();

	//-----------------------------------------------------
	for(int iShape=0; iShape<pShapes->Get_Count() && Set_Progress(iShape, pShapes->Get_Count()); iShape++)
	{
		CSG_Shape	*pShape	= pShapes->Get_Shape(iShape);

		switch( pShapes->Get_Type() )
		{
		default:
			break;

		//---------------------------------------------
		case SHAPE_TYPE_Point:
			if( pShape->is_Valid() )
			{
				Stream.Printf("\"%s\",\"%s\",%d\n",
					pShape->asString(iName1),
					pShape->asString(iName2),
					1
				);

				TSG_Point	p	= pShape->Get_Point(0);
				Stream.Printf("%f,%f\n", p.x, p.y);
			}
			break;

		//---------------------------------------------
		case SHAPE_TYPE_Line:
			if( pShape->is_Valid() )
			{
				for(int iPart=0; iPart<pShape->Get_Part_Count(); iPart++)
				{
					Stream.Printf("\"%s\",\"%s\",%d\n",
						pShape->asString(iName1),
						pShape->asString(iName2),
						pShape->Get_Point_Count(iPart)
					);

					for(int iPoint=0; iPoint<pShape->Get_Point_Count(iPart); iPoint++)
					{
						TSG_Point	p	= pShape->Get_Point(iPoint, iPart);
						Stream.Printf("%f,%f\n", p.x, p.y);
					}
				}
			}
			break;

		//---------------------------------------------
		case SHAPE_TYPE_Polygon:
			if( pShape->is_Valid() )
			{
				Stream.Printf("\"%s\",\"%s\",%d\n",
					pShape->asString(iName1),
					pShape->asString(iName2),
					pShape->Get_Point_Count()
				);

				for(int iPart=0; iPart<pShape->Get_Part_Count(); iPart++)
				{
					for(int iPoint=0; iPoint<pShape->Get_Point_Count(iPart); iPoint++)
					{
						TSG_Point	p	= pShape->Get_Point(iPoint, iPart);
						Stream.Printf("%f,%f\n", p.x, p.y);
					}
				}
			}
			break;
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

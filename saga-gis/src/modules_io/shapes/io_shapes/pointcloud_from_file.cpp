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
//                       io_shapes                       //
//                                                       //
//-------------------------------------------------------//
//                                                       //
//                pointcloud_from_file.cpp               //
//                                                       //
//                 Copyright (C) 2009 by                 //
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
//                University of Hamburg                  //
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
#include "pointcloud_from_file.h"


///////////////////////////////////////////////////////////
//														 //
//														 //
//														 //
///////////////////////////////////////////////////////////

//---------------------------------------------------------
CPointCloud_From_File::CPointCloud_From_File(void)
{
	//-----------------------------------------------------
	Set_Name		(_TL("Import Point Cloud from Shape File"));

	Set_Author		(SG_T("O. Conrad (c) 2009"));

	Set_Description	(_TW(
		"Allows to import a point cloud from a point shapefile."
	));

	//-----------------------------------------------------
	Parameters.Add_PointCloud_Output(
		NULL	, "POINTS"		, _TL("Point Cloud"),
		_TL("")
	);

	Parameters.Add_FilePath(
		NULL	, "FILE"		, _TL("File"),
		_TL(""),
		CSG_String::Format(SG_T("%s|%s|%s|%s"),
			_TL("ESRI Shapefiles")	, SG_T("*.shp"),
			_TL("All Files")		, SG_T("*.*")
		)
	);
}


///////////////////////////////////////////////////////////
//														 //
//														 //
//														 //
///////////////////////////////////////////////////////////

//---------------------------------------------------------
bool CPointCloud_From_File::On_Execute(void)
{
	CSG_String	fName(Parameters("FILE")->asString());

	if( Read_Shapefile(fName) )
	{
		return( true );
	}

	return( false );
}


///////////////////////////////////////////////////////////
//														 //
//														 //
//														 //
///////////////////////////////////////////////////////////

//---------------------------------------------------------
bool CPointCloud_From_File::Read_Shapefile(const CSG_String &fName)
{
	int			iField, iPoint;
	CSG_Shapes	Shapes;

	if( !Shapes.Create(fName) || Shapes.Get_Count() == 0 )
	{
		return( false );
	}

	if( Shapes.Get_Type() != SHAPE_TYPE_Point )
	{
		 return( false );
	}

	//-----------------------------------------------------
	CSG_PointCloud	*pPoints	= SG_Create_PointCloud();
	pPoints->Set_Name(SG_File_Get_Name(fName, false));
	Parameters("POINTS")->Set_Value(pPoints);

	for(iField=0; iField<Shapes.Get_Field_Count(); iField++)
	{
		pPoints->Add_Field(Shapes.Get_Field_Name(iField), SG_DATATYPE_Double);
	}

	for(iPoint=0; iPoint<Shapes.Get_Count(); iPoint++)
	{
		CSG_Shape	*pPoint	= Shapes.Get_Shape(iPoint);

		pPoints->Add_Point(pPoint->Get_Point(0).x, pPoint->Get_Point(0).y, 0.0);

		for(iField=0; iField<Shapes.Get_Field_Count(); iField++)
		{
			pPoints->Set_Value(3 + iField, pPoint->asDouble(iField));
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

/**********************************************************
 * Version $Id$
 *********************************************************/
/*******************************************************************************
    GraticuleBuilder.cpp
    Copyright (C) Victor Olaya
    
    This program is free software; you can redistribute it and/or modify
    it under the terms of the GNU General Public License as published by
    the Free Software Foundation; either version 2 of the License, or
    (at your option) any later version.

    This program is distributed in the hope that it will be useful,
    but WITHOUT ANY WARRANTY; without even the implied warranty of
    MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
    GNU General Public License for more details.

    You should have received a copy of the GNU General Public License
    along with this program; if not, write to the Free Software
    Foundation, Inc., 51 Franklin Street, 5th Floor, Boston, MA 02110-1301, USA
*******************************************************************************/ 

///////////////////////////////////////////////////////////
//                                                       //
//                                                       //
//                                                       //
///////////////////////////////////////////////////////////

//---------------------------------------------------------
#include "GraticuleBuilder.h"


///////////////////////////////////////////////////////////
//                                                       //
//                                                       //
//                                                       //
///////////////////////////////////////////////////////////

//---------------------------------------------------------
CGraticuleBuilder::CGraticuleBuilder(void)
{
	Set_Name		(_TL("Create Graticule"));

	Set_Author		(SG_T("V.Olaya (c) 2004"));

	Set_Description	(_TW(
		"(c) 2004 by Victor Olaya. "
	));

	Parameters.Add_Shapes(
		NULL	, "GRATICULE"	, _TL("Graticule"),
		_TL(""),
		PARAMETER_OUTPUT
	);

	Parameters.Add_Shapes(
		NULL	, "EXTENT"		, _TL("Extent"), 
		_TL(""),
		PARAMETER_INPUT_OPTIONAL
	);
				
	Parameters.Add_Range(
		NULL	, "X_EXTENT"	, _TL("Width"),
		_TL(""),
		-180.0, 180.0
	);

	Parameters.Add_Range(
		NULL	, "Y_EXTENT"	, _TL("Height"),
		_TL(""),
		-90.0, 90.0
	);

	Parameters.Add_Value(
		NULL	, "DISTX"		, _TL("Division Width"),
		_TL(""), 
		PARAMETER_TYPE_Double, 10
	);

	Parameters.Add_Value(
		NULL	, "DISTY"		, _TL("Division Height"),
		_TL(""), 
		PARAMETER_TYPE_Double, 10
	);

	Parameters.Add_Choice(
		NULL	, "TYPE"		, _TL("Type"),
		_TL(""),
		CSG_String::Format(SG_T("%s|%s|"),
			_TL("Lines"),
			_TL("Rectangles")
		), 0
	);
}

//---------------------------------------------------------
CGraticuleBuilder::~CGraticuleBuilder(void)
{}


///////////////////////////////////////////////////////////
//                                                       //
//                                                       //
//                                                       //
///////////////////////////////////////////////////////////

//---------------------------------------------------------
bool CGraticuleBuilder::On_Execute(void)
{
	//-----------------------------------------------------
	CSG_Rect	Extent;
	
	if( Parameters("EXTENT")->asShapes() )
	{
		Extent.Assign(Parameters("EXTENT")->asShapes()->Get_Extent());
	}
	else
	{
		Extent.Assign(
			Parameters("X_EXTENT")->asRange()->Get_LoVal(),
			Parameters("Y_EXTENT")->asRange()->Get_LoVal(),
			Parameters("X_EXTENT")->asRange()->Get_HiVal(),
			Parameters("Y_EXTENT")->asRange()->Get_HiVal()
		);
	}

	//-----------------------------------------------------
	int			id	= 0, row, col;
	double		x, y, dx, dy;
	CSG_Shapes	*pGraticule;

	pGraticule	= Parameters("GRATICULE")	->asShapes();
	dx			= Parameters("DISTX")		->asDouble();
	dy			= Parameters("DISTY")		->asDouble();

	//-----------------------------------------------------
	if( dx <= 0.0 || dx > Extent.Get_XRange() || dy <= 0.0 || dy > Extent.Get_YRange() )
	{
		Error_Set(_TL("invalid division size"));

		return( false );
	}

	switch( Parameters("TYPE")->asInt() )
	{
	//-----------------------------------------------------
	case 0:	// Lines
		{
			pGraticule->Create(SHAPE_TYPE_Line, _TL("Graticule"));

			pGraticule->Add_Field("ID", SG_DATATYPE_Int);

			for(x=Extent.Get_XMin(); x<=Extent.Get_XMax(); x+=dx)
			{
				CSG_Shape	*pShape	= pGraticule->Add_Shape();

				pShape->Set_Value(0, ++id);

				for(y=Extent.Get_YMin(); y<=Extent.Get_YMax(); y+=dy)
				{
					pShape->Add_Point(x,y);
					pShape->Add_Point(x,y);
				}
			}//for

			for(y=Extent.Get_YMin(); y<=Extent.Get_YMax(); y+=dy)
			{
				CSG_Shape	*pShape	= pGraticule->Add_Shape();

				pShape->Set_Value(0, ++id);

				for(x=Extent.Get_XMin(); x<=Extent.Get_XMax(); x+=dx)
				{
					pShape->Add_Point(x,y);
					pShape->Add_Point(x,y);
				}
			}//for
		}
		break;

	//-----------------------------------------------------
	case 1:	// Rectangles
		{
			pGraticule->Create(SHAPE_TYPE_Polygon, _TL("Graticule"));

			pGraticule->Add_Field("ID" , SG_DATATYPE_Int);
			pGraticule->Add_Field("ROW", SG_DATATYPE_Int);
			pGraticule->Add_Field("COL", SG_DATATYPE_Int);
		//	pGraticule->Add_Field("LNK", SG_DATATYPE_String);

			for(x=Extent.Get_XMin(), row=1; x<Extent.Get_XMax(); x+=dx, row++)
			{
				for(y=Extent.Get_YMax(), col=1; y>Extent.Get_YMin(); y-=dy, col++)
				{
					CSG_Shape	*pShape	= pGraticule->Add_Shape();

					pShape->Set_Value(0, ++id);
					pShape->Set_Value(1, row);
					pShape->Set_Value(2, col);
		//			pShape->Set_Value(3, CSG_String::Format(SG_T("ftp://xftp.jrc.it/pub/srtmV4/tiff/srtm_%02d_%02d.zip"), row, col));

					pShape->Add_Point(x     , y     );
					pShape->Add_Point(x     , y - dy);
					pShape->Add_Point(x + dx, y - dy);
					pShape->Add_Point(x + dx, y     );
					pShape->Add_Point(x     , y     );
				}
			}
		}
		break;

	//-----------------------------------------------------
	}

	return( true );
}


///////////////////////////////////////////////////////////
//                                                       //
//                                                       //
//                                                       //
///////////////////////////////////////////////////////////

//---------------------------------------------------------

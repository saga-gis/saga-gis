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

	Set_Author		("V.Olaya (c) 2004");

	Set_Description	(_TW(
		"The tool allows one to create a graticule with a user-specified width and height.\n"
	));

	//-----------------------------------------------------
	Parameters.Add_Shapes(
		NULL	, "GRATICULE_LINE"	, _TL("Graticule"),
		_TL(""),
		PARAMETER_OUTPUT, SHAPE_TYPE_Line
	);

	Parameters.Add_Shapes(
		NULL	, "GRATICULE_RECT"	, _TL("Graticule"),
		_TL(""),
		PARAMETER_OUTPUT, SHAPE_TYPE_Polygon
	);

	Parameters.Add_Choice(
		NULL	, "TYPE"			, _TL("Type"),
		_TL(""),
		CSG_String::Format("%s|%s|",
			_TL("Lines"),
			_TL("Rectangles")
		), 0
	);

	//-----------------------------------------------------
	Parameters.Add_Shapes(
		NULL	, "EXTENT"			, _TL("Extent"), 
		_TL(""),
		PARAMETER_INPUT_OPTIONAL
	);
				
	Parameters.Add_Range(
		NULL	, "EXTENT_X"		, _TL("Width"),
		_TL(""),
		-180.0, 180.0
	);

	Parameters.Add_Range(
		NULL	, "EXTENT_Y"		, _TL("Height"),
		_TL(""),
		-90.0, 90.0
	);

	Parameters.Add_Value(
		NULL	, "DIVISION_X"		, _TL("Division Width"),
		_TL(""), 
		PARAMETER_TYPE_Double, 10.0, 0.0, true
	);

	Parameters.Add_Value(
		NULL	, "DIVISION_Y"		, _TL("Division Height"),
		_TL(""), 
		PARAMETER_TYPE_Double, 10.0, 0.0, true
	);

	Parameters.Add_Choice(
		NULL	, "ALIGNMENT"		, _TL("Alignment"),
		_TL("Determines how the graticule is aligned to the extent, if division sizes do not fit."), 
		CSG_String::Format("%s|%s|%s|%s|%s|",
			_TL("bottom-left"),
			_TL("top-left"),
			_TL("bottom-right"),
			_TL("top-right"),
			_TL("centered")
		), 0
	);

    Parameters.Add_Bool(
        NULL    , "ROUND"           , _TL("Round"),
        _TL("Round bounding box coordinates to whole numbers; this blows up the bounding box."),
        false
    );
}


///////////////////////////////////////////////////////////
//                                                       //
///////////////////////////////////////////////////////////

//---------------------------------------------------------
int CGraticuleBuilder::On_Parameter_Changed(CSG_Parameters *pParameters, CSG_Parameter *pParameter)
{
	return( CSG_Tool::On_Parameter_Changed(pParameters, pParameter) );
}

//---------------------------------------------------------
int CGraticuleBuilder::On_Parameters_Enable(CSG_Parameters *pParameters, CSG_Parameter *pParameter)
{
	if( pParameter->Cmp_Identifier("TYPE") )
	{
		pParameters->Set_Enabled("GRATICULE_LINE", pParameter->asInt() == 0);
		pParameters->Set_Enabled("GRATICULE_RECT", pParameter->asInt() != 0);
	}

	if( pParameter->Cmp_Identifier("EXTENT") )
	{
		pParameters->Set_Enabled("EXTENT_X", pParameter->asShapes() == NULL);
		pParameters->Set_Enabled("EXTENT_Y", pParameter->asShapes() == NULL);
        pParameters->Set_Enabled("ROUND"   , pParameter->asShapes() != NULL);
	}

	return( CSG_Tool::On_Parameters_Enable(pParameters, pParameter) );
}


///////////////////////////////////////////////////////////
//                                                       //
///////////////////////////////////////////////////////////

//---------------------------------------------------------
bool CGraticuleBuilder::On_Execute(void)
{
	//-----------------------------------------------------
	TSG_Rect	Extent;
	
	if( Parameters("EXTENT")->asShapes() )
	{
		Extent		= Parameters("EXTENT")->asShapes()->Get_Extent();

        if( Parameters("ROUND")->asBool() )
        {
            Extent.xMin = floor(Extent.xMin);
            Extent.xMax = ceil(Extent.xMax);
            Extent.yMin = floor(Extent.yMin);
            Extent.yMax = ceil(Extent.yMax);
        }
	}
	else
	{
		Extent.xMin	= Parameters("EXTENT_X")->asRange()->Get_Min();
		Extent.yMin	= Parameters("EXTENT_Y")->asRange()->Get_Min();
		Extent.xMax	= Parameters("EXTENT_X")->asRange()->Get_Max();
		Extent.yMax	= Parameters("EXTENT_Y")->asRange()->Get_Max();
	}

	if( Extent.xMin >= Extent.xMax || Extent.yMin >= Extent.yMax )
	{
		Error_Set(_TL("invalid extent"));

		return( false );
	}

	//-----------------------------------------------------
	double	dx	= Parameters("DIVISION_X")->asDouble();
	double	dy	= Parameters("DIVISION_Y")->asDouble();

	if( dx <= 0.0 || dy <= 0.0 )
	{
		Error_Set(_TL("invalid division size"));

		return( false );
	}

	//-----------------------------------------------------
	int	nx	= (int)ceil((Extent.xMax - Extent.xMin) / dx);
	int	ny	= (int)ceil((Extent.yMax - Extent.yMin) / dy);

	switch( Parameters("ALIGNMENT")->asInt() )
	{
	default:	// bottom-left
	//	Extent.xMax	= Extent.xMin + nx * dx;
	//	Extent.yMax	= Extent.yMin + ny * dy;
		break;

	case  1:	// top-left
	//	Extent.xMax	= Extent.xMin + nx * dx;
		Extent.yMin	= Extent.yMax - ny * dy;
		break;

	case  2:	// bottom-right
		Extent.xMin	= Extent.xMax - nx * dx;
	//	Extent.yMax	= Extent.yMin + ny * dy;
		break;

	case  3:	// top-right
		Extent.xMin	= Extent.xMax - nx * dx;
		Extent.yMin	= Extent.yMax - ny * dy;
		break;

	case  4:	// centered
		{
			double	cx	= Extent.xMin + (Extent.xMax - Extent.xMin) / 2.0;
			double	cy	= Extent.yMin + (Extent.yMax - Extent.yMin) / 2.0;

			Extent.xMin	= cx - nx * dx / 2.0;
			Extent.yMin	= cy - ny * dy / 2.0;
		//	Extent.xMax	= cx + nx * dx / 2.0;
		//	Extent.yMax	= cy + ny * dy / 2.0;
		}
		break;
	}

	//-----------------------------------------------------
	CSG_Shapes	*pGraticule	= Parameters("TYPE")->asInt() == 0
		? Parameters("GRATICULE_LINE")->asShapes()
		: Parameters("GRATICULE_RECT")->asShapes();

	int			x, y;
	TSG_Point	p;

	switch( Parameters("TYPE")->asInt() )
	{
	//-----------------------------------------------------
	case 0:	// Lines
		{
			pGraticule->Create(SHAPE_TYPE_Line, _TL("Graticule"));

			pGraticule->Add_Field("ID", SG_DATATYPE_Int);

			for(x=0, p.x=Extent.xMin; x<=nx; x++, p.x+=dx)
			{
				CSG_Shape	*pLine	= pGraticule->Add_Shape();

				pLine->Set_Value(0, pGraticule->Get_Count());

				for(y=0, p.y=Extent.yMin; y<=ny; y++, p.y+=dy)
				{
					pLine->Add_Point(p.x, p.y);
				}
			}

			for(y=0, p.y=Extent.yMin; y<=ny; y++, p.y+=dy)
			{
				CSG_Shape	*pLine	= pGraticule->Add_Shape();

				pLine->Set_Value(0, pGraticule->Get_Count());

				for(x=0, p.x=Extent.xMin; x<=nx; x++, p.x+=dx)
				{
					pLine->Add_Point(p.x, p.y);
				}
			}
		}
		break;

	//-----------------------------------------------------
	case 1:	// Rectangles
		{
			pGraticule->Create(SHAPE_TYPE_Polygon, _TL("Graticule"));

			pGraticule->Add_Field("ID" , SG_DATATYPE_Int);
			pGraticule->Add_Field("ROW", SG_DATATYPE_Int);
			pGraticule->Add_Field("COL", SG_DATATYPE_Int);

			for(y=0, p.y=Extent.yMin; y<ny; y++, p.y+=dy)
			{
				p.x	= Extent.xMin;

				for(x=0, p.x=Extent.xMin; x<nx; x++, p.x+=dx)
				{
					CSG_Shape	*pRect	= pGraticule->Add_Shape();

					pRect->Set_Value(0, pGraticule->Get_Count());
					pRect->Set_Value(1, 1 + y);
					pRect->Set_Value(2, 1 + x);

					pRect->Add_Point(p.x     , p.y     );
					pRect->Add_Point(p.x     , p.y + dy);
					pRect->Add_Point(p.x + dx, p.y + dy);
					pRect->Add_Point(p.x + dx, p.y     );
					pRect->Add_Point(p.x     , p.y     );
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

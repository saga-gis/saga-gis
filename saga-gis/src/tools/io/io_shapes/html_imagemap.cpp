/**********************************************************
 * Version $Id: html_imagemap.cpp 1921 2014-01-09 10:24:11Z oconrad $
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
//                   html_imagemap.cpp                   //
//                                                       //
//                 Copyright (C) 2016 by                 //
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
#include "html_imagemap.h"


///////////////////////////////////////////////////////////
//														 //
//														 //
//														 //
///////////////////////////////////////////////////////////

//---------------------------------------------------------
CHTML_ImageMap::CHTML_ImageMap(void)
{
	Set_Name		(_TL("Export Polygons to HTML Image Map"));

	Set_Author		("P.Lamp, J.Wehberg, O.Conrad (c) 2016");

	Set_Description	(_TW(
		"Tool to create an HTML ImageMap with polygon dataset and image. "
		"The tool requires a polygon dataset and an georeferenced image file. "
		"It outputs an html file for possible further editing. "
		"The image name in the mapfile html is hard coded to be map.png.\n"
		"\n"
		"<b>How to build the link:</b>"
		"<ul>"
		"<li><b>Link</b> Select link identfier from the attribute table of the polygon data layer.\n"
		"<li><b>Prefix</b> Adds a prefix to the link identifier. If just for icon with alternative text and, add <i>'#'</i>).\n"
		"<li><b>Suffix</b> Adds a suffix of the link identifier. Likewise <i>'.txt, .html, .doc, .png'</i>.\n"
		"</ul>The link will be built with the pattern <i>'Prefix + Identifier + Suffix'</i>.\n"
		"\n"
		"<b>Take care!:</b>"
		"<ul>"
		"<li>Image and Polygon dataset have to share the same projection."
		"<li>Special characters in link (like &ouml;, &szlig, oder &raquo;) are not automatically replaced to html entities. Replace them after creation in editor if needed."
		"</ul>\n"
		"\n"
		"<b>Example for GUI usage:</b>"
		"<ol>"
		"<li>Load your polygon dataset and open it in a map."
		"<li>Create an image of this map using the map's menu command <i>'Save Map to Workspace'</i> and save it to an image file using the file name <i>'map.png'</i>."
		"<li>Run this tool and save the output to the same directory as the image."
		"<li>Open the html file in a browser for preview."
		"<li>Further editing of the html file is suggested."
		"</ol>"
	));

	//-----------------------------------------------------
	CSG_Parameter	*pNode	= Parameters.Add_Shapes(
		NULL	, "POLYGONS"		, _TL("Polygons"),
		_TL(""),
		PARAMETER_INPUT, SHAPE_TYPE_Polygon
	);

	Parameters.Add_Table_Field(
		pNode	, "LINK"			, _TL("Link"),
		_TL(""),
		true
	);

	Parameters.Add_Table_Field(
		pNode	, "TITLE"			, _TL("Title"),
		_TL(""),
		true
	);

	Parameters.Add_Grid_System(
		NULL	, "IMAGE"			, _TL("Image"),
		_TL("grid system of georeferenced image")
	);

	Parameters.Add_FilePath(
		NULL	, "FILE"			, _TL("File"),
		_TL(""),
		CSG_String::Format("%s|%s|%s|%s",
			_TL("HTML File (*.html)"), SG_T("*.html"),
			_TL("All Files")         , SG_T("*.*")
		), NULL, true
 	);

	Parameters.Add_String(
		NULL	, "LINK_PREFIX"		, _TL("Link Prefix"),
		_TL(""),
		"http://www.saga-gis.org/"
	);

	Parameters.Add_String(
		NULL	, "LINK_SUFFIX"		, _TL("Link Suffix"),
		_TL(""),
		"index.html"
	);
}


///////////////////////////////////////////////////////////
//														 //
///////////////////////////////////////////////////////////

//---------------------------------------------------------
bool CHTML_ImageMap::On_Execute(void)
{
	CSG_Shapes	*pPolygons	= Parameters("POLYGONS")->asShapes();

	m_Image	= *Parameters("IMAGE")->asGrid_System();

	if( !m_Image.Get_Extent().Intersects(pPolygons->Get_Extent()) )
	{
		Error_Set(_TL("image and polygons do not overlap"));

		return( false );
	}

	//-----------------------------------------------------
	CSG_MetaData	HTML, *pEntry;

	HTML.Set_Name("body");

	pEntry	= HTML.Add_Child("img");

	pEntry->Add_Property("src"   , "map.png");
	pEntry->Add_Property("width" , m_Image.Get_NX());
	pEntry->Add_Property("height", m_Image.Get_NY());
	pEntry->Add_Property("alt"   , "map");
	pEntry->Add_Property("usemap", "#image_map");

	pEntry	= HTML.Add_Child("map");
	pEntry->Add_Property("name", "image_map");

	//-----------------------------------------------------
	int	Link	= Parameters("LINK" )->asInt();
	int	Title	= Parameters("TITLE")->asInt();

	CSG_String	Prefix	= Parameters("LINK_PREFIX")->asString();
	CSG_String	Suffix	= Parameters("LINK_SUFFIX")->asString();

	for(int iPolygon=0; iPolygon<pPolygons->Get_Count() && Set_Progress(iPolygon, pPolygons->Get_Count()); iPolygon++)
	{
		CSG_Shape_Polygon	*pPolygon	= (CSG_Shape_Polygon *)pPolygons->Get_Shape(iPolygon);

		for(int iPart=0; iPart<pPolygon->Get_Part_Count(); iPart++)
		{
			CSG_String	Coords;

			if( !pPolygon->is_Lake(iPart) && Get_Polygon(Coords, pPolygon->Get_Part(iPart)) )
			{
				CSG_MetaData	*pArea	= pEntry->Add_Child("area");

				pArea->Add_Property("shape", "poly");
				pArea->Add_Property("coords", Coords);

				pArea->Add_Property("href", Prefix + pPolygon->asString(Link) + Suffix);

				CSG_String	s;

				if( Title < 0 )
				{
					s	= CSG_String::Format("%d. %s, %d. %s", 1 + iPolygon, _TL("Polygon"), 1 + iPart, _TL("Part"));
				}
				else
				{
					s	= pPolygon->asString(Title);
				}

				pArea->Add_Property("title", s);
				pArea->Add_Property("alt"  , s);
			}
		}
	}

	//-----------------------------------------------------
	if( !HTML.Save(Parameters("FILE")->asString(), SG_T("html")) )
	{
		Error_Fmt("%s [%s]", _TL("failed to save file"), Parameters("FILE")->asString());

		return( false );
	}

	return( true );
}


///////////////////////////////////////////////////////////
//														 //
///////////////////////////////////////////////////////////

//---------------------------------------------------------
bool CHTML_ImageMap::Get_Polygon(CSG_String &Coords, CSG_Shape_Part *pPolygon)
{
	double	Scale	= m_Image.Get_NX() / m_Image.Get_XRange(true);

	for(int iPoint=0; iPoint<pPolygon->Get_Count(); iPoint++)
	{
		TSG_Point	p	= pPolygon->Get_Point(iPoint);

		if( iPoint > 0 )
		{
			Coords	+= ",";
		}

		Coords	+= CSG_String::Format("%d,%d",
			(int)(0.5 + (p.x - m_Image.Get_XMin(true)) * Scale),
			(int)(0.5 + (m_Image.Get_YMax(true) - p.y) * Scale)
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

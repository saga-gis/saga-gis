
///////////////////////////////////////////////////////////
//                                                       //
//                         SAGA                          //
//                                                       //
//      System for Automated Geoscientific Analyses      //
//                                                       //
//                    User Interface                     //
//                                                       //
//                    Program: SAGA                      //
//                                                       //
//-------------------------------------------------------//
//                                                       //
//              SVG_Interactive_map.cpp                  //
//                                                       //
//          Copyright (C) 2005 by Victor Olaya           //
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
//    contact:    Olaf Conrad                            //
//                Institute of Geography                 //
//                University of Goettingen               //
//                Goldschmidtstr. 5                      //
//                37077 Goettingen                       //
//                Germany                                //
//                                                       //
//    e-mail:     oconrad@saga-gis.org                   //
//                                                       //
///////////////////////////////////////////////////////////

//---------------------------------------------------------


///////////////////////////////////////////////////////////
//														 //
//														 //
//														 //
///////////////////////////////////////////////////////////

//---------------------------------------------------------
#include "wksp_map_layer.h"
#include "wksp_layer_classify.h"

#include "svg_interactive_map.h"


///////////////////////////////////////////////////////////
//														 //
//														 //
//														 //
///////////////////////////////////////////////////////////

//---------------------------------------------------------
#define MAP_WINDOW_WIDTH	550.
#define MAP_WINDOW_HEIGHT	700.


///////////////////////////////////////////////////////////
//														 //
//														 //
//														 //
///////////////////////////////////////////////////////////

//---------------------------------------------------------
CSVG_Interactive_Map::CSVG_Interactive_Map(void)
{
}

//---------------------------------------------------------
CSVG_Interactive_Map::~CSVG_Interactive_Map(void)
{
}


///////////////////////////////////////////////////////////
//														 //
//														 //
//														 //
///////////////////////////////////////////////////////////

//---------------------------------------------------------
void CSVG_Interactive_Map::Create_From_Map(CWKSP_Map *pMap, CSG_Shapes *pIndexLayer, const wxChar *Filename)
{
	//-----------------------------------------------------
	m_Directory	= SG_File_Get_Path(Filename);

	_Add_Opening(pMap->Get_Extent());

	//-----------------------------------------------------
	m_sSVGCode.Append(wxT("<g id=\"mainMapGroup\" transform=\"translate(0,0)\">\n"));

	for(int i=pMap->Get_Count()-1; i>-1; i--)
	{
		switch( pMap->Get_Layer(i)->Get_Layer()->Get_Type() )
		{
		default:	break;
		case WKSP_ITEM_Grid:	_Add_Grid	((CWKSP_Grid   *)pMap->Get_Layer(i)->Get_Layer());	break;
		case WKSP_ITEM_Shapes:	_Add_Shapes	((CWKSP_Shapes *)pMap->Get_Layer(i)->Get_Layer());	break;
		}
	}

	m_sSVGCode.Append(wxT("</g>\n</svg>\n"));

	//-----------------------------------------------------
	_Add_CheckBoxes(pMap);

	_Add_ReferenceMap(pIndexLayer, pMap->Get_Extent());

	m_sSVGCode.Append(_Get_Code_Closing_1());

	Save(Filename);

	//-----------------------------------------------------
	_Write_Code(SG_File_Make_Path(m_Directory, wxT("checkbox")			, wxT("js")), _Get_Code_CheckBox());
	_Write_Code(SG_File_Make_Path(m_Directory, wxT("mapApp")			, wxT("js")), _Get_Code_MapApp	());
	_Write_Code(SG_File_Make_Path(m_Directory, wxT("timer")				, wxT("js")), _Get_Code_Timer	());
	_Write_Code(SG_File_Make_Path(m_Directory, wxT("slider")			, wxT("js")), _Get_Code_Slider	());
	_Write_Code(SG_File_Make_Path(m_Directory, wxT("helper_functions")	, wxT("js")), _Get_Code_Helper	());
	_Write_Code(SG_File_Make_Path(m_Directory, wxT("button")			, wxT("js")), _Get_Code_Buttons	());
	_Write_Code(SG_File_Make_Path(m_Directory, wxT("navigation")		, wxT("js")), CSG_String::Format(wxT("%s%s"), _Get_Code_Navigation_1(), _Get_Code_Navigation_2()));
}

//---------------------------------------------------------
void CSVG_Interactive_Map::_Write_Code(const wxChar *FileName, const wxChar *Code)
{
	CSG_File	Stream;

	if( Stream.Open(FileName, SG_FILE_W, false) )
	{
		Stream.Write((void *)Code, SG_STR_LEN(Code));
	}
}


///////////////////////////////////////////////////////////
//														 //
//														 //
//														 //
///////////////////////////////////////////////////////////

//---------------------------------------------------------
void CSVG_Interactive_Map::_Add_Opening(CSG_Rect r)
{
	CSG_String sViewBox;
	double Width, Height;
	double OffsetX, OffsetY;

	m_sSVGCode.Append(_Get_Opening_Code_1());
	m_sSVGCode.Append(SG_Get_String(r.Get_XRange(),2));
	m_sSVGCode.Append(wxT("),"));
	m_sSVGCode.Append(SG_Get_String(r.Get_XRange() / 400.,2));
	m_sSVGCode.Append(_Get_Opening_Code_2());

	if (r.Get_XRange() / r.Get_YRange()  > MAP_WINDOW_WIDTH / MAP_WINDOW_HEIGHT)
	{
		Width = r.Get_XRange();
		Height =  r.Get_XRange() * MAP_WINDOW_HEIGHT / MAP_WINDOW_WIDTH;
	}
	else
	{
		Height = r.Get_YRange();
		Width = r.Get_YRange() / MAP_WINDOW_HEIGHT * MAP_WINDOW_WIDTH;
	}

	OffsetX = (Width - r.Get_XRange()) / 2.;
	OffsetY = (Height - r.Get_YRange()) / 2.;

	sViewBox.Append(SG_Get_String(r.Get_XMin() - OffsetX,2));
	sViewBox.Append(wxT(" "));
	sViewBox.Append(SG_Get_String(-r.Get_YMax() - OffsetY,2));
	sViewBox.Append(wxT(" "));
	sViewBox.Append(SG_Get_String(Width,2));
	sViewBox.Append(wxT(" "));
	sViewBox.Append(SG_Get_String(Height,2));

	_AddAttribute(wxT("viewBox"), sViewBox);	
	m_sSVGCode.Append(wxT(">\n"));

	m_dWidth = Width;
}

//---------------------------------------------------------
const wxChar * CSVG_Interactive_Map::_Get_Opening_Code_1(void)
{
	return( SG_STR_MBTOSG(
		"<?xml version=\"1.0\" encoding=\"UTF-8\" standalone=\"no\"?>\n"
		"<!DOCTYPE svg PUBLIC \"-//W3C//DTD SVG 1.1//EN\" \"http://www.w3.org/Graphics/SVG/1.1/DTD/svg11.dtd\" [\n"
		"<!ATTLIST svg\n"
		"	xmlns:attrib CDATA #IMPLIED\n"
		"	xmlns:batik CDATA #IMPLIED\n"
		">\n"
		"<!ATTLIST g\n"
		"	batik:static CDATA #IMPLIED\n"
		">\n"
		"<!ATTLIST image\n"
		"	batik:static CDATA #IMPLIED\n"
		">\n"
		"<!ATTLIST path\n"
		"	batik:static CDATA #IMPLIED\n"
		">\n"
		"]>\n"
		"<?AdobeSVGViewer save=\"snapshot\"?>\n"
		"<svg width=\"100%\" height=\"100%\" viewBox=\"0 0 1024 768\" xmlns=\"http://www.w3.org/2000/svg\" xmlns:xlink=\"http://www.w3.org/1999/xlink\" xmlns:attrib=\"http://www.carto.net/attrib\" xmlns:batik=\"http://xml.apache.org/batik/ext\" onload=\"init(evt);\" zoomAndPan=\"disable\">\n"
		"	<script type=\"text/ecmascript\" xlink:href=\"helper_functions.js\"/>\n"
		"	<script type=\"text/ecmascript\" xlink:href=\"mapApp.js\"/>\n"
		"	<script type=\"text/ecmascript\" xlink:href=\"timer.js\"/>\n"
		"	<script type=\"text/ecmascript\" xlink:href=\"slider.js\"/>\n"
		"	<script type=\"text/ecmascript\" xlink:href=\"button.js\"/>\n"
		"	<script type=\"text/ecmascript\" xlink:href=\"checkbox.js\"/>\n"
		"	<script type=\"text/ecmascript\" xlink:href=\"navigation.js\"/>\n"
		"	<script type=\"text/ecmascript\"><![CDATA[\n"
		"		//global variables for mapApp and map object\n"
		"		var myMapApp = new mapApp();\n"
		"		var myMainMap;\n"
		"		function init(evt) {\n"
		"			//dynamic layer array that allow loading from database\n"
		"			var dynamicLayers = new Array();\n"
		"			//initialize digiLayers (layers that allow digitizing)\n"
		"			var digiLayers = new Array();\n"
		"			//initialize myMainMap object\n"
		"			myMainMap = new map(\"mainMap\"),"
	));
}

//---------------------------------------------------------
const wxChar * CSVG_Interactive_Map::_Get_Opening_Code_2(void)
{
	return( SG_STR_MBTOSG(
		"),0.6,0,0,\"\"),1,true,\"coordX\"),\"coordY\"),dynamicLayers,digiLayers,\"\"));\n"
		"			//initialize refMapDragger object\n"
		"			myMapApp.refMapDragger = new dragObj(\"dragRectForRefMap\"),\"referenceMap\"),\"myDragCross\"),0.1,true,\"coordX\"),\"coordY\"),myMainMap);\n"
		"			//create zoom slider	\n"
		"			myMapApp.zoomSlider = new slider(715,75,myMainMap.minWidth,715,165,myMainMap.maxWidth,myMainMap.maxWidth,\"mapZoomSlider\"),\"dimgray\"),2,10,\"sliderSymbol\"),myMapApp.refMapDragger,true);\n"
		"			//now initialize buttons\n"
		"			myMapApp.buttons = new Array();\n"
		"			//groupId,functionToCall,buttonType,buttonText,buttonSymbolId,x,y,width,height,fontSize,fontFamily,textFill,buttonFill,shadeLightFill,shadeDarkFill,shadowOffset\n"
		"			myMapApp.buttons[\"zoomIn\"] = new button(\"zoomIn\"),zoomImageButtons,\"rect\"),undefined,\"magnifyerZoomIn\"),705,47,20,20,10,\"\"),\"\"),\"white\"),\"rgb(235,235,235)\"),\"dimgray\"),1);\n"
		"			myMapApp.buttons[\"zoomOut\"] = new button(\"zoomOut\"),zoomImageButtons,\"rect\"),undefined,\"magnifyerZoomOut\"),705,173,20,20,10,\"\"),\"\"),\"white\"),\"rgb(235,235,235)\"),\"dimgray\"),1);\n"
		"			myMapApp.buttons[\"infoButton\"] = new switchbutton(\"infoButton\"),zoomImageSwitchButtons,\"rect\"),undefined,\"infoBut\"),746,74,20,20,10,\"\"),\"\"),\"white\"),\"rgb(235,235,235)\"),\"dimgray\"),1);\n"
		"			myMapApp.buttons[\"infoButton\"].setSwitchValue(true,false);\n"
		"			statusChange(\"Mode: Infomode\"));\n"
		"			myMapApp.buttons[\"zoomFull\"] = new button(\"zoomFull\"),zoomImageButtons,\"rect\"),undefined,\"magnifyerFull\"),771,74,20,20,10,\"\"),\"\"),\"white\"),\"rgb(235,235,235)\"),\"dimgray\"),1);\n"
		"			myMapApp.buttons[\"zoomManual\"] = new switchbutton(\"zoomManual\"),zoomImageSwitchButtons,\"rect\"),undefined,\"magnifyerManual\"),796,74,20,20,10,\"\"),\"\"),\"white\"),\"rgb(235,235,235)\"),\"dimgray\"),1);\n"
		"			myMapApp.buttons[\"panManual\"] = new switchbutton(\"panManual\"),zoomImageSwitchButtons,\"rect\"),undefined,\"symbPan\"),821,74,20,20,10,\"\"),\"\"),\"white\"),\"rgb(235,235,235)\"),\"dimgray\"),1);\n"
		"			myMapApp.buttons[\"recenterMap\"] = new switchbutton(\"recenterMap\"),zoomImageSwitchButtons,\"rect\"),undefined,\"symbRecenter\"),846,74,20,20,10,\"\"),\"\"),\"white\"),\"rgb(235,235,235)\"),\"dimgray\"),1);\n"
		"			myMapApp.buttons[\"backwardExtent\"] = new button(\"backwardExtent\"),zoomImageButtons,\"rect\"),undefined,\"symbArrowLeft\"),871,74,20,20,10,\"\"),\"\"),\"white\"),\"rgb(235,235,235)\"),\"dimgray\"),1);\n"
		"			myMapApp.buttons[\"forwardExtent\"] = new button(\"forwardExtent\"),zoomImageButtons,\"rect\"),undefined,\"symbArrowRight\"),896,74,20,20,10,\"\"),\"\"),\"white\"),\"rgb(235,235,235)\"),\"dimgray\"),1);\n"
		"			//see if we need to disable buttons\n"
		"			myMainMap.checkButtons();\n"
		"			//load function specific to the current map project\n"
		"			loadProjectSpecific();\n"
		"		}\n"
		"		function loadProjectSpecific() {\n"
		"			//adopt width and height of map extent\n"
		"			document.getElementById(\"myScaleTextW\")).firstChild.nodeValue = formatNumberString(myMainMap.curWidth.toFixed(myMainMap.nrDecimals)) + myMainMap.units;\n"
		"			document.getElementById(\"myScaleTextH\")).firstChild.nodeValue = formatNumberString(myMainMap.curHeight.toFixed(myMainMap.nrDecimals)) + myMainMap.units;\n"
		"		}\n"
		"]]></script>\n"
		"	<defs>\n"
		"		<!-- Symbols for checkboxes -->\n"
		"		<symbol id=\"checkBoxRect\" overflow=\"visible\">\n"
		"			<rect x=\"-6\" y=\"-6\" width=\"12\" height=\"12\" fill=\"white\" stroke=\"dimgray\" stroke-width=\"1.5\"/>\n"
		"		</symbol>\n"
		"		<symbol id=\"checkBoxCross\" overflow=\"visible\" fill=\"none\" stroke=\"dimgray\" stroke-width=\"1\" pointer-events=\"none\">\n"
		"			<line x1=\"-5\" y1=\"-5\" x2=\"5\" y2=\"5\"/>\n"
		"			<line x1=\"-5\" y1=\"5\" x2=\"5\" y2=\"-5\"/>\n"
		"		</symbol>\n"
		"		<!-- Symbols for Zoom Magnifyer glasses -->\n"
		"		<symbol id=\"magnifyerFull\" overflow=\"visible\">\n"
		"			<text y=\"7.5\" font-family=\"sans-serif\" fill=\"dimgray\" font-size=\"18px\" font-weight=\"bold\" text-anchor=\"middle\" pointer-events=\"none\">F</text>\n"
		"		</symbol>\n"
		"		<symbol id=\"infoBut\" overflow=\"visible\">\n"
		"			<circle fill=\"none\" stroke=\"dimgray\" stroke-width=\"1.5\" r=\"7.5\" />\n"
		"			<text y=\"5\" font-family=\"sans-serif\" font-size=\"13px\" font-weight=\"bold\" fill=\"dimgray\" text-anchor=\"middle\" pointer-events=\"none\">i</text>\n"
		"		</symbol>\n"
		"		<symbol id=\"magnifyerManual\" overflow=\"visible\" fill=\"none\" stroke=\"dimgray\" stroke-width=\"1.5\">\n"
		"			<rect x=\"-6\" y=\"-6\" width=\"12\" height=\"12\" stroke-dasharray=\"1.5,1.5\"/>\n"
		"			<line x1=\"-3\" y1=\"0\" x2=\"3\" y2=\"0\" />\n"
		"			<line x1=\"0\" y1=\"-3\" x2=\"0\" y2=\"3\" />\n"
		"		</symbol>\n"
		"		<symbol id=\"magnifyerZoomIn\" overflow=\"visible\" fill=\"none\" stroke=\"dimgray\" stroke-width=\"2\">\n"
		"			<line x1=\"-4\" y1=\"0\" x2=\"4\" y2=\"0\"/>\n"
		"			<line x1=\"0\" y1=\"-4\" x2=\"0\" y2=\"4\"/>\n"
		"		</symbol>\n"
		"		<symbol id=\"magnifyerZoomOut\" overflow=\"visible\">\n"
		"			<line x1=\"-4\" y1=\"0\" x2=\"4\" y2=\"0\" fill=\"none\" stroke=\"dimgray\" stroke-width=\"2\" />\n"
		"		</symbol>\n"
		"		<!-- hand symbol for panning -->\n"
		"		<symbol id=\"symbPan\" overflow=\"visible\">\n"
		"			<path transform=\"scale(1.2)\" fill=\"none\" stroke=\"dimgray\" stroke-width=\"1\" d=\"M-2 6 C -2.2 2.5 -8.0 -0 -5.7 -1.9 C -4.3 -2.5 -3.3 -0.5 -2.5 0.7 C -3.2 -2.1 -5.5 -5.2 -3.6 -5.8 C -2.1 -6.3 -1.6 -3.6 -1.1 -1.9 C -0.9 -4.2 -1.6 -6.4 -0.2 -6.6 C 1.4 -6.8 0.9 -3 1.1 -1.9 C 1.5 -3.5 1.2 -6.1 2.5 -6.1 C 3.9 -6.1 3.5 -3.2 3.6 -1.6 C 4 -2.9 4.1 -4.3 5.3 -4.4 C 7.3 -3.5 4 2.2 3 6z\"/>\n"
		"		</symbol>\n"
		"		<!-- Symbol for Arrows -->\n"
		"		<symbol id=\"symbArrow\" overflow=\"visible\">\n"
		"			<polyline fill=\"none\" stroke=\"dimgray\" stroke-width=\"1\" points=\"-3,-6 3,-6 3,1 5,1 0,7 -5,1 -3,1 -3,-5\"/>\n"
		"		</symbol>\n"
		"		<symbol id=\"symbArrowLeft\" overflow=\"visible\">\n"
		"			<use xlink:href=\"#symbArrow\" transform=\"rotate(90)\" />\n"
		"		</symbol>\n"
		"		<symbol id=\"symbArrowRight\" overflow=\"visible\">\n"
		"			<use xlink:href=\"#symbArrow\" transform=\"rotate(-90)\" />\n"
		"		</symbol>\n"
		"		<!-- Symbol for Recentering Map -->\n"
		"		<symbol id=\"symbRecenter\" overflow=\"visible\">\n"
		"			<circle fill=\"dimgray\" cx=\"0\" cy=\"0\" r=\"1\" pointer-events=\"none\"/>\n"
		"			<g fill=\"none\" stroke=\"dimgray\" stroke-width=\"1.5\" pointer-events=\"none\">\n"
		"				<line x1=\"-7\" y1=\"-7\" x2=\"-3\" y2=\"-3\"/>\n"
		"				<line x1=\"7\" y1=\"7\" x2=\"3\" y2=\"3\"/>\n"
		"				<line x1=\"-7\" y1=\"7\" x2=\"-3\" y2=\"3\"/>\n"
		"				<line x1=\"7\" y1=\"-7\" x2=\"3\" y2=\"-3\"/>\n"
		"			</g>\n"
		"		</symbol>\n"
		"		<!-- Symbol for Slider -->\n"
		"		<symbol id=\"sliderSymbol\" overflow=\"visible\" pointer-events=\"none\">\n"
		"			<line x1=\"0\" y1=\"-5\" x2=\"0\" y2=\"5\" fill=\"none\" stroke=\"dimgray\" stroke-width=\"5\"/>\n"
		"		</symbol>\n"
		"		<!-- Symbol for Dragging if zoomed in far -->\n"
		"		<symbol id=\"myDragCrossSymbol\" overflow=\"visible\" stroke-width=\"2000\" fill=\"none\" stroke=\"darkblue\" pointer-events=\"none\">\n"
		"			<line x1=\"-7000\" y1=\"0\" x2=\"-2500\" y2=\"0\"/>\n"
		"			<line x1=\"7000\" y1=\"0\" x2=\"2500\" y2=\"0\"/>\n"
		"			<line x1=\"0\" y1=\"-3300\" x2=\"0\" y2=\"-7800\"/>\n"
		"			<line x1=\"0\" y1=\"3300\" x2=\"0\" y2=\"7800\"/>\n"
		"		</symbol>\n"
		"		<!-- Marker for Extent-Arrows -->\n"
		"		<marker id=\"myStartArrow\" overflow=\"visible\" orient=\"auto\">\n"
		"			<polyline fill=\"dimgray\" points=\"-0.5,0 8,-2 8,2\"/>\n"
		"		</marker>\n"
		"		<marker id=\"myEndArrow\" overflow=\"visible\" orient=\"auto\">\n"
		"			<polyline fill=\"dimgray\" points=\"0.5,0 -8,-2 -8,2\"/>\n"
		"		</marker>\n"
		"	</defs>\n"
		"	<rect x=\"-500\" y=\"-500\" width=\"3000\" height=\"3000\" stroke=\"none\" fill=\"aliceblue\" />\n"
		"	<!-- Main Map Frame -->\n"
		"	<svg id=\"mainMap\" x=\"0\" y=\"15\" width=\"550\" height=\"700\"	cursor=\"crosshair\" "
	));
}


///////////////////////////////////////////////////////////
//														 //
//														 //
//														 //
///////////////////////////////////////////////////////////

//---------------------------------------------------------
void CSVG_Interactive_Map::_Add_ReferenceMap(CSG_Shapes *pIndexLayer, CSG_Rect r)
{
	int i;
	CSG_String sViewBox;
	double Line_Width, Point_Width;
	double Width, Height;
	double OffsetX, OffsetY;
	CSG_Shape *pShape;

	if (r.Get_XRange() / r.Get_YRange()  > MAP_WINDOW_WIDTH / MAP_WINDOW_HEIGHT)
	{
		Width = r.Get_XRange();
		Height =  r.Get_XRange() * MAP_WINDOW_HEIGHT / MAP_WINDOW_WIDTH;
	}
	else
	{
		Height = r.Get_YRange();
		Width = r.Get_YRange() / MAP_WINDOW_HEIGHT * MAP_WINDOW_WIDTH;
	}

	OffsetX = (Width - r.Get_XRange()) / 2.;
	OffsetY = (Height - r.Get_YRange()) / 2.;

	sViewBox.Append(SG_Get_String(r.Get_XMin() - OffsetX,2));
	sViewBox.Append(wxT(" "));
	sViewBox.Append(SG_Get_String(- r.Get_YMax() - OffsetY,2));
	sViewBox.Append(wxT(" "));
	sViewBox.Append(SG_Get_String(Width,2));
	sViewBox.Append(wxT(" "));
	sViewBox.Append(SG_Get_String(Height,2));

	_AddAttribute(wxT("viewBox"), sViewBox);	

	m_sSVGCode.Append(_Get_Code_3());
	
	if (pIndexLayer)
	{
		m_sSVGCode.Append(wxT("<g id=\"index\">\n"));
		for (i = 0; i < pIndexLayer->Get_Count(); i++)
		{
			pShape = pIndexLayer->Get_Shape(i);
			Line_Width = Point_Width = m_dWidth / MAP_WINDOW_WIDTH;
			Point_Width *= 5;
			_Add_Shape(pShape, 0, 0, Line_Width, Point_Width);
		}
		m_sSVGCode.Append(wxT("</g>\n"));
	}

	m_sSVGCode.Append(_Get_Code_4());
		
	_AddAttribute(wxT("x"), r.Get_XMin() - OffsetX);
	_AddAttribute(wxT("y"), - r.Get_YMax() - OffsetY);
	_AddAttribute(wxT("width"), Width);
	_AddAttribute(wxT("height"), Height);

	m_sSVGCode.Append(_Get_Code_5());

}

//---------------------------------------------------------
void CSVG_Interactive_Map::_Add_Grid(CWKSP_Grid *pLayer)
{
	wxString Filename;
	wxBitmap BMP;
	
	if( pLayer->Get_Image_Grid(BMP) )
	{
		Filename	= SG_File_Make_Path(m_Directory.c_str(), pLayer->Get_Object()->Get_Name(), wxT("jpg"));
		BMP.SaveFile(Filename, wxBITMAP_TYPE_JPEG);

		m_sSVGCode.Append(wxT("<image "));

		_AddAttribute(wxT("id")			, pLayer->Get_Object()->Get_Name());
		_AddAttribute(wxT("x")			, ((CSG_Grid *)pLayer->Get_Object())->Get_XMin());
		_AddAttribute(wxT("y")			,-((CSG_Grid *)pLayer->Get_Object())->Get_YMax());
		_AddAttribute(wxT("width")		, ((CSG_Grid *)pLayer->Get_Object())->Get_XRange());
		_AddAttribute(wxT("height")		, ((CSG_Grid *)pLayer->Get_Object())->Get_YRange());
		_AddAttribute(wxT("xlink:href")	, SG_File_Get_Name(Filename, true));

		m_sSVGCode.Append(wxT("/>"));
	}
}

//---------------------------------------------------------
void CSVG_Interactive_Map::_Add_Shapes(CWKSP_Shapes *pLayer)
{
	int i;
	int iColorField;
	int Line_Color, Fill_Color; 
	double Line_Width, Point_Width;
	double dSize;
	CSG_Shape *pShape;
	CSG_String sLabel;

	m_sSVGCode.Append(wxT("<g id=\""));
	m_sSVGCode.Append(pLayer->Get_Object()->Get_Name());		
	m_sSVGCode.Append(wxT("\" >\n"));
	
	iColorField = pLayer->Get_Color_Field();

	m_sSVGCode.Append(wxT("<g transform=\"scale(1,-1)\">\n"));

	for (i = 0; i < ((CSG_Shapes*)pLayer->Get_Object())->Get_Count(); i++)
	{
		pShape = pLayer->Get_Shapes()->Get_Shape(i);
		Line_Color = Fill_Color = pLayer->Get_Classifier()->Get_Class_Color_byValue(pShape->asDouble(iColorField));
		Line_Width = Point_Width = m_dWidth / MAP_WINDOW_WIDTH;
		Point_Width *= 5;
		_Add_Shape(pShape, Fill_Color, Line_Color, Line_Width, Point_Width);
	}
	
	if (pLayer->Get_Label_Field() >= 0)
	{
		m_sSVGCode.Append(wxT("<g transform=\"scale(0.01,-0.01)\"\n>"));
		for (i = 0; i < ((CSG_Shapes*)pLayer->Get_Object())->Get_Count(); i++)
		{
			pShape = pLayer->Get_Shapes()->Get_Shape(i);
			Line_Width = Point_Width = m_dWidth / MAP_WINDOW_WIDTH;
			sLabel = pShape->asString(pLayer->Get_Label_Field());
			switch( pLayer->Get_Parameters()->Get_Parameter(wxT("LABEL_ATTRIB_SIZE_TYPE"))->asInt() )
			{
				case 0:	default:
					dSize = Line_Width * pLayer->Get_Parameters()->Get_Parameter(wxT("LABEL_ATTRIB_FONT"))->asFont()->GetPointSize();
					_Add_Label(sLabel, pShape, dSize, wxT("%"));
					break;
				case 1:
					dSize =  pLayer->Get_Parameters()->Get_Parameter(wxT("LABEL_ATTRIB_SIZE"))->asDouble();
					_Add_Label(sLabel, pShape, dSize, wxT(""));
					break;
			}
		}

		m_sSVGCode.Append(wxT("</g>\n"));
	}

	m_sSVGCode.Append(wxT("</g>\n</g>\n"));
}

//---------------------------------------------------------
void CSVG_Interactive_Map::_Add_Label(const wxChar* Label, CSG_Shape *pShape, double dSize, const wxChar* Unit)
{

	int iPoint, iPart;
	TSG_Point Point;

	switch( pShape->Get_Type() )
	{
	case SHAPE_TYPE_Point:
	case SHAPE_TYPE_Points:
		for(iPoint=0; iPoint < pShape->Get_Point_Count(0); iPoint++)
		{
			Point = pShape->Get_Point(iPoint);
			Draw_Text(100 * Point.x, -100 * Point.y, Label, 0, wxT("Verdana"), 100 * dSize);
		}
		break;

	case SHAPE_TYPE_Line:
		///************///
		break;

	case SHAPE_TYPE_Polygon:
		
		for (iPart = 0; iPart < pShape->Get_Part_Count(); iPart++)
		{
			if(! ((CSG_Shape_Polygon *)pShape)->is_Lake(iPart) )
			{
				Point = ((CSG_Shape_Polygon *)pShape)->Get_Centroid(iPart);
				Draw_Text(100 * Point.x, -100 * Point.y, Label, 0, wxT("Verdana"), 100 * dSize);
			}
		}
		break;
	}

}

//---------------------------------------------------------
bool CSVG_Interactive_Map::_Add_Shape(CSG_Shape *pShape, int Fill_Color, int Line_Color, double Line_Width, double Point_Width)
{
	if( pShape && pShape->is_Valid() )
	{
		int				iPart, iPoint;
		TSG_Point		Point;
		CSG_Points	Points;

		for(iPart=0; iPart<pShape->Get_Part_Count(); iPart++)
		{
			Points.Clear();

			for(iPoint=0; iPoint<pShape->Get_Point_Count(iPart); iPoint++)
			{
				Point = pShape->Get_Point(iPoint, iPart);
				Points.Add(Point.x, Point.y);
			}

			switch( pShape->Get_Type() )
			{
			case SHAPE_TYPE_Point:
			case SHAPE_TYPE_Points:
				for(iPoint=0; iPoint<Points.Get_Count(); iPoint++)
				{
					Draw_Circle(Points[iPoint].x, Points[iPoint].y, Point_Width, Fill_Color, 0, Line_Width);
				}
				break;

			case SHAPE_TYPE_Line:
				Draw_Line(Points, Line_Width, Line_Color);
				break;

			case SHAPE_TYPE_Polygon:
				if( ((CSG_Shape_Polygon *)pShape)->is_Lake(iPart) )
				{
					Draw_Polygon(Points, -1, 0, 0.01);
				}
				else
				{
					Draw_Polygon(Points, Fill_Color, 0, 0.01);
				}
				break;
			}
		}

		return( true );
	}

	return( false );
}

//---------------------------------------------------------
void CSVG_Interactive_Map::_Add_CheckBoxes(CWKSP_Map *pMap)
{

	int i;
	CSG_String s;
	int x,y;
	int iRow = 1;
	
	m_sSVGCode.Append(_Get_Code_1());	// 2* <g> open
	m_sSVGCode.Append(LNG("Layers"));
	m_sSVGCode.Append(wxT("</text>\n"));

	for (i = 0; i < pMap->Get_Count(); i++)
	{
		y = iRow * 30;

		if (i % 2)
		{
			x = 200;		
			iRow++;
		}
		else
		{
			x = 0;
		}
	
		m_sSVGCode.Append(wxT("<g "));
		s = wxT("translate(");
		s.Append(SG_Get_String(x,0));
		s.Append(wxT(" "));
		s.Append(SG_Get_String(y,0));
		s.Append(wxT("))"));
		_AddAttribute(wxT("transform"), s);
		m_sSVGCode.Append(wxT(">\n"));

		m_sSVGCode.Append(wxT("<use "));
		s = wxT("checkBox");
		s.Append(pMap->Get_Layer(i)->Get_Layer()->Get_Object()->Get_Name());
		_AddAttribute(wxT("id"), s);
		_AddAttribute(wxT("xlink:href"), wxT("#checkBoxRect"));
		s = wxT("checkBoxScript(evt,'");
		s.Append(pMap->Get_Layer(i)->Get_Layer()->Get_Object()->Get_Name());
		s.Append(wxT("');"));
		_AddAttribute(wxT("onclick"), s);
		m_sSVGCode.Append(wxT("/>\n"));

		m_sSVGCode.Append(wxT("<use "));
		s = wxT("checkCross");
		s.Append(pMap->Get_Layer(i)->Get_Layer()->Get_Object()->Get_Name());
		_AddAttribute(wxT("id"), s);
		_AddAttribute(wxT("xlink:href"), wxT("#checkBoxCross"));		
		_AddAttribute(wxT("visibility"), wxT("visible"));
		m_sSVGCode.Append(wxT("/>\n"));

		m_sSVGCode.Append(wxT("</g>\n"));
	}

	m_sSVGCode.Append(wxT("<g font-family=\"sans-serif\" fill=\"dimgray\" font-size=\"15px\" pointer-events=\"none\">\n"));
	
	iRow = 1;
		
	for (i = 0; i < pMap->Get_Count(); i++)
	{
		y = iRow * 30 + 6;

		if (i % 2)
		{
			x = 212;
			iRow++;
		}
		else
		{
			x = 12;
		}

		m_sSVGCode.Append(wxT("<text "));	
		_AddAttribute(wxT("x"), x);
		_AddAttribute(wxT("y"), y);
		m_sSVGCode.Append(wxT(">"));
		m_sSVGCode.Append(pMap->Get_Layer(i)->Get_Layer()->Get_Object()->Get_Name());
		m_sSVGCode.Append(wxT("</text>\n"));	

	}

	m_sSVGCode.Append(wxT("</g>\n"));

	m_sSVGCode.Append(_Get_Code_2());
}


///////////////////////////////////////////////////////////
//														 //
//														 //
//														 //
///////////////////////////////////////////////////////////

//---------------------------------------------------------
const wxChar * CSVG_Interactive_Map::_Get_Code_1(void)
{
	return( SG_STR_MBTOSG(
		"<g>\n"
		"		<g font-family=\"sans-serif\" fill=\"dimgray\" font-size=\"15px\" pointer-events=\"none\">"
		"			<text id=\"coordX\" x=\"745\" y=\"145\">X:</text>\n"
		"			<text id=\"coordY\" x=\"845\" y=\"145\">Y:</text>\n"
		"		</g>\n"
		"		<g>"
		"			<!-- vertical line, displays current height of map extent -->\n"
		"			<line stroke=\"dimgray\" stroke-width=\"1.5\" marker-start=\"url(#myStartArrow)\" marker-end=\"url(#myEndArrow)\" x1=\"561\" y1=\"15\" x2=\"561\" y2=\"715\"/>\n"
		"			<rect fill=\"aliceblue\" x=\"555\" y=\"330\" width=\"12\" height=\"70\"/>\n"
		"			<text id=\"myScaleTextH\" font-family=\"sans-serif\" fill=\"dimgray\" font-size=\"15px\" text-anchor=\"middle\" transform=\"translate(556,365),rotate(90)\" pointer-events=\"none\">-</text>\n"
		"			<!-- horizontal line, displays current width of map extent -->\n"
		"			<line stroke=\"dimgray\" stroke-width=\"1.5\" marker-start=\"url(#myStartArrow)\" marker-end=\"url(#myEndArrow)\" x1=\"0\" y1=\"723\" x2=\"553.1\" y2=\"723\"/>\n"
		"			<rect fill=\"aliceblue\" x=\"242\" y=\"717\" width=\"70\" height=\"12\"/>\n"
		"			<text id=\"myScaleTextW\" font-family=\"sans-serif\" fill=\"dimgray\" font-size=\"15px\" text-anchor=\"middle\" transform=\"translate(277,728)\" pointer-events=\"none\">-</text>\n"
		"			<!-- small rectangle in lower right corner, onclick displays current map extent in real world coordinates -->\n"
		"			<rect fill=\"dimgray\" x=\"558.5\" y=\"720.5\" width=\"5\" height=\"5\" onclick=\"showExtent()\" />\n"
		"		</g>\n"
		"		<g transform=\"translate(590 225)\">\n"
		"			<text font-family=\"sans-serif\" fill=\"dimgray\" font-size=\"18px\" font-weight=\"bold\" x=\"-10\" y=\"0\" pointer-events=\"none\">"
	));
}

//---------------------------------------------------------
const wxChar * CSVG_Interactive_Map::_Get_Code_2(void)
{
	return( SG_STR_MBTOSG(
		"</g>\n"
		"   <g text-rendering=\"optimizeLegibility\" font-family=\"sans-serif\" fill=\"dimgray\" font-size=\"12px\">\n"
		"		<!-- text with this id is required to show help texts -->\n"
		"		<text id=\"statusText\" x=\"745\" y=\"125\">Statusbar: Your Status Text</text>\n"
		"   </g>\n"
		"</g>\n"
		"<svg id=\"referenceMap\" x=\"580\" y=\"45\" "
	));
}

//---------------------------------------------------------
const wxChar * CSVG_Interactive_Map::_Get_Code_3(void)
{
	return( SG_STR_MBTOSG(
		"width=\"118.52\" height=\"150\" onmousedown=\"myMapApp.refMapDragger.handleEvent(evt)\" onmousemove=\"myMapApp.refMapDragger.handleEvent(evt)\" onmouseup=\"myMapApp.refMapDragger.handleEvent(evt)\" onmouseout=\"myMapApp.refMapDragger.handleEvent(evt)\" cursor=\"crosshair\">\n"
		"		<g transform=\"scale(1,-1)\"\n>"
	));
}

//---------------------------------------------------------
const wxChar * CSVG_Interactive_Map::_Get_Code_4(void)
{
	return( SG_STR_MBTOSG(
		"		</g>\n"
		"		<rect id=\"dragRectForRefMap\" fill=\"lightskyblue\" fill-opacity=\"0.4\" "
	));
}

//---------------------------------------------------------
const wxChar * CSVG_Interactive_Map::_Get_Code_5(void)
{
	return( SG_STR_MBTOSG(
		" pointer-events=\"none\" />\n	"
		"		<use id=\"myDragCross\" x=\"0\" y=\"0\" xlink:href=\"#myDragCrossSymbol\" visibility=\"hidden\" />\n"
		"	</svg>\n"
	));
}

//---------------------------------------------------------
const wxChar * CSVG_Interactive_Map::_Get_Code_Closing_1(void)
{
	return( SG_STR_MBTOSG(
		"	<g>\n"
		"		<!-- empty group where zoomSlider is later appended -->\n"
		"		<g id=\"mapZoomSlider\"/>\n"
		"		<!-- button for zoom-in, above slider -->\n"
		"		<g id=\"zoomIn\" cursor=\"pointer\" />\n"
		"		<!-- button for zoom-out, below slider -->\n"
		"		<g id=\"zoomOut\" cursor=\"pointer\" />\n"
		"		<!-- info button -->\n"
		"		<g id=\"infoButton\" cursor=\"pointer\" />\n"
		"		<!-- button for full view (f) -->\n"
		"		<g id=\"zoomFull\" cursor=\"pointer\" />\n"
		"		<!-- button for manual zoom (rectangle) -->\n"
		"		<g id=\"zoomManual\" cursor=\"pointer\" />\n"
		"		<!-- button for manual pan -->\n"
		"		<g id=\"panManual\" cursor=\"pointer\" />\n"
		"		<!-- button for redefining map center -->\n"
		"		<g id=\"recenterMap\" cursor=\"pointer\" />\n"
		"		<!-- button for backwarding map extent (extent history) -->\n"
		"		<g id=\"backwardExtent\" cursor=\"pointer\" />\n"
		"		<!-- button for forwarding map extent (extent history) -->\n"
		"		<g id=\"forwardExtent\" cursor=\"pointer\" />\n"
		"	</g>\n"
	));
}


///////////////////////////////////////////////////////////
//														//
//														//
//														//
///////////////////////////////////////////////////////////

//---------------------------------------------------------
const wxChar * CSVG_Interactive_Map::_Get_Code_CheckBox(void)
{
	return( SG_STR_MBTOSG(
		"function checkBoxScript(evt,myLayer) { //checkBox for toggling layers an contextMenue\n"
		"	var myLayerObj = document.getElementById(myLayer);\n"
		"	var myCheckCrossObj = document.getElementById(\"checkCross\"+myLayer);\n"
		"	var myCheckCrossVisibility = myCheckCrossObj.getAttributeNS(null,\"visibility\"));\n"
		"	if (evt.type == \"click\" && evt.detail == 1) {\n"
		"	if (myCheckCrossVisibility == \"visible\")) {\n"
		"		myLayerObj.setAttributeNS(null,\"visibility\"),\"hidden\"));\n"
		"		myCheckCrossObj.setAttributeNS(null,\"visibility\"),\"hidden\"));\n"
		"		//you can do if/else or switch statements to set different actions on activating a checkbox here\n"
		"		//myLayer holds the currentLayer name\n"
		"	}\n"
		"	else {\n"
		"		myLayerObj.setAttributeNS(null,\"visibility\"),\"visible\"));\n"
		"		myCheckCrossObj.setAttributeNS(null,\"visibility\"),\"visible\"));\n"
		"	}\n"
		"	}\n"
		"}\n"
		"\n"
	));
}

//---------------------------------------------------------
const wxChar * CSVG_Interactive_Map::_Get_Code_MapApp(void)
{
	return( SG_STR_MBTOSG(
		"//holds data on window size\n"
		"function mapApp() {\n"
		"	if (!document.documentElement.getScreenCTM) {\n"
		"	this.resetFactors();\n"
		"	//add resize event to document element\n"
		"	document.documentElement.addEventListener(\"SVGResize\"),this,false);\n"
		"	}\n"
		"}\n"
		"\n"
		"mapApp.prototype.handleEvent = function(evt) {\n"
		"	if (evt.type == \"SVGResize\")) {\n"
		"	this.resetFactors();\n"
		"	}\n"
		"}\n"
		"\n"
		"mapApp.prototype.resetFactors = function() {\n"
		"	if (!document.documentElement.getScreenCTM) {\n"
		"	//case for viewers that don't support .getScreenCTM, such as ASV3\n"
		"	//calculate ratio and offset values of app window\n"
		"	var viewBoxArray = document.documentElement.getAttributeNS(null,\"viewBox\")).split(\" \"));\n"
		"	var myRatio = viewBoxArray[2]/viewBoxArray[3];\n"
		"	if ((window.innerWidth/window.innerHeight) > myRatio) { //case window is more wide than myRatio\n"
		"		this.scaleFactor = viewBoxArray[3] / window.innerHeight;\n"
		"	}\n"
		"	else { //case window is more tall than myRatio\n"
		"		this.scaleFactor = viewBoxArray[2] / window.innerWidth;\n"
		"	}\n"
		"	this.offsetX = (window.innerWidth - viewBoxArray[2] * 1 / this.scaleFactor) / 2;\n"
		"	this.offsetY = (window.innerHeight - viewBoxArray[3] * 1 / this.scaleFactor) / 2;\n"
		"	}\n"
		"}\n"
		"\n"
		"mapApp.prototype.calcCoord = function(evt,ctmNode) {\n"
		"	var svgPoint = document.documentElement.createSVGPoint();\n"
		"	svgPoint.x = evt.clientX;\n"
		"	svgPoint.y = evt.clientY;\n"
		"	if (!document.documentElement.getScreenCTM) {\n"
		"	//case ASV3 a. Corel\n"
		"	svgPoint.x = (svgPoint.x	- this.offsetX) * this.scaleFactor;\n"
		"	svgPoint.y = (svgPoint.y - this.offsetY) * this.scaleFactor;\n"
		"	//undo the effect of transformations\n"
		"	if (ctmNode) {\n"
		"		var ctm = getTransformToRootElement(ctmNode).inverse();\n"
		"	}\n"
		"	else {\n"
		"		var ctm = getTransformToRootElement(evt.target).inverse();\n"
		"	}\n"
		"	svgPoint = svgPoint.matrixTransform(ctm);\n"
		"\n"
		"	}\n"
		"	else {\n"
		"	if (ctmNode) {\n"
		"		var matrix = ctmNode.getScreenCTM();\n"
		"	}\n"
		"	else {\n"
		"		var matrix = evt.target.getScreenCTM();\n"
		"	}\n"
		"	svgPoint = svgPoint.matrixTransform(matrix.inverse());\n"
		"	}\n"
		"	return svgPoint;\n"
		"}\n"
		"\n"
	));
}

//---------------------------------------------------------
const wxChar * CSVG_Interactive_Map::_Get_Code_Timer(void)
{
	return( SG_STR_MBTOSG(
		"// source/credits: \"Algorithm\": http://www.codingforums.com/showthread.php?s=&threadid=10531\n"
		"// The constructor should be called with\n"
		"// the parent object (optional, defaults to window).\n"
		"\n"
		"function Timer(){\n"
		"	this.obj = (arguments.length)?arguments[0]:window;\n"
		"	return this;\n"
		"}\n"
		"\n"
		"// The set functions should be called with:\n"
		"// - The name of the object method (as a string) (required)\n"
		"// - The millisecond delay (required)\n"
		"// - Any number of extra arguments, which will all be\n"
		"//	passed to the method when it is evaluated.\n"
		"\n"
		"Timer.prototype.setInterval = function(func, msec){\n"
		"	var i = Timer.getNew();\n"
		"	var t = Timer.buildCall(this.obj, i, arguments);\n"
		"	Timer.set[i].timer = window.setInterval(t,msec);\n"
		"	return i;\n"
		"}\n"
		"Timer.prototype.setTimeout = function(func, msec){\n"
		"	var i = Timer.getNew();\n"
		"	Timer.buildCall(this.obj, i, arguments);\n"
		"	Timer.set[i].timer = window.setTimeout(\"Timer.callOnce(\"+i+\"));\"),msec);\n"
		"	return i;\n"
		"}\n"
		"\n"
		"// The clear functions should be called with\n"
		"// the return value from the equivalent set function.\n"
		"\n"
		"Timer.prototype.clearInterval = function(i){\n"
		"	if(!Timer.set[i]) return;\n"
		"	window.clearInterval(Timer.set[i].timer);\n"
		"	Timer.set[i] = null;\n"
		"}\n"
		"Timer.prototype.clearTimeout = function(i){\n"
		"	if(!Timer.set[i]) return;\n"
		"	window.clearTimeout(Timer.set[i].timer);\n"
		"	Timer.set[i] = null;\n"
		"}\n"
		"\n"
		"// Private data\n"
		"\n"
		"Timer.set = new Array();\n"
		"Timer.buildCall = function(obj, i, args){\n"
		"	var t = \"\");\n"
		"	Timer.set[i] = new Array();\n"
		"	if(obj != window){\n"
		"	Timer.set[i].obj = obj;\n"
		"	t = \"Timer.set[\"+i+\"].obj.\");\n"
		"	}\n"
		"	t += args[0]+\"(\");\n"
		"	if(args.length > 2){\n"
		"	Timer.set[i][0] = args[2];\n"
		"	t += \"Timer.set[\"+i+\"][0]\");\n"
		"	for(var j=1; (j+2)<args.length; j++){\n"
		"		Timer.set[i][j] = args[j+2];\n"
		"		t += \"), Timer.set[\"+i+\"][\"+j+\"]\");\n"
		"	}}\n"
		"	t += \"));\");\n"
		"	Timer.set[i].call = t;\n"
		"	return t;\n"
		"}\n"
		"Timer.callOnce = function(i){\n"
		"	if(!Timer.set[i]) return;\n"
		"	eval(Timer.set[i].call);\n"
		"	Timer.set[i] = null;\n"
		"}\n"
		"Timer.getNew = function(){\n"
		"	var i = 0;\n"
		"	while(Timer.set[i]) i++;\n"
		"	return i;\n"
		"}\n"
		"\n"
	));
}

//---------------------------------------------------------
const wxChar * CSVG_Interactive_Map::_Get_Code_Slider(void)
{
	return( SG_STR_MBTOSG(
		"//slider properties\n"
		"function slider(x1,y1,value1,x2,y2,value2,startVal,sliderGroupId,sliderColor,visSliderWidth,invisSliderWidth,sliderSymb,functionToCall,mouseMoveBool) {\n"
		"	this.x1 = x1;\n"
		"	this.y1 = y1;\n"
		"	this.value1 = value1;\n"
		"	this.x2 = x2;\n"
		"	this.y2 = y2;\n"
		"	this.value2 = value2;\n"
		"	this.startVal = startVal;\n"
		"	this.value = startVal;\n"
		"	this.sliderGroupId = sliderGroupId;\n"
		"	this.sliderGroup = document.getElementById(this.sliderGroupId);\n"
		"	this.sliderColor = sliderColor;\n"
		"	this.visSliderWidth = visSliderWidth;\n"
		"	this.invisSliderWidth = invisSliderWidth;\n"
		"	this.sliderSymb = sliderSymb;\n"
		"	this.functionToCall = functionToCall;\n"
		"	this.mouseMoveBool = mouseMoveBool;\n"
		"	this.length = toPolarDist((this.x2 - this.x1),(this.y2 - this.y1));\n"
		"	this.direction = toPolarDir((this.x2 - this.x1),(this.y2 - this.y1));\n"
		"	this.sliderLine = null;\n"
		"	this.createSlider();\n"
		"	this.slideStatus = 0;\n"
		"}\n"
		"\n"
		"//create slider\n"
		"slider.prototype.createSlider = function() {\n"
		"	this.sliderLine = document.createElementNS(svgNS,\"line\"));\n"
		"	this.sliderLine.setAttributeNS(null,\"x1\"),this.x1);\n"
		"	this.sliderLine.setAttributeNS(null,\"y1\"),this.y1);\n"
		"	this.sliderLine.setAttributeNS(null,\"x2\"),this.x2);\n"
		"	this.sliderLine.setAttributeNS(null,\"y2\"),this.y2);\n"
		"	this.sliderLine.setAttributeNS(null,\"stroke\"),this.sliderColor);\n"
		"	this.sliderLine.setAttributeNS(null,\"stroke-width\"),this.invisSliderWidth);\n"
		"	this.sliderLine.setAttributeNS(null,\"opacity\"),\"0\"));\n"
		"	this.sliderLine.setAttributeNS(null,\"stroke-linecap\"),\"square\"));\n"
		"	this.sliderLine.setAttributeNS(null,\"id\"),this.sliderGroupId+\"_invisibleSliderLine\"));\n"
		"	this.sliderLine.addEventListener(\"mousedown\"),this,false);\n"
		"	this.sliderGroup.appendChild(this.sliderLine);\n"
		"	var mySliderLine = document.createElementNS(svgNS,\"line\"));\n"
		"	mySliderLine.setAttributeNS(null,\"x1\"),this.x1);\n"
		"	mySliderLine.setAttributeNS(null,\"y1\"),this.y1);\n"
		"	mySliderLine.setAttributeNS(null,\"x2\"),this.x2);\n"
		"	mySliderLine.setAttributeNS(null,\"y2\"),this.y2);\n"
		"	mySliderLine.setAttributeNS(null,\"stroke\"),this.sliderColor);\n"
		"	mySliderLine.setAttributeNS(null,\"stroke-width\"),this.visSliderWidth);\n"
		"	mySliderLine.setAttributeNS(null,\"id\"),this.sliderGroupId+\"_visibleSliderLine\"));\n"
		"	mySliderLine.setAttributeNS(null,\"pointer-events\"),\"none\"));\n"
		"	this.sliderGroup.appendChild(mySliderLine);\n"
		"	mySliderSymb = document.createElementNS(svgNS,\"use\"));\n"
		"	mySliderSymb.setAttributeNS(xlinkNS,\"xlink:href\"),\"#\"+this.sliderSymb);\n"
		"	var myStartDistance = this.length - ((this.value2 - this.startVal) / (this.value2 - this.value1)) * this.length;\n"
		"	var myPosX = this.x1 + toRectX(this.direction,myStartDistance);\n"
		"	var myPosY = this.y1 + toRectY(this.direction,myStartDistance);\n"
		"	var myTransformString = \"translate(\"+myPosX+\"),\"+myPosY+\")) rotate(\" + Math.round(this.direction / Math.PI * 180) + \"))\");\n"
		"	mySliderSymb.setAttributeNS(null,\"transform\"),myTransformString);\n"
		"	mySliderSymb.setAttributeNS(null,\"id\"),this.sliderGroupId+\"_sliderSymbol\"));\n"
		"	this.sliderGroup.appendChild(mySliderSymb);\n"
		"}\n"
		"\n"
		"//remove all slider elements\n"
		"slider.prototype.removeSlider = function() {\n"
		"	var mySliderSymb = document.getElementById(this.sliderGroup+\"_sliderSymbol\"));\n"
		"	this.sliderGroup.removeChild(mySliderSymb);\n"
		"	var mySliderLine = document.getElementById(this.sliderGroup+\"_visibleSliderLine\"));\n"
		"	this.sliderGroup.removeChild(mySliderLine);\n"
		"	var mySliderLine = document.getElementById(this.sliderGroup+\"_invisibleSliderLine\"));\n"
		"	this.sliderGroup.removeChild(mySliderLine);\n"
		"}\n"
		"\n"
		"//handle events\n"
		"slider.prototype.handleEvent = function(evt) {\n"
		"	this.drag(evt);\n"
		"}\n"
		"\n"
		"//drag slider\n"
		"slider.prototype.drag = function(evt) {\n"
		"	if (evt.type == \"mousedown\" || (evt.type == \"mousemove\" && this.slideStatus == 1)) {\n"
		"	//get coordinate in slider coordinate system\n"
		"	var coordPoint = myMapApp.calcCoord(evt,this.sliderLine);\n"
		"	//draw normal line for first vertex\n"
		"	var ax = this.x2 - this.x1;\n"
		"	var ay = this.y2 - this.y1;\n"
		"	//normal vector 1\n"
		"	var px1 = parseFloat(this.x1) + ay * -1;\n"
		"	var py1 = parseFloat(this.y1) + ax;\n"
		"	//normal vector 2\n"
		"	var px2 = parseFloat(this.x2) + ay * -1;\n"
		"	var py2 = parseFloat(this.y2) + ax;\n"
		"		\n"
		"	if (leftOfTest(coordPoint.x,coordPoint.y,this.x1,this.y1,px1,py1) == 0 && leftOfTest(coordPoint.x,coordPoint.y,this.x2,this.y2,px2,py2) == 1) {\n"
		"		if (evt.type == \"mousedown\" && evt.detail == 1) {\n"
		"		this.slideStatus = 1;\n"
		"		document.documentElement.addEventListener(\"mousemove\"),this,false);\n"
		"		document.documentElement.addEventListener(\"mouseup\"),this,false);\n"
		"		}\n"
		"		myNewPos = intersect2lines(this.x1,this.y1,this.x2,this.y2,coordPoint.x,coordPoint.y,coordPoint.x + ay * -1,coordPoint.y + ax);\n"
		"		var myPercentage = toPolarDist(myNewPos['x'] - this.x1,myNewPos['y'] - this.y1) / this.length;\n"
		"		this.value = this.value1 + myPercentage * (this.value2 - this.value1);\n"
		"	}\n"
		"	else {\n"
		"		var myNewPos = new Array();\n"
		"		if (leftOfTest(coordPoint.x,coordPoint.y,this.x1,this.y1,px1,py1) == 0 && leftOfTest(coordPoint.x,coordPoint.y,this.x2,this.y2,px2,py2) == 0) {\n"
		"		//more than max\n"
		"		this.value = this.value2;\n"
		"		myNewPos['x'] = this.x2;\n"
		"		myNewPos['y'] = this.y2;\n"
		"		}\n"
		"		if (leftOfTest(coordPoint.x,coordPoint.y,this.x1,this.y1,px1,py1) == 1 && leftOfTest(coordPoint.x,coordPoint.y,this.x2,this.y2,px2,py2) == 1) {\n"
		"		//less than min\n"
		"		this.value = this.value1;\n"
		"		myNewPos['x'] = this.x1;\n"
		"		myNewPos['y'] = this.y1;\n"
		"		}\n"
		"	}\n"
		"	var myTransformString = \"translate(\"+myNewPos['x']+\"),\"+myNewPos['y']+\")) rotate(\" + Math.round(this.direction / Math.PI * 180) + \"))\");\n"
		"	document.getElementById(this.sliderGroupId+\"_sliderSymbol\")).setAttributeNS(null,\"transform\"),myTransformString);\n"
		"	this.getValue();\n"
		"	}\n"
		"	if (evt.type == \"mouseup\" && evt.detail == 1) {\n"
		"	if (this.slideStatus == 1) {\n"
		"		this.slideStatus = 2;\n"
		"		document.documentElement.removeEventListener(\"mousemove\"),this,false);\n"
		"		document.documentElement.removeEventListener(\"mouseup\"),this,false);\n"
		"		this.getValue();\n"
		"	}\n"
		"	this.slideStatus = 0;\n"
		"	}\n"
		"}\n"
		"\n"
		"//this code is executed, after the slider is released\n"
		"//you can use switch/if to detect which slider was used (use this.sliderGroup) for that\n"
		"slider.prototype.getValue = function() {\n"
		"	if (this.slideStatus == 1 && this.mouseMoveBool == true) {\n"
		"	if (typeof(this.functionToCall) == \"function\")) {\n"
		"		this.functionToCall(\"change\"),this.sliderGroupId,this.value);\n"
		"	}\n"
		"	if (typeof(this.functionToCall) == \"object\")) {\n"
		"		this.functionToCall.getSliderVal(\"change\"),this.sliderGroupId,this.value);\n"
		"	}\n"
		"	if (typeof(this.functionToCall) == \"string\")) {\n"
		"		eval(this.functionToCall+\"('change','\"+this.sliderGroupId+\"',\"+this.value+\"))\"));\n"
		"	}\n"
		"	}\n"
		"	if (this.slideStatus == 2) {\n"
		"	if (typeof(this.functionToCall) == \"function\")) {\n"
		"		this.functionToCall(\"release\"),this.sliderGroupId,this.value);\n"
		"	}\n"
		"	if (typeof(this.functionToCall) == \"object\")) {\n"
		"		this.functionToCall.getSliderVal(\"release\"),this.sliderGroupId,this.value);\n"
		"	}\n"
		"	if (typeof(this.functionToCall) == \"string\")) {\n"
		"		eval(this.functionToCall+\"('release','\"+this.sliderGroupId+\"',\"+this.value+\"))\"));\n"
		"	}\n"
		"	}\n"
		"}	\n"
		"\n"
		"//this is to set the value from other scripts\n"
		"slider.prototype.setValue = function(value) {\n"
		"	var myPercAlLine = (value - this.value1) / (this.value2 - this.value1);\n"
		"	this.value = myPercAlLine;\n"
		"	var myPosX = this.x1 + toRectX(this.direction,this.length * myPercAlLine);\n"
		"	var myPosY = this.y1 + toRectY(this.direction,this.length * myPercAlLine);\n"
		"	var myTransformString = \"translate(\"+myPosX+\"),\"+myPosY+\")) rotate(\" + Math.round(this.direction / Math.PI * 180) + \"))\");\n"
		"	document.getElementById(this.sliderGroupId+\"_sliderSymbol\")).setAttributeNS(null,\"transform\"),myTransformString);\n"
		"}\n"
		"\n"
	));
}

//---------------------------------------------------------
const wxChar * CSVG_Interactive_Map::_Get_Code_Helper(void)
{
	return( SG_STR_MBTOSG(
		"/*\n"
		"ECMAScript helper functions\n"
		"Copyright (C) <2004>	<Andreas Neumann>\n"
		"Version 1.1, 2004-11-18\n"
		"neumann@karto.baug.ethz.ch\n"
		"http://www.carto.net/\n"
		"http://www.carto.net/neumann/\n"
		"\n"
		"Credits: numerous people on svgdevelopers@yahoogroups.com\n"
		"\n"
		"This ECMA script library is free software; you can redistribute it and/or\n"
		"modify it under the terms of the GNU Lesser General Public\n"
		"License as published by the Free Software Foundation; either\n"
		"version 2.1 of the License, or (at your option) any later version.\n"
		"\n"
		"This library is distributed in the hope that it will be useful,\n"
		"but WITHOUT ANY WARRANTY; without even the implied warranty of\n"
		"MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.	See the GNU\n"
		"Lesser General Public License for more details.\n"
		"\n"
		"You should have received a copy of the GNU Lesser General Public\n"
		"License along with this library (http://www.carto.net/papers/svg/resources/lesser_gpl.txt); if not, write to the Free Software\n"
		"Foundation, Inc., 59 Temple Place, Suite 330, Boston, MA	02111-1307	USA\n"
		"\n"
		"----\n"
		"\n"
		"original document site: http://www.carto.net/papers/svg/resources/helper_functions.js\n"
		"Please contact the author in case you want to use code or ideas commercially.\n"
		"If you use this code, please include this copyright header, the included full\n"
		"LGPL 2.1 text and read the terms provided in the LGPL 2.1 license\n"
		"(http://www.gnu.org/copyleft/lesser.txt)\n"
		"\n"
		"-------------------------------\n"
		"\n"
		"Please report bugs and send improvements to neumann@karto.baug.ethz.ch\n"
		"If you use these scripts, please link to the original (http://www.carto.net/papers/svg/navigationTools/)\n"
		"somewhere in the source-code-comment or the \"about\" of your project and give credits, thanks!\n"
		"\n"
		"*/\n"
		"\n"
		"//global variables necessary to create elements in these namespaces, do not delete them!!!!\n"
		"var svgNS = \"http://www.w3.org/2000/svg\");\n"
		"var xlinkNS = \"http://www.w3.org/1999/xlink\");\n"
		"var cartoNS = \"http://www.carto.net/attrib\");\n"
		"var attribNS = \"http://www.carto.net/attrib\");\n"
		"var batikNS = \"http://xml.apache.org/batik/ext\");\n"
		"\n"
		"/* ----------------------- helper functions to calculate stuff ---------------- */\n"
		"/* ---------------------------------------------------------------------------- */\n"
		"function toPolarDir(xdiff,ydiff) { // Subroutine for calculating polar Coordinates\n"
		"	direction = (Math.atan2(ydiff,xdiff));\n"
		"	//result is angle in radian\n"
		"	return(direction);\n"
		"}\n"
		"\n"
		"function toPolarDist(xdiff,ydiff) { // Subroutine for calculating polar Coordinates\n"
		"	distance = Math.sqrt(xdiff * xdiff + ydiff * ydiff);\n"
		"	return(distance);\n"
		"}\n"
		"\n"
		"function toRectX(direction,distance) { // Subroutine for calculating cartesic coordinates\n"
		"	x = distance * Math.cos(direction);\n"
		"	y = distance * Math.sin(direction);\n"
		"	return(x);\n"
		"}\n"
		"\n"
		"function toRectY(direction,distance) { // Subroutine for calculating cartesic coordinates\n"
		"	x = distance * Math.cos(direction);\n"
		"	y = distance * Math.sin(direction);\n"
		"	return(y);\n"
		"}\n"
		"\n"
		"//Converts degrees to radians.\n"
		"function DegToRad(deg) {\n"
		"	return (deg / 180.0 * Math.PI);\n"
		"}\n"
		"\n"
		"//Converts radians to degrees.\n"
		"function RadToDeg(rad) {\n"
		"	return (rad / Math.PI * 180.0);\n"
		"}\n"
		"\n"
		"//converts decimal degrees to degree/minutes/seconds\n"
		"function dd2dms(dd) {\n"
		"	var minutes = (dd - Math.floor(dd)) * 60;\n"
		"	var seconds = (minutes - Math.floor(minutes)) * 60;\n"
		"	var minutes = Math.floor(minutes);\n"
		"	var degrees = Math.floor(dd);\n"
		"	return {deg:degrees,min:minutes,sec:seconds};\n"
		"}\n"
		"\n"
		"//converts degree/minutes/seconds to decimal degrees\n"
		"function dms2dd(deg,min,sec) {\n"
		"	return deg + (min / 60) + (sec / 3600);\n"
		"}\n"
		"\n"
		"//log functions that do not exist in Math object\n"
		"function log(x,b) {\n"
		"	if(b==null) b=Math.E;\n"
		"	return Math.log(x)/Math.log(b);\n"
		"}\n"
		"\n"
		"//gets 4 z-values (4 corners), a position, delta x and delty and a cellsize as input and returns interpolated z-value\n"
		"function intBilinear(za,zb,zc,zd,xpos,ypos,ax,ay,cellsize) { //bilinear interpolation function\n"
		"	e = (xpos - ax) / cellsize;\n"
		"	f = (ypos - ay) / cellsize;\n"
		"\n"
		"	//calculation of weights\n"
		"	wa = (1 - e) * (1 - f);\n"
		"	wb = e * (1 - f);\n"
		"	wc = e * f;\n"
		"	wd = f * (1 - e);\n"
		"\n"
		"	height_interpol = wa * zc + wb * zd + wc * za + wd * zb;\n"
		"\n"
		"	return (height_interpol);	\n"
		"}\n"
		"\n"
		"//test if point is left of or right of, result is 1 (leftof) or 0 (rightof)\n"
		"function leftOfTest(pointx,pointy,linex1,liney1,linex2,liney2) {\n"
		"	result = (liney1 - pointy) * (linex2 - linex1) - (linex1 - pointx) * (liney2 - liney1);\n"
		"	if (result < 0) {\n"
		"	leftof = 1; //case left of\n"
		"	}\n"
		"	else {\n"
		"	leftof = 0; //case left of	\n"
		"	}\n"
		"	return(leftof);\n"
		"}\n"
		"\n"
		"//input is point coordinate, and 2 line coordinates\n"
		"function distFromLine(xpoint,ypoint,linex1,liney1,linex2,liney2) {\n"
		"	dx = linex2 - linex1;\n"
		"	dy = liney2 - liney1;\n"
		"	distance = (dy * (xpoint - linex1) - dx * (ypoint - liney1)) / Math.sqrt(Math.pow(dx,2) + Math.pow(dy,2));\n"
		"	return(distance);\n"
		"}\n"
		"\n"
		"//converts radian value to degrees\n"
		"function radian2deg(radian) {\n"
		"	deg = radian / Math.PI * 180;\n"
		"	return(deg);\n"
		"}\n"
		"\n"
		"//input is two vectors (a1,a2 is vector a, b1,b2 is vector b), output is angle in radian\n"
		"//Formula: Acos from	Scalaproduct of the two vectors divided by ( norm (deutsch Betrag) vector 1 by norm vector 2\n"
		"//see http://www.mathe-online.at/mathint/vect2/i.html#Winkel\n"
		"function angleBetwTwoLines(a1,a2,b1,b2) {\n"
		"	angle = Math.acos((a1 * b1 + a2 * b2) / (Math.sqrt(Math.pow(a1,2) + Math.pow(a2,2)) * Math.sqrt(Math.pow(b1,2) + Math.pow(b2,2))));\n"
		"	return(angle);\n"
		"}\n"
		"\n"
		"//input is two vectors (a1,a2 is vector a, b1,b2 is vector b), output is new vector c2 returned as array\n"
		"//Formula: Vektor a divided by Norm Vector a (Betrag) plus Vektor b divided by Norm Vector b (Betrag)\n"
		"//see http://www.mathe-online.at/mathint/vect1/i.html#Winkelsymmetrale\n"
		"function calcBisectorVector(a1,a2,b1,b2) {\n"
		"	betraga = Math.sqrt(Math.pow(a1,2) + Math.pow(a2,2));\n"
		"	betragb = Math.sqrt(Math.pow(b1,2) + Math.pow(b2,2));\n"
		"	c = new Array();\n"
		"	c[0] = a1 / betraga + b1 / betragb;\n"
		"	c[1] = a2 / betraga + b2 / betragb;\n"
		"	return(c);\n"
		"}\n"
		"\n"
		"//input is two vectors (a1,a2 is vector a, b1,b2 is vector b), output is angle in radian\n"
		"//Formula: Vektor a divided by Norm Vector a (Betrag) plus Vektor b divided by Norm Vector b (Betrag)\n"
		"//see http://www.mathe-online.at/mathint/vect1/i.html#Winkelsymmetrale\n"
		"function calcBisectorAngle(a1,a2,b1,b2) {\n"
		"	betraga = Math.sqrt(Math.pow(a1,2) + Math.pow(a2,2));\n"
		"	betragb = Math.sqrt(Math.pow(b1,2) + Math.pow(b2,2));\n"
		"	c1 = a1 / betraga + b1 / betragb;\n"
		"	c2 = a2 / betraga + b2 / betragb;\n"
		"	angle = toPolarDir(c1,c2);\n"
		"	return(angle);\n"
		"}\n"
		"\n"
		"function intersect2lines(line1x1,line1y1,line1x2,line1y2,line2x1,line2y1,line2x2,line2y2) {\n"
		"	//formula see http://astronomy.swin.edu.au/~pbourke/geometry/lineline2d/\n"
		"	var result = new Array();\n"
		"	var denominator = (line2y2 - line2y1)*(line1x2 - line1x1) - (line2x2 - line2x1)*(line1y2 - line1y1);\n"
		"	if (denominator == 0) {\n"
		"	alert(\"lines are parallel\"));\n"
		"	}\n"
		"	else {\n"
		"	ua = ((line2x2 - line2x1)*(line1y1 - line2y1) - (line2y2 - line2y1)*(line1x1 - line2x1)) / denominator;\n"
		"	ub = ((line1x2 - line1x1)*(line1y1 - line2y1) - (line1y2 - line1y1)*(line1x1 - line2x1)) / denominator;\n"
		"	}\n"
		"	result[\"x\"] = line1x1 + ua * (line1x2 - line1x1);\n"
		"	result[\"y\"] = line1y1 + ua * (line1y2 - line1y1);\n"
		"	return(result);\n"
		"}\n"
		"\n"
		"/* ----------------------- helper function to sort arrays ---------------- */\n"
		"/* ----------------------------------------------------------------------- */\n"
		"//my own sort function, uses only first part of string (population value)\n"
		"function mySort(a,b) {\n"
		"	var myResulta = a.split(\"+\"));\n"
		"	var myResultb = b.split(\"+\"));\n"
		"	if (parseFloat(myResulta[0]) < parseFloat(myResultb[0])) {\n"
		"	return 1;\n"
		"	}\n"
		"	else {\n"
		"	return -1;\n"
		"	}\n"
		"}\n"
		"\n"
		"/* ----------------------- helper function format number strings -------------- */\n"
		"/* ---------------------------------------------------------------------------- */\n"
		"//formatting number strings\n"
		"//this function add's \"'\" to a number every third digit\n"
		"function formatNumberString(myString) {\n"
		"	//check if of type string, if number, convert it to string\n"
		"	if (typeof(myString) == \"number\")) {\n"
		"	myTempString = myString.toString();\n"
		"	}\n"
		"	else {\n"
		"	myTempString = myString;\n"
		"	}\n"
		"	var myNewString=\"\");\n"
		"	//if it contains a comma, it will be split\n"
		"	var splitResults = myTempString.split(\".\"));\n"
		"	var myCounter= splitResults[0].length;\n"
		"	if (myCounter > 3) {\n"
		"	while(myCounter > 0) {\n"
		"		if (myCounter > 3) {\n"
		"		myNewString = \"),\" + splitResults[0].substr(myCounter - 3,3) + myNewString;\n"
		"		}\n"
		"		else {\n"
		"		myNewString = splitResults[0].substr(0,myCounter) + myNewString;\n"
		"		}\n"
		"		myCounter -= 3;\n"
		"	}\n"
		"	}\n"
		"	else {\n"
		"	myNewString = splitResults[0];\n"
		"	}\n"
		"	//concatenate if it contains a comma\n"
		"	if (splitResults[1]) {\n"
		"	myNewString = myNewString + \".\" + splitResults[1];\n"
		"	}\n"
		"	return myNewString;\n"
		"}\n"
		"\n"
		"//function for status Bar\n"
		"function statusChange(statusText) {\n"
		"	document.getElementById(\"statusText\")).firstChild.nodeValue = \"Statusbar: \" + statusText;\n"
		"}\n"
		"\n"
		"//scale an object\n"
		"function scaleObject(evt,factor) {\n"
		"//reference to the currently selected object\n"
		"	var element = evt.currentTarget;\n"
		"	var myX = element.getAttributeNS(null,\"x\"));\n"
		"	var myY = element.getAttributeNS(null,\"y\"));\n"
		"	var newtransform = \"scale(\" + factor + \")) translate(\" + (myX * 1 / factor - myX) + \" \" + (myY * 1 / factor - myY) +\"))\");\n"
		"	element.setAttributeNS(null,'transform', newtransform);\n"
		"}\n"
		"\n"
		"//this code is copied from Kevin Lindsey\n"
		"//http://www.kevlindev.com/tutorials/basics/transformations/toUserSpace/index.htm\n"
		"function getTransformToRootElement(node) {\n"
		"	try {\n"
		"	//this part is for fully conformant players\n"
		"	var CTM = node.getTransformToElement(document.documentElement);\n"
		"	}\n"
		"	catch (ex) {\n"
		"	//this part is for ASV3 or other non-conformant players\n"
		"	// Initialize our CTM the node's Current Transformation Matrix\n"
		"	var CTM = node.getCTM();\n"
		"	// Work our way through the ancestor nodes stopping at the SVG Document\n"
		"	while ( ( node = node.parentNode ) != document ) {\n"
		"		// Multiply the new CTM to the one with what we have accumulated so far\n"
		"		CTM = node.getCTM().multiply(CTM);\n"
		"	}\n"
		"	}\n"
		"	return CTM;\n"
		"}\n"
		"\n"
		"//calculate HSV 2 RGB: HSV (h 0 to 360, sat and val are between 0 and 1), RGB between 0 and 255\n"
		"function hsv2rgb(hue,sat,val) {\n"
		"	//alert(\"Hue:\"+hue);\n"
		"	var rgbArr = new Array();\n"
		"	if ( sat == 0) {\n"
		"	rgbArr[\"red\"] = Math.round(val * 255);\n"
		"	rgbArr[\"green\"] = Math.round(val * 255);\n"
		"	rgbArr[\"blue\"] = Math.round(val * 255);\n"
		"	}\n"
		"	else {\n"
		"	var h = hue / 60;\n"
		"	var i = Math.floor(h);\n"
		"	var f = h - i;\n"
		"	if (i % 2 == 0) {\n"
		"		f = 1 - f;\n"
		"	}\n"
		"	var m = val * (1 - sat); \n"
		"	var n = val * (1 - sat * f);\n"
		"	switch(i) {\n"
		"		case 0:\n"
		"		rgbArr[\"red\"] = val;\n"
		"		rgbArr[\"green\"] = n;\n"
		"		rgbArr[\"blue\"] = m;\n"
		"		break;\n"
		"		case 1:\n"
		"		rgbArr[\"red\"] = n;\n"
		"		rgbArr[\"green\"] = val;\n"
		"		rgbArr[\"blue\"] = m;\n"
		"		break;\n"
		"		case 2:\n"
		"		rgbArr[\"red\"] = m;\n"
		"		rgbArr[\"green\"] = val;\n"
		"		rgbArr[\"blue\"] = n;\n"
		"		break;\n"
		"		case 3:\n"
		"		rgbArr[\"red\"] = m;\n"
		"		rgbArr[\"green\"] = n;\n"
		"		rgbArr[\"blue\"] = val;\n"
		"		break;\n"
		"		case 4:\n"
		"		rgbArr[\"red\"] = n;\n"
		"		rgbArr[\"green\"] = m;\n"
		"		rgbArr[\"blue\"] = val;\n"
		"		break;\n"
		"		case 5:\n"
		"		rgbArr[\"red\"] = val;\n"
		"		rgbArr[\"green\"] = m;\n"
		"		rgbArr[\"blue\"] = n;\n"
		"		break;\n"
		"		case 6:\n"
		"		rgbArr[\"red\"] = val;\n"
		"		rgbArr[\"green\"] = n;\n"
		"		rgbArr[\"blue\"] = m;\n"
		"		break;\n"
		"	}\n"
		"	rgbArr[\"red\"] = Math.round(rgbArr[\"red\"] * 255);\n"
		"	rgbArr[\"green\"] = Math.round(rgbArr[\"green\"] * 255);\n"
		"	rgbArr[\"blue\"] = Math.round(rgbArr[\"blue\"] * 255);\n"
		"	}\n"
		"	return rgbArr;\n"
		"}\n"
		"\n"
		"//calculate rgb to hsv values\n"
		"function rgb2hsv (red,green,blue) {\n"
		"	//input between 0 and 255 --> normalize to 0 to 1\n"
		"	//result = \n"
		"	var hsvArr = new Array();\n"
		"	red = red / 255;\n"
		"	green = green / 255;\n"
		"	blue = blue / 255;\n"
		"	myMax = Math.max(red, Math.max(green,blue));\n"
		"	myMin = Math.min(red, Math.min(green,blue));\n"
		"	v = myMax;\n"
		"	if (myMax > 0) {\n"
		"	s = (myMax - myMin) / myMax;\n"
		"	}\n"
		"	else {\n"
		"	s = 0;\n"
		"	}\n"
		"	if (s > 0) {\n"
		"	myDiff = myMax - myMin;\n"
		"	rc = (myMax - red) / myDiff;\n"
		"	gc = (myMax - green) / myDiff;\n"
		"	bc = (myMax - blue) / myDiff;\n"
		"	if (red == myMax) {\n"
		"		h = (bc - gc) / 6;\n"
		"	}\n"
		"	if (green == myMax) {\n"
		"		h = (2 + rc - bc) / 6;\n"
		"	}\n"
		"	if (blue == myMax) {\n"
		"		h = (4 + gc - rc) / 6;\n"
		"	}\n"
		"	}\n"
		"	else {\n"
		"	h = 0;\n"
		"	}\n"
		"	if (h < 0) {\n"
		"	h += 1;\n"
		"	}\n"
		"	hsvArr[\"hue\"] = Math.round(h * 360);\n"
		"	hsvArr[\"sat\"] = s;\n"
		"	hsvArr[\"val\"] = v;\n"
		"	return hsvArr;\n"
		"}\n"
		"\n"
		"//populate an array that can be addressed by both a key or an index nr\n"
		"function assArrayPopulate(arrayKeys,arrayValues) {\n"
		"	var returnArray = new Array();\n"
		"	if (arrayKeys.length != arrayValues.length) {\n"
		"	alert(\"error: arrays do not have same length!\"));\n"
		"	}\n"
		"	else {\n"
		"	for (i=0;i<arrayKeys.length;i++) {\n"
		"		returnArray[arrayKeys[i]] = arrayValues[i];\n"
		"	}\n"
		"	}\n"
		"	return returnArray;\n"
		"}\n"
		"\n"
		"//replace special (non-ASCII) characters with their charCode\n"
		"function replaceSpecialChars(myString) {\n"
		"	for (i=161;i<256;i++) {\n"
		"		re = new RegExp(\"&#\"+i+\");\"),\"g\"));\n"
		"		myString = myString.replace(re,String.fromCharCode(i));\n"
		"	}\n"
		"	return myString;\n"
		"}\n"
		"\n"
		"/* ----------------------- getXMLData object ----------------------------- */\n"
		"/* ----------------------------------------------------------------------- */\n"
		"//this object allows to make network requests using getURL or XMLHttpRequest\n"
		"//you may specify a url and a callBackFunction\n"
		"//the callBackFunction receives a XML node representing the rootElement of the fragment received\n"
		"function getXMLData(url,callBackFunction) {\n"
		"	this.url = url;\n"
		"	this.callBackFunction = callBackFunction;\n"
		"	this.xmlRequest = null;\n"
		"} \n"
		"\n"
		"getXMLData.prototype.getData = function() {\n"
		"	//call getURL() if available\n"
		"	if (window.getURL) {\n"
		"	getURL(this.url,this);\n"
		"	}\n"
		"	//or call XMLHttpRequest() if available\n"
		"	else if (window.XMLHttpRequest) {\n"
		"	this.xmlRequest = new XMLHttpRequest();\n"
		"	this.xmlRequest.overrideMimeType(\"text/xml\"));\n"
		"	this.xmlRequest.open(\"GET\"),this.url,true);\n"
		"	this.xmlRequest.onreadystatechange = this;\n"
		"	this.xmlRequest.send(null);\n"
		"	}\n"
		"	//write an error message if neither method is available\n"
		"	else {\n"
		"	alert(\"your browser/svg viewer neither supports window.getURL nor window.XMLHttpRequest!\"));\n"
		"	}	\n"
		"}\n"
		"\n"
		"//this is the callback method for the getURL function\n"
		"getXMLData.prototype.operationComplete = function(data) {\n"
		"	//check if data has a success property\n"
		"	if (data.success) {\n"
		"	//parse content of the XML format to the variable \"node\"\n"
		"	var node = parseXML(data.content,document);\n"
		"	this.callBackFunction(node.firstChild);\n"
		"	}\n"
		"	else {\n"
		"	alert(\"something went wrong with dynamic loading of geometry!\"));\n"
		"	}\n"
		"}\n"
		"\n"
		"//this method receives data from XMLHttpRequest\n"
		"getXMLData.prototype.handleEvent = function() {\n"
		"	if (this.xmlRequest.readyState == 4) {\n"
		"	this.callBackFunction(this.xmlRequest.responseXML.documentElement);\n"
		"	}	\n"
		"}\n"
		"\n"
	));
}

//---------------------------------------------------------
const wxChar * CSVG_Interactive_Map::_Get_Code_Buttons(void)
{
	return( SG_STR_MBTOSG(
		"function button(groupId,functionToCall,buttonType,buttonText,buttonSymbolId,x,y,width,height,fontSize,fontFamily,textFill,buttonFill,shadeLightFill,shadeDarkFill,shadowOffset) {\n"
		"	if (arguments.length > 0) {\n"
		"	this.init(groupId,functionToCall,buttonType,buttonText,buttonSymbolId,x,y,width,height,fontSize,fontFamily,textFill,buttonFill,shadeLightFill,shadeDarkFill,shadowOffset);\n"
		"	}\n"
		"}\n"
		"\n"
		"button.prototype.init = function(groupId,functionToCall,buttonType,buttonText,buttonSymbolId,x,y,width,height,fontSize,fontFamily,textFill,buttonFill,shadeLightFill,shadeDarkFill,shadowOffset) {\n"
		"	this.groupId = groupId; //the group id where all new content is appended to\n"
		"	this.functionToCall = functionToCall; //function to be called if button was pressed\n"
		"	this.buttonType = buttonType; //button type: currently either \"rect\" or \"ellipse\"\n"
		"	this.buttonText = buttonText; //default value to be filled in when textbox is created\n"
		"	this.buttonSymbolId = buttonSymbolId; //id to a symbol to be used as a button graphics\n"
		"	this.x = x; //left of button rectangle\n"
		"	this.y = y; //top of button rectangle\n"
		"	this.width = width; //button rectangle width\n"
		"	this.height = height; //button rectangle height\n"
		"	this.fontSize = fontSize; //font size in user units/pixels\n"
		"	this.fontFamily = fontFamily; //font family for text element, can be an empty string for image buttons\n"
		"	this.textFill = textFill; //the fill color of the text for text buttons, can be an empty string for image buttons\n"
		"	this.buttonFill = buttonFill; //the fill color of the button rectangle or ellipse\n"
		"	this.shadeLightFill = shadeLightFill; //light fill color simulating 3d effect\n"
		"	this.shadeDarkFill = shadeDarkFill; //dark fill color simulating 3d effect\n"
		"	this.shadowOffset = shadowOffset; //shadow offset in viewBox units\n"
		"	this.buttonGroup = document.getElementById(this.groupId); //reference to the button group node\n"
		"	this.upperLeftLine = null; //later a reference to the upper left line simulating 3d effect\n"
		"	this.buttonRect = null; //later a reference to the button area (rect)\n"
		"	this.buttonTextElement = null; //later a reference to the button text\n"
		"	this.buttonSymbolInstance = null; //later a reference to the button symbol\n"
		"	this.deActivateRect = null; //later a reference to a rectangle that can be used to deactivate the button\n"
		"	this.activated = true; //a property indicating if button is activated or not\n"
		"	this.lowerRightLine = null; //later a reference to the lower right line simulating 3d effect\n"
		"	this.createButton(); //method to initialize button\n"
		"	this.timer = new Timer(this); //a Timer instance for calling the functionToCall\n"
		"	this.timerMs = 200; //a constant of this object that is used in conjunction with the timer - functionToCall is called after 200 ms\n"
		"}\n"
		"\n"
		"//create button\n"
		"button.prototype.createButton = function() {\n"
		"	//create upper left button line or ellipse\n"
		"	if (this.buttonType == \"rect\")) {\n"
		"	this.upperLeftShadow = document.createElementNS(svgNS,\"rect\"));\n"
		"	this.upperLeftShadow.setAttributeNS(null,\"x\"),this.x - this.shadowOffset);\n"
		"	this.upperLeftShadow.setAttributeNS(null,\"y\"),this.y - this.shadowOffset);\n"
		"	this.upperLeftShadow.setAttributeNS(null,\"width\"),this.width);\n"
		"	this.upperLeftShadow.setAttributeNS(null,\"height\"),this.height);\n"
		"	this.upperLeftShadow.setAttributeNS(null,\"points\"),this.x+\"),\"+(this.y+this.height)+\" \"+this.x+\"),\"+this.y+\" \"+(this.x+this.width)+\"),\"+this.y);\n"
		"	}\n"
		"	else if (this.buttonType == \"ellipse\")) {\n"
		"	this.upperLeftShadow = document.createElementNS(svgNS,\"ellipse\"));\n"
		"	this.upperLeftShadow.setAttributeNS(null,\"cx\"),this.x + this.width * 0.5 - this.shadowOffset);\n"
		"	this.upperLeftShadow.setAttributeNS(null,\"cy\"),this.y + this.height * 0.5 - this.shadowOffset);\n"
		"	this.upperLeftShadow.setAttributeNS(null,\"rx\"),this.width * 0.5);\n"
		"	this.upperLeftShadow.setAttributeNS(null,\"ry\"),this.height * 0.5);\n"
		"	}\n"
		"	else {\n"
		"	alert(\"buttonType '\"+this.buttonType+\"' not supported. You need to specify 'rect' or 'ellipse'\"));\n"
		"	}\n"
		"	this.upperLeftShadow.setAttributeNS(null,\"fill\"),this.shadeLightFill);\n"
		"	this.buttonGroup.appendChild(this.upperLeftShadow);\n"
		"\n"
		"	//create lower right button line or ellipse\n"
		"	if (this.buttonType == \"rect\")) {\n"
		"	this.lowerRightShadow = document.createElementNS(svgNS,\"rect\"));\n"
		"	this.lowerRightShadow.setAttributeNS(null,\"x\"),this.x + this.shadowOffset);\n"
		"	this.lowerRightShadow.setAttributeNS(null,\"y\"),this.y + this.shadowOffset);\n"
		"	this.lowerRightShadow.setAttributeNS(null,\"width\"),this.width);\n"
		"	this.lowerRightShadow.setAttributeNS(null,\"height\"),this.height);\n"
		"	this.lowerRightShadow.setAttributeNS(null,\"points\"),this.x+\"),\"+(this.y+this.height)+\" \"+this.x+\"),\"+this.y+\" \"+(this.x+this.width)+\"),\"+this.y);\n"
		"	}\n"
		"	else if (this.buttonType == \"ellipse\")) {\n"
		"	this.lowerRightShadow = document.createElementNS(svgNS,\"ellipse\"));\n"
		"	this.lowerRightShadow.setAttributeNS(null,\"cx\"),this.x + this.width * 0.5 + this.shadowOffset);\n"
		"	this.lowerRightShadow.setAttributeNS(null,\"cy\"),this.y + this.height * 0.5 + this.shadowOffset);\n"
		"	this.lowerRightShadow.setAttributeNS(null,\"rx\"),this.width * 0.5);\n"
		"	this.lowerRightShadow.setAttributeNS(null,\"ry\"),this.height * 0.5);\n"
		"	}\n"
		"	this.lowerRightShadow.setAttributeNS(null,\"fill\"),this.shadeDarkFill);\n"
		"	this.buttonGroup.appendChild(this.lowerRightShadow);\n"
		"\n"
		"	//create buttonRect\n"
		"	if (this.buttonType == \"rect\")) {\n"
		"	this.buttonRect = document.createElementNS(svgNS,\"rect\"));\n"
		"	this.buttonRect.setAttributeNS(null,\"x\"),this.x);\n"
		"	this.buttonRect.setAttributeNS(null,\"y\"),this.y);\n"
		"	this.buttonRect.setAttributeNS(null,\"width\"),this.width);\n"
		"	this.buttonRect.setAttributeNS(null,\"height\"),this.height);\n"
		"	}\n"
		"	else if (this.buttonType == \"ellipse\")) {\n"
		"	this.buttonRect = document.createElementNS(svgNS,\"ellipse\"));\n"
		"	this.buttonRect.setAttributeNS(null,\"cx\"),this.x + this.width * 0.5);\n"
		"	this.buttonRect.setAttributeNS(null,\"cy\"),this.y + this.height * 0.5);\n"
		"	this.buttonRect.setAttributeNS(null,\"rx\"),this.width * 0.5);\n"
		"	this.buttonRect.setAttributeNS(null,\"ry\"),this.height * 0.5);\n"
		"	}\n"
		"	this.buttonRect.setAttributeNS(null,\"fill\"),this.buttonFill);\n"
		"	this.buttonRect.setAttributeNS(null,\"cursor\"),\"pointer\"));\n"
		"	this.buttonRect.addEventListener(\"mousedown\"),this,false);\n"
		"	this.buttonRect.addEventListener(\"mouseup\"),this,false);\n"
		"	this.buttonRect.addEventListener(\"click\"),this,false);\n"
		"	this.buttonGroup.appendChild(this.buttonRect);\n"
		"	\n"
		"	if (this.buttonText != undefined) {\n"
		"	//create text element and add clipping path attribute\n"
		"	this.buttonTextElement = document.createElementNS(svgNS,\"text\"));\n"
		"	this.buttonTextElement.setAttributeNS(null,\"x\"),(this.x + this.width / 2));\n"
		"	this.buttonTextElement.setAttributeNS(null,\"y\"),(this.y + this.height - ((this.height - this.fontSize) / 1.75)));\n"
		"	this.buttonTextElement.setAttributeNS(null,\"fill\"),this.textFill);\n"
		"	this.buttonTextElement.setAttributeNS(null,\"font-family\"),this.fontFamily);\n"
		"	this.buttonTextElement.setAttributeNS(null,\"font-size\"),this.fontSize+\"px\"));\n"
		"	this.buttonTextElement.setAttributeNS(null,\"pointer-events\"),\"none\"));\n"
		"	this.buttonTextElement.setAttributeNS(null,\"text-anchor\"),\"middle\"));\n"
		"	this.buttonTextElement.setAttributeNS(\"http://www.w3.org/XML/1998/namespace\"),\"space\"),\"preserve\"));\n"
		"	var textboxTextContent = document.createTextNode(this.buttonText);\n"
		"	this.buttonTextElement.appendChild(textboxTextContent);\n"
		"	this.buttonGroup.appendChild(this.buttonTextElement);\n"
		"	}\n"
		"	\n"
		"	if (this.buttonSymbolId != undefined) {\n"
		"	this.buttonSymbolInstance = document.createElementNS(svgNS,\"use\"));\n"
		"	this.buttonSymbolInstance.setAttributeNS(null,\"x\"),(this.x + this.width / 2));\n"
		"	this.buttonSymbolInstance.setAttributeNS(null,\"y\"),(this.y + this.height / 2));\n"
		"	this.buttonSymbolInstance.setAttributeNS(xlinkNS,\"href\"),\"#\"+this.buttonSymbolId);\n"
		"	this.buttonSymbolInstance.setAttributeNS(null,\"pointer-events\"),\"none\"));\n"
		"	this.buttonGroup.appendChild(this.buttonSymbolInstance);\n"
		"	}\n"
		"	\n"
		"	//create rectangle to deactivate the button\n"
		"	if (this.buttonType == \"rect\")) {\n"
		"	this.deActivateRect = document.createElementNS(svgNS,\"rect\"));\n"
		"	this.deActivateRect.setAttributeNS(null,\"x\"),this.x - this.shadowOffset);\n"
		"	this.deActivateRect.setAttributeNS(null,\"y\"),this.y - this.shadowOffset);\n"
		"	this.deActivateRect.setAttributeNS(null,\"width\"),this.width + this.shadowOffset * 2);\n"
		"	this.deActivateRect.setAttributeNS(null,\"height\"),this.height + this.shadowOffset * 2);\n"
		"	}\n"
		"	else if (this.buttonType == \"ellipse\")) {\n"
		"	this.deActivateRect = document.createElementNS(svgNS,\"ellipse\"));\n"
		"	this.deActivateRect.setAttributeNS(null,\"cx\"),this.x + this.width * 0.5);\n"
		"	this.deActivateRect.setAttributeNS(null,\"cy\"),this.y + this.height * 0.5);\n"
		"	this.deActivateRect.setAttributeNS(null,\"rx\"),this.width * 0.5 + this.shadowOffset);\n"
		"	this.deActivateRect.setAttributeNS(null,\"ry\"),this.height * 0.5 + this.shadowOffset);\n"
		"	}\n"
		"	\n"
		"	this.deActivateRect.setAttributeNS(null,\"fill\"),\"white\"));\n"
		"	this.deActivateRect.setAttributeNS(null,\"fill-opacity\"),\"0.5\"));\n"
		"	this.deActivateRect.setAttributeNS(null,\"stroke\"),\"none\"));\n"
		"	this.deActivateRect.setAttributeNS(null,\"display\"),\"none\"));\n"
		"	this.deActivateRect.setAttributeNS(null,\"cursor\"),\"default\"));\n"
		"	this.buttonGroup.appendChild(this.deActivateRect);\n"
		"}\n"
		"\n"
		"//remove all textbox elements\n"
		"button.prototype.removeButton = function() {\n"
		"	this.buttonGroup.removeChild(this.upperLeftShadow);\n"
		"	this.buttonGroup.removeChild(this.lowerRightShadow);\n"
		"	this.buttonGroup.removeChild(this.buttonRect);\n"
		"	if (this.buttonTextElement) {\n"
		"	this.buttonGroup.removeChild(this.buttonTextElement);	\n"
		"	}\n"
		"	if (this.buttonSymbolInstance) {\n"
		"	this.buttonGroup.removeChild(this.buttonSymbolInstance);	\n"
		"	}\n"
		"	this.buttonGroup.removeChild(this.deActivateRect);\n"
		"}\n"
		"\n"
		"//click on and write in textbox\n"
		"button.prototype.handleEvent = function(evt) {\n"
		"	if (evt.type == \"mousedown\")) {\n"
		"	this.togglePressed(\"pressed\"));\n"
		"	document.documentElement.addEventListener(\"mouseup\"),this,false);	\n"
		"	}\n"
		"	if (evt.type == \"mouseup\")) {\n"
		"	this.togglePressed(\"released\"));\n"
		"	document.documentElement.removeEventListener(\"mouseup\"),this,false);	\n"
		"	}\n"
		"	if (evt.type == \"click\")) {\n"
		"	//for some strange reasons I could not forward the evt object here ;-(, the code below using a literal is a workaround\n"
		"	//attention: only some of the evt properties are forwarded here, you can add more, if you need them\n"
		"	var timerEvt = {x:evt.clientX,y:evt.clientY,type:evt.type,detail:evt.detail,timeStamp:evt.timeStamp}\n"
		"	this.timer.setTimeout(\"fireFunction\"),this.timerMs,timerEvt)\n"
		"	}\n"
		"}\n"
		"\n"
		"button.prototype.togglePressed = function(type) {\n"
		"	if (type == \"pressed\")) {\n"
		"	this.upperLeftShadow.setAttributeNS(null,\"fill\"),this.shadeDarkFill);	\n"
		"	this.lowerRightShadow.setAttributeNS(null,\"fill\"),this.shadeLightFill);\n"
		"	}\n"
		"	if (type == \"released\")) {\n"
		"	this.upperLeftShadow.setAttributeNS(null,\"fill\"),this.shadeLightFill);	\n"
		"	this.lowerRightShadow.setAttributeNS(null,\"fill\"),this.shadeDarkFill);	\n"
		"	}\n"
		"}\n"
		"\n"
		"button.prototype.fireFunction = function(evt) {\n"
		"	if (typeof(this.functionToCall) == \"function\")) {\n"
		"	if (this.buttonTextElement) {\n"
		"		this.functionToCall(this.groupId,evt,this.buttonText);\n"
		"	}\n"
		"	if (this.buttonSymbolInstance) {\n"
		"		this.functionToCall(this.groupId,evt);\n"
		"	}\n"
		"	}\n"
		"	if (typeof(this.functionToCall) == \"object\")) {\n"
		"	if (this.buttonTextElement) {\n"
		"		this.functionToCall.buttonPressed(this.groupId,evt,this.buttonText);\n"
		"	}\n"
		"	if (this.buttonSymbolInstance) {\n"
		"		this.functionToCall.buttonPressed(this.groupId,evt);\n"
		"	}\n"
		"	}\n"
		"	if (typeof(this.functionToCall) == undefined) {\n"
		"	return;	\n"
		"	}\n"
		"}\n"
		"\n"
		"button.prototype.getTextValue = function() {\n"
		"	return this.buttonText;\n"
		"}	\n"
		"\n"
		"button.prototype.setTextValue = function(value) {\n"
		"	this.buttonText = value;\n"
		"	this.buttonTextElement.firstChild.nodeValue=this.buttonText;\n"
		"}\n"
		"\n"
		"button.prototype.activate = function(value) {\n"
		"	this.deActivateRect.setAttributeNS(null,\"display\"),\"none\"));\n"
		"	this.activated = true;\n"
		"}\n"
		"\n"
		"button.prototype.deactivate = function(value) {\n"
		"	this.deActivateRect.setAttributeNS(null,\"display\"),\"inherit\"));\n"
		"	this.activated = false;\n"
		"}\n"
		"\n"
		"//switchbutton\n"
		"//initialize inheritance\n"
		"switchbutton.prototype = new button();\n"
		"switchbutton.prototype.constructor = switchbutton;\n"
		"switchbutton.superclass = button.prototype;\n"
		"\n"
		"function switchbutton(groupId,functionToCall,buttonType,buttonText,buttonSymbolId,x,y,width,height,fontSize,fontFamily,textFill,buttonFill,shadeLightFill,shadeDarkFill,shadowOffset) {\n"
		"	if (arguments.length > 0) {\n"
		"	this.init(groupId,functionToCall,buttonType,buttonText,buttonSymbolId,x,y,width,height,fontSize,fontFamily,textFill,buttonFill,shadeLightFill,shadeDarkFill,shadowOffset);\n"
		"	}\n"
		"}\n"
		"\n"
		"switchbutton.prototype.init = function(groupId,functionToCall,buttonType,buttonText,buttonSymbolId,x,y,width,height,fontSize,fontFamily,textFill,buttonFill,shadeLightFill,shadeDarkFill,shadowOffset) {\n"
		"	switchbutton.superclass.init.call(this,groupId,functionToCall,buttonType,buttonText,buttonSymbolId,x,y,width,height,fontSize,fontFamily,textFill,buttonFill,shadeLightFill,shadeDarkFill,shadowOffset);\n"
		"	this.on = false;\n"
		"}\n"
		"\n"
		"//overwriting handleEventcode\n"
		"switchbutton.prototype.handleEvent = function(evt) {\n"
		"	//for some strange reasons I could not forward the evt object here ;-(, the code below using a literal is a workaround\n"
		"	//attention: only some of the evt properties are forwarded here, you can add more, if you need them\n"
		"	var timerEvt = {x:evt.clientX,y:evt.clientY,type:evt.type,detail:evt.detail,timeStamp:evt.timeStamp}\n"
		"	if (evt.type == \"click\")) {\n"
		"	if (this.on) {\n"
		"		this.on = false;\n"
		"		this.togglePressed(\"released\"));\n"
		"		this.timer.setTimeout(\"fireFunction\"),this.timerMs,timerEvt);\n"
		"	}\n"
		"	else {\n"
		"		this.on = true;\n"
		"		this.togglePressed(\"pressed\"));		\n"
		"		this.timer.setTimeout(\"fireFunction\"),this.timerMs,timerEvt);\n"
		"	}\n"
		"	}\n"
		"}\n"
		"\n"
		"switchbutton.prototype.getSwitchValue = function() {\n"
		"	return this.on;\n"
		"}\n"
		"\n"
		"switchbutton.prototype.setSwitchValue = function(onOrOff,firefunction) {\n"
		"	this.on = onOrOff;\n"
		"	//artificial timer event - don't use the values!\n"
		"	var timerEvt = {x:0,y:0,type:\"click\"),detail:1,timeStamp:0}\n"
		"	if (this.on) {\n"
		"	this.togglePressed(\"pressed\"));\n"
		"	if (firefunction) {\n"
		"		this.timer.setTimeout(\"fireFunction\"),this.timerMs,timerEvt);\n"
		"	}\n"
		"	}\n"
		"	else {\n"
		"	this.togglePressed(\"released\"));		\n"
		"	if (firefunction) {\n"
		"		this.timer.setTimeout(\"fireFunction\"),this.timerMs,timerEvt)\n"
		"	}\n"
		"	}\n"
		"}\n"
		"\n"
		"//overwriting fireFunction code\n"
		"switchbutton.prototype.fireFunction = function(evt) {\n"
		"	if (typeof(this.functionToCall) == \"function\")) {\n"
		"	if (this.buttonTextElement) {\n"
		"		this.functionToCall(this.groupId,evt,this.on,this.buttonText);\n"
		"	}\n"
		"	if (this.buttonSymbolInstance) {\n"
		"		this.functionToCall(this.groupId,evt,this.on);\n"
		"	}\n"
		"	}\n"
		"	if (typeof(this.functionToCall) == \"object\")) {\n"
		"	if (this.buttonTextElement) {\n"
		"		this.functionToCall.buttonPressed(this.groupId,evt,this.on,this.buttonText);\n"
		"	}\n"
		"	if (this.buttonSymbolInstance) {\n"
		"		this.functionToCall.buttonPressed(this.groupId,evt,this.on);\n"
		"	}\n"
		"	}\n"
		"	if (typeof(this.functionToCall) == undefined) {\n"
		"	return;	\n"
		"	}\n"
		"}\n"
		"\n"
	));
}

//---------------------------------------------------------
const wxChar * CSVG_Interactive_Map::_Get_Code_Navigation_1(void)
{
	return( SG_STR_MBTOSG(
		"/*\n"
		"Scripts for SVG only webmapping application navigation tools\n"
		"Copyright (C) <2005>  <Andreas Neumann>\n"
		"Version 1.1, 2005-10-05\n"
		"neumann@karto.baug.ethz.ch\n"
		"http://www.carto.net/\n"
		"http://www.carto.net/neumann/\n"
		"\n"
		"Credits: numerous people on svgdevelopers@yahoogroups.com\n"
		"\n"
		"This ECMA script library is free software; you can redistribute it and/or\n"
		"modify it under the terms of the GNU Lesser General Public\n"
		"License as published by the Free Software Foundation; either\n"
		"version 2.1 of the License, or (at your option) any later version.\n"
		"\n"
		"This library is distributed in the hope that it will be useful,\n"
		"but WITHOUT ANY WARRANTY; without even the implied warranty of\n"
		"MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU\n"
		"Lesser General Public License for more details.\n"
		"\n"
		"You should have received a copy of the GNU Lesser General Public\n"
		"License along with this library (http://www.carto.net/papers/svg/navigationTools/lesser_gpl.txt); if not, write to the Free Software\n"
		"Foundation, Inc., 59 Temple Place, Suite 330, Boston, MA  02111-1307  USA\n"
		"\n"
		"----\n"
		"\n"
		"current version: 1.1\n"
		"\n"
		"version history:\n"
		"1.0 initial version\n"
		"1.01 (2005-02-15) added cleanUp() method to map and dragObj objects, some fixes in the documentation\n"
		"1.02 (2005-02-20) fixed problems with double clicks, the second click of a double click is now ignored, using the evt.detail property\n"
		"1.03 (2005-02-22) introduced timestamp and nrLayerToLoad array for dynamic loading (getUrl)\n"
		"1.04 (2005-09-15) introduced a few changes with coordinate calculation to support MozillaSVG, introduced epsg and unitsFactor property based on a suggestion by Bruce Rindahl\n"
		"1.1 (2005-10-05) introduced button object (see button.js file), introduced zoom and pan \"modes\" for repeated actions in the same mode, introduced history of map extents with the ability to go back and forth in the list of previous map extents, some internal changes in the code (e.g. less global variables), added experimental support for the batik SVG 1.2 \"static\" attribute to improve performance while panning the map, added cursors to give better status feedback\n"
		"\n"
		"original document site: http://www.carto.net/papers/svg/navigationTools/\n"
		"Please contact the author in case you want to use code or ideas commercially.\n"
		"If you use this code, please include this copyright header, the included full\n"
		"LGPL 2.1 text and read the terms provided in the LGPL 2.1 license\n"
		"(http://www.gnu.org/copyleft/lesser.txt)\n"
		"\n"
		"-------------------------------\n"
		"\n"
		"Please report bugs and send improvements to neumann@karto.baug.ethz.ch\n"
		"If you use these scripts, please link to the original (http://www.carto.net/papers/svg/navigationTools/)\n"
		"somewhere in the source-code-comment or the \"about\" of your project and give credits, thanks!\n"
		"\n"
		"*/\n"
		"\n"
		"//constructor: holds data on map and initializes various references\n"
		"function map(mapName,maxWidth,minWidth,zoomFact,nrDecimals,epsg,units,unitsFactor,showCoords,coordXId,coordYId,dynamicLayers,digiLayers,activeDigiLayer) {\n"
		"	this.mapName = mapName; //id of svg element containing the map geometry\n"
		"	this.mapSVG = document.getElementById(this.mapName); //reference to nested SVG element holding the map-graphics\n"
		"	this.mainMapGroup = document.getElementById(this.mapName+\"Group\")); //group within mainmap - to be transformed when panning manually\n"
		"	this.dynamicLayers = dynamicLayers; //an associative array holding ids of values that are loaded from the web server dynamically (.getUrl())\n"
		"	this.nrLayerToLoad = 0; //statusVariable to indicate how many layers are still to load\n"
		"	this.maxWidth = maxWidth; //max map width\n"
		"	this.minWidth = minWidth; //min map width, after zooming in\n"
		"	this.zoomFact = zoomFact; //ratio to zoom in or out in relation to previous viewBox\n"
		"	this.digiLayers = digiLayers; //references to digiLayers (for digitizing tools)\n"
		"	this.activeDigiLayer = activeDigiLayer; //active Digi Layer, key is final group id where geometry should be copied to after digitizing\n"
		"	this.pixXOffset = parseFloat(this.mapSVG.getAttributeNS(null,\"x\"))); //offset from left margin of outer viewBox\n"
		"	this.pixYOffset = parseFloat(this.mapSVG.getAttributeNS(null,\"y\"))); //offset from top margin of outer viewBox\n"
		"	var viewBoxArray = this.mapSVG.getAttributeNS(null,\"viewBox\")).split(\" \"));\n"
		"	this.curxOrig = parseFloat(viewBoxArray[0]); //holds the current xOrig\n"
		"	this.curyOrig = parseFloat(viewBoxArray[1]); //holds the current yOrig\n"
		"	this.curWidth = parseFloat(viewBoxArray[2]); //holds the current map width\n"
		"	this.curHeight = parseFloat(viewBoxArray[3]); //holds the current map height\n"
		"	this.pixWidth = parseFloat(this.mapSVG.getAttributeNS(null,\"width\"))); //holds width of the map in pixel coordinates\n"
		"	this.pixHeight = parseFloat(this.mapSVG.getAttributeNS(null,\"height\"))); //holds height of the map in pixel coordinates\n"
		"	this.pixSize = this.curWidth / this.pixWidth; //size of a screen pixel in map units\n"
		"	this.zoomVal = this.maxWidth / this.curWidth * 100; //zoomVal in relation to initial zoom\n"
		"	this.nrDecimals = nrDecimals; //nr of decimal places to be displayed for show coordinates or accuracy when working with digitizing\n"
		"	this.epsg = epsg; //epsg projection code - can be used for building URL strings for loading data from a WMS or spatial database, if you don't need it just input any number\n"
		"	this.navStatus = \"info\"); //to indicate status in navigation, default is \"info\"), no navigation mode active\n"
		"	this.units = units; //holds a string with map units, e.g. \"m\"), alternatively String.fromCharCode(176) for degrees\n"
		"	this.unitsFactor = unitsFactor; //a factor for unit conversion. Can be used f.e. to output feet coordinates when data is meter, If you don't need a conversion, just use a factor of 1\n"
		"	this.showCoords = showCoords;\n"
		"	//initialize array of timestamp/layertoLoad values\n"
		"	this.nrLayerToLoad = new Array();\n"
		"	this.timestamp = null; //holds the timestamp after a zoom or pan occured\n"
		"	//initialize coordinate display if showCoords == true\n"
		"	if (this.showCoords == true) {\n"
		"		//add event listener for coordinate display\n"
		"		this.mapSVG.addEventListener(\"mousemove\"),this,false);\n"
		"		if (typeof(coordXId) == \"string\")) {\n"
		"			this.coordXText = document.getElementById(coordXId).firstChild;\n"
		"		}\n"
		"		else {\n"
		"			alert(\"Error: coordXId needs to be an id of type string\"));\n"
		"		}\n"
		"		if (typeof(coordYId) == \"string\")) {\n"
		"			this.coordYText = document.getElementById(coordYId).firstChild;\n"
		"		}\n"
		"		else {\n"
		"			alert(\"Error: coordYId needs to be an id of type string\"));\n"
		"		}\n"
		"	}\n"
		"	//a new array containing map extents\n"
		"	this.mapExtents = new Array();\n"
		"	this.mapExtents.push({xmin:this.curxOrig,ymin:((this.curyOrig + this.curHeight) * -1),xmax:(this.curxOrig + this.curWidth),ymax:(this.curyOrig * -1)});\n"
		"	this.curMapExtentIndex = 0;\n"
		"	//create background-element to receive events for showing coordinates\n"
		"	//this rect is also used for manual zooming and panning\n"
		"	this.backgroundRect = document.createElementNS(svgNS,\"rect\"));\n"
		"	this.backgroundRect.setAttributeNS(null,\"x\"),this.curxOrig);\n"
		"	this.backgroundRect.setAttributeNS(null,\"y\"),this.curyOrig);\n"
		"	this.backgroundRect.setAttributeNS(null,\"width\"),this.curWidth);\n"
		"	this.backgroundRect.setAttributeNS(null,\"height\"),this.curHeight);\n"
		"	this.backgroundRect.setAttributeNS(null,\"fill\"),\"none\"));\n"
		"	this.backgroundRect.setAttributeNS(null,\"stroke\"),\"none\"));\n"
		"	this.backgroundRect.setAttributeNS(null,\"pointer-events\"),\"fill\"));\n"
		"	this.backgroundRect.setAttributeNS(null,\"id\"),\"mapBackgroundRect\"));\n"
		"	this.backgroundRect.addEventListener(\"mousedown\"),this,false);\n"
		"	this.backgroundRect.addEventListener(\"mousemove\"),this,false);\n"
		"	this.backgroundRect.addEventListener(\"mouseup\"),this,false);\n"
		"	this.backgroundRect.addEventListener(\"mouseout\"),this,false);\n"
		"	this.backgroundRect.addEventListener(\"click\"),this,false);\n"
		"	this.mainMapGroup.insertBefore(this.backgroundRect,this.mainMapGroup.firstChild);\n"
		"	//determine if viewer is capable of getScreenCTM\n"
		"	if (document.documentElement.getScreenCTM) {\n"
		"		this.getScreenCTM = true;\n"
		"	}\n"
		"	else {\n"
		"		this.getScreenCTM = false;		\n"
		"	}\n"
		"}\n"
		"\n"
		"//resets viewBox of main map after zooming and panning\n"
		"map.prototype.newViewBox = function(refRectId,history) {\n"
		"	var myRefRect = document.getElementById(refRectId);\n"
		"	this.curxOrig = parseFloat(myRefRect.getAttributeNS(null,\"x\")));\n"
		"	this.curyOrig = parseFloat(myRefRect.getAttributeNS(null,\"y\")));\n"
		"	this.curWidth = parseFloat(myRefRect.getAttributeNS(null,\"width\")));\n"
		"	this.curHeight = parseFloat(myRefRect.getAttributeNS(null,\"height\")));\n"
		"	var myViewBoxString = this.curxOrig + \" \" + this.curyOrig + \" \" + this.curWidth + \" \" + this.curHeight;\n"
		"	this.pixSize = this.curWidth / this.pixWidth;\n"
		"	this.zoomVal = this.maxWidth / this.curWidth * 100;\n"
		"	this.mapSVG.setAttributeNS(null,\"viewBox\"),myViewBoxString);\n"
		"	myMapApp.zoomSlider.setValue(this.curWidth);\n"
		"	loadProjectSpecific();\n"
		"	if (history) {\n"
		"	    this.mapExtents.push({xmin:this.curxOrig,ymin:((this.curyOrig + this.curHeight) * -1),xmax:(this.curxOrig + this.curWidth),ymax:(this.curyOrig * -1)});\n"
		"               this.curMapExtentIndex = (this.mapExtents.length - 1);\n"
		"	}\n"
		"	this.checkButtons();\n"
		"}\n"
		"\n"
		"map.prototype.backwardExtent = function() {\n"
		"    if (this.curMapExtentIndex != 0) {\n"
		"        this.curMapExtentIndex--;\n"
		"        this.setNewViewBox(this.mapExtents[this.curMapExtentIndex].xmin,this.mapExtents[this.curMapExtentIndex].ymin,this.mapExtents[this.curMapExtentIndex].xmax,this.mapExtents[this.curMapExtentIndex].ymax,false);\n"
		"    }\n"
		"}\n"
		"\n"
		"map.prototype.forwardExtent = function() {\n"
		"    if (this.mapExtents.length != (this.curMapExtentIndex + 1)) {\n"
		"        this.curMapExtentIndex++;\n"
		"        this.setNewViewBox(this.mapExtents[this.curMapExtentIndex].xmin,this.mapExtents[this.curMapExtentIndex].ymin,this.mapExtents[this.curMapExtentIndex].xmax,this.mapExtents[this.curMapExtentIndex].ymax,false);\n"
		"    }\n"
		"}\n"
		"\n"
		"map.prototype.checkButtons = function() {\n"
		"           //check extent history\n"
		"           if (this.curMapExtentIndex == 0) {\n"
		"                if (myMapApp.buttons[\"backwardExtent\"].activated) {\n"
		"                  myMapApp.buttons[\"backwardExtent\"].deactivate();\n"
		"                }\n"
		"           }\n"
		"           else {\n"
		"                if (!myMapApp.buttons[\"backwardExtent\"].activated) {\n"
		"                  myMapApp.buttons[\"backwardExtent\"].activate();\n"
		"                }           \n"
		"           }\n"
		"           if (this.mapExtents.length == (this.curMapExtentIndex + 1)) {\n"
		"                if (myMapApp.buttons[\"forwardExtent\"].activated) {\n"
		"                   myMapApp.buttons[\"forwardExtent\"].deactivate();\n"
		"                }\n"
		"           }\n"
		"           else {\n"
		"                if (!myMapApp.buttons[\"forwardExtent\"].activated) {\n"
		"                  myMapApp.buttons[\"forwardExtent\"].activate();\n"
		"                }           \n"
		"           }\n"
		" \n"
		"           //maximum map width reached, cannot zoom out further\n"
		"           if (this.curWidth >= this.maxWidth) {\n"
		"               if (myMapApp.buttons[\"zoomOut\"].activated) {\n"
		"                   myMapApp.buttons[\"zoomOut\"].deactivate();\n"
		"               }\n"
		"               if (myMapApp.buttons[\"zoomFull\"].activated) {\n"
		"                   myMapApp.buttons[\"zoomFull\"].deactivate();\n"
		"               }\n"
		"               if (myMapApp.buttons[\"panManual\"].activated) {\n"
		"                   myMapApp.buttons[\"panManual\"].deactivate();\n"
		"               }\n"
		"               if (myMapApp.buttons[\"recenterMap\"].activated) {\n"
		"                   myMapApp.buttons[\"recenterMap\"].deactivate();\n"
		"               }\n"
		"               if (!myMapApp.buttons[\"zoomIn\"].activated) {\n"
		"                   myMapApp.buttons[\"zoomIn\"].activate();\n"
		"               }\n"
		"               if (!myMapApp.buttons[\"zoomManual\"].activated) {\n"
		"                   myMapApp.buttons[\"zoomManual\"].activate();\n"
		"               }\n"
		"           }\n"
		"           //minimum map width reached, cannot zoom in further\n"
		"           else if (this.curWidth <= this.minWidth) {\n"
		"               if (myMapApp.buttons[\"zoomIn\"].activated) {\n"
		"                   myMapApp.buttons[\"zoomIn\"].deactivate();\n"
		"               }\n"
		"               if (myMapApp.buttons[\"zoomManual\"].activated) {\n"
		"                   myMapApp.buttons[\"zoomManual\"].deactivate();\n"
		"               }\n"
		"               if (!myMapApp.buttons[\"zoomOut\"].activated) {\n"
		"                   myMapApp.buttons[\"zoomOut\"].activate();\n"
		"               }\n"
		"               if (!myMapApp.buttons[\"zoomFull\"].activated) {\n"
		"                   myMapApp.buttons[\"zoomFull\"].activate();\n"
		"               }\n"
		"               if (!myMapApp.buttons[\"panManual\"].activated) {\n"
		"                   myMapApp.buttons[\"panManual\"].activate();\n"
		"               }\n"
		"               if (!myMapApp.buttons[\"recenterMap\"].activated) {\n"
		"                   myMapApp.buttons[\"recenterMap\"].activate();\n"
		"               }           \n"
		"           }\n"
		"           //in between\n"
		"           else {\n"
		"               if (!myMapApp.buttons[\"zoomIn\"].activated) {\n"
		"                   myMapApp.buttons[\"zoomIn\"].activate();\n"
		"               }\n"
		"               if (!myMapApp.buttons[\"zoomManual\"].activated) {\n"
		"                   myMapApp.buttons[\"zoomManual\"].activate();\n"
		"               }\n"
		"               if (!myMapApp.buttons[\"zoomOut\"].activated) {\n"
		"                   myMapApp.buttons[\"zoomOut\"].activate();\n"
		"               }\n"
		"               if (!myMapApp.buttons[\"zoomFull\"].activated) {\n"
		"                   myMapApp.buttons[\"zoomFull\"].activate();\n"
		"               }\n"
		"               if (!myMapApp.buttons[\"panManual\"].activated) {\n"
		"                   myMapApp.buttons[\"panManual\"].activate();\n"
		"               }\n"
		"               if (!myMapApp.buttons[\"recenterMap\"].activated) {\n"
		"                   myMapApp.buttons[\"recenterMap\"].activate();\n"
		"               }           \n"
		"           }           \n"
		"}\n"
		"\n"
		"map.prototype.setNewViewBox = function(xmin,ymin,xmax,ymax,history) {\n"
		"	//check if within constraints\n"
		"	if (xmin < myMapApp.refMapDragger.constrXmin) {\n"
		"		xmin = myMapApp.refMapDragger.constrXmin;\n"
		"	}\n"
		"	if (xmax > myMapApp.refMapDragger.constrXmax) {\n"
		"		xmax = myMapApp.refMapDragger.constrXmin;\n"
		"	}\n"
		"	if (ymin < (myMapApp.refMapDragger.constrYmax * -1)) {\n"
		"		ymin = myMapApp.refMapDragger.constrYmax * -1;\n"
		"	}\n"
		"	if (ymax > (myMapApp.refMapDragger.constrYmin * -1)) {\n"
		"		ymax = myMapApp.refMapDragger.constrYmin * -1;\n"
		"	}\n"
		"	\n"
		"	var origWidth = xmax - xmin;\n"
		"	var origHeight = ymax - ymin;\n"
		"	var myRatio = this.curWidth/this.curHeight;\n"
		"	\n"
		"	if (origWidth / origHeight > myRatio) { //case rect is more wide than ratio\n"
		"		var newWidth = origWidth;\n"
		"		var newHeight = origWidth * 1 / myRatio;\n"
		"		ymin = (ymax + (newHeight - origHeight) / 2) * -1;\n"
		"	}\n"
		"	else {\n"
		"		var newHeight = origHeight;\n"
		"		var newWidth = newHeight * myRatio;\n"
		"		xmin = xmin - (newWidth - origWidth) / 2;\n"
		"		ymin = ymax * -1;\n"
		"	}\n"
		"	//check if within constraints\n"
		"	if (xmin < myMapApp.refMapDragger.constrXmin) {\n"
		"		xmin = myMapApp.refMapDragger.constrXmin;\n"
		"	}\n"
		"	if (ymin < myMapApp.refMapDragger.constrYmin) {\n"
		"		ymin = myMapApp.refMapDragger.constrYmin;\n"
		"	}\n"
		"	if ((xmin + newWidth) > myMapApp.refMapDragger.constrXmax) {\n"
		"		xmin = myMapApp.refMapDragger.constrXmax - newWidth;\n"
		"	}\n"
		"	if ((ymin + newHeight) > myMapApp.refMapDragger.constrYmax) {\n"
		"		ymin = myMapApp.refMapDragger.constrYmax - newHeight;\n"
		"	}		\n"
		"	myMapApp.refMapDragger.newView(xmin,ymin,newWidth,newHeight);\n"
		"	this.newViewBox(myMapApp.refMapDragger.dragId,history);\n"
		"}\n"
		"\n"
		"//handles events associated with navigation\n"
		"map.prototype.handleEvent = function(evt) {\n"
		"	var callerId = evt.currentTarget.getAttributeNS(null,\"id\"));\n"
		"	if (callerId.match(/\\bzoomBgRectManual/)) {\n"
		"		this.zoomManDragRect(evt);\n"
		"	}\n"
		"	if (callerId.match(/\\bzoomBgRectRecenter/)) {\n"
		"		this.recenterFinally(evt);\n"
		"	}\n"
		"	if (callerId.match(/\\bbgPanManual/)) {\n"
		"		this.panManualFinally(evt);\n"
		"	}\n"
		"	if (callerId == \"mainMap\" && evt.type == \"mousemove\")) {\n"
		"		if (this.navStatus != \"panmanualActive\")) {\n"
		"		    this.showCoordinates(evt);\n"
		"		}\n"
		"	}\n"
		"}\n"
		"\n"
		"//calcs coordinates; relies on myMapApp to handle different window sizes and resizing of windows\n"
		"map.prototype.calcCoord = function(evt) {\n"
		"	//with getScreenCTM the values are already in the inner coordinate system\n"
		"	if (!this.getScreenCTM) {\n"
		"		var coords = myMapApp.calcCoord(evt,this.mapSVG);\n"
		"		coords.x = this.curxOrig + (coords.x - this.pixXOffset) * this.pixSize;\n"
		"		coords.y = (this.curyOrig + (coords.y - this.pixYOffset) * this.pixSize);\n"
		"	}\n"
		"	else {	\n"
		"		var coords = myMapApp.calcCoord(evt,this.mainMapGroup);\n"
		"	}\n"
		"	return coords;\n"
		"}\n"
		"\n"
		"//displays x and y coordinates in two separate text elements\n"
		"map.prototype.showCoordinates = function(evt) {\n"
		"	var mapCoords = this.calcCoord(evt);\n"
		"	this.coordXText.nodeValue = \"X: \" + formatNumberString((mapCoords.x * this.unitsFactor).toFixed(this.nrDecimals)) + this.units;\n"
		"	this.coordYText.nodeValue = \"Y: \" + formatNumberString((mapCoords.y * this.unitsFactor * -1).toFixed(this.nrDecimals)) + this.units;\n"
		"}\n"
		"\n"
		"//checks for and removes temporary rectangle objects\n"
		"map.prototype.stopNavModes = function() {\n"
		"	if (this.navStatus != \"info\")) {\n"
		"                    	if (this.navStatus == \"zoomManual\")) {\n"
		"                    	    myMapApp.buttons[\"zoomManual\"].setSwitchValue(false,false);\n"
		"                    	}\n"
		"                     	if (this.navStatus == \"panmanual\" || this.navStatus == \"panmanualActive\")) {\n"
		"                    	    myMapApp.buttons[\"panManual\"].setSwitchValue(false,false);\n"
		"                    	}\n"
		"                    	if (this.navStatus == \"recenter\")) {\n"
		"                    	    myMapApp.buttons[\"recenterMap\"].setSwitchValue(false,false);\n"
		"                    	}\n"
		"                              this.backgroundRect.setAttributeNS(null,\"id\"),\"mapBackgroundRect\"));\n"
		"		myMapApp.buttons[\"infoButton\"].setSwitchValue(true,false);\n"
		"		this.navStatus = \"info\");\n"
		"	               this.mapSVG.setAttributeNS(null,\"cursor\"),\"crosshair\"));\n"
		"		statusChange(\"Mode: Infomode\"));\n"
		"	               this.mainMapGroup.insertBefore(this.backgroundRect,this.mainMapGroup.firstChild);\n"
		"	}\n"
		"}\n"
		"\n"
		"//starts manual zooming mode\n"
		"map.prototype.zoomManual = function(evt) {\n"
		"	if (Math.round(myMainMap.curWidth) > myMainMap.minWidth && evt.detail == 1) {\n"
		"		this.navStatus = \"zoomManual\");\n"
		"                              this.backgroundRect.setAttributeNS(null,\"id\"),\"zoomBgRectManual\"));	\n"
		"	               this.mainMapGroup.appendChild(this.backgroundRect);\n"
		"	               this.mapSVG.setAttributeNS(null,\"cursor\"),\"se-resize\"));\n"
		"		statusChange(\"Click and drag rectangle for new map extent.\"));\n"
		"	}\n"
		"}\n"
		"\n"
		"//manages manual zooming by drawing a rectangle\n"
		"map.prototype.zoomManDragRect = function(evt) {\n"
		"	var mapCoords = this.calcCoord(evt);\n"
		"	var myX = mapCoords.x;\n"
		"	var myY = mapCoords.y;\n"
		"	var myYXFact = this.curHeight / this.curWidth;\n"
		"	if (evt.type == \"mousedown\")) {\n"
		"		this.manZoomActive = 1;\n"
		"		this.zoomRect = document.createElementNS(svgNS,\"rect\"));\n"
		"		var myLineWidth = this.curWidth * 0.003;\n"
		"		this.zoomRect.setAttributeNS(null,\"id\"),\"zoomRect\"));\n"
		"		this.zoomRect.setAttributeNS(null,\"fill\"),\"white\"));\n"
		"		this.zoomRect.setAttributeNS(null,\"fill-opacity\"),\"0.5\"));\n"
		"		this.zoomRect.setAttributeNS(null,\"stroke\"),\"dimgray\"));\n"
		"		this.zoomRect.setAttributeNS(null,\"stroke-width\"),myLineWidth);\n"
		"		this.zoomRect.setAttributeNS(null,\"stroke-dasharray\"),(myLineWidth*3)+\"),\"+myLineWidth);\n"
		"		this.zoomRect.setAttributeNS(null,\"pointer-events\"),\"none\"));\n"
		"		this.zoomRect.setAttributeNS(null,\"x\"),myX);\n"
		"		this.zoomRect.setAttributeNS(null,\"y\"),myY);\n"
		"		this.zoomRect.setAttributeNS(null,\"width\"),this.minWidth);\n"
		"		this.zoomRect.setAttributeNS(null,\"height\"),this.minWidth * myYXFact);\n"
		"		this.mainMapGroup.appendChild(this.zoomRect);\n"
		"		this.zoomRectOrigX = myX;\n"
		"		this.zoomRectOrigY = myY;\n"
		"	}\n"
		"	if (evt.type == \"mousemove\" && this.manZoomActive == 1) {\n"
		"		var myZoomWidth = myX - this.zoomRectOrigX;\n"
		"		if (myZoomWidth < 0) {\n"
		"			if (Math.abs(myZoomWidth) < this.minWidth) {\n"
		"				this.zoomRect.setAttributeNS(null,\"x\"),this.zoomRectOrigX - this.minWidth);\n"
		"				this.zoomRect.setAttributeNS(null,\"y\"),this.zoomRectOrigY - this.minWidth * myYXFact);\n"
		"				this.zoomRect.setAttributeNS(null,\"width\"),this.minWidth);\n"
		"				this.zoomRect.setAttributeNS(null,\"height\"),this.minWidth * myYXFact);\n"
		"			}\n"
		"			else {\n"
		"				this.zoomRect.setAttributeNS(null,\"x\"),myX);\n"
		"				this.zoomRect.setAttributeNS(null,\"y\"),this.zoomRectOrigY - Math.abs(myZoomWidth) * myYXFact);\n"
		"				this.zoomRect.setAttributeNS(null,\"width\"),Math.abs(myZoomWidth));\n"
		"				this.zoomRect.setAttributeNS(null,\"height\"),Math.abs(myZoomWidth) * myYXFact);			\n"
		"			}\n"
		"		}\n"
		"		else {\n"
		"			this.zoomRect.setAttributeNS(null,\"x\"),this.zoomRectOrigX);\n"
		"			this.zoomRect.setAttributeNS(null,\"y\"),this.zoomRectOrigY);\n"
		"			if (myZoomWidth < this.minWidth) {\n"
		"				this.zoomRect.setAttributeNS(null,\"width\"),this.minWidth);\n"
		"				this.zoomRect.setAttributeNS(null,\"height\"),this.minWidth * myYXFact);		\n"
		"			}\n"
		"			else {\n"
		"				this.zoomRect.setAttributeNS(null,\"width\"),myZoomWidth);\n"
		"				this.zoomRect.setAttributeNS(null,\"height\"),myZoomWidth * myYXFact);\n"
		"			}\n"
		"		}\n"
		"	}\n"
		"	if ((evt.type == \"mouseup\" || evt.type == \"mouseout\")) && this.manZoomActive == 1) {\n"
		"		this.manZoomActive = 0;\n"
		"		if (parseFloat(this.zoomRect.getAttributeNS(null,\"width\"))) > this.curWidth * 0.02) {\n"
		"			myMapApp.refMapDragger.newView(parseFloat(this.zoomRect.getAttributeNS(null,\"x\"))),parseFloat(this.zoomRect.getAttributeNS(null,\"y\"))),parseFloat(this.zoomRect.getAttributeNS(null,\"width\"))),parseFloat(this.zoomRect.getAttributeNS(null,\"height\"))));\n"
		"			this.newViewBox(myMapApp.refMapDragger.dragId,true);\n"
		"		}\n"
		"		this.mainMapGroup.removeChild(this.zoomRect);\n"
		"		statusChange(\"Mode: Manual Zooming\"));\n"
		"	}\n"
		"}\n"
		"\n"
		"//initializes recentering mode\n"
		"map.prototype.recenter = function(evt) {\n"
		"	if (evt.detail == 1) {\n"
		"		this.navStatus = \"recenter\");\n"
		"                              this.backgroundRect.setAttributeNS(null,\"id\"),\"zoomBgRectRecenter\"));	\n"
		"	               this.mainMapGroup.appendChild(this.backgroundRect);\n"
		"	               this.mapSVG.setAttributeNS(null,\"cursor\"),\"pointer\"));\n"
		"		statusChange(\"Click in map to define new map center.\"));\n"
		"	}\n"
		"}\n"
		"\n"
		"//finishes recentering after mouse-click\n"
		"map.prototype.recenterFinally = function(evt) {\n"
		"	if (evt.type == \"click\")) {\n"
		"		var mapCoords = this.calcCoord(evt);\n"
		"		var myX = mapCoords.x;\n"
		"		var myY = mapCoords.y;\n"
		"		var myNewX = myX - this.curWidth / 2;\n"
		"		var myNewY = myY - this.curHeight / 2;\n"
		"		\n"
		"		//check if within constraints\n"
		"		if (myNewX < myMapApp.refMapDragger.constrXmin) {\n"
		"			myNewX = myMapApp.refMapDragger.constrXmin;\n"
		"		}\n"
		"		if (myNewY < myMapApp.refMapDragger.constrYmin) {\n"
		"			myNewY = myMapApp.refMapDragger.constrYmin;\n"
		"		}\n"
		"		if ((myNewX + this.curWidth) > myMapApp.refMapDragger.constrXmax) {\n"
		"			myNewX = myMapApp.refMapDragger.constrXmax - this.curWidth;\n"
		"		}\n"
		"		if ((myNewY + this.curHeight) > myMapApp.refMapDragger.constrYmax) {\n"
		"			myNewY = myMapApp.refMapDragger.constrYmax - this.curHeight;\n"
		"		}\n"
		"		myMapApp.refMapDragger.newView(myNewX,myNewY,this.curWidth,this.curHeight);\n"
		"		this.newViewBox(myMapApp.refMapDragger.dragId,true);\n"
		"		statusChange(\"Mode: Recentering Map\"));\n"
		"	}\n"
		"}\n"
		"\n"
		"//initializes manual panning\n"
		"map.prototype.panManual = function(evt) {\n"
		"	if (evt.detail == 1) {\n"
		"		this.navStatus = \"panmanual\");\n"
		"                              this.backgroundRect.setAttributeNS(null,\"id\"),\"bgPanManual\"));	\n"
		"	               this.mainMapGroup.appendChild(this.backgroundRect);\n"
		"	               this.mapSVG.setAttributeNS(null,\"cursor\"),\"move\"));\n"
		"		statusChange(\"Mouse down and move to pan the map\"));\n"
		"	}\n"
		"}\n"
		"\n"
		"//manages and finishes manual panning\n"
		"map.prototype.panManualFinally = function(evt) {\n"
		"	if (evt.type == \"mousedown\")) {\n"
		"		this.navStatus = \"panmanualActive\");\n"
		"		this.panCoords = this.calcCoord(evt);\n"
		"		this.panCoorX = this.panCoords.x;\n"
		"		this.panCoorY = this.panCoords.y;\n"
		"		this.diffX = 0;\n"
		"		this.diffY = 0;\n"
		"		this.mainMapGroup.setAttributeNS(batikNS,\"static\"),\"true\"));\n"
		"	}\n"
		"	if (evt.type == \"mousemove\" && this.navStatus == \"panmanualActive\")) {\n"
		"		var mapCoords = this.calcCoord(evt);\n"
		"		if (this.getScreenCTM) {\n"
		"			this.diffX = this.panCoorX - mapCoords.x + this.diffX;\n"
		"			this.diffY = this.panCoorY - mapCoords.y + this.diffY;\n"
		"		}\n"
		"		else {\n"
		"			this.diffX = this.panCoorX - mapCoords.x;\n"
		"			this.diffY = this.panCoorY - mapCoords.y;		\n"
		"		}\n"
		"		var myNewX = this.curxOrig + this.diffX;\n"
		"		var myNewY = this.curyOrig + this.diffY;\n"
		"		//check if within constraints\n"
		"		if (myNewX < myMapApp.refMapDragger.constrXmin) {\n"
		"			var myNewXTemp = myMapApp.refMapDragger.constrXmin;\n"
		"			this.diffX = this.diffX + (myNewXTemp - myNewX);\n"
		"			myNewX = myNewXTemp;\n"
		"		}\n"
		"		if (myNewY < myMapApp.refMapDragger.constrYmin) {\n"
		"			var myNewYTemp = myMapApp.refMapDragger.constrYmin;\n"
		"			this.diffY = this.diffY + (myNewYTemp - myNewY);\n"
		"			myNewY = myNewYTemp;\n"
		"		}\n"
		"		if ((myNewX + this.curWidth) > myMapApp.refMapDragger.constrXmax) {\n"
		"			var myNewXTemp = myMapApp.refMapDragger.constrXmax - this.curWidth;\n"
		"			this.diffX = this.diffX + (myNewXTemp - myNewX);\n"
		"			myNewX = myNewXTemp;\n"
		"		}\n"
		"		if ((myNewY + this.curHeight) > myMapApp.refMapDragger.constrYmax) {\n"
		"			var myNewYTemp = myMapApp.refMapDragger.constrYmax - this.curHeight;\n"
		"			this.diffY = this.diffY + (myNewYTemp - myNewY);\n"
		"			myNewY = myNewYTemp;\n"
		"		}	\n"
		"		var transformString = \"translate(\"+(this.diffX * -1) +\"),\"+(this.diffY * -1)+\"))\");\n"
		"		this.mainMapGroup.setAttributeNS(null,\"transform\"),transformString);\n"
		"		myMapApp.refMapDragger.newView(myNewX,myNewY,this.curWidth,this.curHeight);\n"
		"	}\n"
		"	if ((evt.type == \"mouseup\" || evt.type == \"mouseout\")) && this.navStatus == \"panmanualActive\")) {\n"
		"		this.navStatus = \"panmanual\");\n"
		"		this.mainMapGroup.setAttributeNS(batikNS,\"static\"),\"false\"));\n"
		"		this.mainMapGroup.setAttributeNS(null,\"transform\"),\"translate(0,0)\"));\n"
		"		this.newViewBox(myMapApp.refMapDragger.dragId,true);			\n"
		"		statusChange(\"Mode: Manual Panning\"));	\n"
		"	}\n"
		"}\n"
		"\n"
		"//remove all temporarily added elements and event listeners\n"
		"map.prototype.cleanUp = function() {\n"
		"			//remove background rect\n"
		"			this.backgroundRect.parentNode.removeChild(this.backgroundRect);\n"
		"			//remove eventlisteners\n"
		"			if (this.showCoords == true) {\n"
		"				//add event listener for coordinate display\n"
		"				this.mapSVG.removeEventListener(\"mousemove\"),this,false);\n"
		"			}\n"
		"\n"
		"}\n"
		"\n"
		"//make an element (rectangle) draggable within constraints\n"
		"function dragObj(dragId,referenceMap,myDragSymbol,dragSymbThreshold,showCoords,coordXId,coordYId,mainMapObj) {\n"
		"	this.dragId = dragId;\n"
		"	this.myDragger = document.getElementById(this.dragId);\n"
		"	this.myRefMap = document.getElementById(referenceMap);\n"
		"	this.myDragSymbol = document.getElementById(myDragSymbol);	\n"
		"	this.dragSymbThreshold = dragSymbThreshold;\n"
		"	var viewBox = this.myRefMap.getAttributeNS(null,\"viewBox\")).split(\" \"));\n"
		"	this.constrXmin = parseFloat(viewBox[0]);\n"
		"	this.constrYmin = parseFloat(viewBox[1]);\n"
		"	this.constrXmax = this.constrXmin + parseFloat(viewBox[2]);\n"
		"	this.constrYmax = this.constrYmin + parseFloat(viewBox[3]);\n"
		"	this.refMapX = parseFloat(this.myRefMap.getAttributeNS(null,\"x\")));\n"
		"	this.refMapY = parseFloat(this.myRefMap.getAttributeNS(null,\"y\")));\n"
		"	this.refMapWidth = parseFloat(this.myRefMap.getAttributeNS(null,\"width\")));\n"
		"	this.pixSize = (this.constrXmax - this.constrXmin) / this.refMapWidth;\n"
		"	this.mainMapObj = mainMapObj;\n"
		"	//initialize coordinate display if showCoords == true\n"
		"	this.showCoords = showCoords;\n"
		"	if (this.showCoords == true) {\n"
		"		if (typeof(coordXId) == \"string\")) {\n"
		"			this.coordXText = document.getElementById(coordXId).firstChild;\n"
		"		}\n"
		"		else {\n"
		"			alert(\"Error: coordXId needs to be an id of type string\"));\n"
		"		}\n"
		"		if (typeof(coordYId) == \"string\")) {\n"
		"			this.coordYText = document.getElementById(coordYId).firstChild;\n"
		"		}\n"
		"		else {\n"
		"			alert(\"Error: coordYId needs to be an id of type string\"));\n"
		"		}\n"
		"	}\n"
		"	//determine if viewer is capable of getScreenCTM\n"
		"	if (document.documentElement.getScreenCTM) {\n"
		"		this.getScreenCTM = true;\n"
		"	}\n"
		"	else {\n"
		"		this.getScreenCTM = false;		\n"
		"	}\n"
		"	this.status = false;\n"
		"}\n"
		"\n"
		"dragObj.prototype.calcCoord = function(evt) {\n"
		"	//with getScreenCTM the values are already in the inner coordinate system but without using the outer offset\n"
		"	var coordPoint = myMapApp.calcCoord(evt,this.myRefMap);\n"
		"	if (!this.getScreenCTM) {\n"
		"		coordPoint.x = this.constrXmin + (coordPoint.x - this.refMapX) * this.pixSize;\n"
		"		coordPoint.y = this.constrYmin + (coordPoint.y - this.refMapY) * this.pixSize;\n"
		"	}\n"
		"	return coordPoint;\n"
		"}\n"
		"\n"
		"dragObj.prototype.handleEvent = function(evt) {\n"
		"	if (evt.type == \"mousemove\")) {\n"
		"		var mapCoords = this.calcCoord(evt);\n"
		"		this.coordXText.nodeValue = \"X: \" + formatNumberString(mapCoords.x.toFixed(this.mainMapObj.nrDecimals)) + this.mainMapObj.units;\n"
		"		this.coordYText.nodeValue = \"Y: \" + formatNumberString((mapCoords.y * -1).toFixed(this.mainMapObj.nrDecimals)) + this.mainMapObj.units;\n"
		"	}\n"
		"	this.drag(evt);\n"
		"}\n"
		"\n"
		"dragObj.prototype.newView = function(x,y,width,height) {\n"
		"	this.myDragger.setAttributeNS(null,\"x\"),x);\n"
		"	this.myDragger.setAttributeNS(null,\"y\"),y);\n"
		"	this.myDragger.setAttributeNS(null,\"width\"),width);\n"
		"	this.myDragger.setAttributeNS(null,\"height\"),height);\n"
		"	this.myDragSymbol.setAttributeNS(null,\"x\"),(x + width/2));\n"
		"	this.myDragSymbol.setAttributeNS(null,\"y\"),(y + height/2));\n"
		"	if (width < this.dragSymbThreshold) {\n"
		"		this.myDragSymbol.setAttributeNS(null,\"visibility\"),\"visible\"));\n"
		"	}\n"
		"	else {\n"
		"		this.myDragSymbol.setAttributeNS(null,\"visibility\"),\"hidden\"));	\n"
		"	}\n"
		"}\n"
		"\n"
	));
}

//---------------------------------------------------------
const wxChar * CSVG_Interactive_Map::_Get_Code_Navigation_2(void)
{
	return( SG_STR_MBTOSG(
		"//this method was previously called \"resizeDragger\" - now renamed to .getSliderVal\n"
		"//this method receives values from the zoom slider\n"
		"dragObj.prototype.getSliderVal = function(status,sliderGroupName,width) {\n"
		"	var myX = parseFloat(this.myDragger.getAttributeNS(null,\"x\")));\n"
		"	var myY = parseFloat(this.myDragger.getAttributeNS(null,\"y\")));\n"
		"	var myWidth = parseFloat(this.myDragger.getAttributeNS(null,\"width\")));\n"
		"	var myHeight = parseFloat(this.myDragger.getAttributeNS(null,\"height\")));\n"
		"	var myCenterX = myX + myWidth / 2;\n"
		"	var myCenterY = myY + myHeight / 2;\n"
		"	var myRatio = myHeight / myWidth;\n"
		"	var toMoveX = myCenterX - width / 2;\n"
		"	var toMoveY = myCenterY - width * myRatio / 2;\n"
		"	if (toMoveX < this.constrXmin) {\n"
		"		toMoveX = this.constrXmin;\n"
		"	}\n"
		"	if ((toMoveX + width) > this.constrXmax) {\n"
		"		toMoveX = this.constrXmax - width;\n"
		"	}\n"
		"	if (toMoveY < this.constrYmin) {\n"
		"		toMoveY = this.constrYmin;\n"
		"	}\n"
		"	if ((toMoveY + width * myRatio) > this.constrYmax) {\n"
		"		toMoveY = this.constrYmax - width * myRatio;\n"
		"	}\n"
		"	this.newView(toMoveX,toMoveY,width,width * myRatio);\n"
		"	if (status == \"release\")) {\n"
		"	           myMainMap.stopNavModes();\n"
		"		this.mainMapObj.newViewBox(this.dragId,true);\n"
		"	}\n"
		"}\n"
		"\n"
		"dragObj.prototype.drag = function(evt) {\n"
		"	if (evt.type == \"mousedown\")) {\n"
		"		this.myRefMap.setAttributeNS(null,\"cursor\"),\"move\"));\n"
		"		this.status = true;\n"
		"	}\n"
		"	if ((evt.type == \"mousemove\" || evt.type == \"mousedown\")) && this.status == true) {\n"
		"		var coords = this.calcCoord(evt);\n"
		"		var newEvtX = coords.x;\n"
		"		var newEvtY = coords.y;\n"
		"		var myX = parseFloat(this.myDragger.getAttributeNS(null,\"x\")));\n"
		"		var myY = parseFloat(this.myDragger.getAttributeNS(null,\"y\")));\n"
		"		var myWidth = parseFloat(this.myDragger.getAttributeNS(null,\"width\")));\n"
		"		var myHeight = parseFloat(this.myDragger.getAttributeNS(null,\"height\")));\n"
		"		var toMoveX = newEvtX - myWidth / 2;\n"
		"		var toMoveY = newEvtY - myHeight / 2;\n"
		"		if (toMoveX < this.constrXmin) {\n"
		"			toMoveX = this.constrXmin;\n"
		"		}\n"
		"		if ((toMoveX + myWidth) > this.constrXmax) {\n"
		"			toMoveX = this.constrXmax - myWidth;\n"
		"		}\n"
		"		if (toMoveY < this.constrYmin) {\n"
		"			toMoveY = this.constrYmin;\n"
		"		}\n"
		"		if ((toMoveY + myHeight) > this.constrYmax) {\n"
		"			toMoveY = this.constrYmax - myHeight;\n"
		"		}\n"
		"		this.newView(toMoveX,toMoveY,myWidth,myHeight);\n"
		"	}\n"
		"	if ((evt.type == \"mouseup\" || evt.type == \"mouseout\")) && this.status == true) {\n"
		"		this.status = false;\n"
		"		if (evt.detail == 1) { //second click is ignored\n"
		"		               this.myRefMap.setAttributeNS(null,\"cursor\"),\"crosshair\"));\n"
		"			this.mainMapObj.newViewBox('dragRectForRefMap',true);\n"
		"		}\n"
		"	}\n"
		"}\n"
		"\n"
		"dragObj.prototype.zoom = function(inOrOut) {\n"
		"	var myOldX = this.myDragger.getAttributeNS(null,\"x\"));\n"
		"	var myOldY = this.myDragger.getAttributeNS(null,\"y\"));\n"
		"	var myOldWidth = this.myDragger.getAttributeNS(null,\"width\"));\n"
		"	var myOldHeight = this.myDragger.getAttributeNS(null,\"height\"));\n"
		"	switch (inOrOut) {\n"
		"		case \"in\":\n"
		"			var myNewX = parseFloat(myOldX) + myOldWidth / 2 - (myOldWidth * this.mainMapObj.zoomFact * 0.5);\n"
		"			var myNewY = parseFloat(myOldY) + myOldHeight / 2 - (myOldHeight * this.mainMapObj.zoomFact * 0.5);\n"
		"			var myNewWidth = myOldWidth * this.mainMapObj.zoomFact;\n"
		"			var myNewHeight = myOldHeight * this.mainMapObj.zoomFact;\n"
		"			if (myNewWidth < this.mainMapObj.minWidth) {\n"
		"				var myYXFact = this.mainMapObj.curHeight / this.mainMapObj.curWidth;\n"
		"				myNewWidth = this.mainMapObj.minWidth;\n"
		"				myNewHeight = myNewWidth * myYXFact;\n"
		"				myNewX = parseFloat(myOldX) + myOldWidth / 2 - (myNewWidth * 0.5);\n"
		"				myNewY = parseFloat(myOldY) + myOldHeight / 2 - (myNewHeight * 0.5);\n"
		"			}\n"
		"			break;\n"
		"		case \"out\":\n"
		"			var myNewX = parseFloat(myOldX) + myOldWidth / 2 - (myOldWidth * (1 + this.mainMapObj.zoomFact) * 0.5);\n"
		"			var myNewY = parseFloat(myOldY) + myOldHeight / 2 - (myOldHeight * (1 + this.mainMapObj.zoomFact) * 0.5);\n"
		"			var myNewWidth = myOldWidth * (1 + this.mainMapObj.zoomFact);\n"
		"			var myNewHeight = myOldHeight * (1 + this.mainMapObj.zoomFact);\n"
		"			break;\n"
		"		default:\n"
		"			var myNewX = this.constrXmin;\n"
		"			var myNewY = this.constrYmin;\n"
		"			var myNewWidth = this.constrXmax - this.constrXmin;\n"
		"			var myNewHeight = this.constrYmax - this.constrYmin;\n"
		"			break;\n"
		"	}	\n"
		"	//check if within constraints\n"
		"	if (myNewWidth > (this.constrXmax - this.constrXmin)) {\n"
		"		myNewWidth = this.constrXmax - this.constrXmin;\n"
		"	}\n"
		"	if (myNewHeight > (this.constrYmax - this.constrYmin)) {\n"
		"		myNewHeight = this.constrYmax - this.constrYmin;\n"
		"	}\n"
		"	if (myNewX < this.constrXmin) {\n"
		"		myNewX = this.constrXmin;\n"
		"	}\n"
		"	if (myNewY < this.constrYmin) {\n"
		"		myNewY = this.constrYmin;\n"
		"	}\n"
		"	if ((myNewX + myNewWidth) > this.constrXmax) {\n"
		"		myNewX = this.constrXmax - myNewWidth;\n"
		"	}\n"
		"	if ((myNewY + myNewHeight) > this.constrYmax) {\n"
		"		myNewY = this.constrYmax - myNewHeight;\n"
		"	}\n"
		"	this.newView(myNewX,myNewY,myNewWidth,myNewHeight);\n"
		"	this.mainMapObj.newViewBox(this.dragId,true);\n"
		"}\n"
		"\n"
		"dragObj.prototype.pan = function (myX,myY,howmuch) {\n"
		"	//get values from draggable rectangle\n"
		"	var xulcorner = parseFloat(this.myDragger.getAttributeNS(null,\"x\")));\n"
		"	var yulcorner = parseFloat(this.myDragger.getAttributeNS(null,\"y\")));\n"
		"	var width = parseFloat(this.myDragger.getAttributeNS(null,\"width\")));\n"
		"	var height = parseFloat(this.myDragger.getAttributeNS(null,\"height\")));\n"
		"\n"
		"	//set values of draggable rectangle\n"
		"	var rectXulcorner = xulcorner + howmuch * width * myX;\n"
		"	var rectYulcorner = yulcorner + howmuch * height * myY;\n"
		"	//check if within constraints\n"
		"	if (rectXulcorner < this.constrXmin) {\n"
		"		rectXulcorner = this.constrXmin;\n"
		"	}\n"
		"	if (rectYulcorner < this.constrYmin) {\n"
		"		rectYulcorner = this.constrYmin;\n"
		"	}\n"
		"	if ((rectXulcorner + width) > this.constrXmax) {\n"
		"		rectXulcorner = this.constrXmax - width;\n"
		"	}\n"
		"	if ((rectYulcorner + height) > this.constrYmax) {\n"
		"		rectYulcorner = this.constrYmax - height;\n"
		"	}\n"
		"	this.newView(rectXulcorner,rectYulcorner,width,height);\n"
		"\n"
		"	//set viewport of main map\n"
		"	if ((xulcorner != rectXulcorner) || (yulcorner != rectYulcorner)) {\n"
		"		this.mainMapObj.newViewBox(this.dragId,true);\n"
		"	}\n"
		"\n"
		"	statusChange(\"map ready ...\"));\n"
		"}\n"
		"\n"
		"//remove all temporarily used elements and event listeners\n"
		"dragObj.prototype.cleanUp = function() {\n"
		"	//remove eventlisteners\n"
		"	if (this.showCoords == true) {\n"
		"		//add event listener for coordinate display\n"
		"		this.myRefMap.removeEventListener(\"mousemove\"),this,false);\n"
		"	}\n"
		"}\n"
		"\n"
		"function zoomIt(evt,inOrOut) {\n"
		"	if (evt.detail == 1) { //only react on first click, double click: second click is ignored\n"
		"		if (inOrOut == \"in\")) {\n"
		"			if (Math.round(myMainMap.curWidth) > myMainMap.minWidth) {\n"
		"				myMapApp.refMapDragger.zoom(\"in\"));\n"
		"			}\n"
		"			else {\n"
		"				statusChange(\"Maximum zoom factor reached. Cannot zoom in any more.\"));\n"
		"			}\n"
		"		}\n"
		"		if (inOrOut == \"out\")) {\n"
		"			if (Math.round(myMainMap.curWidth) < myMainMap.maxWidth) {\n"
		"				myMapApp.refMapDragger.zoom(\"out\"));\n"
		"			}\n"
		"			else {\n"
		"				statusChange(\"Minimum zoom factor reached. Cannot zoom out any more.\"));\n"
		"			}\n"
		"		}\n"
		"		if (inOrOut == \"full\")) {\n"
		"			if (Math.round(myMainMap.curWidth) < myMainMap.maxWidth) {\n"
		"				myMapApp.refMapDragger.zoom(\"full\"));\n"
		"			}\n"
		"			else {\n"
		"				statusChange(\"Full view already reached.\"));\n"
		"			}\n"
		"		}\n"
		"	}\n"
		"}\n"
		"\n"
		"//this function starts various zoom actions or map extent history functions\n"
		"function zoomImageButtons(id,evt) {\n"
		"	myMainMap.stopNavModes();\n"
		"	if (id == \"zoomIn\")) {\n"
		"		zoomIt(evt,'in');\n"
		"	}\n"
		"	if (id == \"zoomOut\")) {\n"
		"		zoomIt(evt,'out');\n"
		"	}\n"
		"	if (id == \"zoomFull\")) {\n"
		"		zoomIt(evt,'full');\n"
		"	}\n"
		"	if (id == \"backwardExtent\")) {\n"
		"		myMainMap.backwardExtent();\n"
		"	}\n"
		"	if (id == \"forwardExtent\")) {\n"
		"		myMainMap.forwardExtent();\n"
		"	}\n"
		"}\n"
		"\n"
		"//this function starts various interactive zoom or pan modes\n"
		"function zoomImageSwitchButtons(id,evt,onOrOff) {\n"
		"	myMainMap.stopNavModes();\n"
		"	if (onOrOff) {\n"
		"                      if (id == \"infoButton\")) {\n"
		"                      \n"
		"                      }\n"
		"                      else {\n"
		"		    if (id == \"zoomManual\")) {\n"
		"			    myMainMap.zoomManual(evt);\n"
		"		    }\n"
		"		    if (id == \"panManual\")) {\n"
		"			    myMainMap.panManual(evt);\n"
		"		    }\n"
		"		    if (id == \"recenterMap\")) {\n"
		"			    myMainMap.recenter(evt);\n"
		"		    }\n"
		"		    if (myMapApp.buttons[\"infoButton\"].getSwitchValue()) {\n"
		"		        myMapApp.buttons[\"infoButton\"].setSwitchValue(false,false);\n"
		"		    }\n"
		"		}\n"
		"	}\n"
		"	else {\n"
		"	    if (id == \"infoButton\")) {\n"
		"	        myMapApp.buttons[\"infoButton\"].setSwitchValue(true,false);\n"
		"	    }\n"
		"	}\n"
		"}\n"
		"\n"
		"//alert map extent\n"
		"function showExtent() {\n"
		"	with(myMainMap) {\n"
		"		alert(\n"
		"         \"Xmin=\"+curxOrig.toFixed(nrDecimals)+units+\n"
		"         \"); Xmax=\"+(curxOrig + curWidth).toFixed(nrDecimals)+units+\n"
		"         \"\\nYmin=\"+((curyOrig + curHeight) * -1).toFixed(nrDecimals) +units+\n"
		"         \"); Ymax=\"+(curyOrig*-1).toFixed(nrDecimals)+units+\n"
		"         \"\\nWidth=\"+curWidth.toFixed(nrDecimals)+units+\n"
		"         \"); Height=\"+curHeight.toFixed(nrDecimals)+units);\n"
		"	}\n"
		"}\n"
		"\n"
	));
}


///////////////////////////////////////////////////////////
//														 //
//														 //
//														 //
///////////////////////////////////////////////////////////

//---------------------------------------------------------

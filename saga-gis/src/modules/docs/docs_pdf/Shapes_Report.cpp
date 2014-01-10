/**********************************************************
 * Version $Id: Shapes_Report.cpp 1921 2014-01-09 10:24:11Z oconrad $
 *********************************************************/

///////////////////////////////////////////////////////////
//                                                       //
//                         SAGA                          //
//                                                       //
//      System for Automated Geoscientific Analyses      //
//                                                       //
//                    Module Library:                    //
//                     Shapes_Tools                      //
//                                                       //
//-------------------------------------------------------//
//                                                       //
//                   Shapes_Report.cpp                   //
//                                                       //
//                 Copyright (C) 2005 by                 //
//               Olaf Conrad, Victor Olaya               //
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
#include "doc_pdf.h"
#include "Shapes_Report.h"


///////////////////////////////////////////////////////////
//														 //
//														 //
//														 //
///////////////////////////////////////////////////////////

//---------------------------------------------------------
CShapes_Report::CShapes_Report(void)
{
	CSG_Parameter	*pNode;

	//-----------------------------------------------------
	Set_Name		(_TL("Shapes Report"));

	Set_Author		(SG_T("(c) 2005 by O.Conrad, V.Olaya"));

	Set_Description	(_TW(
		""
	));

	//-----------------------------------------------------
	pNode	= Parameters.Add_Shapes(
		NULL	, "SHAPES"		, _TL("Shapes"),
		_TL(""),
		PARAMETER_INPUT
	);

	Parameters.Add_Table_Field(
		pNode	, "SUBTITLE"	, _TL("Subtitle"),
		_TL("")
	);

	Parameters.Add_FilePath(
		NULL	, "FILENAME"	, _TL("PDF File"),
		_TL(""),
		_TL("PDF Files (*.pdf)|*.pdf|All Files|*.*"),
		_TL(""), true, false
	);

	Parameters.Add_Choice(
		NULL	, "PAPER_SIZE"	, _TL("Paper Format"),
		_TL(""),

		CSG_String::Format(SG_T("%s|%s|%s|%s|"),
			_TL("A4 Portrait"),
			_TL("A4 Landscape"),
			_TL("A3 Portrait"),
			_TL("A3 Landscape")
		)
	);

	Parameters.Add_Value(
		pNode	, "COLOR_LINE"	, _TL("Line Color"),
		_TL(""),
		PARAMETER_TYPE_Color, SG_COLOR_BLACK
	);

	Parameters.Add_Value(
		pNode	, "COLOR_FILL"	, _TL("Fill Color"),
		_TL(""),
		PARAMETER_TYPE_Color, SG_COLOR_GREEN
	);

	//-----------------------------------------------------
	pNode	= Parameters.Add_Node(NULL, "NODE_LAYOUT"	, _TL("Layout"), _TL(""));

	Parameters.Add_Choice(
		pNode	, "LAYOUT_MODE"	, _TL("Layout"),
		_TL(""),

		CSG_String::Format(SG_T("%s|%s|"),
			_TL("horizontal"),
			_TL("vertical")
		), 1
	);

	Parameters.Add_Value(
		pNode	, "LAYOUT_BREAK", _TL("Map/Table Size Ratio [%]"),
		_TL(""),
		PARAMETER_TYPE_Double, 50.0, 0.0, true, 100.0, true
	);

	//-----------------------------------------------------
	pNode	= Parameters.Add_Node(NULL, "NODE_TABLE"	, _TL("Attribute Table"), _TL(""));

	Parameters.Add_Value(
		pNode	, "COLUMNS"		, _TL("Columns"),
		_TL("Number of atttribute table columns."),
		PARAMETER_TYPE_Int, 2, 1, true
	);

	Parameters.Add_Choice(
		pNode	, "CELL_MODE"	, _TL("Cell Sizes"),
		_TL(""),

		CSG_String::Format(SG_T("%s|%s|"),
			_TL("fit to page"),
			_TL("fixed cell height")
		)
	);

	Parameters.Add_Value(
		pNode	, "CELL_HEIGHT"	, _TL("Cell Height"),
		_TL(""),
		PARAMETER_TYPE_Int, 8, 1, true
	);
}

//---------------------------------------------------------
CShapes_Report::~CShapes_Report(void)
{
}


///////////////////////////////////////////////////////////
//														 //
//														 //
//														 //
///////////////////////////////////////////////////////////

//---------------------------------------------------------
bool CShapes_Report::On_Execute(void)
{
	int				Page_Orientation;
	TSG_PDF_Page_Size	Page_Size;
	CSG_String		FileName(Parameters("FILENAME")	->asString());
	CSG_Doc_PDF	PDF;

	//-----------------------------------------------------
	m_pShapes		= Parameters("SHAPES")		->asShapes();
	m_iSubtitle		= Parameters("SUBTITLE")	->asInt();
	m_Color_Line	= Parameters("COLOR_LINE")	->asColor();
	m_Color_Fill	= Parameters("COLOR_FILL")	->asColor();
	m_nColumns		= Parameters("COLUMNS")		->asInt();

	switch( Parameters("CELL_MODE")->asInt() )
	{
	default:
	case 0:	m_Cell_Height	= 0;										break;
	case 1:	m_Cell_Height	= Parameters("CELL_HEIGHT")->asDouble();	break;
	}

	switch( Parameters("PAPER_SIZE")->asInt() )
	{
	default:
	case 0:	Page_Size	= PDF_PAGE_SIZE_A4;	Page_Orientation	= PDF_PAGE_ORIENTATION_PORTRAIT;	break;	// A4 Portrait
	case 1:	Page_Size	= PDF_PAGE_SIZE_A4;	Page_Orientation	= PDF_PAGE_ORIENTATION_LANDSCAPE;	break;	// A4 Landscape
	case 2:	Page_Size	= PDF_PAGE_SIZE_A3;	Page_Orientation	= PDF_PAGE_ORIENTATION_PORTRAIT;	break;	// A3 Portrait
	case 3:	Page_Size	= PDF_PAGE_SIZE_A3;	Page_Orientation	= PDF_PAGE_ORIENTATION_LANDSCAPE;	break;	// A3 Landscape
	}

	//-----------------------------------------------------
	if( FileName.Length() > 0 && PDF.Open(Page_Size, Page_Orientation, CSG_String::Format(SG_T("%s: %s"), _TL("Summary"), m_pShapes->Get_Name())) )
	{
		double	d	= Parameters("LAYOUT_BREAK")->asDouble();

		switch( Parameters("LAYOUT_MODE")->asInt() )
		{
		default:
		case 0:	// horizontal
			PDF.Layout_Add_Box(  0,   0, 100,   5, "TITLE");
			PDF.Layout_Add_Box(  0,   5,   d, 100, "SHAPE");
			PDF.Layout_Add_Box(  d,   5, 100, 100, "TABLE");
			break;

		case 1:	// vertical
			PDF.Layout_Add_Box(  0,   0, 100,   5, "TITLE");
			PDF.Layout_Add_Box(  0,   5, 100,   d, "SHAPE");
			PDF.Layout_Add_Box(  0,   d, 100, 100, "TABLE");
			break;
		}

		m_pPDF		= &PDF;

		m_rTitle	= PDF.Layout_Get_Box("TITLE");
		m_rShape	= PDF.Layout_Get_Box("SHAPE");
		m_rTable	= PDF.Layout_Get_Box("TABLE");
		
		return( Add_Shapes() && PDF.Save(FileName) );
	}

	return( false );
}


///////////////////////////////////////////////////////////
//														 //
//														 //
//														 //
///////////////////////////////////////////////////////////

//---------------------------------------------------------
bool CShapes_Report::Add_Shapes(void)
{
	if( m_pShapes && m_pShapes->is_Valid() && m_pPDF && m_pPDF->Add_Page() )
	{
		bool			bAddAll;
		CSG_Rect		r(m_rShape), rWorld(m_pShapes->Get_Extent());
		CSG_String		Title;
		CSG_Strings	sLeft, sRight;

		//-------------------------------------------------
		m_pPDF->Draw_Text(m_rTitle.Get_XCenter(), m_rTitle.Get_YCenter(), _TL("Overview"), (int)(0.7 * m_rTitle.Get_YRange()), PDF_STYLE_TEXT_ALIGN_H_CENTER|PDF_STYLE_TEXT_ALIGN_V_CENTER|PDF_STYLE_TEXT_UNDERLINE);

		//-------------------------------------------------
		r.Deflate(10, false);
		rWorld.Inflate(5, true);

		m_pPDF->Draw_Shapes   (r, m_pShapes, PDF_STYLE_POLYGON_FILLSTROKE, m_Color_Fill, m_Color_Line, 0, &rWorld);
		m_pPDF->Draw_Graticule(r, rWorld, 10);

		//-------------------------------------------------
		sLeft	.Add(CSG_String::Format(SG_T("%s:")	, _TL("Name")));
		sRight	.Add(CSG_String::Format(SG_T("%s")	, m_pShapes->Get_Name()));
		sLeft	.Add(CSG_String::Format(SG_T("%s:")	, _TL("Count")));
		sRight	.Add(CSG_String::Format(SG_T("%d")	, m_pShapes->Get_Count()));

		m_pPDF->Draw_Text(m_rTable.Get_XMin()   , m_rTable.Get_YMax(), sLeft , 8, PDF_STYLE_TEXT_ALIGN_H_LEFT|PDF_STYLE_TEXT_ALIGN_V_TOP);
		m_pPDF->Draw_Text(m_rTable.Get_XCenter(), m_rTable.Get_YMax(), sRight, 8, PDF_STYLE_TEXT_ALIGN_H_LEFT|PDF_STYLE_TEXT_ALIGN_V_TOP);

		//-------------------------------------------------
		bAddAll	= m_pShapes->Get_Selection_Count() == 0;

		for(int iShape=0; iShape<m_pShapes->Get_Count() && Set_Progress(iShape, m_pShapes->Get_Count()); iShape++)
		{
			if( bAddAll || m_pShapes->Get_Record(iShape)->is_Selected() )
			{
				Title.Printf(SG_T("%s: %s"), m_pShapes->Get_Field_Name(m_iSubtitle), m_pShapes->Get_Shape(iShape)->asString(m_iSubtitle));

				Add_Shape(m_pShapes->Get_Shape(iShape), Title);
			}
		}

		return( true );
	}

	return( false );
}

//---------------------------------------------------------
bool CShapes_Report::Add_Shape(CSG_Shape *pShape, const SG_Char *Title)
{
	if( m_pPDF && m_pPDF->Is_Ready_To_Draw() && pShape && pShape->is_Valid() && m_pPDF->Add_Page() )
	{
		CSG_Rect	r(m_rShape), rWorld(pShape->Get_Extent());

		//-------------------------------------------------
		m_pPDF->Add_Outline_Item(Title);

		m_pPDF->Draw_Text(m_rTitle.Get_XCenter(), m_rTitle.Get_YCenter(), Title, (int)(0.7 * m_rTitle.Get_YRange()), PDF_STYLE_TEXT_ALIGN_H_CENTER|PDF_STYLE_TEXT_ALIGN_V_CENTER|PDF_STYLE_TEXT_UNDERLINE);

		r.Deflate(10, false);
		rWorld.Inflate(5, true);

		m_pPDF->Draw_Shape		(r, pShape, PDF_STYLE_POLYGON_FILLSTROKE, m_Color_Fill, m_Color_Line, 0, &rWorld);
		m_pPDF->Draw_Graticule	(r, rWorld, 10);

		//-------------------------------------------------
		if( pShape->Get_Table()->Get_Field_Count() > 0 )
		{
			CSG_Table_Record	*pRecord;
			CSG_Table			Table;

			Table.Set_Name(_TL("Attributes"));

			Table.Add_Field(_TL("Attribute"), SG_DATATYPE_String);
			Table.Add_Field(_TL("Value")	, SG_DATATYPE_String);

			for(int iField=0; iField<pShape->Get_Table()->Get_Field_Count(); iField++)
			{
				pRecord	= Table.Add_Record();

				pRecord->Set_Value(0, pShape->Get_Table()->Get_Field_Name(iField));
				pRecord->Set_Value(1, pShape->asString(iField));
			}

			m_pPDF->Draw_Table(m_rTable, &Table, m_nColumns, m_Cell_Height, 0.0);
		}

		return( true );
	}

	return( false );
}

///////////////////////////////////////////////////////////
//														 //
//														 //
//														 //
///////////////////////////////////////////////////////////

/*/---------------------------------------------------------
void CWKSP_Map::SaveAs_PDF_Indexed(void)
{
	static CSG_Parameters	Parameters(NULL, LNG("[CAP] Save to PDF"), LNG(""), NULL, false);

	//-----------------------------------------------------
	if( Parameters.Get_Count() == 0 )
	{
		Parameters.Add_FilePath(
			NULL	, "FILENAME"	, LNG("[FLD] PDF Document"),
			LNG(""),
			CSG_String::Format(
				wxT("%s (*.pdf)|*pdf|")
				wxT("%s|*.*"),
				LNG("PDF Files"),
				LNG("All Files")
			), NULL, true, false
		);

		Parameters.Add_Choice(
			NULL	, "PAPER_SIZE"	, LNG("[FLD] Paper Format"),
			LNG(""),

			CSG_String::Format(wxT("%s|%s|%s|%s|"),
				LNG("A4 Portrait"),
				LNG("A4 Landscape"),
				LNG("A3 Portrait"),
				LNG("A3 Landscape")
			)
		);

		Parameters.Add_String(
			NULL	, "NAME"		, LNG("[FLD] Title"),
			LNG(""),
			LNG("")
		);

		Parameters.Add_Shapes(
			NULL	, "SHAPES"		, LNG("[FLD] Shapes"),
			LNG(""),
			PARAMETER_INPUT_OPTIONAL
		);

		Parameters.Add_Table_Field(
			Parameters("SHAPES")	, "FIELD"		, LNG("[FLD] Attribute"),
			LNG("")
		);

	//	Parameters.Add_Grid(
	//		NULL	, "GRID"		, LNG("[FLD] Grid"),
	//		LNG(""),
	//		PARAMETER_INPUT_OPTIONAL
	//	);

		Parameters.Add_FilePath(
			NULL	, "FILEICON"	, LNG("[FLD] Icon"),
			LNG(""),

			CSG_String::Format(wxT("%s|*.png;*.jpg|%s|*.png|%s|*.jpg|%s|*.*"),
				LNG("All Recognised Files"),
				LNG("PNG Files"),
				LNG("JPG Files"),
				LNG("All Files")
			), NULL, false, false
		);

		Parameters.Add_Value(
			NULL	, "ROUNDSCALE"	, LNG("[FLD] Round Scale"),
			LNG(""),
			PARAMETER_TYPE_Bool, true
		);
	}

	//-----------------------------------------------------
	if( DLG_Parameters(&Parameters) )
	{
		bool			bResult, bRoundScale;
		int				iField;
		CSG_String		Name, FileName, FileName_Icon, FilePath_Maps;
		CSG_Rect		rOverview, rMap;
		CSG_Shapes			*pShapes;
	//	CSG_Grid			*pGrid;
		CSG_Doc_PDF	PDF;

		MSG_General_Add(CSG_String::Format(wxT("%s..."), LNG("[MSG] Save to PDF")), true, true);

		bResult			= false;
		Name			= Parameters("NAME")		->asString();	if( Name.Length() < 1 )	Name	=  LNG("Maps");
		FileName		= Parameters("FILENAME")	->asString();
		FileName_Icon	= Parameters("FILEICON")	->asString();
		pShapes			= Parameters("SHAPES")		->asShapes();
	//	pGrid			= Parameters("GRID")		->asGrid();
		iField			= Parameters("FIELD")		->asInt();
		bRoundScale		= Parameters("ROUNDSCALE")	->asBool();

		switch( Parameters("PAPER_SIZE")->asInt() )
		{
		default:
		case 0:	PDF.Open(PDF_PAGE_SIZE_A4, PDF_PAGE_ORIENTATION_PORTRAIT , Name);	break;	// A4 Portrait
		case 1:	PDF.Open(PDF_PAGE_SIZE_A4, PDF_PAGE_ORIENTATION_LANDSCAPE, Name);	break;	// A4 Landscape
		case 2:	PDF.Open(PDF_PAGE_SIZE_A3, PDF_PAGE_ORIENTATION_PORTRAIT , Name);	break;	// A3 Portrait
		case 3:	PDF.Open(PDF_PAGE_SIZE_A3, PDF_PAGE_ORIENTATION_LANDSCAPE, Name);	break;	// A3 Landscape
		}

		//-------------------------------------------------
		if( PDF.Is_Open() )
		{
			PDF.Layout_Set_Box_Space(5, false);

			PDF.Layout_Add_Box(  0.0,   0.0, 100.0,  75.0, "MAP");
			PDF.Layout_Add_Box(  0.0,  75.0,  50.0, 100.0, "DIVISIONS");
			PDF.Layout_Add_Box( 50.0,  75.0,  60.0,  80.0, "ICON");
			PDF.Layout_Add_Box( 60.0,  75.0, 100.0,  80.0, "TITLE");
			PDF.Layout_Add_Box( 50.0,  80.0, 100.0, 100.0, "DESCRIPTION");

			FilePath_Maps	= SG_File_Make_Path(SG_File_Get_Path(FileName), SG_File_Get_Name(FileName, false));
			rOverview		= pShapes ? pShapes->Get_Extent() : Get_Extent();
		//	rOverview		= pShapes ? pShapes->Get_Extent() : (pGrid ? pGrid->Get_Extent() : Get_Extent());

			//---------------------------------------------
		//	PDF.Draw_Text		(PDF.Layout_Get_Box("TITLE").Get_XMin(), PDF.Layout_Get_Box("TITLE").Get_YCenter(), LNG("This is a Test!!!"), 24);
		//	PDF.Draw_Rectangle	(PDF.Layout_Get_Box("DIVISIONS"));
		//	PDF.Draw_Grid		(PDF.Layout_Get_Box("DIVISIONS"), Parameters("GRID")->asGrid(), CSG_Colors(), 0.0, 0.0, 0, &rOverview);
		//	PDF.Draw_Shapes		(PDF.Layout_Get_Box("DIVISIONS"), pShapes, PDF_STYLE_POLYGON_STROKE, SG_COLOR_GREEN, SG_COLOR_BLACK, 1, &rOverview);
		//	PDF.Draw_Graticule	(PDF.Layout_Get_Box("DIVISIONS"), rOverview);

			//---------------------------------------------
			Draw_PDF(&PDF, FilePath_Maps, -1, FileName_Icon, Name, rOverview, bRoundScale, iField, pShapes);

			if( pShapes )
			{
				for(int i=0; i<pShapes->Get_Count() && SG_UI_Process_Set_Progress(i, pShapes->Get_Count()); i++)
				{
					Draw_PDF(&PDF, FilePath_Maps, i, FileName_Icon, Name, pShapes->Get_Shape(i)->Get_Extent(), bRoundScale, iField, pShapes);
				}
			}

			//---------------------------------------------
			PROCESS_Set_Okay(true);
			Set_Buisy_Cursor(true);
			bResult	= PDF.Save(FileName);
			Set_Buisy_Cursor(false);
		}

		MSG_General_Add(bResult ? LNG("[MSG] okay") : LNG("[MSG] failed"), false, false, bResult ? SG_UI_MSG_STYLE_SUCCESS : SG_UI_MSG_STYLE_FAILURE);
	}
}

//---------------------------------------------------------
void CWKSP_Map::Draw_PDF(CSG_Doc_PDF *pPDF, const wxChar *FilePath_Maps, int Image_ID, const wxChar *FileName_Icon, const wxChar *Title, CSG_Rect rWorld, bool bRoundScale, int iField, CSG_Shapes *pShapes)
{
	int			FrameSize_1	= 20, FrameSize_2	= 10;
	double		d, e, Scale, Ratio;
	CSG_String	FileName, Description, s;
	CSG_Rect	rBox;
	wxRect		rBMP;
	wxBitmap	BMP;
	wxMemoryDC	dc;

	if( pPDF && rWorld.Get_XRange() > 0.0 && rWorld.Get_YRange() > 0.0 && pPDF->Add_Page() )
	{
		rWorld.Inflate(5.0, true);

		//-------------------------------------------------
		rBox	= pPDF->Layout_Get_Box("MAP");
		rBox.Deflate(FrameSize_1, false);

		rBMP	= wxRect(0, 0, (int)rBox.Get_XRange(), (int)rBox.Get_YRange());
		BMP.Create(rBMP.GetWidth(), rBMP.GetHeight());
		dc.SelectObject(BMP);
		dc.SetBackground(*wxWHITE_BRUSH);
		dc.Clear();

		if( bRoundScale )
		{
			Scale	= rWorld.Get_XRange() / (pPDF->Get_Page_To_Meter() * (double)rBMP.GetWidth());

		//	if( Scale > 1000 )
		//	{
				Ratio	= ((ceil(Scale / 1000.)) / (Scale / 1000.) - 1);
				rWorld.Inflate(Ratio * 100, true);
		//	}
		}

		Scale	= rWorld.Get_XRange() / (pPDF->Get_Page_To_Meter() * rBMP.GetWidth());

		Draw_Map(dc, rWorld, 1.0, rBMP, false);
		dc.SelectObject(wxNullBitmap);
		SG_Dir_Create(FilePath_Maps);
		FileName	= SG_File_Make_Path(FilePath_Maps, CSG_String::Format(wxT("image_%03d"), Image_ID + 1), wxT("png"));
		BMP.SaveFile(FileName.c_str(), wxBITMAP_TYPE_PNG);

		pPDF->Draw_Image	(rBox, FileName);
		pPDF->Draw_Graticule(rBox, rWorld, FrameSize_1);

		//-------------------------------------------------
		rBox	= pPDF->Layout_Get_Box("ICON");

		if( FileName_Icon )
		{
			pPDF->Draw_Image(rBox, FileName_Icon);
		}
		else
		{
			pPDF->Draw_Rectangle(rBox);
		}

		//-------------------------------------------------
		rBox	= pPDF->Layout_Get_Box("DIVISIONS");

		pPDF->Draw_Rectangle(rBox);

		if( pShapes )
		{
			CSG_Rect	rShapes(pShapes->Get_Extent());

			rShapes.Inflate(5.0, true);
			rBox.Deflate(FrameSize_2, false);

			pPDF->Draw_Graticule(rBox, rShapes, FrameSize_2);
			pPDF->Draw_Shapes(rBox, pShapes, PDF_STYLE_POLYGON_FILLSTROKE, SG_COLOR_GREEN, SG_COLOR_BLACK, 0, &rShapes);

			if( Image_ID >= 0 && Image_ID < pShapes->Get_Count() )
			{
				pPDF->Draw_Shape(rBox, pShapes->Get_Shape(Image_ID), PDF_STYLE_POLYGON_FILLSTROKE, SG_COLOR_YELLOW, SG_COLOR_RED, 1, &rShapes);
			}
		}

		//-------------------------------------------------
		rBox	= pPDF->Layout_Get_Box("TITLE");

		pPDF->Draw_Text(rBox.Get_XMin(), rBox.Get_YCenter(), Title, 20, PDF_STYLE_TEXT_ALIGN_H_LEFT|PDF_STYLE_TEXT_ALIGN_V_CENTER|PDF_STYLE_TEXT_UNDERLINE);

		//-------------------------------------------------
		rBox	= pPDF->Layout_Get_Box("DESCRIPTION");

		Description.Append(CSG_String::Format(wxT("%d. %s\n"), Image_ID + 2, LNG("Map")));

		if( pShapes && Image_ID >= 0 && Image_ID < pShapes->Get_Count() )
		{
			switch( pShapes->Get_Type() )
			{
			default:
				break;

			case SHAPE_TYPE_Line:
				d	= ((CSG_Shape_Line    *)pShapes->Get_Shape(Image_ID))->Get_Length();
				e	= d > 1000.0 ? 1000.0    : 1.0;
				s	= d > 1000.0 ? wxT("km") : wxT("m");
				Description.Append(CSG_String::Format(wxT("%s: %f%s\n"), LNG("Length")	, d / e, s.c_str()));
				break;

			case SHAPE_TYPE_Polygon:
				d	= ((CSG_Shape_Polygon *)pShapes->Get_Shape(Image_ID))->Get_Area();
				e	= d > 1000000.0 ? 1000000.0  : (d > 10000.0 ? 10000.0   : 1.0);
				s	= d > 1000000.0 ? wxT("km\xc2\xb2") : (d > 10000.0 ? wxT("ha") : wxT("m\xc2\xb2"));
				Description.Append(CSG_String::Format(wxT("%s: %f%s\n"), LNG("Area")		, d / e, s.c_str()));

				d	= ((CSG_Shape_Polygon *)pShapes->Get_Shape(Image_ID))->Get_Perimeter();
				e	= d > 1000.0 ? 1000.0    : 1.0;
				s	= d > 1000.0 ? wxT("km") : wxT("m");
				Description.Append(CSG_String::Format(wxT("%s: %f%s\n"), LNG("Perimeter")	, d / e, s.c_str()));

				Description.Append(CSG_String::Format(wxT("%s: %d\n")  , LNG("Parts")		, ((CSG_Shape_Polygon *)pShapes->Get_Shape(Image_ID))->Get_Part_Count()));
				break;
			}

			if( iField >= 0 && iField < pShapes->Get_Field_Count() )
			{
				Description.Append(CSG_String::Format(wxT("%s: %s\n"), pShapes->Get_Field_Name(iField), pShapes->Get_Shape(Image_ID)->asString(iField)));
			}
		}

		Description.Append(CSG_String::Format(wxT("%s 1:%s"), LNG("Scale"), SG_Get_String(Scale, 2).c_str()));

		pPDF->Draw_Text(rBox.Get_XMin(), rBox.Get_YMax(), Description, 12, PDF_STYLE_TEXT_ALIGN_H_LEFT|PDF_STYLE_TEXT_ALIGN_V_TOP);
	}
}/**/


///////////////////////////////////////////////////////////
//														 //
//														 //
//														 //
///////////////////////////////////////////////////////////

//---------------------------------------------------------

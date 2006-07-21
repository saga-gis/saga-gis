
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
#include <saga_api/doc_pdf.h>
#include "Shapes_Report.h"


///////////////////////////////////////////////////////////
//														 //
//														 //
//														 //
///////////////////////////////////////////////////////////

//---------------------------------------------------------
CShapes_Report::CShapes_Report(void)
{
	CParameter	*pNode;

	//-----------------------------------------------------
	Set_Name	(_TL("Create PDF Report for Shapes Layer"));

	Set_Author	("Olaf Conrad, Victor Olaya");

	Set_Description(
		_TL("")
	);

	//-----------------------------------------------------
	pNode	= Parameters.Add_Shapes(
		NULL	, "SHAPES"		, _TL("Shapes"),
		"",
		PARAMETER_INPUT
	);

	Parameters.Add_Table_Field(
		pNode	, "SUBTITLE"	, _TL("Subtitle"),
		""
	);

	Parameters.Add_FilePath(
		NULL	, "FILENAME"	, _TL("PDF File"),
		"",
		_TL("PDF Files (*.pdf)|*.pdf|All Files|*.*"),
		"", true, false
	);

	Parameters.Add_Choice(
		NULL	, "PAPER_SIZE"	, _TL("Paper Format"),
		_TL(""),

		_TL("A4 Portrait|"
			"A4 Landscape|"
			"A3 Portrait|"
			"A3 Landscape|"	)
	);

	Parameters.Add_Value(
		pNode	, "COLOR_LINE"	, _TL("Line Color"),
		_TL(""),
		PARAMETER_TYPE_Color, COLOR_DEF_BLACK
	);

	Parameters.Add_Value(
		pNode	, "COLOR_FILL"	, _TL("Fill Color"),
		_TL(""),
		PARAMETER_TYPE_Color, COLOR_DEF_GREEN
	);

	//-----------------------------------------------------
	pNode	= Parameters.Add_Node(NULL, "NODE_LAYOUT"	, _TL("Layout"), _TL(""));

	Parameters.Add_Choice(
		pNode	, "LAYOUT_MODE"	, _TL("Layout"),
		_TL(""),

		_TL("horizontal|"
			"vertical|"	), 1
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

		_TL("fit to page|"
			"fixed cell height|"	)
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
	TPDF_Page_Size	Page_Size;
	CSG_String		FileName(Parameters("FILENAME")	->asString());
	CDoc_PDF	PDF;

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
	if( FileName.Length() > 0 && PDF.Open(Page_Size, Page_Orientation, CSG_String::Format("%s: %s", _TL("Summary"), m_pShapes->Get_Name())) )
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
		sLeft	.Add(CSG_String::Format("%s:"	, _TL("Name")));
		sRight	.Add(CSG_String::Format("%s"	, m_pShapes->Get_Name()));
		sLeft	.Add(CSG_String::Format("%s:"	, _TL("Count")));
		sRight	.Add(CSG_String::Format("%d"	, m_pShapes->Get_Count()));

		m_pPDF->Draw_Text(m_rTable.Get_XMin()   , m_rTable.Get_YMax(), sLeft , 8, PDF_STYLE_TEXT_ALIGN_H_LEFT|PDF_STYLE_TEXT_ALIGN_V_TOP);
		m_pPDF->Draw_Text(m_rTable.Get_XCenter(), m_rTable.Get_YMax(), sRight, 8, PDF_STYLE_TEXT_ALIGN_H_LEFT|PDF_STYLE_TEXT_ALIGN_V_TOP);

		//-------------------------------------------------
		bAddAll	= m_pShapes->Get_Selection_Count() == 0;

		for(int iShape=0; iShape<m_pShapes->Get_Count() && Set_Progress(iShape, m_pShapes->Get_Count()); iShape++)
		{
			if( bAddAll || m_pShapes->Get_Table().Get_Record(iShape)->is_Selected() )
			{
				Title.Printf("%s: %s", m_pShapes->Get_Table().Get_Field_Name(m_iSubtitle), m_pShapes->Get_Shape(iShape)->Get_Record()->asString(m_iSubtitle));

				Add_Shape(m_pShapes->Get_Shape(iShape), Title);
			}
		}

		return( true );
	}

	return( false );
}

//---------------------------------------------------------
bool CShapes_Report::Add_Shape(CShape *pShape, const char *Title)
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
		if( pShape->Get_Record()->Get_Owner()->Get_Field_Count() > 0 )
		{
			CTable_Record	*pRecord;
			CTable			Table;

			Table.Set_Name(_TL("Attributes"));

			Table.Add_Field(_TL("Attribute"), TABLE_FIELDTYPE_String);
			Table.Add_Field(_TL("Value")	, TABLE_FIELDTYPE_String);

			for(int iField=0; iField<pShape->Get_Record()->Get_Owner()->Get_Field_Count(); iField++)
			{
				pRecord	= Table.Add_Record();

				pRecord->Set_Value(0, pShape->Get_Record()->Get_Owner()->Get_Field_Name(iField));
				pRecord->Set_Value(1, pShape->Get_Record()->asString(iField));
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

//---------------------------------------------------------


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
//                    WKSP_Grid.cpp                      //
//                                                       //
//          Copyright (C) 2005 by Olaf Conrad            //
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
#include <wx/dc.h>
#include <wx/dcmemory.h>
#include <wx/image.h>
#include <wx/filename.h>

#include "res_commands.h"
#include "res_dialogs.h"

#include "helper.h"
#include "dc_helper.h"

#include "active.h"
#include "active_attributes.h"

#include "wksp_map_control.h"

#include "wksp_layer_classify.h"
#include "wksp_layer_legend.h"
#include "wksp_grid.h"

#include "view_scatterplot.h"


///////////////////////////////////////////////////////////
//														 //
//														 //
//														 //
///////////////////////////////////////////////////////////

//---------------------------------------------------------
CWKSP_Grid::CWKSP_Grid(CGrid *pGrid)
	: CWKSP_Layer(pGrid)
{
	m_pGrid		= pGrid;

	m_Sel_xN	= -1;

	Create_Parameters();
}

//---------------------------------------------------------
CWKSP_Grid::~CWKSP_Grid(void)
{
}


///////////////////////////////////////////////////////////
//														 //
//														 //
//														 //
///////////////////////////////////////////////////////////

//---------------------------------------------------------
wxString CWKSP_Grid::Get_Name(void)
{
	return( wxString::Format("%02d. %s", 1 + Get_ID(), m_pGrid->Get_Name()) );
}

//---------------------------------------------------------
wxString CWKSP_Grid::Get_Description(void)
{
	wxString	s;

	//-----------------------------------------------------
	s.Append(wxString::Format("<b>%s</b><table border=\"0\">",
		LNG("[CAP] Grid")
	));

	s.Append(wxString::Format("<tr><td>%s</td><td>%s</td></tr>",
		LNG("[CAP] Name")					, m_pGrid->Get_Name()
	));

	s.Append(wxString::Format("<tr><td>%s</td><td>%s</td></tr>",
		LNG("[CAP] File")					, m_pGrid->Get_File_Path()
	));

	s.Append(wxString::Format("<tr><td>%s</td><td>%d (x) * %d (y) = %ld</td></tr>",
		LNG("[CAP] Number of cells")		, m_pGrid->Get_NX(), m_pGrid->Get_NY(), m_pGrid->Get_NCells()
	));

	s.Append(wxString::Format("<tr><td>%s</td><td>%f</td></tr>",
		LNG("[CAP] Cell size")				, m_pGrid->Get_Cellsize()
	));

	s.Append(wxString::Format("<tr><td>%s</td><td>[%f] - [%f] = [%f]</td></tr>",
		LNG("[CAP] East/West")				, m_pGrid->Get_XMin(), m_pGrid->Get_XMax(), m_pGrid->Get_XRange()
	));

	s.Append(wxString::Format("<tr><td>%s</td><td>[%f] - [%f] = [%f]</td></tr>",
		LNG("[CAP] South/North")			, m_pGrid->Get_YMin(), m_pGrid->Get_YMax(), m_pGrid->Get_YRange()
	));

	s.Append(wxString::Format("<tr><td>%s</td><td>%s</td></tr>",
		LNG("[CAP] Value Type")				, GRID_TYPE_NAMES[m_pGrid->Get_Type()]
	));

	s.Append(wxString::Format("<tr><td>%s</td><td>[%f] - [%f] = [%f]</td></tr>",
		LNG("[CAP] Value Range")			, m_pGrid->Get_ZMin(), m_pGrid->Get_ZMax(), m_pGrid->Get_ZRange()
	));

	s.Append(wxString::Format("<tr><td>%s</td><td>%f</td></tr>",
		LNG("[CAP] Arithmetic Mean")		, m_pGrid->Get_ArithMean(true)
	));

	s.Append(wxString::Format("<tr><td>%s</td><td>%f</td></tr>",
		LNG("[CAP] Standard Deviation")		, sqrt(m_pGrid->Get_Variance(true))
	));

	s.Append(wxString::Format("<tr><td>%s</td><td>%fMB</td></tr>",
		LNG("[CAP] Memory Size")			, (double)(m_pGrid->Get_NCells() * m_pGrid->Get_nValueBytes()) / N_MEGABYTE_BYTES
	));

	if( m_pGrid->is_Compressed() )
	{
		s.Append(wxString::Format("<tr><td>%s</td><td>%f%%</td></tr>",
			LNG("[CAP] Memory Compression")	, 100.0 * m_pGrid->Get_Compression_Ratio()
		));
	}

	if( m_pGrid->is_Cached() )
	{
		s.Append(wxString::Format("<tr><td>%s</td><td>%s = %fmb</td></tr>",
			LNG("[CAP] File cache activated")	, LNG("buffer size"), m_pGrid->Get_Buffer_Size() / (double)N_MEGABYTE_BYTES
		));
	}

	s.Append("</table>");

	//-----------------------------------------------------
	s.Append(wxString::Format("<hr><b>%s</b><font size=\"-1\">", LNG("[CAP] Data History")));
	s.Append(m_pGrid->Get_History().Get_HTML());
	s.Append(wxString::Format("</font"));

	return( s );
}

//---------------------------------------------------------
wxMenu * CWKSP_Grid::Get_Menu(void)
{
	wxMenu	*pMenu;

	pMenu	= new wxMenu(m_pGrid->Get_Name());

	CMD_Menu_Add_Item(pMenu, false, ID_CMD_WKSP_ITEM_CLOSE);
	CMD_Menu_Add_Item(pMenu, false, ID_CMD_GRIDS_SAVE);
	CMD_Menu_Add_Item(pMenu, false, ID_CMD_GRIDS_SAVEAS);
	CMD_Menu_Add_Item(pMenu, false, ID_CMD_GRIDS_SAVEAS_IMAGE);
	CMD_Menu_Add_Item(pMenu, false, ID_CMD_GRIDS_SHOW);

	pMenu->AppendSeparator();

	CMD_Menu_Add_Item(pMenu, true , ID_CMD_GRIDS_HISTOGRAM);

	CMD_Menu_Add_Item(pMenu, false, ID_CMD_GRIDS_SCATTERPLOT);
	CMD_Menu_Add_Item(pMenu, false, ID_CMD_GRIDS_EQUALINTERVALS);

	return( pMenu );
}


///////////////////////////////////////////////////////////
//														 //
//														 //
//														 //
///////////////////////////////////////////////////////////

//---------------------------------------------------------
bool CWKSP_Grid::On_Command(int Cmd_ID)
{
	wxString	File_Path;

	switch( Cmd_ID )
	{
	default:
		return( CWKSP_Layer::On_Command(Cmd_ID) );

	case ID_CMD_GRIDS_SAVEAS_IMAGE:
		_Save_Image();
		break;

	case ID_CMD_GRIDS_HISTOGRAM:
		Histogram_Toggle();
		break;

	case ID_CMD_GRIDS_SCATTERPLOT:
		Add_ScatterPlot(Get_Grid());
		break;

	case ID_CMD_GRIDS_EQUALINTERVALS:
		m_pClassify->Metric2EqualElements();
		break;
	}

	return( true );
}

//---------------------------------------------------------
bool CWKSP_Grid::On_Command_UI(wxUpdateUIEvent &event)
{
	switch( event.GetId() )
	{
	default:
		return( CWKSP_Layer::On_Command_UI(event) );

	case ID_CMD_GRIDS_HISTOGRAM:
		event.Check(m_pHistogram != NULL);
		break;
	}

	return( true );
}


///////////////////////////////////////////////////////////
//														 //
//														 //
//														 //
///////////////////////////////////////////////////////////

//---------------------------------------------------------
void CWKSP_Grid::On_Create_Parameters(void)
{
	//-----------------------------------------------------
	// General...

	m_Parameters.Add_String(
		m_Parameters("NODE_GENERAL")	, "GENERAL_Z_UNIT"			, LNG("[CAP] Unit"),
		"",
		m_pGrid->Get_Unit()
	);

	m_Parameters.Add_Value(
		m_Parameters("NODE_GENERAL")	, "GENERAL_Z_FACTOR"		, LNG("[CAP] Z-Factor"),
		"",
		PARAMETER_TYPE_Double
	);

	m_Parameters.Add_Range(
		m_Parameters("NODE_GENERAL")	, "GENERAL_Z_NODATA"		, LNG("[CAP] No Data"),
		""
	);


	//-----------------------------------------------------
	// Memory...

	m_Parameters.Add_Node(
		NULL							, "NODE_MEMORY"				, LNG("[CAP] Memory"),
		""
	);

	m_Parameters.Add_Choice(
		m_Parameters("NODE_MEMORY")		, "MEMORY_MODE"				, LNG("[CAP] Memory Handling"),
		"",
		wxString::Format("%s|%s|%s|",
			LNG("[VAL] Normal"),
			LNG("[VAL] RTL Compression"),
			LNG("[VAL] File Cache")
		), 0
	);

	m_Parameters.Add_Value(
		m_Parameters("NODE_MEMORY")		, "MEMORY_BUFFER_SIZE"		, LNG("[CAP] Buffer Size MB"),
		"",
		PARAMETER_TYPE_Double
	);


	//-----------------------------------------------------
	// Display...

	m_Parameters.Add_Value(
		m_Parameters("NODE_DISPLAY")	, "DISPLAY_TRANSPARENCY"	, LNG("[CAP] Transparency [%]"),
		"",
		PARAMETER_TYPE_Double, 0.0, 0.0, true, 100.0, true
	);

	m_Parameters.Add_Choice(
		m_Parameters("NODE_DISPLAY")	, "DISPLAY_INTERPOLATION"	, LNG("[CAP] Interpolation"),
		"",
		wxString::Format("%s|%s|%s|%s|%s|",
			LNG("[VAL] None"),
			LNG("[VAL] Bilinear"),
			LNG("[VAL] Inverse Distance"),
			LNG("[VAL] Bicubic Spline"),
			LNG("[VAL] B-Spline")
		), 0
	);


	//-----------------------------------------------------
	// Classification...

	((CParameter_Choice *)m_Parameters("COLORS_TYPE")->Get_Data())->Set_Items(
		wxString::Format("%s|%s|%s|%s|%s|",
			LNG("[VAL] Unique Symbol"),
			LNG("[VAL] Lookup Table"),
			LNG("[VAL] Graduated Color"),
			LNG("[VAL] RGB"),
			LNG("[VAL] Shade")
		)
	);

	m_Parameters("COLORS_TYPE")->Set_Value(CLASSIFY_METRIC);


	//-----------------------------------------------------
	// Cell Values...

	m_Parameters.Add_Node(
		NULL							, "NODE_VALUES"		, LNG("[CAP] Display: Cell Values"),
		""
	);

	m_Parameters.Add_Value(
		m_Parameters("NODE_VALUES")		, "VALUES_SHOW"		, LNG("[CAP] Show"),
		"",
		PARAMETER_TYPE_Bool, true
	);

	m_Parameters.Add_Font(
		m_Parameters("NODE_VALUES")		, "VALUES_FONT"		, LNG("[CAP] Font"),
		""
	)->asFont()->SetFamily(wxDECORATIVE);

	m_Parameters.Add_Value(
		m_Parameters("NODE_VALUES")		, "VALUES_SIZE"		, LNG("[CAP] Relative Font Size"),
		"",
		PARAMETER_TYPE_Double, 15, 0, true , 100.0, true
	);

	m_Parameters.Add_Value(
		m_Parameters("NODE_VALUES")		, "VALUES_DECIMALS"	, LNG("[CAP] Decimals"),
		"",
		PARAMETER_TYPE_Int, 2
	);


	//-----------------------------------------------------
	CParameters	Parameters;

	Parameters.Add_Range(NULL, "METRIC_ZRANGE"	, "", "", m_pGrid->Get_ZMin(true), m_pGrid->Get_ZMax(true));
	PROCESS_Set_Okay(true);

	DataObject_Changed(&Parameters);
}

//---------------------------------------------------------
void CWKSP_Grid::On_DataObject_Changed(void)
{
	//-----------------------------------------------------
	m_Parameters("GENERAL_Z_UNIT")			->Set_Value((void *)m_pGrid->Get_Unit());
	m_Parameters("GENERAL_Z_FACTOR")		->Set_Value(m_pGrid->Get_ZFactor());

	m_Parameters("GENERAL_Z_NODATA")->asRange()->Set_Range(
		m_pGrid->Get_NoData_Value(),
		m_pGrid->Get_NoData_hiValue()
	);

	//-----------------------------------------------------
	m_Parameters("MEMORY_MODE")				->Set_Value(
		m_pGrid->is_Compressed() ? 1 : (m_pGrid->is_Cached() ? 2 : 0)
	);

	m_Parameters("MEMORY_BUFFER_SIZE")		->Set_Value(
		(double)m_pGrid->Get_Buffer_Size() / N_MEGABYTE_BYTES
	);
}

//---------------------------------------------------------
void CWKSP_Grid::On_Parameters_Changed(void)
{
	//-----------------------------------------------------
	m_pGrid->Set_Unit		(m_Parameters("GENERAL_Z_UNIT")		->asString());
	m_pGrid->Set_ZFactor	(m_Parameters("GENERAL_Z_FACTOR")	->asDouble());

	m_pGrid->Set_NoData_Value_Range(
		m_Parameters("GENERAL_Z_NODATA")->asRange()->Get_LoVal(),
		m_Parameters("GENERAL_Z_NODATA")->asRange()->Get_HiVal()
	);

	//-----------------------------------------------------
	switch( m_Parameters("MEMORY_MODE")->asInt() )
	{
	case 0:
		if( m_pGrid->is_Compressed() )
		{
			m_pGrid->Set_Compression(false);
		}
		else if( m_pGrid->is_Cached() )
		{
			m_pGrid->Set_Cache(false);
		}
		break;

	case 1:
		if( !m_pGrid->is_Compressed() )
		{
			m_pGrid->Set_Compression(true);
		}
		break;

	case 2:
		if( !m_pGrid->is_Cached() )
		{
			m_pGrid->Set_Cache(true);
		}
		break;
	}
}


///////////////////////////////////////////////////////////
//														 //
//														 //
//														 //
///////////////////////////////////////////////////////////

//---------------------------------------------------------
wxString CWKSP_Grid::Get_Value(CGEO_Point ptWorld, double Epsilon)
{
	double		Value;
	wxString	s;

	if( m_pGrid->Get_Value(ptWorld, Value, GRID_INTERPOLATION_NearestNeighbour, true) )
	{
		switch( m_pClassify->Get_Mode() )
		{
		case CLASSIFY_LUT:
			s	= m_pClassify->Get_Class_Name_byValue(Value);
			break;

		case CLASSIFY_METRIC:	default:
		case CLASSIFY_SHADE:
			switch( m_pGrid->Get_Type() )
			{
			case GRID_TYPE_Byte:	default:
			case GRID_TYPE_Char:
			case GRID_TYPE_Word:
			case GRID_TYPE_Short:
			case GRID_TYPE_DWord:
			case GRID_TYPE_Int:
			case GRID_TYPE_Long:
				s.Printf("%d%s", (int)Value, m_pGrid->Get_Unit());
				break;

			case GRID_TYPE_Float:
			case GRID_TYPE_Double:
				s.Printf("%f%s", Value, m_pGrid->Get_Unit());
				break;
			}
			break;

		case CLASSIFY_RGB:
			s.Printf("R%03d G%03d B%03d", COLOR_GET_R((int)Value), COLOR_GET_G((int)Value), COLOR_GET_B((int)Value));
			break;
		}
	}

	return( s );
}

//---------------------------------------------------------
double CWKSP_Grid::Get_Value_Range(void)
{
	return( m_pGrid->Get_ZRange() );
}


///////////////////////////////////////////////////////////
//														 //
//														 //
//														 //
///////////////////////////////////////////////////////////

//---------------------------------------------------------
bool CWKSP_Grid::On_Edit_On_Key_Down(int KeyCode)
{
	switch( KeyCode )
	{
	default:
		return( false );

	case WXK_DELETE:
		return( _Edit_Del_Selection() );
	}
}

//---------------------------------------------------------
#define SELECTION_MAX	20

//---------------------------------------------------------
bool CWKSP_Grid::On_Edit_On_Mouse_Up(CGEO_Point Point, double ClientToWorld, int Key)
{
	int				x, y;
	CTable_Record	*pRecord;
	CGEO_Rect		rWorld(m_Edit_Mouse_Down, Point);

	m_Sel_xOff	= m_pGrid->Get_System().Get_xWorld_to_Grid(rWorld.Get_XMin());
	if( m_Sel_xOff < 0 )
		m_Sel_xOff	= 0;
	m_Sel_xN	= m_pGrid->Get_System().Get_xWorld_to_Grid(rWorld.Get_XMax());
	if( m_Sel_xN >= m_pGrid->Get_NX() )
		m_Sel_xN	= m_pGrid->Get_NX() - 1;
	m_Sel_xN	= 1 + m_Sel_xN - m_Sel_xOff;

	m_Sel_yOff	= m_pGrid->Get_System().Get_yWorld_to_Grid(rWorld.Get_YMin());
	if( m_Sel_yOff < 0 )
		m_Sel_yOff	= 0;
	m_Sel_yN	= m_pGrid->Get_System().Get_yWorld_to_Grid(rWorld.Get_YMax());
	if( m_Sel_yN >= m_pGrid->Get_NY() )
		m_Sel_yN	= m_pGrid->Get_NY() - 1;
	m_Sel_yN	= 1 + m_Sel_yN - m_Sel_yOff;

	m_Edit_Attributes.Destroy();

	if( m_Sel_xN < 1 || m_Sel_yN < 1 )
	{
		m_Sel_xN	= -1;
	}
	else
	{
		if( m_Sel_xN > SELECTION_MAX )
		{
			m_Sel_xOff	+= (m_Sel_xN - SELECTION_MAX) / 2;
			m_Sel_xN	= SELECTION_MAX;
		}

		if( m_Sel_yN > SELECTION_MAX )
		{
			m_Sel_yOff	+= (m_Sel_yN - SELECTION_MAX) / 2;
			m_Sel_yN	= SELECTION_MAX;
		}

		for(x=0; x<m_Sel_xN; x++)
		{
			m_Edit_Attributes.Add_Field(wxString::Format("%d", x + 1), TABLE_FIELDTYPE_Double);
		}

		for(y=0; y<m_Sel_yN; y++)
		{
			pRecord	= m_Edit_Attributes.Add_Record();

			for(x=0; x<m_Sel_xN; x++)
			{
				pRecord->Set_Value(x, m_pGrid->asDouble(m_Sel_xOff + x, m_Sel_yOff + m_Sel_yN - 1 - y, false));
			}
		}
	}

	g_pACTIVE->Get_Attributes()->Set_Attributes();

	Update_Views(true);

	return( true );
}

//---------------------------------------------------------
bool CWKSP_Grid::On_Edit_Set_Attributes(void)
{
	int				x, y;
	CTable_Record	*pRecord;

	if( m_Sel_xN >= 0 )
	{
		for(y=0; y<m_Sel_yN; y++)
		{
			pRecord	= m_Edit_Attributes.Get_Record(y);

			for(x=0; x<m_Sel_xN; x++)
			{
				m_pGrid->Set_Value(m_Sel_xOff + x, m_Sel_yOff + m_Sel_yN - 1 - y, pRecord->asDouble(x));
			}
		}

		Update_Views(true);

		return( true );
	}

	return( false );
}

//---------------------------------------------------------
TGEO_Rect CWKSP_Grid::On_Edit_Get_Extent(void)
{
	if( m_Sel_xN >= 0 )
	{
		return( CGEO_Rect(
			-m_pGrid->Get_Cellsize() / 2.0 + m_pGrid->Get_System().Get_xGrid_to_World(m_Sel_xOff),
			-m_pGrid->Get_Cellsize() / 2.0 + m_pGrid->Get_System().Get_yGrid_to_World(m_Sel_yOff),
			-m_pGrid->Get_Cellsize() / 2.0 + m_pGrid->Get_System().Get_xGrid_to_World(m_Sel_xOff + m_Sel_xN),
			-m_pGrid->Get_Cellsize() / 2.0 + m_pGrid->Get_System().Get_yGrid_to_World(m_Sel_yOff + m_Sel_yN))
		);
	}

	return( m_pGrid->Get_Extent().m_rect );
}


///////////////////////////////////////////////////////////
//														 //
//														 //
//														 //
///////////////////////////////////////////////////////////

//---------------------------------------------------------
bool CWKSP_Grid::_Edit_Del_Selection(void)
{
	int		x, y;

	if( m_Sel_xN >= 0 && DLG_Message_Confirm(LNG("[DLG] Set selected values to no data."), LNG("[CAP] Delete")) )
	{
		for(y=m_Sel_yOff; y<m_Sel_yOff + m_Sel_yN; y++)
		{
			for(x=m_Sel_xOff; x<m_Sel_xOff + m_Sel_xN; x++)
			{
				m_pGrid->Set_NoData(x, y);
			}
		}

		Update_Views(false);

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
bool CWKSP_Grid::Fit_Color_Range(CGEO_Rect rWorld)
{
	int		x, y, xMin, yMin, xMax, yMax;
	double	z, zMin, zMax;

	if( rWorld.Intersect(Get_Extent()) )
	{
		xMin	= m_pGrid->Get_System().Get_xWorld_to_Grid(rWorld.Get_XMin());
		yMin	= m_pGrid->Get_System().Get_yWorld_to_Grid(rWorld.Get_YMin());
		xMax	= m_pGrid->Get_System().Get_xWorld_to_Grid(rWorld.Get_XMax());
		yMax	= m_pGrid->Get_System().Get_yWorld_to_Grid(rWorld.Get_YMax());
		zMin	= 1.0;
		zMax	= 0.0;

		for(y=yMin; y<=yMax; y++)
		{
			for(x=xMin; x<=xMax; x++)
			{
				if( m_pGrid->is_InGrid(x, y) )
				{
					z	= m_pGrid->asDouble(x, y);

					if( zMin > zMax )
					{
						zMin	= zMax	= z;
					}
					else if( z < zMin )
					{
						zMin	= z;
					}
					else if( z > zMax )
					{
						zMax	= z;
					}
				}
			}
		}

		return( Set_Color_Range(zMin, zMax) );
	}

	return( false );
}


///////////////////////////////////////////////////////////
//														 //
//														 //
//														 //
///////////////////////////////////////////////////////////

//---------------------------------------------------------
bool CWKSP_Grid::asImage(CGrid *pImage)
{
	int			x, y;
	wxBitmap	BMP;

	if( pImage && Get_Image_Grid(&BMP) )
	{
		wxImage	IMG(BMP.ConvertToImage());

		pImage->Create(m_pGrid, GRID_TYPE_Int);

		for(y=0; y<pImage->Get_NY() && PROGRESSBAR_Set_Position(y, pImage->Get_NY()); y++)
		{
			for(x=0; x<pImage->Get_NX(); x++)
			{
				pImage->Set_Value(x, y, COLOR_GET_RGB(IMG.GetRed(x, y), IMG.GetGreen(x, y), IMG.GetBlue(x, y)));
			}
		}

		return( true );
	}

	return( false );
}

//---------------------------------------------------------
void CWKSP_Grid::_Save_Image(void)
{
	int			type;
	FILE		*Stream;
	wxString	file;
	wxBitmap	BMP;
	CParameters	Parms;

	//-----------------------------------------------------
	Parms.Set_Name(LNG("[CAP] Save Grid as Image..."));

	Parms.Add_Value(
		NULL	, "WORLD"	, LNG("Save Georeference"),
		"",
		PARAMETER_TYPE_Bool, 1
	);

	Parms.Add_Value(
		NULL	, "LG"	, LNG("Legend: Save"),
		"",
		PARAMETER_TYPE_Bool, 1
	);

	Parms.Add_Value(
		NULL	, "LZ"	, LNG("Legend: Zoom"),
		"",
		PARAMETER_TYPE_Double, 1.0, 0, true
	);

	//-----------------------------------------------------
	if( DLG_Image_Save(file, type) && DLG_Parameters(&Parms) )
	{
		if( Get_Image_Grid(&BMP) )
		{
			BMP.SaveFile(file, (wxBitmapType)type);
		}

		if( Parms("LG")->asBool() && Get_Image_Legend(&BMP, Parms("LZ")->asDouble()) )
		{
			wxFileName	fn(file);
			fn.SetName(wxString::Format("%s_legend", fn.GetName().c_str()));

			BMP.SaveFile(fn.GetFullPath(), (wxBitmapType)type);
		}

		if( Parms("WORLD")->asBool() )
		{
			wxFileName	fn(file);
			fn.SetExt("world");

			if( (Stream = fopen(wxString::Format("%s.world", file.c_str()), "w")) != NULL )
			{
				fprintf(Stream, "%f\n%f\n%f\n%f\n%f\n%f\n",
					 m_pGrid->Get_Cellsize(),
					 0.0, 0.0,
					-m_pGrid->Get_Cellsize(),
					 m_pGrid->Get_XMin(),
					 m_pGrid->Get_YMax()
				);

				fclose(Stream);
			}
		}
	}
}

//---------------------------------------------------------
bool CWKSP_Grid::Get_Image_Grid(wxBitmap *pBMP)
{
	if( pBMP )
	{
		Set_Buisy_Cursor(true);

		wxMemoryDC		dc;
		wxRect			r(0, 0, m_pGrid->Get_NX(), m_pGrid->Get_NY());
		CWKSP_Map_DC	dc_Map(Get_Extent(), r, 1.0, COLOR_GET_RGB(255, 255, 255));

		On_Draw(dc_Map, false);

		pBMP->Create(r.GetWidth(), r.GetHeight());

		dc.SelectObject(*pBMP);
		dc.SetBackground(*wxWHITE_BRUSH);
		dc.Clear();

		dc_Map.Draw(dc);

		dc.SelectObject(wxNullBitmap);

		Set_Buisy_Cursor(false);

		return( true );
	}

	return( false );
}

//---------------------------------------------------------
bool CWKSP_Grid::Get_Image_Legend(wxBitmap *pBMP, double Zoom)
{
	if( pBMP )
	{
		wxMemoryDC	dc;
		wxSize		s(Get_Legend()->Get_Size(1.0, Zoom));

		pBMP->Create(s.GetWidth(), s.GetHeight());

		dc.SelectObject(*pBMP);
		dc.SetBackground(*wxWHITE_BRUSH);
		dc.Clear();

		Get_Legend()->Draw(dc, 1.0, Zoom, wxPoint(0, 0));

		dc.SelectObject(wxNullBitmap);

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
void CWKSP_Grid::On_Draw(CWKSP_Map_DC &dc_Map, bool bEdit)
{
	int		Interpolation;
	double	Transparency;

	if(	Get_Extent().Intersects(dc_Map.m_rWorld) != INTERSECTION_None )
	{
		switch( m_pClassify->Get_Mode() )
		{
		default:				Transparency	= m_Parameters("DISPLAY_TRANSPARENCY")->asDouble() / 100.0;	break;
		case CLASSIFY_SHADE:	Transparency	= 2.0;	break;
		case CLASSIFY_RGB:		Transparency	= m_Parameters("DISPLAY_TRANSPARENCY")->asDouble() / 100.0;	if( Transparency <= 0.0 )	Transparency	= 3.0;	break;
		}

		if( dc_Map.IMG_Draw_Begin(Transparency) )
		{
			Interpolation	= m_pClassify->Get_Mode() == CLASSIFY_LUT
							? GRID_INTERPOLATION_NearestNeighbour
							: m_Parameters("DISPLAY_INTERPOLATION")->asInt();

			if(	dc_Map.m_DC2World >= m_pGrid->Get_Cellsize()
			||	Interpolation != GRID_INTERPOLATION_NearestNeighbour )
			{
				_Draw_Grid_Points	(dc_Map, Interpolation);
			}
			else
			{
				_Draw_Grid_Cells	(dc_Map);
			}

			dc_Map.IMG_Draw_End();

			_Draw_Values(dc_Map);

			if( bEdit )
			{
				_Draw_Edit(dc_Map);
			}
		}
	}
}

//---------------------------------------------------------
void CWKSP_Grid::_Draw_Grid_Points(CWKSP_Map_DC &dc_Map, int Interpolation)
{
	int			xDC, yDC, axDC, ayDC, bxDC, byDC, Color;
	double		x, y, z;
	CGEO_Rect	rGrid(m_pGrid->Get_Extent());

	rGrid.Inflate(m_pGrid->Get_Cellsize() / 2.0, false);
	rGrid.Intersect(dc_Map.m_rWorld);

	axDC	= (int)dc_Map.xWorld2DC(rGrid.Get_XMin());	if( axDC < 0 )	axDC	= 0;
	bxDC	= (int)dc_Map.xWorld2DC(rGrid.Get_XMax());	if( bxDC > dc_Map.m_rDC.GetWidth() )	bxDC	= dc_Map.m_rDC.GetWidth();
	ayDC	= (int)dc_Map.yWorld2DC(rGrid.Get_YMin());	if( ayDC > dc_Map.m_rDC.GetHeight() )	ayDC	= dc_Map.m_rDC.GetHeight();
	byDC	= (int)dc_Map.yWorld2DC(rGrid.Get_YMax());	if( byDC < 0 )	byDC	= 0;

	for(y=rGrid.Get_YMin(), yDC=ayDC-1; yDC>=byDC; y+=dc_Map.m_DC2World, yDC--)
	{
		for(x=rGrid.Get_XMin(), xDC=axDC; xDC<bxDC; x+=dc_Map.m_DC2World, xDC++)
		{
			if( m_pGrid->Get_Value(x, y, z, Interpolation) && m_pClassify->Get_Class_Color_byValue(z, Color) )
			{
				dc_Map.IMG_Set_Pixel(xDC, yDC, Color);
			}
		}
	}
}

//---------------------------------------------------------
void CWKSP_Grid::_Draw_Grid_Cells(CWKSP_Map_DC &dc_Map)
{
	int		x, y, xa, ya, xb, yb, xaDC, yaDC, xbDC, ybDC, Color;
	double	xDC, yDC, axDC, ayDC, dDC;

	//-----------------------------------------------------
	dDC		= m_pGrid->Get_Cellsize() * dc_Map.m_World2DC;

	xa		= m_pGrid->Get_System().Get_xWorld_to_Grid(dc_Map.m_rWorld.Get_XMin());
	ya		= m_pGrid->Get_System().Get_yWorld_to_Grid(dc_Map.m_rWorld.Get_YMin());
	xb		= m_pGrid->Get_System().Get_xWorld_to_Grid(dc_Map.m_rWorld.Get_XMax());
	yb		= m_pGrid->Get_System().Get_yWorld_to_Grid(dc_Map.m_rWorld.Get_YMax());

	if( xa < 0 )	xa	= 0;	if( xb >= m_pGrid->Get_NX() )	xb	= m_pGrid->Get_NX() - 1;
	if( ya < 0 )	ya	= 0;	if( yb >= m_pGrid->Get_NY() )	yb	= m_pGrid->Get_NY() - 1;

	axDC	= dc_Map.xWorld2DC(m_pGrid->Get_System().Get_xGrid_to_World(xa)) + dDC / 2.0;
	ayDC	= dc_Map.yWorld2DC(m_pGrid->Get_System().Get_yGrid_to_World(ya)) - dDC / 2.0;

	//-----------------------------------------------------
	for(y=ya, yDC=ayDC, yaDC=(int)(ayDC), ybDC=(int)(ayDC+dDC); y<=yb; y++, ybDC=yaDC, yaDC=(int)(yDC-=dDC))
	{
		for(x=xa, xDC=axDC, xaDC=(int)(axDC-dDC), xbDC=(int)(axDC); x<=xb; x++, xaDC=xbDC, xbDC=(int)(xDC+=dDC))
		{
			if( m_pGrid->is_InGrid(x, y) && m_pClassify->Get_Class_Color_byValue(m_pGrid->asDouble(x, y), Color) )
			{
				dc_Map.IMG_Set_Rect(xaDC, yaDC, xbDC, ybDC, Color);
			}
		}
	}
}

//---------------------------------------------------------
void CWKSP_Grid::_Draw_Values(CWKSP_Map_DC &dc_Map)
{
	int		x, y, xa, ya, xb, yb, Decimals;
	double	xDC, yDC, axDC, ayDC, dDC, zFactor;
	wxFont	Font;

	//-----------------------------------------------------
	if(	m_Parameters("VALUES_SHOW")->asBool() && (dDC = m_pGrid->Get_Cellsize() * dc_Map.m_World2DC) > 40 )
	{
		zFactor		=  m_pGrid->Get_ZFactor();
		Decimals	=  m_Parameters("VALUES_DECIMALS")	->asInt();

		xDC			=  m_Parameters("VALUES_SIZE")		->asDouble() / 100.0;
		x			=  m_Parameters("VALUES_FONT")		->asColor();
		Font		= *m_Parameters("VALUES_FONT")		->asFont();
		Font.SetPointSize((int)(xDC * dDC));
		dc_Map.dc.SetFont(Font);
		dc_Map.dc.SetTextForeground(Get_Color_asWX(x));

		x			=  m_Parameters("VALUES_FONT")		->asInt();
		dc_Map.dc.SetTextForeground(wxColour(COLOR_GET_R(x), COLOR_GET_G(x), COLOR_GET_B(x)));

		//-------------------------------------------------
		xa		= m_pGrid->Get_System().Get_xWorld_to_Grid(dc_Map.m_rWorld.Get_XMin());
		ya		= m_pGrid->Get_System().Get_yWorld_to_Grid(dc_Map.m_rWorld.Get_YMin());
		xb		= m_pGrid->Get_System().Get_xWorld_to_Grid(dc_Map.m_rWorld.Get_XMax());
		yb		= m_pGrid->Get_System().Get_yWorld_to_Grid(dc_Map.m_rWorld.Get_YMax());

		if( xa < 0 )	xa	= 0;	if( xb >= m_pGrid->Get_NX() )	xb	= m_pGrid->Get_NX() - 1;
		if( ya < 0 )	ya	= 0;	if( yb >= m_pGrid->Get_NY() )	yb	= m_pGrid->Get_NY() - 1;

		axDC	= dc_Map.xWorld2DC(m_pGrid->Get_System().Get_xGrid_to_World(xa));
		ayDC	= dc_Map.yWorld2DC(m_pGrid->Get_System().Get_yGrid_to_World(ya));

		//-------------------------------------------------
		for(y=ya, yDC=ayDC; y<=yb; y++, yDC-=dDC)
		{
			for(x=xa, xDC=axDC; x<=xb; x++, xDC+=dDC)
			{
				if( m_pGrid->is_InGrid(x, y) )
				{
					Draw_Text(dc_Map.dc, TEXTALIGN_CENTER, (int)xDC, (int)yDC,
						wxString::Format("%.*f", Decimals, zFactor * m_pGrid->asDouble(x, y))
					);
				}
			}
		}
	}
}

//---------------------------------------------------------
void CWKSP_Grid::_Draw_Edit(CWKSP_Map_DC &dc_Map)
{
	if( m_Sel_xN >= 0 )
	{
		CGEO_Rect	r(
			-m_pGrid->Get_Cellsize() / 2.0 + m_pGrid->Get_System().Get_xGrid_to_World(m_Sel_xOff),
			-m_pGrid->Get_Cellsize() / 2.0 + m_pGrid->Get_System().Get_yGrid_to_World(m_Sel_yOff),
			-m_pGrid->Get_Cellsize() / 2.0 + m_pGrid->Get_System().Get_xGrid_to_World(m_Sel_xOff + m_Sel_xN),
			-m_pGrid->Get_Cellsize() / 2.0 + m_pGrid->Get_System().Get_yGrid_to_World(m_Sel_yOff + m_Sel_yN)
		);

		TAPI_iPoint		a(dc_Map.World2DC(r.Get_TopLeft())),
						b(dc_Map.World2DC(r.Get_BottomRight()));

		a.x	-= 1;
		b.x	-= 1;
		a.y	-= 1;
		b.y	-= 1;

		dc_Map.dc.SetPen(wxPen(wxColour(255, 0, 0), 2, wxSOLID));
		dc_Map.dc.DrawLine(a.x, a.y, a.x, b.y);
		dc_Map.dc.DrawLine(a.x, b.y, b.x, b.y);
		dc_Map.dc.DrawLine(b.x, b.y, b.x, a.y);
		dc_Map.dc.DrawLine(a.x, a.y, b.x, a.y);
	}
}


///////////////////////////////////////////////////////////
//														 //
//														 //
//														 //
///////////////////////////////////////////////////////////

//---------------------------------------------------------

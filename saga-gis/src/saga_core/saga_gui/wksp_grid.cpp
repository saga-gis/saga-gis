/**********************************************************
 * Version $Id$
 *********************************************************/

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

#include "wksp_data_manager.h"
#include "wksp_grid_manager.h"
#include "wksp_grid.h"

#include "view_scatterplot.h"


///////////////////////////////////////////////////////////
//														 //
//														 //
//														 //
///////////////////////////////////////////////////////////

//---------------------------------------------------------
CWKSP_Grid::CWKSP_Grid(CSG_Grid *pGrid)
	: CWKSP_Layer(pGrid)
{
	m_pGrid			= pGrid;

	m_pOverlay[0]	= NULL;
	m_pOverlay[1]	= NULL;

	m_Sel_xN		= -1;

	Initialise();
}

//---------------------------------------------------------
CWKSP_Grid::~CWKSP_Grid(void)
{}


///////////////////////////////////////////////////////////
//														 //
//														 //
//														 //
///////////////////////////////////////////////////////////

//---------------------------------------------------------
wxString CWKSP_Grid::Get_Description(void)
{
	wxString	s;

	//-----------------------------------------------------
	s	+= wxString::Format(wxT("<b>%s</b>"), _TL("[CAP] Grid"));

	s	+= wxT("<table border=\"0\">");

	DESC_ADD_STR (_TL("[CAP] Name")					, m_pGrid->Get_Name());
	DESC_ADD_STR (_TL("[CAP] Description")			, m_pGrid->Get_Description());
	DESC_ADD_STR (_TL("[CAP] File")					, SG_File_Exists(m_pGrid->Get_File_Name()) ? m_pGrid->Get_File_Name() : _TL("memory"));
	DESC_ADD_STR (_TL("[CAP] Modified")				, m_pGrid->is_Modified() ? _TL("[VAL] yes") : _TL("[VAL] no"));
	DESC_ADD_STR (_TL("[CAP] Projection")			, m_pGrid->Get_Projection().Get_Description().c_str());
	DESC_ADD_FLT (_TL("[CAP] West")					, m_pGrid->Get_XMin());
	DESC_ADD_FLT (_TL("[CAP] East")					, m_pGrid->Get_XMax());
	DESC_ADD_FLT (_TL("[CAP] West-East")			, m_pGrid->Get_XRange());
	DESC_ADD_FLT (_TL("[CAP] South")				, m_pGrid->Get_YMin());
	DESC_ADD_FLT (_TL("[CAP] North")				, m_pGrid->Get_YMax());
	DESC_ADD_FLT (_TL("[CAP] South-North")			, m_pGrid->Get_YRange());
	DESC_ADD_FLT (_TL("[CAP] Cell Size")			, m_pGrid->Get_Cellsize());
	DESC_ADD_INT (_TL("[CAP] Number of Columns")	, m_pGrid->Get_NX());
	DESC_ADD_INT (_TL("[CAP] Number of Rows")		, m_pGrid->Get_NY());
	DESC_ADD_LONG(_TL("[CAP] Number of Cells")		, m_pGrid->Get_NCells());
	DESC_ADD_LONG(_TL("[CAP] No Data Cells")		, m_pGrid->Get_NoData_Count());
	DESC_ADD_STR (_TL("[CAP] Value Type")			, SG_Data_Type_Get_Name(m_pGrid->Get_Type()).c_str());
	DESC_ADD_FLT (_TL("[CAP] Value Minimum")		, m_pGrid->Get_ZMin());
	DESC_ADD_FLT (_TL("[CAP] Value Maximum")		, m_pGrid->Get_ZMax());
	DESC_ADD_FLT (_TL("[CAP] Value Range")			, m_pGrid->Get_ZRange());
	DESC_ADD_STR (_TL("[CAP] No Data Value")		, m_pGrid->Get_NoData_Value() < m_pGrid->Get_NoData_hiValue() ? CSG_String::Format(SG_T("%f - %f"), m_pGrid->Get_NoData_Value(), m_pGrid->Get_NoData_hiValue()).c_str() : SG_Get_String(m_pGrid->Get_NoData_Value(), -2).c_str());
	DESC_ADD_FLT (_TL("[CAP] Arithmetic Mean")		, m_pGrid->Get_ArithMean(true));
	DESC_ADD_FLT (_TL("[CAP] Standard Deviation")	, m_pGrid->Get_StdDev(true));
	DESC_ADD_STR (_TL("[CAP] Memory Size")			, Get_nBytes_asString(m_pGrid->Get_NCells() * m_pGrid->Get_nValueBytes(), 2).c_str());

	if( m_pGrid->is_Compressed() )
	{
		DESC_ADD_FLT(_TL("[CAP] Memory Compression")	, 100.0 * m_pGrid->Get_Compression_Ratio());
	}

	if( m_pGrid->is_Cached() )
	{
		DESC_ADD_FLT(_TL("[CAP] File Cache [MB]")		, m_pGrid->Get_Buffer_Size() / (double)N_MEGABYTE_BYTES);
	}

	s	+= wxT("</table>");

	//-----------------------------------------------------
//	s.Append(wxString::Format(wxT("<hr><b>%s</b><font size=\"-1\">"), _TL("[CAP] Data History")));
//	s.Append(m_pGrid->Get_History().Get_HTML());
//	s.Append(wxString::Format(wxT("</font")));

	return( s );
}

//---------------------------------------------------------
wxMenu * CWKSP_Grid::Get_Menu(void)
{
	wxMenu	*pMenu, *pSubMenu;

	pMenu		= new wxMenu(m_pGrid->Get_Name());

	CMD_Menu_Add_Item(pMenu		, false, ID_CMD_WKSP_ITEM_CLOSE);
	CMD_Menu_Add_Item(pMenu		, false, ID_CMD_GRIDS_SAVE);
	CMD_Menu_Add_Item(pMenu		, false, ID_CMD_GRIDS_SAVEAS);
	CMD_Menu_Add_Item(pMenu		, false, ID_CMD_GRIDS_SAVEAS_IMAGE);
	CMD_Menu_Add_Item(pMenu		, false, ID_CMD_GRIDS_SHOW);

	pMenu->AppendSeparator();

	CMD_Menu_Add_Item(pMenu		, true , ID_CMD_GRIDS_HISTOGRAM);

	CMD_Menu_Add_Item(pMenu		, false, ID_CMD_GRIDS_SCATTERPLOT);
	CMD_Menu_Add_Item(pMenu		, false, ID_CMD_WKSP_ITEM_SETTINGS_COPY);

	pSubMenu	= new wxMenu(_TL("[MNU] Classificaton"));
	CMD_Menu_Add_Item(pSubMenu	, false, ID_CMD_GRIDS_SET_LUT);
	CMD_Menu_Add_Item(pSubMenu	, false, ID_CMD_GRIDS_RANGE_MINMAX);
	CMD_Menu_Add_Item(pSubMenu	, false, ID_CMD_GRIDS_RANGE_STDDEV150);
	CMD_Menu_Add_Item(pSubMenu	, false, ID_CMD_GRIDS_RANGE_STDDEV200);

	pMenu->Append(ID_CMD_WKSP_FIRST, _TL("[MNU] Classification"), pSubMenu);

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

	case ID_CMD_GRIDS_RANGE_MINMAX:
		Set_Color_Range(
			m_pGrid->Get_ZMin(true),
			m_pGrid->Get_ZMax(true)
		);
		break;

	case ID_CMD_GRIDS_RANGE_STDDEV150:
		Set_Color_Range(
			m_pGrid->Get_ArithMean(true) - 1.5 * m_pGrid->Get_StdDev(true),
			m_pGrid->Get_ArithMean(true) + 1.5 * m_pGrid->Get_StdDev(true)
		);
		break;

	case ID_CMD_GRIDS_RANGE_STDDEV200:
		Set_Color_Range(
			m_pGrid->Get_ArithMean(true) - 2.0 * m_pGrid->Get_StdDev(true),
			m_pGrid->Get_ArithMean(true) + 2.0 * m_pGrid->Get_StdDev(true)
		);
		break;

	case ID_CMD_GRIDS_SET_LUT:
		_LUT_Create();
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
	CWKSP_Layer::On_Create_Parameters();

	//-----------------------------------------------------
	// General...

	m_Parameters.Add_String(
		m_Parameters("NODE_GENERAL")	, "GENERAL_Z_UNIT"			, _TL("[CAP] Unit"),
		_TL(""),
		m_pGrid->Get_Unit()
	);

	m_Parameters.Add_Value(
		m_Parameters("NODE_GENERAL")	, "GENERAL_Z_FACTOR"		, _TL("[CAP] Z-Factor"),
		_TL(""),
		PARAMETER_TYPE_Double
	);


	//-----------------------------------------------------
	// Display...

	m_Parameters.Add_Choice(
		m_Parameters("NODE_DISPLAY")	, "DISPLAY_INTERPOLATION"	, _TL("[CAP] Interpolation"),
		_TL(""),
		CSG_String::Format(SG_T("%s|%s|%s|%s|%s|"),
			_TL("[VAL] None"),
			_TL("[VAL] Bilinear"),
			_TL("[VAL] Inverse Distance"),
			_TL("[VAL] Bicubic Spline"),
			_TL("[VAL] B-Spline")
		), 0
	);


	//-----------------------------------------------------
	// Classification...

	((CSG_Parameter_Choice *)m_Parameters("COLORS_TYPE")->Get_Data())->Set_Items(
		CSG_String::Format(SG_T("%s|%s|%s|%s|%s|%s|%s|"),
			_TL("[VAL] Single Symbol"),		// CLASSIFY_UNIQUE
			_TL("[VAL] Lookup Table"),		// CLASSIFY_LUT
			_TL("[VAL] Discrete Colors"),	// CLASSIFY_METRIC
			_TL("[VAL] Graduated Colors"),	// CLASSIFY_GRADUATED
			_TL("[VAL] Shade"),				// CLASSIFY_SHADE
			_TL("[VAL] RGB Overlay"),		// CLASSIFY_OVERLAY
			_TL("[VAL] RGB")				// CLASSIFY_RGB
		)
	);

	m_Parameters("COLORS_TYPE")->Set_Value(CLASSIFY_GRADUATED);

	//-----------------------------------------------------
	m_Parameters.Add_Node(
		m_Parameters("NODE_COLORS")		, "NODE_SHADE"		, _TL("[CAP] Shade"),
		_TL("")
	);

	m_Parameters.Add_Choice(
		m_Parameters("NODE_SHADE")		, "SHADE_MODE"		, _TL("[CAP] Coloring"),
		_TL(""),
		CSG_String::Format(SG_T("%s|%s|%s|%s|%s|%s|%s|%s|"),
			_TL("bright - dark"),
			_TL("dark - bright"),
			_TL("white - cyan"),
			_TL("cyan - white"),
			_TL("white - magenta"),
			_TL("magenta - white"),
			_TL("white - yellow"),
			_TL("yellow - white")
		), 0
	);

	//-----------------------------------------------------
	m_Parameters.Add_Node(
		m_Parameters("NODE_COLORS")		, "NODE_OVERLAY"	, _TL("[CAP] RGB Overlay"),
		_TL("")
	);

	m_Parameters.Add_Choice(
		m_Parameters("NODE_OVERLAY")	, "OVERLAY_MODE"	, _TL("[CAP] Coloring"),
		_TL(""),
		CSG_String::Format(SG_T("%s|%s|%s|%s|%s|%s|"),
			_TL("red=this, green=1, blue=2"),
			_TL("red=this, green=2, blue=1"),
			_TL("red=1, green=this, blue=2"),
			_TL("red=2, green=this, blue=1"),
			_TL("red=1, green=2, blue=this"),
			_TL("red=2, green=1, blue=this")
		), 0
	);

	m_Parameters.Add_Grid(
		m_Parameters("NODE_OVERLAY")	, "OVERLAY_1"		, _TL("[CAP] Overlay 1"),
		_TL(""),
		PARAMETER_INPUT_OPTIONAL, false
	)->Get_Parent()->Set_Value((void *)&m_pGrid->Get_System());

	m_Parameters.Add_Grid(
		m_Parameters("NODE_OVERLAY")	, "OVERLAY_2"		, _TL("[CAP] Overlay 2"),
		_TL(""),
		PARAMETER_INPUT_OPTIONAL, false
	)->Get_Parent()->Set_Value((void *)&m_pGrid->Get_System());


	//-----------------------------------------------------
	// Cell Values...

	m_Parameters.Add_Value(
		m_Parameters("NODE_GENERAL")	, "VALUES_SHOW"		, _TL("[CAP] Show Cell Values"),
		_TL("shows cell values when zoomed"),
		PARAMETER_TYPE_Bool, false
	);

	m_Parameters.Add_Font(
		m_Parameters("VALUES_SHOW")		, "VALUES_FONT"		, _TL("[CAP] Font"),
		_TL("")
	);

	m_Parameters.Add_Value(
		m_Parameters("VALUES_SHOW")		, "VALUES_SIZE"		, _TL("[CAP] Size"),
		_TL(""),
		PARAMETER_TYPE_Double, 15, 0, true , 100.0, true
	);

	m_Parameters.Add_Value(
		m_Parameters("VALUES_SHOW")		, "VALUES_DECIMALS"	, _TL("[CAP] Decimals"),
		_TL(""),
		PARAMETER_TYPE_Int, 2
	);

	m_Parameters.Add_Choice(
		m_Parameters("VALUES_SHOW")		, "VALUES_EFFECT"	, _TL("[CAP] Boundary Effect"),
		_TL(""),
		CSG_String::Format(SG_T("%s|%s|%s|%s|%s|%s|%s|%s|%s|%s|"),
			_TL("none"),
			_TL("full frame"),
			_TL("top"),
			_TL("top left"),
			_TL("left"),
			_TL("bottom left"),
			_TL("bottom"),
			_TL("bottom right"),
			_TL("right"),
			_TL("top right")
		), 1
	);

	m_Parameters.Add_Value(
		m_Parameters("VALUES_EFFECT")	, "VALUES_EFFECT_COLOR"	, _TL("[CAP] Color"),
		_TL(""),
		PARAMETER_TYPE_Color, SG_GET_RGB(255, 255, 255)
	);

	//-----------------------------------------------------
	// Memory...

	m_Parameters.Add_Choice(
		m_Parameters("NODE_GENERAL")	, "MEMORY_MODE"				, _TL("[CAP] Memory Handling"),
		_TL(""),
		CSG_String::Format(SG_T("%s|%s|%s|"),
			_TL("[VAL] Normal"),
			_TL("[VAL] RTL Compression"),
			_TL("[VAL] File Cache")
		), 0
	);

	m_Parameters.Add_Value(
		m_Parameters("MEMORY_MODE")		, "MEMORY_BUFFER_SIZE"		, _TL("[CAP] Buffer Size MB"),
		_TL(""),
		PARAMETER_TYPE_Double
	);


	//-----------------------------------------------------
	DataObject_Changed((CSG_Parameters *)NULL);
}

//---------------------------------------------------------
void CWKSP_Grid::On_DataObject_Changed(void)
{
	//-----------------------------------------------------
	m_Parameters("GENERAL_Z_UNIT")			->Set_Value((void *)m_pGrid->Get_Unit());
	m_Parameters("GENERAL_Z_FACTOR")		->Set_Value(m_pGrid->Get_ZFactor());

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

	//-----------------------------------------------------
	m_pOverlay[0]	= g_pData->Get_Grids()->Get_Grid(m_Parameters("OVERLAY_1")->asGrid());
	m_pOverlay[1]	= g_pData->Get_Grids()->Get_Grid(m_Parameters("OVERLAY_2")->asGrid());
	m_bOverlay		= m_Parameters("COLORS_TYPE")->asInt() == CLASSIFY_OVERLAY;

	m_pClassify->Set_Shade_Mode(m_Parameters("SHADE_MODE")->asInt());

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

//---------------------------------------------------------
int CWKSP_Grid::On_Parameter_Changed(CSG_Parameters *pParameters, CSG_Parameter *pParameter, int Flags)
{
	if( Flags & PARAMETER_CHECK_ENABLE )
	{
		if(	!SG_STR_CMP(pParameter->Get_Identifier(), SG_T("COLORS_TYPE")) )
		{
			int		Value	= pParameter->asInt();

			pParameters->Get_Parameter("NODE_UNISYMBOL")->Set_Enabled(Value == CLASSIFY_UNIQUE);
			pParameters->Get_Parameter("NODE_LUT"      )->Set_Enabled(Value == CLASSIFY_LUT);
			pParameters->Get_Parameter("NODE_METRIC"   )->Set_Enabled(Value != CLASSIFY_UNIQUE && Value != CLASSIFY_LUT);
			pParameters->Get_Parameter("NODE_SHADE"    )->Set_Enabled(Value == CLASSIFY_SHADE);
			pParameters->Get_Parameter("NODE_OVERLAY"  )->Set_Enabled(Value == CLASSIFY_OVERLAY);
		}

		if(	!SG_STR_CMP(pParameter->Get_Identifier(), SG_T("VALUES_SHOW")) )
		{
			bool	Value	= pParameter->asBool();

			pParameters->Get_Parameter("VALUES_FONT"    )->Set_Enabled(Value);
			pParameters->Get_Parameter("VALUES_SIZE"    )->Set_Enabled(Value);
			pParameters->Get_Parameter("VALUES_DECIMALS")->Set_Enabled(Value);
			pParameters->Get_Parameter("VALUES_EFFECT"  )->Set_Enabled(Value);
		}

		if(	!SG_STR_CMP(pParameter->Get_Identifier(), SG_T("VALUES_EFFECT")) )
		{
			bool	Value	= pParameter->asInt() > 0;

			pParameters->Get_Parameter("VALUES_EFFECT_COLOR")->Set_Enabled(Value);
		}

		if(	!SG_STR_CMP(pParameter->Get_Identifier(), SG_T("MEMORY_MODE")) )
		{
			int		Value	= pParameter->asInt();

			pParameters->Get_Parameter("MEMORY_BUFFER_SIZE")->Set_Enabled(Value != 0);
		}
	}

	return( CWKSP_Layer::On_Parameter_Changed(pParameters, pParameter, Flags) );
}


///////////////////////////////////////////////////////////
//														 //
//														 //
//														 //
///////////////////////////////////////////////////////////

//---------------------------------------------------------
void CWKSP_Grid::_LUT_Create(void)
{
	int				Type;
	CSG_Colors		*pColors;
	CSG_Table		*pLUT;

	//-----------------------------------------------------
	static CSG_Parameters	Parameters;

	if( Parameters.Get_Count() == 0 )
	{
		Parameters.Create(NULL, _TL("Create Lookup Table"), _TL(""));

		Parameters.Add_Colors(
			NULL, "COLOR"	, _TL("Colors"),
			_TL("")
		)->asColors()->Set_Count(10);

		Parameters.Add_Choice(
			NULL, "TYPE"	, _TL("Classification Type"),
			_TL(""),
			CSG_String::Format(SG_T("%s|%s|%s|"),
				_TL("unique values"),
				_TL("equal intervals"),
				_TL("quantiles")
			), 1
		);
	}

	if( !DLG_Parameters(&Parameters) )
	{
		return;
	}

	//-----------------------------------------------------
	pColors	= Parameters("COLOR")	->asColors();
	Type	= Parameters("TYPE")	->asInt();

	pLUT	= m_Parameters("LUT")	->asTable();
	pLUT	->Del_Records();

	switch( Type )
	{
	//-----------------------------------------------------
	case 0:	// unique values
		{
			double		Value;

			for(long iCell=0, jCell; iCell<m_pGrid->Get_NCells() && PROGRESSBAR_Set_Position(iCell, m_pGrid->Get_NCells()); iCell++)
			{
				if( m_pGrid->Get_Sorted(iCell, jCell, false) && (pLUT->Get_Record_Count() == 0 || Value != m_pGrid->asDouble(jCell)) )
				{
					Value	= m_pGrid->asDouble(jCell);

					CSG_Table_Record	*pClass	= pLUT->Add_Record();

					pClass->Set_Value(1, SG_Get_String(Value, -2));		// Name
					pClass->Set_Value(2, SG_Get_String(Value, -2));		// Description
					pClass->Set_Value(3, Value);						// Minimum
					pClass->Set_Value(4, Value);						// Maximum
				}
			}

			pColors->Set_Count(pLUT->Get_Count());

			for(int iClass=0; iClass<pLUT->Get_Count(); iClass++)
			{
				pLUT->Get_Record(iClass)->Set_Value(0, pColors->Get_Color(iClass));
			}
		}
		break;

	//-----------------------------------------------------
	case 1:	// equal intervals
		{
			double	Minimum, Maximum, Interval;

			Interval	= m_pGrid->Get_ZRange() / (double)pColors->Get_Count();
			Minimum		= m_pGrid->Get_ZMin  ();

			for(int iClass=0; iClass<pColors->Get_Count(); iClass++, Minimum+=Interval)
			{
				Maximum	= iClass < pColors->Get_Count() - 1 ? Minimum + Interval : m_pGrid->Get_ZMax() + 1.0;

				CSG_String	sValue;	sValue.Printf(SG_T("%s - %s"),
					SG_Get_String(Minimum, -2).c_str(),
					SG_Get_String(Maximum, -2).c_str()
				);

				CSG_Table_Record	*pClass	= pLUT->Add_Record();

				pClass->Set_Value(0, pColors->Get_Color(iClass));
				pClass->Set_Value(1, sValue);	// Name
				pClass->Set_Value(2, sValue);	// Description
				pClass->Set_Value(3, Minimum);	// Minimum
				pClass->Set_Value(4, Maximum);	// Maximum
			}
		}
		break;

	//-----------------------------------------------------
	case 2:	// quantiles
		{
			if( m_pGrid->Get_NCells() < pColors->Get_Count() )
			{
				pColors->Set_Count(m_pGrid->Get_NCells());
			}

			long	jCell, nCells;
			double	Minimum, Maximum, iCell, Count;

			Maximum	= m_pGrid->Get_ZMin();
			nCells	= m_pGrid->Get_NCells() - m_pGrid->Get_NoData_Count();
			iCell	= Count	= nCells / (double)pColors->Get_Count();

			for(iCell=0.0; iCell<m_pGrid->Get_NCells(); iCell++)
			{
				if( m_pGrid->Get_Sorted((long)iCell, jCell, false) )
				{
					break;
				}
			}

			iCell	+= Count;

			for(int iClass=0; iClass<pColors->Get_Count(); iClass++, iCell+=Count)
			{
				m_pGrid->Get_Sorted((long)iCell, jCell, false);

				Minimum	= Maximum;
				Maximum	= iCell < m_pGrid->Get_NCells() ? m_pGrid->asDouble(jCell) : m_pGrid->Get_ZMax() + 1.0;

				CSG_String	sValue;	sValue.Printf(SG_T("%s - %s"),
					SG_Get_String(Minimum, -2).c_str(),
					SG_Get_String(Maximum, -2).c_str()
				);

				CSG_Table_Record	*pClass	= pLUT->Add_Record();

				pClass->Set_Value(0, pColors->Get_Color(iClass));
				pClass->Set_Value(1, sValue);	// Name
				pClass->Set_Value(2, sValue);	// Description
				pClass->Set_Value(3, Minimum);	// Minimum
				pClass->Set_Value(4, Maximum);	// Maximum
			}
		}
		break;
	}

	//-----------------------------------------------------
	PROGRESSBAR_Set_Position(0);

	DataObject_Changed();

	m_Parameters("COLORS_TYPE")->Set_Value(CLASSIFY_LUT);	// Lookup Table

	Parameters_Changed();
}


///////////////////////////////////////////////////////////
//														 //
//														 //
//														 //
///////////////////////////////////////////////////////////

//---------------------------------------------------------
wxString CWKSP_Grid::Get_Value(CSG_Point ptWorld, double Epsilon)
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

		default:
			switch( m_pGrid->Get_Type() )
			{
			case SG_DATATYPE_Byte:
			case SG_DATATYPE_Char:
			case SG_DATATYPE_Word:
			case SG_DATATYPE_Short:
			case SG_DATATYPE_DWord:
			case SG_DATATYPE_Int:
				s.Printf(wxT("%d%s"), (int)Value, m_pGrid->Get_Unit());
				break;

			case SG_DATATYPE_Float:	default:
			case SG_DATATYPE_Double:
				s.Printf(wxT("%f%s"), Value, m_pGrid->Get_Unit());
				break;
			}
			break;

		case CLASSIFY_RGB:
			s.Printf(wxT("R%03d G%03d B%03d"), SG_GET_R((int)Value), SG_GET_G((int)Value), SG_GET_B((int)Value));
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
bool CWKSP_Grid::On_Edit_On_Mouse_Up(CSG_Point Point, double ClientToWorld, int Key)
{
	int					x, y;
	CSG_Table_Record	*pRecord;
	CSG_Rect			rWorld(m_Edit_Mouse_Down, Point);

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
			m_Edit_Attributes.Add_Field(CSG_String::Format(SG_T("%d"), x + 1), SG_DATATYPE_Double);
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
	CSG_Table_Record	*pRecord;

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
TSG_Rect CWKSP_Grid::On_Edit_Get_Extent(void)
{
	if( m_Sel_xN >= 0 )
	{
		return( CSG_Rect(
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

	if( m_Sel_xN >= 0 && DLG_Message_Confirm(_TL("[DLG] Set selected values to no data."), _TL("[CAP] Delete")) )
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
bool CWKSP_Grid::Fit_Color_Range(CSG_Rect rWorld)
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
bool CWKSP_Grid::asImage(CSG_Grid *pImage)
{
	int			x, y;
	wxBitmap	BMP;

	if( pImage && Get_Image_Grid(BMP) )
	{
		wxImage	IMG(BMP.ConvertToImage());

		pImage->Create(m_pGrid, SG_DATATYPE_Int);

		for(y=0; y<pImage->Get_NY() && PROGRESSBAR_Set_Position(y, pImage->Get_NY()); y++)
		{
			for(x=0; x<pImage->Get_NX(); x++)
			{
				pImage->Set_Value(x, y, SG_GET_RGB(IMG.GetRed(x, y), IMG.GetGreen(x, y), IMG.GetBlue(x, y)));
			}
		}

		return( true );
	}

	return( false );
}

//---------------------------------------------------------
void CWKSP_Grid::_Save_Image(void)
{
	int				type;
	wxString		file;
	wxBitmap		BMP;
	CSG_File		Stream;
	CSG_Parameters	Parms;

	//-----------------------------------------------------
	Parms.Set_Name(_TL("[CAP] Save Grid as Image..."));

	Parms.Add_Value(
		NULL	, "WORLD"	, _TL("Save Georeference"),
		wxT(""),
		PARAMETER_TYPE_Bool, 1
	);

	Parms.Add_Value(
		NULL	, "LG"	, _TL("Legend: Save"),
		wxT(""),
		PARAMETER_TYPE_Bool, 1
	);

	Parms.Add_Value(
		NULL	, "LZ"	, _TL("Legend: Zoom"),
		wxT(""),
		PARAMETER_TYPE_Double, 1.0, 0, true
	);

	//-----------------------------------------------------
	if( DLG_Image_Save(file, type) && DLG_Parameters(&Parms) )
	{
		if( Get_Image_Grid(BMP) )
		{
			BMP.SaveFile(file, (wxBitmapType)type);
		}

		if( Parms("LG")->asBool() && Get_Image_Legend(BMP, Parms("LZ")->asDouble()) )
		{
			wxFileName	fn(file);
			fn.SetName(wxString::Format(wxT("%s_legend"), fn.GetName().c_str()));

			BMP.SaveFile(fn.GetFullPath(), (wxBitmapType)type);
		}

		if( Parms("WORLD")->asBool() )
		{
			wxFileName	fn(file);

			switch( type )
			{
			default:					fn.SetExt(wxT("world"));	break;
			case wxBITMAP_TYPE_BMP:		fn.SetExt(wxT("bpw"));		break;
			case wxBITMAP_TYPE_GIF:		fn.SetExt(wxT("gfw"));		break;
			case wxBITMAP_TYPE_JPEG:	fn.SetExt(wxT("jgw"));		break;
			case wxBITMAP_TYPE_PNG:		fn.SetExt(wxT("pgw"));		break;
			case wxBITMAP_TYPE_PCX:		fn.SetExt(wxT("pxw"));		break;
			case wxBITMAP_TYPE_TIF:		fn.SetExt(wxT("tfw"));		break; 
			}

			if( Stream.Open(fn.GetFullPath().wx_str(), SG_FILE_W, false) )
			{
				Stream.Printf(SG_T("%.10f\n%.10f\n%.10f\n%.10f\n%.10f\n%.10f\n"),
					 m_pGrid->Get_Cellsize(),
					 0.0, 0.0,
					-m_pGrid->Get_Cellsize(),
					 m_pGrid->Get_XMin(),
					 m_pGrid->Get_YMax()
				);
			}
		}
	}
}

//---------------------------------------------------------
bool CWKSP_Grid::Get_Image_Grid(wxBitmap &BMP, bool bFitSize)
{
	if( bFitSize || (BMP.GetWidth() > 0 && BMP.GetHeight() > 0) )
	{
		Set_Buisy_Cursor(true);

		if( bFitSize )
		{
			BMP.Create(m_pGrid->Get_NX(), m_pGrid->Get_NY());
		}

		wxMemoryDC		dc;
		wxRect			r(0, 0, BMP.GetWidth(), BMP.GetHeight());
		CWKSP_Map_DC	dc_Map(Get_Extent(), r, 1.0, SG_GET_RGB(255, 255, 255));

		On_Draw(dc_Map, false);

		dc.SelectObject(BMP);
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
bool CWKSP_Grid::Get_Image_Legend(wxBitmap &BMP, double Zoom)
{
	if( Zoom > 0.0 )
	{
		wxMemoryDC	dc;
		wxSize		s(Get_Legend()->Get_Size(Zoom, 1.0));

		BMP.Create(s.GetWidth(), s.GetHeight());

		dc.SelectObject(BMP);
		dc.SetBackground(*wxWHITE_BRUSH);
		dc.Clear();

		Get_Legend()->Draw(dc, Zoom, 1.0, wxPoint(0, 0));

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

		m_pClassify->Set_Shade_Mode(m_Parameters("SHADE_MODE")->asInt());

		if( dc_Map.IMG_Draw_Begin(Transparency) )
		{
			Interpolation	= m_pClassify->Get_Mode() == CLASSIFY_LUT
							? GRID_INTERPOLATION_NearestNeighbour
							: m_Parameters("DISPLAY_INTERPOLATION")->asInt();

			if(	dc_Map.m_DC2World >= m_pGrid->Get_Cellsize()
			||	Interpolation != GRID_INTERPOLATION_NearestNeighbour || m_bOverlay )
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
	bool		bByteWise	= m_pClassify->Get_Mode() == CLASSIFY_RGB;
	int			axDC, ayDC, bxDC, byDC, nyDC, r, g, b;
	CSG_Rect	rMap(dc_Map.m_rWorld);

	switch( m_Parameters("OVERLAY_MODE")->asInt() )
	{
	default:
	case 0:	r = 0; g = 1; b = 2;	break;
	case 1:	r = 0; g = 2; b = 1;	break;
	case 2:	r = 1; g = 0; b = 2;	break;
	case 3:	r = 2; g = 0; b = 1;	break;
	case 4:	r = 1; g = 2; b = 0;	break;
	case 5:	r = 2; g = 1; b = 0;	break;
	}

	m_pOverlay[0]	= g_pData->Get_Grids()->Get_Grid(m_Parameters("OVERLAY_1")->asGrid());
	m_pOverlay[1]	= g_pData->Get_Grids()->Get_Grid(m_Parameters("OVERLAY_2")->asGrid());

	rMap.Intersect(m_pGrid->Get_Extent(true));

	axDC	= (int)dc_Map.xWorld2DC(rMap.Get_XMin());	if( axDC < 0 )	axDC	= 0;
	bxDC	= (int)dc_Map.xWorld2DC(rMap.Get_XMax());	if( bxDC >= dc_Map.m_rDC.GetWidth () )	bxDC	= dc_Map.m_rDC.GetWidth () - 1;
	ayDC	= (int)dc_Map.yWorld2DC(rMap.Get_YMin());	if( ayDC >= dc_Map.m_rDC.GetHeight() )	ayDC	= dc_Map.m_rDC.GetHeight() - 1;
	byDC	= (int)dc_Map.yWorld2DC(rMap.Get_YMax());	if( byDC < 0 )	byDC	= 0;
	nyDC	= abs(ayDC - byDC);

	#pragma omp parallel for
	for(int iyDC=0; iyDC<=nyDC; iyDC++)
	{
		int		xDC , yDC	= ayDC - iyDC;
		double	xMap, yMap	= dc_Map.yDC2World(yDC);

		for(xMap=dc_Map.xDC2World(axDC), xDC=axDC; xDC<=bxDC; xMap+=dc_Map.m_DC2World, xDC++)
		{
			double	Value;

			if( m_pGrid->Get_Value(xMap, yMap, Value, Interpolation, false, bByteWise, true) )
			{
				if( m_bOverlay == false )
				{
					int		c;

					if( m_pClassify->Get_Class_Color_byValue(Value, c) )
					{
						dc_Map.IMG_Set_Pixel(xDC, yDC, c);
					}
				}
				else
				{
					int		c[3];

					c[0]	= (int)(255.0 * m_pClassify->Get_MetricToRelative(Value));

					c[1]	= m_pOverlay[0] && m_pOverlay[0]->Get_Grid()->Get_Value(xMap, yMap, Value, Interpolation, false, false, true)
							? (int)(255.0 * m_pOverlay[0]->m_pClassify->Get_MetricToRelative(Value)) : 255;

					c[2]	= m_pOverlay[1] && m_pOverlay[1]->Get_Grid()->Get_Value(xMap, yMap, Value, Interpolation, false, false, true)
							? (int)(255.0 * m_pOverlay[1]->m_pClassify->Get_MetricToRelative(Value)) : 255;

					dc_Map.IMG_Set_Pixel(xDC, yDC, SG_GET_RGB(
						c[r] < 0 ? 0 : c[r] > 255 ? 255 : c[r],
						c[g] < 0 ? 0 : c[g] > 255 ? 255 : c[g],
						c[b] < 0 ? 0 : c[b] > 255 ? 255 : c[b]
					));
				}
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
	if(	!m_Parameters("VALUES_SHOW")->asBool() || m_pGrid->Get_Cellsize() * dc_Map.m_World2DC <= 40 )
	{
		return;
	}

	//-----------------------------------------------------
	int			ax, ay, bx, by, Decimals, Effect;
	double		axDC, ayDC, dDC, zFactor;
	wxColour	Effect_Color;
	wxFont		Font;

	dDC			= m_pGrid->Get_Cellsize() * dc_Map.m_World2DC;
	zFactor		= m_pGrid->Get_ZFactor();
	Decimals	= m_Parameters("VALUES_DECIMALS")->asInt();
	Font		= Get_Font(m_Parameters("VALUES_FONT"));
	Font.SetPointSize((int)(dDC * m_Parameters("VALUES_SIZE")->asDouble() / 100.0));
	dc_Map.dc.SetFont(Font);
	dc_Map.dc.SetTextForeground(Get_Color_asWX(m_Parameters("VALUES_FONT")->asColor()));

	switch( m_Parameters("VALUES_EFFECT")->asInt() )
	{
	default:	Effect	= TEXTEFFECT_NONE;			break;
	case 1:		Effect	= TEXTEFFECT_FRAME;			break;
	case 2:		Effect	= TEXTEFFECT_TOP;			break;
	case 3:		Effect	= TEXTEFFECT_TOPLEFT;		break;
	case 4:		Effect	= TEXTEFFECT_LEFT;			break;
	case 5:		Effect	= TEXTEFFECT_BOTTOMLEFT;	break;
	case 6:		Effect	= TEXTEFFECT_BOTTOM;		break;
	case 7:		Effect	= TEXTEFFECT_BOTTOMRIGHT;	break;
	case 8:		Effect	= TEXTEFFECT_RIGHT;			break;
	case 9:		Effect	= TEXTEFFECT_TOPRIGHT;		break;
	}

	Effect_Color	= m_Parameters("VALUES_EFFECT_COLOR")->asColor();

	//-------------------------------------------------
	ax		= m_pGrid->Get_System().Get_xWorld_to_Grid(dc_Map.m_rWorld.Get_XMin());
	ay		= m_pGrid->Get_System().Get_yWorld_to_Grid(dc_Map.m_rWorld.Get_YMin());
	bx		= m_pGrid->Get_System().Get_xWorld_to_Grid(dc_Map.m_rWorld.Get_XMax());
	by		= m_pGrid->Get_System().Get_yWorld_to_Grid(dc_Map.m_rWorld.Get_YMax());

	if( ax < 0 )	ax	= 0;	if( bx >= m_pGrid->Get_NX() )	bx	= m_pGrid->Get_NX() - 1;
	if( ay < 0 )	ax	= 0;	if( by >= m_pGrid->Get_NY() )	by	= m_pGrid->Get_NY() - 1;

	axDC	= dc_Map.xWorld2DC(m_pGrid->Get_System().Get_xGrid_to_World(ax));
	ayDC	= dc_Map.yWorld2DC(m_pGrid->Get_System().Get_yGrid_to_World(ay));

	int	ny	= by - ay;

	//-------------------------------------------------
	for(int y=0; y<=ny; y++)
	{
		int		x;
		double	xDC, yDC	= ayDC - y * dDC;// dc_Map.m_World2DC;

		for(x=ax, xDC=axDC; x<=bx; x++, xDC+=dDC)
		{
			if( m_pGrid->is_InGrid(x, ay + y) )
			{
				double	Value	= m_pGrid->asDouble(x, ay + y);

				switch( m_pClassify->Get_Mode() )
				{
				case CLASSIFY_RGB:
					Draw_Text(dc_Map.dc, TEXTALIGN_CENTER, (int)xDC, (int)yDC, wxString::Format(
						wxT("R%03d G%03d B%03d"), SG_GET_R((int)Value), SG_GET_G((int)Value), SG_GET_B((int)Value)
					), Effect, Effect_Color);
					break;

				default:
					Draw_Text(dc_Map.dc, TEXTALIGN_CENTER, (int)xDC, (int)yDC, wxString::Format(
						wxT("%.*f"), Decimals, zFactor * Value
					), Effect, Effect_Color);
					break;
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
		CSG_Rect	r(
			-m_pGrid->Get_Cellsize() / 2.0 + m_pGrid->Get_System().Get_xGrid_to_World(m_Sel_xOff),
			-m_pGrid->Get_Cellsize() / 2.0 + m_pGrid->Get_System().Get_yGrid_to_World(m_Sel_yOff),
			-m_pGrid->Get_Cellsize() / 2.0 + m_pGrid->Get_System().Get_xGrid_to_World(m_Sel_xOff + m_Sel_xN),
			-m_pGrid->Get_Cellsize() / 2.0 + m_pGrid->Get_System().Get_yGrid_to_World(m_Sel_yOff + m_Sel_yN)
		);

		TSG_Point_Int		a(dc_Map.World2DC(r.Get_TopLeft())),
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

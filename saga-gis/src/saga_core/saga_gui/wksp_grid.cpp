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
// 51 Franklin Street, 5th Floor, Boston, MA 02110-1301, //
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
	m_pOverlay[0]	= NULL;
	m_pOverlay[1]	= NULL;

	//-----------------------------------------------------
	On_Create_Parameters();

	DataObject_Changed();

	Fit_Color_Range();
}


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
	s	+= wxString::Format(wxT("<b>%s</b>"), _TL("Grid"));

	s	+= wxT("<table border=\"0\">");

	DESC_ADD_STR (_TL("Name")				, m_pObject->Get_Name());
	DESC_ADD_STR (_TL("Description")		, m_pObject->Get_Description());
	DESC_ADD_STR (_TL("File")				, SG_File_Exists(m_pObject->Get_File_Name()) ? m_pObject->Get_File_Name() : _TL("memory"));
	DESC_ADD_STR (_TL("Modified")			, m_pObject->is_Modified() ? _TL("yes") : _TL("no"));
	DESC_ADD_STR (_TL("Projection")			, m_pObject->Get_Projection().Get_Description().c_str());
	DESC_ADD_FLT (_TL("West")				, Get_Grid()->Get_XMin());
	DESC_ADD_FLT (_TL("East")				, Get_Grid()->Get_XMax());
	DESC_ADD_FLT (_TL("West-East")			, Get_Grid()->Get_XRange());
	DESC_ADD_FLT (_TL("South")				, Get_Grid()->Get_YMin());
	DESC_ADD_FLT (_TL("North")				, Get_Grid()->Get_YMax());
	DESC_ADD_FLT (_TL("South-North")		, Get_Grid()->Get_YRange());
	DESC_ADD_FLT (_TL("Cell Size")			, Get_Grid()->Get_Cellsize());
	DESC_ADD_INT (_TL("Number of Columns")	, Get_Grid()->Get_NX());
	DESC_ADD_INT (_TL("Number of Rows")		, Get_Grid()->Get_NY());
	DESC_ADD_LONG(_TL("Number of Cells")	, Get_Grid()->Get_NCells());
	DESC_ADD_LONG(_TL("No Data Cells")		, Get_Grid()->Get_NoData_Count());
	DESC_ADD_STR (_TL("Value Type")			, SG_Data_Type_Get_Name(Get_Grid()->Get_Type()).c_str());
	DESC_ADD_FLT (_TL("Value Minimum")		, Get_Grid()->Get_ZMin());
	DESC_ADD_FLT (_TL("Value Maximum")		, Get_Grid()->Get_ZMax());
	DESC_ADD_FLT (_TL("Value Range")		, Get_Grid()->Get_ZRange());
	DESC_ADD_STR (_TL("No Data Value")		, Get_Grid()->Get_NoData_Value() < Get_Grid()->Get_NoData_hiValue() ? CSG_String::Format(SG_T("%f - %f"), Get_Grid()->Get_NoData_Value(), Get_Grid()->Get_NoData_hiValue()).c_str() : SG_Get_String(Get_Grid()->Get_NoData_Value(), -2).c_str());
	DESC_ADD_FLT (_TL("Arithmetic Mean")	, Get_Grid()->Get_ArithMean(true));
	DESC_ADD_FLT (_TL("Standard Deviation")	, Get_Grid()->Get_StdDev(true));
	DESC_ADD_STR (_TL("Memory Size")		, Get_nBytes_asString(Get_Grid()->Get_NCells() * Get_Grid()->Get_nValueBytes(), 2).c_str());

	if( Get_Grid()->is_Compressed() )
	{
		DESC_ADD_FLT(_TL("Memory Compression")	, 100.0 * Get_Grid()->Get_Compression_Ratio());
	}

	if( Get_Grid()->is_Cached() )
	{
		DESC_ADD_FLT(_TL("File Cache [MB]")		, Get_Grid()->Get_Buffer_Size() / (double)N_MEGABYTE_BYTES);
	}

	s	+= wxT("</table>");

	//-----------------------------------------------------
//	s.Append(wxString::Format(wxT("<hr><b>%s</b><font size=\"-1\">"), _TL("Data History")));
//	s.Append(Get_Grid()->Get_History().Get_HTML());
//	s.Append(wxString::Format(wxT("</font")));

	return( s );
}

//---------------------------------------------------------
wxMenu * CWKSP_Grid::Get_Menu(void)
{
	wxMenu	*pMenu	= new wxMenu(m_pObject->Get_Name());

	CMD_Menu_Add_Item(pMenu		, false, ID_CMD_WKSP_ITEM_CLOSE);
	CMD_Menu_Add_Item(pMenu		, false, ID_CMD_GRIDS_SAVE);
	CMD_Menu_Add_Item(pMenu		, false, ID_CMD_GRIDS_SAVEAS);
	CMD_Menu_Add_Item(pMenu		, false, ID_CMD_GRIDS_SAVEAS_IMAGE);
	CMD_Menu_Add_Item(pMenu		, false, ID_CMD_GRIDS_SHOW);

	pMenu->AppendSeparator();

	CMD_Menu_Add_Item(pMenu		, true , ID_CMD_GRIDS_HISTOGRAM);

	CMD_Menu_Add_Item(pMenu		, false, ID_CMD_GRIDS_SCATTERPLOT);
	CMD_Menu_Add_Item(pMenu		, false, ID_CMD_WKSP_ITEM_SETTINGS_COPY);

	//-----------------------------------------------------

	//-----------------------------------------------------
	wxMenu	*pSubMenu	= new wxMenu(_TL("Classification"));

	CMD_Menu_Add_Item(pSubMenu	, false, ID_CMD_GRIDS_SET_LUT);
	CMD_Menu_Add_Item(pSubMenu	, false, ID_CMD_GRIDS_FIT_MINMAX);
	CMD_Menu_Add_Item(pSubMenu	, false, ID_CMD_GRIDS_FIT_STDDEV);

	pMenu->Append(ID_CMD_WKSP_FIRST, _TL("Classification"), pSubMenu);

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

	case ID_CMD_GRIDS_FIT_MINMAX:
		Set_Color_Range(
			Get_Grid()->Get_ZMin(true),
			Get_Grid()->Get_ZMax(true)
		);
		break;

	case ID_CMD_GRIDS_FIT_STDDEV:
		{
			double	d	= g_pData->Get_Parameter("GRID_COLORS_FIT_STDDEV")->asDouble();

			Set_Color_Range(
				Get_Grid()->Get_ArithMean(true) - d * Get_Grid()->Get_StdDev(true),
				Get_Grid()->Get_ArithMean(true) + d * Get_Grid()->Get_StdDev(true)
			);
		}
		break;

	case ID_CMD_GRIDS_FIT_DIALOG:
		break;

	case ID_CMD_GRIDS_SET_LUT:
		_LUT_Create();
		break;

	case ID_CMD_GRIDS_SEL_CLEAR:
		_Edit_Clr_Selection();
		break;

	case ID_CMD_GRIDS_SEL_DELETE:
		_Edit_Del_Selection();
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

	case ID_CMD_GRIDS_SEL_CLEAR:
		event.Enable(m_Edit_Attributes.Get_Count() > 0);
		break;

	case ID_CMD_GRIDS_SEL_DELETE:
		event.Enable(m_Edit_Attributes.Get_Count() > 0);
		break;

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
		m_Parameters("NODE_GENERAL")	, "GENERAL_Z_UNIT"			, _TL("Unit"),
		_TL(""),
		Get_Grid()->Get_Unit()
	);

	m_Parameters.Add_Value(
		m_Parameters("NODE_GENERAL")	, "GENERAL_Z_FACTOR"		, _TL("Z-Factor"),
		_TL(""),
		PARAMETER_TYPE_Double
	);

	//-----------------------------------------------------
	// Display...

	m_Parameters.Add_Choice(
		m_Parameters("NODE_DISPLAY")	, "DISPLAY_INTERPOLATION"	, _TL("Interpolation"),
		_TL(""),
		CSG_String::Format(SG_T("%s|%s|%s|%s|%s|"),
			_TL("None"),
			_TL("Bilinear"),
			_TL("Inverse Distance"),
			_TL("Bicubic Spline"),
			_TL("B-Spline")
		), 0
	);

	//-----------------------------------------------------
	// Classification...

	((CSG_Parameter_Choice *)m_Parameters("COLORS_TYPE")->Get_Data())->Set_Items(
		CSG_String::Format(SG_T("%s|%s|%s|%s|%s|%s|%s|"),
			_TL("Single Symbol"   ),	// CLASSIFY_UNIQUE
			_TL("Lookup Table"    ),	// CLASSIFY_LUT
			_TL("Discrete Colors" ),	// CLASSIFY_METRIC
			_TL("Graduated Colors"),	// CLASSIFY_GRADUATED
			_TL("Shade"           ),	// CLASSIFY_SHADE
			_TL("RGB Overlay"     ),	// CLASSIFY_OVERLAY
			_TL("RGB"             )		// CLASSIFY_RGB
		)
	);

	m_Parameters("COLORS_TYPE")->Set_Value(CLASSIFY_GRADUATED);

	//-----------------------------------------------------
	m_Parameters.Add_Node(
		m_Parameters("NODE_COLORS")		, "NODE_SHADE"		, _TL("Shade"),
		_TL("")
	);

	m_Parameters.Add_Choice(
		m_Parameters("NODE_SHADE")		, "SHADE_MODE"		, _TL("Coloring"),
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
		m_Parameters("NODE_COLORS")		, "NODE_OVERLAY"	, _TL("RGB Overlay"),
		_TL("")
	);

	m_Parameters.Add_Choice(
		m_Parameters("NODE_OVERLAY")	, "OVERLAY_MODE"	, _TL("Coloring"),
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
		m_Parameters("NODE_OVERLAY")	, "OVERLAY_1"		, _TL("Overlay 1"),
		_TL(""),
		PARAMETER_INPUT_OPTIONAL, false
	)->Get_Parent()->Set_Value((void *)&Get_Grid()->Get_System());

	m_Parameters.Add_Grid(
		m_Parameters("NODE_OVERLAY")	, "OVERLAY_2"		, _TL("Overlay 2"),
		_TL(""),
		PARAMETER_INPUT_OPTIONAL, false
	)->Get_Parent()->Set_Value((void *)&Get_Grid()->Get_System());

	//-----------------------------------------------------
	// Cell Values...

	m_Parameters.Add_Value(
		m_Parameters("NODE_GENERAL")	, "VALUES_SHOW"		, _TL("Show Cell Values"),
		_TL("shows cell values when zoomed"),
		PARAMETER_TYPE_Bool, false
	);

	m_Parameters.Add_Font(
		m_Parameters("VALUES_SHOW")		, "VALUES_FONT"		, _TL("Font"),
		_TL("")
	);

	m_Parameters.Add_Value(
		m_Parameters("VALUES_SHOW")		, "VALUES_SIZE"		, _TL("Size"),
		_TL(""),
		PARAMETER_TYPE_Double, 15, 0, true , 100.0, true
	);

	m_Parameters.Add_Value(
		m_Parameters("VALUES_SHOW")		, "VALUES_DECIMALS"	, _TL("Decimals"),
		_TL(""),
		PARAMETER_TYPE_Int, 2
	);

	m_Parameters.Add_Choice(
		m_Parameters("VALUES_SHOW")		, "VALUES_EFFECT"	, _TL("Boundary Effect"),
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
		m_Parameters("VALUES_EFFECT")	, "VALUES_EFFECT_COLOR"	, _TL("Color"),
		_TL(""),
		PARAMETER_TYPE_Color, SG_GET_RGB(255, 255, 255)
	);

	//-----------------------------------------------------
	// Memory...

	m_Parameters.Add_Choice(
		m_Parameters("NODE_GENERAL")	, "MEMORY_MODE"				, _TL("Memory Handling"),
		_TL(""),
		CSG_String::Format(SG_T("%s|%s|%s|"),
			_TL("Normal"),
			_TL("RTL Compression"),
			_TL("File Cache")
		), 0
	);

	m_Parameters.Add_Value(
		m_Parameters("MEMORY_MODE")		, "MEMORY_BUFFER_SIZE"		, _TL("Buffer Size MB"),
		_TL(""),
		PARAMETER_TYPE_Double
	);
}


///////////////////////////////////////////////////////////
//														 //
//														 //
//														 //
///////////////////////////////////////////////////////////

//---------------------------------------------------------
void CWKSP_Grid::On_DataObject_Changed(void)
{
	CWKSP_Layer::On_DataObject_Changed();

	//-----------------------------------------------------
	m_Parameters("GENERAL_Z_UNIT"    )->Set_Value(
		Get_Grid()->Get_Unit()
	);

	m_Parameters("GENERAL_Z_FACTOR"  )->Set_Value(
		Get_Grid()->Get_ZFactor()
	);

	//-----------------------------------------------------
	m_Parameters("MEMORY_MODE"       )->Set_Value(
		Get_Grid()->is_Compressed() ? 1 : (Get_Grid()->is_Cached() ? 2 : 0)
	);

	m_Parameters("MEMORY_BUFFER_SIZE")->Set_Value(
		(double)Get_Grid()->Get_Buffer_Size() / N_MEGABYTE_BYTES
	);
}

//---------------------------------------------------------
void CWKSP_Grid::On_Parameters_Changed(void)
{
	CWKSP_Layer::On_Parameters_Changed();

	//-----------------------------------------------------
	Get_Grid()->Set_Unit   (m_Parameters("GENERAL_Z_UNIT"  )->asString());
	Get_Grid()->Set_ZFactor(m_Parameters("GENERAL_Z_FACTOR")->asDouble());

	//-----------------------------------------------------
	m_pOverlay[0]	= (CWKSP_Grid *)g_pData->Get(m_Parameters("OVERLAY_1")->asGrid());
	m_pOverlay[1]	= (CWKSP_Grid *)g_pData->Get(m_Parameters("OVERLAY_2")->asGrid());
	m_bOverlay		= m_Parameters("COLORS_TYPE")->asInt() == CLASSIFY_OVERLAY;

	m_pClassify->Set_Shade_Mode(m_Parameters("SHADE_MODE")->asInt());

	//-----------------------------------------------------
	switch( m_Parameters("MEMORY_MODE")->asInt() )
	{
	case 0:
		if( Get_Grid()->is_Compressed() )
		{
			Get_Grid()->Set_Compression(false);
		}
		else if( Get_Grid()->is_Cached() )
		{
			Get_Grid()->Set_Cache(false);
		}
		break;

	case 1:
		if( !Get_Grid()->is_Compressed() )
		{
			Get_Grid()->Set_Compression(true);
		}
		break;

	case 2:
		if( !Get_Grid()->is_Cached() )
		{
			Get_Grid()->Set_Cache(true);
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

			for(long iCell=0, jCell; iCell<Get_Grid()->Get_NCells() && PROGRESSBAR_Set_Position(iCell, Get_Grid()->Get_NCells()); iCell++)
			{
				if( Get_Grid()->Get_Sorted(iCell, jCell, false) && (pLUT->Get_Record_Count() == 0 || Value != Get_Grid()->asDouble(jCell)) )
				{
					Value	= Get_Grid()->asDouble(jCell);

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

			Interval	= Get_Grid()->Get_ZRange() / (double)pColors->Get_Count();
			Minimum		= Get_Grid()->Get_ZMin  ();

			for(int iClass=0; iClass<pColors->Get_Count(); iClass++, Minimum+=Interval)
			{
				Maximum	= iClass < pColors->Get_Count() - 1 ? Minimum + Interval : Get_Grid()->Get_ZMax() + 1.0;

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
			if( Get_Grid()->Get_NCells() < pColors->Get_Count() )
			{
				pColors->Set_Count(Get_Grid()->Get_NCells());
			}

			long	jCell, nCells;
			double	Minimum, Maximum, iCell, Count;

			Maximum	= Get_Grid()->Get_ZMin();
			nCells	= Get_Grid()->Get_NCells() - Get_Grid()->Get_NoData_Count();
			iCell	= Count	= nCells / (double)pColors->Get_Count();

			for(iCell=0.0; iCell<Get_Grid()->Get_NCells(); iCell++)
			{
				if( Get_Grid()->Get_Sorted((long)iCell, jCell, false) )
				{
					break;
				}
			}

			iCell	+= Count;

			for(int iClass=0; iClass<pColors->Get_Count(); iClass++, iCell+=Count)
			{
				Get_Grid()->Get_Sorted((long)iCell, jCell, false);

				Minimum	= Maximum;
				Maximum	= iCell < Get_Grid()->Get_NCells() ? Get_Grid()->asDouble(jCell) : Get_Grid()->Get_ZMax() + 1.0;

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

	if( Get_Grid()->Get_Value(ptWorld, Value, GRID_INTERPOLATION_NearestNeighbour, true) )
	{
		switch( m_pClassify->Get_Mode() )
		{
		case CLASSIFY_LUT:
			s	= m_pClassify->Get_Class_Name_byValue(Value);
			break;

		default:
			switch( Get_Grid()->Get_Type() )
			{
			case SG_DATATYPE_Byte:
			case SG_DATATYPE_Char:
			case SG_DATATYPE_Word:
			case SG_DATATYPE_Short:
			case SG_DATATYPE_DWord:
			case SG_DATATYPE_Int:
				s.Printf(wxT("%d%s"), (int)Value, Get_Grid()->Get_Unit());
				break;

			case SG_DATATYPE_Float:	default:
			case SG_DATATYPE_Double:
				s.Printf(wxT("%f%s"), Value, Get_Grid()->Get_Unit());
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
	return( Get_Grid()->Get_ZRange() );
}


///////////////////////////////////////////////////////////
//														 //
//														 //
//														 //
///////////////////////////////////////////////////////////

//---------------------------------------------------------
wxMenu * CWKSP_Grid::On_Edit_Get_Menu(void)
{
	if( m_Edit_Attributes.Get_Count() < 1 )
	{
		return( NULL );
	}

	wxMenu	*pMenu	= new wxMenu;

	CMD_Menu_Add_Item(pMenu, true , ID_CMD_GRIDS_SEL_CLEAR);
	CMD_Menu_Add_Item(pMenu, true , ID_CMD_GRIDS_SEL_DELETE);

	return( pMenu );
}

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
bool CWKSP_Grid::On_Edit_On_Mouse_Up(CSG_Point Point, double ClientToWorld, int Key)
{
	if( Key & MODULE_INTERACTIVE_KEY_LEFT )
	{
		g_pACTIVE->Get_Attributes()->Set_Attributes();

		CSG_Rect	rWorld(m_Edit_Mouse_Down, Point);

		m_xSel	= Get_Grid()->Get_System().Get_xWorld_to_Grid(rWorld.Get_XMin()); if( m_xSel < 0 ) m_xSel = 0;
		int	nx	= Get_Grid()->Get_System().Get_xWorld_to_Grid(rWorld.Get_XMax()); if( nx >= Get_Grid()->Get_NX() ) nx = Get_Grid()->Get_NX() - 1;
		nx	= 1 + nx - m_xSel;

		m_ySel	= Get_Grid()->Get_System().Get_yWorld_to_Grid(rWorld.Get_YMin()); if( m_ySel < 0 ) m_ySel = 0;
		int	ny	= Get_Grid()->Get_System().Get_yWorld_to_Grid(rWorld.Get_YMax()); if( ny >= Get_Grid()->Get_NY() ) ny = Get_Grid()->Get_NY() - 1;
		ny	= 1 + ny - m_ySel;

		m_Edit_Attributes.Destroy();

		if( nx > 0 && ny > 0 )
		{
			int	x, y, Maximum = g_pData->Get_Parameter("GRID_SELECT_MAX")->asInt();

			if( nx > Maximum )
			{
				m_xSel	+= (nx - Maximum) / 2;
				nx		= Maximum;
			}

			if( ny > Maximum )
			{
				m_ySel	+= (ny - Maximum) / 2;
				ny		= Maximum;
			}

			for(x=0; x<nx; x++)
			{
				m_Edit_Attributes.Add_Field(CSG_String::Format(SG_T("%d"), x + 1), SG_DATATYPE_Double);
			}

			for(y=0; y<ny; y++)
			{
				CSG_Table_Record	*pRecord	= m_Edit_Attributes.Add_Record();

				for(x=0; x<nx; x++)
				{
					if( !Get_Grid()->is_NoData(m_xSel + x, m_ySel + ny - 1 - y) )
					{
						pRecord->Set_Value(x, Get_Grid()->asDouble(m_xSel + x, m_ySel + ny - 1 - y, false));
					}
					else
					{
						pRecord->Set_NoData(x);
					}
				}
			}
		}

		g_pACTIVE->Get_Attributes()->Set_Attributes();

		Update_Views();

		return( true );
	}

	return( false );
}

//---------------------------------------------------------
bool CWKSP_Grid::On_Edit_Set_Attributes(void)
{
	if( m_Edit_Attributes.Get_Count() > 0 )
	{
		for(int y=0; y<m_Edit_Attributes.Get_Count(); y++)
		{
			CSG_Table_Record	*pRecord	= m_Edit_Attributes.Get_Record(m_Edit_Attributes.Get_Count() - 1 - y);

			for(int x=0; x<m_Edit_Attributes.Get_Field_Count(); x++)
			{
				if( !pRecord->is_NoData(x) )
				{
					Get_Grid()->Set_Value(m_xSel + x, m_ySel + y, pRecord->asDouble(x));
				}
				else
				{
					Get_Grid()->Set_NoData(m_xSel + x, m_ySel + y);
				}
			}
		}

		Update_Views();

		return( true );
	}

	return( false );
}

//---------------------------------------------------------
TSG_Rect CWKSP_Grid::On_Edit_Get_Extent(void)
{
	if( m_Edit_Attributes.Get_Count() > 0 )
	{
		return( CSG_Rect(
			-Get_Grid()->Get_Cellsize() / 2.0 + Get_Grid()->Get_System().Get_xGrid_to_World(m_xSel),
			-Get_Grid()->Get_Cellsize() / 2.0 + Get_Grid()->Get_System().Get_yGrid_to_World(m_ySel),
			-Get_Grid()->Get_Cellsize() / 2.0 + Get_Grid()->Get_System().Get_xGrid_to_World(m_xSel + m_Edit_Attributes.Get_Field_Count()),
			-Get_Grid()->Get_Cellsize() / 2.0 + Get_Grid()->Get_System().Get_yGrid_to_World(m_ySel + m_Edit_Attributes.Get_Count()))
		);
	}

	return( Get_Grid()->Get_Extent().m_rect );
}


///////////////////////////////////////////////////////////
//														 //
//														 //
//														 //
///////////////////////////////////////////////////////////

//---------------------------------------------------------
bool CWKSP_Grid::_Edit_Clr_Selection(void)
{
	if( m_Edit_Attributes.Get_Count() > 0 )
	{
		m_Edit_Attributes.Destroy();

		g_pACTIVE->Get_Attributes()->Set_Attributes();

		Update_Views();

		return( true );
	}

	return( false );
}

//---------------------------------------------------------
bool CWKSP_Grid::_Edit_Del_Selection(void)
{
	if( m_Edit_Attributes.Get_Count() > 0 && DLG_Message_Confirm(_TL("Set selected values to no data."), _TL("Delete")) )
	{
		for(int y=0; y<m_Edit_Attributes.Get_Count(); y++)
		{
			for(int x=0; x<m_Edit_Attributes.Get_Field_Count(); x++)
			{
				m_Edit_Attributes[y].Set_NoData(x);

				Get_Grid()->Set_NoData(m_xSel + x, m_ySel + y);
			}
		}

		g_pACTIVE->Get_Attributes()->Set_Attributes();

		Update_Views();

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
bool CWKSP_Grid::Fit_Color_Range(void)
{
	double		zMin, zMax;

	int		Method	= g_pData->Get_Parameter("GRID_COLORS_FIT")->asInt();

	if( Method == 0 )
	{
		zMin	= Get_Grid()->Get_ZMin(true);
		zMax	= Get_Grid()->Get_ZMax(true);
	}
	else
	{
		double	d	= g_pData->Get_Parameter("GRID_COLORS_FIT_STDDEV")->asDouble();

		zMin	= Get_Grid()->Get_ArithMean(true) - Get_Grid()->Get_StdDev(true) * d;
		zMax	= Get_Grid()->Get_ArithMean(true) + Get_Grid()->Get_StdDev(true) * d;

		if( zMin < Get_Grid()->Get_ZMin(true) )	zMin	= Get_Grid()->Get_ZMin(true);
		if( zMax > Get_Grid()->Get_ZMax(true) )	zMax	= Get_Grid()->Get_ZMax(true);
	}

	return( Set_Color_Range(zMin, zMax) );
}

//---------------------------------------------------------
bool CWKSP_Grid::Fit_Color_Range(CSG_Rect rWorld)
{
	if( !rWorld.Intersect(Get_Extent()) )
	{
		return( false );
	}

	int		xMin	= Get_Grid()->Get_System().Get_xWorld_to_Grid(rWorld.Get_XMin());
	int		yMin	= Get_Grid()->Get_System().Get_yWorld_to_Grid(rWorld.Get_YMin());
	int		xMax	= Get_Grid()->Get_System().Get_xWorld_to_Grid(rWorld.Get_XMax());
	int		yMax	= Get_Grid()->Get_System().Get_yWorld_to_Grid(rWorld.Get_YMax());

	double	zMin	= 1.0;
	double	zMax	= 0.0;

	for(int y=yMin; y<=yMax; y++)
	{
		for(int x=xMin; x<=xMax; x++)
		{
			if( Get_Grid()->is_InGrid(x, y) )
			{
				double	z	= Get_Grid()->asDouble(x, y);

				if( zMin > zMax )	{	zMin	= zMax	= z;	}
				else if( z < zMin )	{	zMin	= z;			}
				else if( z > zMax )	{	zMax	= z;			}
			}
		}
	}

	return( Set_Color_Range(zMin, zMax) );
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

		pImage->Create(Get_Grid(), SG_DATATYPE_Int);

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
	Parms.Set_Name(_TL("Save Grid as Image..."));

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
					 Get_Grid()->Get_Cellsize(),
					 0.0, 0.0,
					-Get_Grid()->Get_Cellsize(),
					 Get_Grid()->Get_XMin(),
					 Get_Grid()->Get_YMax()
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
			BMP.Create(Get_Grid()->Get_NX(), Get_Grid()->Get_NY());
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

			if(	dc_Map.m_DC2World >= Get_Grid()->Get_Cellsize()
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

	m_pOverlay[0]	= (CWKSP_Grid *)g_pData->Get(m_Parameters("OVERLAY_1")->asGrid());
	m_pOverlay[1]	= (CWKSP_Grid *)g_pData->Get(m_Parameters("OVERLAY_2")->asGrid());

	rMap.Intersect(Get_Grid()->Get_Extent(true));

	axDC	= (int)dc_Map.xWorld2DC(rMap.Get_XMin());	if( axDC < 0 )	axDC	= 0;
	bxDC	= (int)dc_Map.xWorld2DC(rMap.Get_XMax());	if( bxDC >= dc_Map.m_rDC.GetWidth () )	bxDC	= dc_Map.m_rDC.GetWidth () - 1;
	ayDC	= (int)dc_Map.yWorld2DC(rMap.Get_YMin());	if( ayDC >= dc_Map.m_rDC.GetHeight() )	ayDC	= dc_Map.m_rDC.GetHeight() - 1;
	byDC	= (int)dc_Map.yWorld2DC(rMap.Get_YMax());	if( byDC < 0 )	byDC	= 0;
	nyDC	= abs(ayDC - byDC);

	if( Get_Grid()->is_Cached() || Get_Grid()->is_Compressed() )
	{
		for(int iyDC=0; iyDC<=nyDC; iyDC++)
		{
			_Draw_Grid_Line(dc_Map, Interpolation, bByteWise, ayDC - iyDC, axDC, bxDC, r, g, b);
		}
	}
	else
	{
		#pragma omp parallel for
		for(int iyDC=0; iyDC<=nyDC; iyDC++)
		{
			_Draw_Grid_Line(dc_Map, Interpolation, bByteWise, ayDC - iyDC, axDC, bxDC, r, g, b);
		}
	}
}

//---------------------------------------------------------
void CWKSP_Grid::_Draw_Grid_Line(CWKSP_Map_DC &dc_Map, int Interpolation, bool bByteWise, int yDC, int axDC, int bxDC, int r, int g, int b)
{
	int		xDC;
	double	xMap, yMap	= dc_Map.yDC2World(yDC);

	for(xMap=dc_Map.xDC2World(axDC), xDC=axDC; xDC<=bxDC; xMap+=dc_Map.m_DC2World, xDC++)
	{
		double	Value;

		if( Get_Grid()->Get_Value(xMap, yMap, Value, Interpolation, false, bByteWise, true) )
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

//---------------------------------------------------------
void CWKSP_Grid::_Draw_Grid_Cells(CWKSP_Map_DC &dc_Map)
{
	int		x, y, xa, ya, xb, yb, xaDC, yaDC, xbDC, ybDC, Color;
	double	xDC, yDC, axDC, ayDC, dDC;

	//-----------------------------------------------------
	dDC		= Get_Grid()->Get_Cellsize() * dc_Map.m_World2DC;

	xa		= Get_Grid()->Get_System().Get_xWorld_to_Grid(dc_Map.m_rWorld.Get_XMin());
	ya		= Get_Grid()->Get_System().Get_yWorld_to_Grid(dc_Map.m_rWorld.Get_YMin());
	xb		= Get_Grid()->Get_System().Get_xWorld_to_Grid(dc_Map.m_rWorld.Get_XMax());
	yb		= Get_Grid()->Get_System().Get_yWorld_to_Grid(dc_Map.m_rWorld.Get_YMax());

	if( xa < 0 )	xa	= 0;	if( xb >= Get_Grid()->Get_NX() )	xb	= Get_Grid()->Get_NX() - 1;
	if( ya < 0 )	ya	= 0;	if( yb >= Get_Grid()->Get_NY() )	yb	= Get_Grid()->Get_NY() - 1;

	axDC	= dc_Map.xWorld2DC(Get_Grid()->Get_System().Get_xGrid_to_World(xa)) + dDC / 2.0;
	ayDC	= dc_Map.yWorld2DC(Get_Grid()->Get_System().Get_yGrid_to_World(ya)) - dDC / 2.0;

	//-----------------------------------------------------
	for(y=ya, yDC=ayDC, yaDC=(int)(ayDC), ybDC=(int)(ayDC+dDC); y<=yb; y++, ybDC=yaDC, yaDC=(int)(yDC-=dDC))
	{
		for(x=xa, xDC=axDC, xaDC=(int)(axDC-dDC), xbDC=(int)(axDC); x<=xb; x++, xaDC=xbDC, xbDC=(int)(xDC+=dDC))
		{
			if( Get_Grid()->is_InGrid(x, y) && m_pClassify->Get_Class_Color_byValue(Get_Grid()->asDouble(x, y), Color) )
			{
				dc_Map.IMG_Set_Rect(xaDC, yaDC, xbDC, ybDC, Color);
			}
		}
	}
}

//---------------------------------------------------------
void CWKSP_Grid::_Draw_Values(CWKSP_Map_DC &dc_Map)
{
	if(	!m_Parameters("VALUES_SHOW")->asBool() || Get_Grid()->Get_Cellsize() * dc_Map.m_World2DC <= 40 )
	{
		return;
	}

	//-----------------------------------------------------
	int			ax, ay, bx, by, Decimals, Effect;
	double		axDC, ayDC, dDC, zFactor;
	wxColour	Effect_Color;
	wxFont		Font;

	dDC			= Get_Grid()->Get_Cellsize() * dc_Map.m_World2DC;
	zFactor		= Get_Grid()->Get_ZFactor();
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
	ax		= Get_Grid()->Get_System().Get_xWorld_to_Grid(dc_Map.m_rWorld.Get_XMin());
	ay		= Get_Grid()->Get_System().Get_yWorld_to_Grid(dc_Map.m_rWorld.Get_YMin());
	bx		= Get_Grid()->Get_System().Get_xWorld_to_Grid(dc_Map.m_rWorld.Get_XMax());
	by		= Get_Grid()->Get_System().Get_yWorld_to_Grid(dc_Map.m_rWorld.Get_YMax());

	if( ax < 0 )	ax	= 0;	if( bx >= Get_Grid()->Get_NX() )	bx	= Get_Grid()->Get_NX() - 1;
	if( ay < 0 )	ax	= 0;	if( by >= Get_Grid()->Get_NY() )	by	= Get_Grid()->Get_NY() - 1;

	axDC	= dc_Map.xWorld2DC(Get_Grid()->Get_System().Get_xGrid_to_World(ax));
	ayDC	= dc_Map.yWorld2DC(Get_Grid()->Get_System().Get_yGrid_to_World(ay));

	int	ny	= by - ay;

	//-------------------------------------------------
	for(int y=0; y<=ny; y++)
	{
		int		x;
		double	xDC, yDC	= ayDC - y * dDC;// dc_Map.m_World2DC;

		for(x=ax, xDC=axDC; x<=bx; x++, xDC+=dDC)
		{
			if( Get_Grid()->is_InGrid(x, ay + y) )
			{
				double	Value	= Get_Grid()->asDouble(x, ay + y);

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
	if( m_Edit_Attributes.Get_Count() > 0 )
	{
		CSG_Rect	r(
			-Get_Grid()->Get_Cellsize() / 2.0 + Get_Grid()->Get_System().Get_xGrid_to_World(m_xSel),
			-Get_Grid()->Get_Cellsize() / 2.0 + Get_Grid()->Get_System().Get_yGrid_to_World(m_ySel),
			-Get_Grid()->Get_Cellsize() / 2.0 + Get_Grid()->Get_System().Get_xGrid_to_World(m_xSel + m_Edit_Attributes.Get_Field_Count()),
			-Get_Grid()->Get_Cellsize() / 2.0 + Get_Grid()->Get_System().Get_yGrid_to_World(m_ySel + m_Edit_Attributes.Get_Count())
		);

		TSG_Point_Int	a(dc_Map.World2DC(r.Get_TopLeft    ()));
		TSG_Point_Int	b(dc_Map.World2DC(r.Get_BottomRight()));

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

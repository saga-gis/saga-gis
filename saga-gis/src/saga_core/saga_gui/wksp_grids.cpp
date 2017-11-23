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
//          Copyright (C) 2017 by Olaf Conrad            //
//                                                       //
//-------------------------------------------------------//
//                                                       //
// This file is part of 'SAGA - System for Automated     //
// Geoscientific Analyses'. SAGA is free software; you   //
// can redistribute it and/or modify it under the terms  //
// of the GNU General Public License as published by the //
// Free Software Foundation, either version 2 of the     //
// License, or (at your option) any later version.       //
//                                                       //
// SAGA is distributed in the hope that it will be       //
// useful, but WITHOUT ANY WARRANTY; without even the    //
// implied warranty of MERCHANTABILITY or FITNESS FOR A  //
// PARTICULAR PURPOSE. See the GNU General Public        //
// License for more details.                             //
//                                                       //
// You should have received a copy of the GNU General    //
// Public License along with this program; if not, see   //
// <http://www.gnu.org/licenses/>.                       //
//                                                       //
//-------------------------------------------------------//
//                                                       //
//    contact:    Olaf Conrad                            //
//                Institute of Geography                 //
//                University of Hamburg                  //
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
#include "wksp_grids.h"

#include "data_source_pgsql.h"


///////////////////////////////////////////////////////////
//														 //
//														 //
//														 //
///////////////////////////////////////////////////////////

//---------------------------------------------------------
CWKSP_Grids::CWKSP_Grids(CSG_Grids *pGrids)
	: CWKSP_Layer(pGrids)
{
	On_Create_Parameters();

	DataObject_Changed();

	Fit_Colors();
}


///////////////////////////////////////////////////////////
//														 //
///////////////////////////////////////////////////////////

//---------------------------------------------------------
wxString CWKSP_Grids::Get_Description(void)
{
	wxString	s;

	//-----------------------------------------------------
	s	+= wxString::Format("<h4>%s</h4>", _TL("Grids"));

	s	+= "<table border=\"0\">";

	DESC_ADD_STR (_TL("Name"               ), m_pObject->Get_Name());
	DESC_ADD_STR (_TL("Description"        ), m_pObject->Get_Description());

	if( SG_File_Exists(m_pObject->Get_File_Name(false)) )
	{
		DESC_ADD_STR (_TL("File"           ), m_pObject->Get_File_Name(false));

		if( m_pObject->Get_MetaData()("GDAL_DRIVER") )
			DESC_ADD_STR (_TL("Driver"     ), m_pObject->Get_MetaData()["GDAL_DRIVER"].Get_Content().c_str());
	}
	else if( m_pObject->Get_MetaData_DB().Get_Children_Count() )
	{
		DESC_ADD_STR(_TL("File"            ), m_pObject->Get_File_Name(false));
		//const CSG_MetaData	&DB	= m_pObject->Get_MetaData_DB();
		//if( DB("DBMS") ) DESC_ADD_STR (_TL("DBMS"    ), DB["DBMS"].Get_Content().c_str());
		//if( DB("HOST") ) DESC_ADD_STR (_TL("Host"    ), DB["DBMS"].Get_Content().c_str());
		//if( DB("PORT") ) DESC_ADD_STR (_TL("Port"    ), DB["DBMS"].Get_Content().c_str());
		//if( DB("NAME") ) DESC_ADD_STR (_TL("Database"), DB["NAME"].Get_Content().c_str());
	}
	else
	{
		DESC_ADD_STR (_TL("File"          ), _TL("memory"));
	}

	DESC_ADD_STR (_TL("Modified"          ), m_pObject->is_Modified() ? _TL("yes") : _TL("no"));
	DESC_ADD_STR (_TL("Projection"        ), m_pObject->Get_Projection().Get_Description().c_str());
	DESC_ADD_STR (_TL("West"              ), SG_Get_String(Get_Grids()->Get_XMin        (), -CSG_Grid_System::Get_Precision()).c_str());
	DESC_ADD_STR (_TL("East"              ), SG_Get_String(Get_Grids()->Get_XMax        (), -CSG_Grid_System::Get_Precision()).c_str());
	DESC_ADD_STR (_TL("West-East"         ), SG_Get_String(Get_Grids()->Get_XRange      (), -CSG_Grid_System::Get_Precision()).c_str());
	DESC_ADD_STR (_TL("South"             ), SG_Get_String(Get_Grids()->Get_YMin        (), -CSG_Grid_System::Get_Precision()).c_str());
	DESC_ADD_STR (_TL("North"             ), SG_Get_String(Get_Grids()->Get_YMax        (), -CSG_Grid_System::Get_Precision()).c_str());
	DESC_ADD_STR (_TL("South-North"       ), SG_Get_String(Get_Grids()->Get_YRange      (), -CSG_Grid_System::Get_Precision()).c_str());
	DESC_ADD_STR (_TL("Cell Size"         ), SG_Get_String(Get_Grids()->Get_Cellsize    (), -CSG_Grid_System::Get_Precision()).c_str());
	DESC_ADD_INT (_TL("Number of Columns" ), Get_Grids()->Get_NX          ());
	DESC_ADD_INT (_TL("Number of Rows"    ), Get_Grids()->Get_NY          ());
	DESC_ADD_INT (_TL("Number of Bands"   ), Get_Grids()->Get_NZ          ());
	DESC_ADD_LONG(_TL("Number of Cells"   ), Get_Grids()->Get_NCells      ());
	DESC_ADD_LONG(_TL("No Data Cells"     ), Get_Grids()->Get_NoData_Count());
	DESC_ADD_STR (_TL("Z-Attribute"       ), Get_Grids()->Get_Attributes().Get_Field_Name(Get_Grids()->Get_Z_Attribute()));
	DESC_ADD_STR (_TL("Value Type"        ), SG_Data_Type_Get_Name(Get_Grids()->Get_Type()).c_str());
	DESC_ADD_FLT (_TL("Value Minimum"     ), Get_Grids()->Get_Min         ());
	DESC_ADD_FLT (_TL("Value Maximum"     ), Get_Grids()->Get_Max         ());
	DESC_ADD_FLT (_TL("Value Range"       ), Get_Grids()->Get_Range       ());
	DESC_ADD_STR (_TL("No Data Value"     ), Get_Grids()->Get_NoData_Value() < Get_Grids()->Get_NoData_hiValue() ? CSG_String::Format("%f - %f", Get_Grids()->Get_NoData_Value(), Get_Grids()->Get_NoData_hiValue()).c_str() : SG_Get_String(Get_Grids()->Get_NoData_Value(), -2).c_str());
	DESC_ADD_FLT (_TL("Arithmetic Mean"   ), Get_Grids()->Get_Mean        ());
	DESC_ADD_FLT (_TL("Standard Deviation"), Get_Grids()->Get_StdDev      ());
	DESC_ADD_STR (_TL("Memory Size"       ), Get_nBytes_asString(Get_Grids()->Get_Memory_Size(), 2).c_str());

	s	+= "</table>";

	s	+= Get_TableInfo_asHTML(&Get_Grids()->Get_Attributes());

	//-----------------------------------------------------
	return( s );
}

//---------------------------------------------------------
wxMenu * CWKSP_Grids::Get_Menu(void)
{
	wxMenu	*pMenu	= new wxMenu(m_pObject->Get_Name());

	CMD_Menu_Add_Item(pMenu, false, ID_CMD_WKSP_ITEM_CLOSE);
	CMD_Menu_Add_Item(pMenu, false, ID_CMD_GRID_SHOW);

	pMenu->AppendSeparator();
	CMD_Menu_Add_Item(pMenu, false, ID_CMD_DATA_SAVE);
	CMD_Menu_Add_Item(pMenu, false, ID_CMD_DATA_SAVEAS);

	if( PGSQL_has_Connections() )
		CMD_Menu_Add_Item(pMenu, false, ID_CMD_DATA_SAVETODB);

	CMD_Menu_Add_Item(pMenu, false, ID_CMD_GRID_SAVEAS_IMAGE);

	if( m_pObject->is_File_Native() && m_pObject->is_Modified() )
		CMD_Menu_Add_Item(pMenu, false, ID_CMD_DATA_RELOAD);

	if( m_pObject->is_File_Native() )
		CMD_Menu_Add_Item(pMenu, false, ID_CMD_DATA_DEL_FILES);

	pMenu->AppendSeparator();
	CMD_Menu_Add_Item(pMenu, false, ID_CMD_DATA_PROJECTION);

	if( m_pObject->Get_MetaData().Get_Children_Count() > 0 )
		CMD_Menu_Add_Item(pMenu, false, ID_CMD_DATA_METADATA);

	pMenu->AppendSeparator();
	CMD_Menu_Add_Item(pMenu, true , ID_CMD_GRID_HISTOGRAM);
	CMD_Menu_Add_Item(pMenu, false, ID_CMD_GRID_SCATTERPLOT);

	pMenu->AppendSeparator();
	CMD_Menu_Add_Item(pMenu, false, ID_CMD_WKSP_ITEM_SETTINGS_COPY);
	CMD_Menu_Add_Item(pMenu, false, ID_CMD_GRID_SET_LUT);

	return( pMenu );
}


///////////////////////////////////////////////////////////
//														 //
///////////////////////////////////////////////////////////

//---------------------------------------------------------
bool CWKSP_Grids::On_Command(int Cmd_ID)
{
	switch( Cmd_ID )
	{
	default:
		return( CWKSP_Layer::On_Command(Cmd_ID) );

	case ID_CMD_GRID_SAVEAS_IMAGE:
		_Save_Image();
		break;

	case ID_CMD_GRID_HISTOGRAM:
		Histogram_Toggle();
		break;

	case ID_CMD_GRID_SCATTERPLOT:
		Add_ScatterPlot();
		break;

	case ID_CMD_GRID_SET_LUT:
		_LUT_Create();
		break;
	}

	return( true );
}

//---------------------------------------------------------
bool CWKSP_Grids::On_Command_UI(wxUpdateUIEvent &event)
{
	switch( event.GetId() )
	{
	default:
		return( CWKSP_Layer::On_Command_UI(event) );

	case ID_CMD_GRID_HISTOGRAM:
		event.Check(m_pHistogram != NULL);
		break;
	}

	return( true );
}


///////////////////////////////////////////////////////////
//														 //
///////////////////////////////////////////////////////////

//---------------------------------------------------------
enum
{
	GRIDS_CLASSIFY_UNIQUE	= 0,
	GRIDS_CLASSIFY_LUT,
	GRIDS_CLASSIFY_METRIC,
	GRIDS_CLASSIFY_GRADUATED,
	GRIDS_CLASSIFY_OVERLAY
};


///////////////////////////////////////////////////////////
//														 //
///////////////////////////////////////////////////////////

//---------------------------------------------------------
void CWKSP_Grids::On_Create_Parameters(void)
{
	CWKSP_Layer::On_Create_Parameters();

	//-----------------------------------------------------
	// General...

	m_Parameters.Add_String("NODE_GENERAL", "OBJECT_Z_UNIT"  , _TL("Unit"    ), _TL(""), Get_Grids()->Get_Unit   ());
	m_Parameters.Add_Double("NODE_GENERAL", "OBJECT_Z_FACTOR", _TL("Z-Scale" ), _TL(""), Get_Grids()->Get_Scaling());
	m_Parameters.Add_Double("NODE_GENERAL", "OBJECT_Z_OFFSET", _TL("Z-Offset"), _TL(""), Get_Grids()->Get_Offset ());

	m_Parameters.Add_Double("NODE_GENERAL",
		"MAX_SAMPLES"	, _TL("Maximum Samples"),
		_TL("Maximum number of samples used to build statistics and histograms expressed as percent of the total number of cells."),
		100.0 * (double)Get_Grids()->Get_Max_Samples() / (double)Get_Grids()->Get_NCells(), 0., true, 100., true
	);

	//-----------------------------------------------------
	m_Parameters.Add_Choice("NODE_GENERAL", "DIM_ATTRIBUTE"	    , _TL("Attribute"),
		_TL(""),
		_Get_List_Attributes(), Get_Grids()->Get_Z_Attribute()
	);

	m_Parameters.Add_Choice("NODE_GENERAL", "DIM_NAME"			, _TL("Name"),
		_TL(""),
		_Get_List_Attributes(), Get_Grids()->Get_Z_Attribute()
	);

	//-----------------------------------------------------
	// Display...

	m_Parameters.Add_Choice("NODE_DISPLAY", "DISPLAY_RESAMPLING", _TL("Resampling"),
		_TL(""),
		CSG_String::Format("%s|%s|%s|%s|",
			_TL("Nearest Neighbour"),
			_TL("Bilinear Interpolation"),
			_TL("Bicubic Spline Interpolation"),
			_TL("B-Spline Interpolation")
		), 0
	);

	//-----------------------------------------------------
	// Classification...

	m_Parameters("COLORS_TYPE")->asChoice()->Set_Items(
		CSG_String::Format("%s|%s|%s|%s|%s|",
			_TL("Single Symbol"   ), // GRIDS_CLASSIFY_UNIQUE
			_TL("Classified"      ), // GRIDS_CLASSIFY_LUT
			_TL("Discrete Colors" ), // GRIDS_CLASSIFY_METRIC
			_TL("Graduated Colors"), // GRIDS_CLASSIFY_GRADUATED
			_TL("RGB Composite"   )  // GRIDS_CLASSIFY_OVERLAY
		)
	);

	m_Parameters("COLORS_TYPE")->Set_Value(GRIDS_CLASSIFY_OVERLAY);

	//-----------------------------------------------------
	m_Parameters.Add_Choice("NODE_COLORS",
		"STRETCH_DEFAULT"	, _TL("Histogram Stretch"),
		_TL("Histogram stretch used for RGB composite and when fitting to zoomed extent in a map window."),
		CSG_String::Format("%s|%s|%s|",
			_TL("Linear"),
			_TL("Standard Deviation"),
			_TL("Percentile")
		), g_pData->Get_Parameter("GRID_STRETCH_DEFAULT")->asInt()
	);

	m_Parameters.Add_Double("STRETCH_DEFAULT",
		"STRETCH_LINEAR"	, _TL("Linear Percent Stretch"),
		_TL("Linear percent stretch allows you to trim extreme values from both ends of the histogram using the percentage specified here."),
		2.0, 0.0, true, 50.0, true
	);

	m_Parameters.Add_Double("STRETCH_DEFAULT",
		"STRETCH_STDDEV"	, _TL("Standard Deviation"),
		_TL(""),
		2.0, 0.0, true
	);

	m_Parameters.Add_Bool("STRETCH_STDDEV",
		"STRETCH_INRANGE"	, _TL("Keep in Range"),
		_TL("Prevents that minimum or maximum stretch value fall outside the data value range."),
		true
	);

	m_Parameters.Add_Double("STRETCH_DEFAULT",
		"STRETCH_PCTL"		, _TL("Percentile"),
		_TL(""),
		2.0, 0.0, true, 50.0, true
	);

	//-----------------------------------------------------
	m_Parameters.Add_Choice("NODE_METRIC", "BAND", _TL("Band" ), _TL(""), _Get_List_Bands(), 0);

	//-----------------------------------------------------
	m_Parameters.Add_Node("NODE_COLORS", "NODE_OVERLAY", _TL("RGB Composite"),
		_TL("")
	);

	m_Parameters.Add_Choice("NODE_OVERLAY",
		"OVERLAY_STATISTICS", _TL("Statistics"),
		_TL(""),
		CSG_String::Format("%s|%s",
			_TL("all bands"),
			_TL("each band")
		), 1
	);

	m_Parameters.Add_Choice("NODE_OVERLAY", "BAND_R", _TL("Red"  ), _TL(""), _Get_List_Bands(), 0);
	m_Parameters.Add_Choice("NODE_OVERLAY", "BAND_G", _TL("Green"), _TL(""), _Get_List_Bands(), 1);
	m_Parameters.Add_Choice("NODE_OVERLAY", "BAND_B", _TL("Blue" ), _TL(""), _Get_List_Bands(), 2);

//	m_Parameters.Add_Range("BAND_R", "BAND_R_RANGE", _TL("Value Range"), _TL(""));
//	m_Parameters.Add_Range("BAND_G", "BAND_G_RANGE", _TL("Value Range"), _TL(""));
//	m_Parameters.Add_Range("BAND_B", "BAND_B_RANGE", _TL("Value Range"), _TL(""));
}


///////////////////////////////////////////////////////////
//														 //
///////////////////////////////////////////////////////////

//---------------------------------------------------------
CSG_Grid * CWKSP_Grids::Get_Grid(void)
{
	return( Get_Grids()->Get_Grid_Ptr(m_Parameters("BAND")->asInt()) );
}

//---------------------------------------------------------
CSG_Grid * CWKSP_Grids::Get_Grid(int i)
{
	switch( i )
	{
	default: return( Get_Grids()->Get_Grid_Ptr(m_Parameters("BAND_R")->asInt()) );
	case  1: return( Get_Grids()->Get_Grid_Ptr(m_Parameters("BAND_G")->asInt()) );
	case  2: return( Get_Grids()->Get_Grid_Ptr(m_Parameters("BAND_B")->asInt()) );
	}
}


///////////////////////////////////////////////////////////
//														 //
///////////////////////////////////////////////////////////

//---------------------------------------------------------
CSG_String CWKSP_Grids::_Get_List_Attributes(void)
{
	CSG_String	List;

	for(int i=0; i<Get_Grids()->Get_Attributes().Get_Field_Count(); i++)
	{
		List	+= Get_Grids()->Get_Attributes().Get_Field_Name(i); List += "|";
	}

	return( List );
}

//---------------------------------------------------------
CSG_String CWKSP_Grids::_Get_List_Bands(int Attribute)
{
	if( Attribute < 0 || Attribute >= Get_Grids()->Get_Attributes().Get_Field_Count() )
	{
		Attribute	= Get_Grids()->Get_Z_Name_Field();
	}

	CSG_String	List;

	for(int i=0; i<Get_Grids()->Get_NZ(); i++)
	{
		List	+= Get_Grids()->Get_Attributes()[i].asString(Attribute); List += "|";
	}

	return( List );
}


///////////////////////////////////////////////////////////
//														 //
///////////////////////////////////////////////////////////

//---------------------------------------------------------
void CWKSP_Grids::On_DataObject_Changed(void)
{
	CWKSP_Layer::On_DataObject_Changed();

	//-----------------------------------------------------
	m_Edit_Attributes.Create(Get_Grids()->Get_Attributes());

	//-----------------------------------------------------
	m_Parameters("OBJECT_Z_UNIT"  )->Set_Value(Get_Grids()->Get_Unit   ());
	m_Parameters("OBJECT_Z_FACTOR")->Set_Value(Get_Grids()->Get_Scaling());
	m_Parameters("OBJECT_Z_OFFSET")->Set_Value(Get_Grids()->Get_Offset ());

	//-----------------------------------------------------
	m_Parameters("MAX_SAMPLES")->Set_Value(
		100.0 * (double)Get_Grids()->Get_Max_Samples() / (double)Get_Grids()->Get_NCells()
	);

	//-----------------------------------------------------
	CSG_String	List;

	List	= _Get_List_Attributes();

	m_Parameters("DIM_ATTRIBUTE")->asChoice()->Set_Items(List);
	m_Parameters("DIM_ATTRIBUTE")->Set_Value(Get_Grids()->Get_Z_Attribute ());

	m_Parameters("DIM_NAME"     )->asChoice()->Set_Items(List);
	m_Parameters("DIM_NAME"     )->Set_Value(Get_Grids()->Get_Z_Name_Field());

	//-----------------------------------------------------
	List	= _Get_List_Bands();

	m_Parameters("BAND"  )->asChoice()->Set_Items(List);
	m_Parameters("BAND_R")->asChoice()->Set_Items(List);
	m_Parameters("BAND_G")->asChoice()->Set_Items(List);
	m_Parameters("BAND_B")->asChoice()->Set_Items(List);
}

//---------------------------------------------------------
void CWKSP_Grids::On_Parameters_Changed(void)
{
	CWKSP_Layer::On_Parameters_Changed();

	//-----------------------------------------------------
	Get_Grids()->Set_Unit   (m_Parameters("OBJECT_Z_UNIT"  )->asString());
	Get_Grids()->Set_Scaling(m_Parameters("OBJECT_Z_FACTOR")->asDouble(), m_Parameters("OBJECT_Z_OFFSET")->asDouble());

	Get_Grids()->Set_Max_Samples(Get_Grid()->Get_NCells() * (m_Parameters("MAX_SAMPLES")->asDouble() / 100.0) );

	if( m_Parameters("COLORS_TYPE")->asInt() == GRIDS_CLASSIFY_OVERLAY && m_Parameters("OVERLAY_STATISTICS")->asInt() != 0 )
	{
		_Fit_Colors();
	}

	//-----------------------------------------------------
	Get_Grids()->Set_Z_Attribute(m_Parameters("DIM_ATTRIBUTE")->asInt());

	if( Get_Grids()->Get_Z_Name_Field() != m_Parameters("DIM_NAME")->asInt() )
	{
		Get_Grids()->Set_Z_Name_Field(m_Parameters("DIM_NAME")->asInt());

		CSG_String	List	= _Get_List_Bands();

		m_Parameters("BAND"  )->asChoice()->Set_Items(List);
		m_Parameters("BAND_R")->asChoice()->Set_Items(List);
		m_Parameters("BAND_G")->asChoice()->Set_Items(List);
		m_Parameters("BAND_B")->asChoice()->Set_Items(List);
	}
}

//---------------------------------------------------------
bool CWKSP_Grids::Update(CWKSP_Layer *pChanged)
{
	if( pChanged == this )
	{
		return( true );
	}

	return( false );
}

//---------------------------------------------------------
bool CWKSP_Grids::Edit_Set_Attributes(void)
{
	if( Get_Grids()->Get_Attributes_Ptr()->Assign_Values(&m_Edit_Attributes) && Get_Grids()->Update_Z_Order() )
	{
		DataObject_Changed();

	//	_Fit_Colors(true); Update_Views(true);

		return( true );
	}

	return( false );
}


///////////////////////////////////////////////////////////
//														 //
///////////////////////////////////////////////////////////

//---------------------------------------------------------
int CWKSP_Grids::On_Parameter_Changed(CSG_Parameters *pParameters, CSG_Parameter *pParameter, int Flags)
{
	//-----------------------------------------------------
	if( Flags & PARAMETER_CHECK_VALUES )
	{
		if(	!SG_STR_CMP(pParameter->Get_Identifier(), "OBJECT_Z_FACTOR")
		||  !SG_STR_CMP(pParameter->Get_Identifier(), "OBJECT_Z_OFFSET") )
		{
			double	newFactor	= pParameters->Get("OBJECT_Z_FACTOR")->asDouble(), oldFactor	= m_Parameters("OBJECT_Z_FACTOR")->asDouble();
			double	newOffset	= pParameters->Get("OBJECT_Z_OFFSET")->asDouble(), oldOffset	= m_Parameters("OBJECT_Z_OFFSET")->asDouble();

			if( newFactor != 0.0 && oldFactor != 0.0 )
			{
				CSG_Parameter_Range	*newRange	= pParameters->Get("METRIC_ZRANGE")->asRange();
				CSG_Parameter_Range	*oldRange	= m_Parameters.Get("METRIC_ZRANGE")->asRange();

				newRange->Set_LoVal(((oldRange->Get_LoVal() - oldOffset) / oldFactor) * newFactor + newOffset);
				newRange->Set_HiVal(((oldRange->Get_HiVal() - oldOffset) / oldFactor) * newFactor + newOffset);
			}
		}

		if(	!SG_STR_CMP(pParameter->Get_Identifier(), "DIM_NAME") )
		{
			CSG_String	List	= _Get_List_Bands(pParameter->asInt());

			pParameters->Get("BAND"  )->asChoice()->Set_Items(List);
			pParameters->Get("BAND_R")->asChoice()->Set_Items(List);
			pParameters->Get("BAND_G")->asChoice()->Set_Items(List);
			pParameters->Get("BAND_B")->asChoice()->Set_Items(List);
		}

		if( pParameters->Get("COLORS_TYPE")->asInt() == GRIDS_CLASSIFY_METRIC
		||  pParameters->Get("COLORS_TYPE")->asInt() == GRIDS_CLASSIFY_GRADUATED
		||  pParameters->Get("COLORS_TYPE")->asInt() == GRIDS_CLASSIFY_OVERLAY )
		{
			if(	!SG_STR_CMP(pParameter->Get_Identifier(), "BAND"              )
			||  !SG_STR_CMP(pParameter->Get_Identifier(), "BAND_R"            )
			||  !SG_STR_CMP(pParameter->Get_Identifier(), "BAND_G"            )
			||  !SG_STR_CMP(pParameter->Get_Identifier(), "BAND_B"            )
			||  !SG_STR_CMP(pParameter->Get_Identifier(), "OVERLAY_STATISTICS")
			||  !SG_STR_CMP(pParameter->Get_Identifier(), "COLORS_TYPE"       )
			||  !SG_STR_CMP(pParameter->Get_Identifier(), "STRETCH_DEFAULT"   )
			||  !SG_STR_CMP(pParameter->Get_Identifier(), "STRETCH_LINEAR"    )
			||  !SG_STR_CMP(pParameter->Get_Identifier(), "STRETCH_STDDEV"    )
			||  !SG_STR_CMP(pParameter->Get_Identifier(), "STRETCH_INRANGE"   )
			||  !SG_STR_CMP(pParameter->Get_Identifier(), "STRETCH_PCTL"      ) )
			{
				double	Minimum, Maximum;

				if( _Fit_Colors(*pParameters, Minimum, Maximum) )
				{
					pParameters->Get("METRIC_ZRANGE")->asRange()->Set_Range(Minimum, Maximum);
				}
			}
		}
	}

	//-----------------------------------------------------
	if( Flags & PARAMETER_CHECK_ENABLE )
	{
		if( !SG_STR_CMP(pParameter->Get_Identifier(), "COLORS_TYPE")
		||  !SG_STR_CMP(pParameter->Get_Identifier(), "OVERLAY_STATISTICS") )
		{
			int		Type	= pParameters->Get("COLORS_TYPE")->asInt();

			pParameters->Set_Enabled("NODE_UNISYMBOL"    , Type == GRIDS_CLASSIFY_UNIQUE);
			pParameters->Set_Enabled("NODE_LUT"          , Type == GRIDS_CLASSIFY_LUT);
			pParameters->Set_Enabled("NODE_METRIC"       , Type != GRIDS_CLASSIFY_UNIQUE && Type != GRIDS_CLASSIFY_LUT);
			pParameters->Set_Enabled("NODE_OVERLAY"      , Type == GRIDS_CLASSIFY_OVERLAY);

			pParameters->Set_Enabled("BAND"              , Type == GRIDS_CLASSIFY_METRIC || Type == GRIDS_CLASSIFY_GRADUATED || Type == GRIDS_CLASSIFY_LUT);

			pParameters->Set_Enabled("METRIC_ZRANGE"     , Type == GRIDS_CLASSIFY_METRIC || Type == GRIDS_CLASSIFY_GRADUATED || (Type == GRIDS_CLASSIFY_OVERLAY && pParameters->Get("OVERLAY_STATISTICS")->asInt() == 0));
			pParameters->Set_Enabled("METRIC_SCALE_MODE" , Type == GRIDS_CLASSIFY_METRIC || Type == GRIDS_CLASSIFY_GRADUATED ||  Type == GRIDS_CLASSIFY_OVERLAY);

			pParameters->Set_Enabled("DISPLAY_RESAMPLING", Type != GRIDS_CLASSIFY_LUT && Type != GRIDS_CLASSIFY_UNIQUE);
		}

		if(	!SG_STR_CMP(pParameter->Get_Identifier(), "STRETCH_DEFAULT") )
		{
			pParameters->Set_Enabled("STRETCH_LINEAR", pParameter->asInt() == 0);
			pParameters->Set_Enabled("STRETCH_STDDEV", pParameter->asInt() == 1);
			pParameters->Set_Enabled("STRETCH_PCTL"  , pParameter->asInt() == 2);
		}
	}

	return( CWKSP_Layer::On_Parameter_Changed(pParameters, pParameter, Flags) );
}


///////////////////////////////////////////////////////////
//														 //
///////////////////////////////////////////////////////////

//---------------------------------------------------------
void CWKSP_Grids::_LUT_Create(void)
{
	//-----------------------------------------------------
	static CSG_Parameters	Parameters;

	if( Parameters.Get_Count() == 0 )
	{
		Parameters.Create(NULL, _TL("Classify"), _TL(""));
		Parameters.Add_Colors("", "COLOR" , _TL("Colors"        ), _TL(""))->asColors()->Set_Count(11);
		Parameters.Add_Choice("", "METHOD", _TL("Classification"), _TL(""),
			CSG_String::Format("%s|%s|%s|%s|",
				_TL("unique values"),
				_TL("equal intervals"),
				_TL("quantiles"),
				_TL("natural breaks")
			), 1
		);
	}

	if( !DLG_Parameters(&Parameters) )
	{
		return;
	}

	//-----------------------------------------------------
	CSG_Colors	Colors(*Parameters("COLOR")->asColors());

	CSG_Table	Classes(m_Parameters("LUT")->asTable());

	switch( Parameters("METHOD")->asInt() )
	{
	//-----------------------------------------------------
	case 0:	// unique values
	{
		CSG_Unique_Number_Statistics	s;

		#define MAX_CLASSES	1024

		for(sLong iCell = 0; iCell<Get_Grid()->Get_NCells() && s.Get_Count()<MAX_CLASSES && PROGRESSBAR_Set_Position(iCell, Get_Grid()->Get_NCells()); iCell++)
		{
			if( !Get_Grid()->is_NoData(iCell) )
			{
				s	+= Get_Grid()->asDouble(iCell);
			}
		}

		Colors.Set_Count(s.Get_Count());

		for(int iClass=0; iClass<s.Get_Count(); iClass++)
		{
			double		Value	= s.Get_Value(iClass);

			CSG_String	Name	= SG_Get_String(Value, -2);

			CSG_Table_Record	*pClass	= Classes.Add_Record();

			pClass->Set_Value(0, Colors[iClass]);	// Color
			pClass->Set_Value(1, Name          );	// Name
			pClass->Set_Value(2, Name          );	// Description
			pClass->Set_Value(3, Value         );	// Minimum
			pClass->Set_Value(4, Value         );	// Maximum
		}

		break;
	}

	//-----------------------------------------------------
	case 1:	// equal intervals
		if( Get_Grid()->Get_Range() && Colors.Get_Count() > 0 )
		{
			double	Minimum, Maximum, Interval;

			Interval	= Get_Grid()->Get_Range() / (double)Colors.Get_Count();
			Minimum		= Get_Grid()->Get_Min  ();

			for(int iClass=0; iClass<Colors.Get_Count(); iClass++, Minimum+=Interval)
			{
				Maximum	= iClass < Colors.Get_Count() - 1 ? Minimum + Interval : Get_Grid()->Get_Max() + 1.0;

				CSG_String	Name	= SG_Get_String(Minimum, -2)
							+ " - " + SG_Get_String(Maximum, -2);

				CSG_Table_Record	*pClass	= Classes.Add_Record();

				pClass->Set_Value(0, Colors[iClass]);	// Color
				pClass->Set_Value(1, Name          );	// Name
				pClass->Set_Value(2, Name          );	// Description
				pClass->Set_Value(3, Minimum       );	// Minimum
				pClass->Set_Value(4, Maximum       );	// Maximum
			}
		}
		break;

	//-----------------------------------------------------
	case 2:	// quantiles
		{
			if( Get_Grid()->Get_NCells() < Colors.Get_Count() )
			{
				Colors.Set_Count(Get_Grid()->Get_NCells());
			}

			sLong	jCell, nCells;
			double	Minimum, Maximum, iCell, Count;

			Maximum	= Get_Grid()->Get_Min();
			nCells	= Get_Grid()->Get_NCells() - Get_Grid()->Get_NoData_Count();
			iCell	= Count	= nCells / (double)Colors.Get_Count();

			for(iCell=0.0; iCell<Get_Grid()->Get_NCells(); iCell++)
			{
				if( Get_Grid()->Get_Sorted(iCell, jCell, false) )
				{
					break;
				}
			}

			iCell	+= Count;

			for(int iClass=0; iClass<Colors.Get_Count(); iClass++, iCell+=Count)
			{
				Get_Grid()->Get_Sorted(iCell, jCell, false);

				Minimum	= Maximum;
				Maximum	= iCell < Get_Grid()->Get_NCells() ? Get_Grid()->asDouble(jCell) : Get_Grid()->Get_Max() + 1.0;

				CSG_String	Name	= SG_Get_String(Minimum, -2)
							+ " - " + SG_Get_String(Maximum, -2);

				CSG_Table_Record	*pClass	= Classes.Add_Record();

				pClass->Set_Value(0, Colors[iClass]);	// Color
				pClass->Set_Value(1, Name          );	// Name
				pClass->Set_Value(2, Name          );	// Description
				pClass->Set_Value(3, Minimum       );	// Minimum
				pClass->Set_Value(4, Maximum       );	// Maximum
			}
		}
		break;

	//-----------------------------------------------------
	case 3:	// natural breaks
		{
			CSG_Natural_Breaks	Breaks(Get_Grid(), Colors.Get_Count(), 255);

			if( Breaks.Get_Count() <= Colors.Get_Count() ) return;

			for(int iClass=0; iClass<Colors.Get_Count(); iClass++)
			{
				CSG_Table_Record	*pClass	= Classes.Add_Record();

				double	Minimum	= Breaks[iClass    ];
				double	Maximum	= Breaks[iClass + 1];

				CSG_String	Name	= SG_Get_String(Minimum, -2)
							+ " - " + SG_Get_String(Maximum, -2);

				pClass->Set_Value(0, Colors[iClass]);	// Color
				pClass->Set_Value(1, Name          );	// Name
				pClass->Set_Value(2, Name          );	// Description
				pClass->Set_Value(3, Minimum       );	// Minimum
				pClass->Set_Value(4, Maximum       );	// Maximum
			}
		}
		break;
	}

	//-----------------------------------------------------
	PROGRESSBAR_Set_Position(0);

	if( Classes.Get_Count() > 0 )
	{
		m_Parameters("LUT")->asTable()->Assign(&Classes);

		m_Parameters("COLORS_TYPE")->Set_Value(CLASSIFY_LUT);	// Lookup Table

		Parameters_Changed();
	}
}


///////////////////////////////////////////////////////////
//														 //
///////////////////////////////////////////////////////////

//---------------------------------------------------------
wxString CWKSP_Grids::Get_Value(CSG_Point ptWorld, double Epsilon)
{
	wxString	s;	double	Value;

	switch( m_pClassify->Get_Mode() )
	{
	default:
		break;

	case CLASSIFY_METRIC:
	case CLASSIFY_GRADUATED:
		if( Get_Grid()->Get_Value(ptWorld, Value, GRID_RESAMPLING_NearestNeighbour) )
		{
			s	= SG_Get_String(Value, -12).c_str();

			if( Get_Grids()->Get_Unit() && *Get_Grids()->Get_Unit() )
			{
				s += " "; s += Get_Grids()->Get_Unit();
			}
		}
		break;

	case CLASSIFY_OVERLAY:
		if( Get_Grid(0)->Get_Value(ptWorld, Value, GRID_RESAMPLING_NearestNeighbour) )
		{
			s	+= wxString::Format("R%s ", SG_Get_String(Value, -12).c_str());
		}
		if( Get_Grid(1)->Get_Value(ptWorld, Value, GRID_RESAMPLING_NearestNeighbour) )
		{
			s	+= wxString::Format("G%s ", SG_Get_String(Value, -12).c_str());
		}
		if( Get_Grid(2)->Get_Value(ptWorld, Value, GRID_RESAMPLING_NearestNeighbour) )
		{
			s	+= wxString::Format("B%s ", SG_Get_String(Value, -12).c_str());
		}
		break;

	case CLASSIFY_LUT:
		if( Get_Grid()->Get_Value(ptWorld, Value, GRID_RESAMPLING_NearestNeighbour) )
		{
			s	= m_pClassify->Get_Class_Name_byValue(Value);
		}
		break;
	}

	return( s );
}

//---------------------------------------------------------
double CWKSP_Grids::Get_Value_Minimum(void)	{	return( ((CSG_Grids *)m_pObject)->Get_Min   () );	}
double CWKSP_Grids::Get_Value_Maximum(void)	{	return( ((CSG_Grids *)m_pObject)->Get_Max   () );	}
double CWKSP_Grids::Get_Value_Range  (void)	{	return( ((CSG_Grids *)m_pObject)->Get_Range () );	}
double CWKSP_Grids::Get_Value_Mean   (void)	{	return( ((CSG_Grids *)m_pObject)->Get_Mean  () );	}
double CWKSP_Grids::Get_Value_StdDev (void)	{	return( ((CSG_Grids *)m_pObject)->Get_StdDev() );	}


///////////////////////////////////////////////////////////
//														 //
//														 //
//														 //
///////////////////////////////////////////////////////////

//---------------------------------------------------------
bool CWKSP_Grids::Fit_Colors(void)
{
	CSG_String	List	= _Get_List_Bands();

	m_Parameters("BAND"  )->asChoice()->Set_Items(List);
	m_Parameters("BAND_R")->asChoice()->Set_Items(List);
	m_Parameters("BAND_G")->asChoice()->Set_Items(List);
	m_Parameters("BAND_B")->asChoice()->Set_Items(List);

	return( _Fit_Colors() );
}

//---------------------------------------------------------
bool CWKSP_Grids::_Fit_Colors(bool bRefresh)
{
	if( m_Parameters("COLORS_TYPE")->asInt() == GRIDS_CLASSIFY_METRIC
	||  m_Parameters("COLORS_TYPE")->asInt() == GRIDS_CLASSIFY_GRADUATED )
	{
		double	Minimum, Maximum;

		return( _Fit_Colors(m_Parameters, Minimum, Maximum) && Set_Color_Range(Minimum, Maximum) );
	}

	if( m_Parameters("COLORS_TYPE")->asInt() == GRIDS_CLASSIFY_OVERLAY )
	{
		if( m_Parameters("STRETCH_DEFAULT")->asInt() != 2 )	// no quantiles
		{
			CSG_Simple_Statistics	s;

			if( m_Parameters("OVERLAY_STATISTICS")->asInt() == 0 )	// overall band statistics
			{
				if( s.Create(Get_Grids()->Get_Statistics()) && _Fit_Colors(s, m_pClassify) )
				{
					return( !bRefresh || Update_Views() );
				}
			}
			else	// band wise statistics
			{
				if( s.Create(Get_Grid(0)->Get_Statistics()) && _Fit_Colors(s, &m_Classify[0])
				&&  s.Create(Get_Grid(1)->Get_Statistics()) && _Fit_Colors(s, &m_Classify[1])
				&&  s.Create(Get_Grid(2)->Get_Statistics()) && _Fit_Colors(s, &m_Classify[2]) )
				{
					return( !bRefresh || Update_Views() );
				}
			}
		}
		else // quantiles
		{
			int		Mode	= m_Parameters("METRIC_SCALE_MODE")->asInt   ();
			double	Log		= m_Parameters("METRIC_SCALE_LOG" )->asDouble();
			double	d		= m_Parameters("STRETCH_PCTL"     )->asDouble();

			if( m_Parameters("OVERLAY_STATISTICS")->asInt() == 0 )	// overall band statistics
			{
				m_pClassify->Set_Metric(Mode, Log, Get_Grids()->Get_Quantile(d), Get_Grids()->Get_Quantile(100 - d));
			}
			else	// band wise statistics
			{
				m_Classify[0].Set_Metric(Mode, Log, Get_Grid(0)->Get_Quantile(d), Get_Grid(0)->Get_Quantile(100 - d));
				m_Classify[1].Set_Metric(Mode, Log, Get_Grid(1)->Get_Quantile(d), Get_Grid(1)->Get_Quantile(100 - d));
				m_Classify[2].Set_Metric(Mode, Log, Get_Grid(2)->Get_Quantile(d), Get_Grid(2)->Get_Quantile(100 - d));
			}

			return( !bRefresh || Update_Views() );
		}
	}

	return( false );
}

//---------------------------------------------------------
bool CWKSP_Grids::Fit_Colors(const CSG_Rect &rWorld)
{
	CSG_Simple_Statistics	s;

	int	Method	= m_Parameters("STRETCH_DEFAULT")->asInt();	// == 2 >> fit to quantiles

	if( m_Parameters("COLORS_TYPE")->asInt() == GRIDS_CLASSIFY_METRIC
	||  m_Parameters("COLORS_TYPE")->asInt() == GRIDS_CLASSIFY_GRADUATED )
	{
		return( Get_Grid()->Get_Statistics(rWorld, s, Method == 2) && _Fit_Colors(s, m_pClassify, true) );
	}

	if( m_Parameters("COLORS_TYPE")->asInt() == GRIDS_CLASSIFY_OVERLAY )
	{
		if( m_Parameters("OVERLAY_STATISTICS")->asInt() == 0 )	// overall band statistics
		{
			if( Get_Grids()->Get_Statistics(rWorld, s, Method == 2) && _Fit_Colors(s, m_pClassify) )
			{
				return( Update_Views() );
			}
		}
		else	// band wise statistics
		{
			if( Get_Grid(0)->Get_Statistics(rWorld, s, Method == 2) && _Fit_Colors(s, &m_Classify[0])
			&&  Get_Grid(1)->Get_Statistics(rWorld, s, Method == 2) && _Fit_Colors(s, &m_Classify[1])
			&&  Get_Grid(2)->Get_Statistics(rWorld, s, Method == 2) && _Fit_Colors(s, &m_Classify[2]) )
			{
				return( Update_Views() );
			}
		}
	}

	return( false );
}

//---------------------------------------------------------
bool CWKSP_Grids::_Fit_Colors(CSG_Parameters &Parameters, double &Minimum, double &Maximum)
{
	if( Parameters("COLORS_TYPE")->asInt() == GRIDS_CLASSIFY_OVERLAY )
	{
		CSG_Grids	*pGrids	= Get_Grids();

		switch( Parameters("STRETCH_DEFAULT")->asInt() )
		{
		default: {	double	d	= Parameters("STRETCH_LINEAR")->asDouble() * 0.01 * pGrids->Get_Range();
			Minimum	= pGrids->Get_Min() + d;
			Maximum	= pGrids->Get_Max() - d;
			break;	}

		case  1: {	double	d	= Parameters("STRETCH_STDDEV")->asDouble() * pGrids->Get_StdDev();
			Minimum	= pGrids->Get_Mean() - d; if( Parameters("STRETCH_INRANGE")->asBool() && Minimum < pGrids->Get_Min() ) Minimum = pGrids->Get_Min();
			Maximum	= pGrids->Get_Mean() + d; if( Parameters("STRETCH_INRANGE")->asBool() && Maximum > pGrids->Get_Max() ) Maximum = pGrids->Get_Max();
			break;	}

		case  2: {	double	d	= Parameters("STRETCH_PCTL")->asDouble();
			Minimum	= pGrids->Get_Quantile(      d);
			Maximum	= pGrids->Get_Quantile(100 - d);
			break;	}
		}
	}
	else
	{
		CSG_Grid	*pGrid	= Get_Grids()->Get_Grid_Ptr(Parameters("BAND")->asInt());

		switch( Parameters("STRETCH_DEFAULT")->asInt() )
		{
		default: {	double	d	= Parameters("STRETCH_LINEAR")->asDouble() * 0.01 * pGrid->Get_Range();
			Minimum	= pGrid->Get_Min() + d;
			Maximum	= pGrid->Get_Max() - d;
			break;	}

		case  1: {	double	d	= Parameters("STRETCH_STDDEV")->asDouble() * pGrid->Get_StdDev();
			Minimum	= pGrid->Get_Mean() - d; if( Parameters("STRETCH_INRANGE")->asBool() && Minimum < pGrid->Get_Min() ) Minimum = pGrid->Get_Min();
			Maximum	= pGrid->Get_Mean() + d; if( Parameters("STRETCH_INRANGE")->asBool() && Maximum > pGrid->Get_Max() ) Maximum = pGrid->Get_Max();
			break;	}

		case  2: {	double	d	= Parameters("STRETCH_PCTL")->asDouble();
			Minimum	= pGrid->Get_Quantile(      d);
			Maximum	= pGrid->Get_Quantile(100 - d);
			break;	}
		}
	}

	return( true );
}

//---------------------------------------------------------
bool CWKSP_Grids::_Fit_Colors(CSG_Simple_Statistics &s, CWKSP_Layer_Classify *pClassify, bool bRefresh)
{
	if( s.Get_Count() <= 0 )
	{
		return( false );
	}

	double	Minimum, Maximum;

	switch( m_Parameters("STRETCH_DEFAULT")->asInt() )
	{
	default: {	double	d	= m_Parameters("STRETCH_LINEAR")->asDouble() * 0.01 * s.Get_Range();
		Minimum	= s.Get_Minimum() + d;
		Maximum	= s.Get_Maximum() - d;
		break;	}

	case  1: {	double	d	= m_Parameters("STRETCH_STDDEV")->asDouble() * s.Get_StdDev();
		Minimum	= s.Get_Mean() - d; if( m_Parameters("STRETCH_INRANGE")->asBool() && Minimum < s.Get_Minimum() ) Minimum = s.Get_Minimum();
		Maximum	= s.Get_Mean() + d; if( m_Parameters("STRETCH_INRANGE")->asBool() && Maximum > s.Get_Maximum() ) Maximum = s.Get_Maximum();
		break;	}

	case  2: {	double	d	= m_Parameters("STRETCH_PCTL")->asDouble();
		Minimum	= s.Get_Quantile(      d);
		Maximum	= s.Get_Quantile(100 - d);
		break;	}
	}

	pClassify->Set_Metric(m_Parameters("METRIC_SCALE_MODE")->asInt(), m_Parameters("METRIC_SCALE_LOG")->asDouble(), Minimum, Maximum);

	return( !bRefresh || Set_Color_Range(Minimum, Maximum) );
}


///////////////////////////////////////////////////////////
//														 //
//														 //
//														 //
///////////////////////////////////////////////////////////

//---------------------------------------------------------
bool CWKSP_Grids::asImage(CSG_Grid *pImage)
{
	wxBitmap	BMP;

	if( pImage && Get_Image_Grid(BMP) )
	{
		wxImage	IMG(BMP.ConvertToImage());

		pImage->Create(Get_Grids()->Get_System(), SG_DATATYPE_Int);

		for(int y=0; y<pImage->Get_NY() && PROGRESSBAR_Set_Position(y, pImage->Get_NY()); y++)
		{
			for(int x=0; x<pImage->Get_NX(); x++)
			{
				pImage->Set_Value(x, y, SG_GET_RGB(IMG.GetRed(x, y), IMG.GetGreen(x, y), IMG.GetBlue(x, y)));
			}
		}

		return( true );
	}

	return( false );
}

//---------------------------------------------------------
void CWKSP_Grids::_Save_Image(void)
{
	int				type;
	wxString		file;
	wxBitmap		BMP;
	CSG_File		Stream;
	CSG_Parameters	Parms;

	//-----------------------------------------------------
	Parms.Set_Name(_TL("Save Grid as Image..."));

	Parms.Add_Bool  ("", "WORLD", _TL("Save Georeference"), _TL(""), true);
	Parms.Add_Bool  ("", "LG"   , _TL("Legend: Save"     ), _TL(""), true);
	Parms.Add_Double("", "LZ"   , _TL("Legend: Zoom"     ), _TL(""), 1.0, 0.0, true);

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
			fn.SetName(wxString::Format("%s_legend", fn.GetName().c_str()));

			BMP.SaveFile(fn.GetFullPath(), (wxBitmapType)type);
		}

		if( Parms("WORLD")->asBool() )
		{
			wxFileName	fn(file);

			switch( type )
			{
			default                : fn.SetExt("world");	break;
			case wxBITMAP_TYPE_BMP : fn.SetExt("bpw"  );	break;
			case wxBITMAP_TYPE_GIF : fn.SetExt("gfw"  );	break;
			case wxBITMAP_TYPE_JPEG: fn.SetExt("jgw"  );	break;
			case wxBITMAP_TYPE_PNG : fn.SetExt("pgw"  );	break;
			case wxBITMAP_TYPE_PCX : fn.SetExt("pxw"  );	break;
			case wxBITMAP_TYPE_TIF : fn.SetExt("tfw"  );	break; 
			}

			if( Stream.Open(fn.GetFullPath().wx_str(), SG_FILE_W, false) )
			{
				Stream.Printf("%.10f\n%.10f\n%.10f\n%.10f\n%.10f\n%.10f\n",
					 Get_Grids()->Get_Cellsize(),
					 0.0, 0.0,
					-Get_Grids()->Get_Cellsize(),
					 Get_Grids()->Get_XMin(),
					 Get_Grids()->Get_YMax()
				);
			}
		}
	}
}

//---------------------------------------------------------
bool CWKSP_Grids::Get_Image_Grid(wxBitmap &BMP, bool bFitSize)
{
	if( bFitSize || (BMP.GetWidth() > 0 && BMP.GetHeight() > 0) )
	{
		Set_Buisy_Cursor(true);

		if( bFitSize )
		{
			BMP.Create(Get_Grids()->Get_NX(), Get_Grids()->Get_NY());
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
bool CWKSP_Grids::Get_Image_Legend(wxBitmap &BMP, double Zoom)
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
///////////////////////////////////////////////////////////

//---------------------------------------------------------
void CWKSP_Grids::On_Draw(CWKSP_Map_DC &dc_Map, int Flags)
{
	if(	Get_Extent().Intersects(dc_Map.m_rWorld) == INTERSECTION_None )
	{
		return;
	}

	//-----------------------------------------------------
	double	Transparency	= m_Parameters("DISPLAY_TRANSPARENCY")->asDouble() / 100.0;

	if( !dc_Map.IMG_Draw_Begin(Transparency) )
	{
		return;
	}

	//-----------------------------------------------------
	switch( m_Parameters("COLORS_TYPE")->asInt() )
	{
	case GRIDS_CLASSIFY_UNIQUE   :	m_pClassify->Set_Mode(CLASSIFY_UNIQUE   );	break;
	case GRIDS_CLASSIFY_LUT      :	m_pClassify->Set_Mode(CLASSIFY_LUT      );	break;
	case GRIDS_CLASSIFY_METRIC   :	m_pClassify->Set_Mode(CLASSIFY_METRIC   );	break;
	case GRIDS_CLASSIFY_GRADUATED:	m_pClassify->Set_Mode(CLASSIFY_GRADUATED);	break;
	case GRIDS_CLASSIFY_OVERLAY  :	m_pClassify->Set_Mode(CLASSIFY_OVERLAY  );	break;
	}

	//-----------------------------------------------------
	TSG_Grid_Resampling	Resampling;

	if( m_pClassify->Get_Mode() == GRIDS_CLASSIFY_UNIQUE
	||  m_pClassify->Get_Mode() == GRIDS_CLASSIFY_LUT )
	{
		Resampling	= GRID_RESAMPLING_NearestNeighbour;
	}
	else switch( m_Parameters("DISPLAY_RESAMPLING")->asInt() )
	{
	default: Resampling = GRID_RESAMPLING_NearestNeighbour; break;
	case  1: Resampling = GRID_RESAMPLING_Bilinear        ; break;
	case  2: Resampling = GRID_RESAMPLING_BicubicSpline   ; break;
	case  3: Resampling = GRID_RESAMPLING_BSpline         ; break;
	}

	//-----------------------------------------------------
	CSG_Grid	*pBands[3];

	if( m_pClassify->Get_Mode() == CLASSIFY_OVERLAY )
	{
		pBands[0]	= Get_Grid(0);
		pBands[1]	= Get_Grid(1);
		pBands[2]	= Get_Grid(2);
	}
	else
	{
		pBands[0]	= Get_Grid();
	}

	//-----------------------------------------------------
	if(	dc_Map.m_DC2World >= Get_Grids()->Get_Cellsize()
	||	Resampling != GRID_RESAMPLING_NearestNeighbour )
	{
		_Draw_Grid_Points	(dc_Map, pBands, Resampling);
	}
	else
	{
		_Draw_Grid_Cells	(dc_Map, pBands);
	}

	//-----------------------------------------------------
	dc_Map.IMG_Draw_End();
}

//---------------------------------------------------------
void CWKSP_Grids::_Draw_Grid_Points(CWKSP_Map_DC &dc_Map, CSG_Grid *pBands[3], TSG_Grid_Resampling Resampling)
{
	CSG_Rect	rMap(dc_Map.m_rWorld);	rMap.Intersect(Get_Grids()->Get_Extent(true));

	int	axDC	= (int)dc_Map.xWorld2DC(rMap.Get_XMin());	if( axDC < 0 )	axDC	= 0;
	int	bxDC	= (int)dc_Map.xWorld2DC(rMap.Get_XMax());	if( bxDC >= dc_Map.m_rDC.GetWidth () )	bxDC	= dc_Map.m_rDC.GetWidth () - 1;
	int	ayDC	= (int)dc_Map.yWorld2DC(rMap.Get_YMin());	if( ayDC >= dc_Map.m_rDC.GetHeight() )	ayDC	= dc_Map.m_rDC.GetHeight() - 1;
	int	byDC	= (int)dc_Map.yWorld2DC(rMap.Get_YMax());	if( byDC < 0 )	byDC	= 0;
	int	nyDC	= abs(ayDC - byDC);

	#pragma omp parallel for
	for(int iyDC=0; iyDC<=nyDC; iyDC++)
	{
		_Draw_Grid_Line(dc_Map, pBands, Resampling, 0, ayDC - iyDC, axDC, bxDC);
	}
}

//---------------------------------------------------------
void CWKSP_Grids::_Draw_Grid_Line(CWKSP_Map_DC &dc_Map, CSG_Grid *pBands[3], TSG_Grid_Resampling Resampling, int Mode, int yDC, int axDC, int bxDC)
{
	bool	bBandWise	= m_Parameters("OVERLAY_STATISTICS")->asInt() != 0;	// bandwise statistics

	double	xMap	= dc_Map.xDC2World(axDC);
	double	yMap	= dc_Map.yDC2World( yDC);

	for(int xDC=axDC; xDC<=bxDC; xMap+=dc_Map.m_DC2World, xDC++)
	{
		if( m_pClassify->Get_Mode() != CLASSIFY_OVERLAY )
		{
			double	z;

			if( pBands[0]->Get_Value(xMap, yMap, z, Resampling, false) )
			{
				int		c;

				if( m_pClassify->Get_Class_Color_byValue(z, c) )
				{
					dc_Map.IMG_Set_Pixel(xDC, yDC, c);
				}
			}
		}
		else
		{
			double	z[3];

			if( pBands[0]->Get_Value(xMap, yMap, z[0], Resampling, false)
			&&  pBands[1]->Get_Value(xMap, yMap, z[1], Resampling, false)
			&&  pBands[2]->Get_Value(xMap, yMap, z[2], Resampling, false) )
			{
				int		c[3];

				if( bBandWise )
				{
					c[0]	= (int)(255.0 * m_Classify[0].Get_MetricToRelative(z[0]));
					c[1]	= (int)(255.0 * m_Classify[1].Get_MetricToRelative(z[1]));
					c[2]	= (int)(255.0 * m_Classify[2].Get_MetricToRelative(z[2]));
				}
				else
				{
					c[0]	= (int)(255.0 * m_pClassify ->Get_MetricToRelative(z[0]));
					c[1]	= (int)(255.0 * m_pClassify ->Get_MetricToRelative(z[1]));
					c[2]	= (int)(255.0 * m_pClassify ->Get_MetricToRelative(z[2]));
				}

				if( c[0] < 0 ) c[0] = 0; else if( c[0] > 255 ) c[0] = 255;
				if( c[1] < 0 ) c[1] = 0; else if( c[1] > 255 ) c[1] = 255;
				if( c[2] < 0 ) c[2] = 0; else if( c[2] > 255 ) c[2] = 255;

				dc_Map.IMG_Set_Pixel(xDC, yDC, SG_GET_RGB(c[0], c[1], c[2]));
			}
		}
	}
}

//---------------------------------------------------------
void CWKSP_Grids::_Draw_Grid_Cells(CWKSP_Map_DC &dc_Map, CSG_Grid *pBands[3])
{
	bool	bBandWise	= m_Parameters("OVERLAY_STATISTICS")->asInt() != 0;	// bandwise statistics

	int		x, y, xa, ya, xb, yb, xaDC, yaDC, xbDC, ybDC;
	double	xDC, yDC, axDC, ayDC, dDC;

	//-----------------------------------------------------
	dDC		= Get_Grids()->Get_Cellsize() * dc_Map.m_World2DC;

	xa		= Get_Grids()->Get_System().Get_xWorld_to_Grid(dc_Map.m_rWorld.Get_XMin());
	ya		= Get_Grids()->Get_System().Get_yWorld_to_Grid(dc_Map.m_rWorld.Get_YMin());
	xb		= Get_Grids()->Get_System().Get_xWorld_to_Grid(dc_Map.m_rWorld.Get_XMax());
	yb		= Get_Grids()->Get_System().Get_yWorld_to_Grid(dc_Map.m_rWorld.Get_YMax());

	if( xa < 0 )	xa	= 0;	if( xb >= Get_Grids()->Get_NX() )	xb	= Get_Grids()->Get_NX() - 1;
	if( ya < 0 )	ya	= 0;	if( yb >= Get_Grids()->Get_NY() )	yb	= Get_Grids()->Get_NY() - 1;

	axDC	= dc_Map.xWorld2DC(Get_Grids()->Get_System().Get_xGrid_to_World(xa)) + dDC / 2.0;
	ayDC	= dc_Map.yWorld2DC(Get_Grids()->Get_System().Get_yGrid_to_World(ya)) - dDC / 2.0;

	//-----------------------------------------------------
	for(y=ya, yDC=ayDC, yaDC=(int)(ayDC), ybDC=(int)(ayDC+dDC); y<=yb; y++, ybDC=yaDC, yaDC=(int)(yDC-=dDC))
	{
		for(x=xa, xDC=axDC, xaDC=(int)(axDC-dDC), xbDC=(int)(axDC); x<=xb; x++, xaDC=xbDC, xbDC=(int)(xDC+=dDC))
		{
			if( m_pClassify->Get_Mode() != CLASSIFY_OVERLAY )
			{
				int		c;

				if( pBands[0]->is_InGrid(x, y) && m_pClassify->Get_Class_Color_byValue(pBands[0]->asDouble(x, y), c) )
				{
					dc_Map.IMG_Set_Rect(xaDC, yaDC, xbDC, ybDC, c);
				}
			}
			else
			{
				if( pBands[0]->is_InGrid(x, y) && pBands[1]->is_InGrid(x, y) && pBands[2]->is_InGrid(x, y) )
				{
					int		c[3];

					if( bBandWise )
					{
						c[0]	= (int)(255.0 * m_Classify[0].Get_MetricToRelative(pBands[0]->asDouble(x, y)));
						c[1]	= (int)(255.0 * m_Classify[1].Get_MetricToRelative(pBands[1]->asDouble(x, y)));
						c[2]	= (int)(255.0 * m_Classify[2].Get_MetricToRelative(pBands[2]->asDouble(x, y)));
					}
					else
					{
						c[0]	= (int)(255.0 * m_pClassify ->Get_MetricToRelative(pBands[0]->asDouble(x, y)));
						c[1]	= (int)(255.0 * m_pClassify ->Get_MetricToRelative(pBands[1]->asDouble(x, y)));
						c[2]	= (int)(255.0 * m_pClassify ->Get_MetricToRelative(pBands[2]->asDouble(x, y)));
					}

					if( c[0] < 0 ) c[0] = 0; else if( c[0] > 255 ) c[0] = 255;
					if( c[1] < 0 ) c[1] = 0; else if( c[1] > 255 ) c[1] = 255;
					if( c[2] < 0 ) c[2] = 0; else if( c[2] > 255 ) c[2] = 255;

					dc_Map.IMG_Set_Rect(xaDC, yaDC, xbDC, ybDC, SG_GET_RGB(c[0], c[1], c[2]));
				}
			}
		}
	}
}


///////////////////////////////////////////////////////////
//														 //
//														 //
//														 //
///////////////////////////////////////////////////////////

//---------------------------------------------------------

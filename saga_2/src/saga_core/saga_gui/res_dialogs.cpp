
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
//                   RES_Dialogs.cpp                     //
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
#include <wx/utils.h>
#include <wx/msgdlg.h>
#include <wx/dirdlg.h> 
#include <wx/filedlg.h>
#include <wx/fontdlg.h>
#include <wx/textdlg.h>
#include <wx/colordlg.h>
#include <wx/choicdlg.h>

#include <saga_api/saga_api.h>

#include "res_dialogs.h"

#include "helper.h"

#include "dlg_parameters.h"
#include "dlg_text.h"
#include "dlg_table.h"
#include "dlg_list_grid.h"
#include "dlg_list_table.h"
#include "dlg_list_shapes.h"
#include "dlg_list_tin.h"
#include "dlg_colors.h"

#include "wksp_map_manager.h"
#include "wksp_map.h"


///////////////////////////////////////////////////////////
//														 //
//														 //
//														 //
///////////////////////////////////////////////////////////

//---------------------------------------------------------
#define CONFIG_GROUP_FILE_DLG	"/FileDialogs"


///////////////////////////////////////////////////////////
//														 //
//														 //
//														 //
///////////////////////////////////////////////////////////

//---------------------------------------------------------
const char * DLG_Get_Text(int ID_DLG)
{
	switch( ID_DLG )
	{
	case ID_DLG_ABOUT:					return( LNG("[TXT] SAGA\nSystem for Automated Geoscientific Analyses\nVersion 2.0") );
	case ID_DLG_CLOSE:					return( LNG("[TXT] Do you want to exit SAGA?") );
	case ID_DLG_DELETE:					return( LNG("[TXT] Do you want to delete the selection?") );
	}

	return( LNG("[TXT] This is not a text!") );
}

//---------------------------------------------------------
const char * DLG_Get_Caption(int ID_DLG)
{
	switch( ID_DLG )
	{
	case ID_DLG_CLOSE:					return( LNG("[CAP] Exit") );
	case ID_DLG_ABOUT:					return( LNG("[CAP] About SAGA") );
	case ID_DLG_DELETE:					return( LNG("[CAP] Delete") );
	}

	return( LNG("[CAP] This Is Not A Caption!") );
}


///////////////////////////////////////////////////////////
//														 //
//														 //
//														 //
///////////////////////////////////////////////////////////

//---------------------------------------------------------
const char * DLG_Get_FILE_Caption(int ID_DLG)
{
	switch( ID_DLG )
	{
	case ID_DLG_WKSP_OPEN:			return( LNG("[CAP] Load") );

	case ID_DLG_MODULES_OPEN:		return( LNG("[CAP] Load Module Library") );

	case ID_DLG_PROJECT_OPEN:		return( LNG("[CAP] Load Project") );
	case ID_DLG_PROJECT_SAVE:		return( LNG("[CAP] Save Project") );

	case ID_DLG_GRIDS_OPEN:			return( LNG("[CAP] Load Grid") );
	case ID_DLG_GRIDS_SAVE:			return( LNG("[CAP] Save Grid") );

	case ID_DLG_SHAPES_OPEN:		return( LNG("[CAP] Load Shapes") );
	case ID_DLG_SHAPES_SAVE:		return( LNG("[CAP] Save Shapes") );

	case ID_DLG_TABLES_OPEN:		return( LNG("[CAP] Load Table") );
	case ID_DLG_TABLES_SAVE:		return( LNG("[CAP] Save Table") );

	case ID_DLG_TIN_OPEN:			return( LNG("[CAP] Load T.I.N.") );
	case ID_DLG_TIN_SAVE:			return( LNG("[CAP] Save T.I.N.") );

	case ID_DLG_TEXT_OPEN:			return( LNG("[CAP] Load Text") );
	case ID_DLG_TEXT_SAVE:			return( LNG("[CAP] Save Text") );

	case ID_DLG_PARAMETERS_OPEN:	return( LNG("[CAP] Load Parameters") );
	case ID_DLG_PARAMETERS_SAVE:	return( LNG("[CAP] Save Parameters") );

	case ID_DLG_COLORS_OPEN:		return( LNG("[CAP] Load Colors") );
	case ID_DLG_COLORS_SAVE:		return( LNG("[CAP] Save Colors") );
	}

	return( LNG("[CAP] Select File") );
}

//---------------------------------------------------------
const char * DLG_Get_FILE_Filter(int ID_DLG)
{
	switch( ID_DLG )
	{
	case ID_DLG_WKSP_OPEN:			return( LNG("[FIL] "
		"All Recognised Files"					"|*.sprj;*.mlb;*.dll;*.so;*sgrd;*.dgm;*.grd;*.shp;*.txt;*.dbf|"
		"SAGA Project (*.sprj)"					"|*.sprj|"
		"SAGA Module Libraries (*.dll, *.so)"	"|*.mlb;*.dll;*.so|"
		"Grids (*.sgrd, *.dgm)"					"|*.sgrd;*.dgm;*.grd|"
		"ESRI Shape Files (*.shp)"				"|*.shp|"
		"Tables (*.txt, *.dbf)"					"|*.txt;*.dbf|"
		"All Files"								"|*.*"
	) );

	case ID_DLG_MODULES_OPEN:		return( LNG("[FIL] SAGA Module Libraries (*.dll, *.so)"	"|*.mlb;*.dll;*.so"		"|All Files|*.*") );

	case ID_DLG_PROJECT_OPEN:
	case ID_DLG_PROJECT_SAVE:		return( LNG("[FIL] SAGA Projects (*.sprj)"				"|*.sprj"				"|All Files|*.*") );

	case ID_DLG_GRIDS_OPEN:			return( LNG("[FIL] Grids (*.sgrd, *.dgm)"				"|*.sgrd;*.dgm;*.grd"	"|All Files|*.*") );
	case ID_DLG_GRIDS_SAVE:			return( LNG("[FIL] Grids (*.sgrd)"						"|*.sgrd"				"|All Files|*.*") );

	case ID_DLG_SHAPES_OPEN:
	case ID_DLG_SHAPES_SAVE:		return( LNG("[FIL] ESRI Shape Files (*.shp)"			"|*.shp"				"|All Files|*.*") );

	case ID_DLG_TABLES_OPEN:		return( LNG("[FIL] Tables (*.txt, *.dbf)"				"|*.txt;*.dbf"			"|All Files|*.*") );
	case ID_DLG_TABLES_SAVE:		return( LNG("[FIL] Text (*.txt)|*.txt|DBase (*.dbf)"	"|*.dbf"				"|All Files|*.*") );

	case ID_DLG_TIN_OPEN:
	case ID_DLG_TIN_SAVE:			return( LNG("[FIL] ESRI Shape Files (*.shp)"			"|*.shp"				"|All Files|*.*") );

	case ID_DLG_TEXT_OPEN:
	case ID_DLG_TEXT_SAVE:			return( LNG("[FIL] Text Files (*.txt)"					"|*.txt"				"|All Files|*.*") );

	case ID_DLG_PARAMETERS_OPEN:
	case ID_DLG_PARAMETERS_SAVE:	return( LNG("[FIL] SAGA Parameter Files (*.sprm)"		"|*.sprm"				"|All Files|*.*") );

	case ID_DLG_COLORS_OPEN:
	case ID_DLG_COLORS_SAVE:		return( LNG("[FIL] SAGA Colors (*.pal)"					"|*.pal"				"|All Files|*.*") );
	}

	return( LNG("[FIL] All Files|*.*") );
}

//---------------------------------------------------------
const char * DLG_Get_FILE_Config(int ID_DLG)
{
	switch( ID_DLG )
	{
	case ID_DLG_WKSP_OPEN:			return( "ALL_LOAD" );

	case ID_DLG_MODULES_OPEN:		return( "MLB_LOAD" );

	case ID_DLG_PROJECT_OPEN:		return( "PRJ_LOAD" );
	case ID_DLG_PROJECT_SAVE:		return( "PRJ_SAVE" );

	case ID_DLG_GRIDS_OPEN:			return( "GRD_LOAD" );
	case ID_DLG_GRIDS_SAVE:			return( "GRD_SAVE" );

	case ID_DLG_SHAPES_OPEN:		return( "SHP_LOAD" );
	case ID_DLG_SHAPES_SAVE:		return( "SHP_SAVE" );

	case ID_DLG_TABLES_OPEN:		return( "TAB_LOAD" );
	case ID_DLG_TABLES_SAVE:		return( "TAB_SAVE" );

	case ID_DLG_TIN_OPEN:			return( "TIN_LOAD" );
	case ID_DLG_TIN_SAVE:			return( "TIN_SAVE" );

	case ID_DLG_TEXT_OPEN:			return( "TXT_LOAD" );
	case ID_DLG_TEXT_SAVE:			return( "TXT_SAVE" );

	case ID_DLG_PARAMETERS_OPEN:	return( "PRM_LOAD" );
	case ID_DLG_PARAMETERS_SAVE:	return( "PRM_SAVE" );

	case ID_DLG_COLORS_OPEN:		return( "COL_LOAD" );
	case ID_DLG_COLORS_SAVE:		return( "COL_SAVE" );
	}

	return( "FILE" );
}


///////////////////////////////////////////////////////////
//														 //
//														 //
//														 //
///////////////////////////////////////////////////////////

//---------------------------------------------------------
wxRect		DLG_Get_Def_Rect(void)
{
	wxRect	r(wxGetClientDisplayRect());

	r.Deflate((int)(0.15 * r.GetWidth()), (int)(0.15 * r.GetHeight()));

	return( r );
}

//---------------------------------------------------------
wxPoint		DLG_Get_Def_Position(void)
{
	return( DLG_Get_Def_Rect().GetPosition() );
}

//---------------------------------------------------------
wxSize		DLG_Get_Def_Size(void)
{
	return( DLG_Get_Def_Rect().GetSize() );
}


///////////////////////////////////////////////////////////
//														 //
//														 //
//														 //
///////////////////////////////////////////////////////////

//---------------------------------------------------------
bool		DLG_Parameters(CParameters *pParameters)
{
	if( pParameters && pParameters->Get_Count() > 0 )
	{
		CDLG_Parameters	dlg(pParameters);

		return( dlg.ShowModal() == wxID_OK );
	}

	return( false );
}

//---------------------------------------------------------
bool		DLG_Text(const char *Caption, wxString &Text)
{
	CDLG_Text		dlg(&Text, Caption);

	return( dlg.ShowModal() == wxID_OK );
}

//---------------------------------------------------------
bool		DLG_Table(const char *Caption, CTable *pTable)
{
	CDLG_Table		dlg(pTable, Caption);

	return( dlg.ShowModal() == wxID_OK );
}

//---------------------------------------------------------
bool		DLG_List_Grid(const char *Caption, CParameter_Grid_List *pList)
{
	CDLG_List_Grid		dlg(pList, Caption);

	return( dlg.ShowModal() == wxID_OK );
}

//---------------------------------------------------------
bool		DLG_List_Table(const char *Caption, CParameter_Table_List *pList)
{
	CDLG_List_Table		dlg(pList, Caption);

	return( dlg.ShowModal() == wxID_OK );
}

//---------------------------------------------------------
bool		DLG_List_Shapes(const char *Caption, CParameter_Shapes_List *pList)
{
	CDLG_List_Shapes	dlg(pList, Caption);

	return( dlg.ShowModal() == wxID_OK );
}

//---------------------------------------------------------
bool		DLG_List_TIN(const char *Caption, CParameter_TIN_List *pList)
{
	CDLG_List_TIN		dlg(pList, Caption);

	return( dlg.ShowModal() == wxID_OK );
}

//---------------------------------------------------------
bool		DLG_Colors(CSG_Colors *pColors)
{
	CDLG_Colors		dlg(pColors);

	return( dlg.ShowModal() == wxID_OK );
}


///////////////////////////////////////////////////////////
//														 //
//														 //
//														 //
///////////////////////////////////////////////////////////

//---------------------------------------------------------
bool		DLG_Colors(int &Palette)
{
	wxString	Palettes[SG_COLORS_COUNT];

	Palettes[SG_COLORS_DEFAULT]			= LNG("default");
	Palettes[SG_COLORS_DEFAULT_BRIGHT]	= LNG("default (same brightness)");
	Palettes[SG_COLORS_BLACK_WHITE]		= LNG("greyscale");
	Palettes[SG_COLORS_BLACK_RED]		= LNG("black > red");
	Palettes[SG_COLORS_BLACK_GREEN]		= LNG("black > green");
	Palettes[SG_COLORS_BLACK_BLUE]		= LNG("black > blue");
	Palettes[SG_COLORS_WHITE_RED]		= LNG("white > red");
	Palettes[SG_COLORS_WHITE_GREEN]		= LNG("white > green");
	Palettes[SG_COLORS_WHITE_BLUE]		= LNG("white > blue");
	Palettes[SG_COLORS_YELLOW_RED]		= LNG("yellow > red");
	Palettes[SG_COLORS_YELLOW_GREEN]	= LNG("yellow > green");
	Palettes[SG_COLORS_YELLOW_BLUE]		= LNG("yellow > blue");
	Palettes[SG_COLORS_RED_GREEN]		= LNG("red > green");
	Palettes[SG_COLORS_RED_BLUE]		= LNG("red > blue");
	Palettes[SG_COLORS_GREEN_BLUE]		= LNG("green > blue");
	Palettes[SG_COLORS_RED_GREY_BLUE]	= LNG("red > grey > blue");
	Palettes[SG_COLORS_RED_GREY_GREEN]	= LNG("red > grey > green");
	Palettes[SG_COLORS_GREEN_GREY_BLUE]	= LNG("green > grey > blue");
	Palettes[SG_COLORS_RED_GREEN_BLUE]	= LNG("red > green > blue");
	Palettes[SG_COLORS_RED_BLUE_GREEN]	= LNG("red > blue > green");
	Palettes[SG_COLORS_GREEN_RED_BLUE]	= LNG("green > red > blue");
	Palettes[SG_COLORS_RAINBOW]			= LNG("Rainbow");
	Palettes[SG_COLORS_NEON]			= LNG("Neon");

	wxSingleChoiceDialog	dlg(
		MDI_Get_Frame(),
		"",
		LNG("[CAP] Preset Selection"),		
		SG_COLORS_COUNT, Palettes
	);

	if( dlg.ShowModal() == wxID_OK )
	{
		Palette	= dlg.GetSelection();

		return( true );
	}

	return( false );
}

//---------------------------------------------------------
bool		DLG_Color(long &_Colour)
{
	wxColour		Colour(SG_GET_R(_Colour), SG_GET_G(_Colour), SG_GET_B(_Colour));
	wxColourDialog	dlg(MDI_Get_Frame());

	dlg.GetColourData().SetColour(Colour);

	if( dlg.ShowModal() == wxID_OK )
	{
		Colour	= dlg.GetColourData().GetColour();
		_Colour	= Get_Color_asInt(Colour);

		return( true );
	}

	return( false );
}

//---------------------------------------------------------
bool		DLG_Font(wxFont *pFont, long &_Colour)
{
	wxColour		Colour(SG_GET_R(_Colour), SG_GET_G(_Colour), SG_GET_B(_Colour));
	wxFontDialog	dlg(MDI_Get_Frame());

	dlg.GetFontData().SetInitialFont(*pFont);
	dlg.GetFontData().SetColour(Colour);

	if( dlg.ShowModal() == wxID_OK )
	{
		*pFont	= dlg.GetFontData().GetChosenFont();
		Colour	= dlg.GetFontData().GetColour();
		_Colour	= Get_Color_asInt(Colour);

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
bool		DLG_Get_Number(double &Number, const char *Caption, const char *Text)
{
	wxTextEntryDialog	dlg(MDI_Get_Frame(), Text, Caption, wxString::Format("%f", Number));

	return( dlg.ShowModal() == wxID_OK && dlg.GetValue().ToDouble(&Number) );
}

bool		DLG_Get_Number(double &Number)
{
	return( DLG_Get_Number(Number, LNG("[CAP] Input"), LNG("[DLG] Please enter a numeric value:")) );
}

//---------------------------------------------------------
bool		DLG_Get_Number(int &Number, const char *Caption, const char *Text)
{
	long				lValue;
	wxTextEntryDialog	dlg(MDI_Get_Frame(), Text, Caption, wxString::Format("%d", Number));

	if( dlg.ShowModal() == wxID_OK && dlg.GetValue().ToLong(&lValue) )
	{
		Number	= lValue;

		return( true );
	}

	return( false );
}

bool		DLG_Get_Number(int &Number)
{
	return( DLG_Get_Number(Number, LNG("[CAP] Input"), LNG("[DLG] Please enter a numeric value:")) );
}


///////////////////////////////////////////////////////////
//														 //
//														 //
//														 //
///////////////////////////////////////////////////////////

//---------------------------------------------------------
bool		DLG_Directory(wxString &Directory, const char *Caption, const char *def_Dir)
{
	wxDirDialog	dlg(MDI_Get_Frame(), Caption, def_Dir);

	if( dlg.ShowModal() == wxID_OK )
	{
		Directory	= dlg.GetPath();

		return( true );
	}

	return( false );
}

bool		DLG_Directory(wxString &Directory, const char *Caption)
{
	return( DLG_Directory(Directory, Caption, SG_File_Get_Path(Directory)) );
}

//---------------------------------------------------------
bool		DLG_Save(wxString &File_Path, const char *Caption, const char *def_Dir, const char *def_File, const char *Filter)
{
	wxFileDialog	dlg(MDI_Get_Frame(), Caption, def_Dir, def_File, Filter, wxSAVE|wxOVERWRITE_PROMPT);

	if( dlg.ShowModal() == wxID_OK )
	{
		File_Path	= dlg.GetPath();

		return( true );
	}

	return( false );
}

bool		DLG_Save(wxString &File_Path, int ID_DLG)
{
	wxString	def_Dir;

	CONFIG_Read(CONFIG_GROUP_FILE_DLG, DLG_Get_FILE_Config(ID_DLG), def_Dir);

	if( DLG_Save(File_Path, DLG_Get_FILE_Caption(ID_DLG), def_Dir, "", DLG_Get_FILE_Filter(ID_DLG)) )
	{
		CONFIG_Write(CONFIG_GROUP_FILE_DLG, DLG_Get_FILE_Config(ID_DLG), SG_File_Get_Path(File_Path));

		return( true );
	}

	return( false );
}

bool		DLG_Save(wxString &File_Path, const char *Caption, const char *Filter)
{
	return( DLG_Save(File_Path, Caption, SG_File_Get_Path(File_Path), SG_File_Get_Name(File_Path, true), Filter) );
}

//---------------------------------------------------------
bool		DLG_Open(wxString &File_Path, const char *Caption, const char *def_Dir, const char *def_File, const char *Filter)
{
	wxFileDialog	dlg(MDI_Get_Frame(), Caption, def_Dir, def_File, Filter, wxOPEN|wxFILE_MUST_EXIST);

	if( dlg.ShowModal() == wxID_OK )
	{
		File_Path	= dlg.GetPath();

		return( true );
	}

	return( false );
}

bool		DLG_Open(wxString &File_Path, int ID_DLG)
{
	wxString	def_Dir;

	CONFIG_Read(CONFIG_GROUP_FILE_DLG, DLG_Get_FILE_Config(ID_DLG), def_Dir);

	if( DLG_Open(File_Path, DLG_Get_FILE_Caption(ID_DLG), def_Dir, "", DLG_Get_FILE_Filter(ID_DLG)) )
	{
		CONFIG_Write(CONFIG_GROUP_FILE_DLG, DLG_Get_FILE_Config(ID_DLG), SG_File_Get_Path(File_Path));

		return( true );
	}

	return( false );
}

bool		DLG_Open(wxString &File_Path, const char *Caption, const char *Filter)
{
	return( DLG_Open(File_Path, Caption, SG_File_Get_Path(File_Path), SG_File_Get_Name(File_Path, true), Filter) );
}

//---------------------------------------------------------
bool		DLG_Open(wxArrayString &File_Paths, const char *Caption, const char *def_Dir, const char *Filter)
{
	wxFileDialog	dlg(MDI_Get_Frame(), Caption, def_Dir, "", Filter, wxOPEN|wxFILE_MUST_EXIST|wxMULTIPLE);

	if( dlg.ShowModal() == wxID_OK )
	{
		dlg.GetPaths(File_Paths);

		return( File_Paths.GetCount() > 0 );
	}

	return( false );
}

bool		DLG_Open(wxArrayString &File_Paths, int ID_DLG)
{
	wxString	def_Dir;

	CONFIG_Read(CONFIG_GROUP_FILE_DLG, DLG_Get_FILE_Config(ID_DLG), def_Dir);

	if( DLG_Open(File_Paths, DLG_Get_FILE_Caption(ID_DLG), def_Dir, DLG_Get_FILE_Filter(ID_DLG)) )
	{
		CONFIG_Write(CONFIG_GROUP_FILE_DLG, DLG_Get_FILE_Config(ID_DLG), SG_File_Get_Path(File_Paths[0]));

		return( true );
	}

	return( false );
}

bool		DLG_Open(wxArrayString &File_Paths, const char *Caption, const char *Filter)
{
	return( DLG_Open(File_Paths, Caption, "", Filter) );
}


///////////////////////////////////////////////////////////
//														 //
//														 //
//														 //
///////////////////////////////////////////////////////////

//---------------------------------------------------------
bool		DLG_Image_Save(wxString &File_Path, int &Type, const char *def_Dir, const char *def_File)
{
	static	int	Filter_Index	= 3;

	wxFileDialog	dlg(
		MDI_Get_Frame(), LNG("[CAP] Save As Image"), def_Dir, def_File,
			"Windows or OS/2 Bitmap (*.bmp)"				"|*.bmp|"
			"JPEG - JFIF Compliant (*.jpg, *.jif, *.jpeg)"	"|*.jpg;*.jif;*.jpeg|"
			"Tagged Image File Format (*.tif, *.tiff)"		"|*.tif;*.tiff|"
			"Portable Network Graphics (*.png)"				"|*.png|"
			"CompuServe Graphics Interchange (*.gif)"		"|*.gif|"
			"Zsoft Paintbrush (*.pcx)"						"|*.pcx",
		wxSAVE|wxOVERWRITE_PROMPT
	);

	dlg.SetFilterIndex(Filter_Index);

	if( dlg.ShowModal() == wxID_OK )
	{
		File_Path		= dlg.GetPath();
		Filter_Index	= dlg.GetFilterIndex();

		switch( Filter_Index )
		{
		default:
		case 0:	Type	= wxBITMAP_TYPE_BMP;	break;
		case 1:	Type	= wxBITMAP_TYPE_JPEG;	break;
		case 2:	Type	= wxBITMAP_TYPE_TIF;	break;
		case 3:	Type	= wxBITMAP_TYPE_PNG;	break;
		case 4:	Type	= wxBITMAP_TYPE_GIF;	break;
 		case 5:	Type	= wxBITMAP_TYPE_PCX;	break;
		case 6:	Type	= wxBITMAP_TYPE_PNM;	break;
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
void		DLG_Message_Show(const char *Message, const char *Caption)
{
	wxMessageDialog	dlg(MDI_Get_Frame(), Message, Caption, wxOK);

	dlg.ShowModal();
}

void		DLG_Message_Show(int ID_DLG)
{
	DLG_Message_Show(DLG_Get_Text(ID_DLG), DLG_Get_Caption(ID_DLG));
}

void		DLG_Message_Show(const char *Message)
{
	DLG_Message_Show(Message, DLG_Get_Caption(-1));
}

//---------------------------------------------------------
int			DLG_Message_Show_Error(const char *Message, const char *Caption)
{
	wxMessageDialog	dlg(MDI_Get_Frame(), Message, Caption, wxOK|wxCANCEL|wxICON_ERROR);

	switch( dlg.ShowModal() )
	{
		case wxID_OK: default:
			return( 1 );
			
		case wxID_CANCEL:
			return( 0 );
	}
}

int			DLG_Message_Show_Error(int ID_DLG)
{
	return( DLG_Message_Show_Error(DLG_Get_Text(ID_DLG), DLG_Get_Caption(ID_DLG)) );
}

//---------------------------------------------------------
bool		DLG_Message_Confirm(const char *Message, const char *Caption)
{
	wxMessageDialog	dlg(MDI_Get_Frame(), Message, Caption, wxYES_NO|wxICON_QUESTION);

	return( dlg.ShowModal() == wxID_YES );
}

bool		DLG_Message_Confirm(int ID_DLG)
{
	return( DLG_Message_Confirm(DLG_Get_Text(ID_DLG), DLG_Get_Caption(ID_DLG)) );
}


///////////////////////////////////////////////////////////
//														 //
//														 //
//														 //
///////////////////////////////////////////////////////////

//---------------------------------------------------------
int			DLG_Maps_Add(void)
{
	bool		bOk;
	int			i;
	wxString	*Maps;

	if( g_pMaps )
	{
		if( g_pMaps->Get_Count() <= 0 )
		{
			return( 0 );
		}
		else
		{
			Maps	= new wxString[g_pMaps->Get_Count() + 1];

			for(i=0; i<g_pMaps->Get_Count(); i++)
			{
				Maps[i]	= g_pMaps->Get_Map(i)->Get_Name();
			}

			Maps[i]	= LNG("[VAL] New");

			wxSingleChoiceDialog	dlg(
				MDI_Get_Frame(),
				LNG("[CAP] Map Selection"),
				LNG("[DLG] Add layer to selected map"),
				g_pMaps->Get_Count() + 1,
				Maps
			);

			dlg.SetSelection(g_pMaps->Get_Count());

			bOk		= dlg.ShowModal() == wxID_OK;

			delete[](Maps);

			if( bOk )
			{
				return( dlg.GetSelection() );
			}
		}
	}

	return( -1 );
}


///////////////////////////////////////////////////////////
//														 //
//														 //
//														 //
///////////////////////////////////////////////////////////

//---------------------------------------------------------

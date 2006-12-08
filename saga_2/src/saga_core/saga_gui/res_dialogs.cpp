
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
#define CONFIG_GROUP_FILE_DLG	wxT("/FileDialogs")


///////////////////////////////////////////////////////////
//														 //
//														 //
//														 //
///////////////////////////////////////////////////////////

//---------------------------------------------------------
const wxChar * DLG_Get_Text(int ID_DLG)
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
const wxChar * DLG_Get_Caption(int ID_DLG)
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
const wxChar * DLG_Get_FILE_Caption(int ID_DLG)
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

	case ID_DLG_TIN_OPEN:			return( LNG("[CAP] Load TIN") );
	case ID_DLG_TIN_SAVE:			return( LNG("[CAP] Save TIN") );

	case ID_DLG_TEXT_OPEN:			return( LNG("[CAP] Load Text") );
	case ID_DLG_TEXT_SAVE:			return( LNG("[CAP] Save Text") );

	case ID_DLG_PARAMETERS_OPEN:	return( LNG("[CAP] Load Settings") );
	case ID_DLG_PARAMETERS_SAVE:	return( LNG("[CAP] Save Settings") );

	case ID_DLG_COLORS_OPEN:		return( LNG("[CAP] Load Colors") );
	case ID_DLG_COLORS_SAVE:		return( LNG("[CAP] Save Colors") );
	}

	return( LNG("[CAP] Select File") );
}

//---------------------------------------------------------
const wxString DLG_Get_FILE_Filter(int ID_DLG)
{
	switch( ID_DLG )
	{
	case ID_DLG_WKSP_OPEN:
		return( wxString::Format(
			wxT("%s|*.sprj;*.mlb;*.dll;*.so;*sgrd;*.dgm;*.grd;*.shp;*.txt;*.dbf|")
			wxT("%s|*.sprj|")
			wxT("%s|*.mlb;*.dll;*.so|")
			wxT("%s|*.sgrd;*.dgm;*.grd|")
			wxT("%s|*.shp|")
			wxT("%s|*.txt;*.dbf|")
			wxT("%s|*.*"),
			LNG("All Recognised Files"),
			LNG("SAGA Project (*.sprj)"),
			LNG("SAGA Module Libraries (*.dll, *.so)"),
			LNG("Grids (*.sgrd, *.dgm)"),
			LNG("ESRI Shape Files (*.shp)"),
			LNG("Tables (*.txt, *.dbf)"),
			LNG("All Files")
		));

	case ID_DLG_MODULES_OPEN:
		return( wxString::Format(wxT("%s|%s|%s|*.*"),
			LNG("SAGA Module Libraries (*.dll, *.so)"),
			wxT("*.mlb;*.dll;*.so"),
			LNG("All Files")
		));

	case ID_DLG_PROJECT_OPEN:
	case ID_DLG_PROJECT_SAVE:
		return( wxString::Format(wxT("%s|%s|%s|*.*"),
			LNG("SAGA Projects (*.sprj)"),
			wxT("*.sprj"),
			LNG("All Files")
		));

	case ID_DLG_GRIDS_OPEN:
		return( wxString::Format(wxT("%s|%s|%s|*.*"),
			LNG("Grids (*.sgrd, *.dgm)"),
			wxT("*.sgrd;*.dgm;*.grd"),
			LNG("All Files")
		));

	case ID_DLG_GRIDS_SAVE:
		return( wxString::Format(wxT("%s|%s|%s|*.*"),
			LNG("Grids (*.sgrd)"),
			wxT("*.sgrd"),
			LNG("All Files")
		));

	case ID_DLG_SHAPES_OPEN:
	case ID_DLG_SHAPES_SAVE:
		return( wxString::Format(wxT("%s|%s|%s|*.*"),
			LNG("ESRI Shape Files (*.shp)"),
			wxT("*.shp"),
			LNG("All Files")
		));

	case ID_DLG_TABLES_OPEN:
		return( wxString::Format(wxT("%s|%s|%s|*.*"),
			LNG("Tables (*.txt, *.dbf)"),
			wxT("*.txt;*.dbf"),
			LNG("All Files")
		));
	case ID_DLG_TABLES_SAVE:
		return( wxString::Format(wxT("%s|%s|%s|%s|%s|*.*"),
			LNG("Text (*.txt)"),
			wxT("*.txt"),
			LNG("DBase (*.dbf)"),
			wxT("*.dbf"),
			LNG("All Files")
		));

	case ID_DLG_TIN_OPEN:
	case ID_DLG_TIN_SAVE:
		return( wxString::Format(wxT("%s|%s|%s|*.*"),
			LNG("ESRI Shape Files (*.shp)"),
			wxT("*.shp"),
			LNG("All Files")
		));

	case ID_DLG_TEXT_OPEN:
	case ID_DLG_TEXT_SAVE:
		return( wxString::Format(wxT("%s|%s|%s|*.*"),
			LNG("Text Files (*.txt)"),
			wxT("*.txt"),
			LNG("All Files")
		));

	case ID_DLG_PARAMETERS_OPEN:
	case ID_DLG_PARAMETERS_SAVE:
		return( wxString::Format(wxT("%s|%s|%s|*.*"),
			LNG("SAGA Parameter Files (*.sprm)"),
			wxT("*.sprm"),
			LNG("All Files")
		));

	case ID_DLG_COLORS_OPEN:
	case ID_DLG_COLORS_SAVE:
		return( wxString::Format(wxT("%s|%s|%s|*.*"),
			LNG("SAGA Colors (*.pal)"),
			wxT("*.pal"),
			LNG("All Files")
		));
	}

	return( wxString::Format(wxT("%s|*.*"), LNG("All Files")) );
}

//---------------------------------------------------------
const wxChar * DLG_Get_FILE_Config(int ID_DLG)
{
	switch( ID_DLG )
	{
	case ID_DLG_WKSP_OPEN:			return( wxT("ALL_LOAD") );

	case ID_DLG_MODULES_OPEN:		return( wxT("MLB_LOAD") );

	case ID_DLG_PROJECT_OPEN:		return( wxT("PRJ_LOAD") );
	case ID_DLG_PROJECT_SAVE:		return( wxT("PRJ_SAVE") );

	case ID_DLG_GRIDS_OPEN:			return( wxT("GRD_LOAD") );
	case ID_DLG_GRIDS_SAVE:			return( wxT("GRD_SAVE") );

	case ID_DLG_SHAPES_OPEN:		return( wxT("SHP_LOAD") );
	case ID_DLG_SHAPES_SAVE:		return( wxT("SHP_SAVE") );

	case ID_DLG_TABLES_OPEN:		return( wxT("TAB_LOAD") );
	case ID_DLG_TABLES_SAVE:		return( wxT("TAB_SAVE") );

	case ID_DLG_TIN_OPEN:			return( wxT("TIN_LOAD") );
	case ID_DLG_TIN_SAVE:			return( wxT("TIN_SAVE") );

	case ID_DLG_TEXT_OPEN:			return( wxT("TXT_LOAD") );
	case ID_DLG_TEXT_SAVE:			return( wxT("TXT_SAVE") );

	case ID_DLG_PARAMETERS_OPEN:	return( wxT("PRM_LOAD") );
	case ID_DLG_PARAMETERS_SAVE:	return( wxT("PRM_SAVE") );

	case ID_DLG_COLORS_OPEN:		return( wxT("COL_LOAD") );
	case ID_DLG_COLORS_SAVE:		return( wxT("COL_SAVE") );
	}

	return( wxT("FILE") );
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
bool		DLG_Parameters(CSG_Parameters *pParameters)
{
	if( pParameters && pParameters->Get_Count() > 0 )
	{
		CDLG_Parameters	dlg(pParameters);

		return( dlg.ShowModal() == wxID_OK );
	}

	return( false );
}

//---------------------------------------------------------
bool		DLG_Text(const wxChar *Caption, wxString &Text)
{
	CDLG_Text		dlg(&Text, Caption);

	return( dlg.ShowModal() == wxID_OK );
}

//---------------------------------------------------------
bool		DLG_Table(const wxChar *Caption, CSG_Table *pTable)
{
	CDLG_Table		dlg(pTable, Caption);

	return( dlg.ShowModal() == wxID_OK );
}

//---------------------------------------------------------
bool		DLG_List_Grid(const wxChar *Caption, CSG_Parameter_Grid_List *pList)
{
	CDLG_List_Grid		dlg(pList, Caption);

	return( dlg.ShowModal() == wxID_OK );
}

//---------------------------------------------------------
bool		DLG_List_Table(const wxChar *Caption, CSG_Parameter_Table_List *pList)
{
	CDLG_List_Table		dlg(pList, Caption);

	return( dlg.ShowModal() == wxID_OK );
}

//---------------------------------------------------------
bool		DLG_List_Shapes(const wxChar *Caption, CSG_Parameter_Shapes_List *pList)
{
	CDLG_List_Shapes	dlg(pList, Caption);

	return( dlg.ShowModal() == wxID_OK );
}

//---------------------------------------------------------
bool		DLG_List_TIN(const wxChar *Caption, CSG_Parameter_TIN_List *pList)
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
		wxT(""),
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
bool		DLG_Get_Number(double &Number, const wxChar *Caption, const wxChar *Text)
{
	wxTextEntryDialog	dlg(MDI_Get_Frame(), Text, Caption, wxString::Format(wxT("%f"), Number));

	return( dlg.ShowModal() == wxID_OK && dlg.GetValue().ToDouble(&Number) );
}

bool		DLG_Get_Number(double &Number)
{
	return( DLG_Get_Number(Number, LNG("[CAP] Input"), LNG("[DLG] Please enter a numeric value:")) );
}

//---------------------------------------------------------
bool		DLG_Get_Number(int &Number, const wxChar *Caption, const wxChar *Text)
{
	long				lValue;
	wxTextEntryDialog	dlg(MDI_Get_Frame(), Text, Caption, wxString::Format(wxT("%d"), Number));

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
bool		DLG_Directory(wxString &Directory, const wxChar *Caption, const wxChar *def_Dir)
{
	wxDirDialog	dlg(MDI_Get_Frame(), Caption, def_Dir);

	if( dlg.ShowModal() == wxID_OK )
	{
		Directory	= dlg.GetPath();

		return( true );
	}

	return( false );
}

bool		DLG_Directory(wxString &Directory, const wxChar *Caption)
{
	return( DLG_Directory(Directory, Caption, SG_File_Get_Path(Directory)) );
}

//---------------------------------------------------------
bool		DLG_Save(wxString &File_Path, const wxChar *Caption, const wxChar *def_Dir, const wxChar *def_File, const wxChar *Filter)
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

	if( DLG_Save(File_Path, DLG_Get_FILE_Caption(ID_DLG), def_Dir, wxT(""), DLG_Get_FILE_Filter(ID_DLG)) )
	{
		CONFIG_Write(CONFIG_GROUP_FILE_DLG, DLG_Get_FILE_Config(ID_DLG), SG_File_Get_Path(File_Path));

		return( true );
	}

	return( false );
}

bool		DLG_Save(wxString &File_Path, const wxChar *Caption, const wxChar *Filter)
{
	return( DLG_Save(File_Path, Caption, SG_File_Get_Path(File_Path), SG_File_Get_Name(File_Path, true), Filter) );
}

//---------------------------------------------------------
bool		DLG_Open(wxString &File_Path, const wxChar *Caption, const wxChar *def_Dir, const wxChar *def_File, const wxChar *Filter)
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

	if( DLG_Open(File_Path, DLG_Get_FILE_Caption(ID_DLG), def_Dir, wxT(""), DLG_Get_FILE_Filter(ID_DLG)) )
	{
		CONFIG_Write(CONFIG_GROUP_FILE_DLG, DLG_Get_FILE_Config(ID_DLG), SG_File_Get_Path(File_Path));

		return( true );
	}

	return( false );
}

bool		DLG_Open(wxString &File_Path, const wxChar *Caption, const wxChar *Filter)
{
	return( DLG_Open(File_Path, Caption, SG_File_Get_Path(File_Path), SG_File_Get_Name(File_Path, true), Filter) );
}

//---------------------------------------------------------
bool		DLG_Open(wxArrayString &File_Paths, const wxChar *Caption, const wxChar *def_Dir, const wxChar *Filter)
{
	wxFileDialog	dlg(MDI_Get_Frame(), Caption, def_Dir, wxT(""), Filter, wxOPEN|wxFILE_MUST_EXIST|wxMULTIPLE);

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

bool		DLG_Open(wxArrayString &File_Paths, const wxChar *Caption, const wxChar *Filter)
{
	return( DLG_Open(File_Paths, Caption, wxT(""), Filter) );
}


///////////////////////////////////////////////////////////
//														 //
//														 //
//														 //
///////////////////////////////////////////////////////////

//---------------------------------------------------------
bool		DLG_Image_Save(wxString &File_Path, int &Type, const wxChar *def_Dir, const wxChar *def_File)
{
	static	int	Filter_Index	= 3;

	wxFileDialog	dlg(
		MDI_Get_Frame(), LNG("[CAP] Save As Image"), def_Dir, def_File, wxString::Format(
			wxT("%s|*.bmp|")
			wxT("%s|*.jpg;*.jif;*.jpeg|")
			wxT("%s|*.tif;*.tiff|")
			wxT("%s|*.png|")
			wxT("%s|*.gif|")
			wxT("%s|*.pcx"),
			LNG("Windows or OS/2 Bitmap (*.bmp)"),
			LNG("JPEG - JFIF Compliant (*.jpg, *.jif, *.jpeg)"),
			LNG("Tagged Image File Format (*.tif, *.tiff)"),
			LNG("Portable Network Graphics (*.png)"),
			LNG("CompuServe Graphics Interchange (*.gif)"),
			LNG("Zsoft Paintbrush (*.pcx)")
		), wxSAVE|wxOVERWRITE_PROMPT
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
void		DLG_Message_Show(const wxChar *Message, const wxChar *Caption)
{
	wxMessageDialog	dlg(MDI_Get_Frame(), Message, Caption, wxOK);

	dlg.ShowModal();
}

void		DLG_Message_Show(int ID_DLG)
{
	DLG_Message_Show(DLG_Get_Text(ID_DLG), DLG_Get_Caption(ID_DLG));
}

void		DLG_Message_Show(const wxChar *Message)
{
	DLG_Message_Show(Message, DLG_Get_Caption(-1));
}

//---------------------------------------------------------
int			DLG_Message_Show_Error(const wxChar *Message, const wxChar *Caption)
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
bool		DLG_Message_Confirm(const wxChar *Message, const wxChar *Caption)
{
	wxMessageDialog	dlg(MDI_Get_Frame(), Message, Caption, wxYES_NO|wxICON_QUESTION);

	return( dlg.ShowModal() == wxID_YES );
}

bool		DLG_Message_Confirm(int ID_DLG)
{
	return( DLG_Message_Confirm(DLG_Get_Text(ID_DLG), DLG_Get_Caption(ID_DLG)) );
}

//---------------------------------------------------------
int			DLG_Message_YesNoCancel(const wxChar *Message, const wxChar *Caption)
{
	wxMessageDialog	dlg(MDI_Get_Frame(), Message, Caption, wxYES|wxNO|wxCANCEL|wxICON_QUESTION);

	switch( dlg.ShowModal() )
	{
		case wxID_YES: default:
			return( 0 );

		case wxID_NO:
			return( 1 );

		case wxID_CANCEL:
			return( 2 );
	}
}

int			DLG_Message_YesNoCancel(int ID_DLG)
{
	return( DLG_Message_YesNoCancel(DLG_Get_Text(ID_DLG), DLG_Get_Caption(ID_DLG)) );
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

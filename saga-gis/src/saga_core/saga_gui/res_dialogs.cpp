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

#include "saga_frame.h"

#include "res_dialogs.h"

#include "helper.h"

#include "dlg_parameters.h"
#include "dlg_text.h"
#include "dlg_table.h"
#include "dlg_list_grid.h"
#include "dlg_list_table.h"
#include "dlg_list_shapes.h"
#include "dlg_list_tin.h"
#include "dlg_list_pointcloud.h"
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
wxString DLG_Get_Text(int ID_DLG)
{
	switch( ID_DLG )
	{
	case ID_DLG_ABOUT:					return( _TL("SAGA\nSystem for Automated Geoscientific Analyses\nVersion 2.0") );
	case ID_DLG_CLOSE:					return( _TL("Do you want to exit SAGA?") );
	case ID_DLG_DELETE:					return( _TL("Do you want to delete the selection?") );
	}

	return( _TL("This is not a text!") );
}

//---------------------------------------------------------
wxString DLG_Get_Caption(int ID_DLG)
{
	switch( ID_DLG )
	{
	case ID_DLG_CLOSE:					return( _TL("Exit") );
	case ID_DLG_ABOUT:					return( _TL("About SAGA") );
	case ID_DLG_DELETE:					return( _TL("Delete") );
	}

	return( _TL("This Is Not A Caption!") );
}


///////////////////////////////////////////////////////////
//														 //
//														 //
//														 //
///////////////////////////////////////////////////////////

//---------------------------------------------------------
wxString DLG_Get_FILE_Caption(int ID_DLG)
{
	switch( ID_DLG )
	{
	case ID_DLG_WKSP_OPEN:			return( _TL("Load") );

	case ID_DLG_MODULES_OPEN:		return( _TL("Load Module Library") );

	case ID_DLG_PROJECT_OPEN:		return( _TL("Load Project") );
	case ID_DLG_PROJECT_SAVE:		return( _TL("Save Project") );

	case ID_DLG_GRIDS_OPEN:			return( _TL("Load Grid") );
	case ID_DLG_GRIDS_SAVE:			return( _TL("Save Grid") );

	case ID_DLG_SHAPES_OPEN:		return( _TL("Load Shapes") );
	case ID_DLG_SHAPES_SAVE:		return( _TL("Save Shapes") );

	case ID_DLG_TABLES_OPEN:		return( _TL("Load Table") );
	case ID_DLG_TABLES_SAVE:		return( _TL("Save Table") );

	case ID_DLG_TIN_OPEN:			return( _TL("Load TIN") );
	case ID_DLG_TIN_SAVE:			return( _TL("Save TIN") );

	case ID_DLG_POINTCLOUD_OPEN:	return( _TL("Load Point Cloud") );
	case ID_DLG_POINTCLOUD_SAVE:	return( _TL("Save Point Cloud") );

	case ID_DLG_TEXT_OPEN:			return( _TL("Load Text") );
	case ID_DLG_TEXT_SAVE:			return( _TL("Save Text") );

	case ID_DLG_PARAMETERS_OPEN:	return( _TL("Load Settings") );
	case ID_DLG_PARAMETERS_SAVE:	return( _TL("Save Settings") );

	case ID_DLG_COLORS_OPEN:		return( _TL("Load Colors") );
	case ID_DLG_COLORS_SAVE:		return( _TL("Save Colors") );
	}

	return( _TL("Select File") );
}

//---------------------------------------------------------
wxString DLG_Get_FILE_Filter(int ID_DLG)
{
	switch( ID_DLG )
	{
	case ID_DLG_ALLFILES_OPEN:
		return( wxString::Format(
			wxT("%s|*.sprj;*.mlb;*.dll;*.so;*sgrd;*.dgm;*.grd;*.shp;*.spc;*.txt;*.csv;*.dbf;*.bmp;*.gif;*.jpg;*.png;*.pcx;*.tif;*.img;*.asc;*.flt;*.bil|")
			wxT("%s (*.sprj)|*.sprj|")
			wxT("%s (*.sgrd)|*.sgrd;*.dgm;*.grd|")
			wxT("%s (*.shp)|*.shp|")
			wxT("%s (*.txt, *.csv, *.dbf)|*.txt;*.csv;*.dbf|")
			wxT("%s (*.spc)|*.spc|")
			wxT("%s|*.*"),
			_TL("Recognised Files"),
			_TL("SAGA Projects"),
			_TL("Grids"),
			_TL("ESRI Shape Files"),
			_TL("Tables"),
			_TL("Point Clouds"),
			_TL("All Files")
		));

	case ID_DLG_WKSP_OPEN:
		return( wxString::Format(
			wxT("%s|*.sprj;*.mlb;*.dll;*.so;*sgrd;*.dgm;*.grd;*.shp;*.spc;*.txt;*.csv;*.dbf|")
			wxT("%s (*.sprj)|*.sprj|")
			wxT("%s (*.dll, *.so)|*.dll;*.so;*.mlb|")
			wxT("%s (*.sgrd)|*.sgrd;*.dgm;*.grd|")
			wxT("%s (*.shp)|*.shp|")
			wxT("%s (*.txt, *.csv, *.dbf)|*.txt;*.csv;*.dbf|")
			wxT("%s (*.spc)|*.spc|")
			wxT("%s|*.*"),
			_TL("All Recognised Files"),
			_TL("SAGA Projects"),
			_TL("SAGA Module Libraries"),
			_TL("Grids"),
			_TL("ESRI Shape Files"),
			_TL("Tables"),
			_TL("Point Clouds"),
			_TL("All Files")
		));

	case ID_DLG_MODULES_OPEN:
		return( wxString::Format(
			wxT("%s (*.dll, *.so)|*.mlb;*.dll;*.so|")
			wxT("%s|*.*"),
			_TL("SAGA Module Libraries"),
			_TL("All Files")
		));

	case ID_DLG_PROJECT_OPEN:
	case ID_DLG_PROJECT_SAVE:
		return( wxString::Format(
			wxT("%s (*.sprj)|*.sprj|")
			wxT("%s|*.*"),
			_TL("SAGA Projects"),
			_TL("All Files")
		));

	case ID_DLG_GRIDS_OPEN:
		return( wxString::Format(
			wxT("%s (*.sgrd)|*.sgrd;*.dgm;*.grd|")
			wxT("%s|*.*"),
			_TL("Grids"),
			_TL("All Files")
		));

	case ID_DLG_GRIDS_SAVE:
		return( wxString::Format(
			wxT("%s (*.sgrd)|*.sgrd|")
			wxT("%s|*.*"),
			_TL("Grids"),
			_TL("All Files")
		));

	case ID_DLG_SHAPES_OPEN:
	case ID_DLG_SHAPES_SAVE:
		return( wxString::Format(
			wxT("%s (*.shp)|*.shp|")
			wxT("%s|*.*"),
			_TL("ESRI Shape Files"),
			_TL("All Files")
		));

	case ID_DLG_TABLES_OPEN:
		return( wxString::Format(
			wxT("%s (*.txt, *.csv, *.dbf)|*.txt;*.csv;*.dbf|")
			wxT("%s|*.*"),
			_TL("Tables"),
			_TL("All Files")
		));

	case ID_DLG_TABLES_SAVE:
		return( wxString::Format(
			wxT("%s (*.txt)|*.txt|")
			wxT("%s (*.csv)|*.csv|")
			wxT("%s (*.dbf)|*.dbf|")
			wxT("%s|*.*"),
			_TL("Text"),
			_TL("Comma Separated Values"),
			_TL("DBase"),
			_TL("All Files")
		));

	case ID_DLG_TIN_OPEN:
	case ID_DLG_TIN_SAVE:
		return( wxString::Format(
			wxT("%s (*.shp)|*.shp|")
			wxT("%s|*.*"),
			_TL("ESRI Shape Files"),
			_TL("All Files")
		));

	case ID_DLG_POINTCLOUD_OPEN:
	case ID_DLG_POINTCLOUD_SAVE:
		return( wxString::Format(
			wxT("%s (*.spc)|*.spc|")
			wxT("%s|*.*"),
			_TL("SAGA Point Clouds"),
			_TL("All Files")
		));

	case ID_DLG_TEXT_OPEN:
	case ID_DLG_TEXT_SAVE:
		return( wxString::Format(
			wxT("%s (*.txt)|*.txt|")
			wxT("%s|*.*"),
			_TL("Text Files"),
			_TL("All Files")
		));

	case ID_DLG_PARAMETERS_OPEN:
	case ID_DLG_PARAMETERS_SAVE:
		return( wxString::Format(
			wxT("%s (*.sprm)|*.sprm|")
			wxT("%s|*.*"),
			_TL("SAGA Parameter Files"),
			_TL("All Files")
		));

	case ID_DLG_COLORS_OPEN:
	case ID_DLG_COLORS_SAVE:
		return( wxString::Format(
			wxT("%s (*.pal)|*.pal|")
			wxT("%s|*.*"),
			_TL("SAGA Colors"),
			_TL("All Files")
		));
	}

	return( wxString::Format(wxT("%s|*.*"), _TL("All Files")) );
}

//---------------------------------------------------------
wxString DLG_Get_FILE_Config(int ID_DLG)
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

	case ID_DLG_POINTCLOUD_OPEN:	return( wxT("PTS_LOAD") );
	case ID_DLG_POINTCLOUD_SAVE:	return( wxT("PTS_SAVE") );

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
//	wxRect	r(wxGetClientDisplayRect());
	wxRect	r(g_pSAGA_Frame->GetScreenRect());

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
bool		DLG_Parameters(CSG_Parameters *pParameters, const wxString &Caption)
{
	bool	bResult	= false;

	if( pParameters && pParameters->Get_Count() > 0 )
	{
		CSG_String	Name	= pParameters->Get_Name();

		if( !Caption.IsEmpty() )
		{
			pParameters->Set_Name(&Caption);
		}

		CDLG_Parameters	dlg(pParameters);

		bResult	= dlg.ShowModal() == wxID_OK;

		pParameters->Set_Name(Name);
	}

	return( bResult );
}

//---------------------------------------------------------
bool		DLG_Text(const wxString &Caption, wxString &Text)
{
	CDLG_Text		dlg(&Text, Caption);

	return( dlg.ShowModal() == wxID_OK );
}

//---------------------------------------------------------
bool		DLG_Login(wxString &Username, wxString &Password, const wxString &Caption)
{
	CSG_Parameters	Login(NULL, _TL("Login"), _TL(""));

	if( Caption.Length() > 0 )
	{
		Login.Set_Name(&Caption);
	}

	Login.Add_String(NULL, "USERNAME", _TL("Username"), _TL(""), &Username, false, false);
	Login.Add_String(NULL, "PASSWORD", _TL("Password"), _TL(""), &Password, false, true );

	if( DLG_Parameters(&Login) )
	{
		Username	= Login("USERNAME")->asString();
		Password	= Login("PASSWORD")->asString();

		return( true );
	}

	return( false );
}

//---------------------------------------------------------
bool		DLG_Table(const wxString &Caption, CSG_Table *pTable)
{
	CDLG_Table		dlg(pTable, Caption);

	return( dlg.ShowModal() == wxID_OK );
}

//---------------------------------------------------------
bool		DLG_Table_Fields(const wxString &Caption, CSG_Parameter_Table_Fields *pFields)
{
	CSG_Table	*pTable	= pFields->Get_Table();

	if( pTable )
	{
		int				i;
		CSG_Parameters	P;

		for(i=0; i<pTable->Get_Field_Count(); i++)
		{
			P.Add_Value(NULL, SG_Get_String(i, 0), pTable->Get_Field_Name(i), _TL(""), PARAMETER_TYPE_Bool, false);
		}

		for(i=0; i<pFields->Get_Count(); i++)
		{
			P(pFields->Get_Index(i))->Set_Value(true);
		}

		if( DLG_Parameters(&P) )
		{
			CSG_String	s;

			for(i=0; i<pTable->Get_Field_Count(); i++)
			{
				if( P(i)->asBool() )
				{
					s	+= CSG_String::Format(s.Length() ? SG_T(",%d") : SG_T("%d"), i);
				}
			}

			pFields->Set_Value(s);

			return( true );
		}
	}

	return( false );
}

//---------------------------------------------------------
bool		DLG_List(const wxString &Caption, CSG_Parameter_List *pList)
{
	CDLG_List_Base	*pDialog;

	switch( pList->Get_Type() )
	{
	default:								pDialog	= NULL;																break;
	case PARAMETER_TYPE_Grid_List:			pDialog	= new CDLG_List_Grid      ((CSG_Parameter_Grid_List       *)pList, Caption);	break;
	case PARAMETER_TYPE_Table_List:			pDialog	= new CDLG_List_Table     ((CSG_Parameter_Table_List      *)pList, Caption);	break;
	case PARAMETER_TYPE_Shapes_List:		pDialog	= new CDLG_List_Shapes    ((CSG_Parameter_Shapes_List     *)pList, Caption);	break;
	case PARAMETER_TYPE_TIN_List:			pDialog	= new CDLG_List_TIN       ((CSG_Parameter_TIN_List        *)pList, Caption);	break;
	case PARAMETER_TYPE_PointCloud_List:	pDialog	= new CDLG_List_PointCloud((CSG_Parameter_PointCloud_List *)pList, Caption);	break;
	}

	if( pDialog )
	{
		bool	bResult	= pDialog->ShowModal() == wxID_OK;

		delete(pDialog);

		return( bResult );
	}

	return( false );
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

	for(int i=0; i<SG_COLORS_COUNT; i++)
	{
		Palettes[i]	= SG_Colors_Get_Name(i).c_str();
	}

	wxSingleChoiceDialog	dlg(
		MDI_Get_Top_Window(),
		wxT(""),
		_TL("Preset Selection"),		
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
	static wxColourData	Colours;

	Colours.SetChooseFull(true);

	wxColour		Colour(SG_GET_R(_Colour), SG_GET_G(_Colour), SG_GET_B(_Colour));
	wxColourDialog	dlg(MDI_Get_Top_Window(), &Colours);

	dlg.GetColourData().SetColour(Colour);

	if( dlg.ShowModal() == wxID_OK )
	{
		Colours	= dlg.GetColourData();
		Colour	= dlg.GetColourData().GetColour();
		_Colour	= Get_Color_asInt(Colour);

		return( true );
	}

	return( false );
}

//---------------------------------------------------------
bool		DLG_Font(CSG_Parameter *pFont)
{
	wxFont		Font;
	wxColour	Colour;

	if( Set_Font(pFont, Font, Colour) )
	{
		wxFontDialog	dlg(MDI_Get_Top_Window());

		dlg.GetFontData().SetInitialFont(Font);
		dlg.GetFontData().SetColour     (Colour);

		if( dlg.ShowModal() == wxID_OK )
		{
			Font	= dlg.GetFontData().GetChosenFont();
			Colour	= dlg.GetFontData().GetColour();

			return( Set_Font(Font, Colour, pFont) );
		}
	}

	return( false );
}


///////////////////////////////////////////////////////////
//														 //
//														 //
//														 //
///////////////////////////////////////////////////////////

//---------------------------------------------------------
bool		DLG_Get_Number(double &Number, const wxString &Caption, const wxString &Text)
{
	wxTextEntryDialog	dlg(MDI_Get_Top_Window(), Text, Caption, wxString::Format(wxT("%f"), Number));

	return( dlg.ShowModal() == wxID_OK && dlg.GetValue().ToDouble(&Number) );
}

bool		DLG_Get_Number(double &Number)
{
	return( DLG_Get_Number(Number, _TL("Input"), _TL("Please enter a numeric value:")) );
}

//---------------------------------------------------------
bool		DLG_Get_Number(int &Number, const wxString &Caption, const wxString &Text)
{
	long				lValue;
	wxTextEntryDialog	dlg(MDI_Get_Top_Window(), Text, Caption, wxString::Format(wxT("%d"), Number));

	if( dlg.ShowModal() == wxID_OK && dlg.GetValue().ToLong(&lValue) )
	{
		Number	= lValue;

		return( true );
	}

	return( false );
}

bool		DLG_Get_Number(int &Number)
{
	return( DLG_Get_Number(Number, _TL("Input"), _TL("Please enter a numeric value:")) );
}


///////////////////////////////////////////////////////////
//														 //
//														 //
//														 //
///////////////////////////////////////////////////////////

//---------------------------------------------------------
bool		DLG_Directory(wxString &Directory, const wxString &Caption, const wxString &def_Dir)
{
	wxDirDialog	dlg(MDI_Get_Top_Window(), Caption, def_Dir);

	if( dlg.ShowModal() == wxID_OK )
	{
		Directory	= dlg.GetPath();

		return( true );
	}

	return( false );
}

bool		DLG_Directory(wxString &Directory, const wxString &Caption)
{
	return( DLG_Directory(Directory, Caption, SG_File_Get_Path(Directory).w_str()) );
}

//---------------------------------------------------------
bool		DLG_Save(wxString &File_Path, const wxString &Caption, const wxString &def_Dir, const wxString &def_File, const wxString &Filter)
{
	wxFileDialog	dlg(MDI_Get_Top_Window(), Caption, def_Dir, def_File, Filter, wxFD_SAVE|wxFD_OVERWRITE_PROMPT);

	if( dlg.ShowModal() == wxID_OK )
	{
		File_Path	= dlg.GetPath();

		return( true );
	}

	return( false );
}

bool		DLG_Save(wxString &File_Path, int ID_DLG)
{
	wxString	def_Dir, def_Name;

	def_Name	= SG_File_Get_Name(File_Path, true).w_str();
	def_Dir		= SG_File_Get_Path(File_Path).w_str();

	if( !wxFileExists(def_Dir) )
	{
		CONFIG_Read(CONFIG_GROUP_FILE_DLG, DLG_Get_FILE_Config(ID_DLG), def_Dir);
	}

	if( DLG_Save(File_Path, DLG_Get_FILE_Caption(ID_DLG), def_Dir, def_Name, DLG_Get_FILE_Filter(ID_DLG)) )
	{
		CONFIG_Write(CONFIG_GROUP_FILE_DLG, DLG_Get_FILE_Config(ID_DLG), SG_File_Get_Path(File_Path).w_str());

		return( true );
	}

	return( false );
}

bool		DLG_Save(wxString &File_Path, const wxString &Caption, const wxString &Filter)
{
	return( DLG_Save(File_Path, Caption, SG_File_Get_Path(File_Path).w_str(), SG_File_Get_Name(File_Path, true).w_str(), Filter) );
}

//---------------------------------------------------------
bool		DLG_Open(wxString &File_Path, const wxString &Caption, const wxString &def_Dir, const wxString &def_File, const wxString &Filter)
{
	wxFileDialog	dlg(MDI_Get_Top_Window(), Caption, def_Dir, def_File, Filter, wxFD_OPEN|wxFD_FILE_MUST_EXIST);

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
		CONFIG_Write(CONFIG_GROUP_FILE_DLG, DLG_Get_FILE_Config(ID_DLG), SG_File_Get_Path(File_Path).w_str());

		return( true );
	}

	return( false );
}

bool		DLG_Open(wxString &File_Path, const wxString &Caption, const wxString &Filter)
{
	return( DLG_Open(File_Path, Caption, SG_File_Get_Path(File_Path).w_str(), SG_File_Get_Name(File_Path, true).w_str(), Filter) );
}

//---------------------------------------------------------
bool		DLG_Open(wxArrayString &File_Paths, const wxString &Caption, const wxString &def_Dir, const wxString &Filter)
{
	wxFileDialog	dlg(MDI_Get_Top_Window(), Caption, def_Dir, wxT(""), Filter, wxFD_OPEN|wxFD_FILE_MUST_EXIST|wxFD_MULTIPLE);

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
		CONFIG_Write(CONFIG_GROUP_FILE_DLG, DLG_Get_FILE_Config(ID_DLG), SG_File_Get_Path(File_Paths[0]).w_str());

		return( true );
	}

	return( false );
}

bool		DLG_Open(wxArrayString &File_Paths, const wxString &Caption, const wxString &Filter)
{
	return( DLG_Open(File_Paths, Caption, wxT(""), Filter) );
}


///////////////////////////////////////////////////////////
//														 //
//														 //
//														 //
///////////////////////////////////////////////////////////

//---------------------------------------------------------
bool		DLG_Image_Save(wxString &File_Path, int &Type, const wxString &def_Dir, const wxString &def_File)
{
	static	int	Filter_Index	= 3;

	wxFileDialog	dlg(
		MDI_Get_Top_Window(), _TL("Save As Image"), def_Dir, def_File, wxString::Format(
			wxT("%s (*.bmp)|*.bmp|")
			wxT("%s (*.jpg)|*.jpg;*.jif;*.jpeg|")
			wxT("%s (*.tif)|*.tif;*.tiff|")
			wxT("%s (*.png)|*.png|")
			wxT("%s (*.gif)|*.gif|")
			wxT("%s (*.pcx)|*.pcx"),
			_TL("Windows or OS/2 Bitmap"),
			_TL("JPEG - JFIF Compliant"),
			_TL("Tagged Image File Format"),
			_TL("Portable Network Graphics"),
			_TL("CompuServe Graphics Interchange"),
			_TL("Zsoft Paintbrush")
		), wxFD_SAVE|wxFD_OVERWRITE_PROMPT
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
void		DLG_Message_Show(const wxString &Message, const wxString &Caption)
{
	wxMessageDialog	dlg(MDI_Get_Top_Window(), Message, Caption, wxOK);

	dlg.ShowModal();
}

void		DLG_Message_Show(int ID_DLG)
{
	DLG_Message_Show(DLG_Get_Text(ID_DLG), DLG_Get_Caption(ID_DLG));
}

void		DLG_Message_Show(const wxString &Message)
{
	DLG_Message_Show(Message, DLG_Get_Caption(-1));
}

//---------------------------------------------------------
int			DLG_Message_Show_Error(const wxString &Message, const wxString &Caption)
{
	wxMessageDialog	dlg(MDI_Get_Top_Window(), Message, Caption, wxOK|wxCANCEL|wxICON_ERROR);

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
bool		DLG_Message_Confirm(const wxString &Message, const wxString &Caption)
{
	wxMessageDialog	dlg(MDI_Get_Top_Window(), Message, Caption, wxYES_NO|wxICON_EXCLAMATION);

	return( dlg.ShowModal() == wxID_YES );
}

bool		DLG_Message_Confirm(int ID_DLG)
{
	return( DLG_Message_Confirm(DLG_Get_Text(ID_DLG), DLG_Get_Caption(ID_DLG)) );
}

//---------------------------------------------------------
int			DLG_Message_YesNoCancel(const wxString &Message, const wxString &Caption)
{
	wxMessageDialog	dlg(MDI_Get_Top_Window(), Message, Caption, wxYES|wxNO|wxCANCEL|wxICON_QUESTION);

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

			Maps[i]	= _TL("New");

			wxSingleChoiceDialog	dlg(
				MDI_Get_Top_Window(),
				_TL("Map Selection"),
				_TL("Add layer to selected map"),
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

/**********************************************************
 * Version $Id: gdal_import.cpp 1921 2014-01-09 10:24:11Z oconrad $
 *********************************************************/

///////////////////////////////////////////////////////////
//                                                       //
//                         SAGA                          //
//                                                       //
//      System for Automated Geoscientific Analyses      //
//                                                       //
//                     Tool Library                      //
//                                                       //
//                       io_gdal                         //
//                                                       //
//-------------------------------------------------------//
//                                                       //
//                   gdal_import.cpp                     //
//                                                       //
//            Copyright (C) 2007 O. Conrad               //
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
//    e-mail:     oconrad@saga-gis.de                    //
//                                                       //
//    contact:    Olaf Conrad                            //
//                Bundesstr. 55                          //
//                D-20146 Hamburg                        //
//                Germany                                //
//                                                       //
///////////////////////////////////////////////////////////


///////////////////////////////////////////////////////////
//														 //
//														 //
//														 //
///////////////////////////////////////////////////////////

//---------------------------------------------------------
#include "gdal_import.h"


///////////////////////////////////////////////////////////
//														 //
//														 //
//														 //
///////////////////////////////////////////////////////////

//---------------------------------------------------------
CGDAL_Import::CGDAL_Import(void)
{
	//-----------------------------------------------------
	Set_Name	(_TL("Import Raster"));

	Set_Author	("O.Conrad (c) 2007 (A.Ringeler)");

	CSG_String	Description, Filter, Filter_All;

	Description	= _TW(
		"The \"GDAL Raster Import\" tool imports grid data from various file formats using the "
		"\"Geospatial Data Abstraction Library\" (GDAL) by Frank Warmerdam. "
		"For more information have a look at the GDAL homepage:\n"
		"  <a target=\"_blank\" href=\"http://www.gdal.org/\">"
		"  http://www.gdal.org</a>\n"
	);

	Description	+= CSG_String::Format("\nGDAL %s:%s\n\n", _TL("Version"), SG_Get_GDAL_Drivers().Get_Version().c_str());

	Description	+= _TL("Following raster formats are currently supported:");

	Description	+= CSG_String::Format("\n<table border=\"1\"><tr><th>%s</th><th>%s</th><th>%s</th></tr>",
		_TL("ID"), _TL("Name"), _TL("Extension")
	);

	for(int i=0; i<SG_Get_GDAL_Drivers().Get_Count(); i++)
    {
		if( SG_Get_GDAL_Drivers().is_Raster(i) && SG_Get_GDAL_Drivers().Can_Read(i) )
		{
			CSG_String	ID		= SG_Get_GDAL_Drivers().Get_Description(i).c_str();
			CSG_String	Name	= SG_Get_GDAL_Drivers().Get_Name       (i).c_str();
			CSG_String	Ext		= SG_Get_GDAL_Drivers().Get_Extension  (i).c_str();

			Description	+= "<tr><td>" + ID + "</td><td>" + Name + "</td><td>" + Ext + "</td></tr>";

			if( !Ext.is_Empty() )
			{
				Ext.Replace("/", ";");

				Filter		+= Name + "|*." + Ext + "|";
				Filter_All	+= (Filter_All.is_Empty() ? "*." : ";*.") + Ext;
			}
		}
    }

	Description	+= "</table>";

	Set_Description(Description);

	Filter.Prepend(CSG_String::Format("%s|%s|" , _TL("All Recognized Files"), Filter_All.c_str()));
	Filter.Append (CSG_String::Format("%s|*.*" , _TL("All Files")));

	//-----------------------------------------------------
	Parameters.Add_Grid_List(NULL,
		"GRIDS"		, _TL("Grids"),
		_TL(""),
		PARAMETER_OUTPUT, false
	);

	Parameters.Add_FilePath(NULL,
		"FILES"		, _TL("Files"),
		_TL(""),
		Filter, NULL, false, false, true
	);

	//-----------------------------------------------------
	Parameters.Add_String(NULL,
		"SELECTION"		, _TL("Select from Multiple Bands"),
		_TL("Semicolon separated list of band indexes. Do not set to select all bands for import."),
		""
	)->Set_UseInGUI(false);

	Parameters.Add_Bool(NULL,
		"SELECT"		, _TL("Select from Multiple Bands"),
		_TL(""),
		true
	)->Set_UseInCMD(false);

	Parameters.Add_Bool(SG_UI_Get_Window_Main() ? Parameters("SELECT") : NULL,
		"SELECT_SORT"	, _TL("Alphanumeric Sorting"),
		_TL(""),
		true
	);

	//-----------------------------------------------------
	Parameters.Add_Bool(NULL,
		"TRANSFORM"		, _TL("Transformation"),
		_TL("align grid to coordinate system"),
		true
	);

	Parameters.Add_Choice(Parameters("TRANSFORM"),
		"RESAMPLING"	, _TL("Resampling"),
		_TL("Resampling type to be used, if grid needs to be aligned to coordinate system."),
		CSG_String::Format("%s|%s|%s|%s|",
			_TL("Nearest Neighbour"),
			_TL("Bilinear Interpolation"),
			_TL("Bicubic Spline Interpolation"),
			_TL("B-Spline Interpolation")
		), 0
	);
}


///////////////////////////////////////////////////////////
//														 //
///////////////////////////////////////////////////////////

//---------------------------------------------------------
int CGDAL_Import::On_Parameters_Enable(CSG_Parameters *pParameters, CSG_Parameter *pParameter)
{
	if(	!SG_STR_CMP(pParameter->Get_Identifier(), "TRANSFORM") )
	{
		pParameters->Set_Enabled("RESAMPLING" , pParameter->asBool());
	}

	if(	!SG_STR_CMP(pParameter->Get_Identifier(), "SELECT") )
	{
		pParameters->Set_Enabled("SELECT_SORT", pParameter->asBool());
	}

	return( CSG_Tool::On_Parameters_Enable(pParameters, pParameter) );
}

//---------------------------------------------------------
int CGDAL_Import::On_Selection_Changed(CSG_Parameter *pParameter, int Flags)
{
	if( pParameter && pParameter->Get_Owner() && pParameter->Get_Owner()->Get_Owner() )
	{
		if( Flags & PARAMETER_CHECK_ENABLE )
		{
			if( !SG_STR_CMP(pParameter->Get_Identifier(), "ALL") )
			{
				pParameter->Get_Owner()->Set_Enabled("BANDS", pParameter->asBool() == false);
			}
		}

		return( 1 );
	}

	return( 0 );
}


///////////////////////////////////////////////////////////
//														 //
///////////////////////////////////////////////////////////

//---------------------------------------------------------
bool CGDAL_Import::On_Execute(void)
{
	CSG_Strings	Files;

	if( !Parameters("FILES")->asFilePath()->Get_FilePaths(Files) )
	{
		return( false );
	}

	//-----------------------------------------------------
	(m_pGrids = Parameters("GRIDS")->asGridList())->Del_Items();

	for(int i=0; i<Files.Get_Count(); i++)
	{
		Message_Add(CSG_String::Format("\n%s: %s", _TL("loading"), Files[i].c_str()), false);

		if( Load(Files[i]) == false )
		{
			Message_Add(_TL("failed: could not find a suitable import driver"));
		}
	}

	//-----------------------------------------------------
	return( m_pGrids->Get_Count() > 0 );
}


///////////////////////////////////////////////////////////
//														 //
///////////////////////////////////////////////////////////

//---------------------------------------------------------
bool CGDAL_Import::Load(const CSG_String &File)
{
	//-----------------------------------------------------
	CSG_GDAL_DataSet	DataSet;

	if( DataSet.Open_Read(File) == false )
	{
		return( false );
	}

	//-----------------------------------------------------
	if( DataSet.Get_Count() < 1 )
	{
		return( Load_Subset(DataSet) );
	}

	//-----------------------------------------------------
	CSG_Table	Bands;	Bands.Add_Field("NAME", SG_DATATYPE_String);

	{
		for(int i=0; i<DataSet.Get_Count(); i++)
		{
			Bands.Add_Record()->Set_Value(0, DataSet.Get_Name(i));
		}
	}

	if( Parameters("SELECT_SORT")->asBool() )
	{
		Bands.Set_Index(0, TABLE_INDEX_Ascending);
	}

	//-----------------------------------------------------
	if( DataSet.Get_Count() > 1 )
	{
		if( !SG_UI_Get_Window_Main() )
		{
			CSG_String_Tokenizer	Indexes(Parameters("SELECTION")->asString(), ";,");

			while( Indexes.Has_More_Tokens() )
			{
				int	i;

				if( Indexes.Get_Next_Token().asInt(i) && i >= 0 && i < Bands.Get_Count() )
				{
					Bands.Select(Bands[i].Get_Index(), true);
				}
			}
		}
		else if( Parameters("SELECT")->asBool() )
		{
			CSG_Parameters	Selection(this, _TL("Select from Multiple Bands"), _TL(""), SG_T("SELECTION"));

			Selection.Set_Callback_On_Parameter_Changed(&On_Selection_Changed);

			Selection.Add_Bool(NULL, "ALL"  , _TL("Load all bands"), _TL(""), false);
			Selection.Add_Node(NULL, "BANDS", _TL("Bands"         ), _TL(""));

			for(int i=0; i<Bands.Get_Count(); i++)
			{
				Selection.Add_Bool(Selection(1), SG_Get_String(i), Bands[i].asString(0), CSG_String::Format("%s: %d\n%s: %d\n%s: %s",
					_TL("List Order"), i,
					_TL("Index"     ), Bands[i].Get_Index(),
					_TL("Name"      ), Bands[i].asString(0)), false
				);
			}

			if( Dlg_Parameters(&Selection, _TL("Select from Multiple Bands")) )
			{
				for(int i=0; i<Bands.Get_Count(); i++)
				{
					if( Selection(0)->asBool() || Selection(i + 2)->asBool() )
					{
						Bands.Select(Bands[i].Get_Index(), true);
					}
				}
			}

			if( Bands.Get_Selection_Count() <= 0 )
			{
				return( false );
			}
		}
	}

	//-----------------------------------------------------
	TSG_Grid_Resampling	Resampling;

	switch( Parameters("RESAMPLING")->asInt() )
	{
	default:	Resampling	= GRID_RESAMPLING_NearestNeighbour;	break;
	case  1:	Resampling	= GRID_RESAMPLING_Bilinear;			break;
	case  2:	Resampling	= GRID_RESAMPLING_BicubicSpline;	break;
	case  3:	Resampling	= GRID_RESAMPLING_BSpline;			break;
	}

	CSG_Vector	A;	CSG_Matrix	B;	DataSet.Get_Transformation(A, B);

	bool	bTransform	= Parameters("TRANSFORM")->asBool() && DataSet.Needs_Transformation();

	//-----------------------------------------------------
	Message_Add("\n", false);
	Message_Add(CSG_String::Format("\n%s: %s", _TL("Driver" ), DataSet.Get_DriverID().c_str()), false);
	Message_Add(CSG_String::Format("\n%s: %d", _TL("Bands"  ), DataSet.Get_Count   ()        ), false);
	Message_Add(CSG_String::Format("\n%s: %d", _TL("Rows"   ), DataSet.Get_NX      ()        ), false);
	Message_Add(CSG_String::Format("\n%s: %d", _TL("Columns"), DataSet.Get_NY      ()        ), false);
	Message_Add("\n", false);

	if( DataSet.Needs_Transformation() )
	{
		Message_Add(CSG_String::Format("\n%s:", _TL("Transformation")                               ), false);
		Message_Add(CSG_String::Format("\n  x' = %.6f + x * %.6f + y * %.6f", A[0], B[0][0], B[0][1]), false);
		Message_Add(CSG_String::Format("\n  y' = %.6f + x * %.6f + y * %.6f", A[1], B[1][0], B[1][1]), false);
		Message_Add("\n", false);
	}

	//-----------------------------------------------------
	for(int i=0; i<DataSet.Get_Count() && Process_Get_Okay(); i++)
	{
		if( !Bands.Get_Selection_Count() || Bands[i].is_Selected() )
		{
			CSG_String	Message	= "%s: " + SG_File_Get_Name(File, false);	if( DataSet.Get_Count() > 1 )	Message	+= CSG_String::Format(" [%d/%d]", i + 1, DataSet.Get_Count());

			Process_Set_Text(CSG_String::Format(Message.c_str(), _TL("loading")));

			CSG_Grid	*pGrid	= DataSet.Read(Bands[i].Get_Index());

			if( pGrid != NULL )
			{
				if( bTransform )
				{
					Process_Set_Text(CSG_String::Format(Message.c_str(), _TL("translation")));

					DataSet.Get_Transformation(&pGrid, Resampling, true);
				}

				pGrid->Set_File_Name(DataSet.Get_File_Name());
				pGrid->Set_Name(SG_File_Get_Name(File, false) + (DataSet.Get_Count() == 1 ? CSG_String("") : CSG_String::Format(" [%s]", Bands[i].asString(0))));

				m_pGrids->Add_Item(pGrid);

				DataObject_Add       (pGrid);
				DataObject_Set_Colors(pGrid, CSG_Colors(11, SG_COLORS_RAINBOW, false));
			}
		}
    }

	//-----------------------------------------------------
	return( true );
}


///////////////////////////////////////////////////////////
//														 //
///////////////////////////////////////////////////////////

//---------------------------------------------------------
bool CGDAL_Import::Load_Subset(CSG_GDAL_DataSet &DataSet)
{
	CSG_MetaData	MetaData;

	if( !DataSet.Get_MetaData(MetaData, "SUBDATASETS") )
	{
		return( false );
	}

	//-----------------------------------------------------
	int		i;

	CSG_Parameters	Subsets;

	for(i=0; ; i++)
	{
		CSG_String	ID	= CSG_String::Format("SUBDATASET_%d_", i + 1);

		if( MetaData(ID + "NAME") )
		{
			Subsets.Add_Bool(NULL,
				MetaData.Get_Content(ID + "NAME"),
				MetaData.Get_Content(ID + "DESC"),
				"", SG_UI_Get_Window_Main() == NULL
			);
		}
		else
		{
			break;
		}
	}

	//-----------------------------------------------------
	if( SG_UI_Get_Window_Main() && !Dlg_Parameters(&Subsets, _TL("Select from Subdatasets...")) )	// with gui
	{
		return( false );
	}

	//-----------------------------------------------------
	for(i=0; i<Subsets.Get_Count() && Process_Get_Okay(false); i++)
	{
		if( Subsets(i)->asBool() )
		{
			if( Load(Subsets(i)->Get_Identifier()) == false )
			{
				Error_Fmt("%s: %s", _TL("failed to import subset"), Subsets(i)->Get_Name());
			}
		}
	}

	//-----------------------------------------------------
	return( true );
}


///////////////////////////////////////////////////////////
//														 //
//														 //
//														 //
///////////////////////////////////////////////////////////

//---------------------------------------------------------

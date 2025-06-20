
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
//    e-mail:     oconrad@saga-gis.de                    //
//                                                       //
//    contact:    Olaf Conrad                            //
//                Bundesstr. 55                          //
//                D-20146 Hamburg                        //
//                Germany                                //
//                                                       //
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

	Add_Reference("GDAL/OGR contributors", "2019",
		"GDAL/OGR Geospatial Data Abstraction software Library",
		"A translator library for raster and vector geospatial data formats. Open Source Geospatial Foundation.",
		SG_T("https://gdal.org"), SG_T("Link")
	);

	CSG_String	Description, Filter, Filter_All;

	Description	= _TW(
		"The \"GDAL Raster Import\" tool imports grid data from various file formats using the "
		"\"Geospatial Data Abstraction Library\" (GDAL) by Frank Warmerdam. "
	);

	Description	+= CSG_String::Format("\nGDAL %s:%s\n\n", _TL("Version"), SG_Get_GDAL_Drivers().Get_Version().c_str());

	Description	+= _TL("Following raster formats are currently supported:");

	Description	+= CSG_String::Format("\n<table border=\"1\"><tr><th>%s</th><th>%s</th><th>%s</th></tr>",
		_TL("Name"), _TL("ID"), _TL("Extension")
	);

	typedef struct {
        CSG_String	ID;
		CSG_String	Ext;
    }D_INFO;

	std::map<std::wstring, D_INFO>	Drivers;	// key = long name, values = shortname and file extension (if available)

	for(int i=0; i<SG_Get_GDAL_Drivers().Get_Count(); i++)
    {
		if( SG_Get_GDAL_Drivers().is_Raster(i) && SG_Get_GDAL_Drivers().Can_Read(i) )
		{
			D_INFO d;
			std::wstring Name	= SG_Get_GDAL_Drivers().Get_Name       (i).c_str();
			d.ID				= SG_Get_GDAL_Drivers().Get_Description(i).c_str();
			d.Ext				= SG_Get_GDAL_Drivers().Get_Extension  (i).c_str();

			Drivers.insert(std::pair<std::wstring, D_INFO>(Name, d));
		}
	}

	for(std::map<std::wstring, D_INFO>::iterator it=Drivers.begin(); it!=Drivers.end(); ++it)
	{
		Description	+= "<tr><td>" + CSG_String(it->first.c_str()) + "</td><td>" + it->second.ID  + "</td><td>" + it->second.Ext + "</td></tr>";

		if( !it->second.Ext.is_Empty() )
		{
			it->second.Ext.Replace("/", ";");

			Filter	+= "|" + CSG_String(it->first.c_str()) + "|*." + it->second.Ext;
			Filter_All	+= (Filter_All.is_Empty() ? "*." : ";*.") + it->second.Ext;
		}
	}

	Description	+= "</table>";

	Set_Description(Description);

	Filter.Prepend(CSG_String::Format("%s|%s|%s|*.*", _TL("All Recognized Files"), Filter_All.c_str(), _TL("All Files")));

	//-----------------------------------------------------
	Parameters.Add_Grid_List("",
		"GRIDS"		, _TL("Grids"),
		_TL(""),
		PARAMETER_OUTPUT, false
	);

	Parameters.Add_FilePath("",
		"FILES"		, _TL("Files"),
		_TL(""),
		Filter, NULL, false, false, true
	);

	//-----------------------------------------------------
	Parameters.Add_Choice("",
		"MULTIPLE"		, _TL("Multiple Bands Output"),
		_TL(""),
		CSG_String::Format("%s|%s|%s",
			_TL("single grids"),
			_TL("grid collection"),
			_TL("automatic")
		), 2
	);

	Parameters.Add_String("",
		"SUBSETS"		, _TL("Subsets"),
		_TL("Semicolon separated list of subset names or indexes (zero-based). If empty (default) all subsets will be imported (if there are any)."),
		""
	)->Set_UseInGUI(false);

	Parameters.Add_String("",
		"SELECTION"		, _TL("Select from Multiple Bands"),
		_TL("Semicolon separated list of band indexes (zero-based). If empty (default) all bands will be imported."),
		""
	)->Set_UseInGUI(false);

	Parameters.Add_Bool("",
		"SELECT"		, _TL("Select from Multiple Bands"),
		_TL(""),
		false
	)->Set_UseInCMD(false);

	Parameters.Add_Bool(has_GUI() ? "SELECT" : "",
		"SELECT_SORT"	, _TL("Alphanumeric Sorting"),
		_TL("Sorts bands by their name before the selection dialog opens."),
		false
	)->Set_UseInCMD(false);

	//-----------------------------------------------------
	Parameters.Add_Bool("",
		"TRANSFORM"		, _TL("Transformation"),
		_TL("align grid to coordinate system"),
		true
	);

	Parameters.Add_Choice("TRANSFORM",
		"RESAMPLING"	, _TL("Resampling"),
		_TL("Resampling type to be used, if grid needs to be aligned to coordinate system."),
		CSG_String::Format("%s|%s|%s|%s",
			_TL("Nearest Neighbour"),
			_TL("Bilinear Interpolation"),
			_TL("Bicubic Spline Interpolation"),
			_TL("B-Spline Interpolation")
		), 0
	);

	//-----------------------------------------------------
	Parameters.Add_Choice("",
		"EXTENT"		, _TL("Extent"),
		_TL(""),
		CSG_String::Format("%s|%s|%s|%s",
			_TL("original"),
			_TL("user defined"),
			_TL("grid system"),
			_TL("shapes extent")
		), 0
	);

	Parameters.Add_Double("EXTENT", "EXTENT_XMIN", _TL("Left"  ), _TL(""));
	Parameters.Add_Double("EXTENT", "EXTENT_XMAX", _TL("Right" ), _TL(""));
	Parameters.Add_Double("EXTENT", "EXTENT_YMIN", _TL("Bottom"), _TL(""));
	Parameters.Add_Double("EXTENT", "EXTENT_YMAX", _TL("Top"   ), _TL(""));

	Parameters.Add_Grid_System("EXTENT",
		"EXTENT_GRID"	, _TL("Grid System"),
		_TL("")
	);

	Parameters.Add_Shapes("EXTENT",
		"EXTENT_SHAPES"	, _TL("Shapes Extent"),
		_TL(""),
		PARAMETER_INPUT
	);

	Parameters.Add_Double("EXTENT",
		"EXTENT_BUFFER"	, _TL("Buffer"),
		_TL(""),
		0., 0., true
	);
}


///////////////////////////////////////////////////////////
//														 //
///////////////////////////////////////////////////////////

//---------------------------------------------------------
int CGDAL_Import::On_Parameters_Enable(CSG_Parameters *pParameters, CSG_Parameter *pParameter)
{
	if(	pParameter->Cmp_Identifier("TRANSFORM") )
	{
		pParameters->Set_Enabled("RESAMPLING" , pParameter->asBool());
	}

	if(	pParameter->Cmp_Identifier("SELECT") )
	{
		pParameters->Set_Enabled("SELECT_SORT", pParameter->asBool());
	}

	if(	pParameter->Cmp_Identifier("EXTENT") )
	{
		pParameters->Set_Enabled("EXTENT_XMIN"  , pParameter->asInt() == 1);
		pParameters->Set_Enabled("EXTENT_XMAX"  , pParameter->asInt() == 1);
		pParameters->Set_Enabled("EXTENT_YMIN"  , pParameter->asInt() == 1);
		pParameters->Set_Enabled("EXTENT_YMAX"  , pParameter->asInt() == 1);
		pParameters->Set_Enabled("EXTENT_GRID"  , pParameter->asInt() == 2);
		pParameters->Set_Enabled("EXTENT_SHAPES", pParameter->asInt() == 3);
		pParameters->Set_Enabled("EXTENT_BUFFER", pParameter->asInt() >= 2);
	}

	return( CSG_Tool::On_Parameters_Enable(pParameters, pParameter) );
}

//---------------------------------------------------------
int CGDAL_Import::On_Selection_Changed(CSG_Parameter *pParameter, int Flags)
{
	if( pParameter && pParameter->Get_Parameters() && pParameter->Get_Parameters()->Cmp_Identifier("SELECTION") )
	{
		if( Flags & PARAMETER_CHECK_ENABLE )
		{
			if( pParameter->Cmp_Identifier("ALL") )
			{
				pParameter->Get_Parameters()->Set_Enabled("BANDS", pParameter->asBool() == false);
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
	TSG_Grid_Resampling	Resampling;

	switch( Parameters("RESAMPLING")->asInt() )
	{
	default: Resampling = GRID_RESAMPLING_NearestNeighbour; break;
	case  1: Resampling = GRID_RESAMPLING_Bilinear        ; break;
	case  2: Resampling = GRID_RESAMPLING_BicubicSpline   ; break;
	case  3: Resampling = GRID_RESAMPLING_BSpline         ; break;
	}

	//-----------------------------------------------------
	CSG_Rect Extent; CSG_Projection Projection;

	switch( Parameters("EXTENT")->asInt() )
	{
	case  1:
		Extent.Assign(
			Parameters("EXTENT_XMIN")->asDouble(),
			Parameters("EXTENT_YMIN")->asDouble(),
			Parameters("EXTENT_XMAX")->asDouble(),
			Parameters("EXTENT_YMAX")->asDouble()
		);
		break;

	case  2:
		Extent       = Parameters("EXTENT_GRID"  )->asGrid_System()->Get_Extent();
		Extent.Inflate(Parameters("EXTENT_BUFFER")->asDouble(), false);
		break;

	case  3:
		Projection   = Parameters("EXTENT_SHAPES")->asShapes     ()->Get_Projection();
		Extent       = Parameters("EXTENT_SHAPES")->asShapes     ()->Get_Extent();
		Extent.Inflate(Parameters("EXTENT_BUFFER")->asDouble(), false);
		break;
	}

	//-----------------------------------------------------
	Parameters("GRIDS")->asGridList()->Del_Items();

	for(int i=0; i<Files.Get_Count(); i++)
	{
		Load(Files[i], Resampling, Extent, Projection);
	}

	//-----------------------------------------------------
	return( Parameters("GRIDS")->asGridList()->Get_Grid_Count() > 0 );
}


///////////////////////////////////////////////////////////
//														 //
///////////////////////////////////////////////////////////

//---------------------------------------------------------
bool CGDAL_Import::Load(const CSG_String &File, TSG_Grid_Resampling Resampling, const CSG_Rect &Extent, const CSG_Projection &Projection)
{
	CSG_GDAL_DataSet DataSet;

	if( !DataSet.Open_Read(File) )
	{
		Message_Add(_TL("failed: could not find a suitable import driver"));

		return( false );
	}

	if( DataSet.Get_Count() < 1 )
	{
		return( Load_Subsets(DataSet, Resampling, Extent, Projection) );
	}

	if( Extent.Get_Area() > 0. && !DataSet.Open_Read(File, Extent) )
	{
		Message_Add(_TL("failed: there is no intersection of dataset's extent and targeted extent."));

		return( false );
	}

	//-----------------------------------------------------
	CSG_Table Bands; Bands.Add_Field("NAME", SG_DATATYPE_String);

	for(int i=0; i<DataSet.Get_Count(); i++)
	{
		Bands.Add_Record()->Set_Value(0, DataSet.Get_Name(i));
	}

	//-----------------------------------------------------
	if( DataSet.Get_Count() > 1 )
	{
		if( !has_GUI() )
		{
			CSG_Strings	Indexes	= SG_String_Tokenize(Parameters("SELECTION")->asString(), ";,");

			for(int i=0, Index; i<Indexes.Get_Count(); i++)
			{
				if( Indexes[i].asInt(Index) && Index >= 0 && Index < Bands.Get_Count() )
				{
					Bands.Select(Bands[Index].Get_Index(), true);
				}
			}
		}
		else if( Parameters("SELECT")->asBool() )
		{
			if( Parameters("SELECT_SORT")->asBool() )
			{
				Bands.Set_Index(0, TABLE_INDEX_Ascending);
			}

			CSG_Parameters	P(_TL("Select from Multiple Bands"), _TL(""), SG_T("SELECTION"));

			P.Set_Callback_On_Parameter_Changed(&On_Selection_Changed);

			P.Add_Bool("", "ALL", _TL("Load all bands"), _TL(""), false);

			CSG_Parameter_Choices	*pBands	= P.Add_Choices("", "BANDS", _TL("Bands"), _TL(""), "")->asChoices();

			for(int i=0; i<Bands.Get_Count(); i++)
			{
				pBands->Add_Item(Bands[i].asString(0));
			}

			if( Dlg_Parameters(&P, _TL("Select from Multiple Bands")) )
			{
				for(int i=0; i<Bands.Get_Count(); i++)
				{
					if( P[0].asBool() || pBands->is_Selected(i) )
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
	CSG_Vector A; CSG_Matrix B; DataSet.Get_Transformation(A, B);

	bool bTransform = Parameters("TRANSFORM")->asBool() && DataSet.Needs_Transformation();

	Message_Fmt("\n");
	Message_Fmt("\n%s: %s", _TL("Dataset"), File.c_str());
	Message_Fmt("\n%s: %s", _TL("Driver" ), DataSet.Get_DriverID().c_str());
	Message_Fmt("\n%s: %d", _TL("Bands"  ), DataSet.Get_Count   ()        );
	Message_Fmt("\n%s: %d", _TL("Rows"   ), DataSet.Get_NX      ()        );
	Message_Fmt("\n%s: %d", _TL("Columns"), DataSet.Get_NY      ()        );
	Message_Fmt("\n");

	if( bTransform )
	{
		Message_Fmt("\n%s:", _TL("Transformation")                               );
		Message_Fmt("\n  x' = %.6f + x * %.6f + y * %.6f", A[0], B[0][0], B[0][1]);
		Message_Fmt("\n  y' = %.6f + x * %.6f + y * %.6f", A[1], B[1][0], B[1][1]);
		Message_Fmt("\n");
	}

	//-----------------------------------------------------
	CSG_Array_Pointer pGrids;

	for(int i=0; i<DataSet.Get_Count() && Process_Get_Okay(); i++)
	{
		if( !Bands.Get_Selection_Count() || Bands[i].is_Selected() )
		{
			CSG_String	Message	= "%s: " + SG_File_Get_Name(File, false);	if( DataSet.Get_Count() > 1 )	Message	+= CSG_String::Format(" [%d/%d]", i + 1, DataSet.Get_Count());

			Process_Set_Text(Message.c_str(), _TL("loading"));

			CSG_Grid	*pGrid	= DataSet.Read((int)Bands[i].Get_Index());

			if( pGrid != NULL )
			{
				if( bTransform )
				{
					Process_Set_Text(Message.c_str(), _TL("translation"));

					DataSet.Get_Transformation(&pGrid, Resampling, true);
				}

				if( !Extent.Get_Area() ) // don't associate it with the original file if it's only a subset!
				{
					pGrid->Set_File_Name(DataSet.Get_File_Name());
				}

				pGrid->Set_Name(SG_File_Get_Name(File, false) + (DataSet.Get_Count() == 1 ? CSG_String("") : CSG_String::Format(" [%s]", Bands[i].asString(0))));

				if( !pGrid->Get_Projection().is_Okay() && Projection.is_Okay() )
				{
					pGrid->Get_Projection().Create(Projection);
				}

				pGrids.Add(pGrid);
			}
		}
    }

	//-----------------------------------------------------
	CSG_Parameter_Grid_List	*pList	= Parameters("GRIDS")->asGridList();

	if( Parameters("MULTIPLE")->asInt() == 0 || (Parameters("MULTIPLE")->asInt() == 2 && pGrids.Get_Size() == 1) )
	{
		for(sLong i=0; i<pGrids.Get_Size(); i++)
		{
			pList->Add_Item((CSG_Grid *)pGrids[i]);
		}
	}
	else if( pGrids.Get_Size() > 0 )
	{
		CSG_Grids	*pCollection	= SG_Create_Grids();

		if( !Extent.Get_Area() ) // don't associate it with the original file if it's only a subset!
		{
			pCollection->Set_File_Name(DataSet.Get_File_Name());
		}

		pCollection->Set_Name(SG_File_Get_Name(File, false));
		pCollection->Set_Description(DataSet.Get_Description());

		pCollection->Get_MetaData().Add_Child("GDAL_DRIVER", DataSet.Get_DriverID());

		DataSet.Get_MetaData(*pCollection->Get_MetaData().Add_Child("Metadata"));

		int fDesc = 0;

		for(sLong i=0; !fDesc && i<pGrids.Get_Size(); i++)
		{
			if( *((CSG_Grid *)pGrids.Get(i))->Get_Description() )
			{
				fDesc = pCollection->Get_Attributes().Get_Field_Count();

				pCollection->Add_Attribute("Description", SG_DATATYPE_String);
			}
		}

		for(sLong i=0; i<pGrids.Get_Size(); i++)
		{
			CSG_Grid *pGrid = (CSG_Grid *)pGrids.Get(i); const SG_Char *Description = pGrid->Get_Description();

			pCollection->Add_Grid((double)i, pGrid, true);

			if( fDesc )
			{
				pCollection->Get_Attributes_Ptr()->Set_Value(i, fDesc, Description);
			}
		}

		pList->Add_Item(pCollection);
	}

	//-----------------------------------------------------
	return( true );
}


///////////////////////////////////////////////////////////
//														 //
///////////////////////////////////////////////////////////

//---------------------------------------------------------
bool CGDAL_Import::Load_Subsets(CSG_GDAL_DataSet &DataSet, TSG_Grid_Resampling Resampling, const CSG_Rect &Extent, const CSG_Projection &Projection)
{
	CSG_MetaData MetaData;

	if( !DataSet.Get_MetaData(MetaData, "SUBDATASETS") )
	{
		return( false );
	}

	//-----------------------------------------------------
	CSG_Parameters Subsets;

	for(int i=0; i == Subsets.Get_Count(); i++)
	{
		CSG_String ID = CSG_String::Format("SUBDATASET_%d_", i + 1);

		if( MetaData(ID + "NAME") )
		{
			Subsets.Add_Bool("",
				MetaData.Get_Content(ID + "NAME"),
				MetaData.Get_Content(ID + "DESC"), "", true
			);
		}
	}

	if( has_GUI() && !Dlg_Parameters(&Subsets, _TL("Select from Subdatasets...")) )	// with gui
	{
		return( false );
	}

	for(int i=0; Process_Get_Okay(false) && i<Subsets.Get_Count(); i++)
	{
		if( Subsets[i].asBool() )
		{
			if( !Load(Subsets[i].Get_Identifier(), Resampling, Extent, Projection) )
			{
				Error_Fmt("%s %d: %s", _TL("failed to import subset"), i + 1, Subsets[i].Get_Name());
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

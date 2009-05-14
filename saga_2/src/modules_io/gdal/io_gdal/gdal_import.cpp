
///////////////////////////////////////////////////////////
//                                                       //
//                         SAGA                          //
//                                                       //
//      System for Automated Geoscientific Analyses      //
//                                                       //
//                    Module Library                     //
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
// 59 Temple Place - Suite 330, Boston, MA 02111-1307,   //
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
	Set_Name	(_TL("GDAL: Import Raster"));

	Set_Author	(SG_T("(c) 2007 by O.Conrad (A.Ringeler)"));

	CSG_String	Description;

	Description	= _TW(
		"The \"GDAL Raster Import\" module imports grid data from various file formats using the "
		"\"Geospatial Data Abstraction Library\" (GDAL) by Frank Warmerdam. "
		"For more information have a look at the GDAL homepage:\n"
		"  <a target=\"_blank\" href=\"http://www.gdal.org/\">"
		"  http://www.gdal.org</a>\n"
		"\n"
		"Following raster formats are currently supported:\n"
		"<table border=\"1\"><tr><th>ID</th><th>Name</th></tr>\n"
	);

	for(int i=0; i<g_GDAL_Driver.Get_Count(); i++)
    {
		Description	+= CSG_String::Format(SG_T("<tr><td>%s</td><td>%s</td></tr>\n"),
			SG_STR_MBTOSG(g_GDAL_Driver.Get_Description(i)),
			SG_STR_MBTOSG(g_GDAL_Driver.Get_Name(i))
		);
    }

	Description	+= SG_T("</table>");

	Set_Description(Description);

	//-----------------------------------------------------
	Parameters.Add_Grid_List(
		NULL, "GRIDS"	, _TL("Grids"),
		_TL(""),
		PARAMETER_OUTPUT, false
	);

	Parameters.Add_FilePath(
		NULL, "FILE"	, _TL("File"),
		_TL("")
	);
}

//---------------------------------------------------------
CGDAL_Import::~CGDAL_Import(void)
{}


///////////////////////////////////////////////////////////
//														 //
//														 //
//														 //
///////////////////////////////////////////////////////////

//---------------------------------------------------------
bool CGDAL_Import::On_Execute(void)
{
	CSG_String		File_Name;
	CGDAL_System	System;

	//-----------------------------------------------------
	File_Name	= Parameters("FILE")	->asString();

	m_pGrids	= Parameters("GRIDS")	->asGridList();

	m_pGrids	->Del_Items();

	//-----------------------------------------------------
	if( System.Create(File_Name, IO_READ) == false )
	{
		Message_Add(_TL("could not find suitable import driver"));
	}
	else if( System.Get_Count() <= 0 )
	{
		return( Load_Sub(System, SG_File_Get_Name(File_Name, false)) );
	}
	else
    {
		return( Load(System, SG_File_Get_Name(File_Name, false)) );
	}

	return( false );
}


///////////////////////////////////////////////////////////
//														 //
//														 //
//														 //
///////////////////////////////////////////////////////////

//---------------------------------------------------------
bool CGDAL_Import::Load_Sub(CGDAL_System &System, const CSG_String &Name)
{
	if( System.is_Reading() )
	{
		char	**pMetaData	= System.Get_DataSet()->GetMetadata("SUBDATASETS");

		if( CSLCount(pMetaData) > 0 )
		{
			int				i, n;
			CSG_String		s, sID, sName, sDesc;
			CSG_Parameters	P;

			for(i=0; pMetaData[i]!=NULL; i++)
			{
				Message_Add(CSG_String::Format(SG_T("  %s\n"), pMetaData[i]), false);

				s		= pMetaData[i];

				if( s.Contains(SG_T("SUBDATASET_")) && s.Contains(SG_T("_NAME=")) )
				{
					sID		= s.AfterFirst('_').BeforeFirst('_');
					sName	= s.AfterFirst('=');
					sDesc	= _TL("no description available");

					if( pMetaData[i + 1] != NULL )
					{
						s		= pMetaData[i + 1];

						if( s.Contains(SG_T("SUBDATASET_")) && s.Contains(SG_T("_DESC")) )
						{
							sDesc	= s.AfterFirst ('=');
						}
					}

					P.Add_Value(NULL, sName, sDesc, SG_T(""), PARAMETER_TYPE_Bool, false);
				}
			}

			if( Dlg_Parameters(&P, _TL("Select from Subdatasets...")) )
			{
				for(i=0, n=0; i<P.Get_Count() && Process_Get_Okay(false); i++)
				{
					if( P(i)->asBool() && System.Create(P(i)->Get_Identifier(), IO_READ) && Load(System, P(i)->Get_Name()) )
					{
						n++;
					}
				}

				return( n > 0 );
			}
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
bool CGDAL_Import::Load(CGDAL_System &System, const CSG_String &Name)
{
	//-----------------------------------------------------
	if( System.is_Reading() )
	{
		Message_Add(CSG_String::Format(
			SG_T("\n%s: %s/%s\n"),
			_TL("Driver"),
			System.Get_Driver()->GetDescription(), 
			System.Get_Driver()->GetMetadataItem(GDAL_DMD_LONGNAME)
		), false);

		Message_Add(CSG_String::Format(
			SG_T("%s: x %d, y %d\n%s: %d\n%s x: %.6f, %.6f, %.6f\n%s y: %.6f, %.6f, %.6f"),
			_TL("Cells")			, System.Get_NX(), System.Get_NY(),
			_TL("Bands")			, System.Get_Count(),
			_TL("Transformation")	, System.Get_Transform(0), System.Get_Transform(1), System.Get_Transform(2),
			_TL("Transformation")	, System.Get_Transform(3), System.Get_Transform(4), System.Get_Transform(5)
		), false);

		if( System.Get_Projection() != NULL )
		{
			CSG_String	s(System.Get_Projection());

			s.Replace(SG_T("[")  , SG_T("\t"));
			s.Replace(SG_T("]],"), SG_T("\n"));
			s.Replace(SG_T("]]") , SG_T("\n"));
			s.Replace(SG_T("],") , SG_T("\n"));
			s.Replace(SG_T(",")  , SG_T("\t"));

			Message_Add(CSG_String::Format(
				SG_T("\n%s:\n%s"),
				_TL("Projection"),
				s.c_str()
			), false);
		}

		//-------------------------------------------------
		int			i, n;
		CSG_Grid	*pGrid;

		for(i=0, n=0; i<System.Get_Count(); i++)
		{
			if( (pGrid = System.Read_Band(i)) != NULL )
			{
				n++;

				pGrid->Set_Name(System.Get_Count() > 1
					? CSG_String::Format(SG_T("%s [%02d]"), Name.c_str(), i + 1).c_str()
					: Name.c_str()
				);

				m_pGrids->Add_Item(pGrid);

				DataObject_Add			(pGrid);
				DataObject_Set_Colors	(pGrid, CSG_Colors(100, SG_COLORS_BLACK_WHITE, false));
			}
        }

		//-------------------------------------------------
		return( n > 0 );
	}

	//-----------------------------------------------------
	return( false );
}


///////////////////////////////////////////////////////////
//														 //
//														 //
//														 //
///////////////////////////////////////////////////////////

//---------------------------------------------------------

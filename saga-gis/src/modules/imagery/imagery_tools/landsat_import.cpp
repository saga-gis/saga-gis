/**********************************************************
 * Version $Id: landsat_import.cpp 911 2011-02-14 16:38:15Z reklov_w $
 *********************************************************/

///////////////////////////////////////////////////////////
//                                                       //
//                         SAGA                          //
//                                                       //
//      System for Automated Geoscientific Analyses      //
//                                                       //
//                    Module Library:                    //
//                     imagery_tools                     //
//                                                       //
//-------------------------------------------------------//
//                                                       //
//                   landsat_import.cpp                  //
//                                                       //
//                 Copyright (C) 2014 by                 //
//                      Olaf Conrad                      //
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
//    e-mail:     oconrad@saga-gis.org                   //
//                                                       //
//    contact:    Olaf Conrad                            //
//                Institute of Geography                 //
//                University of Hamburg                  //
//                Germany                                //
//                                                       //
///////////////////////////////////////////////////////////

//---------------------------------------------------------


///////////////////////////////////////////////////////////
//														 //
//														 //
//														 //
///////////////////////////////////////////////////////////

//---------------------------------------------------------
#include "landsat_import.h"


///////////////////////////////////////////////////////////
//														 //
//														 //
//														 //
///////////////////////////////////////////////////////////

//---------------------------------------------------------
CLandsat_Import::CLandsat_Import(void)
{
	//-----------------------------------------------------
	Set_Name		(_TL("Landsat Import with Options"));

	Set_Author		("O.Conrad (c) 2014");

	Set_Description	(_TW(
		"This tool facilitates the import and display of Landsat scenes, "
		"which have each band given as a single GeoTIFF file.\n"
		"\n"
		"The development of this tool has been requested and sponsored by "
		"Rohan Fisher, Charles Darwin University, Australia. "
	));

	//-----------------------------------------------------
	CSG_Parameter	*pNode;

	Parameters.Add_FilePath(
		NULL	, "FILES"		, _TL("Files"),
		_TL(""),
		CSG_String::Format(SG_T("%s|*.tif;*.tiff|%s|*.*"),
			_TL("GeoTIFF (*.tif)"),
			_TL("All Files")
		), NULL, false, false, true
	);

	Parameters.Add_Grid_List(
		NULL	, "BANDS"		, _TL("Bands"),
		_TL(""),
		PARAMETER_OUTPUT, false
	);

	pNode	= Parameters.Add_Choice(
		NULL	, "PROJECTION"	, _TL("Coordinate System"),
		_TL(""),
		CSG_String::Format(SG_T("%s|%s|%s|"),
			_TL("UTM North"),
			_TL("UTM South"),
			_TL("Geographic Coordinates")
		), 0
	);

	Parameters.Add_Choice(
		pNode	, "INTERPOLATION"	, _TL("Interpolation"),
		_TL(""),
		CSG_String::Format(SG_T("%s|%s|%s|"),
			_TL("Nearest Neighbour"),
			_TL("Bilinear Interpolation"),
			_TL("Cubic Convolution")
		), 2
	);

	pNode	= Parameters.Add_Value(
		NULL	, "SHOW_RGB"	, _TL("Show a Composite"),
		_TL(""),
		PARAMETER_TYPE_Bool, true
	);

	Parameters.Add_Choice(pNode, "SHOW_R", _TL("Red"  ), _TL(""), _TL("no choice available"));
	Parameters.Add_Choice(pNode, "SHOW_G", _TL("Green"), _TL(""), _TL("no choice available"));
	Parameters.Add_Choice(pNode, "SHOW_B", _TL("Blue" ), _TL(""), _TL("no choice available"));
}


///////////////////////////////////////////////////////////
//														 //
///////////////////////////////////////////////////////////

//---------------------------------------------------------
int CLandsat_Import::On_Parameters_Enable(CSG_Parameters *pParameters, CSG_Parameter *pParameter)
{
	if( !SG_STR_CMP(pParameter->Get_Identifier(), "FILES") )
	{
		CSG_Strings	Files;	pParameter->asFilePath()->Get_FilePaths(Files);

		if( Files.Get_Count() < 3 )
		{
			pParameters->Set_Enabled("SHOW_RGB", false);
		}
		else
		{
			pParameters->Set_Enabled("SHOW_RGB", true);

			CSG_String	Choices;

			for(int i=0; i<Files.Get_Count(); i++)
			{
				Choices	+= SG_File_Get_Name(Files[i], false) + "|";
			}

			int	iR	= pParameters->Get_Parameter("SHOW_R")->asChoice()->Get_Count() > 1 ? pParameters->Get_Parameter("SHOW_R")->asInt() : 2;
			int	iG	= pParameters->Get_Parameter("SHOW_G")->asChoice()->Get_Count() > 1 ? pParameters->Get_Parameter("SHOW_G")->asInt() : 1;
			int	iB	= pParameters->Get_Parameter("SHOW_B")->asChoice()->Get_Count() > 1 ? pParameters->Get_Parameter("SHOW_B")->asInt() : 0;

			pParameters->Get_Parameter("SHOW_R")->asChoice()->Set_Items(Choices); pParameters->Get_Parameter("SHOW_R")->Set_Value(iR);
			pParameters->Get_Parameter("SHOW_G")->asChoice()->Set_Items(Choices); pParameters->Get_Parameter("SHOW_G")->Set_Value(iG);
			pParameters->Get_Parameter("SHOW_B")->asChoice()->Set_Items(Choices); pParameters->Get_Parameter("SHOW_B")->Set_Value(iB);
		}
	}

	if( !SG_STR_CMP(pParameter->Get_Identifier(), "PROJECTION") )
	{
		pParameters->Set_Enabled("INTERPOLATION", pParameter->asInt() == 2);
	}

	if( !SG_STR_CMP(pParameter->Get_Identifier(), "SHOW_RGB") )
	{
		pParameters->Set_Enabled("SHOW_R", pParameter->asBool());
		pParameters->Set_Enabled("SHOW_G", pParameter->asBool());
		pParameters->Set_Enabled("SHOW_B", pParameter->asBool());
	}

	return( 1 );
}


///////////////////////////////////////////////////////////
//														 //
///////////////////////////////////////////////////////////

//---------------------------------------------------------
bool CLandsat_Import::On_Execute(void)
{
	CSG_Strings	Files;

	if( !Parameters("FILES")->asFilePath()->Get_FilePaths(Files) || Files.Get_Count() <= 0 )
	{
		return( false );
	}

	//-----------------------------------------------------
	CSG_Parameter_Grid_List	*pBands	= Parameters("BANDS")->asGridList();

	pBands->Del_Items();

	for(int i=0; i<Files.Get_Count(); i++)
	{
		Message_Add(CSG_String::Format(SG_T("%s: %s"), _TL("loading"), SG_File_Get_Name(Files[i], false).c_str()));

		CSG_Grid	*pBand	= Get_Band(Files[i]);

		if( pBand )
		{
			pBands->Add_Item(pBand);

			DataObject_Add(pBand);
			DataObject_Set_Colors(pBand, 11, SG_COLORS_BLACK_WHITE);
		}
	}

	//-----------------------------------------------------
	if( Parameters("SHOW_RGB")->is_Enabled() && Parameters("SHOW_RGB")->asBool() )
	{
		CSG_Grid	*pR	= pBands->asGrid(Parameters("SHOW_R")->asInt());
		CSG_Grid	*pG	= pBands->asGrid(Parameters("SHOW_G")->asInt());
		CSG_Grid	*pB	= pBands->asGrid(Parameters("SHOW_B")->asInt());

		if( pR && pG && pB )
		{
			DataObject_Set_Parameter(pR, "COLORS_TYPE" , 5);	// _TL("RGB Overlay")	// CLASSIFY_OVERLAY
			DataObject_Set_Parameter(pR, "OVERLAY_MODE", 0);	// _TL("red=this, green=1, blue=2")
			DataObject_Set_Parameter(pR, "OVERLAY_G"   , pG);
			DataObject_Set_Parameter(pR, "OVERLAY_B"   , pB);

			DataObject_Update(pR, true);
		}
	}

	//-----------------------------------------------------
	return( true );
}


///////////////////////////////////////////////////////////
//														 //
///////////////////////////////////////////////////////////

//---------------------------------------------------------
CSG_Grid * CLandsat_Import::Get_Band(const CSG_String &File)
{
	CSG_Data_Manager	tmpMgr;

	if( !tmpMgr.Add(File) || !tmpMgr.Get_Grid_System(0) || !tmpMgr.Get_Grid_System(0)->Get(0) )
	{
		Error_Set(CSG_String::Format(SG_T("%s: %s"), _TL("could not load file"), File.c_str()));

		return( NULL );
	}

	tmpMgr.Get_Grid_System(0)->Get(0)->Set_NoData_Value(0);	// landsat 8 pretends to use a value of 65535 (2^16 - 1)

	CSG_Grid	*pBand	= NULL;

	//-----------------------------------------------------
	if( !tmpMgr.Get_Grid_System(0)->Get(0)->Get_Projection().is_Okay() )
	{
		// undefined coordinate system, nothing to do be done further...
	}

	//-----------------------------------------------------
	else if( Parameters("PROJECTION")->asInt() == 2 )	// Geographic Coordinates
	{
		pBand	= Get_Projection((CSG_Grid *)tmpMgr.Get_Grid_System(0)->Get(0), "+proj=longlat +ellps=WGS84 +datum=WGS84");
	}

	//-----------------------------------------------------
	else												// UTM
	{
		CSG_Grid	*pTmp	= (CSG_Grid *)tmpMgr.Get_Grid_System(0)->Get(0);

		CSG_String	Projection	= pTmp->Get_Projection().Get_Proj4();

		if( Projection.Find("+proj=utm") >= 0
		&&  (  (Projection.Find("+south") >= 0 && Parameters("PROJECTION")->asInt() == 0)
		    || (Projection.Find("+south") <  0 && Parameters("PROJECTION")->asInt() == 1))
		&&  (pBand = SG_Create_Grid(pTmp->Get_Type(), pTmp->Get_NX(), pTmp->Get_NY(), pTmp->Get_Cellsize(),
				pTmp->Get_XMin(), pTmp->Get_YMin() + (Parameters("PROJECTION")->asInt() == 1 ? 10000000 : -10000000)
			)) != NULL )
		{
			if( Parameters("PROJECTION")->asInt() == 1 )
				Projection.Append (" +south");
			else
				Projection.Replace(" +south", "");

			pBand->Get_Projection().Create(Projection, SG_PROJ_FMT_Proj4);

			pBand->Set_Name              (pTmp->Get_Name());
			pBand->Set_Description       (pTmp->Get_Description());
			pBand->Set_NoData_Value_Range(pTmp->Get_NoData_Value(), pTmp->Get_NoData_hiValue());
			pBand->Set_Scaling           (pTmp->Get_Scaling(), pTmp->Get_Offset());

			#pragma omp parallel for
			for(int y=0; y<pBand->Get_NY(); y++)
			{
				for(int x=0; x<pBand->Get_NX(); x++)
				{
					pBand->Set_Value(x, y, pTmp->asDouble(x, y));
				}
			}
		}
	}

	//-----------------------------------------------------
	if( !pBand )
	{
		pBand	= (CSG_Grid *)tmpMgr.Get_Grid_System(0)->Get(0);

		tmpMgr.Delete(tmpMgr.Get_Grid_System(0)->Get(0), true);	// make permanent, detach from temporary data manager
	}

	return( pBand );
}


///////////////////////////////////////////////////////////
//														 //
///////////////////////////////////////////////////////////

//---------------------------------------------------------
CSG_Grid * CLandsat_Import::Get_Projection(CSG_Grid *pGrid, const CSG_String &Proj4)
{
	if( pGrid->Get_Projection().is_Okay() == false )
	{
		return( NULL );
	}

	CSG_Module	*pModule	= SG_Get_Module_Library_Manager().Get_Module(SG_T("pj_proj4"), 4);	// Coordinate Transformation (Grid)

	if(	pModule == NULL )
	{
		return( NULL );
	}

	int	Interpolation;

	switch( Parameters("INTERPOLATION")->asInt() )
	{
	case  0:	Interpolation	= GRID_RESAMPLING_NearestNeighbour;	break;
	case  1:	Interpolation	= GRID_RESAMPLING_Bilinear        ;	break;
	default:	Interpolation	= GRID_RESAMPLING_BSpline         ;	break;
	}

	Message_Add(CSG_String::Format(SG_T("\n%s (%s: %s)\n"), _TL("re-projection to geographic coordinates"), _TL("original"), pGrid->Get_Projection().Get_Name().c_str()), false);

	pModule->Settings_Push(NULL);

	if( pModule->Set_Parameter("CRS_PROJ4"    , Proj4        )
	&&  pModule->Set_Parameter("INTERPOLATION", Interpolation)
	&&  pModule->Set_Parameter("SOURCE"       , pGrid        )
	&&  pModule->Execute() )
	{
		pGrid	= pModule->Get_Parameters("TARGET")->Get_Parameter("GRID")->asGrid();

		pModule->Settings_Pop();

		return( pGrid );
	}

	pModule->Settings_Pop();

	Message_Add(CSG_String::Format(SG_T("\n%s: %s\n"), _TL("re-projection"), _TL("failed")), false);

	return( NULL );
}


///////////////////////////////////////////////////////////
//														 //
//														 //
//														 //
///////////////////////////////////////////////////////////

//---------------------------------------------------------

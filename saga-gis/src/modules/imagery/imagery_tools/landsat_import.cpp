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
		"which have each band given as a single GeoTIFF file. "
	));

	//-----------------------------------------------------
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

	Parameters.Add_Value(
		NULL	, "UTM_SOUTH"	, _TL("UTM South"),
		_TL(""),
		PARAMETER_TYPE_Bool, false
	);

	CSG_Parameter	*pNode	= Parameters.Add_Value(
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

		CSG_Grid	*pBand	= Get_Band(Files[i], Parameters("UTM_SOUTH")->asBool());

		if( pBand )
		{
			pBands->Add_Item(pBand);

			pBand->Set_NoData_Value(0);	// landsat 8 pretends to use a value of 65535 (2^16 - 1)

			DataObject_Add(pBand);
			DataObject_Set_Colors(pBand, 11, SG_COLORS_BLACK_WHITE);
		}
	}

	//-----------------------------------------------------
	if( Parameters("SHOW_RGB")->asBool() )
	{
		CSG_Grid	*pR	= pBands->asGrid(Parameters("SHOW_R")->asInt());
		CSG_Grid	*pG	= pBands->asGrid(Parameters("SHOW_G")->asInt());
		CSG_Grid	*pB	= pBands->asGrid(Parameters("SHOW_B")->asInt());

		if( pR && pG && pB )
		{
			DataObject_Set_Parameter(pR, "COLORS_TYPE" , 5);	// _TL("RGB Overlay")	// CLASSIFY_OVERLAY
			DataObject_Set_Parameter(pR, "OVERLAY_MODE", 0);	// _TL("red=this, green=1, blue=2")
			DataObject_Set_Parameter(pR, "OVERLAY_1"   , pG);
			DataObject_Set_Parameter(pR, "OVERLAY_2"   , pB);

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
CSG_Grid * CLandsat_Import::Get_Band(const CSG_String &File, bool bSouth)
{
	CSG_Data_Manager	tmpMgr;

	CSG_Grid	*pBand	= (CSG_Grid *)(tmpMgr.Add(File) && tmpMgr.Get_Grid_System(0) ? tmpMgr.Get_Grid_System(0)->Get(0) : NULL);

	if( !pBand )
	{
		Error_Set(CSG_String::Format(SG_T("%s: %s"), _TL("could not load file"), File.c_str()));

		return( NULL );
	}

	//-----------------------------------------------------
	CSG_String	Projection	= pBand->Get_Projection().Get_Proj4();

	if( Projection.Find("+proj=utm") < 0
	|| (Projection.Find("+south") >= 0 && bSouth == true )
	|| (Projection.Find("+south") <  0 && bSouth == false) )
	{
		tmpMgr.Delete(pBand, true);	// make permanent, detach from temporary data manager

		return( pBand );
	}

	//-----------------------------------------------------
	CSG_Grid	*pCopy	= SG_Create_Grid(pBand->Get_Type(), pBand->Get_NX(), pBand->Get_NY(), pBand->Get_Cellsize(),
		pBand->Get_XMin(), pBand->Get_YMin() + (bSouth ? 10000000 : -10000000)
	);

	if( !pCopy )
	{
		Error_Set(_TL("memory allocation failed"));

		return( NULL );
	}

	if( bSouth )	Projection.Append (" +south");	else	Projection.Replace(" +south", "");

	pCopy->Get_Projection().Create(Projection, SG_PROJ_FMT_Proj4);

	pCopy->Set_Name              (pBand->Get_Name());
	pCopy->Set_Description       (pBand->Get_Description());
	pCopy->Set_NoData_Value_Range(pBand->Get_NoData_Value(), pBand->Get_NoData_hiValue());
	pCopy->Set_ZFactor           (pBand->Get_ZFactor());

	#pragma omp parallel for
	for(int y=0; y<pCopy->Get_NY(); y++)
	{
		for(int x=0; x<pCopy->Get_NX(); x++)
		{
			pCopy->Set_Value(x, y, pBand->asDouble(x, y));
		}
	}

	return( pCopy );
}


///////////////////////////////////////////////////////////
//														 //
//														 //
//														 //
///////////////////////////////////////////////////////////

//---------------------------------------------------------

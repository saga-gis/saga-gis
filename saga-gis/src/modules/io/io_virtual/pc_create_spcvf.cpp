/**********************************************************
 * Version $Id$
 *********************************************************/

///////////////////////////////////////////////////////////
//                                                       //
//                         SAGA                          //
//                                                       //
//      System for Automated Geoscientific Analyses      //
//                                                       //
//                    Module Library:                    //
//                      io_virtual                       //
//                                                       //
//-------------------------------------------------------//
//                                                       //
//                  pc_create_spcvf.cpp                  //
//                                                       //
//                 Copyright (C) 2014 by                 //
//                    Volker Wichmann                    //
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
//    e-mail:     wichmann@laserdata.at                  //
//                                                       //
//    contact:    LASERDATA GmbH                         //
//                Management and Analysis of             //
//                Laserscanning Data                     //
//                Technikerstr. 21a                      //
//                6020 Innsbruck                         //
//                Austria                                //
//                www.laserdata.at                       //
//                                                       //
///////////////////////////////////////////////////////////

//---------------------------------------------------------


///////////////////////////////////////////////////////////
//														 //
//														 //
//														 //
///////////////////////////////////////////////////////////

//---------------------------------------------------------
#include "pc_create_spcvf.h"

#include <vector>
#include <limits>


///////////////////////////////////////////////////////////
//														 //
//														 //
//														 //
///////////////////////////////////////////////////////////

//---------------------------------------------------------
CPointCloud_Create_SPCVF::CPointCloud_Create_SPCVF(void)
{
	Set_Name		(_TL("Create Virtual Point Cloud Dataset"));

	Set_Author		(SG_T("V. Wichmann, LASERDATA GmbH (c) 2014"));

	Set_Description	(_TW(
		"The module allows one to create a virtual point cloud dataset "
		"from a set of SAGA point cloud files. Such a virtual "
		"dataset is a simple XML format with the file extension .spcvf, "
		"which describes a mosaic of individual point cloud files. Such "
		"a virtual point cloud dataset can be used for seamless data "
		"access with the 'Get Subset from Virtual Point Cloud' module.\n"
		"All point cloud input datasets must share the same attribute "
		"table structure, NoData value and projection.\n\n"
	));

	//-----------------------------------------------------
	Parameters.Add_FilePath(
		NULL	, "FILES"		, _TL("Input Files"),
		_TL("The input point cloud files to use"),
		CSG_String::Format(SG_T("%s|%s|%s|%s"),
			_TL("SAGA Point Clouds")	, SG_T("*.spc"),
            _TL("All Files")            , SG_T("*.*")
        ), NULL, false, false, true
	);

	Parameters.Add_FilePath(
		NULL	, "FILENAME"		, _TL("Filename"),
		_TL("The full path and name of the .spcvf file"),
		CSG_String::Format(SG_T("%s|%s|%s|%s"),
			_TL("SAGA Point Cloud Virtual Format (*.spcvf)")	, SG_T("*.spcvf"),
			_TL("All Files")									, SG_T("*.*")
		), NULL, true, false, false
 	);

	Parameters.Add_Choice(
		NULL	, "METHOD_PATHS"	, _TL("File Paths"),
		_TL("Choose how to handle file paths. With relative paths, you can package the *.spcvf and your point cloud tiles easily."),
		CSG_String::Format(SG_T("%s|%s"),
			_TL("absolute"),
			_TL("relative")
		), 1
	);
}


///////////////////////////////////////////////////////////
//														 //
//														 //
//														 //
///////////////////////////////////////////////////////////

//---------------------------------------------------------
bool CPointCloud_Create_SPCVF::On_Execute(void)
{
	CSG_Strings					sFiles;
	CSG_String					sFileName;
	int							iMethodPaths;

	CSG_MetaData				SPCVF;
	CSG_Projection				projSPCVF;
	double						dNoData;
	std::vector<TSG_Data_Type>	vFieldTypes;
	std::vector<CSG_String>		vFieldNames;
	double						dBBoxXMin = std::numeric_limits<int>::max();
	double						dBBoxYMin = std::numeric_limits<int>::max();
	double						dBBoxXMax = std::numeric_limits<int>::min();
	double						dBBoxYMax = std::numeric_limits<int>::min();
	int							iSkipped = 0, iEmpty = 0;
	int							iDatasetCount = 0;
	double						dPointCount = 0.0;
	double						dZMin = std::numeric_limits<double>::max();
	double						dZMax = std::numeric_limits<double>::min();

	//-----------------------------------------------------
	sFileName		= Parameters("FILENAME")->asString();
	iMethodPaths	= Parameters("METHOD_PATHS")->asInt();

	//-----------------------------------------------------
    if( !Parameters("FILES")->asFilePath()->Get_FilePaths(sFiles) )
	{
		SG_UI_Msg_Add_Error(_TL("Please provide some input files!"));
		return( false );
	}

	//-----------------------------------------------------
	SPCVF.Set_Name(SG_T("SPCVFDataset"));
	SPCVF.Add_Property(SG_T("Version"), SG_T("1.1"));
	
	switch( iMethodPaths )
	{
	default:
	case 0:		SPCVF.Add_Property(SG_T("Paths"), SG_T("absolute"));	break;
	case 1:		SPCVF.Add_Property(SG_T("Paths"), SG_T("relative"));	break;
	}

	//-----------------------------------------------------
	CSG_MetaData	*pSPCVFBBox		= SPCVF.Add_Child(SG_T("BBox"));

	CSG_MetaData	*pSPCVFNoData	= SPCVF.Add_Child(SG_T("NoData"));

	CSG_MetaData	*pSRS			= SPCVF.Add_Child(SG_T("SRS"));

	CSG_MetaData	*pSPCVFAttr		= SPCVF.Add_Child(SG_T("Attributes"));

	CSG_MetaData	*pSPCVFMeta		= SPCVF.Add_Child(SG_T("Metadata"));

	//-----------------------------------------------------
	CSG_MetaData	*pSPCVFDatasets;
	

	//-----------------------------------------------------
	for(int i=0; i<sFiles.Get_Count() && Set_Progress(i, sFiles.Get_Count()); i++)
	{
		CSG_PointCloud	*pPC		= SG_Create_PointCloud(sFiles[i]);

		//-----------------------------------------------------
		if( i==0 )		// first dataset determines projection, NoData value and table structure
		{
			projSPCVF	= pPC->Get_Projection();
			dNoData		= pPC->Get_NoData_Value();

			pSPCVFNoData->Add_Property(SG_T("Value"), dNoData);

			pSPCVFAttr->Add_Property(SG_T("Count"), pPC->Get_Field_Count());

			for(int iField=0; iField<pPC->Get_Field_Count(); iField++)
			{
				vFieldTypes.push_back(pPC->Get_Field_Type(iField));
				vFieldNames.push_back(pPC->Get_Field_Name(iField));

				CSG_MetaData	*pSPCVFField = pSPCVFAttr->Add_Child(CSG_String::Format(SG_T("Field_%d"), iField + 1));

				pSPCVFField->Add_Property(SG_T("Name"), pPC->Get_Field_Name(iField));
				pSPCVFField->Add_Property(SG_T("Type"), gSG_Data_Type_Identifier[pPC->Get_Field_Type(iField)]);

			}

			if( projSPCVF.is_Okay() )
			{
				pSRS->Add_Property(SG_T("Projection"), projSPCVF.Get_Name());
				pSRS->Add_Property(SG_T("WKT"), projSPCVF.Get_WKT());
			}
			else
			{
				pSRS->Add_Property(SG_T("Projection"), SG_T("Undefined Coordinate System"));
			}

			pSPCVFDatasets	= SPCVF.Add_Child(SG_T("Datasets"));
		}
		else		// validate projection, NoData value and table structure
		{
			bool	bSkip = false;

			if( pPC->Get_Field_Count() != (int)vFieldTypes.size() )
			{
				bSkip = true;
			}

			if( !bSkip && projSPCVF.is_Okay() )
			{
				if ( !pPC->Get_Projection().is_Okay() || SG_STR_CMP(pPC->Get_Projection().Get_WKT(), projSPCVF.Get_WKT()) )
				{
					bSkip = true;
				}
			}

			if( !bSkip )
			{
				for(int iField=0; iField<pPC->Get_Field_Count(); iField++)
				{
					if( pPC->Get_Field_Type(iField) != vFieldTypes.at(iField) )
					{
						bSkip = true;
						break;
					}

					if( SG_STR_CMP(pPC->Get_Field_Name(iField), vFieldNames.at(iField)) )
					{
						bSkip = true;
						break;
					}
				}
			}

			if( bSkip )
			{
				SG_UI_Msg_Add(CSG_String::Format(_TL("Skipping dataset %s because of incompatibility with the first input dataset!"), sFiles[i].c_str()), true);
				delete( pPC );
				iSkipped++;
				continue;
			}
		}

		//-----------------------------------------------------
		if( pPC->Get_Point_Count() <= 0 )
		{
			delete( pPC );
			iEmpty++;
			continue;
		}

		//-----------------------------------------------------
		CSG_MetaData	*pDataset	= pSPCVFDatasets->Add_Child(SG_T("PointCloud"));

		CSG_String		sFilePath;

		switch( iMethodPaths )
		{
		default:
		case 0:		sFilePath = SG_File_Get_Path_Absolute(sFiles.Get_String(i));									break;
		case 1:		sFilePath = SG_File_Get_Path_Relative(SG_File_Get_Path(sFileName), sFiles.Get_String(i));		break;
		}

		sFilePath.Replace(SG_T("\\"), SG_T("/"));

		pDataset->Add_Property(SG_T("File"), sFilePath);

		pDataset->Add_Property(SG_T("Points"), pPC->Get_Point_Count());

		pDataset->Add_Property(SG_T("ZMin"), pPC->Get_ZMin());
		pDataset->Add_Property(SG_T("ZMax"), pPC->Get_ZMax());

		//-----------------------------------------------------
		CSG_MetaData	*pBBox		= pDataset->Add_Child(SG_T("BBox"));

		pBBox->Add_Property(SG_T("XMin"), pPC->Get_Extent().Get_XMin());
		pBBox->Add_Property(SG_T("YMin"), pPC->Get_Extent().Get_YMin());
		pBBox->Add_Property(SG_T("XMax"), pPC->Get_Extent().Get_XMax());
		pBBox->Add_Property(SG_T("YMax"), pPC->Get_Extent().Get_YMax());

		if( dBBoxXMin > pPC->Get_Extent().Get_XMin() )
			dBBoxXMin = pPC->Get_Extent().Get_XMin();
		if( dBBoxYMin > pPC->Get_Extent().Get_YMin() )
			dBBoxYMin = pPC->Get_Extent().Get_YMin();
		if( dBBoxXMax < pPC->Get_Extent().Get_XMax() )
			dBBoxXMax = pPC->Get_Extent().Get_XMax();
		if( dBBoxYMax < pPC->Get_Extent().Get_YMax() )
			dBBoxYMax = pPC->Get_Extent().Get_YMax();
		
		iDatasetCount	+= 1;
		dPointCount		+= pPC->Get_Point_Count();

		if( dZMin > pPC->Get_ZMin() )
			dZMin = pPC->Get_ZMin();
		if( dZMax < pPC->Get_ZMax() )
			dZMax = pPC->Get_ZMax();


		delete( pPC );
	}

	//-----------------------------------------------------
	pSPCVFBBox->Add_Property(SG_T("XMin"), dBBoxXMin);
	pSPCVFBBox->Add_Property(SG_T("YMin"), dBBoxYMin);
	pSPCVFBBox->Add_Property(SG_T("XMax"), dBBoxXMax);
	pSPCVFBBox->Add_Property(SG_T("YMax"), dBBoxYMax);

	pSPCVFMeta->Add_Property(SG_T("Datasets"), iDatasetCount);
	pSPCVFMeta->Add_Property(SG_T("Points"), CSG_String::Format(SG_T("%.0f"), dPointCount));
	pSPCVFMeta->Add_Property(SG_T("ZMin"), dZMin);
	pSPCVFMeta->Add_Property(SG_T("ZMax"), dZMax);

	//-----------------------------------------------------
	if( !SPCVF.Save(sFileName) )
	{
		SG_UI_Msg_Add_Error(CSG_String::Format(_TL("Unable to save %s file!"), sFileName.c_str()));

		return( false );
	}

	//-----------------------------------------------------
	if( iSkipped > 0 )
	{
		SG_UI_Msg_Add(CSG_String::Format(_TL("WARNING: %d dataset(s) skipped because of incompatibilities!"), iSkipped), true);
	}

	if( iEmpty > 0 )
	{
		SG_UI_Msg_Add(CSG_String::Format(_TL("WARNING: %d dataset(s) skipped because they are empty!"), iEmpty), true);
	}

	SG_UI_Msg_Add(CSG_String::Format(_TL("SPCVF successfully created from %d dataset(s)."), iDatasetCount), true);

	//-----------------------------------------------------
	return( true );
}


///////////////////////////////////////////////////////////
//														 //
//														 //
//														 //
///////////////////////////////////////////////////////////

//---------------------------------------------------------

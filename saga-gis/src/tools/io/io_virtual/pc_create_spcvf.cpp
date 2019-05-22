/**********************************************************
 * Version $Id$
 *********************************************************/

///////////////////////////////////////////////////////////
//                                                       //
//                         SAGA                          //
//                                                       //
//      System for Automated Geoscientific Analyses      //
//                                                       //
//                     Tool Library                      //
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

	Set_Author		("V. Wichmann, LASERDATA GmbH (c) 2014-9");

	Set_Description	(_TW(
		"The tool allows one to create a virtual point cloud dataset "
		"from a set of SAGA point cloud files. For a large number of "
		"files, it is advised to use an input file list, i.e. a text "
		"file with the full path to an input point cloud on each line. "
		"If possible, you should make use of the point cloud headers files to "
		"construct the virtual dataset. This avoids that each dataset "
		"has to be loaded and thus reduces execution time enormously.\n"
		"A virtual point cloud "
		"dataset is a simple XML format with the file extension .spcvf, "
		"which describes a mosaic of individual point cloud files. Such "
		"a virtual point cloud dataset can be used for seamless data "
		"access with the 'Get Subset from Virtual Point Cloud' tool.\n"
		"All point cloud input datasets must share the same attribute "
		"table structure, NoData value and projection.\n\n"
	));

	//-----------------------------------------------------
	Parameters.Add_FilePath(
		"", "FILES"				, _TL("Input Files"),
		_TL("The input point cloud files to use"),
		CSG_String::Format("%s|*.spc;*.sg-pts;*.sg-pts-z|%s|*.*",
			_TL("SAGA Point Clouds"),
            _TL("All Files")
        ), NULL, false, false, true
	);

	Parameters.Add_FilePath(
		"", "INPUT_FILE_LIST"	, _TL("Input File List"),
		_TL("A text file with the full path to an input point cloud on each line"),
		CSG_String::Format("%s|*.txt|%s|*.*",
			_TL("Text Files"),
            _TL("All Files" )
        ), NULL, false, false, false
	);

	Parameters.Add_FilePath(
		"", "FILENAME"			, _TL("Filename"),
		_TL("The full path and name of the .spcvf file"),
		CSG_String::Format("%s|*.spcvf|%s|*.*",
			_TL("SAGA Point Cloud Virtual Format (*.spcvf)"),
			_TL("All Files")
		), NULL, true, false, false
 	);

	Parameters.Add_Choice(
		NULL	, "METHOD_PATHS"	, _TL("File Paths"),
		_TL("Choose how to handle file paths. With relative paths, you can package the *.spcvf and your point cloud tiles easily."),
		CSG_String::Format("%s|%s",
			_TL("absolute"),
			_TL("relative")
		), 1
	);

	Parameters.Add_Value(
		NULL	, "USE_HEADER"	, _TL("Use Header File"),
		_TL("Check this parameter to use (only) the point cloud header file to construct the virtual dataset."),
		PARAMETER_TYPE_Bool, false
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
	CSG_String					sFileInputList, sFileName;
	int							iMethodPaths;
	bool						bHeader;

	CSG_MetaData				SPCVF;
	CSG_Projection				projSPCVF;
	double						dNoData;
	double						dBBoxXMin = std::numeric_limits<int>::max();
	double						dBBoxYMin = std::numeric_limits<int>::max();
	double						dBBoxXMax = std::numeric_limits<int>::min();
	double						dBBoxYMax = std::numeric_limits<int>::min();
	int							iSkipped = 0, iEmpty = 0, iNoHeader = 0;
	int							iDatasetCount = 0;
	double						dPointCount = 0.0;
	double						dZMin = std::numeric_limits<double>::max();
	double						dZMax = -std::numeric_limits<double>::max();

	//-----------------------------------------------------
	sFileName		= Parameters("FILENAME")->asString();
	iMethodPaths	= Parameters("METHOD_PATHS")->asInt();
	sFileInputList	= Parameters("INPUT_FILE_LIST")->asString();
	bHeader			= Parameters("USE_HEADER")->asBool();

	//-----------------------------------------------------
	if( !Parameters("FILES")->asFilePath()->Get_FilePaths(sFiles) && sFileInputList.Length() <= 0 )
	{
		SG_UI_Msg_Add_Error(_TL("Please provide some input files!"));
		return( false );
	}

	//-----------------------------------------------------
	if( sFiles.Get_Count() <= 0 )
	{
		CSG_Table	*pTable = new CSG_Table();

		if( !pTable->Create(sFileInputList, TABLE_FILETYPE_Text_NoHeadLine) )
		{
			SG_UI_Msg_Add_Error(_TL("Input file list could not be opened!"));
			delete( pTable );
			return( false );
		}

		sFiles.Clear();

		for (int i=0; i<pTable->Get_Record_Count(); i++)
		{
			sFiles.Add(pTable->Get_Record(i)->asString(0));
		}

		delete( pTable );
	}


	//-----------------------------------------------------
	SPCVF.Set_Name("SPCVFDataset");
	SPCVF.Add_Property("Version", "1.1");
	
	switch( iMethodPaths )
	{
	default:
	case 0:		SPCVF.Add_Property("Paths", "absolute");	break;
	case 1:		SPCVF.Add_Property("Paths", "relative");	break;
	}

	//-----------------------------------------------------
	CSG_MetaData	*pSPCVFHeader	= SPCVF.Add_Child(			"Header");

	CSG_MetaData	*pSPCVFFiles	= pSPCVFHeader->Add_Child(	"Datasets");
	CSG_MetaData	*pSPCVFPoints	= pSPCVFHeader->Add_Child(	"Points");
	CSG_MetaData	*pSRS			= pSPCVFHeader->Add_Child(	"SRS");
	CSG_MetaData	*pSPCVFBBox		= pSPCVFHeader->Add_Child(	"BBox");
	CSG_MetaData	*pSPCVFZStats	= pSPCVFHeader->Add_Child(	"ZStats");
	CSG_MetaData	*pSPCVFNoData	= pSPCVFHeader->Add_Child(	"NoData");
	CSG_MetaData	*pSPCVFAttr		= pSPCVFHeader->Add_Child(	"Attributes");

	CSG_MetaData	*pSPCVFDatasets	= NULL;
	

	//-----------------------------------------------------
	if( bHeader )
	{
		std::vector<CSG_String>		vFieldTypes, vFieldNames;

		//-----------------------------------------------------
		for(int i=0; i<sFiles.Get_Count() && Set_Progress(i, sFiles.Get_Count()); i++)
		{
			CSG_MetaData	Header;
			bool			bResult = CSG_PointCloud::Get_Header_Content(sFiles[i], Header);

			if( !bResult )
			{
				iNoHeader++;
				continue;
			}

			//-----------------------------------------------------
			if( i==0 )		// first dataset determines projection, NoData value and table structure
			{
				projSPCVF	= _Get_Projection(sFiles[i]);
				
				if( projSPCVF.is_Okay() )
				{
					pSRS->Add_Property("Projection", projSPCVF.Get_Name());
					pSRS->Add_Property("WKT", projSPCVF.Get_WKT());
				}
				else
				{
					pSRS->Add_Property("Projection", "Undefined Coordinate System");
				}

				Header.Get_Child("NoData")->Get_Property("Value", dNoData);
				pSPCVFNoData->Add_Property("Value", dNoData);

				int	iFieldCount;
				Header.Get_Child("Attributes")->Get_Property("Count", iFieldCount);
				pSPCVFAttr->Add_Property("Count", iFieldCount);

				for(int iField=0; iField<iFieldCount; iField++)
				{
					CSG_String		Name, Type;
					Header.Get_Child("Attributes")->Get_Child(CSG_String::Format("Field_%d", iField + 1))->Get_Property("Name", Name);
					Header.Get_Child("Attributes")->Get_Child(CSG_String::Format("Field_%d", iField + 1))->Get_Property("Type", Type);

					vFieldTypes.push_back(Type);
					vFieldNames.push_back(Name);

					CSG_MetaData	*pSPCVFField = pSPCVFAttr->Add_Child(CSG_String::Format("Field_%d", iField + 1));
					pSPCVFField->Add_Property("Name", Name);
					pSPCVFField->Add_Property("Type", Type);
				}

				pSPCVFDatasets	= SPCVF.Add_Child("Datasets");
			}
			else		// validate projection, NoData value and table structure
			{
				bool	bSkip = false;

				int	iFieldCount;
				Header.Get_Child("Attributes")->Get_Property("Count", iFieldCount);

				if( iFieldCount != (int)vFieldTypes.size() )
				{
					bSkip = true;
				}

				if( !bSkip && projSPCVF.is_Okay() )
				{
					CSG_Projection Projection = _Get_Projection(sFiles[i]);

					if ( !Projection.is_Okay() || SG_STR_CMP(Projection.Get_WKT(), projSPCVF.Get_WKT()) )
					{
						bSkip = true;
					}
				}

				if( !bSkip )
				{
					for(int iField=0; iField<iFieldCount; iField++)
					{
						CSG_String		Name, Type;
						Header.Get_Child("Attributes")->Get_Child(CSG_String::Format("Field_%d", iField + 1))->Get_Property("Name", Name);
						Header.Get_Child("Attributes")->Get_Child(CSG_String::Format("Field_%d", iField + 1))->Get_Property("Type", Type);

						if( SG_STR_CMP(Type, vFieldTypes.at(iField)) )
						{
							bSkip = true;
							break;
						}

						if( SG_STR_CMP(Name, vFieldNames.at(iField)) )
						{
							bSkip = true;
							break;
						}
					}
				}

				if( bSkip )
				{
					SG_UI_Msg_Add(CSG_String::Format(_TL("Skipping dataset %s because of incompatibility with the first input dataset!"), sFiles[i].c_str()), true);
					iSkipped++;
					continue;
				}
			}

			//-----------------------------------------------------
			int		iPoints;
			Header.Get_Child("Points")->Get_Property("Value", iPoints);
			
			if( iPoints <= 0 )
			{
				iEmpty++;
				continue;
			}

			//-----------------------------------------------------
			CSG_MetaData	*pDataset	= pSPCVFDatasets->Add_Child("PointCloud");

			CSG_String		sFilePath;

			switch( iMethodPaths )
			{
			default:
			case 0:		sFilePath = SG_File_Get_Path_Absolute(sFiles.Get_String(i));									break;
			case 1:		sFilePath = SG_File_Get_Path_Relative(SG_File_Get_Path(sFileName), sFiles.Get_String(i));		break;
			}

			sFilePath.Replace("\\", "/");

			pDataset->Add_Property("File", sFilePath);

			pDataset->Add_Property("Points", iPoints);

			//-----------------------------------------------------
			double	dXMin, dYMin, dZMinPC, dXMax, dYMax, dZMaxPC;

			Header.Get_Child("BBox")->Get_Property("XMin", dXMin);
			Header.Get_Child("BBox")->Get_Property("YMin", dYMin);
			Header.Get_Child("BBox")->Get_Property("ZMin", dZMinPC);
			Header.Get_Child("BBox")->Get_Property("XMax", dXMax);
			Header.Get_Child("BBox")->Get_Property("YMax", dYMax);
			Header.Get_Child("BBox")->Get_Property("ZMax", dZMaxPC);

			pDataset->Add_Property("ZMin", dZMinPC);
			pDataset->Add_Property("ZMax", dZMaxPC);

			//-----------------------------------------------------
			CSG_MetaData	*pBBox		= pDataset->Add_Child("BBox");

			pBBox->Add_Property("XMin", dXMin);
			pBBox->Add_Property("YMin", dYMin);
			pBBox->Add_Property("XMax", dXMax);
			pBBox->Add_Property("YMax", dYMax);

			if( dBBoxXMin > dXMin )
				dBBoxXMin = dXMin;
			if( dBBoxYMin > dYMin )
				dBBoxYMin = dYMin;
			if( dBBoxXMax < dXMax )
				dBBoxXMax = dXMax;
			if( dBBoxYMax < dYMax )
				dBBoxYMax = dYMax;
		
			iDatasetCount	+= 1;
			dPointCount		+= iPoints;

			if( dZMin > dZMinPC )
				dZMin = dZMinPC;
			if( dZMax < dZMaxPC )
				dZMax = dZMaxPC;
		}
	}
	else
	{
		std::vector<TSG_Data_Type>	vFieldTypes;
		std::vector<CSG_String>		vFieldNames;

		//-----------------------------------------------------
		for(int i=0; i<sFiles.Get_Count() && Set_Progress(i, sFiles.Get_Count()); i++)
		{
			CSG_PointCloud	*pPC		= SG_Create_PointCloud(sFiles[i]);

			//-----------------------------------------------------
			if( i==0 )		// first dataset determines projection, NoData value and table structure
			{
				projSPCVF	= pPC->Get_Projection();
				dNoData		= pPC->Get_NoData_Value();

				pSPCVFNoData->Add_Property("Value", dNoData);

				pSPCVFAttr->Add_Property("Count", pPC->Get_Field_Count());

				for(int iField=0; iField<pPC->Get_Field_Count(); iField++)
				{
					vFieldTypes.push_back(pPC->Get_Field_Type(iField));
					vFieldNames.push_back(pPC->Get_Field_Name(iField));

					CSG_MetaData	*pSPCVFField = pSPCVFAttr->Add_Child(CSG_String::Format("Field_%d", iField + 1));

					pSPCVFField->Add_Property("Name", pPC->Get_Field_Name(iField));
					pSPCVFField->Add_Property("Type", gSG_Data_Type_Identifier[pPC->Get_Field_Type(iField)]);

				}

				if( projSPCVF.is_Okay() )
				{
					pSRS->Add_Property("Projection", projSPCVF.Get_Name());
					pSRS->Add_Property("WKT", projSPCVF.Get_WKT());
				}
				else
				{
					pSRS->Add_Property("Projection", "Undefined Coordinate System");
				}

				pSPCVFDatasets	= SPCVF.Add_Child("Datasets");
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
			CSG_MetaData	*pDataset	= pSPCVFDatasets->Add_Child("PointCloud");

			CSG_String		sFilePath;

			switch( iMethodPaths )
			{
			default:
			case 0:		sFilePath = SG_File_Get_Path_Absolute(sFiles.Get_String(i));									break;
			case 1:		sFilePath = SG_File_Get_Path_Relative(SG_File_Get_Path(sFileName), sFiles.Get_String(i));		break;
			}

			sFilePath.Replace("\\", "/");

			pDataset->Add_Property("File", sFilePath);

			pDataset->Add_Property("Points", pPC->Get_Point_Count());

			pDataset->Add_Property("ZMin", pPC->Get_ZMin());
			pDataset->Add_Property("ZMax", pPC->Get_ZMax());

			//-----------------------------------------------------
			CSG_MetaData	*pBBox		= pDataset->Add_Child("BBox");

			pBBox->Add_Property("XMin", pPC->Get_Extent().Get_XMin());
			pBBox->Add_Property("YMin", pPC->Get_Extent().Get_YMin());
			pBBox->Add_Property("XMax", pPC->Get_Extent().Get_XMax());
			pBBox->Add_Property("YMax", pPC->Get_Extent().Get_YMax());

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
	}
	
	//-----------------------------------------------------
	pSPCVFBBox->Add_Property("XMin", dBBoxXMin);
	pSPCVFBBox->Add_Property("YMin", dBBoxYMin);
	pSPCVFBBox->Add_Property("XMax", dBBoxXMax);
	pSPCVFBBox->Add_Property("YMax", dBBoxYMax);

	pSPCVFFiles->Add_Property("Count", iDatasetCount);
	pSPCVFPoints->Add_Property("Count", CSG_String::Format("%.0f", dPointCount));
	pSPCVFZStats->Add_Property("ZMin", dZMin);
	pSPCVFZStats->Add_Property("ZMax", dZMax);

	//-----------------------------------------------------
	if( !SPCVF.Save(sFileName) )
	{
		SG_UI_Msg_Add_Error(CSG_String::Format(_TL("Unable to save %s file!"), sFileName.c_str()));

		return( false );
	}

	//-----------------------------------------------------
	if( bHeader )
	{
		if( iNoHeader > 0 )
		{
			SG_UI_Msg_Add(CSG_String::Format(_TL("WARNING: %d dataset(s) skipped because corresponding header file was not found!"), iNoHeader), true);
		}
	}
	
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

//---------------------------------------------------------
CSG_Projection CPointCloud_Create_SPCVF::_Get_Projection(const CSG_String &FileName)
{
	CSG_Projection	Projection;

	if( SG_File_Cmp_Extension(FileName, "sg-pts-z") ) // POINTCLOUD_FILE_FORMAT_Compressed
	{
		CSG_File_Zip	Stream(FileName, SG_FILE_R);

		CSG_String	_FileName(SG_File_Get_Name(FileName, false) + ".");

		if( Stream.Get_File(_FileName + "sg-prj") )
		{
			Projection.Load(Stream, SG_PROJ_FMT_WKT);
		}
	}
	else // if( SG_File_Cmp_Extension(FileName, "sg-pts"/"spc") ) // POINTCLOUD_FILE_FORMAT_Normal
	{
		Projection.Load(SG_File_Make_Path("", FileName, "sg-prj"), SG_PROJ_FMT_WKT);
	}

	return( Projection );
}


///////////////////////////////////////////////////////////
//														 //
//														 //
//														 //
///////////////////////////////////////////////////////////

//---------------------------------------------------------

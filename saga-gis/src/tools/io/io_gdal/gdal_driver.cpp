/**********************************************************
 * Version $Id: gdal_driver.cpp 1921 2014-01-09 10:24:11Z oconrad $
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
//                   gdal_driver.cpp                     //
//                                                       //
//            Copyright (C) 2007 by O. Conrad            //
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
#include "gdal_driver.h"

#include <gdal_priv.h>

#include <cpl_string.h>
#include <cpl_error.h>

#include <vrtdataset.h>


///////////////////////////////////////////////////////////
//														 //
//														 //
//														 //
///////////////////////////////////////////////////////////

//---------------------------------------------------------
CSG_GDAL_Drivers	gSG_GDAL_Drivers;

const CSG_GDAL_Drivers &	SG_Get_GDAL_Drivers	(void)
{
	return( gSG_GDAL_Drivers );
}


///////////////////////////////////////////////////////////
//														 //
//														 //
//														 //
///////////////////////////////////////////////////////////

//---------------------------------------------------------
CSG_GDAL_Drivers::CSG_GDAL_Drivers(void)
{
	GDALAllRegister();

	// affects Windows only, might be appropriate for applications
	// that treat filenames as being in the local encoding.
	// for more info see: http://trac.osgeo.org/gdal/wiki/ConfigOptions
	CPLSetConfigOption("GDAL_FILENAME_IS_UTF8", "NO");

	m_pDrivers	= GetGDALDriverManager();
}

//---------------------------------------------------------
CSG_GDAL_Drivers::~CSG_GDAL_Drivers(void)
{
	GDALDestroyDriverManager();	
}


///////////////////////////////////////////////////////////
//														 //
///////////////////////////////////////////////////////////

//---------------------------------------------------------
CSG_String CSG_GDAL_Drivers::Get_Version(void) const
{
	return( GDALVersionInfo("RELEASE_NAME") );
}

//---------------------------------------------------------
int CSG_GDAL_Drivers::Get_Count(void) const
{
	return( m_pDrivers->GetDriverCount() );
}

//---------------------------------------------------------
GDALDriver * CSG_GDAL_Drivers::Get_Driver(const CSG_String &Name) const
{
	return( (GDALDriver *)GDALGetDriverByName(Name) );
}

//---------------------------------------------------------
GDALDriver * CSG_GDAL_Drivers::Get_Driver(int Index) const
{
	return( (GDALDriver *)GDALGetDriver(Index) );
}

//---------------------------------------------------------
CSG_String CSG_GDAL_Drivers::Get_Name(int Index) const
{
	return( Get_Driver(Index)->GetMetadataItem(GDAL_DMD_LONGNAME) );
}

//---------------------------------------------------------
CSG_String CSG_GDAL_Drivers::Get_Description(int Index) const
{
	return( Get_Driver(Index)->GetDescription() );
}

//---------------------------------------------------------
CSG_String CSG_GDAL_Drivers::Get_Extension(int Index) const
{
	return( Get_Driver(Index)->GetMetadataItem(GDAL_DMD_EXTENSION) );
}

//---------------------------------------------------------
bool CSG_GDAL_Drivers::is_Raster(int Index) const
{
#ifdef USE_GDAL_V2
	return( Get_Driver(Index) && CSLFetchBoolean(Get_Driver(Index)->GetMetadata(), GDAL_DCAP_RASTER, false) );
#else
	return( true );
#endif
}

//---------------------------------------------------------
bool CSG_GDAL_Drivers::Can_Read(int Index) const
{
	return( Get_Driver(Index) != NULL );	// ? CSLFetchBoolean(Get_Driver(Index)->GetMetadata(), GDAL_DCAP_CREATE, false) != 0 : false );
}

//---------------------------------------------------------
bool CSG_GDAL_Drivers::Can_Write(int Index) const
{
	return( Get_Driver(Index) && CSLFetchBoolean(Get_Driver(Index)->GetMetadata(), GDAL_DCAP_CREATE, false) );
}


///////////////////////////////////////////////////////////
//														 //
///////////////////////////////////////////////////////////

//---------------------------------------------------------
int CSG_GDAL_Drivers::Get_GDAL_Type(TSG_Data_Type Type)
{
	switch( Type )
	{
	case SG_DATATYPE_Bit: 		return( GDT_Byte );			// Eight bit unsigned integer
	case SG_DATATYPE_Byte: 		return( GDT_Byte );			// Eight bit unsigned integer
	case SG_DATATYPE_Char: 		return( GDT_Byte );			// Eight bit unsigned integer
	case SG_DATATYPE_Word:		return( GDT_UInt16 );		// Sixteen bit unsigned integer
	case SG_DATATYPE_Short:		return( GDT_Int16 );		// Sixteen bit signed integer
	case SG_DATATYPE_DWord:		return( GDT_UInt32 );		// Thirty two bit unsigned integer
	case SG_DATATYPE_Int: 		return( GDT_Int32 );		// Thirty two bit signed integer
	case SG_DATATYPE_Float: 	return( GDT_Float32 );		// Thirty two bit floating point
	case SG_DATATYPE_Double: 	return( GDT_Float64 );		// Sixty four bit floating point

	default:					return( GDT_Float64 );
	}
}

//---------------------------------------------------------
TSG_Data_Type CSG_GDAL_Drivers::Get_SAGA_Type(int Type)
{
	switch( Type )
	{
	case GDT_Byte: 		return( SG_DATATYPE_Byte );			// Eight bit unsigned integer
	case GDT_UInt16:	return( SG_DATATYPE_Word );			// Sixteen bit unsigned integer
	case GDT_Int16:		return( SG_DATATYPE_Short );		// Sixteen bit signed integer
	case GDT_UInt32:	return( SG_DATATYPE_DWord );		// Thirty two bit unsigned integer
	case GDT_Int32: 	return( SG_DATATYPE_Int );			// Thirty two bit signed integer
	case GDT_Float32: 	return( SG_DATATYPE_Float );		// Thirty two bit floating point
	case GDT_Float64: 	return( SG_DATATYPE_Double );		// Sixty four bit floating point

	case GDT_CInt16: 	return( SG_DATATYPE_Undefined );	// Complex Int16
	case GDT_CInt32: 	return( SG_DATATYPE_Undefined );	// Complex Int32
	case GDT_CFloat32: 	return( SG_DATATYPE_Undefined );	// Complex Float32
	case GDT_CFloat64: 	return( SG_DATATYPE_Undefined );	// Complex Float64

	default:			return( SG_DATATYPE_Undefined );
	}
}


///////////////////////////////////////////////////////////
//														 //
//														 //
//														 //
///////////////////////////////////////////////////////////

//---------------------------------------------------------
CSG_GDAL_DataSet::CSG_GDAL_DataSet(void)
{
	m_pDataSet	= m_pVrtSource	= NULL;

	m_TF_A.Create(2);
	m_TF_B.Create(2, 2);
}

//---------------------------------------------------------
CSG_GDAL_DataSet::CSG_GDAL_DataSet(const CSG_String &File_Name)
{
	m_pDataSet	= m_pVrtSource	= NULL;

	m_TF_A.Create(2);
	m_TF_B.Create(2, 2);

	Open_Read(File_Name);
}

//---------------------------------------------------------
CSG_GDAL_DataSet::~CSG_GDAL_DataSet(void)
{
	Close();
}


///////////////////////////////////////////////////////////
//														 //
///////////////////////////////////////////////////////////

//---------------------------------------------------------
bool CSG_GDAL_DataSet::Open_Read(const CSG_String &File_Name)
{
	Close();

	if( (m_pDataSet = (GDALDataset *)GDALOpen(File_Name, GA_ReadOnly)) == NULL )
	{
		return( false );
	}

	//-----------------------------------------------------
	m_File_Name	= File_Name;

	m_Access	= SG_GDAL_IO_READ;

	return( _Set_Transformation() );
}

//---------------------------------------------------------
bool CSG_GDAL_DataSet::Open_Read(const CSG_String &File_Name, const CSG_Grid_System &System)
{
	Close();

	if( (m_pVrtSource = (GDALDataset *)GDALOpen(File_Name, GA_ReadOnly)) == NULL )
	{
		return( false );
	}

	//-----------------------------------------------------
	if( (m_pDataSet = (VRTDataset *)VRTCreate(System.Get_NX(), System.Get_NY())) == NULL )
	{
		Close();

		return( false );
	}

	m_pDataSet->SetProjection(m_pVrtSource->GetProjectionRef());

	double	Transform[6]	=
	{
		System.Get_XMin(true), System.Get_Cellsize(), 0.0,
		System.Get_YMax(true), 0.0, -System.Get_Cellsize()
	};

	m_pDataSet->SetGeoTransform(Transform);

	//-----------------------------------------------------
	m_pVrtSource->GetGeoTransform(Transform);

	if( Transform[2] != 0.0 || Transform[4] != 0.0 )
	{
		return( false );	// geotransform is rotated, this configuration is not supported...
	}

	int	xOff	= (int)floor((System.Get_XMin  (true) - Transform[0]) /      Transform[1]  + 0.001);
	int	yOff	= (int)floor((System.Get_YMax  (true) - Transform[3]) /      Transform[5]  + 0.001);
	int	xSize	= (int)     ( System.Get_XRange(true)                 /      Transform[1]  + 0.5  );
	int	ySize	= (int)     ( System.Get_YRange(true)                 / fabs(Transform[5]) + 0.5  );

	//-----------------------------------------------------
	for(int i=0; i<m_pVrtSource->GetRasterCount(); i++)
	{
		GDALRasterBand	*pSrcBand	= m_pVrtSource->GetRasterBand(i + 1);

		m_pDataSet->AddBand(pSrcBand->GetRasterDataType(), NULL);

		VRTSourcedRasterBand	*pVrtBand	= (VRTSourcedRasterBand *)m_pDataSet->GetRasterBand(i + 1);

		pVrtBand->AddSimpleSource(pSrcBand,
			xOff, yOff, xSize, ySize, 0, 0, System.Get_NX(), System.Get_NY()
		);

//#if GDAL_VERSION_MAJOR >= 2	// instead of pVrtBand->AddSimpleSource(...)
//		VRTSimpleSource	*pSrcSimple = new VRTSimpleSource();
//
//	//	pSrcSimple->SetResampling(pszResampling);
//
//		pVrtBand->ConfigureSource(pSrcSimple, pSrcBand, 0,
//			xOff, yOff, xSize, ySize, 0, 0, System.Get_NX(), System.Get_NY()
//		);
//
//		pVrtBand->AddSource(pSrcSimple);
//#endif
	}

	//-----------------------------------------------------
	m_File_Name	= File_Name;

	m_Access	= SG_GDAL_IO_READ;

	return( _Set_Transformation() );
}

//---------------------------------------------------------
bool CSG_GDAL_DataSet::_Set_Transformation(void)
{
	if( !m_pDataSet )
	{
		return( false );
	}

	double	Transform[6];

	m_NX	= m_pDataSet->GetRasterXSize();
	m_NY	= m_pDataSet->GetRasterYSize();

	if( m_pDataSet->GetGeoTransform(Transform) != CE_None )
	{
		m_bTransform	= false;
		m_Cellsize		= 1.0;
		m_xMin			= 0.5;
		m_yMin			= 0.5;
	}
	else if( Transform[1] == -Transform[5] && Transform[2] == 0.0 && Transform[4] == 0.0 )	// nothing to transform
	{
		m_bTransform	= false;
		m_Cellsize		= Transform[1];								// pixel width (== pixel height)
		m_xMin			= Transform[0] + m_Cellsize *  0.5;			// center (x) of left edge pixels
		m_yMin			= Transform[3] + m_Cellsize * (0.5 - m_NY);	// center (y) of lower edge pixels
	}
	else
	{
		m_bTransform	= true;
		m_Cellsize		= 1.0;
		m_xMin			= 0.0;
		m_yMin			= 0.0;
	}

	m_TF_A[0]		= Transform[0];
	m_TF_A[1]		= Transform[3];
	m_TF_B[0][0]	= Transform[1];
	m_TF_B[0][1]	= Transform[2];
	m_TF_B[1][0]	= Transform[4];
	m_TF_B[1][1]	= Transform[5];
	m_TF_BInv		= m_TF_B.Get_Inverse();

	return( true );
}


///////////////////////////////////////////////////////////
//														 //
///////////////////////////////////////////////////////////

//---------------------------------------------------------
bool CSG_GDAL_DataSet::Open_Write(const CSG_String &File_Name, const CSG_String &Driver, const CSG_String &Options, TSG_Data_Type Type, int NBands, const CSG_Grid_System &System, const CSG_Projection &Projection)
{
	Close();

	//--------------------------------------------------------
	char	**pOptions	= NULL;
	
	if( !Options.is_Empty() )
	{
		char	**pTokens	= CSLTokenizeString2(Options, " ", CSLT_STRIPLEADSPACES);
	  
		for(int i=0; pTokens && pTokens[i]; i++)
		{
			pOptions	= CSLAddString(pOptions, pTokens[i]);
		}
	}

	//--------------------------------------------------------
	GDALDriver	*pDriver;

	if( (pDriver = gSG_GDAL_Drivers.Get_Driver(Driver)) == NULL )
	{
		SG_UI_Msg_Add_Error(CSG_String::Format(SG_T("%s: %s"), _TL("driver not found."), Driver.c_str()));

		return( false );
	}

	if( !GDALValidateCreationOptions(pDriver, pOptions) )
	{
		SG_UI_Msg_Add_Error(CSG_String::Format(SG_T("%s: %s"), _TL("Creation option(s) not supported by the driver"), Options.c_str()));

		return( false );
	}

	if( CSLFetchBoolean(pDriver->GetMetadata(), GDAL_DCAP_CREATE, false) == false )
	{
		SG_UI_Msg_Add_Error(_TL("Driver does not support file creation."));

		return( false );
	}

	if( (m_pDataSet = pDriver->Create(File_Name, System.Get_NX(), System.Get_NY(), NBands, (GDALDataType)gSG_GDAL_Drivers.Get_GDAL_Type(Type), pOptions)) == NULL )
	{
		SG_UI_Msg_Add_Error(_TL("Could not create dataset."));

		return( false );
	}

	//--------------------------------------------------------
	m_File_Name	= File_Name;

	m_Access	= SG_GDAL_IO_WRITE;

	if( Projection.is_Okay() )
	{
		m_pDataSet->SetProjection(Projection.Get_WKT());
	}

	double	Transform[6]	=
	{
		System.Get_XMin() - 0.5 * System.Get_Cellsize(), System.Get_Cellsize(), 0.0,
		System.Get_YMax() + 0.5 * System.Get_Cellsize(), 0.0, -System.Get_Cellsize()
	};

	m_pDataSet->SetGeoTransform(Transform);

	m_NX			= m_pDataSet->GetRasterXSize();
	m_NY			= m_pDataSet->GetRasterYSize();

	m_bTransform	= false;
	m_Cellsize		= 1.0;
	m_xMin			= 0.5;
	m_yMin			= 0.5;

	return( true );
}

//---------------------------------------------------------
bool CSG_GDAL_DataSet::Close(void)
{
	if( m_pVrtSource )
	{
		GDALClose(m_pVrtSource); m_pVrtSource = NULL;

		if( m_pDataSet )
		{
		//	GDALClose(m_pDataSet);	// this crashes in debug mode, gdal2.0dev!!!(???)
			m_pDataSet = NULL;
		}
	}

	if( m_pDataSet )
	{
		GDALClose(m_pDataSet); m_pDataSet = NULL;
	}

	m_File_Name.Clear();

	m_Access	= SG_GDAL_IO_CLOSED;

	if( strlen(CPLGetLastErrorMsg()) > 3 )
	{
		SG_UI_Msg_Add_Error(CSG_String::Format(SG_T("%s: %s"), _TL("Dataset creation failed"), SG_STR_MBTOSG(CPLGetLastErrorMsg())));

		CPLErrorReset();

		return( false );
	}

	return( true );
}


///////////////////////////////////////////////////////////
//														 //
///////////////////////////////////////////////////////////

//---------------------------------------------------------
GDALDriver * CSG_GDAL_DataSet::Get_Driver(void)	const
{
	return( m_pDataSet ? m_pDataSet->GetDriver() : NULL );
}

//---------------------------------------------------------
CSG_String CSG_GDAL_DataSet::Get_DriverID(void)	const
{
	return( m_pDataSet && m_pDataSet->GetDriver() && m_pDataSet->GetDriver()->GetDescription() ? m_pDataSet->GetDriver()->GetDescription() : "" );
}

//---------------------------------------------------------
const char * CSG_GDAL_DataSet::Get_Projection(void)	const
{
	return( m_pDataSet && m_pDataSet->GetProjectionRef() ? m_pDataSet->GetProjectionRef() : "" );
}

//---------------------------------------------------------
CSG_String CSG_GDAL_DataSet::Get_Name(void)	const
{
	return( m_pDataSet ? m_pDataSet->GetMetadataItem(GDAL_DMD_LONGNAME) : "" );
}

//---------------------------------------------------------
CSG_String CSG_GDAL_DataSet::Get_Description(void)	const
{
	return( m_pDataSet ? m_pDataSet->GetDescription() : "" );
}

//---------------------------------------------------------
CSG_String CSG_GDAL_DataSet::Get_File_Name(void)	const
{
	return( m_File_Name );
}

//---------------------------------------------------------
const char * CSG_GDAL_DataSet::Get_MetaData_Item(const char *pszName, const char *pszDomain)	const
{
	return( m_pDataSet ? m_pDataSet->GetMetadataItem(pszName, pszDomain) : "" );
}

//---------------------------------------------------------
const char ** CSG_GDAL_DataSet::Get_MetaData(const char *pszDomain)	const
{
	return( m_pDataSet ? (const char **)m_pDataSet->GetMetadata(pszDomain) : NULL );
}

//---------------------------------------------------------
bool CSG_GDAL_DataSet::Get_MetaData_Item(CSG_String &MetaData, const char *pszName, const char *pszDomain)		const
{
	const char	*Item	= Get_MetaData_Item(pszName, pszDomain);

	if( Item && *Item )
	{
		MetaData	= Item;

		return( true );
	}

	return( false );
}

//---------------------------------------------------------
bool CSG_GDAL_DataSet::Get_MetaData(CSG_MetaData &MetaData)	const
{
	if( m_pDataSet && is_Reading() )
	{
		char	**pMetaData	= m_pDataSet->GetMetadata() + 0;

		if( pMetaData )
		{
			while( *pMetaData )
			{
				CSG_String	s(*pMetaData);

				MetaData.Add_Child(s.BeforeFirst(SG_T('=')), s.AfterFirst(SG_T('=')));

				pMetaData++;
			}

			return( true );
		}
	}

	return( false );
}

//---------------------------------------------------------
bool CSG_GDAL_DataSet::Get_MetaData(CSG_MetaData &MetaData, const char *pszDomain) const
{
	if( m_pDataSet && is_Reading() )
	{
		char	**pMetaData	= m_pDataSet->GetMetadata(pszDomain) + 0;

		if( pMetaData )
		{
			while( *pMetaData )
			{
				CSG_String	s(*pMetaData);

				MetaData.Add_Child(s.BeforeFirst(SG_T('=')), s.AfterFirst(SG_T('=')));

				pMetaData++;
			}

			return( true );
		}
	}

	return( false );
}


///////////////////////////////////////////////////////////
//														 //
///////////////////////////////////////////////////////////

//---------------------------------------------------------
int CSG_GDAL_DataSet::Get_Count(void)	const
{
	return( m_pDataSet ? m_pDataSet->GetRasterCount() : 0 );
}

//---------------------------------------------------------
CSG_String CSG_GDAL_DataSet::Get_Name(int i)	const
{
	CSG_String		Name;

	GDALRasterBand	*pBand;

	if( is_Reading() && (pBand = m_pDataSet->GetRasterBand(i + 1)) != NULL )
	{
		const char	*s;

		//-------------------------------------------------
		if( !SG_STR_CMP(m_pDataSet->GetDriver()->GetDescription(), "GRIB") )
		{
			if( (s = pBand->GetMetadataItem("GRIB_COMMENT")) != NULL && *s )
			{
				Name	= s;

				if( (s = pBand->GetMetadataItem("GRIB_ELEMENT"   )) != NULL && *s )	{	Name += "["; Name += s; Name += "]";	}
				if( (s = pBand->GetMetadataItem("GRIB_SHORT_NAME")) != NULL && *s )	{	Name += "["; Name += s; Name += "]";	}
				if( (s = pBand->GetMetadataItem("GRIB_VALID_TIME")) != NULL && *s )	{	Name += CSG_String::Format(SG_T("[%s]"), CSG_Time_Converter::Get_String(atoi(s), SG_TIME_FMT_Seconds_Unix).c_str());	}
			}
		}

		//-------------------------------------------------
		if( !SG_STR_CMP(m_pDataSet->GetDriver()->GetDescription(), "netCDF") )
		{
			if( (s = pBand->GetMetadataItem("NETCDF_VARNAME"        )) != NULL && *s )	{	Name += "["; Name += s; Name += "]";	}
			if( (s = pBand->GetMetadataItem("NETCDF_DIMENSION_time" )) != NULL && *s )	{	Name += "["; Name += s; Name += "]";	}
			if( (s = pBand->GetMetadataItem("NETCDF_DIMENSION_level")) != NULL && *s )	{	Name += "["; Name += s; Name += "]";	}
		}

		//-------------------------------------------------
		if( Name.is_Empty() )
		{
			Name	= Get_Name();

			if( Name.is_Empty() )
			{
				Name	= _TL("Band");
			}

			Name	+= CSG_String::Format(SG_T(" %0*d"), SG_Get_Digit_Count(Get_Count() + 1), i + 1);
		}
	}

	return( Name );
}

//---------------------------------------------------------
CSG_String CSG_GDAL_DataSet::Get_Description(int i)	const
{
	CSG_String		Description;

	GDALRasterBand	*pBand;

	if( is_Reading() && (pBand = m_pDataSet->GetRasterBand(i + 1)) != NULL )
	{
		char	**pMetaData	= pBand->GetMetadata() + 0;

		if( pMetaData )
		{
			while( *pMetaData )
			{
				CSG_String	s(*pMetaData);

				Description	+= s + SG_T("\n");

				pMetaData++;
			}
		}
	}

	return( Description );
}

//---------------------------------------------------------
bool CSG_GDAL_DataSet::Get_MetaData(int i, CSG_MetaData &MetaData)	const
{
	GDALRasterBand	*pBand;

	if( is_Reading() && (pBand = m_pDataSet->GetRasterBand(i + 1)) != NULL )
	{
		char	**pMetaData	= pBand->GetMetadata() + 0;

		if( pMetaData )
		{
			while( *pMetaData )
			{
				CSG_String	s(*pMetaData);

				MetaData.Add_Child(s.BeforeFirst(SG_T('=')), s.AfterFirst(SG_T('=')));

				pMetaData++;
			}

			return( true );
		}
	}

	return( false );
}

//---------------------------------------------------------
const char * CSG_GDAL_DataSet::Get_MetaData_Item(int i, const char *pszName)	const
{
	GDALRasterBand	*pBand	= m_pDataSet->GetRasterBand(i + 1);

	return( pBand ? pBand->GetMetadataItem(pszName) : "" );
}

bool CSG_GDAL_DataSet::Get_MetaData_Item(int i, const char *pszName, CSG_String &MetaData)	const
{
	GDALRasterBand	*pBand;

	if( (pBand = m_pDataSet->GetRasterBand(i + 1)) != NULL )
	{
		const char	*pMetaData	= pBand->GetMetadataItem(pszName);

		if( pMetaData && *pMetaData )
		{
			MetaData	= pMetaData;

			return( true );
		}
	}

	return( false );
}

//---------------------------------------------------------
CSG_Grid * CSG_GDAL_DataSet::Read(int i)
{
	//-------------------------------------------------
	if( !is_Reading() )
	{
		return( NULL );
	}

	//-------------------------------------------------
	GDALRasterBand	*pBand	= m_pDataSet->GetRasterBand(i + 1);

	if( !pBand )
	{
		return( NULL );
	}

	//-------------------------------------------------
	TSG_Data_Type	Type	= gSG_GDAL_Drivers.Get_SAGA_Type(pBand->GetRasterDataType());

	CSG_Grid	*pGrid	= SG_Create_Grid(Type, Get_NX(), Get_NY(), Get_Cellsize(), Get_xMin(), Get_yMin());

	if( !pGrid )
	{
		return( NULL );
	}

	//-------------------------------------------------
	int		bSuccess;

	double	zScale	= pBand->GetScale (&bSuccess);	if( !bSuccess || !zScale )	zScale	= 1.0;
	double	zOffset	= pBand->GetOffset(&bSuccess);	if( !bSuccess            )	zOffset	= 0.0;

	pGrid->Set_Name			(Get_Name       (i));
	pGrid->Set_Description	(Get_Description(i));
	pGrid->Set_Unit			(CSG_String(pBand->GetUnitType()));
	pGrid->Set_Scaling		(zScale, zOffset);

	pBand->GetNoDataValue(&bSuccess);

	if( bSuccess )
	{
		pGrid->Set_NoData_Value(pBand->GetNoDataValue(&bSuccess));
	}

	pGrid->Get_Projection().Create(Get_Projection(), SG_PROJ_FMT_WKT);

	Get_MetaData(i, pGrid->Get_MetaData());

	//-------------------------------------------------
	double	*zLine	= (double *)SG_Malloc(Get_NX() * sizeof(double));

	for(int y=0; y<Get_NY() && SG_UI_Process_Set_Progress(y, Get_NY()); y++)
	{
		int	yy	= m_bTransform ? y : Get_NY() - 1 - y;

		if( pBand->RasterIO(GF_Read, 0, y, Get_NX(), 1, zLine, Get_NX(), 1, GDT_Float64, 0, 0) == CE_None )
		{
			for(int x=0; x<Get_NX(); x++)
			{
			//	double	NaN	= 0.0;	NaN	= -1.0 / NaN;	if( NaN == zLine[x] )	pGrid->Set_NoData(x, yy); else
				pGrid->Set_Value(x, yy, zLine[x], false);
			}
		}
	}

	SG_Free(zLine);

	return( pGrid );
}

//---------------------------------------------------------
bool CSG_GDAL_DataSet::Write(int i, CSG_Grid *pGrid, double noDataValue)
{
	if( !m_pDataSet || !pGrid || pGrid->Get_NX() != Get_NX() || pGrid->Get_NY() != Get_NY() || i < 0 || i >= Get_Count() )
	{
		return( false );
	}

	GDALRasterBand	*pBand	= m_pDataSet->GetRasterBand(i + 1);

	//-----------------------------------------------------
	CPLErr	Error	= CE_None;

	double	*zLine	= (double *)SG_Malloc(Get_NX() * sizeof(double));

	for(int y=0, yy=Get_NY()-1; Error==CE_None && y<Get_NY() && SG_UI_Process_Set_Progress(y, Get_NY()); y++, yy--)
	{
		for(int x=0; x<Get_NX(); x++)
		{
			zLine[x]	= pGrid->is_NoData(x, yy) ? noDataValue : pGrid->asDouble(x, yy);
		}

		Error	= pBand->RasterIO(GF_Write, 0, y, Get_NX(), 1, zLine, Get_NX(), 1, GDT_Float64, 0, 0);
	}

	SG_Free(zLine);

	//-----------------------------------------------------
	if( Error != CE_None )
	{
		SG_UI_Msg_Add_Error(CSG_String::Format(SG_T("%s"), _TL("Writing dataset failed.")));

		return( false );
	}

	//-----------------------------------------------------
	pBand->SetNoDataValue	(noDataValue);
	pBand->SetStatistics	(pGrid->Get_ZMin(), pGrid->Get_ZMax(), pGrid->Get_Mean(), pGrid->Get_StdDev());

	return( true );	
}

//---------------------------------------------------------
bool CSG_GDAL_DataSet::Write(int i, CSG_Grid *pGrid)
{
	return (CSG_GDAL_DataSet::Write (i, pGrid, pGrid->Get_NoData_Value()));
}


///////////////////////////////////////////////////////////
//														 //
///////////////////////////////////////////////////////////

//---------------------------------------------------------
CSG_Rect CSG_GDAL_DataSet::Get_Extent(bool bTransform)	const
{
	if( bTransform && Needs_Transformation() )
	{
		CSG_Grid_System	System;

		if( Get_Transformation(System, false) )
		{
			return( System.Get_Extent() );
		}
	}

	return( CSG_Rect(Get_xMin(), Get_yMin(), Get_xMax(), Get_yMax()) );
}

//---------------------------------------------------------
CSG_Grid_System CSG_GDAL_DataSet::Get_System(void)	const
{
	CSG_Grid_System	System;

	if( !Needs_Transformation() || !Get_Transformation(System, false) )
	{
		System.Assign(Get_Cellsize(), Get_xMin(), Get_yMin(), Get_NX(), Get_NY());
	}

	return( System );
}


///////////////////////////////////////////////////////////
//														 //
///////////////////////////////////////////////////////////

//---------------------------------------------------------
bool CSG_GDAL_DataSet::Get_Transformation(CSG_Grid_System &System, bool bVerbose) const
{
	CSG_Vector	A;
	CSG_Matrix	B;

	Get_Transformation(A, B);

	//-----------------------------------------------------
	if( Needs_Transformation() )
	{
		CSG_Vector	v(2);
		CSG_Rect	r;

		v[0]	= Get_xMin() + 0.5;	v[1]	= Get_yMin() + 0.5;	v	= B * v + A;	r.Assign(v[0], v[1], v[0], v[1]);
		v[0]	= Get_xMin() + 0.5;	v[1]	= Get_yMax() - 0.5;	v	= B * v + A;	r.Union(CSG_Point(v[0], v[1]));
		v[0]	= Get_xMax() - 0.5;	v[1]	= Get_yMax() - 0.5;	v	= B * v + A;	r.Union(CSG_Point(v[0], v[1]));
		v[0]	= Get_xMax() - 0.5;	v[1]	= Get_yMin() + 0.5;	v	= B * v + A;	r.Union(CSG_Point(v[0], v[1]));

		v[0]	= 1;	v[1] = 0;	v = B * v;	double	dx	= v.Get_Length();
		v[0]	= 0;	v[1] = 1;	v = B * v;	double	dy	= v.Get_Length();

		if( dx != dy )
		{
			if( bVerbose )
			{
				SG_UI_Msg_Add_Execution(CSG_String::Format("\n%s: %s\n\t%s: %f",
					_TL("warning"), _TL("top-to-bottom and left-to-right cell sizes differ."), _TL("Difference"), fabs(dy - dx)), false
				);
			}

			if( dx > dy )
			{
				dx	= dy;
			}

			if( bVerbose )
			{
				SG_UI_Msg_Add_Execution(CSG_String::Format("\n\t%s: %f\n", _TL("using cellsize"), dx), false);
			}
		}

		return( System.Assign(dx, r) );
	}

	//-----------------------------------------------------
	return( false );
}

//---------------------------------------------------------
bool CSG_GDAL_DataSet::Get_Transformation(CSG_Grid **ppGrid, TSG_Grid_Resampling	Interpolation, bool bVerbose)	const
{
	CSG_Grid_System	System;

	if( Get_Transformation(System, bVerbose) )
	{
		return( Get_Transformation(ppGrid, Interpolation, System, bVerbose) );
	}

	return( false );
}

//---------------------------------------------------------
bool CSG_GDAL_DataSet::Get_Transformation(CSG_Grid **ppGrid, TSG_Grid_Resampling	Interpolation, const CSG_Grid_System &System, bool bVerbose)	const
{
	if( !System.is_Valid() )
	{
		return( false );
	}

	//-----------------------------------------------------
	CSG_Vector	A;
	CSG_Matrix	B, BInv;

	Get_Transformation(A, B);

	BInv	= B.Get_Inverse();

	//-----------------------------------------------------
	CSG_Grid	*pImage	= *ppGrid;
	CSG_Grid	*pWorld	= SG_Create_Grid(System, pImage->Get_Type());

	if( !pWorld )
	{
		return( false );
	}

	*ppGrid	= pWorld;

	pWorld->Set_Name              (pImage->Get_Name        ());
	pWorld->Set_Description       (pImage->Get_Description ());
	pWorld->Set_Unit              (pImage->Get_Unit        ());
	pWorld->Set_Scaling           (pImage->Get_Scaling     (), pImage->Get_Offset());
	pWorld->Set_NoData_Value_Range(pImage->Get_NoData_Value(), pImage->Get_NoData_hiValue());
	pWorld->Get_MetaData()	     = pImage->Get_MetaData    ();
	pWorld->Get_Projection()     = pImage->Get_Projection  ();

	//-----------------------------------------------------
//	#pragma omp parallel for
//	for(int y=0; y<pWorld->Get_NY(); y++)
//	{
//		Process_Get_Okay();

	for(int y=0; y<pWorld->Get_NY() && SG_UI_Process_Set_Progress(y, pWorld->Get_NY()); y++)
	{
		#pragma omp parallel for
		for(int x=0; x<pWorld->Get_NX(); x++)
		{
			double		z;
			CSG_Vector	vWorld(2), vImage;

			vWorld[0]	= pWorld->Get_XMin() + (x - 0.5) * pWorld->Get_Cellsize();
			vWorld[1]	= pWorld->Get_YMin() + (y + 0.5) * pWorld->Get_Cellsize();

			vImage	= BInv * (vWorld - A);

			if( pImage->Get_Value(vImage[0], vImage[1], z, Interpolation, false, true) )
			{
				pWorld->Set_Value(x, y, z);
			}
			else
			{
				pWorld->Set_NoData(x, y);
			}
		}
	}

	delete(pImage);

	return( true );
}


///////////////////////////////////////////////////////////
//														 //
//														 //
//														 //
///////////////////////////////////////////////////////////

//---------------------------------------------------------
TSG_Data_Type	SG_Get_Grid_Type	(CSG_Parameter_Grid_List *pGrids)
{
	TSG_Data_Type	Type	= SG_DATATYPE_Byte;

	if( pGrids )
	{
		for(int i=0; i<pGrids->Get_Count(); i++)
		{
			if( SG_Data_Type_Get_Size(Type) <= SG_Data_Type_Get_Size(pGrids->asGrid(i)->Get_Type()) )
			{
				Type	= pGrids->asGrid(i)->Get_Type();
			}
		}
	}

	return( Type );
}


///////////////////////////////////////////////////////////
//														 //
//														 //
//														 //
///////////////////////////////////////////////////////////

//---------------------------------------------------------

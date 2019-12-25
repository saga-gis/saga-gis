
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
#include "gdal_driver.h"

#include <gdal_vrt.h>

#include <ogr_srs_api.h>

#include <cpl_string.h>
#include <cpl_error.h>


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
	return( GDALGetDriverCount() );
}

//---------------------------------------------------------
GDALDriverH CSG_GDAL_Drivers::Get_Driver(const CSG_String &Name) const
{
	return( GDALGetDriverByName(Name) );
}

//---------------------------------------------------------
GDALDriverH CSG_GDAL_Drivers::Get_Driver(int Index) const
{
	return( GDALGetDriver(Index) );
}

//---------------------------------------------------------
CSG_String CSG_GDAL_Drivers::Get_Name(int Index) const
{
	const char	*s	= GDALGetMetadataItem(Get_Driver(Index), GDAL_DMD_LONGNAME, "");

	return( s ? s : "" );
}

//---------------------------------------------------------
CSG_String CSG_GDAL_Drivers::Get_Description(int Index) const
{
	const char	*s	= GDALGetDescription(Get_Driver(Index));

	return( s ? s : "" );
}

//---------------------------------------------------------
CSG_String CSG_GDAL_Drivers::Get_Extension(int Index) const
{
	const char	*s	= GDALGetMetadataItem(Get_Driver(Index), GDAL_DMD_EXTENSION, "");

	return( s ? s : "" );
}

//---------------------------------------------------------
bool CSG_GDAL_Drivers::has_Capability(GDALDriverH pDriver, const char *Capapility)
{
	const char	*s	= GDALGetMetadataItem(pDriver, Capapility, "");

	return( s && SG_STR_CMP(s, "YES") == 0 );
}

//---------------------------------------------------------
bool CSG_GDAL_Drivers::is_Raster(int Index) const
{
#ifdef GDAL_V2_0_OR_NEWER
	return( has_Capability(Get_Driver(Index), GDAL_DCAP_RASTER) );
#else
	return( true );
#endif
}

//---------------------------------------------------------
bool CSG_GDAL_Drivers::Can_Read(int Index) const
{
#ifdef GDAL_DCAP_OPEN
	return( has_Capability(Get_Driver(Index), GDAL_DCAP_OPEN) );
#else
	return( true );
#endif
}

//---------------------------------------------------------
bool CSG_GDAL_Drivers::Can_Write(int Index) const
{
	return( has_Capability(Get_Driver(Index), GDAL_DCAP_CREATE) );
}

//---------------------------------------------------------
bool CSG_GDAL_Drivers::Can_Copy(int Index) const
{
	return( has_Capability(Get_Driver(Index), GDAL_DCAP_CREATECOPY) );
}


///////////////////////////////////////////////////////////
//														 //
///////////////////////////////////////////////////////////

//---------------------------------------------------------
int CSG_GDAL_Drivers::Get_GDAL_Type(TSG_Data_Type Type)
{
	switch( Type )
	{
	case SG_DATATYPE_Bit   :	return( GDT_Byte    );	// Eight bit unsigned integer
	case SG_DATATYPE_Byte  :	return( GDT_Byte    );	// Eight bit unsigned integer
	case SG_DATATYPE_Char  :	return( GDT_Byte    );	// Eight bit unsigned integer
	case SG_DATATYPE_Word  :	return( GDT_UInt16  );	// Sixteen bit unsigned integer
	case SG_DATATYPE_Short :	return( GDT_Int16   );	// Sixteen bit signed integer
	case SG_DATATYPE_DWord :	return( GDT_UInt32  );	// Thirty two bit unsigned integer
	case SG_DATATYPE_Int   :	return( GDT_Int32   );	// Thirty two bit signed integer
	case SG_DATATYPE_Float :	return( GDT_Float32 );	// Thirty two bit floating point
	case SG_DATATYPE_Double:	return( GDT_Float64 );	// Sixty four bit floating point

	default                :	return( GDT_Float64 );
	}
}

//---------------------------------------------------------
TSG_Data_Type CSG_GDAL_Drivers::Get_SAGA_Type(int Type)
{
	switch( Type )
	{
	case GDT_Byte    :	return( SG_DATATYPE_Byte      );	// Eight bit unsigned integer
	case GDT_UInt16  :	return( SG_DATATYPE_Word      );	// Sixteen bit unsigned integer
	case GDT_Int16   :	return( SG_DATATYPE_Short     );	// Sixteen bit signed integer
	case GDT_UInt32  :	return( SG_DATATYPE_DWord     );	// Thirty two bit unsigned integer
	case GDT_Int32   : 	return( SG_DATATYPE_Int       );	// Thirty two bit signed integer
	case GDT_Float32 : 	return( SG_DATATYPE_Float     );	// Thirty two bit floating point
	case GDT_Float64 : 	return( SG_DATATYPE_Double    );	// Sixty four bit floating point

	case GDT_CInt16  : 	return( SG_DATATYPE_Undefined );	// Complex Int16
	case GDT_CInt32  : 	return( SG_DATATYPE_Undefined );	// Complex Int32
	case GDT_CFloat32: 	return( SG_DATATYPE_Undefined );	// Complex Float32
	case GDT_CFloat64: 	return( SG_DATATYPE_Undefined );	// Complex Float64

	default          :	return( SG_DATATYPE_Undefined );
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
bool CSG_GDAL_DataSet::Open_Read(const CSG_String &File_Name, const char *Drivers[])
{
	Close();

	#ifdef GDAL_V2_0_OR_NEWER
	if( Drivers )
	{
		m_pDataSet	= GDALOpenEx(File_Name, GA_ReadOnly, Drivers, NULL, NULL);
	}
	#else
	m_pDataSet = NULL;
	#endif

	if( !m_pDataSet && (m_pDataSet = GDALOpen(File_Name, GA_ReadOnly)) == NULL )
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

	if( (m_pVrtSource = GDALOpen(File_Name, GA_ReadOnly)) == NULL )
	{
		return( false );
	}

	//-----------------------------------------------------
	if( (m_pDataSet = VRTCreate(System.Get_NX(), System.Get_NY())) == NULL )
	{
		Close();

		return( false );
	}

	GDALSetProjection(m_pDataSet, GDALGetProjectionRef(m_pVrtSource));

	double	Transform[6]	=
	{
		System.Get_XMin(true), System.Get_Cellsize(), 0.0,
		System.Get_YMax(true), 0.0, -System.Get_Cellsize()
	};

	GDALSetGeoTransform(m_pDataSet, Transform);

	//-----------------------------------------------------
	GDALGetGeoTransform(m_pVrtSource, Transform);

	if( Transform[2] != 0.0 || Transform[4] != 0.0 )
	{
		return( false );	// geotransform is rotated, this configuration is not supported...
	}

	int	xOff	= (int)floor((System.Get_XMin  (true) - Transform[0]) /      Transform[1]  + 0.001);
	int	yOff	= (int)floor((System.Get_YMax  (true) - Transform[3]) /      Transform[5]  + 0.001);
	int	xSize	= (int)     ( System.Get_XRange(true)                 /      Transform[1]  + 0.5  );
	int	ySize	= (int)     ( System.Get_YRange(true)                 / fabs(Transform[5]) + 0.5  );

	//-----------------------------------------------------
	for(int i=0; i<GDALGetRasterCount(m_pVrtSource); i++)
	{
		GDALRasterBandH pSrcBand	= GDALGetRasterBand(m_pVrtSource, i + 1);

		GDALAddBand(m_pDataSet, GDALGetRasterDataType(pSrcBand), NULL);

		VRTSourcedRasterBandH	pVrtBand	= (VRTSourcedRasterBandH)GDALGetRasterBand(m_pDataSet, i + 1);

		VRTAddSimpleSource(pVrtBand, pSrcBand,
			xOff, yOff, xSize, ySize, 0, 0, System.Get_NX(), System.Get_NY(), "near", VRT_NODATA_UNSET
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
CSG_Strings CSG_GDAL_DataSet::Get_SubDataSets(bool bDescription) const
{
	CSG_MetaData	MetaData;	Get_MetaData(MetaData, "SUBDATASETS");

	CSG_Strings	SubDataSets;

	const SG_Char	*Type	= bDescription ? SG_T("DESC") : SG_T("NAME");

	for(int i=0; i==SubDataSets.Get_Count(); i++)
	{
		CSG_MetaData	*pSubDataSet	= MetaData(CSG_String::Format("SUBDATASET_%d_%s", i + 1, Type));

		if( pSubDataSet )
		{
			SubDataSets	+= pSubDataSet->Get_Content();
		}
	}

	return( SubDataSets );
}

//---------------------------------------------------------
bool CSG_GDAL_DataSet::_Get_Transformation(double Transform[6])
{
	if( GDALGetGeoTransform(m_pDataSet, Transform) == CE_None )
	{
		return( true );
	}

	//-----------------------------------------------------
	Transform[0]	= 0.;
	Transform[1]	= 1.;
	Transform[2]	= 0.;
	Transform[3]	= 0.;
	Transform[4]	= 0.;
	Transform[5]	= 1.;

	bool	bResult	= false;	CSG_String	Data;

	if( Get_MetaData_Item(Data, "XORIG") && Data.asDouble(Transform[0]) ) { bResult = true; }
	if( Get_MetaData_Item(Data, "XCELL") && Data.asDouble(Transform[1]) ) { bResult = true; }
	if( Get_MetaData_Item(Data, "YORIG") && Data.asDouble(Transform[3]) ) { bResult = true; }
	if( Get_MetaData_Item(Data, "YCELL") && Data.asDouble(Transform[5]) ) { bResult = true; }

	return( bResult );
}

//---------------------------------------------------------
bool CSG_GDAL_DataSet::_Set_Transformation(void)
{
	if( !m_pDataSet )
	{
		return( false );
	}

	double	Transform[6];

	m_NX	= GDALGetRasterXSize(m_pDataSet);
	m_NY	= GDALGetRasterYSize(m_pDataSet);

	if( _Get_Transformation(Transform) == false )
	{
		m_bTransform	= false;
		m_Cellsize		= 1.0;
		m_xMin			= 0.0;
		m_yMin			= 0.0;
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
		m_xMin			= 0.5;
		m_yMin			= 0.5;
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
	GDALDriverH	pDriver;

	if( (pDriver = gSG_GDAL_Drivers.Get_Driver(Driver)) == NULL )
	{
		SG_UI_Msg_Add_Error(CSG_String::Format("%s: %s", _TL("driver not found."), Driver.c_str()));

		return( false );
	}

	if( !CSG_GDAL_Drivers::has_Capability(pDriver, GDAL_DCAP_CREATE) )
	{
		SG_UI_Msg_Add_Error(_TL("Driver does not support file creation."));

		return( false );
	}

	//--------------------------------------------------------
	char	**pOptions	= Options.is_Empty() ? NULL : CSLTokenizeString2(Options, " ", CSLT_STRIPLEADSPACES);

	if( !GDALValidateCreationOptions(pDriver, pOptions) )
	{
		SG_UI_Msg_Add_Error(CSG_String::Format("%s: %s", _TL("Creation option(s) not supported by the driver"), Options.c_str()));

		CSLDestroy(pOptions);

		return( false );
	}

	if( (m_pDataSet = GDALCreate(pDriver, File_Name, System.Get_NX(), System.Get_NY(), NBands, (GDALDataType)gSG_GDAL_Drivers.Get_GDAL_Type(Type), pOptions)) == NULL )
	{
		SG_UI_Msg_Add_Error(_TL("Could not create dataset."));

		CSLDestroy(pOptions);

		return( false );
	}

	CSLDestroy(pOptions);

	//--------------------------------------------------------
	m_File_Name	= File_Name;

	m_Access	= SG_GDAL_IO_WRITE;

	if( Projection.is_Okay() )
	{
		GDALSetProjection(m_pDataSet, Projection.Get_WKT());
	}

	double	Transform[6]	=
	{
		System.Get_XMin() - 0.5 * System.Get_Cellsize(), System.Get_Cellsize(), 0.0,
		System.Get_YMax() + 0.5 * System.Get_Cellsize(), 0.0, -System.Get_Cellsize()
	};

	GDALSetGeoTransform(m_pDataSet, Transform);

	m_NX			= GDALGetRasterXSize(m_pDataSet);
	m_NY			= GDALGetRasterYSize(m_pDataSet);

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
		SG_UI_Msg_Add_Error(CSG_String::Format("%s: %s", _TL("Dataset creation failed"), SG_STR_MBTOSG(CPLGetLastErrorMsg())));

		CPLErrorReset();

		return( false );
	}

	return( true );
}


///////////////////////////////////////////////////////////
//														 //
///////////////////////////////////////////////////////////

//---------------------------------------------------------
GDALDriverH CSG_GDAL_DataSet::Get_Driver(void)	const
{
	return( m_pDataSet ? GDALGetDatasetDriver(m_pDataSet) : NULL );
}

//---------------------------------------------------------
CSG_String CSG_GDAL_DataSet::Get_DriverID(void)	const
{
	const char	*s	= GDALGetDescription(Get_Driver());

	return( s ? s : "" );
}

//---------------------------------------------------------
const char * CSG_GDAL_DataSet::Get_Projection(void)	const
{
	const char	*s	= GDALGetProjectionRef(m_pDataSet);

	return( s ? s : "" );
}

//---------------------------------------------------------
CSG_String CSG_GDAL_DataSet::Get_Name(void)	const
{
	const char	*s	= GDALGetMetadataItem(m_pDataSet, GDAL_DMD_LONGNAME, 0);

	return( s ? s : "" );
}

//---------------------------------------------------------
CSG_String CSG_GDAL_DataSet::Get_Description(void)	const
{
	const char	*s	= GDALGetDescription(m_pDataSet);

	return( s ? s : "" );
}

//---------------------------------------------------------
CSG_String CSG_GDAL_DataSet::Get_File_Name(void)	const
{
	return( m_File_Name );
}

//---------------------------------------------------------
const char * CSG_GDAL_DataSet::Get_MetaData_Item(const char *pszName, const char *pszDomain)	const
{
	const char	*s	= GDALGetMetadataItem(m_pDataSet, pszName, pszDomain);

	return( s ? s : "" );
}

//---------------------------------------------------------
const char ** CSG_GDAL_DataSet::Get_MetaData(const char *pszDomain)	const
{
	return( m_pDataSet ? (const char **)GDALGetMetadata(m_pDataSet, pszDomain) : NULL );
}

//---------------------------------------------------------
bool CSG_GDAL_DataSet::Get_MetaData_Item(CSG_String &MetaData, const char *pszName, const char *pszDomain)		const
{
	const char	*s	= Get_MetaData_Item(pszName, pszDomain);

	if( s && *s )
	{
		MetaData	= s;

		return( true );
	}

	return( false );
}

//---------------------------------------------------------
CSG_Strings CSG_GDAL_DataSet::Get_MetaData_Domains(void)	const
{
	CSG_Strings	Domains;

	if( m_pDataSet && is_Reading() )
	{
		char	**pDomains	= GDALGetMetadataDomainList(m_pDataSet);

		if( pDomains )
		{
			while( *pDomains )
			{
				Domains	+= *pDomains;

				pDomains++;
			}
		}
	}

	return( Domains );
}

//---------------------------------------------------------
bool CSG_GDAL_DataSet::Get_MetaData(CSG_MetaData &MetaData)	const
{
	if( m_pDataSet && is_Reading() )
	{
		char	**pMetaData	= GDALGetMetadata(m_pDataSet, 0);

		if( pMetaData )
		{
			while( *pMetaData )
			{
				CSG_String	s(*pMetaData);

				MetaData.Add_Child(s.BeforeFirst('='), s.AfterFirst('='));

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
		char	**pMetaData	= GDALGetMetadata(m_pDataSet, pszDomain);

		if( pMetaData )
		{
			while( *pMetaData )
			{
				CSG_String	s(*pMetaData);

				MetaData.Add_Child(s.BeforeFirst('='), s.AfterFirst('='));

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
	return( m_pDataSet ? GDALGetRasterCount(m_pDataSet) : 0 );
}

//---------------------------------------------------------
CSG_String CSG_GDAL_DataSet::Get_Name(int i)	const
{
	CSG_String		Name;

	GDALRasterBandH	pBand;

	if( is_Reading() && (pBand = GDALGetRasterBand(m_pDataSet, i + 1)) != NULL )
	{
		const char	*s;

		//-------------------------------------------------
		if( !Get_DriverID().Cmp("GRIB") )
		{
			if( (s = GDALGetMetadataItem(pBand, "GRIB_COMMENT", 0)) != NULL && *s )
			{
				Name	= s;	CSG_DateTime	d;

				if( (s = GDALGetMetadataItem(pBand, "GRIB_ELEMENT"   , 0)) != NULL && *s )	{	Name += "["; Name += s; Name += "]";	}
				if( (s = GDALGetMetadataItem(pBand, "GRIB_SHORT_NAME", 0)) != NULL && *s )	{	Name += "["; Name += s; Name += "]";	}
				if( (s = GDALGetMetadataItem(pBand, "GRIB_VALID_TIME", 0)) != NULL && *s )	{	d.Set_Unix_Time(atoi(s)); Name += "[" + d.Format_ISOCombined() + "]";	}
			}
		}

		//-------------------------------------------------
		if( !Get_DriverID().Cmp("netCDF") )
		{
			if( (s = GDALGetMetadataItem(pBand, "NETCDF_VARNAME"        , 0)) != NULL && *s )	{	Name += "["; Name += s; Name += "]";	}
			if( (s = GDALGetMetadataItem(pBand, "NETCDF_DIMENSION_time" , 0)) != NULL && *s )	{	Name += "["; Name += s; Name += "]";	}
			if( (s = GDALGetMetadataItem(pBand, "NETCDF_DIMENSION_level", 0)) != NULL && *s )	{	Name += "["; Name += s; Name += "]";	}
		}

		//-------------------------------------------------
		if( Name.is_Empty() )
		{
			Name	= Get_Name();

			if( Name.is_Empty() )
			{
				Name	= _TL("Band");
			}

			Name	+= CSG_String::Format(" %0*d", SG_Get_Digit_Count(Get_Count() + 1), i + 1);
		}
	}

	return( Name );
}

//---------------------------------------------------------
CSG_String CSG_GDAL_DataSet::Get_Description(int i)	const
{
	CSG_String		Description;

	GDALRasterBandH	pBand;

	if( is_Reading() && (pBand = GDALGetRasterBand(m_pDataSet, i + 1)) != NULL )
	{
		char	**pMetaData	= GDALGetMetadata(pBand, 0);

		if( pMetaData )
		{
			while( *pMetaData )
			{
				CSG_String	s(*pMetaData);

				Description	+= s + "\n";

				pMetaData++;
			}
		}
	}

	return( Description );
}

//---------------------------------------------------------
bool CSG_GDAL_DataSet::Get_MetaData(int i, CSG_MetaData &MetaData)	const
{
	GDALRasterBandH	pBand;

	if( is_Reading() && (pBand = GDALGetRasterBand(m_pDataSet, i + 1)) != NULL )
	{
		char	**pMetaData	= GDALGetMetadata(pBand, 0);

		if( pMetaData )
		{
			while( *pMetaData )
			{
				CSG_String	s(*pMetaData);

				MetaData.Add_Child(s.BeforeFirst('='), s.AfterFirst('='));

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
	GDALRasterBandH	pBand	= GDALGetRasterBand(m_pDataSet, i + 1);

	return( pBand ? GDALGetMetadataItem(pBand, pszName, 0) : "" );
}

bool CSG_GDAL_DataSet::Get_MetaData_Item(int i, const char *pszName, CSG_String &MetaData)	const
{
	GDALRasterBandH	pBand	= GDALGetRasterBand(m_pDataSet, i + 1);

	if( pBand != NULL )
	{
		const char	*pMetaData	= GDALGetMetadataItem(pBand, pszName, 0);

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
	GDALRasterBandH	pBand	= GDALGetRasterBand(m_pDataSet, i + 1);

	if( !pBand )
	{
		return( NULL );
	}

	//-------------------------------------------------
	TSG_Data_Type	Type	= gSG_GDAL_Drivers.Get_SAGA_Type(GDALGetRasterDataType(pBand));

	CSG_Grid	*pGrid	= SG_Create_Grid(Type, Get_NX(), Get_NY(), Get_Cellsize(), Get_xMin(), Get_yMin());

	if( !pGrid )
	{
		return( NULL );
	}

	//-------------------------------------------------
	int		bSuccess;

	double	zScale	= GDALGetRasterScale (pBand, &bSuccess);	if( !bSuccess || !zScale )	zScale	= 1.0;
	double	zOffset	= GDALGetRasterOffset(pBand, &bSuccess);	if( !bSuccess            )	zOffset	= 0.0;

	pGrid->Set_Name			(Get_Name       (i));
	pGrid->Set_Description	(Get_Description(i));
	pGrid->Set_Unit			(CSG_String(GDALGetRasterUnitType(pBand)));
	pGrid->Set_Scaling		(zScale, zOffset);

	//-------------------------------------------------
	OGRSpatialReferenceH	SRef	= OSRNewSpatialReference(Get_Projection());

	char	*Proj4	= NULL;

	if( OSRExportToProj4(SRef, &Proj4) == OGRERR_NONE )
	{
		pGrid->Get_Projection().Create(Get_Projection(), Proj4);

		CPLFree(Proj4);
	}
	else
	{
		pGrid->Get_Projection().Create(Get_Projection());
	}

	CPLFree(SRef);

	if( pGrid->Get_Projection().is_Okay() == false )
	{
		CSG_String	Data;	int	EPSG;

		if( !Get_MetaData_Item(Data, "EPSG") || !Data.asInt(EPSG) || !pGrid->Get_Projection().Create(EPSG) )
		{
			if( Get_MetaData_Item(Data, "proj4_string") )
			{
				pGrid->Get_Projection().Create(Data, SG_PROJ_FMT_Proj4);
			}
		}
	}

	//-------------------------------------------------
	CSG_MetaData	&MetaData	= pGrid->Get_MetaData();

	MetaData.Add_Child("GDAL_DRIVER", Get_DriverID());

	Get_MetaData(MetaData);

	Get_MetaData(i, *MetaData.Add_Child("Band"));

	//-------------------------------------------------
	double	zNoData	= GDALGetRasterNoDataValue(pBand, &bSuccess);

	if( bSuccess )
	{
		switch( Type )
		{
		default                : pGrid->Set_NoData_Value(   (int)zNoData); break;
		case SG_DATATYPE_Float : pGrid->Set_NoData_Value( (float)zNoData); break;
		case SG_DATATYPE_Double: pGrid->Set_NoData_Value((double)zNoData); break;
		}
	}

	//-------------------------------------------------
	void	*zLine;	GDALDataType	zType;

	switch( Type )
	{
	default                : zLine = SG_Malloc(Get_NX() * sizeof(   int)); zType = GDT_Int32  ; break;
	case SG_DATATYPE_Float : zLine = SG_Malloc(Get_NX() * sizeof( float)); zType = GDT_Float32; break;
	case SG_DATATYPE_Double: zLine = SG_Malloc(Get_NX() * sizeof(double)); zType = GDT_Float64; break;
	}

	for(int y=0; y<Get_NY() && SG_UI_Process_Set_Progress(y, Get_NY()); y++)
	{
		int	yy	= m_bTransform ? y : Get_NY() - 1 - y;

		if( GDALRasterIO(pBand, GF_Read, 0, y, Get_NX(), 1, zLine, Get_NX(), 1, zType, 0, 0) == CE_None )
		{
			for(int x=0; x<Get_NX(); x++)
			{
				switch( Type )
				{
				default                : pGrid->Set_Value(x, yy, ((int    *)zLine)[x], false); break;
				case SG_DATATYPE_Float : pGrid->Set_Value(x, yy, ((float  *)zLine)[x], false); break;
				case SG_DATATYPE_Double: pGrid->Set_Value(x, yy, ((double *)zLine)[x], false); break;
				}
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

	GDALRasterBandH	pBand	= GDALGetRasterBand(m_pDataSet, i + 1);

	//-----------------------------------------------------
	CPLErr	Error	= CE_None;

	double	*zLine	= (double *)SG_Malloc(Get_NX() * sizeof(double));

	for(int y=0, yy=Get_NY()-1; Error==CE_None && y<Get_NY() && SG_UI_Process_Set_Progress(y, Get_NY()); y++, yy--)
	{
		for(int x=0; x<Get_NX(); x++)
		{
			zLine[x]	= pGrid->is_NoData(x, yy) ? noDataValue : pGrid->asDouble(x, yy);
		}

		Error	= GDALRasterIO(pBand, GF_Write, 0, y, Get_NX(), 1, zLine, Get_NX(), 1, GDT_Float64, 0, 0);
	}

	SG_Free(zLine);

	//-----------------------------------------------------
	if( Error != CE_None )
	{
		SG_UI_Msg_Add_Error(CSG_String::Format("%s", _TL("Writing dataset failed.")));

		return( false );
	}

	//-----------------------------------------------------
	GDALSetRasterNoDataValue(pBand, noDataValue);
	GDALSetRasterStatistics (pBand, pGrid->Get_Min(), pGrid->Get_Max(), pGrid->Get_Mean(), pGrid->Get_StdDev());

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

		v[0] = Get_xMin(); v[1] = Get_yMin(); v	= B * v + A; r.Assign(v[0], v[1], v[0], v[1]);
		v[0] = Get_xMin(); v[1] = Get_yMax(); v	= B * v + A; r.Union(CSG_Point(v[0], v[1]));
		v[0] = Get_xMax(); v[1] = Get_yMax(); v	= B * v + A; r.Union(CSG_Point(v[0], v[1]));
		v[0] = Get_xMax(); v[1] = Get_yMin(); v	= B * v + A; r.Union(CSG_Point(v[0], v[1]));

		v[0] = 1; v[1] = 0; v = B * v; double dx = v.Get_Length();
		v[0] = 0; v[1] = 1; v = B * v; double dy = v.Get_Length();

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
bool CSG_GDAL_DataSet::Get_Transformation(CSG_Grid **ppGrid, TSG_Grid_Resampling Interpolation, bool bVerbose)	const
{
	CSG_Grid_System	System;

	if( Get_Transformation(System, bVerbose) )
	{
		return( Get_Transformation(ppGrid, Interpolation, System, bVerbose) );
	}

	return( false );
}

//---------------------------------------------------------
bool CSG_GDAL_DataSet::Get_Transformation(CSG_Grid **ppGrid, TSG_Grid_Resampling Interpolation, const CSG_Grid_System &System, bool bVerbose)	const
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

			vWorld[0]	= pWorld->Get_XMin() + x * pWorld->Get_Cellsize();
			vWorld[1]	= pWorld->Get_YMin() + y * pWorld->Get_Cellsize();

			vImage	= BInv * (vWorld - A);

			if( pImage->Get_Value(vImage[0], vImage[1], z, Interpolation, true) )
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
		for(int i=0; i<pGrids->Get_Grid_Count(); i++)
		{
			if( SG_Data_Type_Get_Size(Type) <= SG_Data_Type_Get_Size(pGrids->Get_Grid(i)->Get_Type()) )
			{
				Type	= pGrids->Get_Grid(i)->Get_Type();
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

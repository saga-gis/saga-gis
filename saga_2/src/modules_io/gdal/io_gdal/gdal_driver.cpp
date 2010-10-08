
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
#include "gdal_driver.h"

#include <gdal_priv.h>


///////////////////////////////////////////////////////////
//														 //
//														 //
//														 //
///////////////////////////////////////////////////////////

//---------------------------------------------------------
CGDAL_Driver	g_GDAL_Driver;


///////////////////////////////////////////////////////////
//														 //
//														 //
//														 //
///////////////////////////////////////////////////////////

//---------------------------------------------------------
CGDAL_Driver::CGDAL_Driver(void)
{
	GDALAllRegister();

	pManager	= GetGDALDriverManager();
}

//---------------------------------------------------------
CGDAL_Driver::~CGDAL_Driver(void)
{
	GDALDestroyDriverManager();	
}


///////////////////////////////////////////////////////////
//														 //
///////////////////////////////////////////////////////////

//---------------------------------------------------------
int CGDAL_Driver::Get_Count(void)
{
	return( pManager->GetDriverCount() );
}

//---------------------------------------------------------
const char * CGDAL_Driver::Get_Name(int Index)
{
	return( Get_Driver(Index)->GetMetadataItem(GDAL_DMD_LONGNAME) );
}

//---------------------------------------------------------
const char * CGDAL_Driver::Get_Description(int Index)
{
	return( Get_Driver(Index)->GetDescription() );
}

//---------------------------------------------------------
bool CGDAL_Driver::is_ReadOnly(int Index)
{
	return( Index >= 0 && Index < Get_Count() ? CSLFetchBoolean(Get_Driver(Index)->GetMetadata(), GDAL_DCAP_CREATE, false) == 0 : false );
}

//---------------------------------------------------------
GDALDriver * CGDAL_Driver::Get_Driver(int Index)
{
	return( (GDALDriver *)GDALGetDriver(Index) );
}

//---------------------------------------------------------
GDALDriver * CGDAL_Driver::Get_Driver(const char *Name)
{
	return( (GDALDriver *)GDALGetDriverByName(Name) );
}


///////////////////////////////////////////////////////////
//														 //
///////////////////////////////////////////////////////////

//---------------------------------------------------------
int CGDAL_Driver::Get_GDAL_Type(TSG_Data_Type Type)
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
TSG_Data_Type CGDAL_Driver::Get_Grid_Type(int Type)
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

//---------------------------------------------------------
TSG_Data_Type CGDAL_Driver::Get_Grid_Type(CSG_Parameter_Grid_List *pGrids)
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
CGDAL_System::CGDAL_System(void)
{
	m_pDataSet	= NULL;

	m_TF_A.Create(2);
	m_TF_B.Create(2, 2);
}

//---------------------------------------------------------
CGDAL_System::CGDAL_System(const CSG_String &File_Name, int ioAccess)
{
	m_pDataSet	= NULL;

	m_TF_A.Create(2);
	m_TF_B.Create(2, 2);

	Open_Read(File_Name);
}

//---------------------------------------------------------
CGDAL_System::~CGDAL_System(void)
{
	Close();
}


///////////////////////////////////////////////////////////
//														 //
///////////////////////////////////////////////////////////

//---------------------------------------------------------
bool CGDAL_System::Open_Read(const CSG_String &File_Name)
{
	Close();

	if( (m_pDataSet = (GDALDataset *)GDALOpen(SG_STR_SGTOMB(File_Name), GA_ReadOnly)) == NULL )
	{
		return( false );
	}

	//-----------------------------------------------------
	double	Transform[6];

	m_Access	= IO_READ;

	m_NX		= m_pDataSet->GetRasterXSize();
	m_NY		= m_pDataSet->GetRasterYSize();

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

//---------------------------------------------------------
bool CGDAL_System::Open_Write(const CSG_String &File_Name, const CSG_String &Driver, TSG_Data_Type Type, int NBands, const CSG_Grid_System &System, const CSG_Projection &Projection)
{
	char		**pOptions	= NULL;
	GDALDriver	*pDriver;

	Close();

	//--------------------------------------------------------
	if( (pDriver = g_GDAL_Driver.Get_Driver(SG_STR_SGTOMB(Driver))) == NULL )
	{
		SG_UI_Msg_Add_Error(_TL("GeoTIFF driver not found."));

		return( false );
	}

	if( CSLFetchBoolean(pDriver->GetMetadata(), GDAL_DCAP_CREATE, false) == false )
	{
		SG_UI_Msg_Add_Error(_TL("Driver does not support file creation."));

		return( false );
	}

	if( (m_pDataSet = pDriver->Create(SG_STR_SGTOMB(File_Name), System.Get_NX(), System.Get_NY(), NBands, (GDALDataType)g_GDAL_Driver.Get_GDAL_Type(Type), pOptions)) == NULL )
	{
		SG_UI_Msg_Add_Error(_TL("Could not create dataset."));

		return( false );
	}

	//--------------------------------------------------------
	m_Access	= IO_WRITE;

	if( Projection.is_Okay() )
	{
		m_pDataSet->SetProjection(SG_STR_SGTOMB(Projection.Get_WKT()));
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
bool CGDAL_System::Close(void)
{
	if( m_pDataSet )
	{
		GDALClose(m_pDataSet);

		m_pDataSet	= NULL;
	}

	m_Access	= IO_CLOSED;

	return( true );
}


///////////////////////////////////////////////////////////
//														 //
///////////////////////////////////////////////////////////

//---------------------------------------------------------
GDALDriver * CGDAL_System::Get_Driver(void)	const
{
	return( m_pDataSet ? m_pDataSet->GetDriver() : NULL );
}

//---------------------------------------------------------
const char * CGDAL_System::Get_Projection(void)	const
{
	return( m_pDataSet && m_pDataSet->GetProjectionRef() ? m_pDataSet->GetProjectionRef() : "" );
}

//---------------------------------------------------------
const char * CGDAL_System::Get_Name(void)	const
{
	return( m_pDataSet ? m_pDataSet->GetMetadataItem(GDAL_DMD_LONGNAME) : "" );
}

//---------------------------------------------------------
const char * CGDAL_System::Get_Description(void)	const
{
	return( m_pDataSet ? m_pDataSet->GetDescription() : "" );
}

//---------------------------------------------------------
const char ** CGDAL_System::Get_MetaData(const char *pszDomain)	const
{
	return( m_pDataSet ? (const char **)m_pDataSet->GetMetadata(pszDomain) : NULL );
}

//---------------------------------------------------------
const char * CGDAL_System::Get_MetaData_Item(const char *pszName, const char *pszDomain)	const
{
	return( m_pDataSet ? m_pDataSet->GetMetadataItem(pszName, pszDomain) : "" );
}


///////////////////////////////////////////////////////////
//														 //
///////////////////////////////////////////////////////////

//---------------------------------------------------------
int CGDAL_System::Get_Count(void)	const
{
	return( m_pDataSet ? m_pDataSet->GetRasterCount() : 0 );
}

//---------------------------------------------------------
CSG_Grid * CGDAL_System::Read_Band(int i)
{
	GDALRasterBand	*pBand;

	if( is_Reading() && (pBand = m_pDataSet->GetRasterBand(i + 1)) != NULL )
	{
		CSG_String	Name, Description;

		CSG_Grid	*pGrid	= SG_Create_Grid(g_GDAL_Driver.Get_Grid_Type(pBand->GetRasterDataType()),
			Get_NX(), Get_NY(), Get_Cellsize(), Get_xMin(), Get_yMin()
		);

		//-------------------------------------------------
		char	**pMetaData	= pBand->GetMetadata() + 0;

		if( pMetaData )
		{
			while( *pMetaData )
			{
				Description	+= SG_STR_MBTOSG(*pMetaData);
				Description	+= SG_T("\n");

				pMetaData++;
			}
		}

		//-------------------------------------------------
		const char	*s;

		if( (s = pBand->GetMetadataItem("GRIB_COMMENT")) != NULL && *s )
		{
			Name	= SG_STR_MBTOSG(s);
		}
		else if( (s = pBand->GetMetadataItem(GDAL_DMD_LONGNAME)) != NULL && *s )
		{
			Name	= SG_STR_MBTOSG(s);
		}
		else
		{
			Name.Printf(SG_T("%d"), i + 1);
		}

		//-------------------------------------------------
		pGrid->Set_Name			(Name);
		pGrid->Set_Description	(Description);
		pGrid->Set_Unit			(SG_STR_MBTOSG(pBand->GetUnitType()));
		pGrid->Set_NoData_Value	(pBand->GetNoDataValue());
		pGrid->Set_ZFactor		(pBand->GetScale());

		//-------------------------------------------------
		double		zMin, *zLine;

		zLine	= (double *)SG_Malloc(Get_NX() * sizeof(double));
		zMin	= pBand->GetOffset();

		for(int y=0; y<Get_NY() && SG_UI_Process_Set_Progress(y, Get_NY()); y++)
		{
			int	yy	= m_bTransform ? y : Get_NY() - 1 - y;

			if( pBand->RasterIO(GF_Read, 0, y, Get_NX(), 1, zLine, Get_NX(), 1, GDT_Float64, 0, 0) == CE_None )
			{
				for(int x=0; x<Get_NX(); x++)
				{
				//	double	NaN	= 0.0;	NaN	= -1.0 / NaN;	if( NaN == zLine[x] )	pGrid->Set_NoData(x, yy); else

					pGrid->Set_Value (x, yy, zMin + zLine[x]);
				}
			}
		}

		SG_Free(zLine);

		return( pGrid );
	}

	return( NULL );
}

//---------------------------------------------------------
bool CGDAL_System::Write_Band(int i, CSG_Grid *pGrid)
{
	if( !m_pDataSet || !pGrid || pGrid->Get_NX() != Get_NX() || pGrid->Get_NY() != Get_NY() || i < 0 || i >= Get_Count() )
	{
		return( false );
	}

	GDALRasterBand	*pBand	= m_pDataSet->GetRasterBand(i + 1);

	double	*zLine	= (double *)SG_Malloc(Get_NX() * sizeof(double));

	for(int y=0; y<Get_NY() && SG_UI_Process_Set_Progress(y, Get_NY()); y++)
	{
		for(int x=0; x<Get_NX(); x++)
		{
			zLine[x]	= pGrid->asDouble(x, Get_NY() - 1 - y);
		}

		pBand->RasterIO(GF_Write, 0, y, Get_NX(), 1, zLine, Get_NX(), 1, GDT_Float64, 0, 0);
	}

	SG_Free(zLine);

	return( true );
}


///////////////////////////////////////////////////////////
//														 //
//														 //
//														 //
///////////////////////////////////////////////////////////

//---------------------------------------------------------


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

	//-----------------------------------------------------
	for(int i=0; i<Get_Count(); i++)
    {
    }
}

//---------------------------------------------------------
CGDAL_Driver::~CGDAL_Driver(void)
{
	GDALDestroyDriverManager();	
}

//---------------------------------------------------------
TSG_Data_Type CGDAL_Driver::Get_Grid_Type(GDALDataType Type)
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
GDALDataType CGDAL_Driver::Get_GDAL_Type(TSG_Data_Type Type)
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
GDALDataType CGDAL_Driver::Get_GDAL_Type(CSG_Parameter_Grid_List *pGrids)
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

	return( Get_GDAL_Type(Type) );
}

//---------------------------------------------------------
bool CGDAL_Driver::Set_Transform(GDALDataset *pDataset, CSG_Grid_System *pSystem)
{
	if( pDataset && pSystem )
	{
		double	Transform[6]	=
		{
			pSystem->Get_XMin() - 0.5 * pSystem->Get_Cellsize(), pSystem->Get_Cellsize(), 0.0,
			pSystem->Get_YMax() + 0.5 * pSystem->Get_Cellsize(), 0.0, -pSystem->Get_Cellsize()
		};

		pDataset->SetGeoTransform(Transform);

		return( true );
	}

	return( false );
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

	Create(File_Name, ioAccess);
}

//---------------------------------------------------------
CGDAL_System::~CGDAL_System(void)
{
	Destroy();
}


///////////////////////////////////////////////////////////
//														 //
///////////////////////////////////////////////////////////

//---------------------------------------------------------
bool CGDAL_System::Create(const CSG_String &File_Name, int ioAccess)
{
	Destroy();

	//-----------------------------------------------------
	if( ioAccess == IO_READ )
	{
		if( (m_pDataSet = (GDALDataset *)GDALOpen(SG_STR_SGTOMB(File_Name), GA_ReadOnly)) != NULL )
		{
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
	}

	//-----------------------------------------------------
	else if( ioAccess == IO_WRITE )
	{
	}

	Destroy();

	return( false );
}

//---------------------------------------------------------
bool CGDAL_System::Destroy(void)
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

		while( *pMetaData )
		{
			Description	+= SG_STR_MBTOSG(*pMetaData);
			Description	+= SG_T("\n");

			pMetaData++;
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


///////////////////////////////////////////////////////////
//														 //
//														 //
//														 //
///////////////////////////////////////////////////////////

//---------------------------------------------------------

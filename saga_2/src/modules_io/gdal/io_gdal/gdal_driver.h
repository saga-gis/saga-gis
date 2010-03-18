
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
//                    gdal_driver.h                      //
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
#ifndef HEADER_INCLUDED__gdal_driver_H
#define HEADER_INCLUDED__gdal_driver_H

//---------------------------------------------------------
#include "MLB_Interface.h"

#include <gdal_priv.h>


///////////////////////////////////////////////////////////
//														 //
//														 //
//														 //
///////////////////////////////////////////////////////////

//---------------------------------------------------------
class CGDAL_Driver
{
public:
	CGDAL_Driver(void);
	virtual ~CGDAL_Driver(void);

	int						Get_Count		(void)				{	return( pManager->GetDriverCount() );	}
	const char *			Get_Name		(int Index)			{	return( Get_Driver(Index)->GetMetadataItem(GDAL_DMD_LONGNAME) );	}
	const char *			Get_Description	(int Index)			{	return( Get_Driver(Index)->GetDescription() );	}
	GDALDriver *			Get_Driver		(int Index)			{	return( (GDALDriver *)GDALGetDriver(Index) );	}
	GDALDriver *			Get_Driver		(const char *Name)	{	return( (GDALDriver *)GDALGetDriverByName(Name) );	}


	static TSG_Data_Type	Get_Grid_Type	(GDALDataType  Type);
	static GDALDataType		Get_GDAL_Type	(TSG_Data_Type Type);
	static GDALDataType		Get_GDAL_Type	(CSG_Parameter_Grid_List *pGrids);
	static bool				Set_Transform	(GDALDataset *pDataset, CSG_Grid_System *pSystem);


private:

	GDALDriverManager		*pManager;

};

//---------------------------------------------------------
#define IO_CLOSED		0x00
#define IO_READ		0x01
#define IO_WRITE		0x02
#define IO_READWRITE	(IO_READ|IO_WRITE)

//---------------------------------------------------------
class CGDAL_System
{
public:
	CGDAL_System(void);
	CGDAL_System(const CSG_String &File_Name, int ioAccess = IO_READ);
	virtual ~CGDAL_System(void);

	bool					Create			(const CSG_String &File_Name, int ioAccess = IO_READ);
	bool					Destroy			(void);

	bool					is_Okay			(void)	const	{	return( m_pDataSet != NULL );	}
	bool					is_Reading		(void)	const	{	return( m_pDataSet != NULL && m_Access & IO_READ );	 }
	bool					is_Writing		(void)	const	{	return( m_pDataSet != NULL && m_Access & IO_WRITE );	}

	int						Get_NX			(void)	const	{	return( m_NX );			}
	int						Get_NY			(void)	const	{	return( m_NY );			}
	double					Get_xMin		(void)	const	{	return( m_xMin );		}
	double					Get_yMin		(void)	const	{	return( m_yMin );		}
	double					Get_Cellsize	(void)	const	{	return( m_Cellsize );	}

	bool					Needs_Transform	(void)	const	{	return( m_bTransform );	}
	void					Get_Transform	(CSG_Vector &A, CSG_Matrix &B)	const	{	A	= m_TF_A;	B	= m_TF_B;	}

	GDALDriver *			Get_Driver		(void)	const	{	return( m_pDataSet ? m_pDataSet->GetDriver() : NULL );	}
	const char *			Get_Projection	(void)	const	{	return( m_pDataSet && m_pDataSet->GetProjectionRef() ? m_pDataSet->GetProjectionRef() : "" );	}

	int						Get_Count		(void)	const	{	return( m_pDataSet ? m_pDataSet->GetRasterCount() : 0    );	}
	CSG_Grid *				Read_Band		(int i);

	GDALDataset *			Get_DataSet		(void)	{	return( m_pDataSet );	}


private:

	bool					m_bTransform;

	int						m_Access, m_NX, m_NY;

	double					m_xMin, m_yMin, m_Cellsize;

	CSG_Vector				m_TF_A;

	CSG_Matrix				m_TF_B, m_TF_BInv;

	GDALDataset				*m_pDataSet;


public:

	bool					to_World		(double x, double y, double &xWorld, double &yWorld)
	{
		if( m_pDataSet )
		{
			xWorld	= m_TF_A[0] + x * m_TF_B[0][1] + y * m_TF_B[0][1];
			yWorld	= m_TF_A[1] + x * m_TF_B[1][0] + y * m_TF_B[1][1];

			return( true );
		}

		return( false );
	}

	bool					from_World		(double xWorld, double yWorld, double &x, double &y)
	{
		if( m_pDataSet )
		{
			xWorld	-= m_TF_A[0];
			yWorld	-= m_TF_A[1];

			x	= xWorld * m_TF_BInv[0][0] + yWorld * m_TF_BInv[0][1];
			y	= xWorld * m_TF_BInv[1][0] + yWorld * m_TF_BInv[1][1];

			return( true );
		}

		return( false );
	}

};

//---------------------------------------------------------
extern CGDAL_Driver	g_GDAL_Driver;


///////////////////////////////////////////////////////////
//														 //
//														 //
//														 //
///////////////////////////////////////////////////////////

//---------------------------------------------------------
#endif // #ifndef HEADER_INCLUDED__gdal_driver_H

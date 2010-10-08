
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

	int							Get_Count			(void);
	const char *				Get_Name			(int Index);
	const char *				Get_Description		(int Index);
	class GDALDriver *			Get_Driver			(int Index);
	class GDALDriver *			Get_Driver			(const char *Name);

	bool						is_ReadOnly			(int Index);

	static int					Get_GDAL_Type		(TSG_Data_Type Type);
	static TSG_Data_Type		Get_Grid_Type		(int           Type);
	static TSG_Data_Type		Get_Grid_Type		(CSG_Parameter_Grid_List *pGrids);


private:

	class GDALDriverManager		*pManager;

};


///////////////////////////////////////////////////////////
//														 //
///////////////////////////////////////////////////////////

//---------------------------------------------------------
#define IO_CLOSED		0x00
#define IO_READ			0x01
#define IO_WRITE		0x02
#define IO_READWRITE	(IO_READ|IO_WRITE)

//---------------------------------------------------------
class CGDAL_System
{
public:
	CGDAL_System(void);
	CGDAL_System(const CSG_String &File_Name, int ioAccess = IO_READ);
	virtual ~CGDAL_System(void);

	bool						Open_Read			(const CSG_String &File_Name);
	bool						Open_Write			(const CSG_String &File_Name, const CSG_String &Driver, TSG_Data_Type Type, int NBands, const CSG_Grid_System &System, const CSG_Projection &Projection);
	bool						Close				(void);

	bool						is_Okay				(void)	const	{	return( m_pDataSet != NULL );	}
	bool						is_Reading			(void)	const	{	return( m_pDataSet != NULL && m_Access & IO_READ );	 }
	bool						is_Writing			(void)	const	{	return( m_pDataSet != NULL && m_Access & IO_WRITE );	}

	int							Get_NX				(void)	const	{	return( m_NX );			}
	int							Get_NY				(void)	const	{	return( m_NY );			}
	double						Get_xMin			(void)	const	{	return( m_xMin );		}
	double						Get_yMin			(void)	const	{	return( m_yMin );		}
	double						Get_Cellsize		(void)	const	{	return( m_Cellsize );	}

	bool						Needs_Transform		(void)	const	{	return( m_bTransform );	}
	void						Get_Transform		(CSG_Vector &A, CSG_Matrix &B)	const	{	A	= m_TF_A;	B	= m_TF_B;	}

	class GDALDriver *			Get_Driver			(void)	const;
	const char *				Get_Name			(void)	const;
	const char *				Get_Description		(void)	const;
	const char *				Get_Projection		(void)	const;
	const char **				Get_MetaData		(const char *pszDomain = "")	const;
	const char *				Get_MetaData_Item	(const char *pszName, const char *pszDomain = "")	const;

	int							Get_Count			(void)	const;
	CSG_Grid *					Read_Band			(int i);
	bool						Write_Band			(int i, CSG_Grid *pGrid);


private:

	bool						m_bTransform;

	int							m_Access, m_NX, m_NY;

	double						m_xMin, m_yMin, m_Cellsize;

	CSG_Vector					m_TF_A;

	CSG_Matrix					m_TF_B, m_TF_BInv;

	class GDALDataset			*m_pDataSet;


public:

	bool						to_World			(double x, double y, double &xWorld, double &yWorld)
	{
		if( m_pDataSet )
		{
			xWorld	= m_TF_A[0] + x * m_TF_B[0][1] + y * m_TF_B[0][1];
			yWorld	= m_TF_A[1] + x * m_TF_B[1][0] + y * m_TF_B[1][1];

			return( true );
		}

		return( false );
	}

	bool						from_World			(double xWorld, double yWorld, double &x, double &y)
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

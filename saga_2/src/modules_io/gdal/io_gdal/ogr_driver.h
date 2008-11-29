
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
//                     ogr_driver.h                      //
//                                                       //
//            Copyright (C) 2008 O. Conrad               //
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
#ifndef HEADER_INCLUDED__ogr_driver_H
#define HEADER_INCLUDED__ogr_driver_H

//---------------------------------------------------------
#include "MLB_Interface.h"

#include <gdal_priv.h>
#include <ogrsf_frmts.h>


///////////////////////////////////////////////////////////
//														 //
//														 //
//														 //
///////////////////////////////////////////////////////////

//---------------------------------------------------------
class COGR_Driver
{
public:
	COGR_Driver(void);
	virtual ~COGR_Driver(void);

	int							Get_Count		(void)				{	return( m_pManager->GetDriverCount() );	}
	CSG_String					Get_Name		(int iDriver)		{	return( SG_STR_MBTOSG(m_pManager->GetDriver(iDriver)->GetName()) );	}
	CSG_String					Get_Description	(int iDriver);
	OGRSFDriver *				Get_Driver		(int iDriver)		{	return( m_pManager->GetDriver(iDriver) );			}
	OGRSFDriver *				Get_Driver		(const CSG_String &Name);

	bool						Can_Read		(int iDriver);
	bool						Can_Write		(int iDriver);

	static TSG_Shape_Type		Get_Type		(OGRwkbGeometryType   Type);
	static OGRwkbGeometryType	Get_Type		(TSG_Shape_Type       Type);
	static TSG_Table_Field_Type	Get_Type		(OGRFieldType         Type);
	static OGRFieldType			Get_Type		(TSG_Table_Field_Type Type);


private:

	OGRSFDriverRegistrar		*m_pManager;

};

//---------------------------------------------------------
class COGR_DataSource
{
public:
	COGR_DataSource(void);
	COGR_DataSource(const CSG_String &File);
	COGR_DataSource(const CSG_String &File, const CSG_String &DriverName);
	virtual ~COGR_DataSource(void);

	bool						Create			(const CSG_String &File);
	bool						Create			(const CSG_String &File, const CSG_String &DriverName);
	bool						Destroy			(void);

	int							Get_Count		(void);
	OGRLayer *					Get_Layer		(int iLayer);
	TSG_Shape_Type				Get_Type		(int iLayer);

	CSG_Shapes *				Read_Shapes		(int iLayer);
	bool						Write_Shapes	(CSG_Shapes *pShapes);


private:

	OGRDataSource				*m_pDataSource;


	bool						_Read_Geometry	(CSG_Shape *pShape, OGRGeometry *pGeometry);
	bool						_Read_Line		(CSG_Shape *pShape, OGRLineString *pLine);
	bool						_Read_Polygon	(CSG_Shape *pShape, OGRPolygon *pPolygon);

	bool						_Write_Geometry	(CSG_Shape *pShape, OGRFeature *pFeature);
	bool						_Write_Line		(CSG_Shape *pShape, OGRLineString *pLine, int iPart);

};

//---------------------------------------------------------
extern COGR_Driver	g_OGR_Driver;


///////////////////////////////////////////////////////////
//														 //
//														 //
//														 //
///////////////////////////////////////////////////////////

//---------------------------------------------------------
#endif // #ifndef HEADER_INCLUDED__ogr_driver_H

/**********************************************************
 * Version $Id: ogr_driver.h 1921 2014-01-09 10:24:11Z oconrad $
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
#ifndef HEADER_INCLUDED__ogr_driver_H
#define HEADER_INCLUDED__ogr_driver_H

//---------------------------------------------------------
#include "MLB_Interface.h"

#include <ogr_core.h>


///////////////////////////////////////////////////////////
//														 //
//														 //
//														 //
///////////////////////////////////////////////////////////

//---------------------------------------------------------
typedef enum ESG_OGR_WKB_Type_Choice
{
	OGR_WKB_AUTOMATIC = 0,
	OGR_WKB_POINT             , OGR_WKB_POINT25D             ,
	OGR_WKB_MULTIPOINT        , OGR_WKB_MULTIPOINT25D        ,
	OGR_WKB_LINESTRING        , OGR_WKB_LINESTRING25D        ,
	OGR_WKB_MULTILINESTRING   , OGR_WKB_MULTILINESTRING25D   ,
	OGR_WKB_POLYGON           , OGR_WKB_POLYGON25D           ,
	OGR_WKB_MULTIPOLYGON      , OGR_WKB_MULTIPOLYGON25D      ,
//	OGR_WKB_GEOMETRYCOLLECTION, OGR_WKB_GEOMETRYCOLLECTION25D,
	OGR_WKB_TYPE_Count
}
TSG_OGR_WKB_Type_Choice;

//---------------------------------------------------------
OGRwkbGeometryType	SG_Get_OGR_WKB_Type_Choice_Key	(int Type);
CSG_String			SG_Get_OGR_WKB_Type_Choice_Name	(int Type);
CSG_String			SG_Get_OGR_WKB_Type_Choices		(void);


///////////////////////////////////////////////////////////
//														 //
///////////////////////////////////////////////////////////

//---------------------------------------------------------
class CSG_OGR_Drivers
{
public:
	CSG_OGR_Drivers(void);
	virtual ~CSG_OGR_Drivers(void);

	CSG_String					Get_Version			(void)						const;

	int							Get_Count			(void)						const;

#ifdef USE_GDAL_V2
	class GDALDriver *			Get_Driver			(const CSG_String &Name)	const;
	class GDALDriver *			Get_Driver			(int Index)					const;
#else
	class OGRSFDriver *			Get_Driver			(const CSG_String &Name)	const;
	class OGRSFDriver *			Get_Driver			(int Index)					const;
#endif

	CSG_String					Get_Name			(int Index)					const;
	CSG_String					Get_Description		(int Index)					const;
	CSG_String					Get_Extension		(int Index)					const;

	bool						is_Vector			(int Index)					const;

	bool						Can_Read			(int Index)					const;
	bool						Can_Write			(int Index)					const;

	static TSG_Vertex_Type		Get_Vertex_Type		(OGRwkbGeometryType Type);
	static TSG_Shape_Type		Get_Shape_Type		(OGRwkbGeometryType Type);
	static int					Get_Shape_Type		(    TSG_Shape_Type Type, bool bZ);

	static TSG_Data_Type		Get_Data_Type		(int            Type);
	static int					Get_Data_Type		(TSG_Data_Type  Type);


private:

#ifdef USE_GDAL_V2
	class GDALDriverManager		*m_pDrivers;
#else
	class OGRSFDriverRegistrar	*m_pDrivers;
#endif

};

//---------------------------------------------------------
const CSG_OGR_Drivers &	SG_Get_OGR_Drivers	(void);


///////////////////////////////////////////////////////////
//														 //
///////////////////////////////////////////////////////////

//---------------------------------------------------------
class CSG_OGR_DataSet
{
public:
	CSG_OGR_DataSet(void);
	CSG_OGR_DataSet(const CSG_String &File);
	CSG_OGR_DataSet(const CSG_String &File, const CSG_String &DriverName);
	virtual ~CSG_OGR_DataSet(void);

	bool						Create				(const CSG_String &File);
	bool						Create				(const CSG_String &File, const CSG_String &DriverName);
	bool						Destroy				(void);

	CSG_String					Get_DriverID		(void)			const;
	CSG_String					Get_Description		(void)			const;
	CSG_String					Get_Description		(int iLayer)	const;

	int							Get_Count			(void)			const;
	class OGRLayer *			Get_Layer			(int iLayer)	const;
	TSG_Shape_Type				Get_Type			(int iLayer)	const;
	TSG_Vertex_Type				Get_Coordinate_Type	(int iLayer)	const;
	CSG_Projection				Get_Projection		(int iLayer)	const;

	CSG_Shapes *				Read				(int iLayer, int iGeomTypeChoice);
	bool						Write				(CSG_Shapes *pShapes);


private:

#ifdef USE_GDAL_V2
	class GDALDataset			*m_pDataSet;
#else
	class OGRDataSource			*m_pDataSet;
#endif


	bool						_Read_Geometry		(CSG_Shape *pShape, class OGRGeometry *pGeometry);
	bool						_Read_Line			(CSG_Shape *pShape, class OGRLineString *pLine);
	bool						_Read_Polygon		(CSG_Shape *pShape, class OGRPolygon *pPolygon);

	bool						_Write_Geometry		(CSG_Shape *pShape, class OGRFeature *pFeature, bool bZ);
	bool						_Write_Line			(CSG_Shape *pShape, class OGRLineString *pLine, int iPart, bool bZ);

};


///////////////////////////////////////////////////////////
//														 //
//														 //
//														 //
///////////////////////////////////////////////////////////

//---------------------------------------------------------
#endif // #ifndef HEADER_INCLUDED__ogr_driver_H

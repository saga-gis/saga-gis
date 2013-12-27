/**********************************************************
 * Version $Id$
 *********************************************************/

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


//---------------------------------------------------------
typedef enum ESG_Geom_Type_Choice_Key
{
	AUTOMATIC				= 0,
	WKBPOINT,
	WKBPOINT25D,
	WKBMULTIPOINT,
	WKBMULTIPOINT25D,
	WKBLINESTRING,
	WKBLINESTRING25D,
	WKBMULTILINESTRING,
	WKBMULTILINESTRING25D,
	WKBPOLYGON,
	WKBPOLYGON25D,
	WKBMULTIPOLYGON,
	WKBMULTIPOLYGON25D,
	WKBGEOMETRYCOLLECTION,
	WKBGEOMETRYCOLLECTION25D,
	GEOM_TYPE_KEY_Count
}
TSG_Geom_Type_Choice_Key;


//---------------------------------------------------------
const SG_Char	gSG_Geom_Type_Choice_Key_Name[GEOM_TYPE_KEY_Count][32]	=
{
	SG_T("automatic"),
	SG_T("wkbPoint"),
	SG_T("wkbPoint25D"),
	SG_T("wkbMultiPoint"),
	SG_T("wkbMultiPoint25D"),
	SG_T("wkbLineString"),
	SG_T("wkbLineString25D"),
	SG_T("wkbMultiLineString"),
	SG_T("wkbMultiLineString25D"),
	SG_T("wkbPolygon"),
	SG_T("wkbPolygon25D"),
	SG_T("wkbMultiPolygon"),
	SG_T("wkbMultiPolygon25D"),
	SG_T("wkbGeometryCollection"),
	SG_T("wkbGeometryCollection25D")
};

///////////////////////////////////////////////////////////
//														 //
//														 //
//														 //
///////////////////////////////////////////////////////////

//---------------------------------------------------------
class io_gdal_EXPORT CSG_OGR_Drivers
{
public:
	CSG_OGR_Drivers(void);
	virtual ~CSG_OGR_Drivers(void);

	int							Get_Count			(void)						const;

	class OGRSFDriver *			Get_Driver			(const CSG_String &Name)	const;
	class OGRSFDriver *			Get_Driver			(int Index)					const;

	CSG_String					Get_Name			(int Index)					const;
	CSG_String					Get_Description		(int Index)					const;

	bool						Can_Read			(int Index)					const;
	bool						Can_Write			(int Index)					const;

	static TSG_Vertex_Type		Get_Vertex_Type		(int Type);
	static TSG_Shape_Type		Get_Shape_Type		(int Type);
	static int					Get_Shape_Type		(TSG_Shape_Type Type, bool bZ);

	static TSG_Data_Type		Get_Data_Type		(int            Type);
	static int					Get_Data_Type		(TSG_Data_Type  Type);


private:

	class OGRSFDriverRegistrar	*m_pDrivers;

};

//---------------------------------------------------------
io_gdal_EXPORT const CSG_OGR_Drivers &	SG_Get_OGR_Drivers	(void);


///////////////////////////////////////////////////////////
//														 //
///////////////////////////////////////////////////////////

//---------------------------------------------------------
class io_gdal_EXPORT CSG_OGR_DataSource
{
public:
	CSG_OGR_DataSource(void);
	CSG_OGR_DataSource(const CSG_String &File);
	CSG_OGR_DataSource(const CSG_String &File, const CSG_String &DriverName);
	virtual ~CSG_OGR_DataSource(void);

	bool						Create				(const CSG_String &File);
	bool						Create				(const CSG_String &File, const CSG_String &DriverName);
	bool						Destroy				(void);

	int							Get_Count			(void);
	class OGRLayer *			Get_Layer			(int iLayer);
	TSG_Shape_Type				Get_Type			(int iLayer);
	TSG_Vertex_Type				Get_Coordinate_Type	(int iLayer);

	CSG_Shapes *				Read				(int iLayer, int iGeomTypeChoice);
	bool						Write				(CSG_Shapes *pShapes, const CSG_String &DriverName);


private:

	class OGRDataSource			*m_pDataSource;


	int							_Get_GeomType_Choice(int iGeomTypeChoice);

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


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

	static TSG_Shape_Type		Get_Shape_Type		(int            Type);
	static int					Get_Shape_Type		(TSG_Shape_Type Type);

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

	CSG_Shapes *				Read				(int iLayer);
	bool						Write				(CSG_Shapes *pShapes);


private:

	class OGRDataSource			*m_pDataSource;


	bool						_Read_Geometry		(CSG_Shape *pShape, class OGRGeometry *pGeometry);
	bool						_Read_Line			(CSG_Shape *pShape, class OGRLineString *pLine);
	bool						_Read_Polygon		(CSG_Shape *pShape, class OGRPolygon *pPolygon);

	bool						_Write_Geometry		(CSG_Shape *pShape, class OGRFeature *pFeature);
	bool						_Write_Line			(CSG_Shape *pShape, class OGRLineString *pLine, int iPart);

};


///////////////////////////////////////////////////////////
//														 //
//														 //
//														 //
///////////////////////////////////////////////////////////

//---------------------------------------------------------
#endif // #ifndef HEADER_INCLUDED__ogr_driver_H

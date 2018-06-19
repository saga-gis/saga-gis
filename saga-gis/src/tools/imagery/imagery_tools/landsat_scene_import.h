
///////////////////////////////////////////////////////////
//                                                       //
//                         SAGA                          //
//                                                       //
//      System for Automated Geoscientific Analyses      //
//                                                       //
//                    Module Library:                    //
//                     image_tools                       //
//                                                       //
//-------------------------------------------------------//
//                                                       //
//                landsat_scene_import.h                 //
//                                                       //
//                 Olaf Conrad (C) 2017                  //
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
//    e-mail:     oconrad@saga-gis.org                   //
//                                                       //
//    contact:    Olaf Conrad                            //
//                Institute of Geography                 //
//                University of Hamburg                  //
//                Germany                                //
//                                                       //
///////////////////////////////////////////////////////////

//---------------------------------------------------------


///////////////////////////////////////////////////////////
//														 //
//                                                       //
//														 //
///////////////////////////////////////////////////////////

//---------------------------------------------------------
#ifndef HEADER_INCLUDED__landsat_scene_import_H
#define HEADER_INCLUDED__landsat_scene_import_H


///////////////////////////////////////////////////////////
//														 //
//														 //
//														 //
///////////////////////////////////////////////////////////

//---------------------------------------------------------
#include <saga_api/saga_api.h>


///////////////////////////////////////////////////////////
//														 //
//														 //
//														 //
///////////////////////////////////////////////////////////

//---------------------------------------------------------
enum
{
	SENSOR_MSS	= 0,
	SENSOR_TM,
	SENSOR_ETM,
	SENSOR_OLI_TIRS,
	SENSOR_UNKNOWN
};


///////////////////////////////////////////////////////////
//														 //
///////////////////////////////////////////////////////////

//---------------------------------------------------------
class CLandsat_Scene_Import : public CSG_Tool
{
public:
	CLandsat_Scene_Import(void);

	virtual CSG_String		Get_MenuPath			(void)	{	return( _TL("A:File|Satellite Imagery") );	}


protected:

	virtual int				On_Parameters_Enable	(CSG_Parameters *pParameters, CSG_Parameter *pParameter);

	virtual bool			On_Execute				(void);


private:

	bool					is_Panchromatic			(int Sensor, int Band);
	bool					is_Multispectral		(int Sensor, int Band);
	bool					is_Cirrus				(int Sensor, int Band);
	bool					is_Thermal				(int Sensor, int Band);

	bool					Load_Metadata			(CSG_MetaData &Metadata, const CSG_String &File);
	bool					Load_Metadata			(const CSG_String &Line, CSG_String &Key, CSG_String &Value);

	int						Get_Info_Version		(const CSG_MetaData &Metadata);
	int						Get_Info_Sensor			(const CSG_MetaData &Metadata);

	bool					Get_Info				(const CSG_MetaData &Metadata, CSG_Strings &File_Bands, CSG_Table &Info_Bands, CSG_MetaData &Info_Scene);
	bool					Get_Info_Band			(const CSG_MetaData &Metadata, int Version, int Sensor, int Band, int Key, CSG_String &Value);
	bool					Set_Info_Band			(int Sensor, int Band, CSG_Table_Record &Info);

	CSG_Grid *				Load_Band				(const CSG_String &File);

	bool					Get_Float				(CSG_Grid *pBand, CSG_Grid &DN);
	bool					Get_Radiance			(CSG_Grid *pBand, const CSG_Table_Record &Info_Band);
	bool					Get_Reflectance			(CSG_Grid *pBand, const CSG_Table_Record &Info_Band, double SunHeight);
	bool					Get_Thermal				(CSG_Grid *pBand, const CSG_Table_Record &Info_Band);

};


///////////////////////////////////////////////////////////
//														 //
//														 //
//														 //
///////////////////////////////////////////////////////////

//---------------------------------------------------------
#endif // #ifndef HEADER_INCLUDED__landsat_scene_import_H


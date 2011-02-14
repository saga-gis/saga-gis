
///////////////////////////////////////////////////////////
//                                                       //
//                         SAGA                          //
//                                                       //
//    System for an Automated Geo-Scientific Analysis    //
//                                                       //
//                    Module Library:                    //
//                    grid_analysis                      //
//                                                       //
//-------------------------------------------------------//
//                                                       //
//                    Soil_Texture.h                     //
//                                                       //
//                 Copyright (C) 2007 by                 //
//                   Gianluca Massei                     //
//                                                       //
//-------------------------------------------------------//
//                                                       //
// This file is part of 'SAGA - System for an Automated  //
// Geo-Scientific Analysis'. SAGA is free software; you  //
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
//    e-mail:     g_massa@libero.it				     	 //
//                                                       //
//    contact:    Gianluca Massei                        //
//                Department of Economics and Appraisal  //
//                University of Perugia - Italy			 //
//                www.unipg.it                           //
//                                                       //
///////////////////////////////////////////////////////////


///////////////////////////////////////////////////////////
//														 //
//														 //
//														 //
///////////////////////////////////////////////////////////

//---------------------------------------------------------
#ifndef HEADER_INCLUDED__Soil_Texture_H
#define HEADER_INCLUDED__Soil_Texture_H

//---------------------------------------------------------
#include "MLB_Interface.h"


///////////////////////////////////////////////////////////
//														 //
//														 //
//														 //
///////////////////////////////////////////////////////////

//---------------------------------------------------------
class CSoil_Texture : public CSG_Module_Grid
{
public:
	CSoil_Texture(void);
	virtual ~CSoil_Texture(void);

	virtual const SG_Char *	Get_MenuPath	(void)	{	return( _TL("R:Soil Analysis") );	}


protected:

	virtual bool			On_Execute(void);


private:


	///////////////////////////////////////
	/* Declare specific texture function */
	///////////////////////////////////////

	int Clay_Texture(float, float);                   // 1 define Clay soils function
	int SiltyClay_Texture(float, float);              // 2 define SiltyClay soils function
	int SiltyClayLoam_Texture(float, float);          // 3 define SiltyClayLoam soils function
	int SandyClay_Texture(float, float);              // 4 define SandyClay soils function
	int SandyClayLoam_Texture(float, float);          // 5 define SandyClayLoam soils function
	int ClayLoam_Texture(float, float);               // 6 define ClayLoam soils function
	int Silt_Texture(float, float);                   // 7 define Silt soils function
	int SiltLoam_Texture(float, float);               // 8 define SiltLoam soils function
	int Loam_Texture(float, float);                   // 9 define Loam soils function
	int Sand_Texture(float, float);                   // 10 define Sand soils function
	int LoamySand_Texture(float, float);              // 11 define LoamySand soils function
	int SandyLoam_Texture(float, float);              // 12 define SandyLoam soils function
	int OutTexture(float, float);                     //function output   

};


///////////////////////////////////////////////////////////
//														 //
//														 //
//														 //
///////////////////////////////////////////////////////////

//---------------------------------------------------------
#endif // #ifndef HEADER_INCLUDED__Soil_Texture_H

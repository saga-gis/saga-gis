/**********************************************************
 * Version $Id$
 *********************************************************/

///////////////////////////////////////////////////////////
//                                                       //
//                         SAGA                          //
//                                                       //
//      System for Automated Geoscientific Analyses      //
//                                                       //
//                     Tool Library                      //
//                      Grid_Filter                      //
//                                                       //
//-------------------------------------------------------//
//                                                       //
//                 bin_erosion_reconst.h                 //
//                                                       //
//                 Copyright (C) 2013 by                 //
//                     HfT Stuttgart                     //
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
//    e-mail:     johannes.engels@hft-stuttgart.de       //
//                                                       //
//    contact:    Johannes Engels                        //
//                Hochschule fuer Technik Stuttgart      //
//                Schellingstr. 24                       //
//                70174 Stuttgart                        //
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
#ifndef HEADER_INCLUDED__bin_erosion_reconst_H
#define HEADER_INCLUDED__bin erosion_reconst_H

//---------------------------------------------------------


#include "MLB_Interface.h"


///////////////////////////////////////////////////////////
//														 //
//														 //
//														 //
///////////////////////////////////////////////////////////

//---------------------------------------------------------
class Cbin_erosion_reconst : public CSG_Tool_Grid
{
public:
	Cbin_erosion_reconst (void);


protected:

	virtual bool On_Execute (void);


private:
	int						m_Radius, m_Threshold;

	CSG_Grid				m_Kernel, *m_pInput;


	bool					Get_Range		(int x, int y, double &Minimum, double &Maximum);

};


///////////////////////////////////////////////////////////
//														 //
//														 //
//														 //
///////////////////////////////////////////////////////////

//---------------------------------------------------------
#endif // #ifndef HEADER_INCLUDED__bin_erosion_reconst_H

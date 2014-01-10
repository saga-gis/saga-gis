/**********************************************************
 * Version $Id: ChannelNetwork.h 1921 2014-01-09 10:24:11Z oconrad $
 *********************************************************/

///////////////////////////////////////////////////////////
//                                                       //
//                         SAGA                          //
//                                                       //
//      System for Automated Geoscientific Analyses      //
//                                                       //
//                    Module Library:                    //
//                      ta_channels                      //
//                                                       //
//-------------------------------------------------------//
//                                                       //
//                   ChannelNetwork.h                    //
//                                                       //
//                 Copyright (C) 2003 by                 //
//                      Olaf Conrad                      //
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
//    e-mail:     oconrad@saga-gis.org                   //
//                                                       //
//    contact:    Olaf Conrad                            //
//                Institute of Geography                 //
//                University of Goettingen               //
//                Goldschmidtstr. 5                      //
//                37077 Goettingen                       //
//                Germany                                //
//                                                       //
///////////////////////////////////////////////////////////

//---------------------------------------------------------


///////////////////////////////////////////////////////////
//														 //
//														 //
//														 //
///////////////////////////////////////////////////////////

//---------------------------------------------------------
#ifndef HEADER_INCLUDED__ChannelNetwork_H
#define HEADER_INCLUDED__ChannelNetwork_H


///////////////////////////////////////////////////////////
//														 //
//														 //
//														 //
///////////////////////////////////////////////////////////

//---------------------------------------------------------
#include "MLB_Interface.h"


///////////////////////////////////////////////////////////
//														 //
//														 //
//														 //
///////////////////////////////////////////////////////////

//---------------------------------------------------------
class ta_channels_EXPORT CChannelNetwork : public CSG_Module_Grid
{
public:
	CChannelNetwork(void);
	virtual ~CChannelNetwork(void);


protected:

	virtual bool			On_Execute			(void);


private:

	int						*Direction, Direction_Buffer, minLength, maxDivCells;

	CSG_Grid					*pDTM, *pStart, *pTrace, *pConvergence, *pChannels, *pChannelRoute;

	CSG_Shapes					*pShapes;


	void					Set_Route_Standard	(int x, int y);
	void					Set_Route_Weighted	(int x, int y, CSG_Grid *pWeight, double Weight_Threshold);

	void					Set_Channel_Route	(int x, int y);
	void					Set_Channel_Order	(int x, int y);
	void					Set_Channel_Mouth	(int x, int y);

	void					Set_Vector			(int x, int y, int ID);

};


///////////////////////////////////////////////////////////
//														 //
//														 //
//														 //
///////////////////////////////////////////////////////////

//---------------------------------------------------------
#endif // #ifndef HEADER_INCLUDED__ChannelNetwork_H

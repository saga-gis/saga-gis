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
//                      ta_channels                      //
//                                                       //
//-------------------------------------------------------//
//                                                       //
//               ChannelNetwork_Distance.h               //
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
#ifndef HEADER_INCLUDED__ChannelNetwork_Distance_H
#define HEADER_INCLUDED__ChannelNetwork_Distance_H


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
class CChannelNetwork_Distance : public CSG_Tool_Grid  
{
public:
	CChannelNetwork_Distance(void);


protected:

	virtual int				On_Parameters_Enable	(CSG_Parameters *pParameters, CSG_Parameter *pParameter);

	virtual bool			On_Execute				(void);


private:

	double					m_Flow_B, m_Flow_K, m_Flow_R;

	CSG_Grid				*m_pDEM, *m_pRoute, m_Dir, m_Flow[9], *m_pFlow_K, *m_pFlow_R,
							*m_pDistance, *m_pDistVert, *m_pDistHorz,
							*m_pFields, *m_pPasses, *m_pTime, *m_pSDR;


	double					Get_Travel_Time			(int x, int y, int Direction);

	void					Initialize_D8			(void);
	void					Initialize_D8			(int x, int y);
	void					Execute_D8				(int x, int y);

	void					Initialize_MFD			(void);
	void					Initialize_MFD			(int x, int y);
	void					Execute_MFD				(int x, int y);

};


///////////////////////////////////////////////////////////
//														 //
//														 //
//														 //
///////////////////////////////////////////////////////////

//---------------------------------------------------------
#endif // #ifndef HEADER_INCLUDED__ChannelNetwork_Distance_H

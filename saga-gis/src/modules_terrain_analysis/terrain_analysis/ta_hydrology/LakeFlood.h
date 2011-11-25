/**********************************************************
 * Version $Id$
 *********************************************************/

///////////////////////////////////////////////////////////
//                                                       //
//                         SAGA                          //
//                                                       //
//      System for Automated Geoscientific Analyses      //
//                                                       //
//                    Module Library:                    //
//                     ta_hydrology                      //
//                                                       //
//-------------------------------------------------------//
//                                                       //
//                      LakeFlood.h                      //
//                                                       //
//                 Copyright (C) 2005-6 by               //
//                    Volker Wichmann                    //
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
//    e-mail:     volkerwichmann@ku-eichstaett.de        //
//                                                       //
//    contact:    Volker Wichmann                        //
//                Research Associate                     //
//                Chair of Physical Geography		     //
//				  KU Eichst�tt-Ingolstadt				 //
//                Ostenstr. 18                           //
//                85072 Eichst�tt                        //
//                Germany                                //
//                                                       //
///////////////////////////////////////////////////////////

///////////////////////////////////////////////////////////
//														 //
//														 //
//														 //
///////////////////////////////////////////////////////////

//---------------------------------------------------------
#ifndef HEADER_INCLUDED__LakeFlood_H
#define HEADER_INCLUDED__LakeFlood_H


///////////////////////////////////////////////////////////
//														 //
//														 //
//														 //
///////////////////////////////////////////////////////////

//---------------------------------------------------------
#include "MLB_Interface.h"
#include <list>


///////////////////////////////////////////////////////////
//														 //
//														 //
//														 //
///////////////////////////////////////////////////////////

//---------------------------------------------------------
class CTraceOrder
{
public:
	CTraceOrder(void)
	{
		x = y = 0;
		prev = NULL;
		next = NULL;
	};

	~CTraceOrder(void)
	{
		if(next != NULL)
			delete (next);
		next = NULL;
	};

	int		x;
	int		y;
	CTraceOrder *prev;
	CTraceOrder	*next;

};

//---------------------------------------------------------
class ta_hydrology_EXPORT CLakeFlood : public CSG_Module_Grid
{
public:
	CLakeFlood(void);
	virtual ~CLakeFlood(void);

	virtual CSG_String		Get_MenuPath		(void)	{	return( _TL("R:Miscellaneous") );	}


protected:

	virtual bool			On_Execute			(void);

	typedef struct
	{
		int		x;			// grid x
		int		y;			// grid y
		double	z;			// absolute water level
	}SEED;

private:



};

//---------------------------------------------------------
class ta_hydrology_EXPORT CLakeFloodInteractive : public CSG_Module_Grid_Interactive
{
public:
	CLakeFloodInteractive(void);
	virtual ~CLakeFloodInteractive(void);

	virtual CSG_String		Get_MenuPath		(void)	{	return( _TL("R:Miscellaneous") );	}


protected:
	virtual bool			On_Execute			(void);
	virtual bool			On_Execute_Finish	(void);
	virtual bool			On_Execute_Position	(CSG_Point ptWorld, TSG_Module_Interactive_Mode Mode);

private:
	CSG_Grid				*pElev, *pOdepth, *pOlevel;
	CTraceOrder				*newCell, *firstCell, *iterCell, *lastCell;
	bool					m_bLevel;
	double					m_water;


};


///////////////////////////////////////////////////////////
//														 //
//														 //
//														 //
///////////////////////////////////////////////////////////

//---------------------------------------------------------
#endif // #ifndef HEADER_INCLUDED__LakeFlood_H

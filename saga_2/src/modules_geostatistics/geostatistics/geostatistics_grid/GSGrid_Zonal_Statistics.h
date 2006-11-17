
///////////////////////////////////////////////////////////
//                                                       //
//                         SAGA                          //
//                                                       //
//      System for Automated Geoscientific Analyses      //
//                                                       //
//                    Module Library:                    //
//                  Geostatistics_Grid                   //
//                                                       //
//-------------------------------------------------------//
//                                                       //
//              GSGrid_Zonal_Statistics.h                //
//                                                       //
//                 Copyright (C) 2005 by                 //
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
//    e-mail:     volkerwichmann@web.de                  //
//                                                       //
//    contact:    Volker Wichmann                        //
//                Research Associate                     //
//                Chair of Physical Geography		     //
//				  KU Eichstaett-Ingolstadt				 //
//                Ostenstr. 18                           //
//                85072 Eichstaett                       //
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
#ifndef HEADER_INCLUDED__GSGrid_Zonal_Statistics_H
#define HEADER_INCLUDED__GSGrid_Zonal_Statistics_H


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
class CList_Stat
{
public:
	CList_Stat(void)
	{
		min		= max	= 0.0;
		sum		= dev	= 0.0;
		next	= NULL;
		dummy	= true;
	};

	~CList_Stat(void)
	{
		if( next != NULL )
			delete(next);
		next	= NULL;
	};


	double				min, max, sum, dev;
	bool				dummy;

	CList_Stat			*next;

};

//---------------------------------------------------------
class CList_Conti
{
public:
	CList_Conti(void)
	{
		cat			= NULL;
		count		= 0;
		next		= NULL;
		previous	= NULL;
		parent		= NULL;
		sub			= NULL;
		stats		= NULL;
		dummy		= true;
	};

	~CList_Conti(void)
	{
		if( stats != NULL )
			delete(stats);

		if( sub != NULL )
			delete(sub);
		sub		= NULL;

		if( next != NULL )
			delete(next);
		next	 = NULL;					
	};


	int					cat, count;
	bool				dummy;

	CList_Conti			*next, *previous, *parent, *sub;

	CList_Stat			*stats;

};

//---------------------------------------------------------
class CGSGrid_Zonal_Statistics : public CSG_Module_Grid
{
public:
	CGSGrid_Zonal_Statistics(void);
	virtual ~CGSGrid_Zonal_Statistics(void);


protected:

	virtual bool		On_Execute		(void);


private:

};


///////////////////////////////////////////////////////////
//														 //
//														 //
//														 //
///////////////////////////////////////////////////////////

//---------------------------------------------------------
#endif // #ifndef HEADER_INCLUDED__GSGrid_Zonal_Statistics_H

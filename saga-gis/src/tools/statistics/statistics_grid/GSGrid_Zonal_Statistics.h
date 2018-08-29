
///////////////////////////////////////////////////////////
//                                                       //
//                         SAGA                          //
//                                                       //
//      System for Automated Geoscientific Analyses      //
//                                                       //
//                     Tool Library                      //
//                    statistics_grid                    //
//                                                       //
//-------------------------------------------------------//
//                                                       //
//              GSGrid_Zonal_Statistics.h                //
//                                                       //
//                Copyright (C) 2005-9 by                //
//                    Volker Wichmann                    //
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
#ifndef HEADER_INCLUDED__GSGrid_Zonal_Statistics_H
#define HEADER_INCLUDED__GSGrid_Zonal_Statistics_H

//---------------------------------------------------------
#include <saga_api/saga_api.h>


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
		n		= 0;
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

	int					n;
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
		cat			= 0;
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
class CGSGrid_Zonal_Statistics : public CSG_Tool_Grid
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

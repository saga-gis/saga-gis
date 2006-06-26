/*******************************************************************************
    Grid_Random_Terrain.h
    Copyright (C) Victor Olaya
    
    This program is free software; you can redistribute it and/or modify
    it under the terms of the GNU General Public License as published by
    the Free Software Foundation; either version 2 of the License, or
    (at your option) any later version.

    This program is distributed in the hope that it will be useful,
    but WITHOUT ANY WARRANTY; without even the implied warranty of
    MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
    GNU General Public License for more details.

    You should have received a copy of the GNU General Public License
    along with this program; if not, write to the Free Software
    Foundation, Inc., 59 Temple Place, Suite 330, Boston, MA  02111-1307  USA
*******************************************************************************/ 
//---------------------------------------------------------
#ifndef HEADER_INCLUDED__Grid_Random_Terrain_H
#define HEADER_INCLUDED__Grid_Random_Terrain_H

#include "MLB_Interface.h"

class CGrid_Random_Terrain : public CModule
{
public:
	CGrid_Random_Terrain(void);
	virtual ~CGrid_Random_Terrain(void);

	virtual const char *	Get_MenuPath	(void)	{	return( _TL("R:Grid Generation") );	}


protected:

	CGrid *pGrid;

	bool On_Execute(void);
	void addBump (CGrid*, int);


private:

	CGrid* Get_Target_Grid(CParameters *pParameters);

};

#endif // #ifndef HEADER_INCLUDED__Grid_Random_Terrain_H

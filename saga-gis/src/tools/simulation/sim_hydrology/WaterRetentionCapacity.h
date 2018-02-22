/**********************************************************
 * Version $Id: WaterRetentionCapacity.h 911 2011-02-14 16:38:15Z reklov_w $
 *********************************************************/
/*******************************************************************************
    WaterRetentionCapacity.h
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
    Foundation, Inc., 51 Franklin Street, 5th Floor, Boston, MA 02110-1301, USA
*******************************************************************************/

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
class CWaterRetentionCapacity : public CSG_Tool_Grid
{
public:
	CWaterRetentionCapacity(void);


protected:

	virtual bool		On_Execute				(void);


private:

	void				Get_WaterRetention		(CSG_Matrix &Data, double fC, CSG_Shape *pPoint);

	double				Get_He					(double *Horizon);
	double				Get_CIL					(double *Horizon);
	double				Get_CCC					(double *Horizon);
	double				Get_K					(double PermI, double PermS, double fC);
	int					Get_Permeability		(double fCCC, double fCIL);

};


///////////////////////////////////////////////////////////
//														 //
//														 //
//														 //
///////////////////////////////////////////////////////////

//---------------------------------------------------------

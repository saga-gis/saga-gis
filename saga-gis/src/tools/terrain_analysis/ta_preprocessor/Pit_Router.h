/**********************************************************
 * Version $Id: Pit_Router.h 1922 2014-01-09 10:28:46Z oconrad $
 *********************************************************/

///////////////////////////////////////////////////////////
//                                                       //
//                         SAGA                          //
//                                                       //
//      System for Automated Geoscientific Analyses      //
//                                                       //
//                     Tool Library                      //
//                    ta_preprocessor                    //
//                                                       //
//-------------------------------------------------------//
//                                                       //
//                     Pit_Router.h                      //
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
#ifndef HEADER_INCLUDED__Pit_Router_H
#define HEADER_INCLUDED__Pit_Router_H


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
class ta_preprocessor_EXPORT CPit_Router : public CSG_Tool_Grid
{
public:
	CPit_Router(void);
	virtual ~CPit_Router(void);

	int							Get_Routes		(CSG_Grid *pDEM, CSG_Grid *pRoutes, double Threshold = -1.0);


protected:

	virtual bool				On_Execute		(void);


private:

	typedef struct
	{
		int						xMin, yMin, xMax, yMax;
	}
	TGEO_iRect;

	typedef struct
	{
		bool					bDrained;

		double					z;
	}
	TPit;

	typedef struct TPit_Outlet
	{
		int						x, y, Pit_ID[8];

		struct TPit_Outlet		*Prev, *Next;
	}
	TPit_Outlet;


	int							**m_Junction, *m_nJunctions;

	double						m_Threshold, m_zThr, m_zMax;

	CSG_Grid					*m_pDEM, *m_pRoute, *m_pFlats, *m_pPits, m_Route;

	TGEO_iRect					*m_Flat;

	TPit						*m_Pit;

	TPit_Outlet					*m_Outlets;


	bool						Initialize		(void);
	void						Finalize		(void);

	int							Find_Pits		(void);
	int							Find_Outlets	(int nPits);
	int							Find_Route		(TPit_Outlet *pOutlet);

	void						Add_Junction	(int iID, int jID);
	bool						Get_Junction	(int iID, int jID);

	void						Drain_Pit		(int x, int y, int Pit_ID);
	void						Drain_Flat		(int x, int y );

	void						Mark_Flat		(int x, int y, TGEO_iRect *pFlat, int Flat_ID, int Pit_ID);


	int							Process_Threshold	(void);
	void						Check_Threshold		(int x, int y);

};


///////////////////////////////////////////////////////////
//														 //
//														 //
//														 //
///////////////////////////////////////////////////////////

//---------------------------------------------------------
#endif // #ifndef HEADER_INCLUDED__Pit_Router_H

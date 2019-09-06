
///////////////////////////////////////////////////////////
//                                                       //
//                         SAGA                          //
//                                                       //
//      System for Automated Geoscientific Analyses      //
//                                                       //
//-------------------------------------------------------//
//                                                       //
//           diffusion_gradient_concentration.h          //
//                                                       //
//                Copyright (C) 2007 by                  //
//                O.Conrad, R.Heinrich                   //
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
// Public License along with this program; if not,       //
// write to the Free Software Foundation, Inc.,          //
// 59 Temple Place - Suite 330, Boston, MA 02111-1307,   //
// USA.                                                  //
//                                                       //
//-------------------------------------------------------//
//                                                       //
//    contact:    Ralph Heinrich                         //
//                                                       //
//    e-mail:     heinrich-ralph@web.de                  //
//                                                       //
//    phone:      +49-35603-152006                       //
//                                                       //
///////////////////////////////////////////////////////////

//---------------------------------------------------------
#ifndef HEADER_INCLUDED__diffusion_gradient_concentration_H
#define HEADER_INCLUDED__diffusion_gradient_concentration_H


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
class CSim_Diffusion_Gradient : public CSG_Tool_Grid
{
public:
	CSim_Diffusion_Gradient(void);

	virtual CSG_String		Get_MenuPath			(void)	{	return( "Diffusion, Flow and Concentration Gradient Simulation" );	}


protected:

	virtual bool			On_Execute				(void);


	//-----------------------------------------------------
	CSG_Grid				*m_pMask, m_Tmp;


	//-----------------------------------------------------
	bool					Surface_Initialise		(CSG_Grid *pSurface);
	bool					Surface_Interpolate		(CSG_Grid *pSurface);
	double					Surface_Set_Means		(CSG_Grid *pSurface);
	bool					Surface_Get_Gradient	(CSG_Grid *pSurface, CSG_Grid *pGradient);


	//-----------------------------------------------------
	bool					is_Lake					(int x, int y);

};


///////////////////////////////////////////////////////////
//														 //
///////////////////////////////////////////////////////////

//---------------------------------------------------------
class CSim_Diffusion_Concentration : public CSim_Diffusion_Gradient
{
public:
	CSim_Diffusion_Concentration(void);


protected:

	virtual bool			On_Execute					(void);


	double					m_Conc_In, m_Conc_Out, m_MinGradient;


	bool					Concentration_Interpolate	(CSG_Grid *pConcentration, CSG_Grid *pGradient);


private:

	bool					_Concentration_Interpolate	(CSG_Grid *pConcentration, CSG_Grid *pGradient, bool bNeumann);
	double					_Concentration_Interpolate	(CSG_Grid *pConcentration, CSG_Grid *pGradient, bool bNeumann, double f);
	int						_Concentration_Set_Means	(CSG_Grid *pConcentration, CSG_Grid *pGradient, bool bNeumann, double f, double &Conc_Out);
	bool					_Concentration_Initialise	(CSG_Grid *pConcentration);

};


///////////////////////////////////////////////////////////
//														 //
///////////////////////////////////////////////////////////

//---------------------------------------------------------
class CSim_Diffusion_Gradient_And_Concentration : public CSim_Diffusion_Concentration
{
public:
	CSim_Diffusion_Gradient_And_Concentration(void);


protected:

	virtual bool			On_Execute				(void);

};


///////////////////////////////////////////////////////////
//														 //
//														 //
//														 //
///////////////////////////////////////////////////////////

//---------------------------------------------------------
#endif // #ifndef HEADER_INCLUDED__diffusion_gradient_concentration_H

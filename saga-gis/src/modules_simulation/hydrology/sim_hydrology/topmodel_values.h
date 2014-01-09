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
//                     sim_hydrology                     //
//                                                       //
//-------------------------------------------------------//
//                                                       //
//                   topmodel_values.h                   //
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
#ifndef HEADER_INCLUDED__topmodel_values_H
#define HEADER_INCLUDED__topmodel_values_H


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
class CTOPMODEL_Class
{
public:
	CTOPMODEL_Class(double Srz_Init);
	virtual ~CTOPMODEL_Class(void);

	double				qt_, qo_, qv_,
						Srz_, Suz_, S_,
						AtanB, Area_Rel;

};


///////////////////////////////////////////////////////////
//														 //
//														 //
//														 //
///////////////////////////////////////////////////////////

//---------------------------------------------------------
class CTOPMODEL_Values
{
public:
	CTOPMODEL_Values(void);
	virtual ~CTOPMODEL_Values(void);

	int					ndelay_, nreach_;

	double				qt_Total, qo_Total, qv_Total,
						qs_Total,

						lnTe_,
						_qs_,

						*Add,				// nreach_'s:

						*Qt_,				// input.ntimestep's:

						Sbar_,

						p_Srz_Max,			// Maximum root zone storage deficit [m]
						p_Model,			// Model parameter m...
						p_Suz_TimeDelay,	// Unsaturated zone time delay per unit storage deficit ( > 0.0) [h]
											// OR -alpha: Effective vertical hydraulic gradient ( <= 0.0); -10 means that alpha = 10
						p_K0,				// Surface hydraulic conductivity [m/h]
						p_Psi,				// Wetting front suction [m]
						p_dTheta;			// Water content change across the wetting front


	void				Create(double dTime, int anTimeSteps, CSG_Parameters *pParameters, CSG_Grid *pAtanB, int anClasses, CSG_Grid *pClass);
	void				Destroy(void);

	//-----------------------------------------------------
	int					Get_Count(void)
	{
		return( nClasses );
	}

	//-----------------------------------------------------
	double				Get_Area_Total(void)
	{
		return( Area_Total );
	}

	//-----------------------------------------------------
	CTOPMODEL_Class *	Get_Class(int iClass)
	{
		if( iClass >= 0 && iClass < nClasses )
		{
			return( Classes[iClass] );
		}

		return( NULL );
	}

	//-----------------------------------------------------
	double				Get_Lambda(void)
	{
		return( Lambda );
	}


private:

	int					nClasses, Channel_Count;

	double				Lambda, *Channel_Distance, *Channel_AreaRatio,
						Area_Total;		// Total catchment area [m^2]

	CTOPMODEL_Class		**Classes;

};


///////////////////////////////////////////////////////////
//														 //
//														 //
//														 //
///////////////////////////////////////////////////////////

//---------------------------------------------------------
#endif // #ifndef HEADER_INCLUDED__topmodel_values_H

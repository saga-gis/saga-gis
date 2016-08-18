/**********************************************************
 * Version $Id: topmodel_values.cpp 1921 2014-01-09 10:24:11Z oconrad $
 *********************************************************/

///////////////////////////////////////////////////////////
//                                                       //
//                         SAGA                          //
//                                                       //
//      System for Automated Geoscientific Analyses      //
//                                                       //
//                     Tool Library                      //
//                     sim_hydrology                     //
//                                                       //
//-------------------------------------------------------//
//                                                       //
//                  topmodel_values.cpp                  //
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
#include "topmodel_values.h"

//---------------------------------------------------------
#define RESET_ARRAY(a)		if( a ) { free(a); a = NULL; }
#define RESET_MATRIX(n, m)	if( n > 0 && m ) { for(int i=0; i<n; i++) { if( m[i] ) { free(m[i]); } } free(m); m = NULL; }


///////////////////////////////////////////////////////////
//														 //
//														 //
//														 //
///////////////////////////////////////////////////////////

//---------------------------------------------------------
CTOPMODEL_Class::CTOPMODEL_Class(double Srz_Init)
{
	Srz_		= Srz_Init;
	Suz_		= 0.0;
	S_			= 0.0;

	qt_			= 0.0;
	qo_			= 0.0;
	qv_			= 0.0;

	AtanB		= 0.0;
	Area_Rel	= 0.0;
}

//---------------------------------------------------------
CTOPMODEL_Class::~CTOPMODEL_Class(void)
{}


///////////////////////////////////////////////////////////
//														 //
//														 //
//														 //
///////////////////////////////////////////////////////////

//---------------------------------------------------------
CTOPMODEL_Values::CTOPMODEL_Values(void)
{
	nClasses	= 0;
	Lambda		= 0;

	Add			= NULL;
	Qt_			= NULL;

	//-----------------------------------------------------
	Channel_Count			= 3;

	Channel_Distance		= (double *)malloc(Channel_Count * sizeof(double));
	Channel_AreaRatio		= (double *)malloc(Channel_Count * sizeof(double));

	Channel_Distance[0]		= 500;
	Channel_AreaRatio[0]	= 0.0;

	Channel_Distance[1]		= 1000;
	Channel_AreaRatio[1]	= 0.5;

	Channel_Distance[2]		= 1500;
	Channel_AreaRatio[2]	= 1.0;
}

//---------------------------------------------------------
CTOPMODEL_Values::~CTOPMODEL_Values(void)
{
	Destroy();

	free(Channel_Distance);
	free(Channel_AreaRatio);
}


///////////////////////////////////////////////////////////
//														 //
//														 //
//														 //
///////////////////////////////////////////////////////////

//---------------------------------------------------------
void CTOPMODEL_Values::Destroy(void)
{
	int		iClass;

	if( nClasses > 0 )
	{
		for(iClass=0; iClass<nClasses; iClass++)
		{
			delete( Classes[iClass] );
		}

		free(Classes);

		nClasses	= 0;
	}

	qt_Total	= 0.0;
	qo_Total	= 0.0;
	qv_Total	= 0.0;

	RESET_ARRAY(Add);
	RESET_ARRAY(Qt_);
}

//---------------------------------------------------------
void CTOPMODEL_Values::Create(double dTime, int anTimeSteps, CSG_Parameters *pParameters, CSG_Grid *pAtanB, int anClasses, CSG_Grid *pClass)
{
	sLong	n, iClass, nCells;

	double	zMin, zRange, dz;

	Destroy();

	if( pAtanB && anClasses > 0 )
	{
		//-------------------------------------------------
		// 1. Topographic Index Classification etc...

		nClasses	= anClasses;

		Classes		= (CTOPMODEL_Class **)calloc(nClasses, sizeof(CTOPMODEL_Class *));

		for(iClass=0; iClass<nClasses; iClass++)
		{
			Classes[iClass]	= new CTOPMODEL_Class(pParameters->Get_Parameter("P_SR0")->asDouble());
		}

		zMin		= pAtanB->Get_ZMin();
		zRange		= pAtanB->Get_ZMax() - zMin;
		dz			= zRange / (nClasses + 1);
		nCells		= 0;

		pClass->Create(pAtanB, SG_DATATYPE_Short);
		pClass->Set_NoData_Value(-9999);

		for(n=0; n<pAtanB->Get_NCells(); n++)
		{
			if( !pAtanB->is_NoData(n) )
			{
				nCells++;

				iClass			= (int)((nClasses - 1.0) * (pAtanB->asDouble(n) - zMin) / zRange);

				Classes[iClass]->Area_Rel++;

				pClass->Set_Value(n, iClass);
			}
			else
			{
				pClass->Set_NoData(n);
			}
		}

		Area_Total	= (double)nCells * pAtanB->Get_Cellsize() * pAtanB->Get_Cellsize();

		for(iClass=0; iClass<nClasses; iClass++)
		{
			Classes[iClass]->AtanB		= zMin + dz * (iClass + 0.5);	// mid of class -> + 0.5...
			Classes[iClass]->Area_Rel	/= (double)nCells;
		}


		//-------------------------------------------------
		// 2. Calculate Lambda, the catchment average topographic index...

		for(iClass=0, Lambda=0.0; iClass<nClasses; iClass++)
		{
			Lambda	+= Classes[iClass]->Area_Rel * Classes[iClass]->AtanB;
		}


		//-------------------------------------------------
		// 3. Parameter Initialisation...

		//-------------------------------------------------
		p_Srz_Max		= pParameters->Get_Parameter("P_SRZMAX")->asDouble();
		p_Model			= pParameters->Get_Parameter("P_MODEL")->asDouble();
		p_Suz_TimeDelay	= pParameters->Get_Parameter("P_SUZ_TD")->asDouble();
		p_K0			= pParameters->Get_Parameter("P_K0")->asDouble();
		p_Psi			= pParameters->Get_Parameter("P_PSI")->asDouble();
		p_dTheta		= pParameters->Get_Parameter("P_DTHETA")->asDouble();


		//-------------------------------------------------
		int		i, j, t;

		double	A1, A2,
				qs0_,	// Initial subsurface flow per unit area [m/h], "The first streamflow input is assumed to represent only the subsurface flow contribution in the watershed."
				vch_,	// Main channel routing velocity [m/h]
				vr_,	// Internal subcatchment routing velocity [m/h]
				*tch_;	// params.nch's

		//-------------------------------------------------
		lnTe_		= log(dTime)	+ pParameters->Get_Parameter("P_LNTE")->asDouble();
		vch_		= dTime			* pParameters->Get_Parameter("P_VCH")->asDouble();
		vr_			= dTime			* pParameters->Get_Parameter("P_VR")->asDouble();
		qs0_		= dTime			* pParameters->Get_Parameter("P_QS0")->asDouble();
		_qs_		= exp(lnTe_ - Lambda);

		//-------------------------------------------------
		tch_		= (double *)malloc(Channel_Count * sizeof(double));
		tch_[0]		= Channel_Distance[0] / vch_;

		for(i=1; i<Channel_Count; i++)
		{
			tch_[i]		= tch_[0] + (Channel_Distance[i] - Channel_Distance[0]) / vr_;
		}

		//-------------------------------------------------
		nreach_		= (int)tch_[Channel_Count - 1];
		if( (double)nreach_ < tch_[Channel_Count - 1] )
		{
			nreach_++;
		}

		ndelay_		= (int)tch_[0];
		nreach_		-= ndelay_;

		//-------------------------------------------------
		Add			= (double *)malloc(nreach_ * sizeof(double));

		for(i=0; i<nreach_; i++)
		{
			t			= ndelay_ + i + 1;
			if( t > tch_[Channel_Count - 1])
			{
				Add[i]		= 1.0;
			}
			else
			{
				for(j=1; j<Channel_Count; j++)
				{
					if( t <= tch_[j] )
					{
						Add[i]		= Channel_AreaRatio[j - 1]
									+ (Channel_AreaRatio[j] - Channel_AreaRatio[j - 1])
									* (t - tch_[j - 1]) / (tch_[j] - tch_[j - 1]);
						break;
					}
				}
			}
		}

		A1			= Add[0];
		Add[0]		*= Area_Total;

		for(i=1; i<nreach_; i++)
		{
			A2			= Add[i];
			Add[i]		= A2 - A1;
			A1			= A2;
			Add[i]		*= Area_Total;
		}

		//-------------------------------------------------
		Sbar_		= -p_Model * log(qs0_ / _qs_);

		//-------------------------------------------------
		Qt_			= (double *)calloc(anTimeSteps, sizeof(double));
		for(i=0; i<anTimeSteps; i++)
		{
			Qt_[i]		= 0.0;
		}

		for(i=0; i<ndelay_; i++)
		{
			Qt_[i] = qs0_ * Area_Total;
		}

		A1			= 0.0;

		for(i=0; i<nreach_; i++)
		{
			A1					+= Add[i];
			Qt_[ndelay_ + i]	= qs0_ * (Area_Total - A1);
		}

		//-------------------------------------------------
		RESET_ARRAY(tch_);
	}
}

/**********************************************************
 * Version $Id: topmodel.cpp 1921 2014-01-09 10:24:11Z oconrad $
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
//                     topmodel.cpp                      //
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
#include "topmodel.h"


///////////////////////////////////////////////////////////
//														 //
//														 //
//														 //
///////////////////////////////////////////////////////////

//---------------------------------------------------------
CTOPMODEL::CTOPMODEL(void)
{
	CSG_Parameter	*pNode;

	//-----------------------------------------------------
	// Place information about your module here...

	Set_Name		(_TL("TOPMODEL"));

	Set_Author		(SG_T("(c) 2003 by O.Conrad"));

	Set_Description	(_TW(
		"Simple Subcatchment Version of TOPMODEL\n\n"

		"Based on the 'TOPMODEL demonstration program v95.02' by Keith Beven "
		"(Centre for Research on Environmental Systems and Statistics, "
		"Institute of Environmental and Biological Sciences, "
		"Lancaster University, Lancaster LA1 4YQ, UK) "
		"and the C translation of the Fortran source codes implementated in GRASS.\n\n"

		"This program allows single or multiple subcatchment calculations "
		"but with single average rainfall and potential evapotranspiration "
		"inputs to the whole catchment.  Subcatchment discharges are routed "
		"to the catchment outlet using a linear routing algorithm with "
		"constant main channel velocity and internal subcatchment "
		"routing velocity.  The program requires ln(a/tanB) distributions "
		"for each subcatchment.  These may be calculated using the "
		"GRIDATB program which requires raster elevation data as input. "
		"It is recommended that those data should be 50 m resolution or "
		"better.\n\n"

		"NOTE that TOPMODEL is not intended to be a traditional model "
		"package but is more a collection of concepts that can be used "
		"**** where appropriate ****. It is up to the user to verify that "
		"the assumptions are appropriate (see discussion in "
		"Beven et al.(1994).   This version of the model  will be "
		"best suited to catchments with shallow soils and moderate "
		"topography which do not suffer from excessively long dry "
		"periods.  Ideally predicted contributing areas should be "
		"checked against what actually happens in the catchment.\n\n"

		"It includes infiltration excess calculations and parameters "
		"based on the exponential conductivity Green-Ampt model of "
		"Beven (HSJ, 1984) but if infiltration excess does occur it "
		"does so over whole area of a subcatchment.  Spatial variability "
		"in conductivities can however be handled by specifying "
		"Ko parameter values for different subcatchments, even if they "
		"have the same ln(a/tanB) and routing parameters, ie. to "
		"represent different parts of the area.\n\n"

		"Note that time step calculations are explicit ie. SBAR "
		"at start of time step is used to determine contributing area. "
		"Thus with long (daily) time steps contributing area depends on "
		"initial value together with any volume filling effect of daily "
		"inputs.  Also baseflow at start of time step is used to update "
		"SBAR at end of time step."

		"\n\nReferences\n"
		"- Beven, K., Kirkby, M.J., Schofield, N., Tagg, A.F. (1984): "
		"  Testing a physically-based flood forecasting model (TOPMODEL) for threee U.K. catchments, "
		"  Journal of Hydrology, H.69, S.119-143.\n"
		"\n"
		"- Beven, K. (1997): "
		"  TOPMODEL - a critique, "
		"  Hydrological Processes, Vol.11, pp.1069-1085.\n"
	));


	//-----------------------------------------------------

	Parameters.Add_Grid(
		NULL	, "ATANB"		, _TL("Topographic Wetness Index"),
		_TL(""),
		PARAMETER_INPUT
	);

	Parameters.Add_Grid(
		NULL	, "MOIST"		, _TL("Soil Moisture Deficit"),
		_TL(""),
		PARAMETER_OUTPUT_OPTIONAL
	);

	pNode	= Parameters.Add_Table(
		NULL	, "WEATHER"		, _TL("Weather Records"),
		_TL(""),
		PARAMETER_INPUT
	);

	Parameters.Add_Table_Field(
		pNode	, "RECORD_P"	, _TL("Precipitation [m / dt]"),
		_TL("")
	);

	Parameters.Add_Table_Field(
		pNode	, "RECORD_ET"	, _TL("Evapotranspiration [m / dt]"),
		_TL("")
	);

	Parameters.Add_Table_Field(
		pNode	, "RECORD_DATE"	, _TL("Date/Time"),
		_TL(""),
		true
	);

	Parameters.Add_Table(
		NULL	, "TABLE"		, _TL("Simulation Output"),
		_TL(""),
		PARAMETER_OUTPUT
	);

	Parameters.Add_Value(
		NULL	, "DTIME"		, _TL("Time Step [h]"),
		_TL(""),
		PARAMETER_TYPE_Double	, 1.0
	);

	Parameters.Add_Value(
		NULL	, "NCLASSES"	, _TL("Number of Classes"),
		_TL(""),
		PARAMETER_TYPE_Int		, 30	, 1	, true
	);

	pNode	= NULL;	// = Parameters("MOIST");

	Parameters.Add_Value(
		pNode, "P_QS0"			, _TL("Initial subsurface flow per unit area [m/h]"),
		_TL(""),
		PARAMETER_TYPE_Double	, 3.28e-05
	);

	Parameters.Add_Value(
		pNode, "P_LNTE"			, _TL("Areal average of ln(T0) = ln(Te) [ln(m^2/h)]"),
		_TL(""),
		PARAMETER_TYPE_Double	, 5.0
	);

	Parameters.Add_Value(
		pNode, "P_MODEL"		, _TL("Model parameter [m]"),
		_TL(""),
		PARAMETER_TYPE_Double	, 0.032
	);

	Parameters.Add_Value(
		pNode, "P_SR0"			, _TL("Initial root zone storage deficit [m]"),
		_TL(""),
		PARAMETER_TYPE_Double	, 0.002
	);

	Parameters.Add_Value(
		pNode, "P_SRZMAX"		, _TL("Maximum root zone storage deficit [m]"),
		_TL(""),
		PARAMETER_TYPE_Double	, 0.05
	);

	Parameters.Add_Value(
		pNode, "P_SUZ_TD"		, _TL("Unsaturated zone time delay per unit storage deficit [h]"),
		_TL(""),
		PARAMETER_TYPE_Double	, 50.0
	);

	Parameters.Add_Value(
		pNode, "P_VCH"			, _TL("Main channel routing velocity [m/h]"),
		_TL(""),
		PARAMETER_TYPE_Double	, 3600.0
	);

	Parameters.Add_Value(
		pNode, "P_VR"			, _TL("Internal subcatchment routing velocity [m/h]"),
		_TL(""),
		PARAMETER_TYPE_Double	, 3600.0
	);

	Parameters.Add_Value(
		pNode, "P_K0"			, _TL("Surface hydraulic conductivity [m/h]"),
		_TL(""),
		PARAMETER_TYPE_Double	, 1.0
	);

	Parameters.Add_Value(
		pNode, "P_PSI"			, _TL("Wetting front suction [m]"),
		_TL(""),
		PARAMETER_TYPE_Double	, 0.02
	);

	Parameters.Add_Value(
		pNode, "P_DTHETA"		, _TL("Water content change across the wetting front"),
		_TL(""),
		PARAMETER_TYPE_Double	, 0.1
	);

	Parameters.Add_Value(
		pNode, "BINF"			, _TL("Green-Ampt Infiltration"),
		_TL(""),
		PARAMETER_TYPE_Bool		, true
	);
}

//---------------------------------------------------------
CTOPMODEL::~CTOPMODEL(void)
{}


///////////////////////////////////////////////////////////
//														 //
//														 //
//														 //
///////////////////////////////////////////////////////////

//---------------------------------------------------------
bool CTOPMODEL::On_Execute(void)
{
	bool				bInfiltration;
	sLong				n;
	int					iClass, nClasses, iTime, nTimeSteps, k;
	double				Precipitation, Evaporation, Infiltration, Infiltration_Excess;
	CSG_String			Time;
	CSG_Grid			*pAtanB, *pMoist, gClass;
	CSG_Table_Record	*pRecord;
	CSG_Table			*pTable;


	//-----------------------------------------------------
	// Get user inputs from the 'Parameters' object...

	pAtanB			= Parameters("ATANB")		->asGrid();
	m_pWeather		= Parameters("WEATHER")		->asTable();
	m_fP			= Parameters("RECORD_P")	->asInt();
	m_fET			= Parameters("RECORD_ET")	->asInt();
	m_fTime			= Parameters("RECORD_DATE")	->asInt();
	dTime			= Parameters("DTIME")		->asDouble();
	nClasses		= Parameters("NCLASSES")	->asInt();
	bInfiltration	= Parameters("BINF")		->asBool();

	nTimeSteps		= m_pWeather->Get_Record_Count();

	if( (pMoist = Parameters("MOIST")->asGrid()) != NULL )
	{
		pMoist->Set_Name(_TL("Soil Moisture Deficit"));
		DataObject_Set_Colors(pMoist, 100, SG_COLORS_RED_GREY_BLUE, true);
	}


	//-----------------------------------------------------
	pTable			= Parameters("TABLE")->asTable();
	pTable->Destroy();
	pTable->Set_Name(_TL("TOPMODEL - Simulation Output"));

	pTable->Add_Field(_TL("Time")										, SG_DATATYPE_String);
	pTable->Add_Field(_TL("Total flow (in watershed) [m^3/dt]")			, SG_DATATYPE_Double);
	pTable->Add_Field(_TL("Total flow [m/dt]")							, SG_DATATYPE_Double);
	pTable->Add_Field(_TL("Saturation overland flow [m/dt]")			, SG_DATATYPE_Double);
	pTable->Add_Field(_TL("Subsurface flow [m/dt]")						, SG_DATATYPE_Double);
	pTable->Add_Field(_TL("Vertical (drainage) flux [m/dt]")			, SG_DATATYPE_Double);
	pTable->Add_Field(_TL("Mean saturation deficit (in watershed) [m]")	, SG_DATATYPE_Double);
	pTable->Add_Field(_TL("Infiltration rate [m/dt]")					, SG_DATATYPE_Double);
	pTable->Add_Field(_TL("Infiltration excess runoff [m/dt]")			, SG_DATATYPE_Double);

	//-----------------------------------------------------
	Vals.Create(dTime, nTimeSteps, &Parameters, pAtanB, nClasses, &gClass);

	//-----------------------------------------------------
	inf_bPonding	= false;
	inf_cumf		= 0.0;

	for(iTime=0; iTime<nTimeSteps && Set_Progress(iTime, nTimeSteps); iTime++)
	{
		Get_Weather(iTime, Precipitation, Evaporation, Time);

		if( bInfiltration && Precipitation > 0.0 )
		{
			Infiltration		= dTime * Get_Infiltration((iTime + 1) * dTime, Precipitation / dTime);
			Infiltration_Excess	= Precipitation - Infiltration;
			Precipitation		= Infiltration;
		}
		else
		{
			Infiltration		= 0.0;
			Infiltration_Excess	= 0.0;
		}

		Run(Evaporation, Precipitation, Infiltration_Excess);

		for(iClass=0; iClass<Vals.nreach_; iClass++)
		{
			k		= iTime + iClass + Vals.ndelay_;
			if( k > nTimeSteps - 1 )
				break;

			Vals.Qt_[k]	+= Vals.qt_Total * Vals.Add[iClass];
		}

		if( pMoist )
		{
			for(n=0; n<gClass.Get_NCells(); n++)
			{
				iClass	= gClass.asInt(n);

				if( iClass >= 0 && iClass < nClasses )
				{
					pMoist->Set_Value(n, Vals.Get_Class(iClass)->S_);
				}
				else
				{
					pMoist->Set_NoData(n);
				}
			}

		//	DataObject_Update(pMoist);
			DataObject_Update(pMoist, 0, 0.35, true);
		}

		pRecord	= pTable->Add_Record();
		pRecord->Set_Value(0, Time);				// Time
		pRecord->Set_Value(1, Vals.Qt_[iTime]);		// QT
		pRecord->Set_Value(2, Vals.qt_Total);		// qt
		pRecord->Set_Value(3, Vals.qo_Total);		// q0
		pRecord->Set_Value(4, Vals.qs_Total);		// qs
		pRecord->Set_Value(5, Vals.qv_Total);		// qv
		pRecord->Set_Value(6, Vals.Sbar_);			// SBar
		pRecord->Set_Value(7, Infiltration);		// Infiltration
		pRecord->Set_Value(8, Infiltration_Excess);	// Infiltration Excess
		DataObject_Update(pTable);
	}

	//-----------------------------------------------------
	return( true );
}


///////////////////////////////////////////////////////////
//														 //
//														 //
//														 //
///////////////////////////////////////////////////////////

//---------------------------------------------------------
void CTOPMODEL::Run(double Evaporation, double Precipitation, double Infiltration_Excess)
{
	int				iClass;
	double			d, Excess;
	CTOPMODEL_Class	*pClass;

	Vals.qo_Total	= 0.0;
	Vals.qv_Total	= 0.0;
	Vals.qs_Total	= Vals._qs_ * exp(-Vals.Sbar_ / Vals.p_Model);

	for(iClass=0; iClass<Vals.Get_Count(); iClass++)
	{
		pClass			= Vals.Get_Class(iClass);


		//-------------------------------------------------
		//  CALCULATE LOCAL STORAGE DEFICIT

		pClass->S_		= Vals.Sbar_ + Vals.p_Model * (Vals.Get_Lambda() - pClass->AtanB);

		if( pClass->S_ < 0.0 )
		{
			pClass->S_		= 0.0;
		}


		//-------------------------------------------------
		//  ROOT ZONE CALCULATIONS

		pClass->Srz_	-= Precipitation;

		if( pClass->Srz_ < 0.0 )
		{
			pClass->Suz_	-= pClass->Srz_;
			pClass->Srz_	= 0.0;
		}


		//-------------------------------------------------
		//  UNSATURATED ZONE CALCULATIONS

		if( pClass->Suz_ > pClass->S_ )
		{
			Excess			= pClass->Suz_ - pClass->S_;
			pClass->Suz_	= pClass->S_;
		}
		else
		{
			Excess			= 0.0;
		}


		//-------------------------------------------------
		//  CALCULATE DRAINAGE FROM SUZ (Vertical Soil Water Flux (qv))...

		if( pClass->S_ > 0.0 )
		{
			if( Vals.p_Suz_TimeDelay > 0.0 )
			{	// Methode 1...
				d			= pClass->Suz_ / (pClass->S_ * Vals.p_Suz_TimeDelay) * dTime;	// GRASS
			}
			else
			{	// Methode 2...
				d			= -Vals.p_Suz_TimeDelay * Vals.p_K0 * exp(-pClass->S_ / Vals.p_Model);
			}

			if( d > pClass->Suz_ )
			{
				d			= pClass->Suz_;
			}

			pClass->Suz_	-= d;

			if( pClass->Suz_ < 0.0000001 )
			{
				pClass->Suz_	= 0.0;
			}

			pClass->qv_		= d * pClass->Area_Rel;
			Vals.qv_Total	+= pClass->qv_;
		}
		else
		{
			pClass->qv_		= 0.0;
		}


		//-------------------------------------------------
		//  CALCULATE EVAPOTRANSPIRATION FROM ROOT ZONE DEFICIT

		if( Evaporation > 0.0 )
		{
			d		= Evaporation * (1.0 - pClass->Srz_ / Vals.p_Srz_Max);

			if( d > Vals.p_Srz_Max - pClass->Srz_ )
			{
				d		= Vals.p_Srz_Max - pClass->Srz_;
			}

			pClass->Srz_	+= d;
		}


		//-------------------------------------------------
		pClass->qo_		= Excess * pClass->Area_Rel;
		Vals.qo_Total	+= pClass->qo_;

		pClass->qt_		= pClass->qo_ + Vals.qs_Total;
	}

	Vals.qo_Total	+= Infiltration_Excess;

	Vals.qt_Total	= Vals.qo_Total + Vals.qs_Total;

	Vals.Sbar_		+= Vals.qs_Total - Vals.qv_Total;
}


///////////////////////////////////////////////////////////
//														 //
//														 //
//														 //
///////////////////////////////////////////////////////////

//---------------------------------------------------------
bool CTOPMODEL::Get_Weather(int iTimeStep, double &Precipitation, double &Evaporation, CSG_String &Time)
{
	CSG_Table_Record	*pRecord;

	if( m_pWeather && (pRecord = m_pWeather->Get_Record(iTimeStep)) != NULL )
	{
		Precipitation	= pRecord->asDouble(m_fP);
		Evaporation		= pRecord->asDouble(m_fET);

		if( m_fTime >= 0 )
		{
			Time		= pRecord->asString(m_fTime);
		}
		else
		{
			Time.Printf(SG_T("%d"), iTimeStep);
		}

		return( true );
	}

	Precipitation	= 0.0;
	Evaporation		= 0.0;

	return( false );
}


///////////////////////////////////////////////////////////
//														 //
//														 //
//														 //
///////////////////////////////////////////////////////////

//---------------------------------------------------------
#define NEWTON_EPSILON		0.001
#define NEWTON_MAXITER		100
#define	NEWTON_NTERMS		10

//---------------------------------------------------------
double CTOPMODEL::Get_Infiltration(double t, double R)
{
	int		i, j, factorial;

	double	f, f_, f1, f2, fc, R2, cnst, pt, psi_dtheta, sum;


	if( R <= 0.0 )
	{
		inf_cumf		= 0.0;
		inf_bPonding	= 0;

		return( 0.0 );
	}

	psi_dtheta	= Vals.p_Psi * Vals.p_dTheta;

	if( !inf_bPonding )
	{
		if( inf_cumf )
		{
			f1				= inf_cumf;
			R2				= -Vals.p_K0 / Vals.p_Model * (psi_dtheta + f1) / (1 - exp(f1 / Vals.p_Model));

			if( R2 < R )
			{
				f_				= inf_cumf;
				pt				= t - dTime;
				inf_bPonding	= 1;

				goto cont1;
			}
		}

		f2				= inf_cumf + R * dTime;
		R2				= -Vals.p_K0 / Vals.p_Model * (psi_dtheta + f2) / (1 - exp(f2 / Vals.p_Model));

		if( f2 == 0.0 || R2 > R )
		{
			f				= R;
			inf_cumf		+= f * dTime;
			inf_bPonding	= 0;

			return( f );
		}

		f_				= inf_cumf + R2 * dTime;

		for(i=0; i<NEWTON_MAXITER; i++)
		{
			R2				= -Vals.p_K0 / Vals.p_Model * (psi_dtheta + f_) / (1 - exp(f_ / Vals.p_Model));

			if( R2 > R )
			{
				f1				= f_;
				f_				= (f_ + f2) / 2.0;
				f				= f_ - f1;
			}
			else
			{
				f2				= f_;
				f_				= (f_ + f1) / 2.0;
				f				= f_ - f2;
			}

			if( fabs(f) < NEWTON_EPSILON )
				break;
		}

		if( i == NEWTON_MAXITER )
		{
			// G_set_d_null_value(&f, 1);
			return( 0.0 );
		}

		pt				= t - dTime + (f_ - inf_cumf) / R;

		if( pt > t )
		{
			f				= R;
			inf_cumf		+= f * dTime;
			inf_bPonding	= 0;

			return( f );
		}

cont1:
		cnst			= 0.0;
		factorial		= 1;
		fc				= (f_ + psi_dtheta);

		for(j=1; j<=NEWTON_NTERMS; j++)
		{
			factorial		*= j;
			cnst			+= pow(fc / Vals.p_Model, (double) j) / (double) (j * factorial);
		}

		cnst			= log(fc) - (log(fc) + cnst) / exp(psi_dtheta / Vals.p_Model);
		f_				+= R * (t - pt) / 2.0;
		inf_bPonding	= 1;
	}

	for(i=0; i<NEWTON_MAXITER; i++)
	{
		fc				= f_ + psi_dtheta;
		sum				= 0.0;
		factorial		= 1;

		for(j=1; j<=NEWTON_NTERMS; j++)
		{
			factorial		*= j;
			sum				+= pow(fc / Vals.p_Model, (double) j) / (double) (j * factorial);
		}

		f1				= - (log(fc) - (log(fc) + sum) / exp(psi_dtheta / Vals.p_Model) - cnst) / (Vals.p_K0 / Vals.p_Model) - (t - pt);
		f2				= (exp(f_ / Vals.p_Model) - 1.0) / (fc * Vals.p_K0 / Vals.p_Model);
		f				= - f1 / f2;
		f_				+= f;

		if( fabs(f) < NEWTON_EPSILON )
			break;
	}

	if( i == NEWTON_MAXITER )
	{
		// G_set_d_null_value(&f, 1);
		return( 0.0 );
	}

	if( f_ < inf_cumf + R )
	{
		f				= (f_ - inf_cumf) / dTime;
		inf_cumf		= f_;
		f_				+= f * dTime;
	}

	return( f );
}

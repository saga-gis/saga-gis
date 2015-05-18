/**********************************************************
 * Version $Id$
 *********************************************************/
/*******************************************************************************
    IsochronesVar.cpp
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

#include "IsochronesVar.h"
#include "Helper.h"

#ifndef max
#define max(a,b) (((a) > (b)) ? (a) : (b))
#endif


//-----------------------------------------------------
CIsochronesVar::CIsochronesVar(void){

	Parameters.Set_Name(_TL("Isochrones Variable Speed"));
	Set_Author(_TL("V.Olaya (c) 2004, V.Wichmann (c) 2015"));
	Parameters.Set_Description(_TW(
		"Calculation of isochrones with variable speed.\n"
		"In case a cell in an optional input grid is NoData, the corresponding parameter value will "
		"be used instead of skipping this cell.\n\n"
		"References:\n"
		"1. Al-Smadi, Mohammad: Incorporating spatial and temporal variation of "
		"watershed response in a gis-based hydrologic model. Faculty of the Virginia Polythecnic"
		"Insitute and State University. MsC Thesis. 1998\n"
		"Available at scholar.lib.vt.edu/theses/available/etd-121698-112858/unrestricted/smadi.pdf"
		"2. Martínez Álvarez, V.; Dal-Ré Tenreiro, R.; García García, A. I.; Ayuga Téllez, F. "
		"Modelación distribuida de la escorrentía superficial en pequeñas cuencas mediante SIG. Evaluación experimental.\n"
		"3. Olaya, V. Hidrologia computacional y modelos digitales del terreno. Alqua. 536 pp. 2004\n\n"));

	Parameters.Add_Grid(NULL,
						"DEM",
						_TL("Elevation"),
						_TL(""),
						PARAMETER_INPUT);

	Parameters.Add_Grid(NULL,
						"SLOPE",
						_TL("Slope"),
						_TL(""),
						PARAMETER_INPUT);

	Parameters.Add_Grid(NULL,
						"FLOWACC",
						_TL("Catchment Area"),
						_TL(""),
						PARAMETER_INPUT);

	Parameters.Add_Grid(NULL,
						"CN",
						_TL("Curve Number"),
						_TL(""),
						PARAMETER_INPUT_OPTIONAL);

	Parameters.Add_Grid(NULL,
						"MANNING",
						_TL("Manning's N"),
						_TL(""),
						PARAMETER_INPUT_OPTIONAL);

	Parameters.Add_Value(NULL,
						"AVGMANNING",
						_TL("Avg. Manning's N"),
						_TL(""),
						PARAMETER_TYPE_Double,
						0.15);

	Parameters.Add_Value(NULL,
						"AVGCN",
						_TL("Avg. Curve Number"),
						_TL(""),
						PARAMETER_TYPE_Double,
						75);

	Parameters.Add_Value(NULL,
						"THRSMIXED",
						_TL("Mixed Flow Threshold (ha)"),
						_TL(""),
						PARAMETER_TYPE_Double,
						18);

	Parameters.Add_Value(NULL,
						"THRSCHANNEL",
						_TL("Channel Definition Threshold (ha)"),
						_TL(""),
						PARAMETER_TYPE_Double,
						360);

	Parameters.Add_Value(NULL,
						"AVGRAINFALL",
						_TL("Avg. Rainfall Intensity (mm/h)"),
						_TL(""),
						PARAMETER_TYPE_Double,
						1);


	Parameters.Add_Value(NULL,
						"CHANSLOPE",
						_TL("Channel side slope(m/m)"),
						_TL(""),
						PARAMETER_TYPE_Double,
						0.5);

	Parameters.Add_Value(NULL,
						"MINSPEED",
						_TL("Min. Flow Speed (m/s)"),
						_TL(""),
						PARAMETER_TYPE_Double,
						0.05);

	Parameters.Add_Grid(NULL,
						"TIME",
						_TL("Time Out(h)"),
						_TL(""),
						PARAMETER_OUTPUT,
						true,
						SG_DATATYPE_Double);

	Parameters.Add_Grid(NULL,
						"SPEED",
						_TL("Speed (m/s)"),
						_TL(""),
						PARAMETER_OUTPUT,
						true,
						SG_DATATYPE_Double);

}//constructor


//-----------------------------------------------------
CIsochronesVar::~CIsochronesVar(void){
	Execute_Finish();
}


//-----------------------------------------------------
void CIsochronesVar::_CalculateTime(int x, int y)
{
	CSG_Grid_Stack	Stack;
	double	dDist = 1;
    double	dD = 0;
    double	dSlope;
    double	dSpeed;
    double	dQ = 0;
    double	dH;
    double	dSup;
    double	dInf;
    double	dAcc;
    double	dArea = 0;
    double	dPerim;
    double	dDif;
    double	dManning;
	double	dCN;
    double	dI = 0;
	int		iIter = 0;
	int		iDir;


	Stack.Push(x, y);

	//-----------------------------------------------------
	while( Stack.Get_Size() > 0 && Process_Get_Okay() )
	{
		Stack.Pop(x, y);

		if (m_pCN != NULL && !m_pCN->is_NoData(x, y))
		{
			dCN = m_pCN->asDouble(x, y);
		}
		else
		{
			dCN = m_dCN;
		}

		dI = Runoff(m_dRainfall, dCN);
		dI /= 3600.0;	// in mm/s
        dI /= 1000.0;	// m/s of runoff;

		iDir  = m_Direction.asInt(x, y);
		dDist = Get_Length(iDir);	// length to previous, i.e. successor cell

		dSlope = m_pSlope->asDouble(x, y);
		dSlope = fabs(tan(dSlope));
		dSlope = max(0.001, dSlope);

		dAcc = m_pCatchArea->asDouble(x, y);

        if (dAcc < m_dMixedThresh)
		{
            dD = sqrt(2.0 * dAcc / 3.14159);
			
			if (m_pManning != NULL && !m_pManning->is_NoData(x, y))
			{
				dManning = m_pManning->asDouble(x, y);
			}
			else
			{
				dManning = m_dManning;
			}

            dSpeed = max(m_dMinSpeed, pow(dI * dD, 0.4) * pow(dSlope, 0.3) / pow(dManning, 0.6));
        }
        else
		{
			if (dAcc < m_dChannelThresh)
			{
				dManning = 0.06;
			}
			else
			{
				dManning= 0.05;
			}

            dQ		= dI * dAcc; // Q m3/s
            dSup	= 60;
            dInf	= 0;
            dH		= 2;
            dArea	= dH * dH / m_dChannelSlope;
            dPerim	= 2.0 * (dH / m_dChannelSlope + sqrt(dH * dH + pow(dH / m_dChannelSlope, 2.0)));
            dDif	= (sqrt(dSlope) * pow(dArea, 5.0 / 3.0) / pow(dPerim, 2.0 / 3.0) / dManning) - dQ;
            iIter	= 0;

			do
			{
                if (dDif > 0)
				{
                    dSup	= dH;
                    dH		= (dInf + dH) / 2.0;
                }
                else if (dDif < 0)
				{
                    dInf	= dH;
                    dH		= (dSup + dH) / 2.0;
                }

                dArea	= dH * dH / m_dChannelSlope;
                dPerim	= 2.0 * (dH / m_dChannelSlope + sqrt(dH * dH + pow(dH / m_dChannelSlope, 2.0)));
                dDif	= (sqrt(dSlope) * pow(dArea, 5.0 / 3.0) / pow(dPerim, 2.0 / 3.0) / dManning) - dQ;
				iIter++;
            }
			while (fabs(dDif) > 0.1);

            dSpeed = max(m_dMinSpeed, dQ / dArea);
        }

		int ix = Get_xTo(iDir, x);
		int iy = Get_yTo(iDir, y);

		m_pTime->Set_Value(x, y, m_pTime->asDouble(ix, iy) + dDist / dSpeed);
		m_pSpeed->Set_Value(x, y, dSpeed);

		//-------------------------------------------------
		for(int i=0; i<8; i++)
		{
			ix	= Get_xFrom(i, x);
			iy	= Get_yFrom(i, y);

			if( m_pDEM->is_InGrid(ix, iy) && i == m_Direction.asInt(ix, iy) )
			{
				Stack.Push(ix, iy);
			}
		}
	}

	return;
}


//-----------------------------------------------------
double CIsochronesVar::Runoff(
        double dRainfall,
        double dCN) {

    double dS;
    double dRunoff;

    dS = (25400.0 / dCN) - 254;

    if (dRainfall < (0.2 * dS)) {
        return 0.1; //can´t be 0, because that would mean speed = 0
    }// if

    dRunoff = pow(dRainfall - 0.2 * dS, 2)
            / (dRainfall + 0.8 * dS);

    return dRunoff;

}// method


//-----------------------------------------------------
void CIsochronesVar::ZeroToNoData(void){


    for(int y=0; y<Get_NY() && Set_Progress(y); y++)
	{
		#pragma omp parallel for
		for(int x=0; x<Get_NX(); x++)
		{
            if (m_pTime->asDouble(x, y) == 0)
			{
				m_pTime->Set_NoData(x, y);
			}

            if (m_pSpeed->asDouble(x, y) == 0)
			{
				m_pSpeed->Set_NoData(x, y);
			}
        }// for
    }// for

}//method


//-----------------------------------------------------
bool CIsochronesVar::On_Execute(void){

	m_pDEM = Parameters("DEM")->asGrid();
	m_pTime = Parameters("TIME")->asGrid();
	m_pSpeed = Parameters("SPEED")->asGrid();
	m_pManning = Parameters("MANNING")->asGrid();
	m_pSlope = Parameters("SLOPE")->asGrid();
	m_pCatchArea = Parameters("FLOWACC")->asGrid();
	m_pCN = Parameters("CN")->asGrid();
	m_dMixedThresh = Parameters("THRSMIXED")->asDouble() * 10000;
	m_dChannelThresh = Parameters("THRSCHANNEL")->asDouble() * 10000;
	m_dCN = Parameters("AVGCN")->asDouble();
	m_dManning = Parameters("AVGMANNING")->asDouble();
	m_dRainfall = Parameters("AVGRAINFALL")->asDouble();
	m_dChannelSlope = Parameters("CHANSLOPE")->asDouble();
	m_dMinSpeed = Parameters("MINSPEED")->asDouble();

	m_pTime->Assign(0.0);

	m_Direction.Create(*Get_System(), SG_DATATYPE_Char);
	m_Direction.Set_NoData_Value(-1);

	Init_FlowDirectionsD8(m_pDEM, &m_Direction);

	return( true );

}//method


//-----------------------------------------------------
bool CIsochronesVar::On_Execute_Finish()
{
	m_Direction.Destroy();

	return( true );
}


//-----------------------------------------------------
bool CIsochronesVar::On_Execute_Position(CSG_Point ptWorld, TSG_Module_Interactive_Mode Mode)
{
	int iX, iY;

	if(	Mode != MODULE_INTERACTIVE_LDOWN || !Get_Grid_Pos(iX, iY) )
	{
		return( false );
	}
	
	m_pTime->Assign(0.0);

	_CalculateTime(iX, iY);

	for(int y=0; y<Get_NY() && Set_Progress(y); y++)
	{
		#pragma omp parallel for
		for(int x=0; x<Get_NX(); x++)
		{
			m_pTime->Set_Value(x, y, m_pTime->asDouble(x,y)/3600.0);
        }
    }

	ZeroToNoData();

	DataObject_Update(m_pTime, SG_UI_DATAOBJECT_SHOW_LAST_MAP);

	return( true );

}//method

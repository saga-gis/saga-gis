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
    Foundation, Inc., 59 Temple Place, Suite 330, Boston, MA  02111-1307  USA
*******************************************************************************/

#include "IsochronesVar.h"
#include "Helper.h"

#ifndef max
#define max(a,b) (((a) > (b)) ? (a) : (b))
#endif

CIsochronesVar::CIsochronesVar(void){

	Parameters.Set_Name(_TL("Isochrones Variable Speed"));
	Parameters.Set_Description(_TW(
		"(c) 2004 by Victor Olaya. Cálculo del tiempo de salida con velocidad variable.\r\n"
		"References:\r\n"
		"1. Al-Smadi, Mohammad: Incorporating spatial and temporal variation of "
		"watershed response in a gis-based hydrologic model. Faculty of the Virginia Polythecnic"
		"Insitute and State University. MsC Thesis. 1998 \r\n"
		"Available at scholar.lib.vt.edu/theses/available/ etd-121698-112858/unrestricted/smadi.pdf"
		"2. Martínez Álvarez, V.; Dal-Ré Tenreiro, R.; García García, A. I.; Ayuga Téllez, F. "
		"Modelación distribuida de la escorrentía superficial en pequeñas cuencas mediante SIG. Evaluación experimental.\r\n "
		"3. Olaya, V. Hidrologia computacional y modelos digitales del terreno. Alqua. 536 pp. 2004"));

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
						GRID_TYPE_Double);

	Parameters.Add_Grid(NULL,
						"SPEED",
						_TL("Speed (m/s)"),
						_TL(""),
						PARAMETER_OUTPUT,
						true,
						GRID_TYPE_Double);

}//constructor


CIsochronesVar::~CIsochronesVar(void){
	Execute_Finish();
}


void CIsochronesVar::writeTimeOut(
        int iX1,
        int iY1,
        int iX2,
        int iY2) {

    double dDist = 1;
    double dD = 0;
    double dSlope;
    double dSpeed;
    double dQ = 0;
    double dH;
    double dSup;
    double dInf;
    double dAcc;
    double dArea = 0;
    double dPerim;
    double dDif;
    double dManning;
	double dCN;
    double dI = 0;
	int iIter;
	int iNextX, iNextY;

	if (iX1 < 0 || iX1 >= m_pDEM->Get_NX() || iY1 < 0 || iY1 >= m_pDEM->Get_NY()
            || m_pDEM->asFloat(iX1,iY1) == m_pDEM->Get_NoData_Value()) {
    }// if
	else {

		if (m_pCN!=NULL){
			dCN = m_pCN->asDouble(iX1, iY1);
			if (dCN == m_pCN->Get_NoData_Value()) {
                dCN = m_dCN;
			}// if
		}//if
		else{
			dCN = m_dCN;
		}//else
		dI = Runoff(m_dRainfall, dCN);
		dI /= 3600.0;// in mm/s
        dI /= 1000.0;// m/s of runoff;

		if (abs(iX1 - iX2 + iY1 - iY2) == 1) {
            dDist = m_pDEM->Get_Cellsize();
        }// if
        else {
            dDist = 1.44 * m_pDEM->Get_Cellsize();
        }// else
		dSlope = m_pSlope->asDouble(iX1,iY1);
		dSlope = fabs(tan(dSlope));
		dSlope = max(0.001, dSlope);
		dAcc = m_pCatchArea->asDouble(iX1,iY1);
        if (dAcc < m_dMixedThresh) {
            dD = sqrt(2.0 * dAcc / 3.14159);
			if (m_pManning!=NULL){
				dManning = m_pManning->asDouble(iX1, iY1);
				if (dManning == m_pManning->Get_NoData_Value()) {
	                dManning = m_dManning;
		        }// if
			}//id
			else{
				dManning = m_dManning;
			}//else
            dSpeed = max(m_dMinSpeed, pow(dI * dD, 0.4)
                    * pow(dSlope, 0.3) / pow(dManning, 0.6));
        }// if
        else{
			if (dAcc < m_dChannelThresh) {
				dManning = 0.06;
			}//if
			else{
				dManning= 0.05;
			}//if
            dQ = dI * dAcc; // Q m3/s
            dSup = 60;
            dInf = 0;
            dH = 2;
            dArea = dH * dH / m_dChannelSlope;
            dPerim = 2.0 * (dH / m_dChannelSlope + sqrt(dH * dH
					+ pow(dH / m_dChannelSlope, 2.0)));
            dDif = (sqrt(dSlope)
                    * pow(dArea, 5.0 / 3.0)
                    / pow(dPerim, 2.0 / 3.0) / dManning)
                    - dQ;
            iIter = 0;
			do {
                if (dDif > 0) {
                    dSup = dH;
                    dH = (dInf + dH) / 2.0;
                }// if
                else if (dDif < 0) {
                    dInf = dH;
                    dH = (dSup + dH) / 2.0;
                }// else if
                dArea = dH * dH / m_dChannelSlope;
                dPerim = 2.0 * (dH / m_dChannelSlope + sqrt(dH * dH
						+ pow(dH / m_dChannelSlope, 2.0)));
                dDif = (sqrt(dSlope)
                        * pow(dArea, 5.0 / 3.0)
                        / pow(dPerim, 2.0 / 3.0) / dManning)
                        - dQ;
				iIter++;
            }while (fabs(dDif) > 0.1);
            dSpeed = max(m_dMinSpeed, dQ / dArea);
        }// else

		m_pTime->Set_Value(iX1,iY1,m_pTime->asDouble(iX2,iY2) + dDist / dSpeed);
		m_pSpeed->Set_Value(iX1,iY1, dSpeed);

		for (int i = -1; i<2; i++){
			for (int j = -1; j<2; j++){
				if (!(i == 0) || !(j == 0)) {
					getNextCell(m_pDEM, iX1 + i, iY1 + j, iNextX, iNextY);
					if (iNextY == iY1 && iNextX == iX1) {
						writeTimeOut(iX1 + i, iY1 + j, iX1, iY1);
					}// if
				}//if
			}//for
		}//for

    }// else

}// method

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

void CIsochronesVar::ZeroToNoData(void){


    for(int y=0; y<Get_NY() && Set_Progress(y); y++){
		for(int x=0; x<Get_NX(); x++){
            if (m_pTime->asDouble(x,y) == 0){
				m_pTime->Set_Value(x,y,m_pTime->Get_NoData_Value());
			}//if
            if (m_pSpeed->asDouble(x,y) == 0){
				m_pSpeed->Set_Value(x,y,m_pSpeed->Get_NoData_Value());
			}//if
        }// for
    }// for

}//method

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

	m_pTime->Assign((double)0);

	return true;

}//method

bool CIsochronesVar::On_Execute_Finish()
{
	return( true );
}

bool CIsochronesVar::On_Execute_Position(CSG_Point ptWorld, TSG_Module_Interactive_Mode Mode)
{
	int iX, iY;

	if(	Mode != MODULE_INTERACTIVE_LDOWN || !Get_Grid_Pos(iX, iY) )
	{
		return( false );
	}
	
	m_pTime->Assign((double)0);

	writeTimeOut(iX, iY, iX, iY);

	for(int y=0; y<Get_NY() && Set_Progress(y); y++){
		for(int x=0; x<Get_NX(); x++){
			m_pTime->Set_Value(x,y,m_pTime->asDouble(x,y)/3600.);
        }// for
    }// for

	ZeroToNoData();

	DataObject_Update(m_pTime, true);

	return (true);

}//method

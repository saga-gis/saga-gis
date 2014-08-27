/**********************************************************
 * Version $Id: SimulateVariableWind.cpp 911 2011-02-14 16:38:15Z reklov_w $
 *********************************************************/
/*******************************************************************************
    SimulateVariableWind.cpp
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

//#include "../../GRID/Grid_Shapes/Grid2Contour.h"
#include "SimulateVariableWind.h"
#include <string>
#include <vector>
#include <fstream>
#include <locale>
#include <time.h>

#define NODATA -9999;

#define KMH2FTMIN (1000. / 0.3048 / 60.)
#define FTMIN2MMIN 0.3048
#define BTU2KCAL 0.252164401
#define FT2M 0.3048
#define NO_TIME_LIMIT -1
#define WINDOW_WIDTH 5

//#define TIME_INTERVAL_IN_CURVES 10.
//#define TIME_LIMIT 240
//#define MAX_FIRES_IN_CONTOURS 2000
//#define MAX_CONTOUR_VALUE 1500

CSimulateVariableWind::CSimulateVariableWind(void){
	
	Set_Name		(_TL("Simulación (Viento variable)"));

	Set_Description	(_TW(
		"(c) 2004 by Victor Olaya. Simulación con viento variable."
	));

	Parameters.Add_Grid(NULL, 
						"DEM", 
						_TL("MDE"), 
						_TL(""), 
						PARAMETER_INPUT);
	
	Parameters.Add_Grid(NULL, 
						"FUEL", 
						_TL("Modelo de combustible"), 
						_TL(""), 
						PARAMETER_INPUT);

	Parameters.Add_Grid_List(NULL, 
							"WINDSPD", 
							_TL("Velocidad del viento"),
							_TL("Velocidad del viento (km/h)"),
							PARAMETER_INPUT);
	
	Parameters.Add_Grid_List(NULL, 
							"WINDDIR", 
							_TL("Dirección del viento"), 
							_TL("Dirección del viento (grados desde el norte en sentido antihorario)"),
							PARAMETER_INPUT);

	Parameters.Add_Grid(NULL, 
						"M1H", 
						_TL("Humedad del combustible muerto en 1-hora"), 
						_TL(""), 
						PARAMETER_INPUT);

	Parameters.Add_Grid(NULL, 
						"M10H", 
						_TL("Humedad del combustible muerto en 10-horas"), 
						_TL(""), 
						PARAMETER_INPUT);

	Parameters.Add_Grid(NULL, 
						"M100H", 
						_TL("Humedad del combustible muerto en 100-horas"), 
						_TL(""), 
						PARAMETER_INPUT);

	Parameters.Add_Grid(NULL, 
						"MHERB", 
						_TL("Humedad del combustible herbáceo vivo"), 
						_TL(""), 
						PARAMETER_INPUT);

	Parameters.Add_Grid(NULL, 
						"MWOOD", 
						_TL("Humedad del combustible leñoso vivo"), 
						_TL(""), 
						PARAMETER_INPUT);
	
	Parameters.Add_Grid(NULL, 
						"TIME", 
						_TL("Tiempo"), 
						_TL(""), 
						PARAMETER_OUTPUT);

	Parameters.Add_Grid(NULL, 
						"FLAME", 
						_TL("Altura de llama"), 
						_TL("Altura de llama (m)"), 
						PARAMETER_OUTPUT);

	Parameters.Add_Grid(NULL, 
						"INTENSITY", 
						_TL("Intensidad"), 
						_TL("Intensidad (Kcal/m)"), 
						PARAMETER_OUTPUT);

	Parameters.Add_Value(NULL, 
						"IGNTIME", 
						_TL("Tiempo de inicio (min)"), 
						_TL(""), 
						PARAMETER_TYPE_Double, 
						0, 
						0, 
						true);

	Parameters.Add_Value(NULL, 
						"INTERVAL", 
						_TL("Intervalo de tiempo entre capas (min)"), 
						_TL(""), 
						PARAMETER_TYPE_Double, 
						30, 
						1, 
						true);

	Parameters.Add_Value(NULL, 
						"SIMULATIONTIME", 
						_TL("Tiempo de simulacion (min)"), 
						_TL(""), 
						PARAMETER_TYPE_Double, 
						180, 
						1, 
						true);

	Parameters.Add_Value(NULL, 
						"DEFAULTWINDDIR", 
						_TL("Dirección del viento"), 
						_TL("Dirección del viento (grados desde el norte)"),
						PARAMETER_TYPE_Double, 
						0);

	Parameters.Add_Value(NULL, 
						"DEFAULTWINDSPD", 
						_TL("Velocidad del viento"),
						_TL("Velocidad del viento (km/h)"),
						PARAMETER_TYPE_Double, 
						0);
	
	Parameters.Add_Value(NULL, 
						"COORDX", 
						_TL("Coordenada X"), 
						_TL("Coordenada X del punto de ignición (sólo si no se usa grid de puntos de ignición)"), 
						PARAMETER_TYPE_Double, 
						0);

	Parameters.Add_Value(NULL, 
						"COORDY", 
						_TL("Coordenada Y"), 
						_TL("Coordenada Y del punto de ignición (sólo si no se usa grid de puntos de ignición)"), 
						PARAMETER_TYPE_Double, 
						0);

	Parameters.Add_FilePath(NULL,
							"REPORTFILE",
							_TL("Informe"),
							_TL("Informe"),
							_TL(""),
							_TL(""),
							true);
}//constructor

CSimulateVariableWind::~CSimulateVariableWind(void){}

bool CSimulateVariableWind::On_Execute(void){

	AssignParameters();
	CalculateFire();
	
	DeleteObjects();
	return true;

}//method

void CSimulateVariableWind::DeleteObjects(){

	delete m_pAspectGrid;
	delete m_pSlopeGrid;

	delete m_pReactionIntensityGrid;
	delete m_pEffectiveWindGrid;
	delete m_pHeatPerUnitAreaGrid;

	m_CentralPoints	.Clear();
	m_AdjPoints		.Clear();

	if (m_bDeleteWindSpdGrid){
		delete m_pWindSpdGrids[0];
	}//if

	if (m_bDeleteWindDirGrid){
		delete m_pWindDirGrids[0];
	}//if

}//method

bool CSimulateVariableWind::AssignParameters(){

	int x,y;

	m_pDEM = Parameters("DEM")->asGrid();
	m_pFuelGrid = Parameters("FUEL")->asGrid();

	m_iWindDirGrids	= Parameters("WINDDIR")->asInt();
	m_pWindDirGrids	=(CSG_Grid **)Parameters("WINDDIR")->asPointer();
	m_iWindSpdGrids	= Parameters("WINDSPD")->asInt();
	m_pWindSpdGrids	=(CSG_Grid **)Parameters("WINDSPD")->asPointer();
	m_pM1Grid = Parameters("M1H")->asGrid();
	m_pM10Grid = Parameters("M10H")->asGrid();
	m_pM100Grid = Parameters("M100H")->asGrid();
	m_pMHerbGrid = Parameters("MHERB")->asGrid();
	m_pMWoodGrid = Parameters("MWOOD")->asGrid();
	m_pTimeGrid = Parameters("TIME")->asGrid();
	m_pFlameGrid = Parameters("FLAME")->asGrid();
	m_pIntensityGrid = Parameters("INTENSITY")->asGrid();

	m_fTimeLimit = Parameters("SIMULATIONTIME")->asInt();

	m_fIgnTime = Parameters("IGNTIME")->asDouble();
	m_fInterval = Parameters("INTERVAL")->asDouble();

	m_fWorldX = Parameters("COORDX")->asDouble();
	m_fWorldY = Parameters("COORDY")->asDouble();
	m_iGridX = (int) ((m_fWorldX - m_pDEM->Get_XMin()) / m_pDEM->Get_Cellsize());
	m_iGridY = (int) ((m_fWorldY - m_pDEM->Get_YMin()) / m_pDEM->Get_Cellsize());

    m_Catalog = Fire_FuelCatalogCreateStandard("Standard", 13);
    Fire_FlameLengthTable(m_Catalog, 500, 0.1);

	if (!m_iWindDirGrids){
		m_pWindDirGrids = new CSG_Grid*[1];
		m_pWindDirGrids[0] = SG_Create_Grid(m_pDEM);
		m_pWindDirGrids[0]->Assign(Parameters("DEFAULTWINDDIR")->asDouble());
		m_bDeleteWindDirGrid = true;
	}//if
	else{
		m_bDeleteWindDirGrid = false;
	}//else
	
	if (!m_iWindSpdGrids){
		m_pWindSpdGrids = new CSG_Grid*[1];
		m_pWindSpdGrids[0] = SG_Create_Grid(m_pDEM);
		m_pWindSpdGrids[0]->Assign(Parameters("DEFAULTWINDSPD")->asDouble());
		m_bDeleteWindSpdGrid = true;
	}//if
	else{
		m_bDeleteWindSpdGrid = false;
	}//else

	//substitute no-data values
	for(y=0; y<Get_NY() && Set_Progress(y); y++){		
		for(x=0; x<Get_NX(); x++){

			/*if (m_pWindSpdGrid->is_NoData(x, y)){
				m_pWindSpdGrid->Set_Value(x, y, 0.);
			}//if
			if (m_pWindDirGrid->is_NoData(x, y)){
				m_pWindDirGrid->Set_Value(x, y, 0.);
			}//if*/
			if (m_pM1Grid->is_NoData(x, y)){
				m_pM1Grid->Set_Value(x, y, 0.);
			}//if
			if (m_pM10Grid->is_NoData(x, y)){
				m_pM10Grid->Set_Value(x, y, 0.);
			}//if
			if (m_pM100Grid->is_NoData(x, y)){
				m_pM100Grid->Set_Value(x, y, 0.);
			}//if
			if (m_pMHerbGrid->is_NoData(x, y)){
				m_pMHerbGrid->Set_Value(x, y, 0.);
			}//if
			if (m_pMWoodGrid->is_NoData(x, y)){
				m_pMWoodGrid->Set_Value(x, y, 0.);
			}//if

		}//for
	}//for

	m_pReactionIntensityGrid	= SG_Create_Grid(m_pDEM, SG_DATATYPE_Double);
	m_pEffectiveWindGrid		= SG_Create_Grid(m_pDEM, SG_DATATYPE_Double);
	m_pHeatPerUnitAreaGrid		= SG_Create_Grid(m_pDEM, SG_DATATYPE_Double);

	//-----------------------------------------------------
	m_pSlopeGrid	= SG_Create_Grid(m_pDEM, SG_DATATYPE_Float);
	m_pAspectGrid	= SG_Create_Grid(m_pDEM, SG_DATATYPE_Float);

	for(y=0; y<Get_NY() && Set_Progress(y); y++)
	{
		for(x=0; x<Get_NX(); x++)
		{
			double	Slope, Aspect;

			if( m_pDEM->Get_Gradient(x, y, Slope, Aspect) )
			{
				m_pSlopeGrid ->Set_Value(x, y, Slope);
				m_pAspectGrid->Set_Value(x, y, Aspect);
			}
			else
			{
				m_pSlopeGrid ->Set_NoData(x, y);
				m_pAspectGrid->Set_NoData(x, y);
			}
		}
	}

	//-----------------------------------------------------
	m_pTimeGrid->Assign((double)0);

	return true;

}//method

void CSimulateVariableWind::CalculateFire(){
		
	Process_Set_Text(_TL("Simulando..."));

	m_CentralPoints	.Clear();
	m_AdjPoints		.Clear();

	m_CentralPoints.Clear();
	if (m_pDEM->is_InGrid(m_iGridX, m_iGridY,false)){
		m_CentralPoints.Add(m_iGridX, m_iGridY);
		m_pTimeGrid->Set_Value(m_iGridX, m_iGridY,0.0);
	}//if
	
	if (CalculateFireSpreading(m_fTimeLimit)){
		m_pTimeGrid->Set_NoData_Value(0.);
		CreateReport();
	}//if

}//method


int CSimulateVariableWind::CalculateFireSpreading(double fTimeLimit){

	int x,y;
	int x2,y2;
	int i,j;
	bool bReturn = false;
	/* neighbor's address*/   /* N  NE   E  SE   S  SW   W  NW */
	static int nX[8] =        {  0,  1,  1,  1,  0, -1, -1, -1};
    static int nY[8] =        {  1,  1,  0, -1, -1, -1,  0,  1};
	double fDist;			  /* distance to neighbor */
    double fAz;				  /* compass azimuth to neighbor (0=N) */	
	size_t modelNumber;       /* fuel model number at current cell */
    double moisture[6];       /* fuel moisture content at current cell */
    double dSpreadRate;       /* spread rate in direction of neighbor */
    double dSpreadTime;       /* time to spread from cell to neighbor */
    double dIgnTime;          /* time neighbor is ignited by current cell */
	double dWindSpd;
	double dWindDir;
	int iBurntCells = 0;

	while (m_CentralPoints.Get_Count()!=0){

		for (int iPt=0; iPt<m_CentralPoints.Get_Count();iPt++){

			x = m_CentralPoints.Get_X(iPt);
			y = m_CentralPoints.Get_Y(iPt);

			if (!m_pDEM->is_NoData(x,y) && !m_pFuelGrid->is_NoData(x,y)){

				modelNumber = (size_t) m_pFuelGrid->asInt(x, y);
				moisture[0] = m_pM1Grid->asDouble(x, y) / 100.;
				moisture[1] = m_pM10Grid->asDouble(x, y) / 100.;
				moisture[2] = m_pM100Grid->asDouble(x, y) / 100.;
				moisture[3] = m_pM100Grid->asDouble(x, y) / 100.;
				moisture[4] = m_pMHerbGrid->asDouble(x, y) / 100.;
				moisture[5] = m_pMWoodGrid->asDouble(x, y) / 100.;
				dWindSpd = getWindSpeed(x, y, m_pTimeGrid->asDouble(x,y)) * KMH2FTMIN; 
				dWindDir = 360. - getWindDirection(x, y, m_pTimeGrid->asDouble(x,y));
				Fire_SpreadNoWindNoSlope(m_Catalog, modelNumber, moisture);
				Fire_SpreadWindSlopeMax(m_Catalog, modelNumber, dWindSpd,
										 dWindDir, tan(m_pSlopeGrid->asDouble(x,y)),
										 m_pAspectGrid->asDouble(x,y));

				for (i = -2; i < 3 ; i++){
					for (j = -2; j < 3; j++){						
						if (i!= 0 || j!=0){					
							x2 = x + i;
							y2 = y + j;
							if (m_pTimeGrid->is_InGrid(x2,y2,false)){
								fAz = getAzimuth(i,j);
								Fire_SpreadAtAzimuth(m_Catalog, modelNumber, fAz, FIRE_BYRAMS);
								dSpreadRate = Fuel_SpreadAny(m_Catalog, modelNumber); // in ft/min (awkward...)					
								dSpreadRate *= FTMIN2MMIN; //a bit better...
								if (dSpreadRate > Smidgen){
									fDist = sqrt(pow(i,2.) + pow(j,2.)) * m_pTimeGrid->Get_Cellsize();
									dSpreadTime = fDist / dSpreadRate;							
									dIgnTime = 	m_pTimeGrid->asDouble(x,y) + dSpreadTime;
									if (dIgnTime < fTimeLimit){
										if (m_pTimeGrid->asDouble(x2,y2) == 0.0 
												|| m_pTimeGrid->asDouble(x2, y2)>dIgnTime){
											m_pTimeGrid->Set_Value(x2, y2, dIgnTime);
											m_AdjPoints.Add(x2,y2);
											Fire_FlameScorch(m_Catalog, modelNumber, FIRE_FLAME);
											m_pFlameGrid->Set_Value(x2, y2, Fuel_FlameLength(m_Catalog, modelNumber) * FT2M);									
											m_pIntensityGrid->Set_Value(x2, y2, Fuel_ByramsIntensity(m_Catalog, modelNumber)
																		* BTU2KCAL / FT2M );
											m_pReactionIntensityGrid->Set_Value(x2, y2, Fuel_RxIntensity(m_Catalog, modelNumber));
											m_pHeatPerUnitAreaGrid->Set_Value(x2, y2, Fuel_HeatPerUnitArea(m_Catalog, modelNumber));
											m_pEffectiveWindGrid->Set_Value(x2,y2, Fuel_EffectiveWind(m_Catalog, modelNumber));
										}//if
									}//if
								}//if					
							}//if
						}//if				
					}//for
				}//for

			}//if
		}//for

		m_CentralPoints.Clear();
		for (int i=0; i<m_AdjPoints.Get_Count(); i++){
			x= m_AdjPoints.Get_X(i);
			y = m_AdjPoints.Get_Y(i);
			m_CentralPoints.Add(x, y);
			iBurntCells++;
		}//for
		m_AdjPoints.Clear();

		if (fTimeLimit == NO_TIME_LIMIT){
			Process_Get_Okay(true);
		}//if		

	}//while

	return iBurntCells;

}//method

double CSimulateVariableWind::getWindSpeed(int iX, 
										  int iY,
										  double fTime /*in mins*/){

	if (fTime <= 0){
		return m_pWindSpdGrids[0]->asDouble(iX,iY);
	}//if

	if (fTime >= (m_iWindSpdGrids - 1) * m_fInterval){
		return m_pWindSpdGrids[m_iWindSpdGrids - 1]->asDouble(iX,iY);
	}//if

	double fClass = fTime / ((m_iWindSpdGrids - 1) * m_fInterval);

	int iClass1 = (int)floor(fClass);
	int iClass2 = (int)ceil(fClass);

	double fValue1 = m_pWindSpdGrids[iClass1]->asDouble(iX,iY);
	double fValue2 = m_pWindSpdGrids[iClass2]->asDouble(iX,iY);

	double fSpeed = fValue1 + (fValue2 - fValue1) * (fClass - iClass1);

	return fSpeed;

}//

double CSimulateVariableWind::getWindDirection(int iX, 
											int iY,
											double fTime /*in mins*/){

	if (fTime <= 0){
		return m_pWindDirGrids[0]->asDouble(iX,iY);
	}//if

	if (fTime >= (m_iWindDirGrids - 1) * m_fInterval){
		return m_pWindDirGrids[m_iWindDirGrids - 1]->asDouble(iX,iY);
	}//if

	double fClass = fTime / ((m_iWindDirGrids - 1) * m_fInterval);

	int iClass1 = (int)floor(fClass);
	int iClass2 = (int)ceil(fClass);

	double fValue1 = m_pWindDirGrids[iClass1]->asDouble(iX,iY);
	if (fValue1 > 180){
		fValue1 = - (fValue1 - 180.);
	}//if

	double fValue2 = m_pWindDirGrids[iClass2]->asDouble(iX,iY);
	if (fValue2 > 180){
		fValue2 = - (fValue2 - 180.);
	}//if

	double fDir = fValue1 + (fValue2 - fValue1) * (fClass - iClass1);

	if (fDir < 0){
		fDir += 360;
	}//if

	return fDir;

}//

void CSimulateVariableWind::CreateReport(){

	CSG_String sReportFile, sFile;
	int i;

	if (Parameters("REPORTFILE")->asString() != NULL){
		
		sReportFile = Parameters("REPORTFILE")->asString();
		
		CalculateReportParameters();

		char cDate[81];
		time_t curTime;
		tm * timeinfo;
		time(&curTime);
		timeinfo = gmtime(&curTime);  

	//	std::locale spanish("esp");
	//	std::locale::global(spanish);
		strftime(cDate, 80, "%#x", timeinfo);

		sFile += "\t\t\t SIMULACIÓN realizada el ";
		sFile += cDate;
		sFile += "\n\t\t ==============================================================================";
		sFile += "\n\n\t Parámetros de entrada\n";
		sFile += "\t --------------------------------\n\n";
		sFile += "\t\t Modelos de combustible (por area) \n:  ";
		for (i = 0; i < 12; i++){
			if (m_pAreaByFuelModel[i]){
				sFile += "\t\t\t * ";
				sFile += SG_Get_String(i + 1, 0) + " : " + SG_Get_String(m_pAreaByFuelModel[i]) + " ha\n";
			}//if
		}//for		
		sFile += "\t\t Humedad de los combustibles muertos (1 h): ";
		sFile += SG_Get_String(m_fDeadFuelMoisture) + " %\n";
		sFile += "\t\t Velocidad del viento a media llama: \n" ;
		for(i = 0; i < m_iWindSpdGrids; i++){
			sFile	+= "\t\t\t * ";
			sFile	+= SG_Get_String(i * m_fInterval, 0) + " min: " 
					 + SG_Get_String(m_pMeanWindSpd[i]) + "Km/h\n";
		}//for
		sFile += "\t\t Dirección del vector viento, desde el norte geográfico: \n";
		for(i = 0; i < m_iWindDirGrids; i++){
			sFile += "\t\t\t * ";
			sFile += SG_Get_String(i * m_fInterval, 0) + " min: " 
					+ SG_Get_String(m_pMeanWindDir[i]) + "º\n";
		}//for

		sFile += "\t\t Pendiente del terreno media: ";	sFile += SG_Get_String(m_fSlope) + " %\n";       
		sFile += "\t\t Orientación del terreno: ";	sFile += SG_Get_String(m_fAspect) + "º\n";
		sFile += "\t\t Foco de partida: X = ";	sFile += SG_Get_String(Parameters("COORDX")->asDouble(), 0) + " / Y = "
				+ SG_Get_String(Parameters("COORDY")->asDouble(), 0) + "\n";
		sFile += "\t\t Tiempo de simulación: 3.0 h";

		sFile += "\n\n\t Resultado de la simulación\n";
		sFile += "\t --------------------------------\n\n";	
		sFile += "\t\t Velocidad de propagación: ";					sFile += SG_Get_String(m_fMeanSpeed) + " m/min\n";
		sFile += "\t\t Calor por unidad de area: ";					sFile += SG_Get_String(m_fHeatPerUnitArea) + " kJ/m^2\n"; //revisar unidades!!
		sFile += "\t\t Intensidad de la línea de fuego: ";			sFile += SG_Get_String(m_fIntensity) + " kCal/m\n";
		sFile += "\t\t Longitud de la llama: ";						sFile += SG_Get_String(m_fFlameHeight) + " m\n";
		sFile += "\t\t Intensidad de reacción: ";					sFile += SG_Get_String(m_fReactionIntensity) + " kCal/m2\n";
		sFile += "\t\t Velocidad efectiva del viento: ";			sFile += SG_Get_String(m_fEffectiveWind / KMH2FTMIN) + " Km/h\n";
		sFile += "\t\t Dirección de máxima propagación, desde el norte geográfico: ";	sFile += SG_Get_String(m_fMaxSpreadDir) + "º\n";
		sFile += "\t\t Area: ";										sFile += SG_Get_String(m_fArea / 10000) + " ha\n";
		sFile += "\t\t Perímetro: ";								sFile += SG_Get_String(m_fPerimeter) + "m\n";
		//Razón Longitud/Ancho:                   1.9
		sFile += "\t\t Distancia de propagación hacia delante: ";	sFile += SG_Get_String(m_fFrontDistance) + " m\n";
		sFile += "\t\t Distancia de propagación hacia atrás: ";		sFile +=  SG_Get_String(m_fRearDistance) + " m\n";

		CSG_File	file;

		if( file.Open(sReportFile, SG_FILE_W, false) )
		{
			file.Write(sFile);
		}
	}//if

}//method

void CSimulateVariableWind::CalculateReportParameters(){

	int i;
	int x,y;
	int iFuelModel;
	int iX, iY, iXOrig, iYOrig;
	int iCells = 0;
	int iOffsetX[] = {0, 1, 1, 1, 0, -1, -1, -1};
    int iOffsetY[] = {-1, -1, 0, 1, 1, 1, 0, -1};
    int iDirection;
    int iDir = 1;
    int iNewDirection[] = {5, 6, 7, 0, 1, 2, 3, 4};
	double fAspect;
	double fDist;
	
	m_fFrontDistance = -1;
	m_fRearDistance = 999999999999999999.f;

	m_fSlope = m_fAspect = m_fFlameHeight = m_fIntensity = m_fMeanSpeed = 0;
	m_fPerimeter = m_fArea = 0;
	m_fDeadFuelMoisture	= m_fHeatPerUnitArea = m_fEffectiveWind = m_fReactionIntensity = 0;

	m_pMeanWindDir = new double[m_iWindDirGrids];
	for (i = 0; i < m_iWindDirGrids; i++){
		m_pMeanWindDir[i] = 0;
	}//for
	m_pMeanWindSpd = new double[m_iWindSpdGrids];
	for (i = 0; i < m_iWindSpdGrids; i++){
		m_pMeanWindSpd[i] = 0;
	}//for

	m_pAreaByFuelModel = new double[12];
	for (i = 0; i < 12; i++){
		m_pAreaByFuelModel[i] = 0;
	}//for

	for(y=0; y<Get_NY(); y++){
		for(x=0; x<Get_NX(); x++){
			if (!m_pTimeGrid->is_NoData(x,y)){
				m_fArea += m_pTimeGrid->Get_Cellarea();
				if (!m_pFuelGrid->is_NoData(x,y)){
					iFuelModel = m_pFuelGrid->asInt(x,y) - 1;
					m_pAreaByFuelModel[iFuelModel] += m_pFuelGrid->Get_Cellarea();
				}//if
				m_fSlope += m_pSlopeGrid->asDouble(x,y);
				if (!m_pAspectGrid->is_NoData(x,y)){
					fAspect = m_pAspectGrid->asDouble(x,y);
					if (fAspect > 180){
						fAspect = 360 - fAspect;
					}//if
					m_fAspect += fAspect;
				}//if
				for (i = 0; i < m_iWindDirGrids; i++){
					fAspect = m_pWindDirGrids[i]->asDouble(x,y);
					if (fAspect > 180){
						fAspect = 360 - fAspect;
					}//if					
					m_pMeanWindDir[i] += fAspect;
				}//for
				for (i = 0; i < m_iWindSpdGrids; i++){
					m_pMeanWindSpd[i] += m_pWindSpdGrids[i]->asDouble(x,y);
				}//for
				m_fFlameHeight += m_pFlameGrid->asDouble(x,y);
				m_fIntensity += m_pIntensityGrid->asDouble(x,y);
				if (m_pM1Grid->asDouble(x,y) > 0){
					m_fDeadFuelMoisture += m_pM1Grid->asDouble(x,y);
				}//if				
				m_fHeatPerUnitArea += m_pHeatPerUnitAreaGrid->asDouble(x,y);
				m_fEffectiveWind += m_pEffectiveWindGrid->asDouble(x,y);
				m_fReactionIntensity += m_pReactionIntensityGrid->asDouble(x,y);
				iCells++;
			}//if
		}//for
	}//for

	m_fSlope /= (double)iCells;
	m_fAspect /= (double)iCells;
	m_fFlameHeight /= (double)iCells;
	m_fIntensity /= (double)iCells;
	m_fDeadFuelMoisture /= (double)iCells;
	m_fHeatPerUnitArea /= (double)iCells;
	m_fHeatPerUnitArea *= (BTU2KCAL * FT2M * FT2M);
	m_fEffectiveWind /= (double)iCells;
	m_fReactionIntensity /= (double)iCells;
	for (i = 0; i < m_iWindSpdGrids; i++){
		m_pMeanWindSpd[i] /= (double)iCells;
	}//for
	for (i = 0; i < m_iWindDirGrids; i++){
		m_pMeanWindDir[i] /= (double)iCells;
	}//for

	for (x = 0; x < Get_NX(); x++) {
	    for (y = 0; y < Get_NY(); y++) {
	        if (!m_pTimeGrid->is_NoData(x,y)) {
	            iX = x;
	            iY = y;
	            goto out;
	        }// if		
	    }// for
	}// for
out:

	iCells = 0;
	
	iXOrig = iX;
	iYOrig = iY;
	
	iDirection = 1;
	do {
	    if (iDirection > 7) {
	        iDirection =  iDirection % 8;
	    }//if
	    for (i = iDirection; i < iDirection + 8; i++) {
	        if (i > 7) {
	            iDir = i % 8;
	        }//if
	        else {
	            iDir = i;
	        }//else
	        if (!m_pTimeGrid->is_NoData(iX + iOffsetX[iDir],iY + iOffsetY[iDir])) {
	            iX = iX + iOffsetX[iDir];
				iY = iY + iOffsetY[iDir];	                        
	            iDirection = iNewDirection[iDir];
	            break;
	        }// if
	    }// for
		m_fPerimeter += sqrt(pow(iOffsetX[iDir],2.) + pow(iOffsetY[iDir],2.));
		fDist = sqrt(pow(iX - m_iGridX, 2.) + pow(m_iGridY - iY,2.)) * m_pTimeGrid->Get_Cellsize();
		if (fDist > m_fFrontDistance){
			m_fFrontDistance = fDist;
			m_fMaxSpreadDir = getAzimuth(iX - m_iGridX, iY - m_iGridY);
			m_fMaxSpreadDir = 360 - m_fMaxSpreadDir;
		}//if
		
		if (fDist < m_fRearDistance){
			m_fRearDistance = fDist;
		}//if
		
		m_fMeanSpeed += (fDist / m_fTimeLimit);
		
		iCells++;

	}while ((iY != iYOrig) || (iX != iXOrig));		

	m_fMeanSpeed /= (double)iCells; 
	m_fPerimeter *=  m_pTimeGrid->Get_Cellsize();

}//method

double CSimulateVariableWind::getAzimuth(double x, double y){


	double fAz;

	fAz = atan(fabs(y)/fabs(x)) * M_RAD_TO_DEG;

	if (y < 0){
		if (x < 0){
			fAz = 270 - fAz;
		}//if
		else if (x == 0){
			fAz = 180;
		}//else if
		else if (x > 0){
			fAz = 90 + fAz;
		}//else if
	}//if
	else{							
		if (x < 0){
			fAz = 270 + fAz;
		}//if
		else if (x == 0){
			fAz = 0;
		}//else if
		else if (x > 0){
			fAz = 90 - fAz;
		}//else if
	}//if

	return fAz;

}//method

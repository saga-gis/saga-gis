/*******************************************************************************
    ThresholdBuffer.cpp
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

#include "Grid_ThresholdBuffer.h"

#define BUFFER  1
#define FEATURE 2
#define THRESHOLD_ABSOLUTE 0
#define THRESHOLD_RELATIVE 1

CThresholdBuffer::CThresholdBuffer(void){

	CSG_Parameter * pNode;
	Parameters.Set_Name(_TL("Threshold Buffer"));
	Parameters.Set_Description(_TW(
		"(c) 2004 by Victor Olaya. Threshold Buffer Creation"));

	Parameters.Add_Grid(NULL,
						"FEATURES",
						_TL("Features Grid"),
						_TL(""),
						PARAMETER_INPUT);

	Parameters.Add_Grid(NULL,
						"VALUE",
						_TL("Value Grid"),
						_TL(""),
						PARAMETER_INPUT);

	Parameters.Add_Grid(NULL,
						"THRESHOLDGRID",
						_TL("Threshold Grid"),
						_TL(""),
						PARAMETER_INPUT_OPTIONAL);

	Parameters.Add_Grid(NULL,
						"BUFFER",
						_TL("Buffer Grid"),
						_TL(""),
						PARAMETER_OUTPUT,
						true,
						SG_DATATYPE_Int);

	pNode = Parameters.Add_Value(NULL,
						"THRESHOLD",
						_TL("Threshold"),
						_TL("Threshold (Grid Units)"),
						PARAMETER_TYPE_Double,
						0);	

	Parameters.Add_Choice(NULL,
						"THRESHOLDTYPE",
						_TL("Threshold Type"),
						_TL(""),
						CSG_String::Format(SG_T("%s|%s|"),
							_TL("Absolute"),
							_TL("Relative from cell value")
						));

}//constructor

CThresholdBuffer::~CThresholdBuffer(void)
{}

bool CThresholdBuffer::On_Execute(void){

	double dValue;
	int iPoint = 0;
	m_pFeatures = Parameters("FEATURES")->asGrid();
	m_pBuffer = Parameters("BUFFER")->asGrid();
	m_pValueGrid = Parameters("VALUE")->asGrid();
	m_pThresholdGrid = Parameters("THRESHOLDGRID")->asGrid();
	m_dThreshold = Parameters("THRESHOLD")->asDouble() ;
	m_iThresholdType = Parameters("THRESHOLDTYPE")->asInt();

	m_pCentralPoints	.Clear();
	m_pAdjPoints		.Clear();

	m_pBuffer->Assign((double)0);

    for(int y=0; y<Get_NY() && Set_Progress(y); y++){
		for(int x=0; x<Get_NX(); x++){
            dValue = m_pFeatures->asDouble(x,y);
			if (dValue != 0 && !m_pFeatures->is_NoData(x,y)){
				BufferPoint(x,y);
			}//if
		}//for
	}//for

	return true;

}//method

void CThresholdBuffer::BufferPoint(int x, int y){

	int x2,y2;
	static int nX[8] = {0,  1,  1,  1,  0, -1, -1, -1};
    static int nY[8] = {1,  1,  0, -1, -1, -1,  0,  1};
	double dValue;
	int iValue;
	int iPt;
	int n;	
	float fBaseValue;
	double dThreshold;

	fBaseValue = m_pValueGrid->asFloat(x,y,true);

	if (m_pThresholdGrid){
		dThreshold = m_pThresholdGrid->asFloat(x,y,true);
	}//if
	else{
		dThreshold = m_dThreshold;
	}//else

	m_pCentralPoints.Add(x,y);
	m_pBuffer->Set_Value(x,y,FEATURE);

	while (m_pCentralPoints.Get_Count()!=0){

		for (iPt=0; iPt<m_pCentralPoints.Get_Count();iPt++){

			x = m_pCentralPoints.Get_X(iPt);
			y = m_pCentralPoints.Get_Y(iPt);

			if (!m_pValueGrid->is_NoData(x,y)){
				for (n=0; n<8; n++){
					x2 = x + nX[n];
					y2 = y + nY[n];
					if (m_pValueGrid->is_InGrid(x2,y2)){
						iValue = m_pBuffer->asInt(x2,y2);
						if (!m_pValueGrid->is_NoData(x2, y2) && iValue == 0){
							if (m_iThresholdType == THRESHOLD_ABSOLUTE){
								dValue = m_pValueGrid->asFloat(x2,y2,true);
							}//if
							else{
								dValue = fabs(m_pValueGrid->asFloat(x2,y2,true) - fBaseValue);
							}//else
							if(dValue < dThreshold){
								m_pBuffer->Set_Value(x2, y2, BUFFER);
								m_pAdjPoints.Add(x2,y2);
							}//if
						}//if
					}//if
				}//for
			}//if
		}//for

		m_pCentralPoints.Clear();
		for (iPt=0; iPt<m_pAdjPoints.Get_Count(); iPt++){
			x= m_pAdjPoints.Get_X(iPt);
			y = m_pAdjPoints.Get_Y(iPt);
			m_pCentralPoints.Add(x, y);
		}//for
		m_pAdjPoints.Clear();

		Process_Get_Okay(true);

	}//while

	m_pCentralPoints.Clear();

}//method
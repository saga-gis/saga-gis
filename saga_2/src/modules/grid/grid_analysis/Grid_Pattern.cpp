/*******************************************************************************
    Grid_Pattern.cpp
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

#include "Grid_Pattern.h"


CGrid_Pattern::CGrid_Pattern(void){

	Set_Name(_TL("Pattern Analysis"));
	Set_Author(_TL("Copyrights (c) 2004 by Victor Olaya"));
	Set_Description(_TL(
		"(c) 2004 by Victor Olaya. Pattern Analysis"));

	Parameters.Add_Grid(NULL, 
						"INPUT", 
						_TL("Input Grid"), 
						"", 
						PARAMETER_INPUT);
	
	Parameters.Add_Grid(NULL, 
						"RELATIVE", 
						_TL("Relative Richness"), 
						"", 
						PARAMETER_OUTPUT, 
						true, 
						GRID_TYPE_Double);

	Parameters.Add_Grid(NULL, 
						"DIVERSITY", 
						_TL("Diversity"), 
						"", 
						PARAMETER_OUTPUT, 
						true, 
						GRID_TYPE_Double);

	Parameters.Add_Grid(NULL, 
						"DOMINANCE", 
						_TL("Dominance"), 
						"", 
						PARAMETER_OUTPUT, 
						true, 
						GRID_TYPE_Double);

	Parameters.Add_Grid(NULL, 
						"FRAGMENTATION", 
						_TL("Fragmentation"), 
						"", 
						PARAMETER_OUTPUT, 
						true, 
						GRID_TYPE_Double);

	Parameters.Add_Grid(NULL, 
						"NDC", 
						_TL("Number of Different Classes"), 
						"", 
						PARAMETER_OUTPUT, 
						true, 
						GRID_TYPE_Int);

	Parameters.Add_Grid(NULL, 
						"CVN", 
						_TL("Center Versus Neighbours"), 
						"", 
						PARAMETER_OUTPUT, 
						true, 
						GRID_TYPE_Int);

	Parameters.Add_Choice(NULL, 
						"WINSIZE", 
						_TL("Size of Analysis Window"), 
						"", 
						"3 X 3|"
						"5 X 5|"
						"7 X 7|",
						0);

	Parameters.Add_Value(NULL, 
						"MAXNUMCLASS", 
						_TL("Max. Number of Classes"), 
						_TL("Maximum number of classes in entire grid."), 
						PARAMETER_TYPE_Int, 
						10);

}//constructor


CGrid_Pattern::~CGrid_Pattern(void)
{}

bool CGrid_Pattern::On_Execute(void){
	
	m_pInput = Parameters("INPUT")->asGrid(); 	
	CGrid *pRelative = Parameters("RELATIVE")->asGrid();
	CGrid *pDominance = Parameters("DOMINANCE")->asGrid();
	CGrid *pDiversity = Parameters("DIVERSITY")->asGrid();
	CGrid *pFragmentation = Parameters("FRAGMENTATION")->asGrid();
	CGrid *pNDC = Parameters("NDC")->asGrid();
	CGrid *pCVN = Parameters("CVN")->asGrid();
	m_iWinSize = Parameters("WINSIZE")->asInt()*2+3;
	m_iNumClasses = Parameters("MAXNUMCLASS")->asInt();
	
    for(int y=m_iWinSize-2; y<Get_NY()-m_iWinSize+2 && Set_Progress(y); y++){		
		for(int x=m_iWinSize-2; x<Get_NX()-m_iWinSize+2; x++){
			double dDiversity = getDiversity(x,y);
			int iNumClasses = getNumberOfClasses(x,y);
			pRelative->Set_Value(x,y,((double)iNumClasses)/((double)m_iNumClasses)*100.0);
			pDominance->Set_Value(x,y,log((double)iNumClasses)-dDiversity);
			pDiversity->Set_Value(x,y,dDiversity);
			pFragmentation->Set_Value(x,y,((double)(iNumClasses-1))/((double)(m_iWinSize*m_iWinSize-1)));
			pNDC->Set_Value(x,y,iNumClasses);
			pCVN->Set_Value(x,y,getCVN(x,y));
        }// for
    }// for

	return true;

}//method


int CGrid_Pattern::getNumberOfClasses(int iX, int iY){

	int *iValues = new int[9];
	int iNumClasses =0;
	int i,j,k;
	int iValue;

	for (i =0; i<9; i++){
		iValues[i] = (int)m_pInput->Get_NoData_Value();
	}//for

	for (i=-m_iWinSize+2; i<m_iWinSize-1; i++){
		for (j=-m_iWinSize+2; j<m_iWinSize-1; j++){
			iValue=m_pInput->asInt(iX+i,iY+j);
			if (iValue!=m_pInput->Get_NoData_Value()){
				for (k=0; k<9; k++){
					if (iValue==iValues[k]){
						continue;
					}//if
					if (iValues[k]==m_pInput->Get_NoData_Value()){
						iValues[k]=iValue;
						iNumClasses++;
						continue;
					}//if
				}//for
			}//if
		}//for
	}//for

	return iNumClasses;

}//method


double CGrid_Pattern::getDiversity(int iX, int iY){

	int iValues[9][2];
	int iMaxClass=0;
	int i,j,k;
	int iValue;
	double dDiversity=0;
	double dProp;

	for (i =0; i<9; i++){
		iValues[i][0] = (int)m_pInput->Get_NoData_Value();
		iValues[i][1] = 0;
	}//for

	for (i=-m_iWinSize+2; i<m_iWinSize-1; i++){
		for (j=-m_iWinSize+2; j<m_iWinSize-1; j++){
			iValue=m_pInput->asInt(iX+i,iY+j);
			if (iValue!=m_pInput->Get_NoData_Value()){
				for (k=0; k<9; k++){
					if (iValue==iValues[k][0]){
						iValues[k][1]++;
						goto out;
					}//if
					if (iValues[k][0]==m_pInput->Get_NoData_Value()){
						iValues[k][0]=iValue;
						iValues[k][1]++;
						iMaxClass=k+1;
						goto out;
					}//if
				}//for
out:;
			}//if
		}//for
	}//for

	for (i=0; i<iMaxClass; i++){
		dProp = ((double)iValues[i][1])/8.0;
		dDiversity += (dProp*log(dProp));
	}//for
	
	//TRACE(dProp);
	
	return dDiversity;


}//method

int CGrid_Pattern::getCVN(int iX, int iY){

	int iValue2;
	int iValue = m_pInput->asInt(iX,iY);
	int iCVN=0;
	
	for (int i=-m_iWinSize+2; i<m_iWinSize-1; i++){
		for (int j=-m_iWinSize+2; j<m_iWinSize-1; j++){
			iValue2=m_pInput->asInt(iX+i,iY+j);
			if (iValue2!=m_pInput->Get_NoData_Value()){
				if (iValue!=iValue2){
					iCVN++;
				}//if
			}//if
		}//for
	}//for

	return iCVN;

}//method

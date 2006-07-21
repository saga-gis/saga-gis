/*******************************************************************************
    Fuzzify.cpp
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

#include "Fuzzify.h"

#define PI2 1.57079632

CFuzzify::CFuzzify(void){

	Parameters.Set_Name(_TL("Fuzzify"));
	Parameters.Set_Description(_TL(
		"(c) 2004 by Victor Olaya. Prepares a grid for fuzzy logic analysis"));

	Parameters.Add_Grid(NULL,
						"INPUT",
						_TL("Input Grid"),
						"",
						PARAMETER_INPUT);

	Parameters.Add_Value(NULL,
						"POINTA",
						_TL("Control Point A"),
						_TL("Control Point A"),
						PARAMETER_TYPE_Double,
						10);

	Parameters.Add_Value(NULL,
						"POINTB",
						_TL("Control Point B"),
						_TL("Control Point B"),
						PARAMETER_TYPE_Double,
						10);

	Parameters.Add_Value(NULL,
						"POINTC",
						_TL("Control Point C"),
						_TL("Control Point C"),
						PARAMETER_TYPE_Double,
						10);

	Parameters.Add_Value(NULL,
						"POINTD",
						_TL("Control Point D"),
						_TL("Control Point D"),
						PARAMETER_TYPE_Double,
						10);

	Parameters.Add_Choice(NULL, 
						"TYPE", 
						_TL("Membership function type"), 
						"", 
						CSG_String::Format("%s|%s|%s|",
							_TL("Linear"),
							_TL("Sigmoidal"),
							_TL("j-shaped")
						),
						0);

	Parameters.Add_Grid(NULL,
						"OUTPUT",
						_TL("Output Grid"),
						"",
						PARAMETER_OUTPUT);

}//constructor


CFuzzify::~CFuzzify(void){}

bool CFuzzify::On_Execute(void){

	int x,y;
	CGrid* pInput = Parameters("INPUT")->asGrid();
	CGrid* pOutput = Parameters("OUTPUT")->asGrid();
	double dPtA = Parameters("POINTA")->asDouble();
	double dPtB = Parameters("POINTB")->asDouble();
	double dPtC = Parameters("POINTC")->asDouble();
	double dPtD = Parameters("POINTD")->asDouble();
	int iType = Parameters("TYPE")->asInt();
	double dValue;
	double dX, dW;
	CSG_Colors	Colors;

	if (dPtA>dPtB || dPtB>dPtC || dPtC>dPtD){
		Message_Add(_TL("** ERROR : Invalid control points ** \n"));
		return false;
	}//if

	Colors.Set_Ramp(COLOR_GET_RGB(0, 0, 0),COLOR_GET_RGB(255, 255, 255));
	DataObject_Set_Colors(pOutput, Colors);
	pOutput->Set_Name(_TL("Fuzzified"));

	for(y=0; y<Get_NY() && Set_Progress(y); y++){
		for(x=0; x<Get_NX(); x++){
			if (!pInput->is_NoData(x,y)){
				dValue = pInput->asDouble(x,y);
				if (dValue <= dPtA || dValue >= dPtD){
					pOutput->Set_Value(x,y,0);
				}//if
				else if (dValue >= dPtB && dValue <= dPtC){
					pOutput->Set_Value(x,y,1);
				}//if
				else{
					if (dValue < dPtB){
						dX = dValue - dPtA;
						dW = dPtB - dPtA;
					}//if
					else{
						dX = dPtD - dValue;
						dW = dPtD - dPtC;
					}//else
					switch (iType){
					case 0:
						pOutput->Set_Value(x, y, dX / dW);
						break;
					case 1:
						pOutput->Set_Value(x, y, pow(sin(dX / dW * PI2), 2));
						break;
					case 2:
						pOutput->Set_Value(x, y, 1. / (1+pow((dW-dX) / dW,2)));
						break;
					}//switch
				}//else
			}//if
			else{
				pOutput->Set_NoData(x,y);
			}//else
		}//for
	}//for

	return true;

}//method

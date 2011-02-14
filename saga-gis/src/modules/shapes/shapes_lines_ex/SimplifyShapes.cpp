/**********************************************************
 * Version $Id$
 *********************************************************/
/*******************************************************************************
    SimplifyShapes.cpp
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

#include "SimplifyShapes.h"
#include "DPHull.h"
#include "PathHull.h"
#include "Containers.h"

using namespace hull;
using namespace std;

CSimplifyShapes::CSimplifyShapes(void){

	
	Parameters.Set_Name(_TL("Simplify Lines"));
	Parameters.Set_Description(_TW(
		"(c) 2004 by Victor Olaya. Use this module to simplify a lines layer, removing points from it."));

	Parameters.Add_Shapes(NULL, 
						"OUT", 
						_TL("Output"), 
						_TL(""), 
						PARAMETER_OUTPUT);

	Parameters.Add_Shapes(NULL, 
						"IN", 
						_TL("Lines"),
						_TL(""),
						PARAMETER_INPUT);

	Parameters.Add_Choice(NULL, 
						"METHOD", 
						_TL("Method"), 
						_TL(""), _TW(
						"Tolerance|"
						"Percentage of initial points|"
						"Number of total points|"),						
						0);


	Parameters.Add_Value(NULL, 
						"NUMPOINTS", 
						_TL("Number of points"),
						_TL("Number of points in the simplified line"), 
						PARAMETER_TYPE_Int, 
						20);
	
	Parameters.Add_Value(NULL, 
						"TOLERANCE", 
						_TL("Tolerance"),
						_TL("Tolerance"), 
						PARAMETER_TYPE_Double, 
						0.1,
						0,
						true);

	Parameters.Add_Value(NULL, 
						"PERCENTAGE", 
						_TL("Percentage of points"),
						_TL("Percentage of points remaining in the simplified line"), 
						PARAMETER_TYPE_Double, 
						10,
						0,
						true);

	Parameters.Add_Value(NULL, 
						"ERROR", 
						_TL("Possible error"),
						_TL("Possible error"), 
						PARAMETER_TYPE_Double, 
						20);

}//constructor


CSimplifyShapes::~CSimplifyShapes(void)
{}

bool CSimplifyShapes::On_Execute(void){

	bool bCopy;
	CSG_Shapes *pIn = Parameters("IN")->asShapes();
	CSG_Shapes *pOut= Parameters("OUT")->asShapes();
	double dError = Parameters("ERROR")->asDouble();
	double dPercentage = Parameters("PERCENTAGE")->asDouble();
	double dTolerance = Parameters("TOLERANCE")->asDouble();
	int iNumPoints = Parameters("NUMPOINTS")->asInt();
	int iMethod = Parameters("METHOD")->asInt();
	TSG_Point	Pt;	
	CSG_Shape *pShape, *pShape2;
	int i;

	if(pIn == pOut){
		bCopy = true;
		pOut = SG_Create_Shapes();
	}//if
	else{
		bCopy = false;
	}//else

	pOut->Create(pIn->Get_Type(), _TL("Simplified Lines"), pIn);	

	vector<float> vx(5000),vy(5000);
	for(i=0; i<pIn->Get_Count(); i++){	
		pShape = pIn->Get_Shape(i);					
		pShape2 = pOut->Add_Shape();

		for(int j=0; j<pShape->Get_Part_Count(); j++){		
			vx.clear(); 
			vy.clear();	
			for(int k=0; k<pShape->Get_Point_Count(j); k++){
				Pt = pShape->Get_Point(k,j);				
				vx.push_back(Pt.x);
				vy.push_back(Pt.y);
			}//for
			CDPHullF dp;				
			dp.SetPoints(vx,vy);
			dp.GetKeys(vx,vy);
			dp.SetTol(dTolerance);
			try{
				switch (iMethod){
				case 0:
					dp.Simplify();
					break;
				case 1:
					dp.ShrinkNorm(dPercentage / 100., dError);
					break;
				case 2:
					dp.Shrink(iNumPoints, ceil(dError));
					break;
				}//switch
			}
			catch(TCHAR* str){}    			
			const CDPHullF::KeyContainer& kc=dp.GetKeys();
			CDPHullF::KeyContainer::const_iterator it;
			TPoint<float> key;
			for (it=kc.begin(); it!=kc.end(); ++it){
				pShape2->Add_Point((*it)->x, (*it)->y, j);
			}//for
		}//for
	}//for

	if(bCopy){
		pIn->Assign(pOut);
		delete(pOut);
	}//if

	return true;

}//method

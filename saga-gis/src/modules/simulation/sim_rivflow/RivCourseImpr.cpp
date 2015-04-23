
///////////////////////////////////////////////////////////
//                                                       //
//                         SAGA                          //
//                                                       //
//      System for Automated Geoscientific Analyses      //
//                                                       //
//                    Module Library:                    //
//                       RivFlow                         //
//                                                       //
//-------------------------------------------------------//
//                                                       //
//                   RivCourseImpr.cpp                   //
//                                                       //
//                 Copyright (C) 2014 by                 //
//                   Christian Alwardt                   //
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
// 59 Temple Place - Suite 330, Boston, MA 02111-1307,   //
// USA.                                                  //
//                                                       //
//-------------------------------------------------------//
//                                                       //
//    e-mail:     alwardt@ifsh.de                        //
//                                                       //
//    contact:    Christian Alwardt                      //
//                Institute for Peace Research           //
//                and Security Policy (IFSH)             //
//                University of Hamburg                  //
//                Germany                                //
//                                                       //
///////////////////////////////////////////////////////////


///////////////////////////////////////////////////////////
//														 //
//		RivTool 0.1										 //
//														 //
///////////////////////////////////////////////////////////


#include "RivCourseImpr.h"
#include <saga_api/module.h>


//---------------------------------------------------------
//#########################################################

CRivCourseImpr::CRivCourseImpr(void)
{
	//-----------------------------------------------------
	// Modul information

	Set_Name		("GridManipulation");
	Set_Author		("Christian Alwardt");
	Set_Description	("Improve RiverCourse by MouseClicks");

	//-----------------------------------------------------
	// Define your parameters list...

	Parameters.Add_Grid(
		NULL, "Input1"	, "Geländemodell (DTM)",
		"Digitales Geländemodell des Flusseinzugsgebietes",
		PARAMETER_INPUT
	);

	Parameters.Add_Value(
		NULL, "diffH"	, "Höhendifferenz",
		"Höhe in [m] um die der Wert der Zelle erniedrigt (positiver Wert) oder erhöht (negativer Wert) wird. ",
		PARAMETER_TYPE_Double, 1	
	);

	Parameters.Add_Value(
		NULL, "true", "Zufluss aus Nachbarzellen erzwingen",
		"Der Zufluss aus allen Nachbarzellen - ausgenommen die Zelle in die Wasser abfließt - wird erzwungen. Hierbei wird die gewählte Zelle um obigen Höhendifferenzwert kleiner, als alle Nachbarzellen gesetzt.",
		PARAMETER_TYPE_Bool, false
	);
	
}


bool CRivCourseImpr::On_Execute(void){
	
	
	m_pDTM = Parameters("Input1")->asGrid(); 
	m_pDiffH = Parameters("diffH")->asDouble();

	return (true);

}


bool CRivCourseImpr::On_Execute_Position(CSG_Point ptWorld, TSG_Module_Interactive_Mode Mode)
{	
	int iX, iY;		

	//-----------------------------------------------------
	if(	Mode != MODULE_INTERACTIVE_LDOWN || !Get_Grid_Pos(iX, iY) )
	{
		return( false );
	}

	double dValue = m_pDTM->asDouble(iX,iY); //Wert der ausgewählten Zelle

	if( Parameters("true")->asBool() )
	{
		minVal = m_pDTM->asDouble(iX, iY); //minVal zunächst auf Wert der ausgwählten Zelle gesetzt
		int iN = m_pDTM->Get_Gradient_NeighborDir(iX, iY);//falls iN = -1 liegt Senke vor!!
		nX = Get_xTo(iN, iX);//Koordinaten der niedrigsten Nachbarzelle (ist niedriger als ausgewählte Zelle)
		nY = Get_yTo(iN, iY);
		double vN = m_pDTM->asDouble(nX, nY); //Wert der niedrigsten Nachbarzelle (ist niedriger als ausgewählte Zelle)
		double setVal;

		if(iN >= 0)
		{
			for(int i = 0; i < 7; i ++)
			{
				if(i != iN)													
					getNeighFlowGridValue(iX, iY, minVal);				
			}
		
			if(minVal > vN)
			{
				setVal = minVal - m_pDiffH;

				if(setVal <= vN  )
					setVal = minVal + ((minVal - vN) * 0.9999);
			}
			else
			{
				Message_Dlg("Achtung, eine Senke wurde generiert!", SG_T("caption"));
				setVal = minVal * 1.0001;
			}

			m_pDTM->Set_Value( iX, iY, setVal );
		}
		else
			m_pDTM->Set_Value( iX, iY, minVal );
	
		DataObject_Update(m_pDTM, SG_UI_DATAOBJECT_UPDATE_ONLY);
	}
	else
	{
		m_pDTM->Set_Value( iX, iY, dValue - m_pDiffH );
		DataObject_Update(m_pDTM, SG_UI_DATAOBJECT_UPDATE_ONLY);
	}

	return (true);

}


void CRivCourseImpr::getNeighFlowGridValue(int x, int y, double kVal)
{
	int i = m_pDTM->Get_Gradient_NeighborDir(x, y);
	int ix = Get_xTo(i, x);
	int iy = Get_yTo(i, y);
	
	if(i >= 0)
	{
		if( ix == nX && iy == nY)
		{}
		else
		{
			if (m_pDTM->asDouble(ix, iy) <= kVal)
				minVal = m_pDTM->asDouble(ix, iy);
		}
	}
}
///////////////////////////////////////////////////////////
//														 //
//														 //
//														 //
///////////////////////////////////////////////////////////

//---------------------------------------------------------

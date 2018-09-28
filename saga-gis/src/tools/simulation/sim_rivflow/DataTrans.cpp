
///////////////////////////////////////////////////////////
//                                                       //
//                         SAGA                          //
//                                                       //
//      System for Automated Geoscientific Analyses      //
//                                                       //
//                     Tool Library                      //
//                       RivFlow                         //
//                                                       //
//-------------------------------------------------------//
//                                                       //
//                     DataTrans.cpp                     //
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
// Free Software Foundation, either version 2 of the     //
// License, or (at your option) any later version.       //
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


#include "DataTrans.h"
#include <math.h>
#include <iostream>
#include "stdio.h"
#include <fstream>
#include <string>
#include "stdlib.h"
#include <sstream>

using namespace std;

#define ESC 27

//---------------------------------------------------------
//#########################################################

CDataTrans::CDataTrans(void)
{
	//-----------------------------------------------------
	// Modul information

	Set_Name		("DataTrans");
	Set_Author		("Christian Alwardt");
	Set_Description	("DataOperations");


	//Parameters list...

	Parameters.Add_Grid_List(
			Parameters("SOURCE_NODE"),
			"SOURCE1"		, SG_T("RunOff"),
			SG_T(""),
			PARAMETER_INPUT, false
		);

		Parameters.Add_Grid_List(
			Parameters("SOURCE_NODE"),
			"SOURCE2"		, SG_T("Drainage"),
			SG_T(""),
			PARAMETER_INPUT, false
		);


}


//#########################################################
//---------------------------------------------------------
//#########################################################



bool CDataTrans::On_Execute(void)
{
	
	//-----------------------------------------------------
	
	CSG_Parameter_Grid_List	*pSources1	= Parameters("SOURCE1")->asGridList(); //RunOff Liste
	CSG_Parameter_Grid_List	*pSources2	= Parameters("SOURCE2")->asGridList(); //Drainage Liste

	if( pSources1->Get_Grid_Count() > 0 && pSources2->Get_Grid_Count() > 0)
	{
		if( pSources1->Get_Grid_Count() == pSources2->Get_Grid_Count()) 
		{
			m_pID = pSources1->Get_Grid_Count();

			for(i=0; i < pSources1->Get_Grid_Count(); i++)
			{
				pSource1 = pSources1->Get_Grid(i);
				pSource2 = pSources2->Get_Grid(i);
	
				Set_TFile(pSource1, pSource2);
			}
	
		}
		else 
			Message_Dlg("Achtung, Anzahl der Einträge stimmt nicht überein");
	}


	
	delete(pSources1);
	delete(pSources1);

	return(true);
}



bool CDataTrans::Set_TFile(CSG_Grid *pSource1, CSG_Grid *pSource2)
{
	if( pSource1->Get_NX() == pSource2->Get_NX() && pSource1->Get_NY() == pSource2->Get_NY())
	{
		NX = pSource1->Get_NX();
		NY = pSource1->Get_NY();
	
		fstream myfile;
		std::stringstream file0;
		std::string file;

		//LandSurfaceModell-Daten
		
		//-----------------------------------------------------
		file0.str("");
		file0 << "dataTemp/lsMData-Day" << i+1 << ".txt";
		file = file0.str();
		myfile.open(file.c_str(), ios::out|ios::trunc);

		for(int x = 0; x < NX; ++x)
		{
			for(int y = 0; y < NY; ++y)
			{
				myfile << x << " " << y;
				myfile << " " << pSource1->asDouble(x,y) << " " << pSource2->asDouble(x,y);
				myfile << "\n";
			}
		}
		myfile.close();
		Process_Set_Text("GridSave || Datensatz %d geschrieben",i+1);
	}

	return(true);
}


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
//                    GridComb.cpp                       //
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


#include "GridComb.h"
#include <math.h>
#include <string>
#include <fstream>
#include <iostream>
#include <sstream>
#include <time.h>

using namespace std;

//ACHTUNG
//////////////////////////////////////////////////////////////////////
//ACHTUNG: Bisher keine Fehlerroutine, die auf unterschiedlich große InputGrids im DateiOrdner hinweist -> moegliche Fehlerursache!!!
//////////////////////////////////////////////////////////////////////


//---------------------------------------------------------
//#########################################################

CGridComb::CGridComb(void)
{
	//-----------------------------------------------------
	// Modul information

	Set_Name		("GridCombination");
	Set_Author		("Christian Alwardt");
	Set_Description	("Grid Combination");

	//-----------------------------------------------------
	// Define your parameters list...

	Parameters.Add_Grid(
		NULL, "INPUT"	, "Gelaendemodell (DTM)",
		"Digitales Gelaendemodell des Flusseinzugsgebietes",
		PARAMETER_INPUT
	);

	Parameters.Add_FilePath(
		Parameters("GENERAL_NODE")	, "Folder1", _TL("Pfad WaterGap Raster"),
		_TL("Ordnerpfad in dem alle zu bearbeitenden WaterGap Raster abgelegt sind"), _TL("SAGA Grid Files (*.sgrd)|*.sgrd|All Files|*.*"),0 ,false ,true ,false
	);

	Parameters.Add_Value(
		NULL, "sY"	, "Start-Jahr",
		"Jahr in dem die Gridoperation startet",
		PARAMETER_TYPE_Int, 1990, 1906, true, 2000, true
	);

	Parameters.Add_Value(
		NULL, "eY"	, "End-Jahr",
		"Jahr in dem der Verarbeitungsprozess enden soll",
		PARAMETER_TYPE_Int, 1990, 1906, true, 2000, true
	);

	Parameters.Add_Value(
		Parameters("GENERAL_NODE"), "DomW", "Domestic Water",
		"Beruecksichtigung der Domestic Water im resultieren Raster.",
		PARAMETER_TYPE_Bool, true
	);


	Parameters.Add_Value(
		Parameters("GENERAL_NODE"), "ElecW", "Electricity Water",
		"Beruecksichtigung der Electricity Water im resultieren Raster.",
		PARAMETER_TYPE_Bool, true
	);


	Parameters.Add_Value(
		Parameters("GENERAL_NODE"), "LiveW", "Livestock Water",
		"Beruecksichtigung der Livestock Water im resultieren Raster.",
		PARAMETER_TYPE_Bool, true
	);


	Parameters.Add_Value(
		Parameters("GENERAL_NODE"), "ManW", "Manufacturing Water",
		"Beruecksichtigung des Manufacturing Water im resultieren Raster.",
		PARAMETER_TYPE_Bool, true
	);


	Parameters.Add_Value(
		Parameters("GENERAL_NODE"), "IrrW", "Irrigation Water",
		"Beruecksichtigung des Irrigation Water im resultieren Raster (moeglicher Einheitenfehler im WaterGapDatensatz!!).",
		PARAMETER_TYPE_Bool, true
	);

	Parameters.Add_Choice(
		Parameters("GENERAL_NODE")	, "FvA"	, _TL("Flaechenverbrauch-Auswahl (FvA)"),
		_TL(""),
		CSG_String::Format(SG_T("%s|%s|"),
			_TL("Resultierendes Raster ueber WasserENTNAHME erstellen"),
			_TL("Resultierendes Raster ueber WasserNUTZUNG erstellen")
		), 0
	);
	

}


//---------------------------------------------------------
//#########################################################
bool CGridComb::On_Execute(void)
{
	//-----------------------------------------------------
	// Get the parameter settings...

	m_pRefGrid = Parameters("INPUT")->asGrid();
	m_pDataFolder = Parameters("Folder1")->asString();
	sY = Parameters("sY")->asInt();
	eY = Parameters("eY")->asInt();

	m_pGridComb = SG_Create_Grid(m_pRefGrid, SG_DATATYPE_Double);
	m_pDomW = SG_Create_Grid(m_pRefGrid, SG_DATATYPE_Double);
	m_pElecW = SG_Create_Grid(m_pRefGrid, SG_DATATYPE_Double);
	m_pLiveW = SG_Create_Grid(m_pRefGrid, SG_DATATYPE_Double);
	m_pManW = SG_Create_Grid(m_pRefGrid, SG_DATATYPE_Double);
	m_pIrrW = SG_Create_Grid(m_pRefGrid, SG_DATATYPE_Double);

	m_pFvA = Parameters("FvA")->asInt(); //Auswahlplatzhalter ob Wasserentnahme oder Wassernutzung beruecksichtigt werden soll

	//-----------------------------------------------------
	//Startdatum/Zeit erfassen
	time_t rawtime;
	struct tm * timeinfo;
	time (&rawtime);
	timeinfo = localtime (&rawtime);
	//-----------------------------------------------------


	//-----------------------------------------------------
	// Do something...

	//-----------------------------------------------------	
	//SpeicherDateipfad (sfile) fuer diese Simulation wird generiert und entsprechender Ordner erstellt - Ordnerformat : ddmmyy_HHMM
	std::stringstream sPath0;
	sPath0.str("");
	char buffer [20];
	strftime(buffer,20,"%d%m%y_%H%M",timeinfo);
	sPath0 << m_pDataFolder.b_str() << "\\Total-" << "FvA" << m_pFvA << "-D" << Parameters("DomW")->asBool() << "E" << Parameters("ElecW")->asBool() << "L" << Parameters("LiveW")->asBool() << "M" << Parameters("ManW")->asBool() << "I" << Parameters("IrrW")->asBool() << "-" << buffer;
	std::string m_pSPath;
	m_pSPath = sPath0.str();							//Speicherdateipfad
	SG_Dir_Create(CSG_String(m_pSPath.c_str()));		//Ordner wird erstellt
	SPath = m_pSPath.c_str();
	//-----------------------------------------------------



	//-----------------------------------------------------
	//Fehlerroutine

	if((eY - sY) < 0)
	{
		Message_Dlg("Fehler: Das EndJahr liegt vor dem Startjahr");
			return (false);
	}

	if( (m_pFvA == 0) && Parameters("LiveW")->asBool() )
		Message_Dlg("Hinweis: Fuer den Livestock liegen nur Wassernutzungsdaten vor. Diese werden deshalb auch fuer die Kalkulation der Wasserentnahme herangezogen");
	//-----------------------------------------------------



	NX = m_pRefGrid->Get_NX();	//Anzahl der Reihen x
	NY = m_pRefGrid->Get_NY();	//Anzahl der Zeilen y


//-----------------------------------------------------
//Jahresschleife
	for(int n = sY; n <= eY; n++ )
	{
		if(Parameters("DomW")->asBool())
		{
			CSG_String sDomW;
			if( m_pFvA == 0 )
				sDomW.Printf(SG_T("%s/domwith_%d.sgrd"), m_pDataFolder.c_str(), n);
			else
				sDomW.Printf(SG_T("%s/domcon_%d.sgrd"), m_pDataFolder.c_str(), n);
			
			if(!m_pDomW->Create(sDomW))
				m_pDomW->Assign(0.0);
		}

		if(Parameters("ElecW")->asBool())
		{
			CSG_String sElecW;
			if( m_pFvA == 0 )
				sElecW.Printf(SG_T("%s/elecwith_%d.sgrd"), m_pDataFolder.c_str(), n);
			else
				sElecW.Printf(SG_T("%s/eleccon_%d.sgrd"), m_pDataFolder.c_str(), n);
			
			if(!m_pElecW->Create(sElecW))
				m_pElecW->Assign(0.0);
		}

		if(Parameters("LiveW")->asBool())
		{
			CSG_String sLiveW;
			sLiveW.Printf(SG_T("%s/livecon_%d.sgrd"), m_pDataFolder.c_str(), n);
			
			if(!m_pLiveW->Create(sLiveW))
				m_pLiveW->Assign(0.0);
		}

		if(Parameters("ManW")->asBool())
		{
			CSG_String sManW;
			if( m_pFvA == 0 )
				sManW.Printf(SG_T("%s/manwith_%d.sgrd"), m_pDataFolder.c_str(), n);
			else
				sManW.Printf(SG_T("%s/mancon_%d.sgrd"), m_pDataFolder.c_str(), n);
			
			if(!m_pManW->Create(sManW))
				m_pManW->Assign(0.0);
		}
		
		
		if(Parameters("IrrW")->asBool())
		{
			CSG_String sIrrW;

			//-----------------------------------------------------
			//Im Fall von IrrigationData -> extra Monatsschleife
			//####ACHTUNG!!######
			//Annahme, dass die Einheit fuer IrrigationData falsch angegebn ist und in Wirklichkeit und m3/month betraegt,
			//deshalb wird durch (86400*30) getielt um Einheit m3/s zu erhalten - siehe unten !!
			for(int i=0; i<12; i++)
			{
				if( m_pFvA == 0 )
					sIrrW.Printf(SG_T("%s/PIrrWW_%d_%d.sgrd"), m_pDataFolder.c_str(), n, i+1);
				else
					sIrrW.Printf(SG_T("%s/PIrrUse_%d_%d.sgrd"), m_pDataFolder.c_str(), n, i+1);
				
				if(!m_pIrrW->Create(sIrrW))
					m_pIrrW->Assign(0.0);
		
				for(int x=0; x < NX; x++)
				{
					for(int y=0; y < NY; y++)
					{
						if(m_pRefGrid->is_NoData(x,y)) 
							m_pGridComb->Set_NoData(x,y);
						else
						{
							m_pGridComb->Set_Value(x, y, (m_pDomW->asDouble(x,y) + m_pElecW->asDouble(x,y) + m_pLiveW->asDouble(x,y) + m_pManW->asDouble(x,y) + ( m_pIrrW->asDouble(x,y) / 86400 / 30 ) )); 
						
						//Monitoring:
						double a = m_pDomW->asDouble(x,y);
						double b = m_pElecW->asDouble(x,y);
						double c = m_pLiveW->asDouble(x,y);
						double d = m_pManW->asDouble(x,y);
						double e = ( m_pIrrW->asDouble(x,y) / 86400 / 30 );
						//		
						}

					}
				}
				CSG_String SaveAsFile;
					
				SaveAsFile.Printf(SG_T("%s/TotalWUse_%d_%d.sgrd"), SPath.c_str(), n, i+1);
										
				m_pGridComb->Save(SaveAsFile);
			}
				//-----------------------------------------------------
		}
		else
		{
			for(int x=0; x < NX; x++)
			{
				for(int y=0; y < NY; y++)
				{
					if(m_pRefGrid->is_NoData(x,y)) 
						m_pGridComb->Set_NoData(x,y);
					else
						m_pGridComb->Set_Value(x, y, (m_pDomW->asDouble(x,y) + m_pElecW->asDouble(x,y) + m_pLiveW->asDouble(x,y) + m_pManW->asDouble(x,y)) ); 
				}
			}
			
			CSG_String SaveAsFile;
			
			SaveAsFile.Printf(SG_T("%s/TotalWUse_%d.sgrd"), SPath.c_str(), n);
			
			m_pGridComb->Save(SaveAsFile);
		}
	}

//-----------------------------------------------------

	SaveParameters();

	delete(m_pDomW);
	delete(m_pElecW);
	delete(m_pLiveW);
	delete(m_pManW);
	delete(m_pIrrW);
	delete(m_pGridComb);

	//-----------------------------------------------------
	// Return 'true' if everything is okay...

	return( true );
}
///////////////////////////////////////////////////////////
//														 //
//														 //
//														 //
///////////////////////////////////////////////////////////

//---------------------------------------------------------




bool CGridComb::SaveParameters()
{
	int i = 0;

	int d = NX * NY;
	time_t rawtime;
	time(&rawtime);

	CSG_String sSaveFile;
	sSaveFile.Printf(SG_T("%s/Parameters.txt"), SPath.c_str() );

	fstream myfile;
	myfile.open(sSaveFile.b_str(), ios::out|ios::trunc);

	myfile << "Einstellungen der Grid-Kombination." << "\n\n";
	myfile << "Timestamp: " << ctime(&rawtime) << "\n\n";
	myfile << "Einstellungen:\n\n";
	myfile << "StartJahr = " << sY << "\n";
	myfile << "EndJahr = " << eY << "\n\n";
	
	myfile << "DomW: " << Parameters("DomW")->asBool() << "\n";
	myfile << "ElecW: " << Parameters("ElecW")->asBool() << "\n";
	myfile << "LiveW: " << Parameters("LiveW")->asBool() << "\n";
	myfile << "ManW: " << Parameters("ManW")->asBool() << "\n";
	myfile << "IrrW: " << Parameters("IrrW")->asBool() << "\n\n";

	myfile << "FlaechennutzungsAuswahl - WaterWithdrawl [=0] oder WaterConsumption[=1]:" << m_pFvA << "\n\n";

	myfile << "SpeicherDateipfad: " << sSaveFile.b_str() << "\n\n";

	myfile.close();

return true;
}

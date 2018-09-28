
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
//                     RivBasin.cpp                      //
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


#include "RivBasin.h"
#include <math.h>
#include <sstream>
#include <fstream>

//ACHTUNG
//////////////////////////////////////////////////////////////////////
//Das Schreiben von  Koordinaten als Gridboxwert ist zur			//
//Zeit auf VIER Stellen beschraenkt -> DoubleFormat == [XXXXYYYY]	//
//Bei der Programmierung beruecksichtigen!!!							//
//																	//
//////////////////////////////////////////////////////////////////////


//---------------------------------------------------------
//#########################################################

CRivBasin::CRivBasin(void)
{
	//-----------------------------------------------------
	// Modul information

	Set_Name		("RiverBasin");
	Set_Author		("Christian Alwardt");
	Set_Description	("Parameters of RiverBasin");

	//-----------------------------------------------------
	// Define your parameters list...

	Parameters.Add_Grid(
		NULL, "INPUT"	, "DTM",
		"Digitales Gelaendemodell des Flusseinzugsgebietes",
		PARAMETER_INPUT
	);

	Parameters.Add_Grid(
		NULL, "INPUT2"	, "HGGrid",
		"Eingaberaster der Hauptgerinnerasterzellen des Flussnetzwerkes",
		PARAMETER_INPUT
	);

	Parameters.Add_Grid(
		NULL, "INPUT3"	, "statisches Entnahmeraster",
		"Eingaberaster mit Angaben zur statischen Flaechenwasserentnahme.",
		PARAMETER_INPUT_OPTIONAL
		//Jahreswerte 	//ACHTUNG auf optionale Eingabe gesetzt -> 'Sicherheits-'Abfrage einbauen
	);

	Parameters.Add_Value(
		NULL, "WCons", "Anteilige Flaechenwasserentnahme",
		"Wenn gesetzt, werden die Werte des statischen Entahmerasters anteilig entnommen und als Raster statWUse ausgegeben",		PARAMETER_TYPE_Bool, false
	); 

	Parameters.Add_Choice(
		Parameters("WTHD_NODE")	, "WCons2"	, _TL("Dynamische Flaechenwassernutzung..."),
		_TL("Auswahl der Art der anteiligen Flaechenwasserwasserentnahme."),
		CSG_String::Format(SG_T("%s|%s"),
			_TL("...anteilig aus den Flussrasterzellen"),
			_TL("...anteilig aus Rasterzellen der Teileinzugegebiete")
		), 0
	);

	Parameters.Add_Grid(
		NULL, "OUTPUT2"	, "Grad",
		"Ausgabe der Abflussgradienten jeder Rasterzelle",
		PARAMETER_OUTPUT
	); // Gradient in Abflussrichtung

		Parameters.Add_Grid(
		NULL, "OUTPUT3"	, "Direc",
		"Ausgabe der Abflussrichtung fuer jede Rasterzelle",
		PARAMETER_OUTPUT
	); // Flussrichtung 0-7		// 7|0 |1
								// 6|-1|2
								// 5|4 |3
	Parameters.Add_Grid(
		NULL, "OUTPUT4"	, "HGGrad",
		"Ausgabe der Abflussgradienten jeder Hauptgerinnerasterzelle",
		PARAMETER_OUTPUT
	);

	Parameters.Add_Grid(
		NULL, "OUTPUT5"	, "RivSpeed",
		"Ausgabe Flussgeschwindigkeiten der Hauptgerinnerasterzellen (siehe Parametereinstellungen)",
		PARAMETER_OUTPUT
	);

	Parameters.Add_Grid(
		NULL, "OUTPUT6"	, "Coordinates",
		"Ausgabe der Koordinatenwerte der Rasterzellen im Format xxxx.yyyy",
		PARAMETER_OUTPUT
	); // Konvention des Rasterzahlenwertes: xxxx.yyyy [x,y-Koordinate]


	Parameters.Add_Grid(
		NULL, "OUTPUT7"	, "BasinShare",
		"Ausagbe der Rasterzellen des Flusseinzugsgebiets",
		PARAMETER_OUTPUT
	); // Grids die in Fluss mueden [>0 = Flussgridkoordinate] , Flussgrids [=0] , keine Flussgrid [-1]
	
	Parameters.Add_Grid(
		NULL, "OUTPUT8"	, "statWUse",
		"Ausagbe der anteiligen Flaechenwasserentnahme je Rasterzelle",
		PARAMETER_OUTPUT_OPTIONAL
	); // Flussgrids [>=0] , keine Flussgrid [-1]

	Parameters.Add_Grid(
		NULL, "OUTPUT9"	, "NumInFlowCells",
		"Ausgaberaster mit Angabe ueber die Anzahl der Rasterzellen, die in eine spezifische Rasterzelle (x,y) abflieszen",
		PARAMETER_OUTPUT
	);

	Parameters.Add_Value(
		NULL, "pCr"	, "Hauptgerinne-Parameter pHG",
		"Parameter pHG zur Berechnung der Lagtime kHG des Hauptgerinneabflusses",
		PARAMETER_TYPE_Double, 0.0035, 0, true
	);

	Parameters.Add_Value(
		NULL, "nCr"	, "Hauptgerinne-Speicherkaskade nHG",
		"Festlegen, wieviele Speicher die ChannelFlow River-Speicherkaskade enthaelt",
		PARAMETER_TYPE_Int, 1, 1, true		
	);

	Parameters.Add_Value(
		Parameters("FLOW_NODE"), "EnfVmax", "Maximal Geschwindigkeit des Hauptgerinnes beruecksichtigen",
		"Angegebene Maximalgeschwindigkeit im Hauptgerinne bei der Berechnung der durchschnittlichen Flieszgeschwindigkeit des Hauptgerinnes beruecksichtigen.",
		PARAMETER_TYPE_Bool, true
	);

	Parameters.Add_Value(
		Parameters("FLOW_NODE"), "VTresh" , "Maximalgeschwindigkeit im Hauptgerinne in km/h",
		"Festlegung der maximalen Wasserflussgeschwindigkeit im Hauptgerinne in km/h (oberer Grenzwert)",
		PARAMETER_TYPE_Double, 4, 0, true, 10, true		//Achtung!! Obergrenze statisch eingestellt
	);


}


//---------------------------------------------------------
//#########################################################
bool CRivBasin::On_Execute(void)
{
	int x, y;
	BasinGrids = 0;
	statV = 0;
	statN = 0;

	//-----------------------------------------------------
	// Get the parameter settings...

	m_pDTM = Parameters("INPUT")->asGrid();
	m_pRivGrids = Parameters("INPUT2")->asGrid();
	m_pWCons = Parameters("INPUT3")->asGrid(); //statisches WaterConsumption Grid (Jahreswerte) in [kg m-2 s-1]
	m_pGrad = Parameters("OUTPUT2")->asGrid();
	m_pDirec = Parameters("OUTPUT3")->asGrid();
	m_pRivGrad = Parameters("OUTPUT4")->asGrid(); //Gibt Grid aus, welches nur die Werte des Flussgefaelles enhaelt -> damit spaeter auschlieszliche Berechnug des max Flussgefaelles moeglich 
	m_pRSpeed = Parameters("OUTPUT5")->asGrid();
	m_pRaster = Parameters("OUTPUT6")->asGrid();
	m_pBasinShare = Parameters("OUTPUT7")->asGrid();
	m_pSharedRivCons = Parameters("OUTPUT8")->asGrid();
	m_pNumInFlowCells = Parameters("OUTPUT9")->asGrid();
	pCr = Parameters("pCr")->asDouble();
	nCr = Parameters("nCr")->asInt();
	m_pVTresh = Parameters("VTresh")->asDouble(); // [km/h]

	//-----------------------------------------------------
	// Do something...
	
	m_pGrad->Assign(0.0);
	m_pDirec->Assign(-1.0);
	m_pRivGrad->Assign(0.0);
	m_pRSpeed->Assign(0.0);
	m_pNumInFlowCells->Assign(0.0);
	m_pBasinShare->Assign(-1.0); // Vorab Werte -1 (kein Flussbasin); Wenn Grid in Fluss muendet > 0 (entspricht Koordinate) oder =0 wenn Flussgrid (>=0 entspricht Flussbasin)
	
	if(m_pSharedRivCons)
		m_pSharedRivCons->Assign(0.0);

	for(x=0; x < m_pDTM->Get_NX(); x++)
	{
		for(y=0; y < m_pDTM->Get_NY(); y++)
		{
			double r = x + 0.0001*y;				// Konvention des Rasterzahlenwertes: xxxx.yyyy [x,y-Koordinate]
			m_pRaster->Set_Value(x, y, r);
			
			if(m_pDTM->is_NoData(x,y))
			{
					m_pGrad->Set_NoData(x,y);
					m_pDirec->Set_NoData(x,y);
					m_pRivGrad->Set_NoData(x,y);
			}
			else
				Set_RivParam(x,y);
		}
	}

	//Durchnittsgeschwindigkeit in Log-Datei schreiben
	double res = statV / statN;
	std::stringstream lStr0;
	std::string lStr;
	lStr0.str("");
	lStr0 << "Durschnittsgeschwindigkeit fuer pHG = " << pCr << ": " << res;
	lStr = lStr0.str();

	WriteLog(lStr);
	//------------------------------------------------


	if(!CRivBasin::Set_BasinShare()) //SetBasin Aufruf
		Message_Dlg("Achtung, Fehler beim Erzeugen der Flussabhaengigen-Anteile wegen nicht gesezter FlussGrids");

	if( !m_pRivGrids )
	{
		m_pRivGrad->Destroy();
		m_pRSpeed->Destroy();
		m_pNumInFlowCells->Destroy();
		m_pBasinShare->Destroy();
	}

	if( (!m_pWCons || !m_pRivGrids) && m_pSharedRivCons)
		m_pSharedRivCons->Destroy();


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



void CRivBasin::Set_RivParam(int x, int y)
{
	int i, ix, iy;
	double z, dzMax, l;


	z = m_pDTM->asDouble(x,y);
	
	dzMax = 0.0;

	i = m_pDTM->Get_Gradient_NeighborDir(x, y);
	
	if( i >=0)
	{
		ix = Get_xTo(i, x); //x-Wert niedrigste Nachbarzelle
		iy = Get_yTo(i, y); //y-Wert niedrigste Nachbarzelle
		
		l = Get_Length(i);	//Weg Mittelpunkt Gitterbox (x,y) nach Gitterboxmitte (ix,iy)
	
		dzMax = (z - m_pDTM->asDouble(ix, iy)) / l; //Steigung dz/l entspricht tan(Steigungswinkel) ->Gefaelle in Abflussrichtung

	}
	else // Senke!!
		dzMax = 0;

	
	
	if( m_pRivGrids && m_pRivGrids->asDouble(x,y) != 0) // Wenn bei (x,y) Flussgrid vorliegt...
	{
		double vMax2 =  pow(dzMax, 0.1 ) / pCr / nCr / 24;

		//--- Werte zur Berechnung der Durchschnittsgeschwindigkeit im HG erfasse
		if(Parameters("EnfVmax")->asInt() && vMax2 > m_pVTresh)
			statV = statV + m_pVTresh;	//Falls vMax2 groeszer als Geschwindigkeitsgrenze m_pVTresh, auf m_pVTresh setzten
		else
			statV = statV + vMax2;
		
		statN = statN + 1;
		//-------------------------


		m_pRivGrad->Set_Value( x, y, dzMax );	//Flussbezogene Grids: Flussgefaelle; (x,y) nach (ix,iy) -> Extra RivGrad-Grid das nur die Gradienten der Fluss-Rasterzellen enthaelt, damit in LandFlow einfach der MaximalGradient bzw. kmin der Fluss-Rasterzellen bestimmt werden kann  ???
		m_pRSpeed->Set_Value(x, y, vMax2);		//Flussbezogene Grids: Flieszgeschwindigkeit der Flussgrids (lt. vorgegebenen pCr und nCr); (x,y) nach (ix,iy)
		m_pGrad->Set_Value( x, y, dzMax);		//Allgemeine Grids: Flussgefaellewert wird auch in der allgemeinen Gefaelledatei gesetzt; (x,y) nach (ix,iy)
		m_pDirec->Set_Value( x, y, i);			//Allgemeine Grids: Flieszrichtung Gitterbox; (x,y) nach (ix,iy)
	}
	else
	{
		m_pGrad->Set_Value( x, y, dzMax);		//Allgemeine Grids: Flussgefaellewert wird gesetzt; (x,y) nach (ix,iy)
		m_pDirec->Set_Value( x, y, i);			//Allgemeine Grids: Flieszrichtung Gitterbox; (x,y) nach (ix,iy)
	}
}


//---------------------------------------------------------
//#########################################################


bool CRivBasin::Set_BasinShare(void) //Fuer alle Werte innerhalb des RiverBasin gilt: Riverbox: Wert==0, Zuflussbox: Wert==>>Riverbox-Koordinate [xxxxyyyy] || alle Boxen Ausserhalb RiverBasin: Wert==-1
{
	if(m_pRivGrids)
	{
		m_pNumInFlowCells->Assign(0.0);

		int NX = m_pDTM->Get_NX();	//Anzahl der Reihen x
		int NY = m_pDTM->Get_NY();	//Anzahl der Zeilen y
		double CellNum0 = 0;
		double CellNum = 0;

		int x, y;
		int i = -1;


		
		//Gesamt-Flusseinzugsgebiet und einzelne Flussrasterzelleneinzuggebiete bestimmen und jeweilige Koordinate der Ziel-Flussrasterzelle setzten
		//---------------------------------------------------------

		for(int l=0; l < m_pDTM->Get_NCells() && Set_Progress_NCells(l); l++) //ueber alle Zellen des m_pDEM-Grids
		{
			m_pDTM->Get_Sorted(l, x, y); //sortieren der Zellen von hoechster (l=0) nach niedrigster...
			
			CellNum = 1; //Initialisierung der Zaehlung "vorherigerNachbarzellen" fuer erste Nachbarzelle (falls folgende if-Bedingung nicht erfuellt -> "gegenstandlos")
			CellNum0 = 0;

			//Erster Durchlauf: Zu "Gipfelzelle l" wird anhand des Abflusses die Muendungszelle im Flussgrid bestimmt (entspricht dem (x,y) fuer das gilt: m_pRivGrids->asDouble(x,y) != 0 )... 
			if(m_pNumInFlowCells->asDouble(x,y) == 0 && !m_pDTM->is_NoData(x,y)) //starten nur wenn Gipfelzelle (enstspricht: m_pNumInputCells->asDouble(x,y) == 0); falls != gab es schon eine Zelle die hoeher war u damit wurde der Zweig schon abgegangen 
			{
				while( !m_pDTM->is_NoData(x,y) && m_pRivGrids->asDouble(x,y) == 0 )
				{

					i = m_pDTM->Get_Gradient_NeighborDir(x, y);
					
					if(i >= 0) //es gibt eine niedrigere Nachbarzelle...
					{
											
						x = Get_xTo(i, x);
						y = Get_yTo(i, y);
						
						if(m_pNumInFlowCells->asDouble(x,y) <= 0) //Wenn Null -> erstes Durchlaufen dieses Astes -> immer plus 1 fuer Naechstebarchbarzelle
						{
							CellNum0 = CellNum;
							CellNum = CellNum + 1;
						}
						else //Ast wurde vorher schon durchlaufen -> vorherige DurchlaufWerte plus die statische Anzahl der Zellen (CellNum) des noch nicht durchlaufenden Astabschnittes 
							CellNum0 = m_pNumInFlowCells->asDouble(x,y) + CellNum;
												
						m_pNumInFlowCells->Set_Value(x, y, CellNum0); //Anzahl voheriger Zellen im Abflusverlauf werden gesetzt... vorher bereits Durchlaufene "aeste" sind beruecksichtigt

					}
					else		//Senke liegt vor
					{
						CellNum0 = m_pNumInFlowCells->asDouble(x,y) * -1;
						m_pNumInFlowCells->Set_Value(x, y, CellNum0);
						break;
					}

				}

				// Bei Zelle (x,y) handelt es sich nun um eine Flussrasterzelle oder eine Senke...
				
				if(m_pRivGrids->asDouble(x,y) != 0) //Wenn es eine Flussrasterzelle (also Muendungszelle) ist, Wert fuer alle alle Flaechenrasterzellen setzen, die in die Flussrasterzelle (x,y) muenden
				{
					
					m_pBasinShare->Set_Value(x, y, 0); //Muendungsrasterzelle wird auf Null gesetzt
									
					//KoordinatenDouble erzeugen FORMAT: xxxxyyyy
					double r = 0;
					int u = 10000*x + y;
					//
										
					r = u; //Wert der gesetzt wird : FORMAT: xxxxyyyy (Koordinate der "Ziel"Flussgridbox in die voherige Gitterboxen muenden)
					
					m_pDTM->Get_Sorted(l, x, y); //selber Durchlauf nochmal;  von der l-ten Gridbox bis Ziel-Flussfgridbox
					
					//Zweiter Durchlauf: Wiederum bei "Gipfelzelle l" beginnend wird nun fuer alle Gitterzellen auf dem Weg zur Muendungszelle der KoordinatenWert r gesetzt
					while( !m_pDTM->is_NoData(x,y) && m_pRivGrids->asDouble(x,y) == 0 )
					{
						i = m_pDTM->Get_Gradient_NeighborDir(x, y);

						if(i >= 0)
						{
							m_pBasinShare->Set_Value(x, y, r); //Koordinaten der Ziel-Flussgridbox werden gesetzt

							x = Get_xTo(i, x);
							y = Get_yTo(i, y);						
						}
						else
							{break;} //Nur fuer den Fall; i = -1 sollte eigentlich nicht mehr moeglich sein, da letzte Gridbox ja Flussgridbox...
					}
				}
				else
					{}
			}


			if(m_pDTM->is_NoData(x,y))
			{
				m_pNumInFlowCells->Set_NoData(x,y);
				m_pBasinShare->Set_NoData(x,y);
			}
		}
		//---------------------------------------------------------
		//#########################################################



		//-----------------------------------------------------
		// Anzahl der Gridboxes des Einzugsgebietes zaehlen:
		for(x=0; x < NX; x++)
		{
			for(y=0; y < NY; y++)
			{
				if(!m_pDTM->is_NoData(x,y) && m_pBasinShare->asDouble(x,y) != -1)
					BasinGrids++;
			}
		}
		

		//-----------------------------------------------------
		//Absolute Entnahmen in den Flussrasterzellen-Einzugsgebieten (Teileinzugsgebiet) berechnen und dort anteilig entnehmen

		if(Parameters("WCons")->asBool() && m_pWCons && m_pSharedRivCons)
		{
			for(x=0; x < NX; x++)
			{
				for(y=0; y < NY; y++)
				{
					if(!m_pDTM->is_NoData(x,y) && m_pRivGrids->asDouble(x,y) > 0 ) 
					{
						// Wenn Flussrasterzelle...
						int sx = 0;
						int sy = 0;
						double sum = 0;			//Summe der Rasterzellen des Teileinzuggebiets (ohne Flussrasterzelle)
						int u;
						double r = 0;
						double conSum = 0;		//Summe der Verbrauchswerte des Teileinzugsgebiets
						double conShare = 0;	//Berechnen der anteiligen Entnahme der Flussrasterzelle / Rasterzellen
						double cellShare = 0;	//Summe aller InFlowCell-Werte des Teileinzuggebiets
						double testSum = 0;
					
						conSum = m_pWCons->asDouble(x, y); //Summe der Verbrauchswerte des Teileinzugsgebiets (hier Flussrasterzelle)
						cellShare = m_pNumInFlowCells->asDouble(x, y); //Summe aller InFlowCell-Werte des Teileinzuggebiets (hier Flussrasterzelle)

						//KoordinatenDouble der derzeitigen Flussrasterzelle erzeugen - FORMAT: xxxxyyyy
						u = 10000*x + y;
						r = u;

						//alle Rasterzellen finden die in die Flussrasterzelle muenden
						for(int sx=0; sx < NX; sx++)
						{
							//#pragma omp parallel for //TEST hier nicht anwendbar
							for(int sy=0; sy < NY; sy++)
							{
								//Wasserverbrauchswerte alle Rasterzellen die in Flussrasterzelle (x,y) muenden addieren...
								if(!m_pDTM->is_NoData(sx,sy) && m_pRivGrids->asDouble(sx,sy) == 0 && m_pBasinShare->asDouble(sx,sy) == r )
								{
									if( m_pWCons)
										conSum = conSum + m_pWCons->asDouble(sx,sy); //Summe der Verbrauchswerte des Teileinzugsgebiets (hier uebrige Rasterzellen)

									sum = sum + 1; //Summe der Rasterzellen des Teileinzuggebiets (ohne Flussrasterzelle)

									cellShare = cellShare + m_pNumInFlowCells->asDouble(sx, sy);	//Summe aller InFlowCell-Werte des Teileinzuggebiets (hier uebrige Rasterzellen)
								}
							}
						}

						if(cellShare > 0)
						{
							if(!Parameters("WCons2")->asBool())
							//1) Gesammte Verbrauchssumme aus der jeweiligen Flussrasterzelle entnehmen
								m_pSharedRivCons->Set_Value(x, y, conSum);
							else
							{
								//2) Prozemtuale, anteilige Entnahme ueber das gesamte Teileinzugsgebiet
												
								conShare = ( m_pNumInFlowCells->asDouble(x, y) / cellShare ) * conSum;	//Berechnen der anteiligen Entnahme der Flussrasterzelle:
																										//Summe aller InFlowCell-Werte des Teileinzugsgebiets geteilt durch NumInFlowCells der Flussrasterzelle (x,y) -> Anteilieg Entnahme in Prozent;
																										//dann mit Gesamtennehame des Teileinzuggebiets conSum multiplizieren -> anteilige Entnahme
							
								m_pSharedRivCons->Set_Value(x, y, conShare);							//Setzen der anteiligen Entnahme der Flussrasterzelle 
								testSum = testSum + conShare;											//PruefSumme

								//Anteilige Entname aller anderen Rasterzellen des Teileinzuggebiets setzen, das in Flussrasterzelle (x,y) muenden
								for(int sx=0; sx < NX; sx++)
								{
									//#pragma omp parallel for //TEST hier nicht anwendbar
									for(int sy=0; sy < NY; sy++)
									{
										if(!m_pDTM->is_NoData(sx,sy) && m_pRivGrids->asDouble(sx,sy) == 0 && m_pBasinShare->asDouble(sx,sy) == r )
										{
											conShare = ( m_pNumInFlowCells->asDouble(sx, sy) / cellShare) * conSum;	//Berechnen der anteiligen Entnahme der Rasterzellen (Methode s. Flussrasterzelle oben)
											m_pSharedRivCons->Set_Value(sx, sy, conShare);								//Setzen der anteiligen Entnahme der Flussrasterzelle 
											testSum = testSum + conShare;												//PruefSumme		
										}
									}
								}

								//if( testSum != conSum ) //durch Rundungsdifferenzen kommt es bei dieser TestAbfrage zu unberechtigten Fehlermeldungen
								//	Message_Dlg("Fehler! Gesamtentnahme aus Teileinzugsgebiet entspricht nicht der Summe der berechneten anteiligen Entnahmen");
											
							//m_pNumInputCells->Set_Value(x, y, sum); //NUR zu Testzwecken
							//m_pBasinShare->Set_Value(x, y, sum/BasinGrids ); //Anzahl der Einzugsgebietszellen (anteilig) die in diese Flussgrid (x,y) fliessen
							}
						}
						else
							m_pSharedRivCons->Set_Value(x, y, conSum);
					}
				}
			}
		}
		return (true);
	}
	else
		return (false);
}


bool CRivBasin::WriteLog(string s)
{
	int i = 0;

	std::stringstream path0;
	std::string path;
	path0.str("");
	path0 << "e:" << "\\Log.txt";
	path = path0.str();

	ofstream myfile;
	myfile.open(path.c_str(), ios_base::app);

			myfile << s << "\n";
			Process_Set_Text(SG_T("Schreibe in Logdatei"));

	myfile.close();

return true;
}

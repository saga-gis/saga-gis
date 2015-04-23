
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
//                     LandFlow.cpp                      //
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


#include "LandFlow.h"
#include <math.h>
#include <string>
#include <iostream>
#include <conio.h>
#include "stdio.h"
#include <fstream>
#include "stdlib.h"
#include <sstream>
#include <exception>
#include <time.h>
#include <atltime.h>
#include <direct.h>

using namespace std;

#define ESC 27


//---------------------------------------------------------
//#########################################################

CLandFlow::CLandFlow(void)
{
	//-----------------------------------------------------
	// Modul information

	Set_Name		("LandFlow Version 1.0 (build 3.5.1b)"); //Manuelle erhöhung der WaterUseDATEN (NUR für Entnahme aus HG-Rasterzelle)
	Set_Author		("Christian Alwardt");
	Set_Description	("Landflow of RiverBasine");
	cVers = 3.51; 
	
	//###################################################################
	//-----------------------------------------------------
	//Parameters list...


	//-----------------------------------------------------
	//Parameter Gruppierung...		
	CSG_Parameter	*pNode0;
	CSG_Parameter	*pNode1;
	CSG_Parameter	*pNode2;
	CSG_Parameter	*pNode3;
	CSG_Parameter	*pNode4;
	CSG_Parameter	*pNode5;
	CSG_Parameter	*pNode11;
	CSG_Parameter	*pNode21;
	CSG_Parameter	*pNode41;
	CSG_Parameter	*pNode42;
	CSG_Parameter	*pNode51;
	CSG_Parameter	*pNode52;
	CSG_Parameter	*pNode53;
	CSG_Parameter	*pNode54;
	CSG_Parameter	*pNode55;

	
	pNode0	= Parameters.Add_Node(
		NULL	, "SOURCE_NODE"	, _TL("Eingangsdaten"),
		_TL("")
	);

	pNode0	= Parameters.Add_Node(
		NULL	, "TARGET_NODE"	, _TL("Ausgabedaten"),
		_TL("")
	);

	pNode1	= Parameters.Add_Node(
		NULL	, "GENERAL_NODE", _TL("Allgemeine Parameter"),
		_TL("")
	);

	pNode11	= Parameters.Add_Node(
		pNode1	, "LS_NODE", _TL("Vertikale Zuflüsse"),
		_TL("")
	);
	
	pNode2	= Parameters.Add_Node(
		NULL	, "FLOW_NODE", _TL("Parameterwerte HD Modell"),
		_TL("")
	);

	pNode21	= Parameters.Add_Node(
		pNode2	, "RIV_NODE", _TL("Hauptgerinne-Parameter (erweitertes HD-Modell)"),
		_TL("")
	);

	pNode3	= Parameters.Add_Node(
		NULL	, "WTHD_NODE", _TL("Dynamische Flächenwassernutzung und Abflussreduktion"),
		_TL("")
	);

	pNode4	= Parameters.Add_Node(
		NULL	, "RM_NODE", _TL("Punktuelle Rasterzellenmanipulation"),
		_TL("")
	);

	pNode41	= Parameters.Add_Node(
		pNode4	, "RM1_NODE", _TL("Rasterzelle 1"),
		_TL("Manipulation des Gerinne- bzw. Hauptgerinneabflusses: Rasterzelle 1")
	);

	pNode42	= Parameters.Add_Node(
		pNode4	, "RM2_NODE", _TL("Rasterzelle 2"),
		_TL("Manipulation des Gerinne- bzw. Hauptgerinneabflusses: Rasterzelle 2")
	);
	
	pNode5	= Parameters.Add_Node(
		NULL	, "MONI_NODE", _TL("Test- und Monitoring-Routinen"),
		_TL("")
	);

	pNode51	= Parameters.Add_Node(
		pNode5	, "EvP1_NODE", _TL("Evaluierungspunkt 1 [Default: Elbe5000]"),
		_TL("")
	);

	pNode52	= Parameters.Add_Node(
		pNode5	, "EvP2_NODE", _TL("Evaluierungspunkt 2 [Default: Elbe5000]"),
		_TL("")
	);

	pNode53	= Parameters.Add_Node(
		pNode5	, "EvP3_NODE", _TL("Evaluierungspunkt 3 [Default: Elbe5000]"),
		_TL("")
	);

	pNode54	= Parameters.Add_Node(
		pNode5	, "RBM_NODE", _TL("Ausgangsrasterzelle Flusseinzugsgebiet [Default: Elbe5000]"),
		_TL("")
	);
	
	pNode55	= Parameters.Add_Node(
		pNode5	, "TEST1_NODE", _TL("TestRoutine 1"),
		_TL("")
	);

	//-----------------------------------------------------
	//Grids...	
	Parameters.Add_Grid(
		Parameters("SOURCE_NODE"), "INPUT", "Geländemodell (DTM)",
		"Digitales Geländemodell des Flusseinzugsgebietes",
		PARAMETER_INPUT
	);

	Parameters.Add_Grid(
		Parameters("SOURCE_NODE"), "INPUT2"	, "Höhengradienten (Grad)",
		"Abflussgradienten jeder Rasterzelle des Flusseinzugsgebietes",
		PARAMETER_INPUT
	);
		
	Parameters.Add_Grid(
		NULL, "INPUT10"	, "Flusseinzugsgebiet (BasinShare)",
		"Abgrenzung des Flusseinzugsgebiets mit den Teileinzugsgebieten des Flussnetzwerkes",
		PARAMETER_INPUT
	);

	Parameters.Add_Grid(
		Parameters("SOURCE_NODE"), "INPUT8"	, "HG-Raster (HGGrid)",
		"Hauptgerinnerasterzellen",
		PARAMETER_INPUT
	);

	Parameters.Add_Grid(
		NULL, "INPUT9"	, "HG-Höhengradienten (HGGrad)",
		"Abflussgradienten der Hauptgerinnerasterzellen",
		PARAMETER_INPUT
	);

	Parameters.Add_Grid(
		NULL, "INPUT11"	, "Referenzverlauf Flussbilanz",
		"Raster des Referenzverlauf des (linearen) Hauptflussarmes, der zur Erstellung der Flusswasserbilanz herangezogen werden soll",
		PARAMETER_INPUT_OPTIONAL
		//ACHTUNG auf optionale Eingabe gesetzt -> 'Sicherheits-'Abfrage einbauen
	);

	Parameters.Add_Grid(
		NULL, "INPUT12"	, "Zuflussrasterzellen (NumInFlowCells)",
		"Raster mit Angabe über die Anzahl der Rasterzellen, die in eine spezifische Rasterzelle (x,y) abfließen",
		PARAMETER_INPUT_OPTIONAL
		//ACHTUNG auf optionale Eingabe gesetzt -> 'Sicherheits-'Abfrage einbauen
	);

	Parameters.Add_Grid(
		Parameters("WTHD_NODE"), "INPUT3" , "statische Wassserentnahme (statWUse)",
		"Flächenbasierte statische Wassserentnahme pro RiverGridbox in. ACHTUNG: Funktioniert ggf. nur bei bereits initiierten Systemen bzw. Vorlauf",
		PARAMETER_INPUT_OPTIONAL
		//ACHTUNG auf optionale Eingabe gesetzt -> 'Sicherheits-'Abfrage einbauen
	);

	Parameters.Add_Grid(
		Parameters("SOURCE_NODE"), "INPUT5"	, "Fluss-Speicher einlesen",
		"Simulation mit spezifischen Gerinne- bzw. Hauptgerinne-Speicherwerten (ChannelFlow) initiieren",
		PARAMETER_INPUT_OPTIONAL
		//ACHTUNG auf optionale Eingabe gesetzt -> 'Sicherheits-'Abfrage einbauen
	);

	Parameters.Add_Grid(
		Parameters("SOURCE_NODE"), "INPUT6"	, "Oberflächenabfluss-Speicher einlesen",
		"Simulation mit spezifischen Oberflächenabfluss-Speicherwerten (OverlandFlow) initiieren",
		PARAMETER_INPUT_OPTIONAL
		//ACHTUNG auf optionale Eingabe gesetzt -> 'Sicherheits-'Abfrage einbauen
	);

	Parameters.Add_Grid(
		Parameters("SOURCE_NODE"), "INPUT7"	, "Grundwasserabfluss-Speicher einlesen",
		"Simulation mit spezifischen Grundwasserabfluss-Speicherwerten (BaseFlow) initiieren",
		PARAMETER_INPUT_OPTIONAL
		//ACHTUNG auf optionale Eingabe gesetzt -> 'Sicherheits-'Abfrage einbauen
	);
	
	Parameters.Add_Grid(
		Parameters("TARGET_NODE"), "OUTPUT"	, "Fluss-Speicher ausgeben",
		"Fluss-Speicher 'CFCache' (ChannelFlowCache) am Ende der Simulation ausgeben - Initialisierungsdaten)",
		PARAMETER_OUTPUT
	);

	Parameters.Add_Grid(
		Parameters("TARGET_NODE"), "OUTPUT2" , "Oberflächenabfluss-Speicher ausgeben",
		"Oberflächenabfluss-Speicher 'OFCache' (OverlandFlowCache) am Ende der Simulation ausgeben - Initialisierungsdaten",
		PARAMETER_OUTPUT
	);

	Parameters.Add_Grid(
		Parameters("TARGET_NODE"), "OUTPUT3" , "Grundwasserabfluss-Speicher ausgeben",
		"Grundwasserabfluss-Speicher 'BFCache' (BaseFlowCache) am Ende der Simulation ausgeben - Initialisierungsdaten",
		PARAMETER_OUTPUT
	);

	Parameters.Add_Grid(
		Parameters("TARGET_NODE"), "OUTPUT4" , "Wasserflussvolumen",
		"Wasserflussvolumen in m³/s",
		PARAMETER_OUTPUT
	);

	Parameters.Add_Grid(
		Parameters("TARGET_NODE"), "OUTPUT5"	, "SumRunoffDrainage",
		"Aufsummieren der vertikalen Runoff und Drainage Zuflüsse für jede Rasterzelle über den gesamten Simulationsverlauf",
		PARAMETER_OUTPUT
	);

	Parameters.Add_Grid(
		Parameters("WTHD_NODE"), "OUTPUT6"	, "DynWUse",
		"Dynamisch eingelesene Flächenwasssernutzung pro Rasterzelle. ACHTUNG: Funktioniert nur bei bereits initiierten Systemen bzw. Vorlauf",
		PARAMETER_OUTPUT
		//siehe Funktion WConsRiv()
		//ACHTUNG auf optionale Eingabe gesetzt -> 'Sicherheits-'Abfrage einbauen
	);
	//-----------------------------------------------------
	
	//Allgemeine Parameter...	
	Parameters.Add_Value(
		Parameters("GENERAL_NODE"), "onlyRB"	, "Berechnung nur im Flusseinzugsgebiet",
		"Der Abfluss wird nur über die Rasterzellen des Flusseinzugsgebietes berechnet",
		PARAMETER_TYPE_Bool, true		
	);
	
	Parameters.Add_Value(
		Parameters("GENERAL_NODE"), "TimeStep"	, "Zeitschrittdauer [s]",
		"Dauer eines Simulations-Zeitschritts in Sekunden",
		PARAMETER_TYPE_Int, 1, 1, true
	);

	Parameters.Add_Value(
		Parameters("GENERAL_NODE"), "CalcT", "automatisierter max. Zeitschritt",
		"Automatisierte Berechnung der maximal möglichen Dauer eines Zeitschrittes in [s] - Zeitschrittdauern wird ignoriert.",
		PARAMETER_TYPE_Bool, true
	);

	Parameters.Add_Value(
		Parameters("GENERAL_NODE"), "sYear"	, "Startjahr",
		"Jahr in dem die Simulation zum 1. Januar gestartet wird",
		PARAMETER_TYPE_Int, 1999, 1979, true, 2009, true
	);

	Parameters.Add_Value(
		Parameters("GENERAL_NODE"), "DayNum"	, "Anzahl der Simulationstage",
		"Anzahl der gesamten Simulationtage über die berechnete wird",
		PARAMETER_TYPE_Int, 365, 1, true, 7670, true 
	);
			
	Parameters.Add_FilePath(
		Parameters("GENERAL_NODE")	, "Folder2", _TL("Speicherordner"),
		_TL("Ordern in den alle Ausgabedaten gespeichert werden sollen"), _TL("Textdatei|*.txt|All Files|*.*"), 0, false ,true ,false
	);
		
	Parameters.Add_FilePath(
		Parameters("LS_NODE")	, "Folder1", _TL("Pfad LS-Daten"),
		_TL("Ordnerpfad der Surface Runoff- und Drainagerasterdaten des Landoberflächenschemas"), _TL("SAGA Grid Files (*.sgrd)|*.sgrd|All Files|*.*"),0 ,false ,true ,false
	);
	
	Parameters.Add_Value(
		Parameters("LS_NODE"), "stRedFacR", "Reduzierungsfaktor Surface Runoff [% / 100]",
		"Statische Reduzierung des vertikalen Zuflusses 'Surface Runoff' in [% / 100] pro Zeitschritt.",
		PARAMETER_TYPE_Double, 0.0, 0.0, true, 1.0, true
	);

	Parameters.Add_Value(
		Parameters("LS_NODE"), "autoFacD"	, "Ausgleich der Surface Runoff Reduktion",
		"Erhöht automatisch die Drainage um denjenigen Volumenbetrag, um den der Surface Runoff reduziert wurde. Wenn gesetzt, wird etwaige Reduzierung der Drainage NICHT berücksichtigt!",
		PARAMETER_TYPE_Bool, 0		
	);

	Parameters.Add_Value(
		Parameters("LS_NODE"), "stRedFacD", "Reduzierungsfaktor Drainage [% / 100]",
		"Statische Reduzierung des vertikalen Zuflusses der 'Drainage' in [% / 100] pro Zeitschritt.",
		PARAMETER_TYPE_Double, 0.0, 0.0, true, 1.0, true
	);

	Parameters.Add_Value(
		Parameters("LS_NODE"), "OffsetR", "Offsetwert Surface Runoff [m³/s]",
		"Statischer Offsetwert auf den vertikalen Zufluss 'Surface Runoff' in [m³/s] je Rasterzelle. ACHTUNG wird auf jede Rasterzelle angewendet, daher limitiert auf +/-1 m³/s. Bei Wahl des Wertes Auflösung beachten! ",
		PARAMETER_TYPE_Double, 0.0, -1.0, true, 1.0, true
	);

	Parameters.Add_Value(
		Parameters("LS_NODE"), "OffsetD", "Offsetwert Drainage [m³/s]",
		"Statischer Offsetwert auf den vertikalen Zufluss 'Drainage' in [m³/s] je Rasterzelle. ACHTUNG wird auf jede Rasterzelle angewendet, daher limitiert auf +/-1 m³/s. Bei Wahl des Wertes Auflösung beachten!",
		PARAMETER_TYPE_Double, 0.0, -1.0, true, 1.0, true
	);

	Parameters.Add_Value(
		Parameters("GENERAL_NODE"), "CacheUse", "Initiierung mit vorhandenen Speichern",
		"Initialisierung der Simulation mit spezifischen Speicherwerten (Raster und NCache)",
		PARAMETER_TYPE_Bool, false
	);

	Parameters.Add_FilePath(
		Parameters("GENERAL_NODE")	, "Folder4", _TL("NCache einlesen"),
		_TL("Einlesen der NCache Textdatei"), _TL("Textdatei (*.txt)|*.txt|All Files|*.*"),0 ,false ,false ,false
	);

	Parameters.Add_Value(
		Parameters("GENERAL_NODE"), "wNC"	, "Ausgabe NCache",
		"Schreibt die Werte der Speicherkaskaden zum Ende Simulation in eine Textdatei (Initialisierungsdaten)",
		PARAMETER_TYPE_Bool, 0		
	);
	//-----------------------------------------------------


	//Parameterwerte HD Modell
	Parameters.Add_Value(
		Parameters("FLOW_NODE"), "ParamC"	, "Gerinne-Parameter cG",
		"Parameter cG zur Berechnung der Lagtime kG des Gerinneabflusses",
		PARAMETER_TYPE_Double, 6e-2, 0, true		
	);

	Parameters.Add_Value(
		Parameters("FLOW_NODE"), "ParamG"	, "Oberflächen-Parameter cO",
		"Parameter cO zur Berechnung der Lagtime kO des Oberflächenabflusses",
		PARAMETER_TYPE_Double, 357e-3, 0, true
	);

	Parameters.Add_Value(
		Parameters("FLOW_NODE"), "ParamB"	, "Grundwasser-Parameter pB",
		"Parameter pB zur Berechnung der Lagtime kB des Grundwasserabflusses",
		PARAMETER_TYPE_Double, 300, 0, true
	);

	Parameters.Add_Value(
		Parameters("FLOW_NODE"), "nG"	, "Gerinne-Speicherkaskade nG",
		"Festlegen, wieviele Speicher die Gerinne-Speicherkaskade nG enthält",
		PARAMETER_TYPE_Int, 3, 1, true		
	);

	Parameters.Add_Choice(
		Parameters("RIV_NODE")	, "RivG"	, _TL("Berücksichtigung der Hauptgerinnerasterzellen?"),
		_TL("Seperate Berücksichtigung und Berechnung über Hauptgerinnerasterzellen - neben den normalen Gerinnerasterzellen."),
		CSG_String::Format(SG_T("%s|%s|")/*(SG_T("%s|%s|%s|%s|")*/,
			_TL("nein"),
			_TL("ja; bestimmen anhand eines Hauptgerinnerasters (HG Raster)")/*,
			_TL("ja; bestimmen über das Wasserflussvolumen (P:RFlow)"),
			_TL("ja; bestimmen über die jeweilige Anzahl der ZuflussRasterzellen (IR:NumInFlowCells, P:NumCells")*/
		), 1
	);

	Parameters.Add_Value(
		Parameters("RIV_NODE"), "ParamCr"	, "Hauptgerinne-Parameter cHG",
		"Parameter cHG zur Berechnung der Lagtime kHG des ChannelFlow-River [optional siehe oben]",
		PARAMETER_TYPE_Double, 0.007, 0, true, 1.0, true		
	);

	Parameters.Add_Value(
		Parameters("RIV_NODE"), "nHG"	, "Hauptgerinne-Speicherkaskade nHG",
		"Festlegen, wieviele Speicher die Hauptgerinne-Speicherkaskade enthält  [optional siehe oben]",
		PARAMETER_TYPE_Int, 1, 1, true		
	);

	Parameters.Add_Value(
		Parameters("FLOW_NODE"), "EnfVmax", "Abflussgeschwindigkeit begrenzen",
		"Die mittlere Wasserabflussgeschwindigkeit wird auf einen Höchstwert begrenzt - Zeitschrittvorgaben und automatisierter Zeitschritt wird überschrieben.",
		PARAMETER_TYPE_Bool, true
	);

	Parameters.Add_Value(
		Parameters("FLOW_NODE"), "VTresh"	, "Oberer Abflussgeschwindigkeitsgrenzwert [km/h]",
		"Maximale mittlere Wasserabflussgeschwindigkeit in km/h",
		PARAMETER_TYPE_Double, 4, 0, true, 10, true		//Achtung!! Obergrenze statisch eingestellt
	);
	//-----------------------------------------------------


	//Dynamische Wasserentnahme und Abflussreduktion
	Parameters.Add_Choice(
		Parameters("WTHD_NODE")	, "WCons"	, _TL("Dynamische Flächenwassernutzung..."),
		_TL("Auswahl der Art der dynamischen Flächenwasserwassernutzung (WUse). ACHTUNG: Funktioniert ggf. nur bei bereits initiierten Systemen bzw. nach Vorlauf"),
		CSG_String::Format(SG_T("%s|%s|%s|%s|"),
			_TL("keine"),
			_TL("...anteilig aus den Hauptgerinnerasterzellen"),
			_TL("...anteilig aus Rasterzellen der Teileinzugegebiete"),
			_TL("...genau den entsprechenden Wert aus der jeweiligen Rasterzelle entnehmen")
		), 0
	);
	
	Parameters.Add_FilePath(
		Parameters("WTHD_NODE")	, "Folder3", _TL("Ordnerpfad der WUse Daten"),
		_TL("Speicherpfad der dynamischen Wassernutzungsdaten"), _TL("SAGA Grid Files (*.sgrd)|*.sgrd|All Files|*.*"),0 ,false ,true ,false
	);


	Parameters.Add_Choice(
		Parameters("WTHD_NODE")	, "WConUnit"	, _TL("WUse Einheit"),
		_TL("Einheit in der die WUse Daten vorliegen"),
		CSG_String::Format(SG_T("%s|%s|"),
			_TL("m³/s"),
			_TL("m³/Monat")
		), 0
	);

	Parameters.Add_Value(
		Parameters("WTHD_NODE"), "WConsD"	, "Vorlauftage ohne Entnahmen",
		"Anzahl der Simulationestage bevor eine Wasserentnahme berücksichtigt wird",
		PARAMETER_TYPE_Int, 0, 0, true
	);

	Parameters.Add_Value(
		Parameters("WTHD_NODE"), "WConThres", "Abflussschwellenwert [m³/s]",
		"Fester Abflussschwellenwert in m³/s für das Hauptgerinne, der durch Entnahmen nicht unterschritten werden soll.",
		PARAMETER_TYPE_Double, 0.0, 0.0, true
	);

	Parameters.Add_Value(
		Parameters("WTHD_NODE"), "stConsAll", "Abflussreduzierungsfaktor Gerinnerasterzellen [% / 100]",
		"Statische Reduzierung des Gesamtabflusses in den Gerinnerasterzellen um [% / 100] pro Zeitschritt.",
		PARAMETER_TYPE_Double, 0.0, 0.0, true, 1.0, true
	);

	Parameters.Add_Value(
		Parameters("WTHD_NODE"), "stConsRiv", "Abflussreduzierungsfaktor HG-Rasterzellen [% / 100]",
		"Statische Reduzierung des Gesamtabflusses  in den Hauptgerinnegitterboxen um [% / 100] pro Zeitschritt.",
		PARAMETER_TYPE_Double, 0.0, 0.0, true, 1.0, true
	);
	//-----------------------------------------------------

	//punktuelle Rasterzellenmanipulation
	//RM1
	Parameters.Add_Choice(
		Parameters("RM1_NODE")	, "vRM1"	, _TL("Abflussmanipulation Rasterzelle 1... "),
		_TL("Der Gerinne- bzw. Hauptgerinneabfluss kann mit dieser Methode für Rasterzelle 1 manipuliert werden. "),
		CSG_String::Format(SG_T("%s|%s|%s|"),
			_TL("nein"),
			_TL("Ja, berechneten Abfluss verändern: res. Abfluss = berechn. Abfluss * q + a"),
			_TL("Ja, Abfluss manuell vorgeben: res. Abfluss = Speicherinhalt * q + a")
		), 0
	);

	Parameters.Add_Value(
		Parameters("RM1_NODE"), "RM1x"	, "Rasterzelle 1 [x-Koord.]",
		"x-Koordinate der Rasterzelle 1, für die der Abfluss manipulatiert werden soll",
		PARAMETER_TYPE_Int, -1, -1, true		
	);

	Parameters.Add_Value(
		Parameters("RM1_NODE"), "RM1y"	, "Rasterzelle 1 [y-Koord.]",
		"y-Koordinate der Rasterzelle 1, für die der Abfluss manipulatiert werden soll",
		PARAMETER_TYPE_Int, -1, -1, true		
	);

	Parameters.Add_Value(
		Parameters("RM1_NODE"), "RM1q"	, "q [%/100]",
		"Prozentualer Faktor q [%/100] für Rasterzelle 1",
		PARAMETER_TYPE_Double, 1, 0, true		
	);

	Parameters.Add_Value(
		Parameters("RM1_NODE"), "RM1a"	, "a [+-m³/s]",
		"Addidativer Offsetwert a [+-m³/s] für Rasterzelle 1",
		PARAMETER_TYPE_Double, 0		
	);

	//RM22
	Parameters.Add_Choice(
		Parameters("RM2_NODE")	, "vRM2"	, _TL("Abflussmanipulation Rasterzelle 2... "),
		_TL("Der Gerinne- bzw. Hauptgerinneabfluss kann mit dieser Methode für Rasterzelle 2 manipuliert werden. "),
		CSG_String::Format(SG_T("%s|%s|%s|"),
			_TL("nein"),
			_TL("Ja, berechneten Abfluss verändern: res. Abfluss = berechn. Abfluss * q + a"),
			_TL("Ja, Abfluss manuell vorgeben: res. Abfluss = Speicherinhalt * q + a")
		), 0
	);

	Parameters.Add_Value(
		Parameters("RM2_NODE"), "RM2x"	, "Rasterzelle 2 [x-Koord.]",
		"x-Koordinate der Rasterzelle 2, für die der Abfluss manipulatiert werden soll",
		PARAMETER_TYPE_Int, -1, -1, true		
	);

	Parameters.Add_Value(
		Parameters("RM2_NODE"), "RM2y"	, "Rasterzelle 2 [y-Koord.]",
		"y-Koordinate der Rasterzelle 2, für die der Abfluss manipulatiert werden soll",
		PARAMETER_TYPE_Int, -1, -1, true		
	);

	Parameters.Add_Value(
		Parameters("RM2_NODE"), "RM2q"	, "q [%/100]",
		"Prozentualer Faktor q [%/100] für Rasterzelle 2",
		PARAMETER_TYPE_Double, 1, 0, true		
	);

	Parameters.Add_Value(
		Parameters("RM2_NODE"), "RM2a"	, "a [+-m³/s]",
		"Addidativer Offsetwert a [+-m³/s] für Rasterzelle 2",
		PARAMETER_TYPE_Double, 0		
	);
	

	//Test- und Monitoring-Routinen
	//EvP1
	Parameters.Add_String(
		Parameters("EvP1_NODE")	, "EvP1s"	, _TL("EvP1 Name"),
		_TL("Name des Evaluierungspunktes 1"),
		SG_T("NeuDarchau.txt")
	);

	Parameters.Add_Value(
		Parameters("EvP1_NODE"), "EvP1x"	, "EvP1 Rasterzelle [x-Koord.]",
		"x-Koordinate der spezifischen Hauptgerinnerasterzelle des Evaluerungspunktes 1",
		PARAMETER_TYPE_Int, 30, 0, true		
	);

	Parameters.Add_Value(
		Parameters("EvP1_NODE"), "EvP1y"	, "EvP1 Rasterzelle [y-Koord.]",
		"y-Koordinate der spezifischen Hauptgerinnerasterzelle des Evaluerungspunktes 1",
		PARAMETER_TYPE_Int, 115, 0, true		
	);

	//EvP2
	Parameters.Add_String(
		Parameters("EvP2_NODE")	, "EvP2s"	, _TL("EvP2 Name"),
		_TL("Name des Evaluierungspunktes 2"),
		SG_T("Lutherstadt-Wittenberg.txt")
	);

	Parameters.Add_Value(
		Parameters("EvP2_NODE"), "EvP2x"	, "EvP2 Rasterzelle [x-Koord.]",
		"x-Koordinate der spezifischen Hauptgerinnerasterzelle des Evaluerungspunktes 2",
		PARAMETER_TYPE_Int, 54, 0, true		
	);

	Parameters.Add_Value(
		Parameters("EvP2_NODE"), "EvP2y"	, "EvP2 Rasterzelle [y-Koord.]",
		"y-Koordinate der spezifischen Hauptgerinnerasterzelle des Evaluerungspunktes 2",
		PARAMETER_TYPE_Int, 85, 0, true		
	);

	//EvP3
	Parameters.Add_String(
		Parameters("EvP3_NODE")	, "EvP3s"	, _TL("EvP3 Name"),
		_TL("Name des Evaluierungspunktes 3"),
		SG_T("Schöna.txt")
	);

	Parameters.Add_Value(
		Parameters("EvP3_NODE"), "EvP3x"	, "EvP3 Rasterzelle [x-Koord.]",
		"x-Koordinate der spezifischen Hauptgerinnerasterzelle des Evaluerungspunktes 3",
		PARAMETER_TYPE_Int, 78, 0, true		
	);

	Parameters.Add_Value(
		Parameters("EvP3_NODE"), "EvP3y"	, "EvP3 Rasterzelle [y-Koord.]",
		"y-Koordinate der spezifischen Hauptgerinnerasterzelle des Evaluerungspunktes 3",
		PARAMETER_TYPE_Int, 65, 0, true		
	);

	//RBM
	Parameters.Add_Value(
		Parameters("RBM_NODE"), "RBMx"	, "Ausgangsrasterzelle [x-Koord.]",
		"x-Koordinate der Ausgangsrasterzelle des Flusseinzugsgebietes",
		PARAMETER_TYPE_Int, 16, 0, true		
	);

	Parameters.Add_Value(
		Parameters("RBM_NODE"), "RBMy"	, "Ausgangsrasterzelle [y-Koord.]",
		"y-Koordinate der Ausgangsrasterzelle des Flusseinzugsgebietes",
		PARAMETER_TYPE_Int, 121, 0, true		
	);
	
	Parameters.Add_Value(
		Parameters("MONI_NODE"), "wP"	, "SimParameters",
		"Schreibt wichtige, der Simulation zugrundeliegenden Parameter in eine Textdatei",
		PARAMETER_TYPE_Bool, 1		
	);
		
	Parameters.Add_Value(
		Parameters("MONI_NODE"), "eP"	, "Fehlerprotokoll",
		"Schreibt Fehler in Textfile",
		PARAMETER_TYPE_Bool, 1		
	);

	Parameters.Add_Value(
		Parameters("MONI_NODE"), "MoniLog1"	, "RiverBasinDay-Monitoring",
		"Monitoring tägicher Werte des Flusseinzugsgebiets",
		PARAMETER_TYPE_Bool, 1		
	);
	
	Parameters.Add_Value(
		Parameters("MONI_NODE"), "MoniLog3"	, "RiverBasinMonth-Monitoring",
		"Monitoring monatlicher Werte des Flusseinzugsgebiets",
		PARAMETER_TYPE_Bool, 1		
	);
	
	Parameters.Add_Value(
		Parameters("MONI_NODE"), "MoniLog2"	, "WSystem-Monitoring",
		"Monitoring von Zu- und Abflusswerten des WasserGesamtsystems",
		PARAMETER_TYPE_Bool, 1		
	);
	
	//Test1
	Parameters.Add_Choice(
		Parameters("TEST1_NODE")	, "Test1"	, _TL("Testroutine1 durchführen... "),
		_TL("Wählen ob TestRoutine 1 durchgeführt werden soll... 1) nur für Teileinzugsgebiet der HG-Rasterzelle oder 2) für das Flusseinzugsgebiet bis zum Erreichen der HG-Rasterzelle."),
		CSG_String::Format(SG_T("%s|%s|%s|"),
			_TL("nein"),
			_TL("Ja, TestRoutine1 nur für Teileinzugsgbiet der HG-Rasterzelle"),
			_TL("Ja, TestRoutine1 für Flusseinzugsgebiet bis zu der HG-Rasterzelle")
		), 0
	);

	Parameters.Add_Value(
		Parameters("TEST1_NODE"), "xt1"	, "Hauptgerinnerasterzelle [x-Koord.]",
		"x-Koordinate der spezifischen Hauptgerinnerasterzelle für TestRoutine 1",
		PARAMETER_TYPE_Int, 0, 0, true		
	);

	Parameters.Add_Value(
		Parameters("TEST1_NODE"), "yt1"	, "Hauptgerinnerasterzelle [y-Koord.]",
		"y-Koordinate der spezifischen Hauptgerinnerasterzelle für TestRoutine 1",
		PARAMETER_TYPE_Int, 0, 0, true		
	);
	//-----------------------------------------------------


	/*	Parameters.Add_Value(
		Parameters("RIV_NODE"), "RFlow"	, "Wasserflussvolumen [untere Grenze]",
		"Unterer Grenzwert des Wasserflussvolumens, ab dem eine Flussrasterzelle vorliegen soll [siehe Auswahl 'Flussrasterzellen berücksichtigen'].",
		PARAMETER_TYPE_Double, 0, 0, true		
	);
		
		Parameters.Add_Value(
		Parameters("RIV_NODE"), "NumCells"	, "ZuflussRasterzellen [untere Grenze]",
		"Unterer Grenzwert der Anzahl an ZuflussRasterzellen, ab der eine Flussrasterzelle vorliegen soll [siehe Auswahl 'Flussrasterzellen berücksichtigen'].",
		PARAMETER_TYPE_Int, 0, 0, true		
	);*/
	
	//###################################################################
}




///////////////////////////////////////////////////////////
//														 //
//	Hauptfunktion										 //
//														 //
///////////////////////////////////////////////////////////


bool CLandFlow::On_Execute(void)
{
	
	//-----------------------------------------------------
	//SAGA-Input
	m_pDTM = Parameters("INPUT")->asGrid();
	m_pGrad = Parameters("INPUT2")->asGrid();
	m_pWConsIn = Parameters("INPUT3")->asGrid(); //statisches WaterConsumption Grid in m3/s -> [kg m-2 s-1]
	m_pCCacheIn = Parameters("INPUT5")->asGrid();
	m_pRCacheIn = Parameters("INPUT6")->asGrid();
	m_pDCacheIn = Parameters("INPUT7")->asGrid();
	m_pRivGrids = Parameters("INPUT8")->asGrid();
	m_pRivGrad = Parameters("INPUT9")->asGrid();
	m_pBasinShare = Parameters("INPUT10")->asGrid();
	m_pRivBalanceGrid = Parameters("INPUT11")->asGrid();
	m_pNumInFlow = Parameters("INPUT12")->asGrid();
	m_pCCacheOut = Parameters("OUTPUT")->asGrid();
	m_pRCacheOut = Parameters("OUTPUT2")->asGrid();
	m_pDCacheOut = Parameters("OUTPUT3")->asGrid();
	m_pChannelFlow = Parameters("OUTPUT4")->asGrid();
	m_pSumRunoffDrainage = Parameters("OUTPUT5")->asGrid();
	m_pWConsOut = Parameters("OUTPUT6")->asGrid(); //dynamisch imm Programmablauf generiertes WaterConsumption Grid in [kg m-2 s-1]
	m_pTStep = Parameters("TimeStep")->asInt(); // [s]
	m_sYear0 = Parameters("sYear")->asInt(); // [y]
	m_pDays = Parameters("DayNum")->asInt();
	m_pLSDataFolder = Parameters("Folder1")->asString();
	m_pDataSaveFolder = Parameters("Folder2")->asString();
	m_pWConDataFolder = Parameters("Folder3")->asString();
	m_pNCacheFolder = Parameters("Folder4")->asString();
	m_pVTresh = Parameters("VTresh")->asDouble(); // [km/h]
	m_pWConsDIn = Parameters("WConsD")->asInt();
	m_pWConThres = Parameters("WConThres")->asDouble();		//Schwellenwert für HG-Abfluss der auch bei Entnahmen nicht unterschritten werden soll
	m_pOffsetR = Parameters("OffsetR")->asDouble();			//Offsetwert auf den vertikalen Zufluss 'RunOff' pro Rasterzelle in m³/s
	m_pOffsetD = Parameters("OffsetD")->asDouble();			//Offsetwert auf den vertikalen Zufluss 'Drainage' pro Rasterzelle in m³/s
	m_pConsFacAll = (Parameters("stConsAll")->asDouble());	// Entnahmefaktor aus Gitterboxen [%]/[dt]
	m_pConsFacRiv = (Parameters("stConsRiv")->asDouble());	// Entnahmefaktor aus Fluss-Gitterboxen [%]/[dt]
	m_pRedFacR = (Parameters("stRedFacR")->asDouble());		// Reduzierungsfaktor des RunOff [%]/[dt]
	m_pRedFacD = (Parameters("stRedFacD")->asDouble());		// Reduzierungsfaktor der Drainage [%]/[dt]
	nC = Parameters("nG")->asInt();							//Anzahl der Speicherkaskaden des ChannelFlow Speichers
	nCr = Parameters("nHG")->asInt();						//Anzahl der Speicherkaskaden des ChannelFlow River-Speichers
	C0 = Parameters("ParamC")->asDouble();
	C0r = Parameters("ParamCr")->asDouble();				//	Parameter für ChannelFlow im River [optional]
	G0 = Parameters("ParamG")->asDouble();
	pB = Parameters("ParamB")->asDouble();
	var2 = Parameters("MoniLog1")->asBool();
	var3 = Parameters("MoniLog2")->asBool();
	var4 = Parameters("MoniLog3")->asBool();
	
	m_pEvP1s = Parameters("EvP1s")->asString();
	m_pEvP2s = Parameters("EvP2s")->asString();
	m_pEvP3s = Parameters("EvP3s")->asString();

	


	//-----------------------------------------------------
	//Setzen von statischen Programmvariablen [0]=off [1]=on 
	var1 = 1;	//Prozentuale Wasserentnahme zulassen
	//-----------------------------------------------------

	//-----------------------------------------------------
	//Simulationsstart: Startzeit und -datum erfassen
	time_t rawtime;
	struct tm * timeinfo;
	time (&rawtime);
	timeinfo = localtime (&rawtime);
	//-----------------------------------------------------


	//-----------------------------------------------------	
	//SpeicherDateipfad (sfile) für diese Simulation wird generiert und entsprechender Ordner erstellt - Ordnerformat : ddmmyy_HHMM
	std::stringstream sPath0;
	sPath0.str("");
	char buffer [20];
	strftime(buffer,20,"%d%m%y_%H%M",timeinfo);
	sPath0 << m_pDataSaveFolder << "\\" << buffer;		//^Speicherdateipfad
	m_pSPath = sPath0.str();							//^
	
	sPath0.str("");
	sPath0 << m_pSPath << "\\Grids";					//^Unter-Speicherdateipfad1
	m_pSPath2 = sPath0.str();							//^
	
	sPath0.str("");
	sPath0 << m_pSPath << "\\RivBalance";				//^Unter-Speicherdateipfad2
	m_pSPath3 = sPath0.str();							//^

	_mkdir(m_pSPath.c_str());							//SaveOrdner zum speichern der Simulationsdaten wird erstellt
	_mkdir(m_pSPath2.c_str());							//Unter-SaveOrdner1 zum speichern der Grid-Dateien wird erstellt
	_mkdir(m_pSPath3.c_str());							//Unter-SaveOrdner2 zum speichern der Grid-Dateien wird erstellt
	//-----------------------------------------------------
	//LoadPfad


	//-----------------------------------------------------
	//FileName-String 
	std::stringstream file0;
	std::string file;

	
	//-----------------------------------------------------
	//Parameter
	m_sYear = m_sYear0;		//Jahresvariable initiieren
	nG = 1;					//Anzahl der Speicherkaskaden des GroundFlow Speichers - STATISCH
	NX = m_pDTM->Get_NX();	//Anzahl der Reihen x
	NY = m_pDTM->Get_NY();	//Anzahl der Zeilen y
	NumRBGrids = 0;			//Anzahl der Rasterzellen im Flusseinzugsgebiet
	NumGrids = 0;			//Anzahl der Rasterzellen im DTM (ohne NoDataZellen)
	
	errC = 0;				//Fehlercode Variable
	errC2 = 0;				//Fehlercode Variable2
	errC3 = 0;				//Fehlercode Variable3
	errC4 = 0;				//Fehlercode Variable4
	
	m_pWConsD = m_pWConsDIn; // Operator für "verbleibende Tage bis Entnahme" wird auf Input-Wert gesetzt


  //######################################################
	//Prüfroutinen
	//-----------------------------------------------------
	//STATISCHE SICHERHEITSANGABEN!! ÜBERARBEITEN!!!
	if(m_sYear == 1989 && m_pDays > 7670)
		WriteLog("Fehler! Anzahl der Tageschritte zu groß - übersteigt Anzahl der Datensätze!");
	if(m_sYear == 1990 && m_pDays > 7305)
		WriteLog("Fehler! Anzahl der Tageschritte zu groß - übersteigt Anzahl der Datensätze!");
	if(m_sYear == 1991 && m_pDays > 6940)
		WriteLog("Fehler! Anzahl der Tageschritte zu groß - übersteigt Anzahl der Datensätze!");
	if(m_sYear == 1992 && m_pDays > 6575)
		WriteLog("Fehler! Anzahl der Tageschritte zu groß - übersteigt Anzahl der Datensätze!");
	if(m_sYear == 1993 && m_pDays > 6209)
		WriteLog("Fehler! Anzahl der Tageschritte zu groß - übersteigt Anzahl der Datensätze!");
	if(m_sYear == 1994 && m_pDays > 5844)
		WriteLog("Fehler! Anzahl der Tageschritte zu groß - übersteigt Anzahl der Datensätze!");
	if(m_sYear == 1995 && m_pDays > 5479)
		WriteLog("Fehler! Anzahl der Tageschritte zu groß - übersteigt Anzahl der Datensätze!");
	if(m_sYear == 1996 && m_pDays > 5114)
		WriteLog("Fehler! Anzahl der Tageschritte zu groß - übersteigt Anzahl der Datensätze!");
	if(m_sYear == 1997 && m_pDays > 4748)
		WriteLog("Fehler! Anzahl der Tageschritte zu groß - übersteigt Anzahl der Datensätze!");
	if(m_sYear == 1998 && m_pDays > 4383)
		WriteLog("Fehler! Anzahl der Tageschritte zu groß - übersteigt Anzahl der Datensätze!");
	if(m_sYear == 1999 && m_pDays > 4018)
		WriteLog("Fehler! Anzahl der Tageschritte zu groß - übersteigt Anzahl der Datensätze!");
	if(m_sYear == 2000 && m_pDays > 3653)
		WriteLog("Fehler! Anzahl der Tageschritte zu groß - übersteigt Anzahl der Datensätze!");
	if(m_sYear == 2001 && m_pDays > 3287)
		WriteLog("Fehler! Anzahl der Tageschritte zu groß - übersteigt Anzahl der Datensätze!");
	if(m_sYear == 2002 && m_pDays > 2922)
		WriteLog("Fehler! Anzahl der Tageschritte zu groß - übersteigt Anzahl der Datensätze!");
	if(m_sYear == 2003 && m_pDays > 2557)
		WriteLog("Fehler! Anzahl der Tageschritte zu groß - übersteigt Anzahl der Datensätze!");
	if(m_sYear == 2004 && m_pDays > 2192)
		WriteLog("Fehler! Anzahl der Tageschritte zu groß - übersteigt Anzahl der Datensätze!");
	if(m_sYear == 2005 && m_pDays > 1826)
		WriteLog("Fehler! Anzahl der Tageschritte zu groß - übersteigt Anzahl der Datensätze!");
	if(m_sYear == 2006 && m_pDays > 1461)
		WriteLog("Fehler! Anzahl der Tageschritte zu groß - übersteigt Anzahl der Datensätze!");
	if(m_sYear == 2007 && m_pDays > 1096)
		WriteLog("Fehler! Anzahl der Tageschritte zu groß - übersteigt Anzahl der Datensätze!");
	if(m_sYear == 2008 && m_pDays > 731)
		WriteLog("Fehler! Anzahl der Tageschritte zu groß - übersteigt Anzahl der Datensätze!");
	if(m_sYear == 2009 && m_pDays > 365)
		WriteLog("Fehler! Anzahl der Tageschritte zu groß - übersteigt Anzahl der Datensätze!");
  //######################################################
	
  //Analyse-------------------------------------------
	for(int x=0; x < NX; x++)
	{
		for(int y=0; y < NY; y++)
		{
				
			if(!m_pDTM->is_NoData(x,y))
			{
				NumGrids++;								//Anzahl der Rasterzellen im DTM (ohne NoDatazellen)
				if(m_pBasinShare->asDouble(x,y) >= 0)	
					NumRBGrids++;						//Anzahl der Rasterzellen im Flusseinzugsgebiet
			}
		}
	}
  //--------------------------------------------------

	
	
  //###################################################################	
  //######################################################
	//Ermittlung kMin, vMax
	//
	double zMax, zMax2;
	double k_Min0;			//temporärer Operator zur bestimmung von KMin und vMax
	int n0;					//temporärer Operator zur bestimmung von KMin und vMax 

	//-----------------------------------------------------
	//nMax Wird benötigt um Einträge in NCArray-Array und -File zu setzten / zu lesen | Eigentlich sollte gelten nC >= nCr 
	if(nC >= nCr)
		nMax = nC;
	else
		nMax = nCr;
	//-----------------------------------------------------
		
	zMax = m_pGrad->Get_ZMax();			//liefert maximale Steigung des DEM Grids über GradGrid
	//minimalsten Retetionskoeffizienten der jeweiligen Abflussarten bestimmen
	kMinG = G0/(nG*2) * (m_pDTM->Get_Cellsize() / 1000 ) / pow(zMax, 0.1 );		//anhand zMax Berechnung der kleinsten GroundFlow-Lagtime
	kMinC = C0/(nC*2) * (m_pDTM->Get_Cellsize() / 1000 ) / pow(zMax, 0.1 );		//anhand zMax Berechnung der kleinsten ChannelFlow-Lagtime
	kMinB = pB * (m_pDTM->Get_Cellsize() / 1000 ) / 50;						//anhand zMax Berechnung der kleinsten BaseFlow-Lagtime
	
	//mittlere vMax der jeweiligen Abflussarten bestimmen (in einer Speicher Kaskade entspricht die Mittlere MAximalgeschwindigkeit der Summe der einzelnen Speicher-Maximalgeschwindigkeiten)
	vMaxC =  m_pDTM->Get_Cellsize() / 1000 / kMinC / nC / 24;
	vMaxG =  m_pDTM->Get_Cellsize() / 1000 / kMinG / nG / 24;
	
	// falls Flusssgrids berücksichtigt werden...
	if( Parameters("RivG")->asInt() > 0 ) 
	{
		if( m_pRivGrids && m_pRivGrad)
		{
			zMax2 = m_pRivGrad->Get_ZMax(); //liefert maximale Steigung der RiverGrids über RiverGradIn
			kMinCr = C0r/(nCr*2) * (m_pDTM->Get_Cellsize() / 1000 ) / pow(zMax2, 0.1 );	//anhand zMax2 Berechnung der kleinsten ChannelFlow River-Lagtime
			vMaxCr =  m_pDTM->Get_Cellsize() / 1000 / kMinCr / nCr / 24;
			//-----------------------------------------------------
			//nMax Wird benötigt um Einträge in NCArray-Array und -File zu setzten / zu lesen | Eigentlich sollte gelten nC >= nCr 
			if(nC >= nCr)
				nMax = nC;
			else
				nMax = nCr;
			//-----------------------------------------------------
		}
		else
		{
			Message_Dlg("Flussgrids können nicht berücksichtigt werden, da kein RiverGrids und/oder RiverGrad-Input gesetzt wurde");
			return (false);
		}
	}
	else //Wenn Flussgrids nicht extra verücksichtigt werden
	{
		kMinCr = -1; 
		vMaxCr = -1;
	}



	//vMax aller Abflussarten bestimmen
	if(vMaxCr >= vMaxC)
		vMax = vMaxCr;
	else
		vMax = vMaxC;
	
	if(vMaxG > vMax)
		vMax = vMaxG;

	//Wenn mittlere Geschwindigkeitsobergrenze gesetzt und diese kleiner als errechnete mittlere Maximalgeschwindigkeit vMax... 
	if( Parameters("EnfVmax")->asBool()) //Retentionskoeffizienten nach "oben" anpassen um mittlere Geschwindigkeitsobergrenze einzuhalten
	{
		if( m_pVTresh < vMax)
		{
			//mittlere Geschwindigkeitsobergrenze wird (bisher) allgemein für alle Abflussarten gesetzt!!
			vMax = m_pVTresh;
						
			if( (m_pDTM->Get_Cellsize() / 1000 / vMax / nC / 24) > kMinC)
				kMinC = m_pDTM->Get_Cellsize() / 1000 / vMax / nC / 24;
			if( Parameters("RivG")->asInt() > 0 && (m_pDTM->Get_Cellsize() / 1000 / vMax / nCr / 24) > kMinCr )
				kMinCr = m_pDTM->Get_Cellsize() / 1000 / vMax / nCr / 24;
			if( (m_pDTM->Get_Cellsize() / 1000 / vMax / nG / 24) > kMinG)
				kMinG = m_pDTM->Get_Cellsize() / 1000 / vMax / nG / 24;
		}
		else
			Message_Dlg("Eingabe der Geschwindigkeitsobergrenze ist größer als maximal berechnete Geschwindigkeit - Eingabe wird ignoriert");
	}
	
	//Bestimmen des minimalsten Retentionskoeffizienten...
	if(kMinC < kMinG)
		k_Min0 = kMinC;
	else
		k_Min0 = kMinG;

	if(kMinCr >= 0 && kMinCr < k_Min0)
		k_Min0 = kMinCr;
	

	if(kMinB < k_Min0) //Sicherheitsabfrage
	{
		Message_Dlg("BaseFlow-Lagtime ist am geringsten - SEHR wahrscheinlicher Fehler!");
	}


	//### entspricht kleinst möglicher Zeitschrittlänge!
	k_Min = k_Min0;
	//###


	if((k_Min*86400) <= 1)
	{
		Message_Dlg("FEHLER!! k_Min ist kleiner als eine Sekunde!!");
		WriteLog("FEHLER!! k_Min war kleiner als eine Sekunde!!");
		errC = 1;
		return (false);
	}

	
	//Setzen des Zeitschrittoperators m_pTStep : entspricht der Zeitschrittlänge dt   
	//WENN GESETZT - Automatisierte max. Zeitschrittlängen-Festlegung:
	if( Parameters("CalcT")->asBool() )
		m_pTStep = k_Min * 86400;		//Problem: k_Min nicht zwangläufig ein vielfaches von 60, daher m_pTStep ggf. nicht glatt durch 86400 teilbar
	// WENN NICHT GESETZT : 1) Verwendung der m_pTStep Eingabe - falls nicht zu groß: -> sonst m_pVTresh = k_Min
	else
	{
		if( m_pTStep >= k_Min * 86400)
			m_pTStep = k_Min * 86400;
	}
	//
  //######################################################






  //######################################################
	//Speicher, Arrays und Membervariablen initiieren
	//
	CreateKArray( NX, NY, 3);
	CreateNcArray( NX, NY, nMax+1);	// Array mit nMax+1 Speicherwerten (n Speichereinträge + Eintrag über Summe der Speicher)
	//CreatePArray( NX, NY, 1);		//zur Zeit: nicht gesetzt
	
	//-----------------------------------------------------
	//SpeicherArrays initieren
	//
	InitKArray();
	InitNcArray(NX, NY, nMax+1);	//Array mit nMax+1 Speicherwerten (n Speichereinträge + Eintrag über Summe der Speicher)
	//InitPArray(NX, NY, 1);		//zur Zeit: nicht gesetzt

	/**///%%%% Monitoring %%%%%%%%%%
	/**///Initiierung
	/**/WSystem = 0;				// Im System befindliches Wasser
	/**/WSystemInit = 0;			// Anfänglicher Speicherinhalt WSystem nach Speicherinitilisierung
	/**/WSystemIn = 0;				// Summe des Wassers was ins System gelangt (Drainage, RunOff)
	/**/WSystemOut = 0;				// Summe des Wassers welches das System verläßt (Systemabfluss)
	/**/WSystemDayIn = 0;			// Summe des Wassers was am jeweiligen Tag ins System gelangt (Drainage, RunOff)
	/**/WSystemDayOut = 0;			// Summe des Wassers welches am jeweiligenTag das System verläßt (Systemabfluss)
	/**/WSystemDayWithd = 0;		// Summe allen Wassers, dass dem WSystem pro Tag entnommen wird oder über Entnahmefaktor verloren geht
	/**/RivBas = 0;					// Im RivBasin befindliches Wasser
	/**/RivBasInit = 0;				// Anfänglicher Speicherinhalt RivBasin nach Speicherinitilisierung
	/**/RivBasIn = 0;				// Summe des Wassers was ins RiverBasin gelangt (Drainage, RunOff)
	/**/RivOut = 0;					// Summe des Wassers welches das RiverBasin über den Fluss verläßt (Flussabfluss)
	/**/RivBasDayIn = 0;			// Summe des Wassers was am jeweiligen Tag ins RiverBasin gelangt (Drainage, RunOff)
	/**/RivDayOut = 0;				// Summe des Wassers welches am je weiligenTag das RiverBasin über den Fluss verläßt (Flussabfluss)
	/**/RivMonthOut = 0;			// Summe des Wassers welches im jeweiligen Monat das RiverBasin über den Fluss verläßt (Flussabfluss)
	/**/RivBasSink = 0;				// Summe allen Wasser das über dem Rechenzeitraum im RivBas in Senken verschwindet (gelöscht wird)
	/**/manWithd = 0;				// Wasser das durch Rasterzellen Manipulation aus dem RivBasinSystem entfernt oder hinzugefügt wurde
	/**/WCCache = 0;				// Speicherabbild des Wassers in den Kaskaden Speichern
	/**/RivBasConMonth = 0;			// Summe der auftretenden Flächenentnahmen innerhalb des gesamten Flusseinzugsgebietes pro Monat
	/**/resRivBasConMonth = 0;		// Summe der tatsächlichen durchgeführten Flächenentnahmen innerhalb des gesamten Flusseinzugsgebietes pro Monat
	/**/remRivBasConMonth = 0;		// Summe der Entnahmen die dem RivBas nicht entnommen werden konnten (weil resFlow kleiner als m_pCon) pro Monat
	/**/RivBasDayWithd = 0;			// Summe allen Wassers, dass dem RivBasin pro Tag entnommen wird oder über Entnahmefaktor verloren geht
	/**/SumRDMonth = 0;				//Summe der Runoff und Drainage Werte des Einzugsgebietes eines Monats
	/**/SumRMonth = 0;				//NUR Summe der Runoff-Werte des Einzugsgebietes eines Monats
	/**/SumDMonth = 0;				//NUR Summe der Drainage-Werte des Einzugsgebietes eines Monats
	/**/m_pTestR1m = 0;				//TestRoutine1
	/**/m_pTestR1RDm = 0;			//TestRoutine1
	/**/m_pTestR1Cm = 0;			//TestRoutine1
	/**/m_pTestR1resCm = 0;			//TestRoutine1
	/**/m_pTestR1remCm = 0;			//TestRoutine1

	/**/SumRD_SubBasin = 0;			//Summe der Runoff und Drainage Werte eines SubBasin [TestRoutine 1]
	/**/SumCon_SubBasin = 0;		//Summe der beabsichtigten Entnahmen eines SubBasin [TestRoutine 1]
	/**/SumResCon_SubBasin = 0;		//Summe der tatsächlichen Entnahmen eines SubBasin [TestRoutine 1]
	/**/SumRemCon_SubBasin = 0;		//Summe der nicht berücksichtigten Entnahmen eines SubBasin [TestRoutine 1]
	/**///%%%%%%%%%%%%%%

	/**///MonitoringLogs initiieren (Legende)
	/**///Monitoringlog 1 - RivBasine
	/**/if(var2)
	/**/{
	/**/	file0.str("");
	/**/	file0 << "Tag RivBasDayIn RivDayOut RivBasIn RivOut RivBas DiffInOut RivBasSink RivBasDayWithd ";
	/**/	file = file0.str();
	/**/	TestLog1( file );
	/**/}
	/**///Monitoringlog 2 - WSystem
	/**/if(var3)
	/**/{
	/**/	file0.str("");
	/**/	file0 << "Tag WSystemDayIn WSystemDayOut WSystemIn WSystemOut WSystem DiffInOut WCCache WSystemDayWithd";
	/**/	file = file0.str();
	/**/	TestLog2( file );
	/**/}
	/**///Monitoringlog 3 - RivBasWUse
	/**/if(var4)
	/**/{
	/**/	file0.str("");
	/**/	file0 << "Jahr Monat RivBasRDMonth RivMonthOut RivBasWUseMonth RivBasResWUseMonth RivBasRemWUseMonth RivBasRMonth RivBasDMonth";
	/**/	file = file0.str();
	/**/	TestLog3( file );
	/**/}
	/**/
	/**/if(Parameters("Test1")->asInt() > 0 && m_pRivGrids->asDouble(Parameters("xt1")->asInt(),Parameters("yt1")->asInt()) != 0 )
	/**/{
	/**/	file0.str("");
	/**/	file0 << "Jahr Monat SubRivBasRDMonth SubRivBasOutMonth SubRivBasWUseMonth SubRivBasResWUseMonth SubRivBasRemWUseMonth";
	/**/	file = file0.str();
	/**/	TestLog4( file );
	/**/}
	/**///%%%%%%%%%%%%%%%%%%%%%%%%%%


	//-----------------------------------------------------
	//Temporäre Speicher anlegen
	m_pTempR = SG_Create_Grid(m_pDTM, SG_DATATYPE_Double);
	m_pTempC = SG_Create_Grid(m_pDTM, SG_DATATYPE_Double);
	m_pTempD = SG_Create_Grid(m_pDTM, SG_DATATYPE_Double);
	m_pTempK = SG_Create_Grid(m_pDTM, SG_DATATYPE_Double);	// Temporärer Speicher um je Zeitschritt dt_n den Channelzufluss je Gitterbox zu speichern und zu Anfang von dt_n+1 in den Kaskadenpeicher nCArray[x][y][0] zu schreiben
	m_pTemp = SG_Create_Grid(m_pDTM, SG_DATATYPE_Double);	// Temporärer Zwischenspeicher
	m_pSumCon = SG_Create_Grid(m_pDTM, SG_DATATYPE_Double);
	m_pSumRemCon = SG_Create_Grid(m_pDTM, SG_DATATYPE_Double);
	m_pSumResCon = SG_Create_Grid(m_pDTM, SG_DATATYPE_Double);
	//TestRoutine1
	if(Parameters("Test1")->asInt() == 2)
		m_pTestR1Share = SG_Create_Grid(m_pDTM, SG_DATATYPE_Double); //gesammtes Einzugsgebiet zu HG-Rasterzelle (x,y) der TestR1 Routine
	//-----------------------------------------------------

	m_pChannelFlow->Assign(0.0);							//erste Initiierung des ChannelFlow Grids
	m_pSumRunoffDrainage->Assign(0.0);						//erste Initiierung des m_pSumRunoffDrainage Grids
	m_pWConsOut->Assign(0.0);								//erste Initiierung des dynamischen m_pWConsOut Grids
	m_pTempK->Assign(0.0);									//erste Initiierung
	m_pSumCon->Assign(0.0);									//erste Initiierung
	m_pSumRemCon->Assign(0.0);								//erste Initiierung
	m_pSumResCon->Assign(0.0);								//erste Initiierung
	
	//TestRoutine1
	if(Parameters("Test1")->asInt() == 2)
	{
		m_pTestR1Share->Assign(-1.0);	//erste Initiierung
		TestR1Share( Parameters("xt1")->asInt(), Parameters("yt1")->asInt() ); //EinzugsgebietsRasterzellen BIS Errreichen der HG-Rasterzelle aus TestRoutine1 bestimmen
	}
	//------------

	m_pHFlowFac = 1.0;										//Initiierung: Weiterleitungsfaktor des Wasserflusses aus einer Gitzterbox
	m_pRFlowFac = 1.0;										//Initiierung: Weiterleitungsfaktor des Wasserflusses aus einer Fluss-Gitterbox
	m_pVRFlowFac = 1.0;										//Initiierung: Zuflussfaktor des vertikalen Zuflusses RunOff
	m_pVDFlowFac = 1.0;										//Initiierung: Zuflussfaktor der vertikalen Zuflsses Drainage
	m_pTestR1 = 0;											//Initiierung: SpeicherVariable TestRoutine 1
	//-----------------------------------------------------
	
	
	//Ggf. Input-Grid-Speicherwerte einlesen
	//
	if( Parameters("CacheUse")->asBool() && m_pRCacheIn && m_pDCacheIn && m_pCCacheIn && !m_pNCacheFolder.is_Empty())
	{
	//-----------------------------------------------------
	//Initialisierung der temporären Speicher mit voherigen Soeicherwerten

		m_pTempR->Assign(m_pRCacheIn); //Initiierung des temporären RunOff-Speichers mit vorhandenen Werten
		m_pTempD->Assign(m_pDCacheIn); //Initiierung des temporären Drainage-Speichers mit vorhandenen Werten
		m_pCCacheOut->Assign(m_pCCacheIn); //Initiierung des ChannelFlow-Grids mit vorhandenen Werten
		
		//RivBas / WSystem Monitoring: Inhalt des RivBas und Wsystem aus Speichern einlesen
		/**///%%%% Monitoring %%%%%%%%%%
		/**/
		/**/for(int x=0; x < NX; x++)
		/**/{
		/**/	for(int y=0; y < NY; y++)
		/**/	{
		/**/		WSystem = WSystem + m_pTempR->asDouble(x,x) + m_pTempD->asDouble(x,y) + m_pCCacheOut->asDouble(x,y);
		/**/
		/**/		if(m_pBasinShare->asDouble(x,y) >= 0 ) //wenn im RivBasin...
		/**/			RivBas = RivBas + m_pTempR->asDouble(x,x) + m_pTempD->asDouble(x,y) + m_pCCacheOut->asDouble(x,y);
		/**/	}
		/**/}
		/**/RivBasInit = RivBas;
		/**/WSystemInit = WSystem;
		//%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%


		ReadNCacheFile(false, "NcCacheIn.txt", NX, NY, 0, 0, nMax, ' ', 0, false); //nC-Speicherdatei in Array einlesen
		//-----------------------------------------------------
	}
	else
	{
		m_pTempR->Assign(0.0);
		m_pTempD->Assign(0.0);
		m_pCCacheOut->Assign(0.0); //erste Initiierung des ChannelCache Grids
	}

	
	//-----------------------------------------------------
	//Zeit Membervariablen initiieren [Funktioniert bisher nur für 1968-2009]
	//

	//Simulationsuhr auf 1. Januar Startjahr 00:00:00 gesetzt
	CTime timeS(m_sYear0, 1, 1, 0, 0, 0);

	dT = 0;		//daytimeCounter - Tageszeitzähler [s]	(0-86400)
	dDC = 1;	//dayDataCounter - Datentag-Zähler [d]	(1-365/366)
	dC = 1;		//dayCounter - Tageszähler [d]
	mYC = 1;	//MonthOfYear Monatszähler [m]			(1-12)
	dMC = 1;	//DayOfMonth Tageszähler [d]			(1-28/29/30/31)

	//UhrzeitTest-Platzhalter
	double Td = 0;
	double Tm = 0;
	double Ty = 0;
	double Th = 0;
	double Tmi = 0;
	//-----------------------
	
	
	//Schaltjahr-Bestimmung - statisch für Zeitraum 1980 - 2009
	if(m_sYear == 1968 || m_sYear == 1972 || m_sYear == 1976 || m_sYear == 1980 || m_sYear == 1984 || m_sYear == 1988 || m_sYear == 1992 || m_sYear == 1996 || m_sYear == 2000 || m_sYear == 2004 || m_sYear ==  2008 )
		m_pDDays = 366;
	else
		m_pDDays = 365;

	numTS = int(86400/m_pTStep);				//Anzahl der Zeitschtritte pro Tag
	//-----------------------------------------------------
	
	//Auslesepunkte - Tagesspeicher
	//STATISCH
	double dV1 = 0;
	double dV2 = 0;	
	double dV3 = 0;
	//
  //######################################################


  //######################################################
	//Ggf. Prozentuale Wasserentnahme initiieren
	//-----------------------------------------------------
	//Falls der Inputparameter Wasserentnahmefaktor != Null, berechnen des effektiven Wasserdurchflussfaktors m_pFlowFac
	//
	if(var1 && m_pConsFacAll != 0)
	{
		/*double pow1 = 1.0 - m_pConsFacAll;
		double pow2 = 1.0 / numTS;
	
		if(m_pConsFac!=0 && m_pConsFac <= 1) 
			m_pFlowFac = pow( pow1, pow2);		//ZinsesZins-Rechnung
		*/
		m_pHFlowFac = 1.0 - m_pConsFacAll;
	}

	if(var1 && m_pConsFacRiv != 0)
		m_pRFlowFac = 1.0 - m_pConsFacRiv;

	if(var1 && m_pRedFacR != 0)
		m_pVRFlowFac = 1.0 - m_pRedFacR;

	if(var1 && m_pRedFacD != 0)
		m_pVDFlowFac = 1.0 - m_pRedFacD;
	//
  //######################################################

	

//######################################################
	// Hauptfunktionsschleife
	//######################################################
	//
	
	//LS-Data Grids initiieren und einlesen für Tag 1
	//-----------------------------------------------------
	CSG_String m_pLSData26File;
	CSG_String m_pLSData27File;
	m_pLSData26File.Printf(SG_T("%s/%d/srunoff_%d_%d.sgrd"), m_pLSDataFolder.w_str(), m_sYear, m_sYear, dDC);
	m_pLSData27File.Printf(SG_T("%s/%d/drainage_%d_%d.sgrd"), m_pLSDataFolder.w_str(), m_sYear, m_sYear, dDC);

	CSG_Grid pRunOff(m_pLSData26File);
	CSG_Grid pDrainage(m_pLSData27File);
	//-----------------------------------------------------
	
	//Dynamische WCons-Data Grids initiieren und einlesen für Tag 1
	//-----------------------------------------------------
	if(Parameters("WCons")->asInt() > 0)
	{
		m_pWConsFile.Printf(SG_T("%s/TotalWUse_%d_%d.sgrd"), m_pWConDataFolder.w_str(), m_sYear, mYC);
		pWConsData.Create(m_pWConsFile);
		//Entnahmen berechnen für Monat 1
		if( !WConsRiv( Parameters("WCons")->asInt() ))
			Message_Dlg("Fehler beim Berechnen der dynamischen Entnahmen!");
	}
	//-----------------------------------------------------

	//######################################################
	//
	while(dC <= m_pDays ) // ...so lange Tageszahl kleiner gleich der festgelegten Simulationstage.
	{				
		m_pTempC->Assign(0.0);	//temporärer Speicher zum Berechnen von m_pCCacheOut (Gültigkeit für einen Zeitschritt)
		m_pTemp->Assign(0.0);	//temporärer Speicher zum Speichern des Zuflusses pro Gitterbox (Gültigkeit für einen Zeitschritt)
		
		//######################################################
		for(int x=0; x < NX; x++)
		{
			for(int y=0; y < NY; y++)
			{
				
				if(Parameters("onlyRB")->asBool() && m_pBasinShare->asDouble(x,y) < 0) //Falls nur im RiverBasin gerechnet werden soll und Koordinaten (x,y) nicht in diesem liegt -> Koordinaten überspringen... 
				{}
				else
				{
					if(m_pDTM->is_NoData(x,y)) 
					{
						m_pCCacheOut->Set_NoData(x,y);
					}
					else
					{
												
						/**///TEST----
						/**/double test1 = pRunOff.asDouble(x,y);
						/**///--------

						//Tageswerte für RunOff und Drainage aus Array einlesen und auf Fläche und Zeitschritt umrechnen
						//Entsprechende Reduktionen und Offsets berücksichtigen bzw. ausgleichen.
						//-----------------------------------------------------
						m_pRun = ( ( pRunOff.asDouble(x,y) * m_pVRFlowFac * pow(m_pDTM->Get_Cellsize(), 2) ) + m_pOffsetR ) * m_pTStep; // Vertikaler Runoff pro Zellenfläche (x,y): [m^3] pro Zeitschrittlänge (Input = m/s) multipliziert mit dem Zuflussfaktor m_pVFlowFac (Möglichkeit der prozentuelle Reduzierung des Runoff-Zuflusses) + OFFSET
						
							//Reduzierungsbetrag RunOff
							double redSumRunOff = pRunOff.asDouble(x,y) * m_pRedFacR;

						if(Parameters("autoFacD")->asBool())
							m_pDrain =  ( ( pDrainage.asDouble(x,y) +  redSumRunOff ) * pow(m_pDTM->Get_Cellsize(), 2) ) * m_pTStep; // Vertikale Drainage pro Zellenfläche (x,y): [m^3] pro Zeitschrittlänge (Input = m/s) + Reduzierungsbetrag des RunOffs
						else
							m_pDrain = ( ( pDrainage.asDouble(x,y) * m_pVDFlowFac * pow(m_pDTM->Get_Cellsize(), 2) ) + m_pOffsetD ) * m_pTStep; // Vertikale Drainage pro Zellenfläche (x,y): [m^3] pro Zeitschrittlänge (Input = m/s) multipliziert mit dem Zuflussfaktor m_pVFlowFac (Möglichkeit der prozentuelle Reduzierung des Drainage-Zuflusses) + OFFSET
						//-----------------------------------------------------
						
						//HAUPTFUNKTION->
						//######################
						Calc_MainFlow(x,y);
						//######################
						//HAUPTFUNKTION->

					
						/**///%%%% Monitoring %%%%%%%%%%
						/**///WCCache Werte Schreiben
						/**/WCCache = WCCache + nCArray[x][y][nMax];
						//%%%%%%%%%%%%%%%%%%%%%%%%%%
					}

				}
			}
		}
			
		m_pCCacheOut->Assign(m_pTempC); //Übernahme des Flussinhaltes (Gerinne- und Hauptgerinnnerasterzellen) nach einem Zeitschritt dt
		m_pTempK->Assign(m_pTemp); //Speichern des Gitterboxzuflusses (== Zufluss in Speicherkaskade N) dieses Zeitschritts zur Verwendung im nä Zeitschritt
		
		DataObject_Update(m_pCCacheOut);
		DataObject_Update(m_pChannelFlow);

		Process_Set_Text(CSG_String::Format(SG_T("dt = %d  Dt = %g  Vmax = %g || Tag: %d"), m_pTStep, dT+m_pTStep, vMax, dC));
		//######################################################
		
		
		//-----------------------------------------------------
		//Zeitoperation -> nächster Zeitschritt
		//
		dT = dT + m_pTStep; //Tageszeitzähler
		timeS.operator+=(m_pTStep); //SimulationsUhr
		//-----------------------------------------------------
		
		
		//Test---------------------
		Td = timeS.GetDay();
		
		if(Th != timeS.GetHour())
			Th = timeS.GetHour();
	
		if(Tmi != timeS.GetMinute())
			Tmi = timeS.GetMinute();
		
		if(Tm != timeS.GetMonth())
			Tm = timeS.GetMonth();

		if(Ty != timeS.GetYear())
			Ty = timeS.GetYear();

		if(timeS.GetDay() != dMC)
		//-------------------------
		
			
		//-----------------------------------------------------
		//TAGESWECHSEL
		
		////if(dT > 86400)
		{
			/**///%%%% Monitoring %%%%%%%%%%
			/**///WSystem Werte Schreiben
			/**/if(var3)
			/**/{	
			/**/	file0;
			/**/	file0.str("");
			/**/	file0 << dC << " " << WSystemDayIn << " " << WSystemDayOut << " " << WSystemIn << " " << WSystemOut << " " << WSystem << " " << (WSystemIn-WSystemOut) << " " << WCCache << " " << WSystemDayWithd;
			/**/	file = file0.str();
			/**/	TestLog2( file );
			/**/}
			/**/
			/**///RivBasin Werte schreiben
			/**/if(var2)
			/**/{
			/**/	file0.str("");
			/**/	file0 << dC << " " << RivBasDayIn << " " << RivDayOut << " " << RivBasIn << " " << RivOut << " " << RivBas << " " << (RivBasIn - RivOut) << " " << RivBasSink << " " << RivBasDayWithd;
			/**/	file = file0.str();
			/**/
			/**/	TestLog1( file );
			/**/}
			/**/
			/**/WSystemDayOut = 0;	//Auf Null setzen um nächsten Tageswert erfaassen zu können
			/**/WSystemDayIn = 0;	//Auf Null setzen um nächsten Tageswert erfaassen zu können
			/**/WSystemDayWithd = 0;//Auf Null setzen um nächsten Tageswert erfaassen zu können
			/**/RivBasDayIn = 0;	//Auf Null setzen um nächsten Tageswert erfaassen zu können
			/**/RivDayOut = 0;		//Auf Null setzen um nächsten Tageswert erfaassen zu können
			/**/RivBasDayWithd = 0;	//Auf Null setzen um nächsten Tageswert erfaassen zu können
			//%%%%%%%%%%%%%%%%%%%%%%%%%%
			

			//-----------------------------------------------------
			//AUSLESEN
			//
		
			//Fixe Messstationen (Auslesekoordinaten) - Tagesmittel in [m3/s] berechnen, Werte speichern
			//Schreiben in Textdatei
			//
			/*ELBE1	dV1 = m_pChannelFlow->asDouble(155,698) / 86400; // [m3/s]*/
			/*ELBE2	dV1 = m_pChannelFlow->asDouble(145,578) / 86400; // [m3/s]*/
			/*ELBE2_5	dV1 = m_pChannelFlow->asDouble(30,115) / 86400; // [m3/s]*/
						dV1 = m_pChannelFlow->asDouble(Parameters("EvP1x")->asInt(),Parameters("EvP1y")->asInt()) / 86400;
						WriteOutput(m_pEvP1s.b_str(), dDC, dV1, -1, -1);
						dV1 =0;
			/*Nile	dV1 = m_pChannelFlow->asDouble(222,563) / 86400; // [m3/s]
						WriteOutput("Dongola.txt",dDC, dV1, -1, -1);
						dV1 =0;*/

			/*ELBE1	dV2 = m_pChannelFlow->asDouble(278,547) / 86400; // [m3/s]*/
			/*ELBE2	dV2 = m_pChannelFlow->asDouble(269,428) / 86400; // [m3/s]*/
			/*ELBE2_5	dV2 = m_pChannelFlow->asDouble(54,85) / 86400; // [m3/s]*/
						dV2 = m_pChannelFlow->asDouble(Parameters("EvP2x")->asInt(),Parameters("EvP2y")->asInt()) / 86400;
						WriteOutput(m_pEvP2s.b_str(), dDC, dV2, -1, -1);
						dV2 =0;
			/*Nile	dV2 = m_pChannelFlow->asDouble(280,487) / 86400; // [m3/s]
						WriteOutput("Khartoum.txt",dDC, dV2, -1, -1);
						dV2 =0;*/

			/*ELBE1	dV3 = m_pChannelFlow->asDouble(399,434) / 86400; // [m3/s]*/
			/*ELBE2	dV3 = m_pChannelFlow->asDouble(388,327) / 86400; // [m3/s]*/
			/*ELBE2_5	dV3 = m_pChannelFlow->asDouble(78,65) / 86400; // [m3/s]*/
						dV3 = m_pChannelFlow->asDouble(Parameters("EvP3x")->asInt(),Parameters("EvP3y")->asInt()) / 86400;
						WriteOutput(m_pEvP3s.b_str(), dDC, dV3, -1, -1);
						dV3 =0;
			/*Nile	dV3 = m_pChannelFlow->asDouble(272,342) / 86400; // [m3/s]
						WriteOutput("Malakal.txt",dDC, dV3, -1, -1);
						dV3 =0;*/
			//-----------------------------------------------------
		
		
					
					//-----------------------------------------------------
					//Datei der FlussWasserBilanz pro Kilometer des letzten Simulationstages schreiben
					if(m_pRivBalanceGrid)
					{
						WriteRivBalance(m_sYear, dC, -1, -1);

						int rx, ry;

						for(int l=0; l < m_pDTM->Get_NCells() && Set_Progress_NCells(l); l++) //über alle Zellen des m_pDEM-Grids
						{
							m_pDTM->Get_Sorted(l, rx, ry); //sortieren der Zellen von höchster (l=0) nach niedrigster...

							if( m_pRivBalanceGrid->asDouble(rx,ry) != 0)
								{
									WriteRivBalance(rx, ry, m_pRivBalanceGrid->asDouble(rx,ry), m_pChannelFlow->asDouble(rx,ry) / 86400 );
								}
						}

					}
					//-----------------------------------------------------
					
			dMC++;
			dDC++;
			dC++;
			dT = 0;
			
			WCCache = 0; //WCCACHE immer NCache-SpeicherAbbild am Ende jedes Simulationstages - daher nach jedem Tageswechsel wieder auf Null setzten
			
			if(m_pWConsD > 0) //Tageszähler "Vorlauf vor Wasserentnahme" minus einen Tag  
				m_pWConsD--;
			
			//-----------------------------------------------------
			//MONATSWECHSEL
			//
			if(timeS.GetMonth() != mYC)
			{
				/**///%%%% Monitoring %%%%%%%%%%
				/**///monthRivBasCon Werte schreiben
				/**/if(var4)
				/**/{
				/**/	file0.str("");
				/**/	file0 << m_sYear << " " << mYC << " " <<  SumRDMonth << " " << RivMonthOut << " " << RivBasConMonth << " " << resRivBasConMonth << " " << remRivBasConMonth << " " << SumRMonth << " " << SumDMonth;
				/**/	file = file0.str();
				/**/
				/**/	TestLog3( file );
				/**/}
				/**/
				/**/RivBasConMonth = 0;		//Auf Null setzen um nächsten Monatswert erfaassen zu können
				/**/RivMonthOut = 0;		//Auf Null setzen um nächsten Monatswert erfaassen zu können
				/**/resRivBasConMonth = 0;	//Auf Null setzen um nächsten Monatswert erfaassen zu können
				/**/remRivBasConMonth = 0;	//Auf Null setzen um nächsten Monatswert erfaassen zu können
				/**/SumRDMonth = 0;			//Auf Null setzen um nächsten Monatswert erfaassen zu können
				/**/SumRMonth = 0;			//Auf Null setzen um nächsten Monatswert erfaassen zu können
				/**/SumDMonth = 0;			//Auf Null setzen um nächsten Monatswert erfaassen zu können
				//%%%%%%%%%%%%%%%%%%%%%%%%%%
				
				if(Parameters("Test1")->asInt() > 0 && m_pRivGrids->asDouble(Parameters("xt1")->asInt(),Parameters("yt1")->asInt()) != 0 )
				{
				/**/SubBasinId(Parameters("xt1")->asInt(), Parameters("yt1")->asInt());
				/**/
				/**/	file0.str("");
				/**/	file0 << m_sYear << " " << mYC << " " << m_pTestR1RDm << " " << m_pTestR1m << " " << m_pTestR1Cm << " " << m_pTestR1resCm << " " << m_pTestR1remCm;
				/**/	file = file0.str();
				/**/	TestLog4( file );
				/**/
				/**/	m_pTestR1m = 0;
				/**/	m_pTestR1RDm = 0;
				/**/	m_pTestR1Cm = 0;
				/**/	m_pTestR1resCm = 0;
				/**/	m_pTestR1remCm = 0;
				}
				
								
				if(mYC >= 12)
					mYC = 1;
				else
					mYC++;
				
				dMC = 1;		//auf ersten Tag des Monats setzen
				
				//Dynamische WCons-Data Grids für neuen Monat berechnen
				//-----------------------------------------------------
				if(Parameters("WCons")->asInt() > 0)
				{
					int Year;
					if(m_sYear <= 2000)			//Sicherheitsabfrage für WaterGapDaten, die nur bis 2000 vorliegen - falls Jahr größer als 2000, werden weiterhin 2000er Daten verwendet
						Year = m_sYear;
					else
						Year = 2000;

					m_pWConsFile.Printf(SG_T("%s/TotalWUse_%d_%d.sgrd"), m_pWConDataFolder.w_str(), Year, mYC);
					pWConsData.Create(m_pWConsFile);

					if( !WConsRiv( Parameters("WCons")->asInt() ))
						Message_Dlg("Fehler beim Berechnen der dynamischen Entnahmen!");
				}
				//-----------------------------------------------------

			}
			//-----------------------------------------------------

			//-----------------------------------------------------
			//JAHRESWECHSEL
			//
			if(timeS.GetYear() > m_sYear)
			//if(dDC > m_pDDays)
			{
				dDC = 1;
				m_sYear = m_sYear + 1;
				mYC = 1;

				if(m_sYear == 1980 || m_sYear == 1984 || m_sYear == 1988 || m_sYear == 1992 || m_sYear == 1996 || m_sYear == 2000 || m_sYear == 2004 || m_sYear ==  2008 )
					m_pDDays = 366;
				else
					m_pDDays = 365;
			}
		
			if(m_sYear > 2009 || dDC > 366) //STATISCHE SICHERHEITSANGABEN
				break;

			if(dC <= m_pDays)
			{

				//Aktuelle LS-Data Grids einlesen
				//-----------------------------------------------------
				m_pLSData26File.Printf(SG_T("%s/%d/srunoff_%d_%d.sgrd"), m_pLSDataFolder.w_str(), m_sYear, m_sYear, dDC);
				m_pLSData27File.Printf(SG_T("%s/%d/drainage_%d_%d.sgrd"), m_pLSDataFolder.w_str(), m_sYear, m_sYear, dDC);

				pRunOff.Create(m_pLSData26File);
				pDrainage.Create(m_pLSData27File);
				//-----------------------------------------------------
						
				m_pChannelFlow->Assign(0.0);
				m_pTest = 0;
			}
			//-----------------------------------------------------
		}
		//-----------------------------------------------------

		

	}

	//
	//######################################################
	//
//######################################################


	//-----------------------------------------------------
	// Testroutine 1 durchführen
	if(Parameters("Test1")->asInt() > 0 && m_pRivGrids->asDouble(Parameters("xt1")->asInt(),Parameters("yt1")->asInt()) != 0 )
		{
			SubBasinId(Parameters("xt1")->asInt(), Parameters("yt1")->asInt());
		}
	//-----------------------------------------------------



	//-----------------------------------------------------
	// Speichern in den Ausgabedateien
	
	m_pRCacheOut->Assign(m_pTempR); //Übernahme des Gitterboxflusses nach m_pDays Simulationstagen
	m_pDCacheOut->Assign(m_pTempD); //Übernahme des Gitterboxflusses nach m_pDays Simulationstagen

	if(Parameters("wNC")->asBool())
		SaveNcCache(nMax);
	
	if(Parameters("wP")->asBool())
		SaveParameters();

	
	//Speichern der AusgabeGrids in Unterordner "grids"
	CSG_String SaveAsFile;
	CSG_String SPath = m_pSPath2.c_str();

	SaveAsFile.Printf(SG_T("%s/CFCache.sgrd"), SPath.w_str());
	m_pCCacheOut->Save(SaveAsFile);									//GridSave CCCache

	SaveAsFile.Printf(SG_T("%s/OFCache.sgrd"), SPath.w_str());
	m_pRCacheOut->Save(SaveAsFile);									//GridSave RCache

	SaveAsFile.Printf(SG_T("%s/BFCache.sgrd"), SPath.w_str());
	m_pDCacheOut->Save(SaveAsFile);									//GridSave DCache

	SaveAsFile.Printf(SG_T("%s/SumRunoffDrainage.sgrd"), SPath.w_str());
	m_pSumRunoffDrainage->Save(SaveAsFile);							//GridSave SumRunoffDrainage

	SaveAsFile.Printf(SG_T("%s/ChannelFlow.sgrd"), SPath.w_str());
	m_pChannelFlow->Save(SaveAsFile);								//GridSave ChannelFlow
	//-----------------------------------------------------

	if(Parameters("Test1")->asInt() == 2)
	{
		SaveAsFile.Printf(SG_T("%s/m_pTestR1Share.sgrd"), SPath.w_str());
		m_pTestR1Share->Save(SaveAsFile);								//GridSave ChannelFlow
		delete(m_pTestR1Share);
	}


	//-----------------------------------------------------
	//	Löschen der temporären Speicher
	//
	delete(m_pTempR);
	delete(m_pTempC);
	delete(m_pTempD);
	delete(m_pTempK);
	delete(m_pTemp);
	delete(m_pSumCon);
	delete(m_pSumRemCon);
	delete(m_pSumResCon);

	CLandFlow::DeleteKArray();
	//CLandFlow::DeleteLSMArray();	//ersetzt durch direktes Laden der LSSchmea-Grids
	CLandFlow::DeleteNcArray();
	//CLandFlow::DeletePArray();	// zur Zeit : nicht verwendet
	//-----------------------------------------------------
	
	return( true );
}


//#########################################################
//---------------------------------------------------------
//#########################################################
///////////////////////////////////////////////////////////
//														 //
//	Funktionen											 //
//														 //
///////////////////////////////////////////////////////////


//######################################################
//
void CLandFlow::Calc_MainFlow(int x, int y)
{
	int i = -1;
	int ix, iy;
	double c, r, d;
	stCons = 0;
	varCons = 0;
		
	r = m_pTempR->asDouble(x,y) + m_pRun;	// Einlesen des Inhalts des Runoff-Speichers + Runoff m_pRun des Zeitschritts
	d = m_pTempD->asDouble(x,y) + m_pDrain; // Einlesen des Inhalts des Drainage-Speichers + Drainage m_pDrain des Zeitschritts
	c = m_pTempK->asDouble(x,y);			// Einlesens des Inhalts des Gitterboxzuflusses des letzten Zeitschritts
	
	/**///%%%% Monitoring %%%%%%%%%%
	/**/WSystemIn = WSystemIn + m_pRun + m_pDrain;
	/**/WSystem = WSystem + m_pRun + m_pDrain;
	/**/
	/**/WSystemDayIn = WSystemDayIn + m_pRun + m_pDrain;
	/**/
	/**/m_pSumRunoffDrainage->Add_Value(x, y, (m_pRun + m_pDrain)); //Grid über die Summe aller abflussrelevanten, vertikalen Zuflüsse je Gitterzelle (x,y) des gesamten Berechnugszeitraums
	/**/
	/**/if(m_pBasinShare->asDouble(x,y) >= 0 ) //Abfrage ob (x,y) Koordinate im RiverBasin liegt
	/**/{
	/**/	RivBasIn = RivBasIn + m_pRun + m_pDrain;
	/**/	RivBas = RivBas + m_pRun + m_pDrain;		
	/**/
	/**/	RivBasDayIn = RivBasDayIn + m_pRun + m_pDrain; //Summe der RunOff und DrainagWerte innerhalb des Flusseinzugsgebietes über einen Monat
	/**/	SumRDMonth = SumRDMonth + m_pRun + m_pDrain;
	/**/	SumRMonth = SumRMonth + m_pRun;
	/**/	SumDMonth = SumDMonth + m_pDrain;
	/**/}
	/**///%%%%%%%%%%%%%%%%%%%%%%%%%%
			
		
	
/**///%%%%%%%%%%%%%%%%%%%%%%%%%%
	//---------------------------------------------------------
	//Berechnung der Abflüsse
	m_pGFlow = Calc_GFlow(x, y, r); // Berechnung Oberflächenabflusses
	m_pBFlow = Calc_BFlow(x,y,d); // Berechnung Grundwasserabfluss
		
	if( Parameters("RivG")->asInt() > 0 && m_pRivGrids->asDouble(x,y) != 0)
		m_pCFlow = Calc_CFlow(x, y, c, nCr); // Berechnung Hauptgerinnerasterzellen-Abfluss-Speicherkaskade
	else
		m_pCFlow = Calc_CFlow(x, y, c, nC); // Berechnung Gerinneabfluss-Speicherkaskade
	//---------------------------------------------------------
/**///%%%%%%%%%%%%%%%%%%%%%%%%%%

		/**///TempSpeicherwert
		/**/double tempN = m_pCFlow;
		/**/
		/**///TempSpeicherwerte
	
		//%%%%%%%%%%%%%%%%%%%%%%%%%% Punktuelle Abflussmanipulation - Anfang %%%%%%%%%%%%%%%%%%%%%%%%%% 
		//Rasterzelle 1
		if( x == Parameters("RM1x")->asInt() && y == Parameters("RM1y")->asInt() && Parameters("vRM1")->asInt() != 0)
			if(Parameters("vRM1")->asInt() == 1)
				m_pCFlow = ( m_pCFlow * Parameters("RM1q")->asDouble() ) + (Parameters("RM1a")->asDouble() * m_pTStep); //Abfluss des HD-Modells verändern
			else
				m_pCFlow = ( m_pCCacheOut->asDouble(x,y) * Parameters("RM1q")->asDouble() ) + (Parameters("RM1a")->asDouble() * m_pTStep); //Abfluss aus Speicher manuell vorgeben
		
		//Rasterzelle 2
		if( x == Parameters("RM2x")->asInt() && y == Parameters("RM2y")->asInt() && Parameters("vRM2")->asInt() != 0)
			if(Parameters("vRM2")->asInt() == 1)
				m_pCFlow = ( m_pCFlow * Parameters("RM2q")->asDouble() ) + (Parameters("RM2a")->asDouble() * m_pTStep); //Abfluss des HD-Modells verändern
			else
				m_pCFlow = ( m_pCCacheOut->asDouble(x,y) * Parameters("RM2q")->asDouble() ) + (Parameters("RM2a")->asDouble() * m_pTStep); //Abfluss aus Speicher manuell vorgeben
		
		if(m_pCFlow < 0)
		{
			Message_Dlg("Negativer Abfluss errechnet -> Punktuelle Rasterzellenmanipulation in diesem Schritt außer Kraft gesetzt");
			m_pCFlow = tempN;
		}
		else		
			//Monitoring der Manipulierten Wasserflussvolumen (pos. Wert == Wasser hinzugefügt)
			manWithd = manWithd + m_pCFlow - tempN;
		
		//%%%%%%%%%%%%%%%%%%%%%%%%%% Punktuelle Abflussmanipulation - Ende %%%%%%%%%%%%%%%%%%%%%%%%%% 

	//---------------------------------------------------------
	//Abflüsse aus den Speichern der Rasterzelle (x,y)
	m_pTempR->Set_Value( x, y, (r - m_pGFlow) ); // Inhalt Runoff-Speicher der in Zelle (x,y) verbleibt 
	m_pTempD->Set_Value( x, y, (d - m_pBFlow ) ); // Inhalt Drainage-Speicher der in Zelle (x,y) verbleibt
	
	m_pTempC->Add_Value( x, y, (m_pCCacheOut->asDouble(x,y) - m_pCFlow) ); //Add_ist_richtig!! Alter Inhalt des ChannelGitterbox-Speichers minus Abfluss dieses Zeitschritts, gleich: verbleibender Inhalt in der Zelle (x,y) - (Grid: m_pCCacheOut ==  Summe des Volumens aller n Kaskadenspeicher) (Achtung! aus anderen Zellen kann aber immer auch nocht etwas in m_pTempC zufließen)
	//---------------------------------------------------------


	//%%%%%%%%%%%%%%%%%%%%%%%%%% EntnahmenCode - Anfang %%%%%%%%%%%%%%%%%%%%%%%%%% 
		//...aus dem Gesamtabfluss (m_pGFlow + m_pBFlow + m_pCFlow) der rasterzelle (x,y)
		
		//---------------------------------------------------------
		//Wasserentnahme - flächenbasiert über das Flussgrid
		//
		//Wenn bei gesetzter Wasserentnahme kein WCons-Grid vorhanden -> Fehlermeldung
		if(Parameters("WCons")->asInt() > 0 && !pWConsData.is_Valid())
		{
			WriteLog("Fehler! Kein WConsDaten als Input!");
			errC = 1;
		}
	
		m_pCon = 0;
	
		//Entnahme statisch (WConsIn) und/oder dynamisch nach Auswahl 1 oder 2 (WConsData)
		if( m_pWConsIn && m_pWConsD == 0 )
			m_pCon = (m_pWConsIn->asDouble(x,y) / 1000 ) * pow(m_pDTM->Get_Cellsize(), 2) * m_pTStep;	//Statische Wasserentnahme Eingangsdaten in m3/s -> [0,001m^3 m^-2 s^-1 == 1 mm s^-1] * (Zellgröße)^2 * Zeitschritt(s)
	
		if( Parameters("WCons")->asInt() != 0 && pWConsData.is_Valid() && m_pWConsOut->is_Valid() && m_pWConsD == 0 )
			if(Parameters("WConUnit")->asInt() == 0)
				m_pCon = m_pCon + (m_pWConsOut->asDouble(x,y) / 1000 ) * pow(m_pDTM->Get_Cellsize(), 2) * m_pTStep;	//Wasserentnahme Eingangsdaten in m3/s -> [0,001m^3 m^-2 s^-1 == 1 mm s^-1] * (Zellgröße)^2 * Zeitschritt(s)
			else
				m_pCon = m_pCon + (m_pWConsOut->asDouble(x,y) / 1000 / 86400 / 30) * pow(m_pDTM->Get_Cellsize(), 2) * m_pTStep;	//Wasserentnahme Eingangsdaten in m3/Monat, deshalb teilen durch (86400*monthDays) -> [0,001m^3 m^-2 s^-1 == 1 mm s^-1] * (Zellgröße)^2 * Zeitschritt(s)
		//---------------------------------------------------------


		/**///%%%% Monitoring %%%%%%%%%%
		/**/if(m_pBasinShare->asDouble(x,y) >= 0 ) //Nur Monitoren wenn (x,y) Koordinate im RiverBasin liegt
		/**/	RivBasConMonth = RivBasConMonth + m_pCon;
		/**/m_pSumCon->Add_Value(x, y, m_pCon);
		/**///%%%%%%%%%%%%%%%%%%%%%%%%%%



		//prozentualle Abflussreduktion vornehmen - abhängig davon, ob Gitterbox oder Flussgitterbox.
		if( Parameters("RivG")->asInt() > 0 && m_pRivGrids->asDouble(x,y) != 0)
			m_pFlowFac = m_pRFlowFac; //Weiterleitungsfaktor-Fluss-Gitterbox setzten
		else
			m_pFlowFac = m_pHFlowFac; //Weiterleitungsfaktor-Gitterbox setzen
		//---------------------------------------------------------
		
	
	i = m_pDTM->Get_Gradient_NeighborDir(x, y); // Falls kein niedrigerer Nachbar -> Wert=-1
	
	//Falls Abflüsse stattfinden können, weil es eine niedrigere Nachbarzelle gibt bzw. ein Gradient größer Null vorliegt...	
	if( i >= 0 || m_pGrad->asDouble(x,y) > 0) 
	{
		ix = Get_xTo(i, x);
		iy = Get_yTo(i, y);		

	//---------------------------------------------------------
	//Prüfen ob Zellenabfluss [(m_pGFlow + m_pBFlow + m_pCFlow)*m_pFlowFac] aus Gitterzelle (x,y) kleiner etwaiger Entnahme m_pCon
	//...wenn der Fall: dann Entnahmen auf Größe des Zellabflusses setzen -> es resultiert Nullabfluss in diesem Zeitschritt
		double resFlow = (m_pGFlow + m_pBFlow + m_pCFlow)*m_pFlowFac;
		
		//Fall Schwellenwert... -> m_pCon Veränderung
		if(m_pWConThres != 0 && Parameters("RivG")->asInt() > 0 && m_pRivGrids->asDouble(x,y) != 0)
		{
			if(resFlow >= ( m_pWConThres * m_pTStep))
			{
				if( (resFlow - ( m_pWConThres * m_pTStep)) < m_pCon) //wenn resultierender Abfluss nach Abzug des Schwellewertes kleiner als m_pCon, dann Entnahme nur in Höhe von resFlow - m_pWConThres
				{	
					if(Parameters("eP")->asBool() && errC2 == 0)
					{
						WriteLog("Warnung! Es wurde in mindestens einem Fall bei Berücksichtigung des Schwellenwertes und einer Entnaheme, ein negativer Abfluss ermittelt!!");
						errC2 = -1;
					}
					
					/**///%%%% Monitoring %%%%%%%%%%
					/**/if(m_pBasinShare->asDouble(x,y) >= 0 )												//Nur Monitoren wenn (x,y) Koordinate im RiverBasin liegt
					/**/	remRivBasConMonth = remRivBasConMonth + (m_pCon - ( resFlow - ( m_pWConThres * m_pTStep)) );		//Summe der Entnahmen die dem RivBasin nicht entnommen werden konnten (weil resFlow kleiner als m_pCon) pro Monat
					/**/m_pSumRemCon->Add_Value(x, y, (m_pCon - ( resFlow - ( m_pWConThres * m_pTStep))) );
					//%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%% 	

					m_pCon = resFlow - ( m_pWConThres * m_pTStep);
				}
				//else: m_pCon bleibt unverändert...
			}
			else //Falls Schwellenwert schon höher als berechneter Abfluss, keine Entnahme mehr...
			{
				if(Parameters("eP")->asBool() && errC3 == 0)
				{
					WriteLog("Warnung! In mindestens einem Fall war der resultierdene Abfluss kleiner als der eingestellte Schwellenwert!!");
					errC3 = -1;
				}
				
				/**///%%%% Monitoring %%%%%%%%%%
				/**/if(m_pBasinShare->asDouble(x,y) >= 0 )									//Nur Monitoren wenn (x,y) Koordinate im RiverBasin liegt
				/**/	remRivBasConMonth = remRivBasConMonth + m_pCon;						//Summe der Entnahmen die dem RivBasin nicht entnommen werden konnten (weil resFlow kleiner als m_pCon) pro Monat
				/**/m_pSumRemCon->Add_Value(x, y, m_pCon);
				//%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%% 	
				
				m_pCon = 0;
			}
		}
		else // falls kein Schwellenwert und/oder keine Hauptgerinnerasterzelle
			if( resFlow < m_pCon)
			{
				if(Parameters("eP")->asBool() && errC4 == 0)
				{
					WriteLog("Warnung! Es wurde in mindestens einem Fall ein negativer Abfluss ermittelt!!");
					errC4 = -1;
				}
			
				/**///%%%% Monitoring %%%%%%%%%%
				/**/if(m_pBasinShare->asDouble(x,y) >= 0 )												//Nur Monitoren wenn (x,y) Koordinate im RiverBasin liegt
				/**/	remRivBasConMonth = remRivBasConMonth + (m_pCon - resFlow);						//Summe der Entnahmen die dem RivBasin nicht entnommen werden konnten (weil resFlow kleiner als m_pCon) pro Monat
				/**/m_pSumRemCon->Add_Value(x, y, (m_pCon - resFlow));
				//%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%% 			
			
				m_pCon = resFlow;
			}
		//---------------------------------------------------------

		/**///%%%% Monitoring %%%%%%%%%%
		/**/WSystemDayWithd = WSystemDayWithd + (m_pGFlow + m_pBFlow + m_pCFlow)*(1-m_pFlowFac);		//faktorielle Tageswasserverluste aus WSystem
		/**/WSystem = WSystem - (m_pGFlow + m_pBFlow + m_pCFlow)*(1-m_pFlowFac);						//faktorielle Wasserverluste aus WSystem pro Zeitschritt
		/**/if(m_pBasinShare->asDouble(x,y) >= 0 ) //Nur Monitoren wenn (x,y) Koordinate im RiverBasin liegt
		/**/{
		/**/	RivBasDayWithd = RivBasDayWithd + (m_pGFlow + m_pBFlow + m_pCFlow)*(1-m_pFlowFac);		//faktorielle tageswasserverluste aus RivBasin
		/**/	RivBas = RivBas - (m_pGFlow + m_pBFlow + m_pCFlow)*(1-m_pFlowFac);						//faktorielle Wasserverluste aus RivBasin pro Zeitschritt
		/**/}
		/**///%%%%%%%%%%%%%%%%%%%%%%%%%%		
		
		/**///%%%% Monitoring %%%%%%%%%%
		/**/WSystemDayWithd = WSystemDayWithd + m_pCon;												//flächenbasierte Tagesentnhamen aus WSystem
		/**/WSystem = WSystem - m_pCon;																//Entnommenes Wasser aus WSystem über die Fläche pro Zeitschritt
		/**/if(m_pBasinShare->asDouble(x,y) >= 0 )													//Nur Monitoren wenn (x,y) Koordinate im RiverBasin liegt
		/**/{
		/**/	RivBasDayWithd = RivBasDayWithd + m_pCon;											//fläcenbasierte Tagesentnahmen aus RivBasin
		/**/	RivBas = RivBas - m_pCon;															//Entnommenes Wasser aus RivBasin über die Fläche pro Zeitschritt
		/**/	resRivBasConMonth = resRivBasConMonth + m_pCon;
		/**/}
		/**/m_pSumResCon->Add_Value(x, y, m_pCon);
		/**///%%%%%%%%%%%%%%%%%%%%%%%%%%	

		//FehlerAbfrage!
		if( (((m_pGFlow + m_pBFlow + m_pCFlow)*m_pFlowFac) - m_pCon) < 0 )
			Message_Dlg("Negativer Abfluss errechnet -> FEHLER");
	//%%%%%%%%%%%%%%%%%%%%%%%%%% EntnahmenCode - Ende %%%%%%%%%%%%%%%%%%%%%%%%%% 

		
	//---------------------------------------------------------
	//resultierende Zuflüsse in die Rasterzelle (ix,iy) entspricht (Abfluss der Rasterzelle (x,y) minus der Entnahmen bzw. faktoriellen Minderungen)

		
		if( m_pDTM->is_InGrid(ix,iy) && !m_pDTM->is_NoData(ix,iy)) //wenn (ix,iy) im Grid liegen und keine noData-Gitterzellen sind... -> Zufluss in die Zelle (ix,iy) 
		{
			m_pChannelFlow->Add_Value(ix, iy, ( (m_pGFlow + m_pBFlow + m_pCFlow)*m_pFlowFac) - m_pCon); // GitterboxZUFLUSS in Gitterbox [ix,iy] minus Entnahme (WCon). Es gilt jeweils am Tagesende (da immer bei Tageswechsel Null gesetzt wird): [m3/d]
			
			m_pTempC->Add_Value( ix, iy, ( (m_pGFlow + m_pBFlow + m_pCFlow)*m_pFlowFac) - m_pCon); // GitterboxZUFLUSS: Zelle (x,y) 'übergibt' Inhalt (OverlandFlow+BaseFlow+ChannelFlow - Entnahme(WCon) ) an niedrigste Umgebungszelle (ix,iy)
			m_pTemp->Add_Value (ix, iy , ( (m_pGFlow + m_pBFlow + m_pCFlow)*m_pFlowFac) - m_pCon); // Zwischenspeicher GitterboxZUFLUSS minus Entnahme(WCon) um diesen im nä Zeitschritt in den ersten Speicher der ChannelKaskade zu schreiben
		}
		else //wenn (ix,iy) nicht im Grid und/oder eine NoData-Gitterzelle ist... -> SYSTEMabfluss (dieser Wasserabfluss wird nicht mehr berücksichtigt)
		{
			/**///%%%% Monitoring %%%%%%%%%% //Da Senke oder Rand -> Speicherrinhalt m_pTempC seit dem letzten Zeitschritts fließt ab (bzw. wird gelöscht). //Für das RiverBasin sollte diese bedingung höchstens für die Ausgangsbox des Basin zutreffen (siehe nächsten MonitoringSchritt)
			/**/WSystemOut = WSystemOut + (((m_pGFlow + m_pBFlow + m_pCFlow)*m_pFlowFac) - m_pCon);
			/**/WSystem = WSystem - (((m_pGFlow + m_pBFlow + m_pCFlow)*m_pFlowFac) - m_pCon);
			/**/
			/**/WSystemDayOut = WSystemDayOut + (((m_pGFlow + m_pBFlow + m_pCFlow)*m_pFlowFac) - m_pCon);
			/**///%%%%%%%%%%%%%%%%%%%%%%%%%%
		}
		//---------------------------------------------------------


		/**///%%%% Monitoring %%%%%%%%%% RiverBasin-AUSGANG (Effektiver Abfluss aus Flusseinzugsgebiet)
		/**/if(x==Parameters("RBMx")->asInt() && y==Parameters("RBMy")->asInt()) //(x==16 && y==121)//Elbe2_5000m | (x==78 && y==607)//Elbe2_1000m | (x==166 && y==843)//Nile_5000m // STATISCH!! Abfrage ob Ausgangsbox aus RiverBasine vorliegt -> RivOut setzen == RiverBasine-Abfluss (Flussabfluss)
		/**/{
		/**/RivOut = RivOut + (((m_pGFlow + m_pBFlow + m_pCFlow)*m_pFlowFac) - m_pCon);
		/**/RivBas = RivBas - (((m_pGFlow + m_pBFlow + m_pCFlow)*m_pFlowFac) - m_pCon);
		/**/
		/**/RivDayOut = RivDayOut + (((m_pGFlow + m_pBFlow + m_pCFlow)*m_pFlowFac) - m_pCon);
		/**/RivMonthOut = RivMonthOut + (((m_pGFlow + m_pBFlow + m_pCFlow)*m_pFlowFac) - m_pCon);
		/**/}
		/**///-------------------------------------------------------
		
		/**/// TestRoutine 1 ----------------------------------------
		if(Parameters("Test1")->asInt() > 0 && x == Parameters("xt1")->asInt() && y == Parameters("yt1")->asInt())
		{//Effektiver Abfluss aus der Hauptgerinnerasterzelle der TestRoutine1 
			m_pTestR1 = m_pTestR1 + (((m_pGFlow + m_pBFlow + m_pCFlow)*m_pFlowFac) - m_pCon); //Summe der errechneten Abflüsse aus Flussrasterzelle (x,y) über alle Zeitschritte
			m_pTestR1m = m_pTestR1m + (((m_pGFlow + m_pBFlow + m_pCFlow)*m_pFlowFac) - m_pCon); //Summe der errechneten Abflüsse aus Flussrasterzelle (x,y) pro Monat		
		}
		/**///%%%%%%%%%%%%%%%%%%%%%%%%%%
	}
	else //Falls KEINE Abflüsse stattfinden können, Systemabfluss des Wassers des vorherigen Zeitschritts!!
	{
		double outflow1 = r - m_pRun;		//Inhalt von m_pTempR->asDouble(x,y) des letzten Zeitschritts abfließen lassen	
		double outflow2 = d - m_pDrain;		//Inhalt von m_pTempD->asDouble(x,y) des letzten Zeitschritts abfließen lassen
		double outflow0 = 0;
		double remWith = 0;					//Wassermenge die nicht entnommen werden kann, weil zuwenig Speicherabfluss nach draußen	

		if( (m_pCCacheOut->asDouble(x,y) + outflow1 + outflow2) >= m_pCon)
			outflow0 = (m_pCCacheOut->asDouble(x,y) + outflow1 + outflow2) - m_pCon;
		else
			remWith = m_pCon - m_pCCacheOut->asDouble(x,y) + outflow1 + outflow2; //bedeutet: outflow0 == 0


		/**///%%%% Monitoring %%%%%%%%%% //Da Senke oder Rand -> Speicherrinhalte seit dem letzten Zeitschritts fliessen ab (bzw. wird gelöscht).
		/**/WSystemOut = WSystemOut + outflow0;
		/**/WSystem = WSystem - outflow0;
		/**/
		/**/WSystemDayOut = WSystemDayOut + outflow0;
		/**/WSystemDayWithd = WSystemDayWithd + m_pCon - remWith;
		/**/		
		/**/if(m_pBasinShare->asDouble(x,y) >= 0 ) //Abfrage ob (x,y) Koordinate im RiverBasin liegt
		/**/{
		/**/	RivBas = RivBas - outflow0;
		/**/	RivBasSink = RivBasSink + outflow0;
		/**/	RivBasDayWithd = RivBasDayWithd + m_pCon - remWith;
		/**/	resRivBasConMonth = resRivBasConMonth + m_pCon - remWith;
		/**/	remRivBasConMonth = remRivBasConMonth + remWith;
		/**///	m_pSumRemCon->Add_Value(x,y,remWith);						//ÄNDERUNG: 081114 - Anscheinden doppelter Eintrag s. Zeilen drunter (verantwortlich für Fehler in der Bilanz?)
		/**/}
		/**/m_pSumRemCon->Add_Value(x, y, remWith);
		/**/m_pSumResCon->Add_Value(x, y, m_pCon - remWith);
		/**///%%%%%%%%%%%%%%%%%%%%%%%%%%		
		
		m_pTempR->Set_Value(x, y, m_pRun);		//Speicher nur auf verikalen Zufluss dieses Zeitschritts setzen
		m_pTempD->Set_Value(x, y, m_pDrain);	//Speicher nur auf verikalen Zufluss dieses Zeitschritts setzten
		m_pCCacheOut->Set_Value( x, y, 0);		// Wenn Senke vorliegt (häufig am Rande des Grids der Fall), wird Inhalt des letzten Zeitschrittes dt_n-1 (m_pCCacheOut(x,y)) gelöscht (forcierter Abfluss nach außen). Zufluss in die Box während des jeweiligen Zeitschrittes dt_n (aus anderen Zellen in m_pTempC(x,y)) wird aber reingeschrieben.
	}
	
}
//
//######################################################



//######################################################
//
double CLandFlow::Calc_GFlow(int x, int y, double f) //OverlandFlow	ACHTUNG!! Funktioniert so nur mit nO = 1 !!
{
	double g = 0.0;
	int i;
	
	if(kArray[x][y][0] > 0)
	{
		for(i=0; i < nG; i++) 
		{
			//--Test
			double w = kArray[x][y][0];
			double q = CalcRet(kArray[x][y][0], nG);
			//--Test

			g = f / CalcRet(kArray[x][y][0], nG);
		}
	}

	return( g );
}
//
//######################################################



//######################################################
//
double CLandFlow::Calc_CFlow(int x, int y, double f, int n) //ChannelFlow/Gerinneabfluss
{
	double g = 0.0;
	double h = 0.0;
	int i;
	
	/**///%%%% Monitoring %%%%%%%%%%
	/**/nCArray[x][y][nMax] = 0;
	/**///%%%%%%%%%%%%%%%%%%%%%%%%%%
	
	if(kArray[x][y][1] > 0 )
	{
		g = f; // Gitterboxzufluss des letzten Zeitschritts in den ersten Kaskadenspeicher [0]
		
		for(i=0; i < n; i++)
		{
			
			h = (nCArray[x][y][i] + g); // Speicherinhalt + Zufluss   //Entnahme wird bisher immer beim Gitterboxzufluss (hier Eingangswert f) des letzten Zeitschritts berücksichtigt!!

			if(n == 1 && f > 0) //testStop
			{
				int ay = 1;
			}

			if(h< 0) //testStop
			{
				int ay = 1;
			}

			//--Test
			double w = kArray[x][y][1];
			double q = CalcRet(kArray[x][y][1], n);
			//--Test

			if( h >= 0)
				g = h / CalcRet(kArray[x][y][1], n);	//Abfluss aus Speicher
			else										
			{
				g = 0;
				if(Parameters("eP")->asBool())
					WriteLog("FEHLER errC! Negativer Abfluss ermittelt - Abfluss für diesen Zeitschritt auf Null gesetzt");
	
				errC = 1;
			}

			if( (h-g) < 0) //testStop
			{
				int az = 1;
			}

			nCArray[x][y][i] = h - g; //verbleibender Speicherinhalt -> g == Zufluss des nächsten Speichers

			/**///%%%% Monitoring %%%%%%%%%%
			/**/nCArray[x][y][nMax] = nCArray[x][y][nMax] + nCArray[x][y][i];
			/**///%%%%%%%%%%%%%%%%%%%%%%%%%%	

		}
	}


	if(g < 0)	//Falls kein positiver Abfluss aus letztem Speicher -> Abfluss gleich Null 
		g = 0;

	return( g ); //Abfluss des letzten Speichers == Abfluss der Gitterbox in diesem Zeitschritt 
}
//
//######################################################



//######################################################
double CLandFlow::Calc_BFlow(int x, int y, double f) //BaseFlow/Grundwasserabfluss ACHTUNG!! Funktioniert so nur mit nB = 1 !!
{
	double g = 0.0;
		
	if(kArray[x][y][2] > 0)
	{
		//--Test
		double w = kArray[x][y][2];
		double q = CalcRet(kArray[x][y][2], 1);
		//--Test

		g = f / CalcRet(kArray[x][y][2], 1);
	}
	
	return( g );
}
//######################################################





//######################################################
double CLandFlow::CalcRet(double q, int n)
{
	return(q * 86400 / m_pTStep); // f[d] und m_pTSTep [s] - ACHTUNG n hier NICHT mehr miteinbezogen (17.02.14) [vormals: q * 86400 / m_pTStep*n ]  
}
//######################################################



//SubBasinId
//######################################################
bool CLandFlow::SubBasinId(int x, int y)
//Funktion gibt aus dem m_pSumRunoffDrainage-Raster die Summe der RunOff und Drainage Werte aller Rasterzellen des Teileinzugsgebiets der Flussrasterzelle (x,y),
//ODER aller Rasterzellen des Abflusspfades bis zum Erreichen der HG-Rasterzelle   
{
	//KoordinatenDouble erzeugen FORMAT: xxxxyyyy
	double r = 0;
	int u = 10000*x + y;
	r = u; //Koordinate der Flussrasterzelle des Teileinzugsgebietes - FORMAT: xxxxyyyy 

	NumGridsTestR1 = 1; //entspricht zumindest einer HG-Raterzelle im Teileinzugsgebiet/Einzugsgebiet

	double SumRD = m_pSumRunoffDrainage->asDouble(x,y); //als erstes die Summe der RunOff und Drainage Werte der Mündungs-HG-Rasterzelle des Teileinzugsgebiets/Einzugsgebiets setzten
	double SumCon = m_pSumCon->asDouble(x,y);
	double SumResCon = m_pSumResCon->asDouble(x,y);
	double SumRemCon = m_pSumRemCon->asDouble(x,y);

	
	if(Parameters("Test1")->asInt() == 1)
	{
		if(m_pBasinShare)
		{
			for(int ix=0; ix < NX; ix++)
			{
				for(int iy=0; iy < NY; iy++)
				{
					if(!m_pDTM->is_NoData(ix,iy) && m_pBasinShare->asDouble(ix,iy) == r)
					{
						SumRD = SumRD + m_pSumRunoffDrainage->asDouble(ix,iy);
						SumCon = SumCon + m_pSumCon->asDouble(ix,iy);
						SumResCon = SumResCon + m_pSumResCon->asDouble(ix,iy);
						SumRemCon = SumRemCon + m_pSumRemCon->asDouble(ix,iy);

						NumGridsTestR1++;
					}
				}
			}
		}
		else
			return(false);
	}

	if(Parameters("Test1")->asInt() == 2)
	{
		if(m_pTestR1Share)
		{
			for(int ix=0; ix < NX; ix++)
			{
				for(int iy=0; iy < NY; iy++)
				{
					if(!m_pDTM->is_NoData(ix,iy) && m_pTestR1Share->asDouble(ix,iy) == r)
					{
						SumRD = SumRD + m_pSumRunoffDrainage->asDouble(ix,iy);
						SumCon = SumCon + m_pSumCon->asDouble(ix,iy);
						SumResCon = SumResCon + m_pSumResCon->asDouble(ix,iy);
						SumRemCon = SumRemCon + m_pSumRemCon->asDouble(ix,iy);

						NumGridsTestR1++;
					}
				}
			}
		}
		else
			return(false);
	}

	m_pTestR1RDm = SumRD - SumRD_SubBasin;
	m_pTestR1Cm = SumCon - SumCon_SubBasin;
	m_pTestR1resCm = SumResCon - SumResCon_SubBasin;
	m_pTestR1remCm = SumRemCon - SumRemCon_SubBasin;

	SumRD_SubBasin = SumRD;
	SumCon_SubBasin = SumCon;
	SumResCon_SubBasin = SumResCon;
	SumRemCon_SubBasin = SumRemCon;
	
	return(true);
}
//######################################################


//######################################################
//Gesamtes Einzugsgebiet der HG-Rasterzelle (HGx,HGy) von TestR1 bestimmen
void CLandFlow::TestR1Share(int HGx, int HGy)
{
	int x, y;
	
	//KoordinatenDouble der HG-Rasterzelle erzeugen FORMAT: xxxxyyyy
	double r = 0;
	int u = 10000*HGx + HGy;
	//
	
	r = u; //Wert der gesetzt wird : FORMAT: xxxxyyyy (Koordinate der "Ziel"HG-Rasterzelle in alle im Abflusspfad voherigen Rasterzellen münden)
	
	
	if(m_pTestR1Share)
	{
		for(int l=0; l < m_pDTM->Get_NCells() && Set_Progress_NCells(l); l++) //über alle Zellen des m_pDEM-Grids
		{
			m_pDTM->Get_Sorted(l, x, y); //sortieren der Zellen von höchster (l=0) nach niedrigster - 1.Durchlauf um zu gucken, ob Abflusspfad in (HGx, HGy) endet

			while( !m_pDTM->is_NoData(x,y) && ( x != HGx || y != HGy ) )
			{
				int i = m_pDTM->Get_Gradient_NeighborDir(x, y);
				
				if(i >= 0) //es gibt eine niedrigere Nachbarzelle...
				{
					x = Get_xTo(i, x);
					y = Get_yTo(i, y);
				
					if(x == HGx && y == HGy)
					{
						m_pDTM->Get_Sorted(l, x, y);		//2.Durchlauf um Abflusspfad mit den koordinaten der HG-Rasterzelle (HGx, HGy) zu markieren.

						while( !m_pDTM->is_NoData(x,y) && ( x != HGx || y != HGy )  )
						{
							int j = m_pDTM->Get_Gradient_NeighborDir(x, y);
			
							if(j >= 0) //es gibt eine niedrigere Nachbarzelle...
							{
								m_pTestR1Share->Set_Value(x, y, r); //Koordinaten der Ziel-Flussgridbox werden gesetzt
								
								x = Get_xTo(j, x);
								y = Get_yTo(j, y);
							}
							else
								{break;} //Nur für den Fall; j = -1 sollte eigentlich nicht mehr möglich sein, da letzte Gridbox ja HG-Rasterzelle...
						}
					}
				}
				else
					break; //Senke liegt vor
			}
		}
	}
}
//######################################################



//
//######################################################
bool CLandFlow::WConsRiv(int i)		//Berechnung der dynamschen Wasserentnahmewerte nach Funktionsauswahl 1 oder 2 (bisher auf Monatsbasis)
{
	double SumCons = 0;
	double SumInFlow = 0;
	double valTest = 0;
	double BasinCon = 0;
	int m = 1; //Multiplikativer Faktor für die Wasserentnahme (NUR für Entnahme aus HG-Rasterzelle) - PARAMETERWAHL wird bisher NICHT gelogt!

	m_pWConsOut->Assign(0.0);
	
	if(i == 3) // falls Entnahme Funktion 3 (Entnahme des genauen Wertes der Eingangsdaten aus jeder Raterzelle) -> dann entspricht WasserentnahmeRaster genau den Eingangsdaten  
		m_pWConsOut->Assign(&pWConsData);
	else
		if(m_pNumInFlow && m_pBasinShare)
		{
			//FlussGrids Identifizieren und damit jeweiliges Teileinzugsgebiet bestimmen
			for(int x=0; x < NX; x++)
			{
				for(int y=0; y < NY; y++)
				{
					if( m_pRivGrids && Parameters("RivG")->asInt() > 0 && m_pRivGrids->asDouble(x,y) != 0) //Wenn HG-Rasterzelle
					{
						double r = 0;					//KoordinatenDouble erzeugen FORMAT: xxxxyyyy
						int u = 10000*x + y;
						r = u; //Wert der gesetzt wird : FORMAT: xxxxyyyy (Koordinate der Flussrasterzelle des Teileinzugsgebietes)
					
						//Falls Entnahme Funktion 1: Entnahme des Teileinzugsgebietes NUR aus der HauptgerinneRasterzelle
						if(i == 1)
						{
							m_pWConsOut->Add_Value(x, y, m*pWConsData.asDouble(x, y)); //Entnahmewert der HG-Rasterzelle aus HG-Rasterzelle entnehmen
							BasinCon = BasinCon + m*pWConsData.asDouble(x, y);		 //Aufsummierung der Entnahme im gesamten Einzugsgebiet 
							
							for(int ix=0; ix < NX; ix++)
							{	
								for(int iy=0; iy < NY; iy++)
								{
									if(!m_pDTM->is_NoData(ix,iy) && m_pBasinShare->asDouble(ix,iy) == r)
										m_pWConsOut->Add_Value(x, y, (m*pWConsData.asDouble(ix, iy)) ); //Entnahmewert der Teileinzugsgebiets-Rasterzelle aus HG-Rasterzelle entnehemn
										BasinCon = BasinCon + (m*pWConsData.asDouble(ix, iy));		  //Aufsummierung der Entnahme im gesamten Einzugsgebiet 
								}
							}
						}

						//Falls Entnahme Funktion 2: Entnahme des Teileinzugsgebietes ANTEILIG aus jeder Rasterzelle des Teileinzuggebietes
						//Berechnugsparameter bestimmen:
						//SumCons=Summe aller Ennahmen des Teileinzuggebietes (inkl HG-Rasterzelle)
						//SumInFlow=Aufsummierte InFlowCell Werte aller Rasterzellen des Teileinzuggebietes
						if(i == 2)
						{
							SumCons = pWConsData.asDouble(x, y);					//Entnahmen im Teileinzugsgebiet: Entnahme der HG-Rasterzelle (x,y) setzten
							SumInFlow = m_pNumInFlow->asDouble(x, y);
		
							for(int ix=0; ix < NX; ix++)
							{
								for(int iy=0; iy < NY; iy++)
								{
									if(!m_pDTM->is_NoData(ix,iy) && m_pBasinShare->asDouble(ix,iy) == r)
									{
										//TEST
										double Tst = pWConsData.asDouble(ix, iy);
										double Tst2 = m_pNumInFlow->asDouble(ix, iy);

										SumCons = SumCons + pWConsData.asDouble(ix, iy);
										SumInFlow = SumInFlow + m_pNumInFlow->asDouble(ix, iy);
									}
								}
							}

							BasinCon = BasinCon + SumCons;		  //Aufsummierung der Entnahme im gesamten Einzugsgebiet
								
							//Auf Basis des Entnahmeanteils P = m_pNumInFlow(Rasterzelle) / SumInFlow , bestimmen des Entnahmewertes über P*SumCons und schreiben in m_pWConsOut
							if(m_pNumInFlow->asDouble(x, y) == 0)	//Wenn SONDERFALL: HG-Rasterzelle hat keine Zuflüsse, daher m_pNumInFlow und SumInFlow == 0 -> keine antielige sonder absolute Entnahme!
							{
								m_pWConsOut->Set_Value(x, y, SumCons); //...bestimmen des Entnahmewertes der HG-Zelle
								valTest = SumCons;						//Validierungstest: Summe der anteiligen Entnahmen im Teileinzugsgebiet
							}
							else //anteilige Entnahme
							{
								m_pWConsOut->Set_Value(x, y, (m_pNumInFlow->asDouble(x, y) / SumInFlow * SumCons)); //...bestimmen des Entnahmewertes der HG-Zelle
								valTest = m_pNumInFlow->asDouble(x, y) / SumInFlow * SumCons;						//Validierungstest: Summe der anteiligen Entnahmen im Teileinzugsgebiet
							}

							for(int ix=0; ix < NX; ix++)
							{
								for(int iy=0; iy < NY; iy++)
								{
									if(!m_pDTM->is_NoData(ix,iy) && m_pBasinShare->asDouble(ix,iy) == r && m_pNumInFlow->asDouble(x, y) > 0)
									{
										double partCon = (m_pNumInFlow->asDouble(ix, iy) / SumInFlow * SumCons);
										valTest = valTest + partCon;				//Validierungstest: Summe der anteiligen Entnahmen im Teileinzugsgebiet
										m_pWConsOut->Set_Value(ix, iy, partCon);	//...bestimmen des Entnahmewertes der übrigen Rasterzelle
									}
								}
							}				
						}
						
					}
				}
			}
		}		//valTest Breakpoint: valTest ungefähr SumCons ??
		else
			return(false);

	return(true);
}
//######################################################







//#########################################################
//---------------------------------------------------------
//#########################################################




///////////////////////////////////////////////////////////
//														 //
//	Sekundär-Funktionen									 //
//														 //
///////////////////////////////////////////////////////////


//---------------------------------------------------------
//Initiierungsfunktionen
//---------------------------------------------------------
//
void CLandFlow::InitKArray() //Warnung : Funktion statisch auf Grid m_pGrad eingestellt
{
	double Cx;
	int nCx;
	
	for(int x=0; x < NX; x++)
		{
			for(int y=0; y < NY; y++)
			{
				if(m_pGrad->is_NoData(x,y) || m_pGrad->asDouble(x,y) == 0)
				{
					kArray[x][y][0] = -1;
					kArray[x][y][1] = -1;
					kArray[x][y][2] = -1;
				}
				
				else
				{
					if( Parameters("RivG")->asInt() > 0 && m_pRivGrids->asDouble(x,y) != 0) // falls Flusssgrids berücksichtigt werden und Flussgrid...
					{
						Cx = C0r;
						nCx = nCr;
					}
					else
					{
						Cx = C0;
						nCx = nC;
					}

					double kG = G0/(nG*2) * (m_pDTM->Get_Cellsize() / 1000 ) / pow(m_pGrad->asDouble(x,y), 0.1 ); // *0* DEFAULT:  pG = 17.87e-2*dx/(grad)^0.1 -> dx in km
					double kC = Cx/(nCx*2) * (m_pDTM->Get_Cellsize() / 1000 ) / pow(m_pGrad->asDouble(x,y), 0.1 ); // *1* DEFAULT: pC = 9.92e-4*dx/(grad)^0.1 -> dx in km
					double kB = pB * (m_pDTM->Get_Cellsize() / 1000 ) / 50; // *2* kB [d], dx [km] / d0=50000 m )
					
					
					if( kG > kMinG )	
						kArray[x][y][0] = kG;
					else
						kArray[x][y][0] = kMinG;
					
					if( kC > kMinC )	
						kArray[x][y][1] = kC;
					else
						kArray[x][y][1] = kMinC;

					if ( kB > kMinB )	
						kArray[x][y][2] = kB;
					else
						kArray[x][y][2] = kMinB;
				}
			}
		}
}


void CLandFlow::InitNcArray(int p1, int p2, int p3)
{
	// Initiieren mit Wert Null
	for(int x=0; x < p1; x++)
	{
		for(int y=0; y < p2; y++)
		{
			for(int i = 0; i < p3; i++)
			{
				nCArray[x][y][i] = 0;
			}
		}
	}
}


/*void CLandFlow::InitPArray(int p1, int p2, int p3) //zur Zeit: nicht gesetzt
{
	// Initiieren mit Wert Null
	for(int x=0; x < p1; x++)
	{
		for(int y=0; y < p2; y++)
		{
			for(int i = 0; i < p3; i++)
			{
				pArray[x][y][i] = 0;
			}
		}
	}
}*/



//---------------------------------------------------------
//Speicherfunktionen bzw. Schreibefunktion
//---------------------------------------------------------
//
bool CLandFlow::SaveNcCache(int nMax) 
{
	int i = 0;
	int o = 0;

	std::stringstream path0;
	std::string path;
	path0.str("");
	path0 << m_pSPath << "\\NcCache.txt";
	path = path0.str();

	int d = NX * NY;
	fstream myfile;
	myfile.open(path, ios::out, ios::trunc);
	
	for(int x = 0; x < NX; x++)
	{
		for(int y = 0; y < NY; y++)
		{
			myfile << x << " " << y;

			for(o = 0; o < nMax; o++)
			{
/**/			if(o == 0 && m_pTempK->asDouble(x,y) >= 0) // Änderung 181114: Gitterboxzufluss des letzten Zeitschritts wird im ersten Speicher gerspeichert...
/**/				myfile << " " << ( nCArray[x][y][o] + m_pTempK->asDouble(x,y) );
/**/			else			
					myfile << " " << nCArray[x][y][o]; 
			}

			myfile << "\n";
			i++;
			Process_Set_Text(CSG_String::Format(SG_T("NcCache || Schreibe Datensatz %d von %d"),i ,d));
		}
	}

	myfile.close();

return true;
}


bool CLandFlow::SaveParameters()
{
	int i = 0;

	std::stringstream path0;
	std::string path;
	path0.str("");
	path0 << m_pSPath << "\\SimParameters.txt";
	path = path0.str();

	int d = NX * NY;
	time_t rawtime;
	time(&rawtime);
	fstream myfile;
	myfile.open(path, ios::out, ios::trunc);

	myfile << "KALIBRIERUNGSVERSION des Programms! EingangsPARAMETER C0, C0r und G0 unabhängig von n." << "\n\n";
	myfile << "Basierend auf: ProgrammBuild " << cVers << "\n";
	myfile << "TimeStamp: " << ctime(&rawtime) << "\n\n";
	myfile << "Parameter:\n\n";
	myfile << "Startjahr = " << m_sYear0 << "\n";
	myfile << "Simulation mit Speicherwerten initialisieren [0 = nein, 1 = ja]: " << Parameters("CacheUse")->asBool() << "\n";
	myfile << "Wasservolumen in den Systemspeichern zu Beginn der Simulation: RivBasInit = " << RivBasInit << " und WSystemInit = " << WSystemInit << "\n";

	myfile << "Rechentage [d] = " << m_pDays << "\n";
	myfile << "Schrittlänge [s] = " << m_pTStep << "\n\n";
	myfile << "vMax [km/h] = " << vMax << "\n";
	myfile << "Auflösung [m] = " << m_pDTM->Get_Cellsize() << "\n";
	myfile << "Anzahl der Reihen NX = " << NX << "\n";
	myfile << "Anzahl der Zeilen NY = " << NY << "\n";
	myfile << "Anzahl der Rasterzellen des DTM (ohne NoDataZellen): " << NumGrids << "\n";
	myfile << "Anzahl der Rasterzellen im Flusseinzugsgebiet: " << NumRBGrids << "\n\n";
	myfile << "LSSchemaDaten-Dateipfad [Ordner]: " << m_pLSDataFolder << "\n";
	myfile << "WaterGapDaten-Dateipfad [Ordner]: " << m_pWConDataFolder << "\n"; 
	myfile << "Datenpfad wo gespeicher werden soll: " << m_pDataSaveFolder << "\n";
	myfile << "Absoluter SpeicherDateipfad: " << m_pSPath << "\n\n";
	

	myfile << "nG = " << nC << "\n";
	myfile << "nHG = " << nCr << "\n";
	myfile << "nO = " << nG << "\n";
	myfile << "cG = " << C0 << "\n";
	myfile << "cHG = " << C0r << "\n";
	myfile << "cO = " << G0 << "\n";
	myfile << "pB = " << pB << "\n\n";
	
	myfile << "Fester Reduzierungsfaktor des Surface Runoff [%/100] = " << m_pRedFacR << "\n";
	myfile << "Fester Reduzierungsfaktor der Drainage [%/100] = " << m_pRedFacD << "\n";
	myfile << "Ausgleich der Surface Runoff Reduktion = " << Parameters("autoFacD")->asBool() << "\n2";
	myfile << "Offsetwert Surface Runoff [m³/s] = " << m_pOffsetR << "\n";
	myfile << "Offsetwert Drainage [m³/s] = " << m_pOffsetD << "\n";myfile << "Flussgrid-Berücksichtigung/Bestimmung = " << Parameters("RivG")->asInt() << "\n";
	
	myfile << "kMinG [d] = " << kMinC << "\n";
	myfile << "kMinHG [d] = " << kMinCr << "\n";
	myfile << "kMinO [d] = " << kMinG << "\n";
	myfile << "kMinB [d] = " << kMinB << "\n";
	myfile << "vMaxG [km/h] = " << vMaxC << "\n";
	myfile << "vMaxHG [km/h] = " << vMaxCr << "\n";
	myfile << "vMaxO [km/h] = " << vMaxG << "\n";
	myfile << "Berechnung NUR im RiverBasin = " << Parameters("onlyRB")->asBool() << "\n\n";

	myfile << "Flächenwasserentnahme - Auswahl [0=keine] = " << Parameters("WCons")->asInt() << "\n";
	myfile << "Fester Gitterbox-Wasserentnahmefaktor [%] = " << m_pConsFacAll << "\n";
	myfile << "Fester FlussGitterbox-Wasserentnahmefaktor [%] = " << m_pConsFacRiv << "\n";
	myfile << "Vorlauftage bevor Wasserentnahme berücksichtigt wird [d] = " << m_pWConsDIn << "\n\n";
	myfile << "Schwellenwert ab dem eine Entnahem erst vorgenommen wird [m³/s] = " << m_pWConThres << "\n";
	myfile << "Einheit der eingelesenen WaterGapDaten ([m³/s] = 0, [m³/Monat] = 1): " << Parameters("WConUnit")->asInt() << "\n\n";

	myfile << "m_pVTresh erzwingen = " << Parameters("EnfVmax")->asBool() << "\n";
	myfile << "m_pVTresh [km/h] = " << m_pVTresh << "\n";
	myfile << "Autom. Zeitschrittlänge = " << Parameters("CalcT")->asBool() << "\n";
	myfile << "k_Min [d] = " << k_Min << "\n\n";
	
	myfile << "ErrorCode = " << errC << "\n\n";

	myfile << "Evaluierungspunkte:" << "\n";
	myfile << "Evaluierungspunkt 1 - " << m_pEvP1s.b_str() << ", x=" << Parameters("EvP1x")->asInt() << ", y=" << Parameters("EvP1y")->asInt() << "\n";
	myfile << "Evaluierungspunkt 2 - " << m_pEvP2s.b_str() << ", x=" << Parameters("EvP2x")->asInt() << ", y=" << Parameters("EvP2y")->asInt() << "\n";
	myfile << "Evaluierungspunkt 3 - " << m_pEvP3s.b_str() << ", x=" << Parameters("EvP3x")->asInt() << ", y=" << Parameters("EvP3y")->asInt() << "\n";
	myfile << "Ausgangsrasterzelle des Flusseinzugsgebiet: " << "x=" << Parameters("RBMx")->asInt() << ", y=" << Parameters("RBMy")->asInt() << "\n\n";

	if(Parameters("vRM1")->asInt() > 0 || Parameters("vRM2")->asInt() > 0)
	{
		myfile << "Abflussmanipulation Rasterzelle 1 - Manipulation des berechnteen Abflusses [1], Abfluss aus Speicher vorgeben [2]: " << Parameters("vRM1")->asInt() << "\n";
		myfile << "Koordinate: x = " << Parameters("RM1x")->asInt() << ", y = " << Parameters("RM1y")->asInt() << "\n";
		myfile << "Faktor q = " << Parameters("RM1q")->asDouble() << " und Offset a = " <<  Parameters("RM1a")->asDouble() << "\n\n";
		
		myfile << "Abflussmanipulation Rasterzelle 2 - Manipulation des berechnteen Abflusses [1], Abfluss aus Speicher vorgeben [2]: " << Parameters("vRM2")->asInt() << "\n";
		myfile << "Koordinate: x = " << Parameters("RM2x")->asInt() << ", y = " << Parameters("RM2y")->asInt() << "\n";
		myfile << "Faktor q = " << Parameters("RM2q")->asDouble() << " und Offset a = " <<  Parameters("RM2a")->asDouble() << "\n\n";

		myfile << "Gesamtmenge des Wassers, die während des Simualtionszeitraumes durch die Rasterzellenmanipulation dem RivBasin hinzugefügt (+) oder entnommen (-) wurde: " << manWithd << "\n\n";
	}

	if(Parameters("Test1")->asInt() > 0)
	{
		myfile << "TestRoutine [1]: 1) nur für Teileinzugsgebiet der HG-Rasterzelle oder [2] für das Flusseinzugsgebiet bis zum Erreichen der HG-Rasterzelle: " << Parameters("Test1")->asInt() << "\n\n";
		myfile << "TestRoutine 1: Flusseinzugsgebiet der Flussrasterzelle (" << Parameters("xt1")->asInt() << "," << Parameters("yt1")->asInt() << ")\n";
	
		if(SumRD_SubBasin >= 0)
		{	
			myfile << "Anzahl der Rasterzellen des Teileinzugsgebietes: " << NumGridsTestR1 << "\n";
			myfile << "Summe der vertikalen Zuflüsse im Teileinzugsgebiet über den gesamten Simulationszeitraum: " << SumRD_SubBasin << "\n";
			myfile << "Summe der Abflüsse aus der Flussrasterzelle über den gesamten Simulationszeitraum: " << m_pTestR1 << "\n";
			myfile << "Differenz: " << SumRD_SubBasin - m_pTestR1 << "\n\n";
		}
		else
			myfile << "FEHLER beim Ausführen der TestRoutine 1.\n\n";
	}


	/*myfile << "x | y | kG | kC | kB | Wcons" << "\n";			//Schreiben aller K-Werte zu den jeweiligen (x,y) Koordinaten
	
	for(int x = 0; x < NX; x++)
	{
		for(int y = 0; y < NY; y++)
		{
			myfile << x << " " << y;
			myfile << " " << kArray[x][y][0];
			myfile << " " << kArray[x][y][1];
			myfile << " " << kArray[x][y][2];
	
			if( Parameters("WCons")->asBool() )
				myfile << " " << (m_pWCons->asDouble(x,y) / 1000) * pow(m_pDEM->Get_Cellsize(), 2) * m_pTStep;
			
			myfile << "\n";
			i++;
			Process_Set_Text(CSG_String::Format("Parameters || Schreibe Datensatz %d von %d",i ,d));
		}
	}*/

	myfile.close();

return true;
}


bool CLandFlow::WriteOutput(string s, double p1, double p2, int x, int y)
{
	int i = 0;
	
	std::stringstream path0;
	std::string path;
	path0.str("");
	path0 << m_pSPath << "\\" << s ;
	path = path0.str();

	ofstream myfile;
	myfile.open(path, ios_base::app);
	
	if(x >= 0 && y >= 0 )
		myfile << x << " " << y << " " << p1 << " " <<  p2 << "\n";
	else
		myfile << p1 << " " << p2 << "\n";
	
	myfile.close();

return true;
}


bool CLandFlow::WriteRivBalance(int i1, int i2, double p1, double p2)
{
	std::stringstream path0;
	std::string path;
	path0.str("");
	path0 << m_pSPath3 << "\\" << "RivWaterBalance_" << m_sYear << "-" <<  dC << ".txt";
	path = path0.str();

	ofstream myfile;
	myfile.open(path, ios_base::app);
	
	if(p1 < 0 && p2 < 0 ) //nur schreiben des headers
	{
		myfile << "Jahr:" << i1 << "    Tag:" << i2 << "\n";
		myfile << "Koordinate    Flusskilometer    m3/s" << "\n";
	}
	else
		myfile << "(" << i1 << "," << i2 << ") " << p1 << " " << p2 << "\n";
	
	myfile.close();

return true;
}


//---------------------------------------------------------
// Logeinträge
//---------------------------------------------------------
//
bool CLandFlow::TestLog1(string s)
{
	int i = 0;

	std::stringstream path0;
	std::string path;
	path0.str("");
	path0 << m_pSPath << "\\Monitoring_RivBasin.txt";
	path = path0.str();
	
	ofstream myfile;
	myfile.open(path, ios_base::app);

			myfile << s << "\n";
			Process_Set_Text(CSG_String::Format(SG_T("Schreibe in Logdatei")));

	myfile.close();

return true;
}

bool CLandFlow::TestLog2(string s)
{
	int i = 0;

	std::stringstream path0;
	std::string path;
	path0.str("");
	path0 << m_pSPath << "\\Monitoring_WSystem.txt";
	path = path0.str();

	ofstream myfile;
	myfile.open(path, ios_base::app);

			myfile << s << "\n";
			Process_Set_Text(CSG_String::Format(SG_T("Schreibe in Logdatei")));

	myfile.close();

return true;
}

bool CLandFlow::TestLog3(string s)
{
	int i = 0;

	std::stringstream path0;
	std::string path;
	path0.str("");
	path0 << m_pSPath << "\\Monitoring_monthWUseRivBasin.txt";
	path = path0.str();

	ofstream myfile;
	myfile.open(path, ios_base::app);

			myfile << s << "\n";
			Process_Set_Text(CSG_String::Format(SG_T("Schreibe in Logdatei")));

	myfile.close();

return true;
}

bool CLandFlow::TestLog4(string s)
{
	int i = 0;

	std::stringstream path0;
	std::string path;
	path0.str("");
	path0 << m_pSPath << "\\TestRoutine1.txt";
	path = path0.str();

	ofstream myfile;
	myfile.open(path, ios_base::app);

			myfile << s << "\n";
			Process_Set_Text(CSG_String::Format(SG_T("Schreibe in Logdatei")));

	myfile.close();

return true;
}


bool CLandFlow::WriteLog(string s)
{
	int i = 0;

	std::stringstream path0;
	std::string path;
	path0.str("");
	path0 << m_pSPath << "\\ErrorLog.txt";
	path = path0.str();

	ofstream myfile;
	myfile.open(path, ios_base::app);

			myfile << s << "\n";
			Process_Set_Text(CSG_String::Format(SG_T("Schreibe in Logdatei")));

	myfile.close();

return true;
}


//-----------------------------------------------------
//	ReadCacheFile Functions
//-----------------------------------------------------
//
double CLandFlow::ReadNCacheFile(bool p0, string s1, int p1, int p2, int p3, int p4, int p5, char p6, int p7, bool p8)
{
	//p0 -	false : Opt1 (Datei in Array einelsen)		/	true : Opt2 (Spez Wert zu Koord (x,y) einlesen)						
	//p1 -	Anzahl der x-Einträge						/	Anzahl der x-Einträge
	//p2 -	Anzahl der y-Einträge						/	Anzahl der y-Einträge
	//p3 -	0											/	x-Koordinate (x0 = 0)
	//p4 -	0											/	y-Koordinate (y0 = 0)
	//p5 -	Anzahl der Speicherwerte pro x,y Koordinate	/	Anzahl n der Speicherwerte pro x,y Koordinate
	//p6 -	Trennzeichen [char]							/	Trennzeichen [char]
	//p7 -	0											/	spez einzulesender Speicherwert s von n
	//p8 -	if(true) - erste Zeile enthält Sonderinf.	/	false 
	
	int x = 0;
	int y = 0;
	int o = 0;
	int r = 0;
	std::string transfer;
	char a;
	double ret = -1;
	
	std::stringstream path0;
	std::string path;
	path0.str("");
	path0 << m_pNCacheFolder;
	path = path0.str();

	ifstream myfile;
	myfile.open(path, ios_base::in);
	
	if( myfile.is_open() && !myfile.eof() )
	{
		if(p0 == false) //p0 -	false : Opt1 (Datei in Array einelsen)
		{
			if(p8 == true)
				getline(myfile, transfer, '\n');

			for(int x = 0; x < p1; x++)
			{
				for(int y = 0; y < p2; y++)
				{
					if(!myfile.eof())
					{
						for(int m = 0; m < 2; m++)
						{
							while( (a = myfile.get()) != p6 && a != '\n')
							{}
						}
					
						for(o = 0; o < p5; o++)
						{		
							if(a != 0)
							{
								if(o<p5-1)
									getline(myfile, transfer, p6);
								else
									getline(myfile, transfer, '\n');

								nCArray[x][y][o] = atof(transfer.c_str());
							}
							else
							{
								WriteLog("FEHLER!! Datei hat zu wenig Speichereinträge - ABBRUCH");
								return ret;
							}
						}
						r++;
						Process_Set_Text(CSG_String::Format(SG_T("NC Datensatz %d wurde eingelesen"),r));
					}
					else
					{
						WriteLog("FEHLER!! Anzahl der angegebenen (x,y) Koordinaten und Anzahl der Speichereinträge der NC-Speicherdatei stimmen nicht überein - ABBRUCH");
						return ret;
					}
				}
			}
			ret = 0;
		}
		else //	true : Opt2 (Spez Wert zu Koord (x,y) einlesen)
		{
			for(int x = 0; x < p1; x++)
			{
				for(int y = 0; y < p2; y++)
				{
					if( !myfile.eof() )
					{
						if(x == p3 && y == p4)
						{
						
							/*int diff = (p1+1)-(p3+1);

							int pos = p3*(p2+1)+(p4+1);

							for(int i = 0; i < pos; i++)
							{
								while(myfile.get() != '\n')
								{}							
							}*/

							if(p5-p6 >= 0)
							{
								for(int m = 0; m < (2+p6-1); m++)
								{
									while( (a = myfile.get()) != p6)
									{}

									if(a == '\n')
									{
										WriteLog("FEHLER!! Angabe der Anzahl der Speicherwerte pro Koordinate ist nicht korrekt - ABBRUCH");
										return ret;
									}
								}
					
								if(p5 - p6 > 0)
									getline(myfile, transfer, p6);
								else
									getline(myfile, transfer, '\n');
						
								ret = atof(transfer.c_str());
								return ret;
							}
							else
							{
								WriteLog("FEHLER!! Einzulesend Speicherwert existiert nicht - ABBRUCH");
								return ret;
							}
						}
						else
						{
							getline(myfile, transfer, '\n');
							//ret = atof(transfer.c_str()); // testzwecke
						}
					}
					else
					{
						WriteLog("FEHLER!! Datei enthält weniger Speichereinträge als abgefragt werden - ABBRUCH");
						return ret;
					}
				}
			}
		}
	}
	else
	{
		WriteLog("FEHLER!! Dateifehler - Datei NC konnte nicht geöffnet werden - ABBRUCH.");
		return ret;
	}	
	
	myfile.close();

	return ret;
}

//-----------------------------------------------------
//	CreateArray Functions
//-----------------------------------------------------
//
void CLandFlow::CreateKArray(int p1, int p2, int p3)
{
		kArray = new double**[p1];
		for (int i = 0; i < p1; i++)
		{
			kArray[i] = new double*[p2];
			for (int j = 0; j < p2; j++)
				kArray[i][j] = new double[p3];
		}
}


void CLandFlow::CreateLSMArray(int p1, int p2, int p3)
{
		lsMArray = new double**[p1];
		for (int i = 0; i < p1; i++)
		{
			lsMArray[i] = new double*[p2];
			for (int j = 0; j < p2; j++)
				lsMArray[i][j] = new double[p3];
		}
}

void CLandFlow::CreateNcArray(int p1, int p2, int p3)
{
	nCArray = new double**[p1];
		for (int i = 0; i < p1; i++)
		{
			nCArray[i] = new double*[p2];
			for (int j = 0; j < p2; j++)
				nCArray[i][j] = new double[p3]; 
		}
}


void CLandFlow::CreatePArray(int p1, int p2, int p3)
{
	pArray = new double**[p1];
		for (int i = 0; i < p1; i++)
		{
			pArray[i] = new double*[p2];
			for (int j = 0; j < p2; j++)
				pArray[i][j] = new double[p3];
		}
}



//-----------------------------------------------------
//	DeleteArray Functions
//-----------------------------------------------------
//ACHTUNG : statisch auf NX/NY festgelegt - Fehlerpotential, weil keine Größenabfrage
void CLandFlow::DeleteKArray()
{
	for(int x = 0; x < NX; x++)
	{
		for(int y = 0; y < NY; y++)
		{
				delete [] kArray[x][y];
		}
		delete [] kArray[x];
	}
	delete [] kArray;

	kArray = NULL; 
}


void CLandFlow::DeletePArray()
{
	for(int x = 0; x < NX; x++)
	{
		for(int y = 0; y < NY; y++)
		{
				delete [] pArray[x][y];
		}
		delete [] pArray[x];
	}
	delete [] pArray;

	pArray = NULL;
}

void CLandFlow::DeleteNcArray()
{
	for(int x = 0; x < NX; x++)
	{
		for(int y = 0; y < NY; y++)
		{
				delete [] nCArray[x][y];
		}
		delete [] nCArray[x];
	}
	delete [] nCArray;

	nCArray = NULL;
}

//#########################################################
//---------------------------------------------------------
//#########################################################

//###################################################################################################
// Vorlage
//###################################################################################################

////////////////TEST
/*			if( dC >9 && dC > 12 && x == 145 && y == 578)
			{	
				double pTest = m_pCCacheOut->asDouble(x,y) - m_pCFlow;
				
				std::stringstream file0;
				file0.str("");
				file0 << "Neu Darchau-Cache: Tag = " << dC << ", Zeitschritt = " << dT << ", Entnahme m_pCFlow = " << m_pCFlow << ", Speicherwert = " << pTest;
				std::string file = file0.str();
				
				TestLog1( file );
			}
			
			if( dC >9 && dC > 12 && ix == 145 && iy == 578)
			{	
				m_pTest = m_pTest + (m_pGFlow + m_pBFlow + m_pCFlow - m_pCon)*m_pStCons;

				std::stringstream file0;
				file0.str("");
				file0 << "Neu Darchau-InFlow: Tag = " << dC << ", Zeitschritt = " << dT << ", InFlow in TStep = " << o << ", Entnahme WCons = " << varCons << ", Sum InFlow = " << m_pTest;
				std::string file = file0.str();

				TestLog2( file );
			}*/
///////////////




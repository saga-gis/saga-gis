
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
//                    RivGridPrep.cpp                    //
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


#include "RivGridPrep.h"


//---------------------------------------------------------
//#########################################################

CRivGridPrep::CRivGridPrep(void)
{
	//-----------------------------------------------------
	// Modul information

	Set_Name		("RiverGridGeneration");
	Set_Author		("Christian Alwardt");
	Set_Description	("Generation of RiverCourse-GridCells");

	//-----------------------------------------------------
	// Define your parameters list...

	Parameters.Add_Grid(
		NULL, "INPUT"	, "Geländemodell (DTM)",
		"Digitales Geländemodell des Flusseinzugsgebietes",
		PARAMETER_INPUT
	);

	Parameters.Add_Grid(
		NULL, "OUTPUT"	, "HG Raster",
		"Schrittweise Ausgabe der gewünschten Abflusspfade des Hauprgerinnerasters",
		PARAMETER_OUTPUT
	); // Flussgrids [!=0] , keine Flussgrid [0]


	Parameters.Add_Value(
		NULL, "SX"	, "Abflusspfad-Quelle, x-Wert",
		"X-Wert der Quellen-Rasterzelle",
		PARAMETER_TYPE_Int, 0, 0, true
	);

	Parameters.Add_Value(
		NULL, "SY"	, "Abflusspfad-Quelle, y-Wert",
		"Y-Wert der Quellen-Rastertzelle",
		PARAMETER_TYPE_Int, 0, 0, true
	);

	Parameters.Add_Value(
		NULL, "MX"	, "Abflusspfad-Mündung, x-Wert",
		"X-Wert des Mündungs-Rasterzelle",
		PARAMETER_TYPE_Int, 0, 0, true
	);

	Parameters.Add_Value(
		NULL, "MY"	, "Abflusspfad-Mündung, y-Wert",
		"Y-Wert des MündungsRasterzelle",
		PARAMETER_TYPE_Int, 0, 0, true
	);

	Parameters.Add_Value(
		NULL, "Owrite", "Overwrite RiverGridCells",
		"Bereits existierende RiverGridCells werden überschrieben (Wenn nicht gesetzt: Abflusspfadende, wenn eine existierende RiverGridCell erreicht wird -> zB bei Fluss mit zwei Quellen sinnvoll).",
		PARAMETER_TYPE_Bool, false
	);
}


///////////////////////////////////////////////////////////
//														 //
//	Hauptfunktion										 //
//														 //
///////////////////////////////////////////////////////////


bool CRivGridPrep::On_Execute(void)
{
	//-----------------------------------------------------
	// Get the parameter settings...

	m_pDTM = Parameters("INPUT")->asGrid();
	m_pRivGridCells = Parameters("OUTPUT")->asGrid();
	m_pSX = Parameters("SX")->asInt();
	m_pSY = Parameters("SY")->asInt();
	m_pMX = Parameters("MX")->asInt();
	m_pMY = Parameters("MY")->asInt();
		
	//-----------------------------------------------------
	// Do something...
	

	if(!Set_RivGridCells(m_pSX, m_pSY, m_pMX, m_pMY))
		Message_Dlg("Achtung, Fehler beim Erzeugen des Flussgrids");

	//-----------------------------------------------------
	// Return 'true' if everything is okay...

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
bool CRivGridPrep::Set_RivGridCells(int sx, int sy, int mx, int my )
{
	//Identifizierung der Flussgrids und setzen des Flusskilometerwertes - immer von Gitterboxmittelpunkt zu Mittelpunkt
	
	
//	if( Parameters("Owrite")->asBool() )
//		m_pRivGrids->Assign(0.0);
	
	int x = sx;
	int y = sy;
	int i = -1;
	double dist = m_pDTM->Get_Cellsize() / 2;	// Wert Flusskilomter der ersten Flussgitterboxmitte
	m_pRivGridCells->Set_Value(x, y, dist); // setzen der Kilometer bis zur Mitte der ersten Flussgitterbox

	while( !m_pDTM->is_NoData(x,y))
	{
		i = m_pDTM->Get_Gradient_NeighborDir(x, y);

		if(i >= 0)
		{
			dist = dist + Get_Length(i);				//bisheriger FlusskilometerWert + Weg zur nächsten Gitterboxmitte (abhängig ob Wasser diagonal oder rechtwinklig in nächste Gitterbox fließt) am Ende der Gridbox wird in FlussGitterbox geschrieben (!=0) 

			x = Get_xTo(i, x);
			y = Get_yTo(i, y);
			
			if(!Parameters("Owrite")->asBool() && m_pRivGridCells->asDouble(x,y) != 0) //falls "überschreiben" nicht gesetzt, bricht Funktion ab sobald ein Grid != 0 erreicht wird.
				return(true);
			
			if(x == mx && y == my)
			{
				m_pRivGridCells->Set_Value(x, y, dist); // setzen der Kilometer bis zur Mitte der letzten Flussgitterbox

				DataObject_Update(m_pRivGridCells);
				return (true);
			}
			else
				m_pRivGridCells->Set_Value(x, y, dist);	//FlusskilometerWert der nächsten Gridbox(mitte) wird in nächste FlussGitterbox i (ix,iy) geschrieben (!=0)

			DataObject_Update(m_pRivGridCells);
		

		}
		else
			return(false);
	}
	
	return (false);
}
///////////////////////////////////////////////////////////
//														 //
//														 //
//														 //
///////////////////////////////////////////////////////////

//---------------------------------------------------------

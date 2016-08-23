
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
//                      LandFlow.h                       //
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

//---------------------------------------------------------
#ifndef HEADER_INCLUDED__LandFlow_H
#define HEADER_INCLUDED__LandFlow_H

//---------------------------------------------------------
#include <saga_api/saga_api.h>
#include <string>
using namespace std;
//---------------------------------------------------------
//#########################################################

class CLandFlow : public CSG_Tool_Grid
{
public: ////// public members and functions: //////////////

	CLandFlow(void);							// constructor


protected: /// protected members and functions: ///////////

	virtual bool		On_Execute	(void);		// always overwrite this function


private: ///// private members and functions: /////////////

/// Members
//Grids
	CSG_Grid *m_pDTM;
	CSG_Grid *m_pGrad;
	CSG_Grid *m_pDirec;
	CSG_Grid *m_pRFlow;
	CSG_Grid *m_pDFlow;
	CSG_Grid *m_pWConsIn;				//statisches WaterConsumption Grid in [kg m-2 s-1]
	CSG_Grid *m_pWConsOut;				//dynamisch generiertes WaterConsumption Grid in [kg m-2 s-1]
	CSG_Grid *m_pCCacheIn;
	CSG_Grid *m_pRCacheIn;
	CSG_Grid *m_pDCacheIn;
	CSG_Grid *m_pCCacheOut;
	CSG_Grid *m_pRCacheOut;
	CSG_Grid *m_pDCacheOut;
	CSG_Grid *m_pTempR;
	CSG_Grid *m_pTempC;
	CSG_Grid *m_pTempD;
	CSG_Grid *m_pChannelFlow;
	CSG_Grid *m_pSumRunoffDrainage;
	CSG_Grid *m_pTemp;
	CSG_Grid *m_pTempK;
	CSG_Grid *m_pRivGrids;
	CSG_Grid *m_pRivGrad;
	CSG_Grid *m_pBasinShare;
	CSG_Grid *m_pRivBalanceGrid;
	CSG_Grid *m_pNumInFlow;
	CSG_Grid *m_pSumCon;				//eigentliche Entnahme
	CSG_Grid *m_pSumResCon;				//resultierende Entnahme 
	CSG_Grid *m_pSumRemCon;				//nicht berücksichtigte Entnahmen 
	CSG_Grid *m_pTestR1Share;			//gesammtes Einzugsgebiet zu HG-Rasterzelle (x,y) der TestR1 Routine
	CSG_Grid pWConsData;


	CSG_String m_pWConsFile;
	CSG_String m_pLSDataFolder;
	CSG_String m_pDataSaveFolder;
	CSG_String m_pWConDataFolder;
	CSG_String m_pNCacheFolder;
	CSG_String m_pDateipfad;
	//CSG_String m_pLSData26File;
	//CSG_String m_pLSData27File;
	CSG_String m_pEvP1s;
	CSG_String m_pEvP2s;
	CSG_String m_pEvP3s;

	std::string m_pSPath;
	std::string m_pSPath2;
	std::string m_pSPath3;
	//std::string m_pLPath;

//Int
	int m_pDays;					//Festgelegte Anzahl an Simulationstagen (Input Parameter)
	int nC;							//Anzahl der Speicherkaskaden des ChannelFlow Speichers
	int nCr;						//Anzahl der Speicherkaskaden des ChannelFlow River-Speichers
	int nMax;
	int nG;
	int NX;
	int NY;
	int dC;							//dayCounter - Tageszähler [d]
	int dDC;						//dayDataCounter - Datentag-Zähler [d] (1-365/366)
	int mYC;						//MonthOfYear Monatszähler (1-12)
	int dMC;						//DayOfMonth Tageszähler [d]			(1-28/29/30/31)
	int m_pDDays;
	int m_pTStep;					//Zeitschrittlänge dt
	int errC;
	int errC2;
	int errC3;
	int errC4;
	int m_sYear0;
	int m_sYear;
	int m_pWConsD;					// verbleibende Tage bevor Entnahme (Operator) es gilt: m_pWConsD <= m_pWConsDIn, m_pWConsD--
	int m_pWConsDIn;				// Tage bevor jedwede Entnahme beginnt (Input-Variable)
	int numTS;						// Anzahl der Zeitschritte pro Tag
	int NumRBGrids;					//Anzahl der Rasterzellen im Flusseinzugsgebiet
	int NumGrids;					//Anzahl der Rasterzellen des DTM (ohne NoDataValues)
	int NumGridsTestR1;			//Anzahl der Rasterzellen des Teileinzugsgebietes der TestRoutine1

	int var1;						//Programmvariable : Prozentuale Wasserentnahme initiieren
	int var2;						//Programmvariable : Testlog 1
	int var3;						//Programmvariable : Testlog 2
	int var4;						//Programmvariable : 

//double
	double cVers;					//Compelierungs-Version
	double m_pGFlow;
	double m_pCFlow;
	double m_pBFlow;
	double m_pRun;
	double m_pDrain;
	double m_pVTresh;
	double k_Min;
	double kMinG;
	double kMinC;
	double kMinCr;
	double kMinB;
	double vMaxC;
	double vMaxCr;
	double vMaxG;
	double vMax;
	double dT;						//daytimeCounter - Tageszeitzähler [s]
	double G0;
	double C0;
	double C0r;						//Parameter für ChannelFlow im River [optional]
	double pB;
	double m_pCon;					//Flächenbasierte Entnahmen (absoluter Wert)
	double m_pConsFacAll;			//Entnahmefaktor aus Gitterboxen [%]/[dt]
	double m_pConsFacRiv;			//Entnahmefaktor aus Fluss-Gitterboxen [%]/[dt]
	double m_pRedFacR;				//Reduzierungsfaktor RunOff [%]/[dt]
	double m_pRedFacD;				//Reduzierungsfaktor Drainage [%]/[dt]
	double m_pHFlowFac;				//Weiterleitungsfaktor des Abfluss aus Gitterboxen in % des tatsächlichen Abflusses
	double m_pRFlowFac;				//Weiterleitungsfaktor des Fluss-Gitterboxen in % des tatsächlichen Abflusses
	double m_pVRFlowFac;			//reduzierter vertikaler Zufluss (Runnoff) in % des tatsächlichen RunOff-Zuflusses
	double m_pVDFlowFac;			//reduzierter vertikaler Zufluss (Drainage) in % des tatsächlichen Drainage-Zuflusses
	double m_pFlowFac;				//Abfrageplatzhalter für m_pHFlowFac oder m_pRFlowFac
	double m_pTest;
	double stCons;
	double varCons;
	double SumRDMonth;				//Summe der Runoff und Drainage Werte des Einzugsgebietes eines Monats
	double SumRMonth;				//NUR Summe der Runoff-Werte des Einzugsgebietes eines Monats
	double SumDMonth;				//NUR Summe der Drainage-Werte des Einzugsgebietes eines Monats
	double SumRD_SubBasin;			//Summe der Runoff und Drainage Werte eines SubBasin [TestRoutine 1]
	double SumCon_SubBasin;			//Summe der beabsichtigten Entnahmen eines SubBasin [TestRoutine 1]
	double SumResCon_SubBasin;		//Summe der tatsächlichen Entnahmen eines SubBasin [TestRoutine 1]
	double SumRemCon_SubBasin;		//Summe der nicht berücksichtigten Entnahmen eines SubBasin [TestRoutine 1]
	double m_pTestR1;				//SpeicherVariable TestRoutine 1
	double m_pTestR1m;				//SpeicherVariable TestRoutine 1
	double m_pTestR1RDm;			//SpeicherVariable TestRoutine 1
	double m_pTestR1Cm;				//SpeicherVariable TestRoutine 1
	double m_pTestR1resCm;			//SpeicherVariable TestRoutine 1
	double m_pTestR1remCm;			//SpeicherVariable TestRoutine 1
	double m_pWConThres;			//Schwellenwert für HG-Abfluss der auch bei Entnahmen nicht unterschritten werden soll
	double m_pOffsetR;				//Offsetwert auf den vertikalen Zufluss 'RunOff' pro Rasterzelle in m³/s
	double m_pOffsetD;				//Offsetwert auf den vertikalen Zufluss 'Drainage' pro Rasterzelle in m³/s
	
	double WSystemDayWithd;			//Summe allen Wassers, dass dem WSystem pro Tag entnommen wird oder über Entnahmefaktor verloren geht
	double WSystem;					// Im System befindliches Wasser
	double WSystemInit;				// Anfänglicher Speicherinhalt WSystem nach Speicherinitilisierung
	double WSystemIn;				// Summe des Wassers was ins System gelangt (Drainage, RunOff)
	double WSystemOut;				// Summe des Wassers welches das System verläßt (Systemabfluss)
	double WSystemDayIn;			// Summe des Wassers was am jeweiligen Tag ins System gelangt (Drainage, RunOff)
	double WSystemDayOut;			// Summe des Wassers welches am je weiligenTag das System verläßt (Systemabfluss)
	
	double RivBas;					// Im RivBasin befindliches Wasser
	double RivBasInit;				// Anfänglicher Speicherinhalt RivBasin nach Speicherinitilisierung
	double RivBasIn;				// Summe des Wassers was ins RiverBasin gelangt (Drainage, RunOff)
	double RivOut;					// Summe des Wassers welches das RiverBasin über den Fluss verläßt (Flussabfluss)
	double RivBasDayIn;				// Summe des Wassers was am jeweiligen Tag ins RiverBasin gelangt (Drainage, RunOff)
	double RivDayOut;				// Summe des Wassers welches am jeweiligen Tag das RiverBasin über den Fluss verläßt (Flussabfluss)
	double RivMonthOut;				// Summe des Wassers welches im jeweiligen Monat das RiverBasin über den Fluss verläßt (Flussabfluss)
	double WCCache;					// Speicherabbild des Wassers in den Kaskaden Speichern
	double RivBasSink;				// Summe allen Wasser das über dem Rechenzeitraum im RivBas in Senken verschwindet (gelöscht wird)
	double RivBasDayWithd;			// Summe allen Wassers, dass dem RivBasin pro Tag entnommen wird oder über Entnahmefaktor verloren geht
	double RivBasConMonth;			// Summe der auftretenden Flächenentnahmen innerhalb des gesamten Flusseinzugsgebietes pro Monat
	double resRivBasConMonth;		// Summe der tatsächlichen durchgeführten Flächenentnahmen innerhalb des gesamten Flusseinzugsgebietes pro Monat
	double remRivBasConMonth;		// Summe der Entnahmen die dem RivBasin nicht entnommen werden konnten (weil resFlow kleiner als m_pCon) pro Monat
	double manWithd;				// Wasser das durch Rasterzellen Manipulation aus dem RivBasinSystem entfernt oder hinzugefügt wurde

	double ***kArray;				
	double ***nCArray;				//Array von n-ChannelFlow-Speichern je (x,y) Koordinate
	double ***lsMArray;
	double ***pArray;

/// Functions
	void Calc_MainFlow(int x, int y);	

	double Calc_GFlow(int x, int y, double f);
	double Calc_CFlow(int x, int y, double f, int n);
	double Calc_BFlow(int x, int y, double f);
	double CalcRet(double f, int n);
	
	bool SubBasinId(int x, int y);								//Funktion gibt aus dem m_pSumRunoffDrainage-Raster die Summe der RunOff und Drainage Werte aller Rasterzellen des Teileinzugsgebiets der Flussrasterzelle (x,y)   
	bool WConsRiv(int i);
	void TestR1Share(int HGx, int HGy);

	void InitKArray();
	void InitNcArray(int p1, int p2, int p3);			//Array von n-Speichern je (x,y) Koordinate 
	void InitPArray(int p1, int p2, int p3);

	void CreateKArray(int p1, int p2, int p3);
	void CreateLSMArray(int p1, int p2, int p3);
	void CreateNcArray(int p1, int p2, int p3);
	void CreatePArray(int p1, int p2, int p3);

	void DeleteKArray();
	void DeleteLSMArray();
	void DeleteNcArray();
	void DeletePArray();

	double ReadNCacheFile(bool p0, string s1, int p1, int p2, int p3, int p4, int p5, char p6, int p7, bool p8);
	double ReadLSMCacheFile(bool p0, string s1, int p1, int p2, int p3, int p4, int p5, char p6, int p7, bool p8);
//	double ReadDNCacheFile(bool p0, string s1, int p1, int p2, int p3, int p4, int p5, char p6, int p7, bool p8);

	bool SaveNcCache(int n);
	bool SaveParameters();

	bool WriteLog(string s);
	bool TestLog1(string s);
	bool TestLog2(string s);
	bool TestLog3(string s);
	bool TestLog4(string s);
	bool WriteOutput(string s, double p1, double p2, int x, int y);
	bool WriteRivBalance(int i1, int i2, double p1, double p2);
};


///////////////////////////////////////////////////////////
//                                                       //
//                                                       //
//                                                       //
///////////////////////////////////////////////////////////

//---------------------------------------------------------
#endif // #ifndef HEADER_INCLUDED__LandFlow_H
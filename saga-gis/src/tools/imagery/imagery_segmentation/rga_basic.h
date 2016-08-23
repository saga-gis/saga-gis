/**********************************************************
 * Version $Id: rga_basic.h 1921 2014-01-09 10:24:11Z oconrad $
 *********************************************************/

///////////////////////////////////////////////////////////
//                                                       //
//                         SAGA                          //
//                                                       //
//      System for Automated Geoscientific Analyses      //
//                                                       //
//                     Tool Library                      //
//                 imagery_segmentation                  //
//                                                       //
//-------------------------------------------------------//
//                                                       //
//                     rga_basic.h                       //
//                                                       //
//                 Copyright (C) 2008 by                 //
//                      Olaf Conrad                      //
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
// 51 Franklin Street, 5th Floor, Boston, MA 02110-1301, //
// USA.                                                  //
//                                                       //
//-------------------------------------------------------//
//                                                       //
//    e-mail:     oconrad@saga-gis.org                   //
//                                                       //
//    contact:    Olaf Conrad                            //
//                Institute of Geography                 //
//                University of Goettingen               //
//                Goldschmidtstr. 5                      //
//                37077 Goettingen                       //
//                Germany                                //
//                                                       //
///////////////////////////////////////////////////////////

//---------------------------------------------------------


///////////////////////////////////////////////////////////
//														 //
//														 //
//														 //
///////////////////////////////////////////////////////////

//---------------------------------------------------------
#ifndef HEADER_INCLUDED__rga_basic_H
#define HEADER_INCLUDED__rga_basic_H

//---------------------------------------------------------
#include "MLB_Interface.h"


///////////////////////////////////////////////////////////
//														 //
//														 //
//														 //
///////////////////////////////////////////////////////////

//---------------------------------------------------------
typedef struct SCandidate
{
	int						x, y, Segment;

	double					Similarity;
}
TCandidate;

//---------------------------------------------------------
class CCandidates
{
public:
	CCandidates(void);
	CCandidates(int nMax);
	~CCandidates(void);

	void					Create			(void);
	void					Create			(int nMax);
	void					Destroy			(void);

	void					Add				(int x, int y, int Segment, double Similarity);
	bool					Get				(int &x, int &y, int &Segment);

	int						Get_Count		(void)	{	return( m_nCandidates );	}

	double					Get_Minimum		(void);
	double					Get_Maximum		(void);


private:

	int						m_nCandidates, m_nMax;

	TCandidate				*m_Candidates;

	CCandidates				*m_pLow, *m_pHigh;


	int						_Find			(double Similarity);

};


///////////////////////////////////////////////////////////
//														 //
//														 //
//														 //
///////////////////////////////////////////////////////////

//---------------------------------------------------------
class CRGA_Basic : public CSG_Tool_Grid
{
public:
	CRGA_Basic(void);


protected:

	virtual int				On_Parameters_Enable	(CSG_Parameters *pParameters, CSG_Parameter *pParameter);

	virtual bool			On_Execute				(void);


private:

	bool					m_bNormalize;

	int						m_Method, m_nFeatures, m_dNeighbour;

	double					m_Var_1, m_Var_2, m_Threshold;

	CSG_Table				*m_pSeeds;

	CSG_Grid				*m_pSegments, *m_pSimilarity;

	CSG_Parameter_Grid_List	*m_pFeatures;

	CCandidates				m_Candidates;


	double					Get_Feature				(int  x, int  y, int iFeature);

	bool					Get_Next_Candidate		(int &x, int &y, int &Segment);

	bool					Add_To_Segment			(int  x, int  y, int  Segment);

	double					Get_Similarity			(int  x, int  y, int  Segment);

};


///////////////////////////////////////////////////////////
//														 //
//														 //
//														 //
///////////////////////////////////////////////////////////

//---------------------------------------------------------
#endif // #ifndef HEADER_INCLUDED__rga_basic_H

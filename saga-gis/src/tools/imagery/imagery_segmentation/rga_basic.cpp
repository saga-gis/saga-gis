
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
//                    rga_basic.cpp                      //
//                                                       //
//                 Copyright (C) 2008 by                 //
//                     Benni Bechtel                     //
//                      Olaf Conrad                      //
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
// Public License along with this program; if not, see   //
// <http://www.gnu.org/licenses/>.                       //
//                                                       //
//-------------------------------------------------------//
//                                                       //
//    e-mail:     oconrad@saga-gis.org                   //
//                                                       //
//    contact:    Olaf Conrad                            //
//                Institute of Geography                 //
//                University of Hamburg                  //
//                Germany                                //
//                                                       //
///////////////////////////////////////////////////////////

//---------------------------------------------------------
#include "rga_basic.h"


///////////////////////////////////////////////////////////
//														 //
//														 //
//														 //
///////////////////////////////////////////////////////////

//---------------------------------------------------------
#define SEEDFIELD_X		2
#define SEEDFIELD_Y		(SEEDFIELD_X + 1)
#define SEEDFIELD_Z		(SEEDFIELD_X + 2)

#define NO_SEGMENT		-1
#define NO_SIMILARITY	-1.

//---------------------------------------------------------
class CCandidate : public CSG_PriorityQueue::CSG_PriorityQueueItem
{
public:

	int		m_x, m_y, m_Segment;	double	m_Similarity;

	CCandidate(int x, int y, int Segment, double Similarity)
		: m_x(x), m_y(y), m_Segment(Segment), m_Similarity(Similarity)
	{}

	virtual int		Compare			(CSG_PriorityQueueItem *pItem)
	{
		if( pItem )
		{
			CCandidate	&Candidate	= *((CCandidate *)pItem);

			if( m_Similarity < Candidate.m_Similarity ) { return( -1 ); }
			if( m_Similarity > Candidate.m_Similarity ) { return(  1 ); }

			return( 0 );
		}

		return( -1 );
	}
};


///////////////////////////////////////////////////////////
//														 //
//														 //
//														 //
///////////////////////////////////////////////////////////

//---------------------------------------------------------
CRGA_Basic::CRGA_Basic(void)
{
	Set_Name		(_TL("Seeded Region Growing"));

	Set_Author		("B. Bechtel, O. Conrad (c) 2008");

	Set_Description	(_TW(
		"The tool allows one to apply a seeded region growing algorithm to a stack of input features "
		"and thus to segmentize the data for object extraction. The required seed points can be created "
		"with the 'Seed Generation' tool, for example. The derived segments can be used, for example, "
		"for object based classification.\n\n"
	));

	Add_Reference("Adams, R. & Bischof, L.", "1994",
		"Seeded Region Growing",
		"IEEE Transactions on Pattern Analysis and Machine Intelligence, Vol.16, No.6, p.641-647.",
		SG_T("https://pdfs.semanticscholar.org/db44/31b2a552d0f3d250df38b2c60959f404536f.pdf"), SG_T("online")
	);

	Add_Reference("Bechtel, B., Ringeler, A. & Boehner, J.", "2008",
		"Segmentation for Object Extraction of Trees using MATLAB and SAGA",
		"In: Boehner, J., Blaschke, T., Montanarella, L. [Eds.]: SAGA - Seconds Out. "
		"Hamburger Beitraege zur Physischen Geographie und Landschaftsoekologie, 19:59-70.",
		SG_T("http://downloads.sourceforge.net/saga-gis/hbpl19_01.pdf"), SG_T("online")
	);

	//-----------------------------------------------------
	Parameters.Add_Grid("",
		"SEEDS"		, _TL("Seeds"),
		_TL(""),
		PARAMETER_INPUT
	);

	Parameters.Add_Grid_List("",
		"FEATURES"	, _TL("Features"),
		_TL(""),
		PARAMETER_INPUT
	);

	Parameters.Add_Grid("",
		"SEGMENTS"	, _TL("Segments"),
		_TL(""),
		PARAMETER_OUTPUT
	);

	Parameters.Add_Grid("",
		"SIMILARITY", _TL("Similarity"),
		_TL(""),
		PARAMETER_OUTPUT
	);

	Parameters.Add_Table("",
		"TABLE"		, _TL("Seeds"),
		_TL(""),
		PARAMETER_OUTPUT
	);

	Parameters.Add_Bool("",
		"NORMALIZE"	, _TL("Normalize Features"),
		_TL("Standardize the input features, i.e. rescale the input data (features) such that the mean equals 0 and the standard deviation equals 1. This is helpful when the input features have different scales, units or outliers."),
		false
	);

	Parameters.Add_Choice("",
		"NEIGHBOUR"	, _TL("Neighbourhood"),
		_TL(""),
		CSG_String::Format("%s|%s",
			_TL("4 (von Neumann)"),
			_TL("8 (Moore)")
		), 0
	);

	Parameters.Add_Choice("",
		"METHOD"	, _TL("Method"),
		_TL(""),
		CSG_String::Format("%s|%s",
			_TL("feature space and position"),
			_TL("feature space")
		), 0
	);

	Parameters.Add_Double("METHOD",
		"SIG_1"		, _TL("Variance in Feature Space"),
		_TL(""),
		1., 0., true	// 0.36
	);

	Parameters.Add_Double("METHOD",
		"SIG_2"		, _TL("Variance in Position Space"),
		_TL(""),
		1., 0., true	// 8.2141
	);

	Parameters.Add_Double("METHOD",
		"THRESHOLD"	, _TL("Similarity Threshold"),
		_TL(""),
		0., 0., true	// 0.15
	);

	Parameters.Add_Bool("",
		"REFRESH"	, _TL("Refresh"),
		_TL(""),
		false
	);

	Parameters.Add_Int("",
		"LEAFSIZE"	, _TL("Leaf Size (for Speed Optimisation)"),
		_TL(""),
		256, 2, true
	);
}


///////////////////////////////////////////////////////////
//														 //
///////////////////////////////////////////////////////////

//---------------------------------------------------------
int CRGA_Basic::On_Parameters_Enable(CSG_Parameters *pParameters, CSG_Parameter *pParameter)
{
	if( pParameter->Cmp_Identifier("METHOD") )
	{
		pParameters->Set_Enabled("SIG_2", pParameter->asInt() == 0);
	}

	return( CSG_Tool_Grid::On_Parameters_Enable(pParameters, pParameter) );
}


///////////////////////////////////////////////////////////
//														 //
///////////////////////////////////////////////////////////

//---------------------------------------------------------
bool CRGA_Basic::On_Execute(void)
{
	m_pSegments		= Parameters("SEGMENTS"  )->asGrid();
	m_pFeatures		= Parameters("FEATURES"  )->asGridList();
	m_nFeatures		= m_pFeatures->Get_Grid_Count();

	m_pSimilarity	= Parameters("SIMILARITY")->asGrid();

	m_dNeighbour	= Parameters("NEIGHBOUR" )->asInt() == 0 ? 2 : 1;

	m_Var_1			= Parameters("SIG_1"     )->asDouble(); m_Var_1 *= m_Var_1;
	m_Var_2			= Parameters("SIG_2"     )->asDouble(); m_Var_2 *= m_Var_2;
	m_Threshold		= Parameters("THRESHOLD" )->asDouble();

	m_bNormalize	= Parameters("NORMALIZE" )->asBool();

	m_Method		= Parameters("METHOD"    )->asInt();

	//-----------------------------------------------------
	m_pSegments  ->Assign(-1); m_pSegments  ->Set_NoData_Value(-1);
	m_pSimilarity->Assign(-1); m_pSimilarity->Set_NoData_Value(-1);

	//-----------------------------------------------------
	CSG_Grid	*pSeeds	= Parameters("SEEDS")->asGrid();

	m_pSeeds	= Parameters("TABLE")->asTable();
	m_pSeeds->Destroy();
	m_pSeeds->Add_Field("ID"  , SG_DATATYPE_Int   );
	m_pSeeds->Add_Field("AREA", SG_DATATYPE_Double);
	m_pSeeds->Add_Field("X"   , SG_DATATYPE_Double);
	m_pSeeds->Add_Field("Y"   , SG_DATATYPE_Double);

	for(int i=0; i<m_pFeatures->Get_Grid_Count(); i++)
	{
		m_pSeeds->Add_Field(m_pFeatures->Get_Grid(i)->Get_Name(), SG_DATATYPE_Double);
	}

	m_Candidates.Create(Parameters("LEAFSIZE")->asInt());

	//-----------------------------------------------------
	for(int y=0; y<Get_NY() && Set_Progress_Rows(y); y++)
	{
		for(int x=0; x<Get_NX(); x++)
		{
			if( !pSeeds->is_NoData(x, y) )
			{
				CSG_Table_Record	*pRec	= m_pSeeds->Add_Record();

				pRec->Set_Value(0, m_pSeeds->Get_Count() - 1);
				pRec->Set_Value(SEEDFIELD_X, x);
				pRec->Set_Value(SEEDFIELD_Y, y);

				for(int i=0; i<m_pFeatures->Get_Grid_Count(); i++)
				{
					pRec->Set_Value(SEEDFIELD_Z + i, Get_Feature(x, y, i));
				}

				m_pSimilarity->Set_Value(x, y, 1.);

				Add_To_Segment(x, y, m_pSeeds->Get_Count() - 1);
			}
		}
	}

	//-----------------------------------------------------
	if( m_pSeeds->Get_Count() > 1 )
	{
		bool bRefresh	= Parameters("REFRESH")->asBool();

		sLong	n	= 0;	int	x, y, Segment;

		while( n++ < Get_NCells() && Set_Progress_Cells(n) && Get_Next_Candidate(x, y, Segment) )
		{
			Add_To_Segment(x, y, Segment);

			if( bRefresh && (n % Get_NX()) == 0 )
			{
				DataObject_Update(m_pSegments, 0, m_pSeeds->Get_Count());

				Process_Set_Text("%.2f", m_Candidates.Get_Size() * 100. / Get_NCells());
			}
		}

		m_Candidates.Destroy();

		return( true );
	}

	//-----------------------------------------------------
	m_Candidates.Destroy();

	return( false );
}


///////////////////////////////////////////////////////////
//														 //
///////////////////////////////////////////////////////////

//---------------------------------------------------------
inline double CRGA_Basic::Get_Feature(int x, int y, int iFeature)
{
	double	Value	= m_pFeatures->Get_Grid(iFeature)->asDouble(x, y);

	if( m_bNormalize )
	{
		Value	= (Value - m_pFeatures->Get_Grid(iFeature)->Get_Mean()) / m_pFeatures->Get_Grid(iFeature)->Get_StdDev();
	}

	return( Value );
}


///////////////////////////////////////////////////////////
//														 //
///////////////////////////////////////////////////////////

//---------------------------------------------------------
bool CRGA_Basic::Get_Next_Candidate(int &x, int &y, int &Segment)
{
	CCandidate	*pCandidate;

	while( (pCandidate = (CCandidate *)m_Candidates.Poll()) != NULL )
	{
		x       = pCandidate->m_x;
		y       = pCandidate->m_y;
		Segment = pCandidate->m_Segment;

		delete(pCandidate);

		if( m_pSegments->is_NoData(x, y) )
		{
			return( true );
		}
	}

	return( false );
}

//---------------------------------------------------------
bool CRGA_Basic::Add_To_Segment(int x, int y, int Segment)
{
	if( is_InGrid(x, y) && m_pSegments->is_NoData(x, y) )			// if the pixel is not element of any segment...
	{
		m_pSegments->Set_Value(x, y, Segment);						// the candidate is added to the corresponding region
	
		for(int i=0, ix, iy; i<8; i+=m_dNeighbour)					// update of candidate-grid - all 8-Neigbours of the added pixel are checked
		{
			if( Get_System().Get_Neighbor_Pos(i, x, y, ix, iy) && m_pSegments->is_NoData(ix, iy) )
			{
				double	Similarity	= Get_Similarity(ix, iy, Segment);

				if(	Similarity >= m_Threshold						// a neigbour-pixel is only added as candidate if its similarity is higher than the preset threshold
				&&	Similarity > m_pSimilarity->asDouble(ix, iy) )	// and it is not candidate for another region with a higher similarity-value yet
				{
					m_Candidates.Add(new CCandidate(ix, iy, Segment, Similarity));

					m_pSimilarity->Set_Value(ix, iy, Similarity);
				}
			}
		}

		return( true );
	}

	return( false );
}

//---------------------------------------------------------
double CRGA_Basic::Get_Similarity(int x, int y, int Segment)
{
	CSG_Table_Record	*pSeed;

	if( is_InGrid(x, y) && (pSeed = m_pSeeds->Get_Record(Segment)) != NULL )
	{
		int	i;	double	a, b, Result;

		switch( m_Method )
		{
		//-------------------------------------------------
		case 0:	// feature space and position
			for(i=0, a=0.; i<m_nFeatures; i++)
			{
				a	+= SG_Get_Square(Get_Feature(x, y, i) - pSeed->asDouble(SEEDFIELD_Z + i));
			}

			b	= SG_Get_Square(x - pSeed->asDouble(SEEDFIELD_X))
				+ SG_Get_Square(y - pSeed->asDouble(SEEDFIELD_Y));

			Result	= a / m_Var_1 + b / m_Var_2;

			break;

		//-------------------------------------------------
		case 1:	// feature space
			for(i=0, a=0.; i<m_nFeatures; i++)
			{
				a	+= SG_Get_Square(Get_Feature(x, y, i) - pSeed->asDouble(SEEDFIELD_Z + i));
			}

			Result	= a / m_Var_1;

			break;
		}

		return( 1. / (1. + Result) );	// from 'distance' to 'similarity' !!!
	//	return( exp(-0.5 * Result) );
	}

	return( -1. );
}


///////////////////////////////////////////////////////////
//														 //
//														 //
//														 //
///////////////////////////////////////////////////////////

//---------------------------------------------------------

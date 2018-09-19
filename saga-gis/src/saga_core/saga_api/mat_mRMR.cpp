/**********************************************************
 * Version $Id$
 *********************************************************/

///////////////////////////////////////////////////////////
//                                                       //
//                         SAGA                          //
//                                                       //
//      System for Automated Geoscientific Analyses      //
//                                                       //
//           Application Programming Interface           //
//                                                       //
//                  Library: SAGA_API                    //
//                                                       //
//-------------------------------------------------------//
//                                                       //
//                       mRMR.cpp                        //
//                                                       //
//                 Copyright (C) 2014 by                 //
//                      Olaf Conrad                      //
//                                                       //
//-------------------------------------------------------//
//                                                       //
// This file is part of 'SAGA - System for Automated     //
// Geoscientific Analyses'.                              //
//                                                       //
// This library is free software; you can redistribute   //
// it and/or modify it under the terms of the GNU Lesser //
// General Public License as published by the Free       //
// Software Foundation, either version 2.1 of the        //
// License, or (at your option) any later version.       //
//                                                       //
// This library is distributed in the hope that it will  //
// be useful, but WITHOUT ANY WARRANTY; without even the //
// implied warranty of MERCHANTABILITY or FITNESS FOR A  //
// PARTICULAR PURPOSE. See the GNU Lesser General Public //
// License for more details.                             //
//                                                       //
// You should have received a copy of the GNU Lesser     //
// General Public License along with this program; if    //
// not, see <http://www.gnu.org/licenses/>.              //
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
// A C++ program to implement the mRMR selection using mutual information
// written by Hanchuan Peng.
//
// Disclaimer: The author of program is Hanchuan Peng
//      at <penghanchuan@yahoo.com>.
//
// The CopyRight is reserved by the author.
//
// Last modification: Jan/25/2007
//
// by Hanchuan Peng
// 2005-10-24: finalize the computing parts of the whole program
// 2005-10-25: add non-discretization for the classification variable. Also slightly change some output info for the web application
// 2005-11-01: add control to the user-defined max variable number and sample number
// 2006-01-26: add gnu_getline.c to convert the code to be compilable under Max OS.
// 2007-01-25: change the address info
//
//---------------------------------------------------------


///////////////////////////////////////////////////////////
//														 //
//														 //
//														 //
///////////////////////////////////////////////////////////

//---------------------------------------------------------
#include "mat_tools.h"
#include "parameters.h"
#include "table.h"


///////////////////////////////////////////////////////////
//														 //
//														 //
//														 //
///////////////////////////////////////////////////////////

//---------------------------------------------------------
#define DELETE_ARRAY(p)	if( p )	{	delete[]p;	p	= NULL;	}

//---------------------------------------------------------
enum ESG_mRMR_Selection
{
	SG_mRMR_SELECTION_RANK	= 0,
	SG_mRMR_SELECTION_INDEX,
	SG_mRMR_SELECTION_NAME,
	SG_mRMR_SELECTION_SCORE
};


///////////////////////////////////////////////////////////
//														 //
///////////////////////////////////////////////////////////

//---------------------------------------------------------
CSG_mRMR::CSG_mRMR(void)
{
	m_Samples		= NULL;
	m_nSamples		= 0;
	m_nVars			= 0;
	m_bDiscretized	= false;	// initialize the data as continous
	m_bVerbose		= false;

	m_pSelection	= new CSG_Table;

	m_pSelection->Add_Field("RANK" , SG_DATATYPE_Int   );	// mRMR_SELFLD_RANK
	m_pSelection->Add_Field("INDEX", SG_DATATYPE_Int   );	// mRMR_SELFLD_INDEX
	m_pSelection->Add_Field("NAME" , SG_DATATYPE_String);	// mRMR_SELFLD_NAME
	m_pSelection->Add_Field("SCORE", SG_DATATYPE_Double);	// mRMR_SELFLD_SCORE
}

//---------------------------------------------------------
CSG_mRMR::~CSG_mRMR(void)
{
	Destroy();

	delete(m_pSelection);
}

//---------------------------------------------------------
void CSG_mRMR::Destroy(void)
{
	if( m_Samples )
	{
		DELETE_ARRAY(m_Samples[0]);
		DELETE_ARRAY(m_Samples);
	}

	m_VarNames		.Clear();
	m_nSamples		= 0;
	m_nVars			= 0;
	m_bDiscretized	= false;	// initialize the data as continous

	m_pSelection	->Del_Records();
}


///////////////////////////////////////////////////////////
//														 //
///////////////////////////////////////////////////////////

//---------------------------------------------------------
CSG_String CSG_mRMR::Get_Description(void)
{
	return( _TW(
		"The minimum Redundancy Maximum Relevance (mRMR) feature selection "
		"algorithm has been developed by Hanchuan Peng <hanchuan.peng@gmail.com>.\n"
		"\n"
		"References:\n"
		"Feature selection based on mutual information: criteria of max-dependency, max-relevance, and min-redundancy. "
		"Hanchuan Peng, Fuhui Long, and Chris Ding, "
		"IEEE Transactions on Pattern Analysis and Machine Intelligence, "
		"Vol. 27, No. 8, pp.1226-1238, 2005.\n"
		"\n"
		"Minimum redundancy feature selection from microarray gene expression data,\n"
		"Chris Ding, and Hanchuan Peng, "
		"Journal of Bioinformatics and Computational Biology, "
		"Vol. 3, No. 2, pp.185-205, 2005.\n"
		"\n"
		"Hanchuan Peng's mRMR Homepage at "
		"<a target=\"_blank\" href=\"http://penglab.janelia.org/proj/mRMR/\">"
		"http://penglab.janelia.org/proj/mRMR/</a>\n"
	));
}


///////////////////////////////////////////////////////////
//														 //
///////////////////////////////////////////////////////////

//---------------------------------------------------------
bool CSG_mRMR::Parameters_Add(CSG_Parameters *pParameters, CSG_Parameter *pNode)
{
	CSG_String	ParentID(pNode ? pNode->Get_Identifier() : SG_T(""));

	pParameters->Add_Int(
		ParentID, "mRMR_NFEATURES"	, _TL("Number of Features"),
		_TL(""),
		50, 1, true
	);

	pParameters->Add_Bool(
		ParentID, "mRMR_DISCRETIZE"	, _TL("Discretization"),
		_TL("uncheck this means no discretizaton (i.e. data is already integer)"),
		true
	);

	pParameters->Add_Double(
		ParentID, "mRMR_THRESHOLD"	, _TL("Discretization Threshold"),
		_TL("a double number of the discretization threshold; set to 0 to make binarization"),
		1.0, 0.0, true
	);

	pParameters->Add_Choice(
		ParentID, "mRMR_METHOD"		, _TL("Selection Method"),
		_TL(""),
		CSG_String::Format("%s|%s|",
			_TL("Mutual Information Difference (MID)"),
			_TL("Mutual Information Quotient (MIQ)")
		), 0
	);

	return( true );
}

//---------------------------------------------------------
int CSG_mRMR::Parameters_Enable(CSG_Parameters *pParameters, CSG_Parameter *pParameter)
{
	if( pParameter->Cmp_Identifier("mRMR_DISCRETIZE") )
	{
		pParameters->Set_Enabled("mRMR_THRESHOLD", pParameter->asBool());
	}

	return( 1 );
}

//---------------------------------------------------------
bool CSG_mRMR::Set_Data(CSG_Table &Data, int ClassField, CSG_Parameters *pParameters)
{
	bool	bDiscretize	= (*pParameters)("mRMR_DISCRETIZE") ? (*pParameters)("mRMR_DISCRETIZE")->asBool  () : true;
	double	Threshold	= (*pParameters)("mRMR_THRESHOLD" ) ? (*pParameters)("mRMR_THRESHOLD" )->asDouble() : 1.0;

	return( Set_Data(Data, ClassField, bDiscretize ? Threshold : -1.0) );
}

bool CSG_mRMR::Set_Data(CSG_Matrix &Data, int ClassField, CSG_Parameters *pParameters)
{
	bool	bDiscretize	= (*pParameters)("mRMR_DISCRETIZE") ? (*pParameters)("mRMR_DISCRETIZE")->asBool  () : true;
	double	Threshold	= (*pParameters)("mRMR_THRESHOLD" ) ? (*pParameters)("mRMR_THRESHOLD" )->asDouble() : 1.0;

	return( Set_Data(Data, ClassField, bDiscretize ? Threshold : -1.0) );
}

//---------------------------------------------------------
bool CSG_mRMR::Get_Selection(CSG_Parameters *pParameters)
{
	int		nFeatures	= (*pParameters)("mRMR_NFEATURES") ? (*pParameters)("mRMR_NFEATURES")->asInt() : 50;
	int		Method		= (*pParameters)("mRMR_METHOD"   ) ? (*pParameters)("mRMR_METHOD"   )->asInt() : 0;

	return( Get_Selection(nFeatures, Method) );
}


///////////////////////////////////////////////////////////
//														 //
///////////////////////////////////////////////////////////

//---------------------------------------------------------
int CSG_mRMR::Get_Count	(void) const
{
	return( m_pSelection->Get_Count() );
}

int CSG_mRMR::Get_Index(int i) const
{
	return( m_pSelection->Get_Record(i)->asInt   (SG_mRMR_SELECTION_INDEX) );
}

CSG_String CSG_mRMR::Get_Name(int i) const
{
	return( m_pSelection->Get_Record(i)->asString(SG_mRMR_SELECTION_NAME ) );
}

double CSG_mRMR::Get_Score(int i) const
{
	return( m_pSelection->Get_Record(i)->asDouble(SG_mRMR_SELECTION_SCORE) );
}


///////////////////////////////////////////////////////////
//														 //
///////////////////////////////////////////////////////////

//---------------------------------------------------------
#define ADD_MESSAGE(Message)	if( m_bVerbose ) SG_UI_Msg_Add_Execution(CSG_String(Message) + "\n", false);
#define ADD_WARNING(Message)	if( m_bVerbose ) SG_UI_Msg_Add_Execution(_TL("Warning") + CSG_String(": ") + CSG_String(Message) + "\n", false, SG_UI_MSG_STYLE_FAILURE);


///////////////////////////////////////////////////////////
//														 //
///////////////////////////////////////////////////////////

//---------------------------------------------------------
bool CSG_mRMR::Get_Memory(int nVars, int nSamples)
{
	Destroy();

	//-----------------------------------------------------
	m_nVars		= nVars;

	if( m_nVars <= 0 )
	{
		SG_UI_Msg_Add_Error("no features");

		return( false );
	}

	m_nSamples	= nSamples;

	if( m_nSamples <= 0 )
	{
		SG_UI_Msg_Add_Error("no samples");

		return( false );
	}

	//-----------------------------------------------------
	m_Samples		= new double *[m_nSamples];

	if( !m_Samples )
	{
		SG_UI_Msg_Add_Error("failed to allocate memory.");

		return( false );
	}

	m_Samples[0]	= new double[m_nSamples * m_nVars];

	if( !m_Samples[0] )
	{
		SG_UI_Msg_Add_Error("failed to allocate memory.");

		return( false );
	}

	return( true );
}

//---------------------------------------------------------
bool CSG_mRMR::Set_Data(CSG_Table &Data, int ClassField, double Threshold)
{
	if( !Get_Memory(Data.Get_Field_Count(), Data.Get_Count()) )
	{
		return( false );
	}

	//-----------------------------------------------------
	if( ClassField < 0 || ClassField >= m_nVars )
	{
		ClassField	= 0;
	}

	Data.Set_Index(ClassField, TABLE_INDEX_Ascending);

	CSG_String	s;

	for(int iSample=0, Class=0; iSample<m_nSamples; iSample++)
	{
		double	*pData	= m_Samples[iSample] = m_Samples[0] + iSample * m_nVars;

		if( s.Cmp(Data[iSample].asString(ClassField)) )
		{
			s	= Data[iSample].asString(ClassField);

			Class++;
		}

		*pData++	= Class;

		for(int iVar=0; iVar<m_nVars; iVar++)
		{
			if( iVar != ClassField )
			{
				*pData++	= Data[iSample].asDouble(iVar);
			}
		}
	}

	Data.Del_Index();

	m_VarNames	+= Data.Get_Field_Name(ClassField);

	for(int iVar=0; iVar<m_nVars; iVar++)
	{
		if( iVar != ClassField )
		{
			m_VarNames	+= Data.Get_Field_Name(iVar);
		}
	}

	//-----------------------------------------------------
	if( Threshold >= 0.0 )	// discretization
	{
		Discretize(Threshold);
	}

	return( true );
}

//---------------------------------------------------------
bool CSG_mRMR::Set_Data(CSG_Matrix &Data, int ClassField, double Threshold)
{
	if( !Get_Memory(Data.Get_NCols(), Data.Get_NRows()) )
	{
		return( false );
	}

	//-----------------------------------------------------
	if( ClassField < 0 || ClassField >= m_nVars )
	{
		ClassField	= 0;
	}

	for(int iSample=0; iSample<m_nSamples; iSample++)
	{
		double	*pData	= m_Samples[iSample] = m_Samples[0] + iSample * m_nVars;

		*pData++	= Data[iSample][ClassField];

		for(int iVar=0; iVar<m_nVars; iVar++)
		{
			if( iVar != ClassField )
			{
				*pData++	= Data[iSample][iVar];
			}
		}
	}

	m_VarNames	+= "CLASS";

	for(int iVar=0; iVar<m_nVars; iVar++)
	{
		if( iVar != ClassField )
		{
			m_VarNames	+= CSG_String::Format(SG_T("FEATURE_%02d"), iVar);
		}
	}

	//-----------------------------------------------------
	if( Threshold >= 0.0 )	// discretization
	{
		Discretize(Threshold);
	}

	return( true );
}


///////////////////////////////////////////////////////////
//														 //
///////////////////////////////////////////////////////////

//---------------------------------------------------------
bool CSG_mRMR::Discretize(double Threshold)
{
	if( !m_Samples[0] || Threshold < 0.0 || m_bDiscretized )
	{
		return( false );
	}

	long	i, j;	// exclude the first column

	//-----------------------------------------------------
	for(j=1; j<m_nVars; j++)	// z-score
	{
		double cursum	= 0;
		double curmean	= 0;
		double curstd	= 0;

		for(i=0; i<m_nSamples; i++)
		{
			cursum	+= m_Samples[i][j];
		}

		curmean	= cursum / m_nSamples;
		cursum	= 0;

		register double tmpf;

		for(i=0; i<m_nSamples; i++)
		{
			tmpf	= m_Samples[i][j] - curmean;
			cursum += tmpf * tmpf;
		}

		curstd	= (m_nSamples == 1) ? 0 : sqrt(cursum / (m_nSamples - 1));	// m_nSamples -1 is an unbiased version for Gaussian
	
		for(i=0; i<m_nSamples; i++)
		{
			m_Samples[i][j]	= (m_Samples[i][j] - curmean) / curstd;
		}
	}

	//-----------------------------------------------------
	for(j=1; j<m_nVars; j++)
	{
		register double tmpf;

		for(i=0; i<m_nSamples; i++)
		{
			tmpf	= m_Samples[i][j];

			if( tmpf > Threshold )
			{
				tmpf	=  1;
			}
			else if( tmpf < -Threshold )
			{
				tmpf	= -1;
			}
			else
			{
				tmpf	=  0;
			}

			m_Samples[i][j]	= tmpf;
		}
	}

	m_bDiscretized	= true;

	return( true );
}


///////////////////////////////////////////////////////////
//														 //
///////////////////////////////////////////////////////////

//---------------------------------------------------------
#define ADD_FEATURE(rank, score)	{\
	CSG_Table_Record	*pFeature	= m_pSelection->Add_Record();\
	\
	pFeature->Set_Value(SG_mRMR_SELECTION_RANK , rank + 1);\
	pFeature->Set_Value(SG_mRMR_SELECTION_INDEX, feaInd[rank]);\
	pFeature->Set_Value(SG_mRMR_SELECTION_NAME , m_VarNames[feaInd[rank]]);\
	pFeature->Set_Value(SG_mRMR_SELECTION_SCORE, score);\
	\
	ADD_MESSAGE(CSG_String::Format(SG_T("%d \t %d \t %s \t %5.3f"),\
		rank + 1, feaInd[rank], m_VarNames[feaInd[rank]].c_str(), score)\
	);\
}

//---------------------------------------------------------
int CSG_mRMR::Pool_Compare(const void *a, const void *b)
{
	if( ((TPool *)a)->mival < ((TPool *)b)->mival )
		return( -1 );

	if( ((TPool *)a)->mival > ((TPool *)b)->mival )
		return(  1 );

	return( 0 );
}

//---------------------------------------------------------
bool CSG_mRMR::Get_Selection(int nFeatures, int Method)
{
	m_pSelection->Del_Records();

	if( !m_Samples[0] )
	{
		SG_UI_Msg_Add_Error("The input data is NULL.");

		return( false );
	}

	if( nFeatures < 0 )
	{
		SG_UI_Msg_Add_Error("The input number of features is negative.");

		return( false );
	}

	long poolUseFeaLen = 500;
	if( poolUseFeaLen > m_nVars - 1 )	// there is a target variable (the first one), that is why must remove one
		poolUseFeaLen = m_nVars - 1;

	if( nFeatures > poolUseFeaLen )
		nFeatures = poolUseFeaLen;

	long	*feaInd	= new long[nFeatures];

	if( !feaInd )
	{
		SG_UI_Msg_Add_Error("Fail to allocate memory.");

		return( false );
	}

	//-----------------------------------------------------
	// determine the pool

	TPool	*Pool	= (TPool *)SG_Malloc(m_nVars * sizeof(TPool));

	if( !Pool )
	{
		SG_UI_Msg_Add_Error("Fail to allocate memory.");

		return( false );
	}

	//-----------------------------------------------------
	long	i, j, k;

	for(i=0; i<m_nVars; i++)	// the Pool[0].mival is the entropy of target classification variable
	{
		Pool[i].mival	= -Get_MutualInfo(0, i);	// set as negative for sorting purpose
		Pool[i].Index	= i;
		Pool[i].Mask	= 1;	// initialized to be everything in pool would be considered
	}

	qsort(Pool + 1, m_nVars - 1, sizeof(TPool), Pool_Compare);

	Pool[0].mival	= -Pool[0].mival;

	ADD_MESSAGE(CSG_String::Format(
		SG_T("\nTarget classification variable (#%d column in the input data) has name=%s \tentropy score=%5.3f"),
		0 + 1, m_VarNames[0].c_str(), Pool[0].mival
	));

	ADD_MESSAGE("\n*** MaxRel features ***");
	ADD_MESSAGE("Order\tFea\tName\tScore");

	for(i=1; i<m_nVars-1; i++)
	{
		Pool[i].mival	= -Pool[i].mival;

		if( i <= nFeatures )
		{
			ADD_MESSAGE(CSG_String::Format(SG_T("%d \t %d \t %s \t %5.3f"),
				i, (int)Pool[i].Index, m_VarNames[(int)Pool[i].Index].c_str(), Pool[i].mival
			));
		}
	}

	//-----------------------------------------------------
	// mRMR selection

	long	poolFeaIndMin	= 1;
	long	poolFeaIndMax	= poolFeaIndMin + poolUseFeaLen - 1;

	feaInd[0]	= Pool[1].Index;
	Pool[feaInd[0]].Mask	= 0;	// after selection, no longer consider this feature
	Pool[0        ].Mask	= 0;	// of course the first one, which corresponds to the classification variable, should not be considered. Just set the mask as 0 for safety.

	ADD_MESSAGE("\n*** mRMR features ***");
	ADD_MESSAGE("Order\tFea\tName\tScore");

	ADD_FEATURE(0, Pool[1].mival);

	for(k=1; k<nFeatures; k++)	//the first one, feaInd[0] has been determined already
	{
		double	relevanceVal, redundancyVal, tmpscore, selectscore;
		long	selectind;
		int		b_firstSelected	= 0;

		for(i=poolFeaIndMin; i<=poolFeaIndMax; i++)
		{
			if( Pool[Pool[i].Index].Mask == 0 )
			{
				continue;	// skip this feature as it was selected already
			}

			relevanceVal	= Get_MutualInfo(0, Pool[i].Index);	// actually no necessary to re-compute it, this value can be retrieved from Pool[].mival vector
			redundancyVal	= 0;

			for(j=0; j<k; j++)
			{
				redundancyVal	+= Get_MutualInfo(feaInd[j], Pool[i].Index);
			}

			redundancyVal /= k;

			switch( Method )
			{
			default:	// fprintf(stderr, "Undefined selection method=%d. Use MID instead.\n", mymethod);
			case SG_mRMR_Method_MID:	tmpscore	= relevanceVal - redundancyVal;				break;
			case SG_mRMR_Method_MIQ:	tmpscore	= relevanceVal / (redundancyVal + 0.0001);	break;
			}

			if( b_firstSelected == 0 )
			{
				selectscore		= tmpscore;
				selectind		= Pool[i].Index;
				b_firstSelected	= 1;
			}
			else if( tmpscore > selectscore )
			{	//update the best feature found and the score
				selectscore		= tmpscore;
				selectind		= Pool[i].Index;
			}
		}

		feaInd[k]	= selectind;
		Pool[selectind].Mask	= 0;

		ADD_FEATURE(k, selectscore);
	}

	//-----------------------------------------------------
	return( true );
}


///////////////////////////////////////////////////////////
//														 //
///////////////////////////////////////////////////////////

//---------------------------------------------------------
double CSG_mRMR::Get_MutualInfo(long v1, long v2)
{
	double	mi	= -1;	// initialized as an illegal value

	if( !m_Samples[0] )
	{
		SG_UI_Msg_Add_Error("The input data is NULL.");

		return mi;
	}

	if( v1 >= m_nVars || v2 >= m_nVars || v1 < 0 || v2 < 0 )
	{
		SG_UI_Msg_Add_Error("The input variable indexes are invalid (out of range).");

		return mi;
	}

	//-----------------------------------------------------
	// copy data

	int	*v1data	= new int[m_nSamples];
	int	*v2data	= new int[m_nSamples];

	if( !v1data || !v2data )
	{
		SG_UI_Msg_Add_Error("Fail to allocate memory.");

		return mi;
	}

	for(long i=0; i<m_nSamples; i++)
	{
		v1data[i]	= (int)m_Samples[i][v1];	// the double already been discretized, should be safe now
		v2data[i]	= (int)m_Samples[i][v2];
	}

	//-----------------------------------------------------
	// compute mutual info

	long	nstate	= 3;	// always true for DataTable, which was discretized as three states

	int		nstate1 = 0, nstate2 = 0;

	double	*pab	= Get_JointProb(v1data, v2data, m_nSamples, nstate, nstate1, nstate2);

	mi	= Get_MutualInfo(pab, nstate1, nstate2);

	//-----------------------------------------------------
	// free memory and return

	DELETE_ARRAY(v1data);
	DELETE_ARRAY(v2data);
	DELETE_ARRAY(pab   );

	return mi;
}

//---------------------------------------------------------
double CSG_mRMR::Get_MutualInfo(double *pab, long pabhei, long pabwid)
{
	//-----------------------------------------------------
	// check if parameters are correct

	if( !pab )
	{
		SG_UI_Msg_Add_Error("Got illeagal parameter in compute_mutualinfo().");

		return( -1.0 );
	}

	//-----------------------------------------------------
	long i, j;

	double	**pab2d	= new double *[pabwid];

	for(j=0; j<pabwid; j++)
	{
		pab2d[j]	= pab + (long)j * pabhei;
	}

	double	*p1 = 0, *p2 = 0;
	long	p1len = 0, p2len = 0;
	int		b_findmarginalprob = 1;

	//-----------------------------------------------------
	//generate marginal probability arrays

	if( b_findmarginalprob != 0 )
	{
		p1len	= pabhei;
		p2len	= pabwid;
		p1		= new double[p1len];
		p2		= new double[p2len];

		for (i = 0; i < p1len; i++)	p1[i] = 0;
		for (j = 0; j < p2len; j++)	p2[j] = 0;

		for (i = 0; i < p1len; i++)	//p1len = pabhei
		{
			for (j = 0; j < p2len; j++)	//p2len = panwid
			{
			//	printf("%f ",pab2d[j][i]);
				p1[i] += pab2d[j][i];
				p2[j] += pab2d[j][i];
			}
		}
	}

	//-----------------------------------------------------
	// calculate the mutual information

	double muInf = 0;

	muInf	= 0.0;

	for (j = 0; j < pabwid; j++)	// should for p2 
	{
		for (i = 0; i < pabhei; i++)	// should for p1
		{
			if (pab2d[j][i] != 0 && p1[i] != 0 && p2[j] != 0)
			{
				muInf += pab2d[j][i] * log (pab2d[j][i] / p1[i] / p2[j]);
			//	printf("%f %fab %fa %fb\n",muInf,pab2d[j][i]/p1[i]/p2[j],p1[i],p2[j]);
			}
		}
	}

	muInf	/= log(2.0);

	//-----------------------------------------------------
	// free memory

	DELETE_ARRAY(pab2d);

	if( b_findmarginalprob != 0 )
	{
		DELETE_ARRAY(p1);
		DELETE_ARRAY(p2);
	}

	return muInf;
}


///////////////////////////////////////////////////////////
//														 //
///////////////////////////////////////////////////////////

//---------------------------------------------------------
template <class T> double * CSG_mRMR::Get_JointProb(T * img1, T * img2, long len, long maxstatenum, int &nstate1, int &nstate2)
{
	long	i, j;

	//-----------------------------------------------------
	// get and check size information

	if (!img1 || !img2 || len < 0)
	{
		SG_UI_Msg_Add_Error("At least one of the input vectors is invalid.");

		return( NULL );
	}

	int	b_findstatenum	= 1;	//  int nstate1 = 0, nstate2 = 0;
	int	b_returnprob	= 1;

	//-----------------------------------------------------
	// copy data into new INT type array (hence quantization) and then reange them begin from 0 (i.e. state1)

	int	*vec1	= new int[len];
	int	*vec2	= new int[len];

	if( !vec1 || !vec2 )
	{
		SG_UI_Msg_Add_Error("Fail to allocate memory.\n");

		return( NULL );
	}

	int	nrealstate1 = 0, nrealstate2 = 0;

	Copy_Vector(img1, len, vec1, nrealstate1);
	Copy_Vector(img2, len, vec2, nrealstate2);

	//update the #state when necessary
	nstate1 = (nstate1 < nrealstate1) ? nrealstate1 : nstate1;
	//printf("First vector #state = %i\n",nrealstate1);
	nstate2 = (nstate2 < nrealstate2) ? nrealstate2 : nstate2;
	//printf("Second vector #state = %i\n",nrealstate2);

	//  if (nstate1!=maxstatenum || nstate2!=maxstatenum)
	//    printf("find nstate1=%d nstate2=%d\n", nstate1, nstate2);

	//-----------------------------------------------------
	// generate the joint-distribution table

	double	 *hab	= new double[nstate1 * nstate2];
	double	**hab2d	= new double *[nstate2];

	if( !hab || !hab2d )
	{
		SG_UI_Msg_Add_Error("Fail to allocate memory.");

		return( NULL );
	}

	for(j=0; j<nstate2; j++)
	{
		hab2d[j]	= hab + (long)j * nstate1;
	}

	for(i=0; i<nstate1; i++)
	{
		for(j=0; j<nstate2; j++)
		{
			hab2d[j][i]	= 0;
		}
	}

	for(i=0; i<len; i++)
	{
		// old method -- slow
		//    indx = (long)(vec2[i]) * nstate1 + vec1[i];
		//    hab[indx] += 1;

		// new method -- fast
		hab2d[vec2[i]][vec1[i]]	+= 1;
		//printf("vec2[%d]=%d vec1[%d]=%d\n", i, vec2[i], i, vec1[i]);
	}

	//-----------------------------------------------------
	// return the probabilities, otherwise return count numbers

	if( b_returnprob )
	{
		for(i=0; i<nstate1; i++)
		{
			for(j=0; j<nstate2; j++)
			{
				hab2d[j][i]	/= len;
			}
		}
	}

	//-----------------------------------------------------
	// finish

	DELETE_ARRAY(hab2d);
	DELETE_ARRAY(vec1);
	DELETE_ARRAY(vec2);

	return hab;
}

//---------------------------------------------------------
template <class T> void CSG_mRMR::Copy_Vector(T *srcdata, long len, int *desdata, int &nstate)
{
	if (!srcdata || !desdata)
	{
		SG_UI_Msg_Add_Error("no points in Copy_Vector()!");

		return;
	}

	// note: originally I added 0.5 before rounding, however seems the negative numbers and 
	//      positive numbers are all rounded towarded 0; hence int(-1+0.5)=0 and int(1+0.5)=1;
	//      This is unwanted because I need the above to be -1 and 1.
	// for this reason I just round with 0.5 adjustment for positive and negative differently

	// copy data
	int	minn, maxx;
	if (srcdata[0] > 0)
	{
		maxx = minn = int (srcdata[0] + 0.5);
	}
	else
	{
		maxx = minn = int (srcdata[0] - 0.5);
	}

	long	i;
	int		tmp;
	double	tmp1;

	for (i = 0; i < len; i++)
	{
		tmp1	= double (srcdata[i]);
		tmp		= (tmp1 > 0) ? (int) (tmp1 + 0.5) : (int) (tmp1 - 0.5);	//round to integers
		minn	= (minn < tmp) ? minn : tmp;
		maxx	= (maxx > tmp) ? maxx : tmp;
		desdata[i] = tmp;
	//	printf("%i ",desdata[i]);
	}

	// make the vector data begin from 0 (i.e. 1st state)
	for (i = 0; i < len; i++)
	{
		desdata[i] -= minn;
	}

	// return the #state
	nstate = (maxx - minn + 1);

	return;
}


///////////////////////////////////////////////////////////
//														 //
//														 //
//														 //
///////////////////////////////////////////////////////////

//---------------------------------------------------------

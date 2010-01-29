
///////////////////////////////////////////////////////////
//                                                       //
//                         SAGA                          //
//                                                       //
//      System for Automated Geoscientific Analyses      //
//                                                       //
//                    Module Library:                    //
//                  Grid_Discretisation                  //
//                                                       //
//-------------------------------------------------------//
//                                                       //
//              Grid_Classify_Supervised.cpp             //
//                                                       //
//                 Copyright (C) 2005 by                 //
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
// 59 Temple Place - Suite 330, Boston, MA 02111-1307,   //
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
#include "Grid_Classify_Supervised.h"


///////////////////////////////////////////////////////////
//														 //
//														 //
//														 //
///////////////////////////////////////////////////////////

//---------------------------------------------------------
enum
{
	CLASS_NR			= 0,
	CLASS_ID,
	CLASS_TOT_N,
	CLASS_ROI_N,

	CLASS_ROI_M,
	CLASS_ROI_S,
	CLASS_ROI_MIN,
	CLASS_ROI_MAX
};

//---------------------------------------------------------
#define CLASS_NPARMS	(CLASS_ROI_MAX - CLASS_ROI_N)


///////////////////////////////////////////////////////////
//														 //
//														 //
//														 //
///////////////////////////////////////////////////////////

//---------------------------------------------------------
CClass_Info::CClass_Info(void)
{
	m_Statistics	= NULL;
	m_nElements		= NULL;
	m_nFeatures		= 0;
}

//---------------------------------------------------------
CClass_Info::~CClass_Info(void)
{
	Destroy();
}

//---------------------------------------------------------
void CClass_Info::Create(int nFeatures)
{
	Destroy();

	if( nFeatures > 0 )
	{
		m_nFeatures	= nFeatures;
	}
}

//---------------------------------------------------------
void CClass_Info::Destroy(void)
{
	if( m_IDs.Get_Count() > 0 )
	{
		for(int i=0; i<m_IDs.Get_Count(); i++)
		{
			delete[](m_Statistics[i]);
		}

		SG_Free(m_Statistics);
		SG_Free(m_nElements);

		m_Statistics	= NULL;
		m_nElements		= NULL;

		m_IDs.Clear();
	}

	m_nFeatures	= 0;
}

//---------------------------------------------------------
CSG_Simple_Statistics * CClass_Info::Get_Statistics(const CSG_String &ID)
{
	if( m_nFeatures > 0 )
	{
		int		i;

		for(i=0; i<m_IDs.Get_Count(); i++)
		{
			if( !m_IDs[i].Cmp(ID) )
			{
				return( m_Statistics[i] );
			}
		}

		m_IDs			+= ID;

		m_nElements		= (int *)SG_Realloc(m_nElements, m_IDs.Get_Count() * sizeof(int));
		m_nElements[i]	= 0;

		m_Statistics	= (CSG_Simple_Statistics **)SG_Realloc(m_Statistics  , m_IDs.Get_Count() * sizeof(CSG_Simple_Statistics *));
		m_Statistics[i]	= new CSG_Simple_Statistics[m_nFeatures];

		return( m_Statistics[i] );
	}

	return( NULL );
}


///////////////////////////////////////////////////////////
//														 //
//														 //
//														 //
///////////////////////////////////////////////////////////

//---------------------------------------------------------
CGrid_Classify_Supervised::CGrid_Classify_Supervised(void)
{
	CSG_Parameter	*pNode;

	//-----------------------------------------------------
	Set_Name		(_TL("Supervised Classification"));

	Set_Author		(SG_T("O.Conrad (c) 2005"));

	Set_Description	(_TW(
		"Supervised Classification: Minimum Distance, Maximum Likelihood.\n"
	));

	//-----------------------------------------------------
	Parameters.Add_Grid_List(
		NULL	, "GRIDS"			, _TL("Grids"),
		_TL(""),
		PARAMETER_INPUT
	);

	pNode	= Parameters.Add_Shapes(
		NULL	, "ROI"				, _TL("Training Areas"),
		_TL(""),
		PARAMETER_INPUT, SHAPE_TYPE_Polygon
	);

	Parameters.Add_Table_Field(
		pNode	, "ROI_ID"			, _TL("Class Identifier"),
		_TL("")
	);

	Parameters.Add_Table(
		NULL	, "CLASS_INFO"		, _TL("Class Information"),
		_TL(""),
		PARAMETER_OUTPUT
	);

	Parameters.Add_Grid(
		NULL	, "CLASSES"			, _TL("Classification"),
		_TL(""),
		PARAMETER_OUTPUT, true, SG_DATATYPE_Short
	);

	Parameters.Add_Grid(
		NULL	, "QUALITY"			, _TL("Quality"),
		_TL("Dependent on chosen method, these are distances or probabilities."),
		PARAMETER_OUTPUT_OPTIONAL
	);

	Parameters.Add_Choice(
		NULL	, "METHOD"			, _TL("Method"),
		_TL(""),
		CSG_String::Format(SG_T("%s|%s|%s|%s|%s|"),
			_TL("parallelepiped"),
			_TL("minimum distance"),
			_TL("mahalanobis distance"),
			_TL("maximum likelihood"),
			_TL("spectral angle mapping")
		), 0
	);

	Parameters.Add_Value(
		NULL	, "NORMALISE"		, _TL("Normalise"),
		_TL(""),
		PARAMETER_TYPE_Bool, false
	);

	Parameters.Add_Value(
		NULL	, "THRESHOLD_MD"	, _TL("Distance Threshold"),
		_TL("Let pixel stay unclassified, if minimum or mahalanobis distance is greater than threshold."),
		PARAMETER_TYPE_Double, 0.0, 0.0, true
	);

	pNode	= Parameters.Add_Value(
		NULL	, "THRESHOLD_ML"	, _TL("Probability Threshold (Percent)"),
		_TL("Let pixel stay unclassified, if maximum likelihood probability is less than threshold."),
		PARAMETER_TYPE_Double, 0.0, 0.0, true, 100.0, true
	);

	Parameters.Add_Choice(
		pNode	, "ML_RELATIVE"		, _TL("Probability Reference"),
		_TL(""),
		CSG_String::Format(SG_T("%s|%s|"),
			_TL("absolute"),
			_TL("relative")
		), 0
	);

	Parameters.Add_Value(
		NULL	, "THRESHOLD_SAM"	, _TL("Spectral Angle Threshold (Degree)"),
		_TL("Let pixel stay unclassified, if spectral angle distance is greater than threshold."),
		PARAMETER_TYPE_Double, 0.0, 0.0, true, 90.0, true
	);
}


///////////////////////////////////////////////////////////
//														 //
//														 //
//														 //
///////////////////////////////////////////////////////////

//---------------------------------------------------------
bool CGrid_Classify_Supervised::On_Execute(void)
{
	//-------------------------------------------------
	m_pGrids		= Parameters("GRIDS")		->asGridList();
 	m_pClasses		= Parameters("CLASSES")		->asGrid();
	m_bNormalise	= Parameters("NORMALISE")	->asBool();
	m_pQuality		= Parameters("QUALITY")		->asGrid();

	//-----------------------------------------------------
	for(int iGrid=m_pGrids->Get_Count()-1; iGrid>=0; iGrid--)
	{
		if( m_pGrids->asGrid(iGrid)->Get_ZRange() <= 0.0 )
		{
			m_pGrids->Del_Item(iGrid);
		}
	}

	if( m_pGrids->Get_Count() <= 0 )
	{
		return( false );
	}

	//-------------------------------------------------
	Process_Set_Text(_TL("analysing training areas"));

	if( !Initialise() )
	{
		return( false );
	}

	//-------------------------------------------------
	Process_Set_Text(_TL("running classification"));

	switch( Parameters("METHOD")->asInt() )
	{
	case 0:	return( Set_Parallel_Epiped       () );
	case 1:	return( Set_Minimum_Distance      () );
	case 2:	return( Set_Mahalanobis_Distance  () );
	case 3:	return( Set_Maximum_Likelihood    () );
	case 4:	return( Set_Spectral_Angle_Mapping() );
	}

	return( false );
}


///////////////////////////////////////////////////////////
//														 //
//														 //
//														 //
///////////////////////////////////////////////////////////

//---------------------------------------------------------
bool CGrid_Classify_Supervised::Initialise(void)
{
	//-----------------------------------------------------
	int			x, y, iGrid;
	TSG_Point	p;

	CSG_Shapes	*pROIs	= Parameters("ROI")		->asShapes();
	int			iROI_ID	= Parameters("ROI_ID")	->asInt();

	m_Class_Info.Create(m_pGrids->Get_Count());

	for(y=0, p.y=Get_YMin(); y<Get_NY() && Set_Progress(y); y++, p.y+=Get_Cellsize())
	{
		for(x=0, p.x=Get_XMin(); x<Get_NX(); x++, p.x+=Get_Cellsize())
		{
			//---------------------------------------------
			bool	bNoData;

			for(iGrid=0, bNoData=false; iGrid<m_pGrids->Get_Count() && !bNoData; iGrid++)
			{
				if( m_pGrids->asGrid(iGrid)->is_NoData(x, y) )
				{
					bNoData	= true;
				}
			}

			if( bNoData )
			{
				m_pClasses->Set_NoData(x, y);

				if( m_pQuality )
				{
					m_pQuality->Set_NoData(x, y);
				}
			}

			//---------------------------------------------
			else
			{
				m_pClasses->Set_Value(x, y, 0.0);

				for(int iROI=0; iROI<pROIs->Get_Count(); iROI++)
				{
					CSG_Simple_Statistics	*pClass;
					CSG_Shape_Polygon		*pROI	= (CSG_Shape_Polygon *)pROIs->Get_Shape(iROI);

					if( pROI->is_Containing(p) && (pClass = m_Class_Info.Get_Statistics(pROI->asString(iROI_ID))) != NULL )
					{
						for(iGrid=0; iGrid<m_pGrids->Get_Count(); iGrid++)
						{
							pClass[iGrid].Add_Value(Get_Value(x, y, iGrid));
						}
					}
				}
			}
		}
	}

	//-----------------------------------------------------
	return( m_Class_Info.Get_Count() > 1 );
}


///////////////////////////////////////////////////////////
//														 //
///////////////////////////////////////////////////////////

//---------------------------------------------------------
bool CGrid_Classify_Supervised::Finalise(void)
{
	int			iClass, iGrid, iOffset, nClasses;
	CSG_String	Name_Method, Name_Quality;
	CSG_Table	*pTable;

	//-----------------------------------------------------
	switch( Parameters("METHOD")->asInt() )
	{
	case 0:
		Name_Method	= _TL("Parallelepiped");
		Name_Quality	= _TL("Memberships");
		break;

	case 1:
		Name_Method	= _TL("Minimum Distance");
		Name_Quality	= _TL("Distance");
		break;

	case 2:
		Name_Method	= _TL("Minimum Distance");
		Name_Quality	= _TL("Distance");
		break;

	case 3:
		Name_Method	= _TL("Maximum Likelihood");
		Name_Quality	= _TL("Proximity");
		break;

	case 4:
		Name_Method	= _TL("Spectral Angle Mapping");
		Name_Quality	= _TL("Angle");
		break;
	}

	//-----------------------------------------------------
	pTable	= Parameters("CLASS_INFO")->asTable();

	pTable->Destroy();
	pTable->Set_Name(CSG_String::Format(SG_T("%s [%s]"), _TL("Class Information"), Name_Method.c_str()));

	pTable->Add_Field(_TL("NR")   , SG_DATATYPE_Int);		// CLASS_NR
	pTable->Add_Field(_TL("ID")   , SG_DATATYPE_String);	// CLASS_ID
	pTable->Add_Field(_TL("TOT_N"), SG_DATATYPE_Int);		// CLASS_TOT_N
	pTable->Add_Field(_TL("ROI_N"), SG_DATATYPE_Int);		// CLASS_ROI_N

	for(iGrid=0; iGrid<m_pGrids->Get_Count(); iGrid++)
	{
		pTable->Add_Field(CSG_String::Format(_TL("%02d_ROI_M")  , iGrid + 1), SG_DATATYPE_Double);	// CLASS_ROI_M
		pTable->Add_Field(CSG_String::Format(_TL("%02d_ROI_S")  , iGrid + 1), SG_DATATYPE_Double);	// CLASS_ROI_S
		pTable->Add_Field(CSG_String::Format(_TL("%02d_ROI_MIN"), iGrid + 1), SG_DATATYPE_Double);	// CLASS_ROI_MIN
		pTable->Add_Field(CSG_String::Format(_TL("%02d_ROI_MAX"), iGrid + 1), SG_DATATYPE_Double);	// CLASS_ROI_MAX
	}

	for(iClass=0; iClass<m_Class_Info.Get_Count(); iClass++)
	{
		CSG_Table_Record	*pRecord	= pTable->Add_Record();

		pRecord->Set_Value(CLASS_NR   , iClass + 1);
		pRecord->Set_Value(CLASS_ID   , m_Class_Info.Get_ID(iClass).c_str());
		pRecord->Set_Value(CLASS_ROI_N, m_Class_Info.Get_Statistics(iClass)->Get_Count());
		pRecord->Set_Value(CLASS_TOT_N, m_Class_Info.Get_Element_Count(iClass));

		for(iGrid=0, iOffset=0; iGrid<m_Class_Info.Get_Feature_Count(); iGrid++, iOffset+=CLASS_NPARMS)
		{
			CSG_Simple_Statistics	*pStatistics	= m_Class_Info.Get_Statistics(iClass) + iGrid;

			double	m	= m_bNormalise ? m_pGrids->asGrid(iGrid)->Get_ArithMean() : 0.0;
			double	s	= m_bNormalise ? m_pGrids->asGrid(iGrid)->Get_StdDev()    : 1.0;

			pRecord->Set_Value(iOffset + CLASS_ROI_M  , pStatistics->Get_Mean   () * s + m);
			pRecord->Set_Value(iOffset + CLASS_ROI_S  , pStatistics->Get_StdDev () * s);
			pRecord->Set_Value(iOffset + CLASS_ROI_MIN, pStatistics->Get_Minimum() * s + m);
			pRecord->Set_Value(iOffset + CLASS_ROI_MAX, pStatistics->Get_Maximum() * s + m);
		}
	}

	//-----------------------------------------------------
	CSG_Parameters	P;

	if( DataObject_Get_Parameters(m_pClasses, P) && P("COLORS_TYPE") && P("LUT") )
	{
		pTable	= P("LUT")->asTable();

		for(iClass=0, nClasses=0; iClass<m_Class_Info.Get_Count(); iClass++)
		{
			if( m_Class_Info.Get_Element_Count(iClass) > 0 )
			{
				CSG_Table_Record	*pRecord	= pTable->Get_Record(nClasses++);

				if( pRecord == NULL )
				{
					pRecord	= pTable->Add_Record();
					pRecord->Set_Value(0, SG_GET_RGB(rand() * 255.0 / RAND_MAX, rand() * 255.0 / RAND_MAX, rand() * 255.0 / RAND_MAX));
				}

				pRecord->Set_Value(1, m_Class_Info.Get_ID(iClass).c_str());
				pRecord->Set_Value(2, m_Class_Info.Get_ID(iClass).c_str());
				pRecord->Set_Value(3, iClass + 1);
				pRecord->Set_Value(4, iClass + 1);
			}
		}

		while( pTable->Get_Record_Count() > m_Class_Info.Get_Count() )
		{
			pTable->Del_Record(pTable->Get_Record_Count() - 1);
		}

		P("COLORS_TYPE")->Set_Value(1);	// Color Classification Type: Lookup Table

		DataObject_Set_Parameters(m_pClasses, P);
	}

	//-----------------------------------------------------
	m_pClasses->Set_Name(CSG_String::Format(SG_T("%s [%s]"), _TL("Classification"), Name_Method.c_str()));

	if( m_pQuality )
	{
		m_pQuality->Set_Name(CSG_String::Format(SG_T("%s [%s]"), _TL("Classification Quality"), Name_Quality.c_str()));

		DataObject_Set_Colors(m_pQuality, 100, SG_COLORS_WHITE_GREEN);
	}

	//-----------------------------------------------------
	return( true );
}


///////////////////////////////////////////////////////////
//														 //
//														 //
//														 //
///////////////////////////////////////////////////////////

//---------------------------------------------------------
inline double CGrid_Classify_Supervised::Get_Value(int x, int y, int iGrid)
{
	CSG_Grid	*pGrid	= m_pGrids->asGrid(iGrid);

	return( m_bNormalise
		? (pGrid->asDouble(x, y) - pGrid->Get_ArithMean()) / pGrid->Get_StdDev()
		:  pGrid->asDouble(x, y)
	);
}

//---------------------------------------------------------
inline bool CGrid_Classify_Supervised::Set_Class(int x, int y, int iClass, double Quality)
{
	if( is_InGrid(x, y) )
	{
		if( iClass >= 0 && iClass < m_Class_Info.Get_Count() )
		{
			m_pClasses->Set_Value(x, y, 1 + iClass);

			m_Class_Info.Inc_Element_Count(iClass);
		}

		if( m_pQuality )
		{
			m_pQuality->Set_Value(x, y, Quality);
		}

		return( true );
	}

	return( false );
}


///////////////////////////////////////////////////////////
//														 //
//														 //
//														 //
///////////////////////////////////////////////////////////

//---------------------------------------------------------
bool CGrid_Classify_Supervised::Set_Parallel_Epiped(void)
{
	for(int y=0; y<Get_NY() && Set_Progress(y); y++)
	{
		for(int x=0; x<Get_NX(); x++)
		{
			if( !m_pClasses->is_NoData(x, y) )
			{
				int		iMember	= -1, nMemberships	= 0;

				for(int iClass=0; iClass<m_Class_Info.Get_Count(); iClass++)
				{
					bool	bMember	= true;

					for(int iGrid=0; bMember && iGrid<m_pGrids->Get_Count(); iGrid++)
					{
						double	d	= Get_Value(x, y, iGrid);
						
						if(	d < m_Class_Info[iClass][iGrid].Get_Minimum()
						||	d > m_Class_Info[iClass][iGrid].Get_Maximum() )
						{
							bMember	= false;
						}
					}

					if( bMember )
					{
						nMemberships++;
						iMember	= iClass;
					}
				}

				Set_Class(x, y, iMember, nMemberships);
			}
		}
	}

	return( Finalise() );
}


///////////////////////////////////////////////////////////
//														 //
///////////////////////////////////////////////////////////

//---------------------------------------------------------
bool CGrid_Classify_Supervised::Set_Minimum_Distance(void)
{
	double	Threshold	= SG_Get_Square(Parameters("THRESHOLD_MD" )->asDouble());

	for(int y=0; y<Get_NY() && Set_Progress(y); y++)
	{
		for(int x=0; x<Get_NX(); x++)
		{
			if( !m_pClasses->is_NoData(x, y) )
			{
				int		iMember	= -1;
				double	dMember	= -1.0;

				for(int iClass=0; iClass<m_Class_Info.Get_Count(); iClass++)
				{
					double	d	= 0.0;

					for(int iGrid=0; iGrid<m_pGrids->Get_Count(); iGrid++)
					{
						d	+= SG_Get_Square(Get_Value(x, y, iGrid) - m_Class_Info[iClass][iGrid].Get_Mean());
					}

					if( dMember > d || dMember < 0.0 )
					{
						dMember	= d;
						iMember	= iClass;
					}
				}

				Set_Class(x, y, Threshold <= 0.0 || dMember <= Threshold ? iMember : -1, sqrt(dMember));
			}
		}
	}

	return( Finalise() );
}


///////////////////////////////////////////////////////////
//														 //
///////////////////////////////////////////////////////////

//---------------------------------------------------------
bool CGrid_Classify_Supervised::Set_Mahalanobis_Distance(void)
{
	int			iClass;

	//-----------------------------------------------------
	CSG_Matrix	b(m_Class_Info.Get_Feature_Count(), m_Class_Info.Get_Count());
	CSG_Matrix	m(m_Class_Info.Get_Feature_Count(), m_Class_Info.Get_Count());

	for(iClass=0; iClass<m_Class_Info.Get_Count(); iClass++)
	{
		for(int iGrid=0; iGrid<m_pGrids->Get_Count(); iGrid++)
		{
			b[iClass][iGrid]	= 1.0 / (m_Class_Info[iClass][iGrid].Get_Variance());
			m[iClass][iGrid]	= m_Class_Info[iClass][iGrid].Get_Mean();
		}
	}

	//-----------------------------------------------------
	double	Threshold	= Parameters("THRESHOLD_MD")->asDouble() / 100.0;

	for(int y=0; y<Get_NY() && Set_Progress(y); y++)
	{
		for(int x=0; x<Get_NX(); x++)
		{
			if( !m_pClasses->is_NoData(x, y) )
			{
				int		iMember	= -1;
				double	dMember	= 0.0, dSum	= 0.0;

				for(iClass=0; iClass<m_Class_Info.Get_Count(); iClass++)
				{
					double	d	= 1.0;

					for(int iGrid=0; iGrid<m_pGrids->Get_Count(); iGrid++)
					{
						d	*= b[iClass][iGrid] * SG_Get_Square(Get_Value(x, y, iGrid) - m[iClass][iGrid]);
					}

					d	= pow(d, 1.0 / m_pGrids->Get_Count());

					if( dMember > d || iMember < 0 )
					{
						dMember	= d;
						iMember	= iClass;
					}
				}

				Set_Class(x, y, Threshold <= 0.0 || dMember <= Threshold ? iMember : -1, sqrt(dMember));
			}
		}
	}

	//-----------------------------------------------------
	return( Finalise() );
}


///////////////////////////////////////////////////////////
//														 //
///////////////////////////////////////////////////////////

//---------------------------------------------------------
bool CGrid_Classify_Supervised::Set_Maximum_Likelihood(void)
{
	int			iClass;

	//-----------------------------------------------------
	CSG_Matrix	a(m_Class_Info.Get_Feature_Count(), m_Class_Info.Get_Count());
	CSG_Matrix	b(m_Class_Info.Get_Feature_Count(), m_Class_Info.Get_Count());
	CSG_Matrix	m(m_Class_Info.Get_Feature_Count(), m_Class_Info.Get_Count());

	for(iClass=0; iClass<m_Class_Info.Get_Count(); iClass++)
	{
		for(int iGrid=0; iGrid<m_pGrids->Get_Count(); iGrid++)
		{
			a[iClass][iGrid]	=  1.0 / sqrt(m_Class_Info[iClass][iGrid].Get_Variance() * 2.0 * M_PI);
			b[iClass][iGrid]	= -1.0 /     (m_Class_Info[iClass][iGrid].Get_Variance() * 2.0);
			m[iClass][iGrid]	= m_Class_Info[iClass][iGrid].Get_Mean();
		}
	}

	//-----------------------------------------------------
	bool	bRelative	= Parameters("ML_RELATIVE" )->asBool() == 1;
	double	Threshold	= Parameters("THRESHOLD_ML")->asDouble() / 100.0;

	for(int y=0; y<Get_NY() && Set_Progress(y); y++)
	{
		for(int x=0; x<Get_NX(); x++)
		{
			if( !m_pClasses->is_NoData(x, y) )
			{
				int		iMember	= -1;
				double	dMember	= 0.0, dSum	= 0.0;

				for(iClass=0; iClass<m_Class_Info.Get_Count(); iClass++)
				{
					double	d	= 1.0;

					for(int iGrid=0; iGrid<m_pGrids->Get_Count(); iGrid++)
					{
						d	*= a[iClass][iGrid] * exp(b[iClass][iGrid] * SG_Get_Square(Get_Value(x, y, iGrid) - m[iClass][iGrid]));
					}

					dSum	+= (d	= pow(d, 1.0 / m_pGrids->Get_Count()));

					if( dMember < d )
					{
						dMember	= d;
						iMember	= iClass;
					}
				}

				if( bRelative )
				{
					dMember	/= dSum;
				}

				Set_Class(x, y, Threshold <= 0.0 || dMember >= Threshold ? iMember : -1, 100.0 * dMember);
			}
		}
	}

	//-----------------------------------------------------
	return( Finalise() );
}


///////////////////////////////////////////////////////////
//														 //
///////////////////////////////////////////////////////////

//---------------------------------------------------------
bool CGrid_Classify_Supervised::Set_Spectral_Angle_Mapping(void)
{
	int			iClass;

	//-----------------------------------------------------
	CSG_Vector	l(m_Class_Info.Get_Count());
	CSG_Matrix	m(m_Class_Info.Get_Feature_Count(), m_Class_Info.Get_Count());

	for(iClass=0; iClass<m_Class_Info.Get_Count(); iClass++)
	{
		double	d	= 0.0;

		for(int iGrid=0; iGrid<m_pGrids->Get_Count(); iGrid++)
		{
			m[iClass][iGrid]	= m_Class_Info[iClass][iGrid].Get_Mean();

			d	+= SG_Get_Square(m[iClass][iGrid]);
		}

		l[iClass]	= sqrt(d);
	}

	//-----------------------------------------------------
	double	Threshold	= Parameters("THRESHOLD_SAM")->asDouble() * M_DEG_TO_RAD;

	for(int y=0; y<Get_NY() && Set_Progress(y); y++)
	{
		for(int x=0; x<Get_NX(); x++)
		{
			if( !m_pClasses->is_NoData(x, y) )
			{
				int		iMember	= -1;
				double	dMember	= -1.0;

				for(iClass=0; iClass<m_Class_Info.Get_Count(); iClass++)
				{
					double	d	= 0.0;
					double	e	= 0.0;

					for(int iGrid=0; iGrid<m_pGrids->Get_Count(); iGrid++)
					{
						double	v	= Get_Value(x, y, iGrid);

						d	+= v * m[iClass][iGrid];
						e	+= v*v;
					}

					d	= acos(d / (sqrt(e) * l[iClass]));

					if( dMember > d || dMember < 0.0 )
					{
						dMember	= d;
						iMember	= iClass;
					}
				}

				Set_Class(x, y, Threshold <= 0.0 || dMember <= Threshold ? iMember : -1, M_RAD_TO_DEG * (dMember));
			}
		}
	}

	//-----------------------------------------------------
	return( Finalise() );
}


///////////////////////////////////////////////////////////
//														 //
//														 //
//														 //
///////////////////////////////////////////////////////////

//---------------------------------------------------------

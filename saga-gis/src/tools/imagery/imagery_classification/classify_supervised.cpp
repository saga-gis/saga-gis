
///////////////////////////////////////////////////////////
//                                                       //
//                         SAGA                          //
//                                                       //
//      System for Automated Geoscientific Analyses      //
//                                                       //
//                     Tool Library                      //
//                imagery_classification                 //
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
//                University of Goettingen               //
//                Goldschmidtstr. 5                      //
//                37077 Goettingen                       //
//                Germany                                //
//                                                       //
///////////////////////////////////////////////////////////

//---------------------------------------------------------
#include "classify_supervised.h"

//---------------------------------------------------------
//#define WITH_WTA


///////////////////////////////////////////////////////////
//														 //
//														 //
//														 //
///////////////////////////////////////////////////////////

//---------------------------------------------------------
CGrid_Classify_Supervised::CGrid_Classify_Supervised(void)
{
	Set_Name		(_TL("Supervised Image Classification"));

	Set_Author		("O.Conrad (c) 2005");

	Set_Description	(_TW(
		"Standard methods for supervised image classification, including "
		"minimum distance, maximum likelihood, spectral angle mapping. "
		"Classifiers can be trained by areas defined through shapes, "
		"samples supplied as table records, or statistics previously "
		"stored to file. "
	));

	//-----------------------------------------------------
	Parameters.Add_Grid_List("",
		"GRIDS"			, _TL("Features"),
		_TL(""),
		PARAMETER_INPUT, false
	);

	Parameters.Add_Bool("GRIDS",
		"NORMALISE"		, _TL("Normalize"),
		_TL(""),
		false
	);

	Parameters.Add_Bool("GRIDS",
		"RGB_COLORS"	, _TL("Colors from Features"),
		_TL("Use the first three features in list to obtain blue, green, red components for class colour in look-up table."),
		false
	)->Set_UseInCMD(false);

	Parameters.Add_Grid_System("",
		"GRID_SYSTEM"	, _TL("Grid System"),
		_TL("")
	);

	Parameters.Add_Grid("GRID_SYSTEM",
		"CLASSES"		, _TL("Classification"),
		_TL(""),
		PARAMETER_OUTPUT, true, SG_DATATYPE_Short
	);

	Parameters.Add_Grid("GRID_SYSTEM",
		"QUALITY"		, _TL("Quality"),
		_TL("Dependent on chosen method, these are distances or probabilities."),
		PARAMETER_OUTPUT_OPTIONAL
	);

	Parameters.Add_Table("",
		"CLASSES_LUT"	, _TL("Look-up Table"),
		_TL("A reference list of the grid values that have been assigned to the training classes."),
		PARAMETER_OUTPUT_OPTIONAL
	);

	//-----------------------------------------------------
	Parameters.Add_Choice("",
		"TRAIN_WITH"	, _TL("Training"),
		_TL(""),
		CSG_String::Format("%s|%s|%s",
			_TL("training areas"),
			_TL("training samples"),
			_TL("load from file")
		)
	);

	Parameters.Add_Shapes("TRAIN_WITH",
		"TRAINING"		, _TL("Training Areas"),
		_TL(""),
		PARAMETER_INPUT
	);

	Parameters.Add_Table_Field("TRAINING",
		"TRAINING_CLASS", _TL("Class Identifier"),
		_TL("")
	);

	Parameters.Add_Double("TRAINING",
		"TRAIN_BUFFER"	, _TL("Buffer Size"),
		_TL("For non-polygon type training areas, creates a buffer with a diameter of specified size."),
		1., 0., true
	);

	Parameters.Add_Table("TRAIN_WITH",
		"TRAIN_SAMPLES"	, _TL("Training Samples"),
		_TL("Provide a class identifier in the first field followed by sample data corresponding to the input feature grids."),
		PARAMETER_INPUT
	);

	Parameters.Add_FilePath("TRAIN_WITH",
		"FILE_LOAD"		, _TL("Load Statistics from File..."),
		_TL(""),
		CSG_String::Format("%s (*.xml)|*.xml|%s|*.*",
			_TL("XML Files"),
			_TL("All Files")
		)
	);

	Parameters.Add_FilePath("TRAIN_WITH",
		"FILE_SAVE"		, _TL("Save Statistics to File..."),
		_TL(""),
		CSG_String::Format("%s (*.xml)|*.xml|%s|*.*",
			_TL("XML Files"),
			_TL("All Files")
		), NULL, true
	);

	//-----------------------------------------------------
	CSG_String Methods;

	for(int i=0; i<SG_CLASSIFY_SUPERVISED_WTA; i++)
	{
		Methods	+= CSG_Classifier_Supervised::Get_Name_of_Method(i) + "|";
	}

#ifdef WITH_WTA
	Methods	+= CSG_Classifier_Supervised::Get_Name_of_Method(SG_CLASSIFY_SUPERVISED_WTA);
#endif

	Parameters.Add_Choice("",
		"METHOD"		, _TL("Method"),
		_TL(""),
		Methods, SG_CLASSIFY_SUPERVISED_MinimumDistance
	);

	Parameters.Add_Double("METHOD",
		"THRESHOLD_DIST", _TL("Distance Threshold"),
		_TL("Let pixel stay unclassified, if minimum euclidean or mahalanobis distance is greater than threshold."),
		0., 0., true
	);

	Parameters.Add_Double("METHOD",
		"THRESHOLD_ANGLE", _TL("Spectral Angle Threshold (Degree)"),
		_TL("Let pixel stay unclassified, if spectral angle distance is greater than threshold."),
		0., 0., true, 90., true
	);

	Parameters.Add_Double("METHOD",
		"THRESHOLD_PROB", _TL("Probability Threshold"),
		_TL("Let pixel stay unclassified, if maximum likelihood probability value is less than threshold."),
		0., 0., true, 100., true
	);

	Parameters.Add_Choice("METHOD",
		"RELATIVE_PROB"	, _TL("Probability Reference"),
		_TL(""),
		CSG_String::Format("%s|%s",
			_TL("absolute"),
			_TL("relative")
		), 1
	);

#ifdef WITH_WTA
	Parameters.Add_Node("METHOD",
		"WTA"			, _TL("Winner Takes All"),
		_TL("")
	);

	for(int i=0; i<SG_CLASSIFY_SUPERVISED_WTA; i++)
	{
		Parameters.Add_Bool("WTA", CSG_String::Format("WTA_%d", i), CSG_Classifier_Supervised::Get_Name_of_Method(i), _TL(""), false);
	}
#endif
}


///////////////////////////////////////////////////////////
//														 //
///////////////////////////////////////////////////////////

//---------------------------------------------------------
int CGrid_Classify_Supervised::On_Parameters_Enable(CSG_Parameters *pParameters, CSG_Parameter *pParameter)
{
	if( pParameter->Cmp_Identifier("GRID_SYSTEM") )
	{
		pParameters->Set_Enabled("CLASSES"        , pParameter->asGrid_System()->is_Valid());

		if( pParameter->asGrid_System()->is_Valid() )
		{
			pParameters->Set_Parameter("TRAIN_BUFFER", pParameter->asGrid_System()->Get_Cellsize());
		}
	}

	if( pParameter->Cmp_Identifier("GRIDS") )
	{
		pParameters->Set_Enabled("RGB_COLORS"     , pParameter->asGridList()->Get_Grid_Count() >= 3);
	}

	if( pParameter->Cmp_Identifier("TRAIN_WITH") )
	{
		pParameters->Set_Enabled("TRAINING"       , pParameter->asInt() == 0);
		pParameters->Set_Enabled("TRAIN_SAMPLES"  , pParameter->asInt() == 1);
		pParameters->Set_Enabled("FILE_LOAD"      , pParameter->asInt() == 2);
		pParameters->Set_Enabled("FILE_SAVE"      , pParameter->asInt() != 2);
	}

	if(	pParameter->Cmp_Identifier("TRAINING") )
	{
		pParameters->Set_Enabled("TRAIN_BUFFER"   , pParameter->asShapes() && pParameter->asShapes()->Get_Type() != SHAPE_TYPE_Polygon);
	}

	if(	pParameter->Cmp_Identifier("METHOD") )
	{
		pParameters->Set_Enabled("THRESHOLD_DIST" , pParameter->asInt() == SG_CLASSIFY_SUPERVISED_MinimumDistance
			||                                      pParameter->asInt() == SG_CLASSIFY_SUPERVISED_Mahalonobis      );
		pParameters->Set_Enabled("THRESHOLD_PROB" , pParameter->asInt() == SG_CLASSIFY_SUPERVISED_MaximumLikelihood);
		pParameters->Set_Enabled("RELATIVE_PROB"  , pParameter->asInt() == SG_CLASSIFY_SUPERVISED_MaximumLikelihood);
		pParameters->Set_Enabled("THRESHOLD_ANGLE", pParameter->asInt() == SG_CLASSIFY_SUPERVISED_SAM              );
#ifdef WITH_WTA
		pParameters->Set_Enabled("WTA"            , pParameter->asInt() == SG_CLASSIFY_SUPERVISED_WTA              );
#endif
	}

	return( CSG_Tool::On_Parameters_Enable(pParameters, pParameter) );
}


///////////////////////////////////////////////////////////
//														 //
///////////////////////////////////////////////////////////

//---------------------------------------------------------
bool CGrid_Classify_Supervised::On_Execute(void)
{
	m_pFeatures = Parameters("GRIDS")->asGridList();

	if( m_pFeatures->Get_Grid_Count() < 1 || !m_pFeatures->Get_Grid(0)->Get_System().is_Valid() )
	{
		Error_Set(_TL("invalid features"));

		return( false );
	}

	m_bNormalize = Parameters("NORMALISE")->asBool();

	//-----------------------------------------------------
	m_System.Create(*Parameters("GRID_SYSTEM")->asGrid_System());

	if( !m_System.is_Valid() )
	{
		m_System.Create(m_pFeatures->Get_Grid(0)->Get_System());
	}

	//-----------------------------------------------------
	CSG_Grid *pClasses = Parameters("CLASSES")->asGrid();

	if( pClasses == NULL )
	{
		Parameters("CLASSES")->Set_Value(pClasses = SG_Create_Grid(m_System, SG_DATATYPE_Short));
	}
	else if( !pClasses->Get_System().is_Equal(m_System) )
	{
		pClasses->Create(m_System, SG_DATATYPE_Short);
	}

	pClasses->Set_NoData_Value(-1); pClasses->Assign_NoData();

	//-----------------------------------------------------
	CSG_Grid *pQuality = (CSG_Grid *)Parameters("QUALITY")->asPointer();

	if( pQuality == DATAOBJECT_CREATE )
	{
		Parameters("QUALITY")->Set_Value(pQuality = SG_Create_Grid(m_System));
	}
	else if( pQuality && !pQuality->Get_System().is_Equal(m_System) )
	{
		pQuality->Create(m_System);
	}

	//-----------------------------------------------------
	Process_Set_Text(_TL("training"));

	CSG_Classifier_Supervised Classifier;

	if( !Set_Classifier(Classifier) )
	{
		Error_Set(_TL("failed to train classifier"));

		return( false );
	}

	Message_Add(Classifier.Print(), false);

	//-----------------------------------------------------
	Process_Set_Text(_TL("prediction"));

	int Method = Parameters("METHOD")->asInt();

	for(int y=0; y<m_System.Get_NY() && Set_Progress(y, m_System.Get_NY()); y++)
	{
		#pragma omp parallel for
		for(int x=0; x<m_System.Get_NX(); x++)
		{
			int Class; double Quality; CSG_Vector Features(m_pFeatures->Get_Grid_Count());

			if( Get_Features(x, y, Features) )
			{
				if( Classifier.Get_Class(Features, Class, Quality, Method) )
				{
					pClasses->Set_Value(x, y, Class);
				}
				else
				{
					pClasses->Set_NoData(x, y);
				}

				if( pQuality ) { pQuality->Set_Value(x, y, Quality); }
			}
			else
			{
				pClasses->Set_NoData(x, y);

				if( pQuality ) { pQuality->Set_NoData(x, y); }
			}
		}
	}

	//-----------------------------------------------------
	return( Set_Classification(Classifier) );
}


///////////////////////////////////////////////////////////
//														 //
///////////////////////////////////////////////////////////

//---------------------------------------------------------
bool CGrid_Classify_Supervised::Get_Features(int x, int y, CSG_Vector &Features)
{
	for(int i=0; i<m_pFeatures->Get_Grid_Count(); i++)
	{
		CSG_Grid *pFeature = m_pFeatures->Get_Grid(i);

		if( pFeature->Get_System().is_Equal(m_System) )
		{
			if( pFeature->is_NoData(x, y) )
			{
				return( false );
			}

			Features[i] = pFeature->asDouble(x, y);
		}
		else if( pFeature->Get_Value(m_System.Get_Grid_to_World(x, y), Features[i]) == false )
		{
			return( false );
		}

		if( m_bNormalize && pFeature->Get_StdDev() > 0. )
		{
			Features[i] = (Features[i] - pFeature->Get_Mean()) / pFeature->Get_StdDev();
		}
	}

	return( true );
}


///////////////////////////////////////////////////////////
//														 //
///////////////////////////////////////////////////////////

//---------------------------------------------------------
bool CGrid_Classify_Supervised::Set_Classifier(CSG_Classifier_Supervised &Classifier)
{
	Classifier.Create(m_pFeatures->Get_Grid_Count());

	Classifier.Set_Threshold_Distance   (Parameters("THRESHOLD_DIST" )->asDouble());
	Classifier.Set_Threshold_Angle      (Parameters("THRESHOLD_ANGLE")->asDouble());
	Classifier.Set_Threshold_Probability(Parameters("THRESHOLD_PROB" )->asDouble());
	Classifier.Set_Probability_Relative (Parameters("RELATIVE_PROB"  )->asBool  ());

#ifdef WITH_WTA
	for(int i=0; i<SG_CLASSIFY_SUPERVISED_WTA; i++)
	{
		Classifier.Set_WTA(i, Parameters(CSG_String::Format("WTA_%d", i))->asBool());
	}
#endif

	//-----------------------------------------------------
	switch( Parameters("TRAIN_WITH")->asInt() )
	{
	default: return( false );

	case  2: // statistics from file
		return( Classifier.Load(Parameters("FILE_LOAD")->asString()) );

	case  1: // training samples
		if( !Set_Classifier(Classifier, Parameters("TRAIN_SAMPLES")->asTable()) )
		{
			return( false );
		}
		break;

	case  0: // training areas
		if( !Set_Classifier(Classifier, Parameters("TRAINING")->asShapes(), Parameters("TRAINING_CLASS")->asInt()) )
		{
			return( false );
		}
		break;
	}

	if( Classifier.Train(true) )
	{
		Classifier.Save(Parameters("FILE_SAVE")->asString());

		return( true );
	}

	return( false );
}

//---------------------------------------------------------
bool CGrid_Classify_Supervised::Set_Classifier(CSG_Classifier_Supervised &Classifier, CSG_Table *pSamples)
{
	if( pSamples->Get_Field_Count() < m_pFeatures->Get_Grid_Count() + 1 )
	{
		Error_Set(_TL("Training samples have to provide a class identifier in the first field followed by a value for each feature."));

		return( false );
	}

	int Field = 0; CSG_Index Index; pSamples->Set_Index(Index, Field);

	for(sLong i=0; i<pSamples->Get_Count() && Set_Progress(i, pSamples->Get_Count()); i++)
	{
		CSG_Table_Record *pSample = pSamples->Get_Record(Index[i]);

		CSG_Vector Features(m_pFeatures->Get_Grid_Count());

		for(int i=0; i<m_pFeatures->Get_Grid_Count(); i++)
		{
			double Feature = pSample->asDouble(i + 1);

			if( m_bNormalize )
			{
				CSG_Grid *pFeature = m_pFeatures->Get_Grid(i);

				Feature = (Feature - pFeature->Get_Mean()) / pFeature->Get_StdDev();
			}

			Features[i] = Feature;
		}

		Classifier.Train_Add_Sample(pSample->asString(Field), Features);
	}

	return( true );
}

//---------------------------------------------------------
bool CGrid_Classify_Supervised::Set_Classifier(CSG_Classifier_Supervised &Classifier, CSG_Shapes *pPolygons, int Field)
{
	CSG_Shapes Polygons;

	if( pPolygons->Get_Type() != SHAPE_TYPE_Polygon )
	{
		double Buffer = Parameters("TRAIN_BUFFER")->asDouble() / 2.;	// diameter, not radius!

		if( Buffer <= 0. )
		{
			Error_Set(_TL("buffer size must not be zero"));

			return( false );
		}

		Polygons.Create(SHAPE_TYPE_Polygon); Polygons.Add_Field(pPolygons->Get_Field_Name(Field), pPolygons->Get_Field_Type(Field));

		for(sLong i=0; i<pPolygons->Get_Count(); i++)
		{
			CSG_Shape *pShape = pPolygons->Get_Shape(i), *pBuffer = Polygons.Add_Shape();

			*pBuffer->Get_Value(0) = *pShape->Get_Value(Field);

			SG_Shape_Get_Offset(pShape, Buffer, 5 * M_DEG_TO_RAD, pBuffer);
		}

		pPolygons = &Polygons; Field = 0;
	}

	//-----------------------------------------------------
	CSG_Index Index; pPolygons->Set_Index(Index, Field);

	for(sLong i=0; i<pPolygons->Get_Count() && Set_Progress(i, pPolygons->Get_Count()); i++)
	{
		Set_Classifier(Classifier, pPolygons->Get_Shape(Index[i])->asPolygon(), Field);
	}

	return( true );
}

//---------------------------------------------------------
bool CGrid_Classify_Supervised::Set_Classifier(CSG_Classifier_Supervised &Classifier, CSG_Shape_Polygon *pPolygon, int Field)
{
	CSG_Vector Features(m_pFeatures->Get_Grid_Count());

	for(int i=0; i<pPolygon->Get_Part_Count(); i++)
	{
		CSG_Shape_Polygon_Part *pPart = pPolygon->Get_Polygon_Part(i);

		if( pPart->Get_Extent().Intersects(m_System.Get_Extent()) )
		{
			int xMin = m_System.Get_xWorld_to_Grid(pPart->Get_Extent().Get_XMin()); if( xMin <  0                 ) { xMin = 0                    ; }
			int xMax = m_System.Get_xWorld_to_Grid(pPart->Get_Extent().Get_XMax()); if( xMax >= m_System.Get_NX() ) { xMax = m_System.Get_NX() - 1; }
			int yMin = m_System.Get_yWorld_to_Grid(pPart->Get_Extent().Get_YMin()); if( yMin <  0                 ) { yMin = 0                    ; }
			int yMax = m_System.Get_yWorld_to_Grid(pPart->Get_Extent().Get_YMax()); if( yMax >= m_System.Get_NY() ) { yMax = m_System.Get_NY() - 1; }

			for(int y=yMin; y<=yMax; y++) for(int x=xMin; x<=xMax; x++)
			{
				if( pPart->Contains(m_System.Get_Grid_to_World(x, y)) && Get_Features(x, y, Features) )
				{
					Classifier.Train_Add_Sample(pPolygon->asString(Field), Features);
				}
			}
		}
	}

	return( true );
}


///////////////////////////////////////////////////////////
//														 //
///////////////////////////////////////////////////////////

//---------------------------------------------------------
bool CGrid_Classify_Supervised::Set_Classification(CSG_Classifier_Supervised &Classifier)
{
	CSG_Grid *pClasses = Parameters("CLASSES")->asGrid(); DataObject_Add(pClasses);

	CSG_Parameter *pLUT = DataObject_Get_Parameter(pClasses, "LUT");

	if( pLUT && pLUT->asTable() )
	{
		CSG_Table &LUT = *pLUT->asTable(); CSG_Colors Colors(Classifier.Get_Class_Count()); Colors.Random();

		for(int i=0; i<Classifier.Get_Class_Count(); i++)
		{
			if( m_pFeatures->Get_Grid_Count() >= 3 && Parameters("RGB_COLORS")->asBool() )
			{
				#define SET_COLOR_COMPONENT(c, b) { CSG_Grid *f = m_pFeatures->Get_Grid(b); c = (int)(127 + (Classifier.Get_Class_Mean(i, b) - f->Get_Mean()) * 127 / f->Get_StdDev()); if( c < 0 ) c = 0; else if( c > 255 ) c = 255; }

				int	r; SET_COLOR_COMPONENT(r, 2);
				int	g; SET_COLOR_COMPONENT(g, 1);
				int	b; SET_COLOR_COMPONENT(b, 0);

				Colors.Set_Color(i, SG_GET_RGB(r, g, b));
			}
			else
			{
				CSG_Table_Record *pClass = LUT.Find_Record(1, Classifier.Get_Class_ID(i));

				if( pClass )
				{
					Colors.Set_Color(i, pClass->asInt(0));
				}
			}
		}

		//-------------------------------------------------
		LUT.Set_Count(Classifier.Get_Class_Count());

		for(int i=0; i<Classifier.Get_Class_Count(); i++)
		{
			CSG_Table_Record &Class = *LUT.Get_Record(i);

			Class.Set_Value(0, Colors[i]);
			Class.Set_Value(1, Classifier.Get_Class_ID(i).c_str());
			Class.Set_Value(2, "");
			Class.Set_Value(3, i);
			Class.Set_Value(4, i);
		}

		DataObject_Set_Parameter(pClasses, pLUT);
		DataObject_Set_Parameter(pClasses, "COLORS_TYPE", 1);	// Color Classification Type: Lookup Table
	}

	pClasses->Fmt_Name("%s [%s]", _TL("Classification"), CSG_Classifier_Supervised::Get_Name_of_Method(Parameters("METHOD")->asInt()).c_str());

	//-----------------------------------------------------
	if( Parameters("CLASSES_LUT")->asTable() )
	{
		CSG_Table &LUT = *Parameters("CLASSES_LUT")->asTable();

		LUT.Destroy();
		LUT.Set_Name(pClasses->Get_Name());
		LUT.Add_Field("VALUE", pClasses->Get_Type());
		LUT.Add_Field("CLASS", SG_DATATYPE_String);

		for(int i=0; i<Classifier.Get_Class_Count(); i++)
		{
			CSG_Table_Record &Class = *LUT.Add_Record();

			Class.Set_Value(0, i);
			Class.Set_Value(1, Classifier.Get_Class_ID(i).c_str());
		}
	}

	//-----------------------------------------------------
	CSG_Grid *pQuality = Parameters("QUALITY")->asGrid(); DataObject_Add(pQuality);

	if( pQuality )
	{
		DataObject_Set_Colors(pQuality, 11, SG_COLORS_YELLOW_GREEN, true);

		pQuality->Fmt_Name("%s [%s]", _TL("Classification Quality"), CSG_Classifier_Supervised::Get_Name_of_Quality(Parameters("METHOD")->asInt()).c_str());
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

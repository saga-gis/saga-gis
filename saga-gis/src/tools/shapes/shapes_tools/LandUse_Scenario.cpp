
///////////////////////////////////////////////////////////
//                                                       //
//                         SAGA                          //
//                                                       //
//      System for Automated Geoscientific Analyses      //
//                                                       //
//                     Tool Library                      //
//                      statistics                       //
//                                                       //
//-------------------------------------------------------//
//                                                       //
//                 LandUse_Scenario.cpp                  //
//                                                       //
//                 Copyright (C) 2015 by                 //
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
#include "LandUse_Scenario.h"


///////////////////////////////////////////////////////////
//														 //
//														 //
//														 //
///////////////////////////////////////////////////////////

//---------------------------------------------------------
#define STATISTICS_HEADCOLS	3


///////////////////////////////////////////////////////////
//														 //
//														 //
//														 //
///////////////////////////////////////////////////////////

//---------------------------------------------------------
CLandUse_Scenario::CLandUse_Scenario(void)
{
	Set_Name		(_TL("Land Use Scenario Generator"));

	Set_Author		("O.Conrad (c) 2015");

	Set_Description	(_TW(
		"This tool generates land use scenarios for fields under "
		"agricultural use based on statistics about the amount of "
		"crop types grown in the investigated area of interest. "
	));

	//-----------------------------------------------------
	Parameters.Add_Shapes("",
		"FIELDS"     , _TL("Fields"           ), _TL(""),
		PARAMETER_INPUT, SHAPE_TYPE_Polygon
	);

	Parameters.Add_Table_Field("FIELDS",
		"FIELD_ID"   , _TL("Field Identifier" ), _TL("")
	);

	Parameters.Add_Shapes("",
		"SCENARIO"   , _TL("Land Use Scenario"), _TL(""),
		PARAMETER_OUTPUT, SHAPE_TYPE_Polygon
	);

	Parameters.Add_Choice("SCENARIO",
		"OUTPUT"     , _TL("Output of..."     ), _TL(""), CSG_String::Format("%s|%s|",
			_TL("Identifier"),
			_TL("Name")
		), 0
	);

	//-----------------------------------------------------
	Parameters.Add_Table("",
		"STATISTICS" , _TL("Crop Statistics"  ),
		_TW("The first column specifies a crop type id. "
			"The second column provides a human readable name for the crop type (e.g. 'potatoes') ."
			"The third column must be an integer value, though this value is not yet used by this tool. "
			"The following columns provide the yearly amount of each crop type [%] for a sequence of years. "),
		PARAMETER_INPUT
	);

	Parameters.Add_Table("",
		"KNOWN_CROPS", _TL("Known Crops"      ),
		_TW("The first column specifies the field id as given by the 'Fields' layer. "
			"The following columns specify the crop type for each field and year "
			"and refer to the crop type identifiers used in the crop statistics table. "
			"The sequence of years must be identical with that of the crop statistics table."),
		PARAMETER_INPUT_OPTIONAL
	);
}


///////////////////////////////////////////////////////////
//														 //
///////////////////////////////////////////////////////////

//---------------------------------------------------------
bool CLandUse_Scenario::On_Execute(void)
{
	CSG_Table *pStatistics = Parameters("STATISTICS")->asTable();

	if( pStatistics->Get_Field_Count() <= STATISTICS_HEADCOLS )
	{
		Error_Set(_TL("statistics table is missing required fields"));

		return( false );
	}

	if( pStatistics->Get_Count() <= 0 )
	{
		Error_Set(_TL("statistics table does not contain any records"));

		return( false );
	}

	//-----------------------------------------------------
	CSG_Shapes *pFields = Parameters("FIELDS")->asShapes();

	int Field_ID = Parameters("FIELD_ID")->asInt();

	if( pFields->Get_Count() <= 0 )
	{
		Error_Set(_TL("fields layer does not contain any records"));

		return( false );
	}

	//-----------------------------------------------------
	CSG_Table Types;

	Types.Add_Field("ID"    , SG_DATATYPE_Int);
	Types.Add_Field("NAME"  , SG_DATATYPE_String);
	Types.Add_Field("RETURN", SG_DATATYPE_Int);

	Types.Set_Count(pStatistics->Get_Count());

	int nYears = pStatistics->Get_Field_Count() - STATISTICS_HEADCOLS;

	CSG_Matrix	Crops(nYears, Types.Get_Count());

	for(int iType=0; iType<Types.Get_Count(); iType++)
	{
		CSG_Table_Record	*pRecord	= pStatistics->Get_Record(iType);

		Types[iType][0]	= pRecord->asInt   (0);
		Types[iType][1]	= pRecord->asString(1);
		Types[iType][2]	= pRecord->asInt   (2);

		for(int iYear=0; iYear<nYears; iYear++)
		{
			Crops[iType][iYear]	= pRecord->asDouble(iYear + STATISTICS_HEADCOLS);
		}
	}

	//-----------------------------------------------------
	CSG_Table Scenario;

	for(int iYear=0; iYear<nYears; iYear++)
	{
		Scenario.Add_Field(pStatistics->Get_Field_Name(iYear + STATISTICS_HEADCOLS), SG_DATATYPE_Int);
	}

	Scenario.Add_Field("ID"  , SG_DATATYPE_Int);
	Scenario.Add_Field("AREA", SG_DATATYPE_Double);

	Scenario.Set_Count(pFields->Get_Count());

	for(int iField=0; iField<pFields->Get_Count(); iField++)
	{
		CSG_Shape_Polygon *pField = pFields->Get_Shape(iField)->asPolygon();

		Scenario[iField][nYears + 0] = pField->asInt(Field_ID);
		Scenario[iField][nYears + 1] = pField->Get_Area();

		for(int iYear=0; iYear<nYears; iYear++)
		{
			Scenario[iField][iYear]	= -1;
		}
	}

	//-----------------------------------------------------
	Get_Known_LandUse(nYears, Scenario, Types);
	Get_Scenario     (nYears, Scenario, Crops);

	//-----------------------------------------------------
	CSG_Shapes *pScenario = Parameters("SCENARIO")->asShapes();

	bool bID = Parameters("OUTPUT")->asInt() == 0;

	pScenario->Create(SHAPE_TYPE_Polygon, CSG_String::Format("%s [%s]", pFields->Get_Name(), _TL("Land Use Scenario")));

	pScenario->Add_Field("ID", SG_DATATYPE_Int);

	for(int iYear=0; iYear<nYears; iYear++)
	{
		pScenario->Add_Field(pStatistics->Get_Field_Name(iYear + STATISTICS_HEADCOLS), bID ? SG_DATATYPE_Int : SG_DATATYPE_String);
	}

	//-----------------------------------------------------
	for(int iField=0; iField<pFields->Get_Count(); iField++)
	{
		CSG_Shape *pField = pScenario->Add_Shape(pFields->Get_Shape(iField), SHAPE_COPY_GEOM);

		pField->Set_Value(0, pFields->Get_Shape(iField)->asInt(Field_ID));

		for(int iYear=0; iYear<nYears; iYear++)
		{
			(*pField)[iYear + 1] = Types[Scenario[iField][iYear].asInt()][bID ? 0 : 1];
		}
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
bool CLandUse_Scenario::Get_Known_LandUse(int nYears, CSG_Table &Scenario, const CSG_Table &Types)
{
	CSG_Table *pLandUse_Known = Parameters("KNOWN_CROPS")->asTable();

	if( !pLandUse_Known || pLandUse_Known->Get_Field_Count() != nYears + 1 )
	{
		return( true );
	}

	for(sLong iKnown=0; iKnown<pLandUse_Known->Get_Count(); iKnown++)
	{
		CSG_Table_Record *pKnown = pLandUse_Known->Get_Record(iKnown);

		for(int iField=0, Field_ID=pKnown->asInt(0); iField<Scenario.Get_Count(); iField++)
		{
			if( Field_ID == Scenario[iField][nYears + 0].asInt() )
			{
				for(int iYear=0; iYear<nYears; iYear++)
				{
					int	Type_ID	= pKnown->asInt(iYear + 1);

					for(int iType=0; iType<Types.Get_Count(); iType++)
					{
						if( Type_ID == Types[iType][0].asInt() )
						{
							Scenario[iField][iYear]	= iType;

							break;
						}
					}
				}

				break;
			}
		}
	}

	return( true );
}


///////////////////////////////////////////////////////////
//														 //
///////////////////////////////////////////////////////////

//---------------------------------------------------------
bool CLandUse_Scenario::Get_Scenario(int nYears, CSG_Table &Scenario, const CSG_Matrix &Crops)
{
	double Sum_Area = 0.;

	for(int iField=0; iField<Scenario.Get_Count(); iField++)
	{
		Sum_Area += Scenario[iField][nYears + 1];
	}

	if( Sum_Area <= 0. )
	{
		return( false );
	}

	CSG_Vector Area_Crop(Crops.Get_NRows());

	//-----------------------------------------------------
	for(int iYear=0; iYear<nYears; iYear++)
	{
		//-------------------------------------------------
		// 1. determine percentage and absolute area for each crop type...

		double Sum_Crops = 0.;

		for(int iType=0; iType<Crops.Get_NRows(); iType++)
		{
			Sum_Crops += Crops[iType][iYear];
		}

		if( Sum_Crops <= 0. )
		{
			continue;
		}

		for(int iType=0; iType<Crops.Get_NRows(); iType++)
		{
			Area_Crop[iType] = Crops[iType][iYear] * Sum_Area / Sum_Crops;
		}

		//-------------------------------------------------
		// 2. remove known field crops...

		double Area_Left = Sum_Area;

		for(int iField=0; iField<Scenario.Get_Count(); iField++)
		{
			if( Scenario[iField][iYear] >= 0 )
			{
				int iType         = Scenario[iField][iYear].asInt();
				Area_Left        -= Scenario[iField][nYears + 1];
				Area_Crop[iType] -= Scenario[iField][nYears + 1];
			}
		}

		//-------------------------------------------------
		// 3. fill remaining gaps with randomized scenario...

		for(int iField=0; iField<Scenario.Get_Count(); iField++)
		{
			if( Scenario[iField][iYear].asInt() < 0 )
			{
				//-----------------------------------------
				// (a) select a crop type...

				double pArea = 0., pCrop = CSG_Random::Get_Uniform(0.0, Area_Left); int Type = -1;

				for(int iType=0; Type<0 && iType<Crops.Get_NRows(); iType++)
				{
					pArea += Area_Crop[iType];

					if( pCrop - pArea < 0.000001 )	// if( pCrop < pArea ) pech mit fließkommagenauigkeit...
					{
						Type = iType;
					}
				}

				//-----------------------------------------
				// (b) Set the Crop...

				if( Type >= 0 )
				{
					Area_Left       -= Scenario[iField][nYears + 1];
					Area_Crop[Type] -= Scenario[iField][nYears + 1];

					Scenario[iField][iYear] = Type;
				}
			}
		}
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

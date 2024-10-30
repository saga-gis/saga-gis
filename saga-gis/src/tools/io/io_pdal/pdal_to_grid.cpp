
///////////////////////////////////////////////////////////
//                                                       //
//                         SAGA                          //
//                                                       //
//      System for Automated Geoscientific Analyses      //
//                                                       //
//                     Tool Library                      //
//                       io_pdal                         //
//                                                       //
//-------------------------------------------------------//
//                                                       //
//                   pdal_to_grid.cpp                    //
//                                                       //
//                  Copyrights (c) 2024                  //
//                     Olaf Conrad                       //
//                   Volker Wichmann                     //
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
#include "pdal_to_grid.h"

#include "pdal_driver.h"

#include <pdal/Options.hpp>
#include <pdal/PointTable.hpp>
#include <pdal/PointLayout.hpp>
#include <pdal/StageFactory.hpp>
#include <pdal/filters/StreamCallbackFilter.hpp>


///////////////////////////////////////////////////////////
//                                                       //
//                                                       //
//                                                       //
///////////////////////////////////////////////////////////

//---------------------------------------------------------
CPDAL_to_Grid::CPDAL_to_Grid(void)
{
    Set_Name	(_TL("Import Grid from Point Cloud"));

    Set_Author	("O.Conrad, V.Wichmann (c) 2024");

	Add_Reference("https://pdal.io/"                , SG_T("PDAL Homepage"          ));
	Add_Reference("https://github.com/ASPRSorg/LAS/", SG_T("ASPRS LAS Specification"));

	CSG_String Description;

	Description = _TW(
		"This tool directly creates a grid from point cloud elevation data and supports "
		"several file formats using the \"Point Data Abstraction Library\" (PDAL).\n"
	);

	Description += CSG_String::Format("\nPDAL %s\n", SG_Get_PDAL_Drivers().Get_Version().c_str());

	Description += CSG_String::Format("\n%s:\n", _TL("Supported point cloud formats"));

	Description += SG_Get_PDAL_Drivers().Get_Description(true);

	Set_Description(Description);

	//-----------------------------------------------------
	Parameters.Add_FilePath("",
		"FILES"        , _TL("Files"),
		_TL(""),
		SG_Get_PDAL_Drivers().Get_Filter(true), NULL, false, false, true
	);

	Parameters.Add_Bool("",
		"STREAM"       , _TL("Open Streamable"),
		_TL("Use streamable file access if supported by driver. Reduces memory requirements."),
		true
	);

	Parameters.Add_String("",
		"CLASSES"      , _TL("Classes"),
		_TL("If classification is available only points are loaded whose identifiers are listed (comma separated, e.g. \"2, 3, 4, 5, 6\"). Ignored if empty."),
		""
	);

	Parameters.Add_Choice("",
		"AGGREGATION"  , _TL("Value Aggregation"),
		_TL("Output generated when more than one point falls into one target cell."),
		CSG_String::Format("%s|%s|%s|%s|%s",
			_TL("first"),
			_TL("last"),
			_TL("minimum"),
			_TL("maximum"),
			_TL("mean")
		), 1
	);

	//-----------------------------------------------------
	m_Grid_Target.Create(&Parameters, false, "", "TARGET_");

	m_Grid_Target.Add_Grid("GRID" , _TL("Grid"            ), false);
	m_Grid_Target.Add_Grid("COUNT", _TL("Number of Values"),  true);
}


///////////////////////////////////////////////////////////
//                                                       //
///////////////////////////////////////////////////////////

//---------------------------------------------------------
int CPDAL_to_Grid::On_Parameter_Changed(CSG_Parameters *pParameters, CSG_Parameter *pParameter)
{
	if(	pParameter->Cmp_Identifier("FILES") && (*pParameters)("TARGET_DEFINITION")->asInt() == 0 ) // user defined!
	{
		CSG_Strings Files; CSG_Rect Extent;

		if( pParameter->asFilePath()->Get_FilePaths(Files) && _Get_Extent(Files, Extent, (*pParameters)("STREAM")->asBool()) )
		{
			m_Grid_Target.Set_User_Defined(pParameters, Extent);
		}
	}

	m_Grid_Target.On_Parameter_Changed(pParameters, pParameter);

	return( CSG_Tool::On_Parameter_Changed(pParameters, pParameter) );
}

//---------------------------------------------------------
int CPDAL_to_Grid::On_Parameters_Enable(CSG_Parameters *pParameters, CSG_Parameter *pParameter)
{
	m_Grid_Target.On_Parameters_Enable(pParameters, pParameter);

	return( CSG_Tool::On_Parameters_Enable(pParameters, pParameter) );
}


///////////////////////////////////////////////////////////
//                                                       //
///////////////////////////////////////////////////////////

//---------------------------------------------------------
bool CPDAL_to_Grid::On_Execute(void)
{
    CSG_Strings Files;

    if( !Parameters("FILES")->asFilePath()->Get_FilePaths(Files) )
    {
        Error_Set(_TL("Empty file list."));

        return( false );
    }

	//-----------------------------------------------------
	CSG_Array_Int Classes;

	if( *Parameters("CLASSES")->asString() ) // not empty
	{
		CSG_Array_Int IDs; CSG_Strings List(SG_String_Tokenize(Parameters("CLASSES")->asString(), ",;"));

		for(int i=0, ID; i<List.Get_Count(); i++)
		{
			if( List[i].asInt(ID) )
			{
				IDs += ID;
			}
		}

		CSG_Index Index(IDs.Get_Size(), IDs.Get_Array()); // sort for better performance

		for(sLong i=0; i<IDs.Get_Size(); i++)
		{
			Classes += IDs[Index[i]];
		}
	}

	//-----------------------------------------------------
	m_pGrid = m_Grid_Target.Get_Grid("GRID");

	if( m_pGrid == NULL )
	{
		Error_Set(_TL("Failed to request target grid."));

		return( false );
	}

	//-----------------------------------------------------
	//CSG_Rect Extent;

	//if( !_Get_Extent(Files, Extent) )
	//{
	//	Error_Set(_TL("Failed to retrieve point cloud extents."));

	//	return( false );
	//}

	//if( !Extent.Intersects(m_pGrid->Get_Extent()) )
	//{
	//	Error_Set(_TL("Point cloud and target grid extents do not intersect."));

	//	return( false );
	//}

	//-----------------------------------------------------
	CSG_String Name(SG_File_Get_Name(Files[0], false));

	m_pGrid->Set_Name(Name);
	m_pGrid->Assign_NoData();

	//-------------------------------------------------
	m_pCount = m_Grid_Target.Get_Grid("COUNT", SG_DATATYPE_Word); CSG_Grid Count;

	if( m_pCount == NULL )
	{
		m_pCount = &Count; Count.Create(m_pGrid->Get_System(), SG_DATATYPE_Word);
	}
	else
	{
		DataObject_Add(m_pCount);
	}

	m_pCount->Fmt_Name("%s [%s]", Name.c_str(), _TL("Count"));
	m_pCount->Set_NoData_Value(0.);
	m_pCount->Assign(0.);

	//-----------------------------------------------------
	m_Aggregation = Parameters("AGGREGATION")->asInt();

	bool bStream = Parameters("STREAM")->asBool();

	//-----------------------------------------------------
    for(int i=0; i<Files.Get_Count() && Process_Get_Okay(); i++)
    {
        Process_Set_Text("[%d/%d] %s: %s", i + 1, Files.Get_Count(), _TL("File"), SG_File_Get_Name(Files[i], true).c_str());

		_Read_Points(Files[i], Classes, bStream);
    }

	//-----------------------------------------------------
	if( m_Aggregation == 4 ) // mean
	{
		for(int y=0; y<m_pGrid->Get_NY() && Set_Progress(y, m_pGrid->Get_NY()); y++)
		{
			#pragma omp parallel for
			for(int x=0; x<m_pGrid->Get_NX(); x++)
			{
				if( m_pCount->asInt(x, y) > 1 )
				{
					m_pGrid->Mul_Value(x, y, 1. / m_pCount->asDouble(x, y));
				}
			}
		}
	}

	//-----------------------------------------------------
	DataObject_Update(m_pGrid); DataObject_Update(m_pCount);

	return( true );
}


///////////////////////////////////////////////////////////
//                                                       //
///////////////////////////////////////////////////////////

//---------------------------------------------------------
bool CPDAL_to_Grid::_Find_Class(const CSG_Array_Int &Classes, int ID)
{
	for(sLong i=0; i<Classes.Get_Size(); i++)
	{
		if( ID  < Classes[i] ) // list is sorted ascending!
		{
			return( false );
		}

		if( ID == Classes[i] )
		{
			return( true );
		}
	}

	return( false );
}


///////////////////////////////////////////////////////////
//														 //
///////////////////////////////////////////////////////////

//---------------------------------------------------------
inline void CPDAL_to_Grid::_Add_Point(double _x, double _y, double z)
{
	int x = m_pGrid->Get_System().Get_xWorld_to_Grid(_x);
	int y = m_pGrid->Get_System().Get_yWorld_to_Grid(_y);

	if( m_pGrid->is_InGrid(x, y, false) )
	{
		if( m_pCount->asInt(x, y) == 0 )
		{
			m_pGrid->Set_Value(x, y, z);
		}
		else switch( m_Aggregation )
		{
		default:	// first
			break;

		case  1:	// last
			m_pGrid->Set_Value(x, y, z);
			break;

		case  2:	// minimum
			if( m_pGrid->asDouble(x, y) > z )
			{
				m_pGrid->Set_Value(x, y, z);
			}
			break;

		case  3:	// maximum
			if( m_pGrid->asDouble(x, y) < z )
			{
				m_pGrid->Set_Value(x, y, z);
			}
			break;

		case  4:	// mean
			m_pGrid->Add_Value(x, y, z);
			break;
		}

		m_pCount->Add_Value(x, y, 1);
	}
}


///////////////////////////////////////////////////////////
//                                                       //
///////////////////////////////////////////////////////////

//---------------------------------------------------------
bool CPDAL_to_Grid::_Read_Points(const CSG_String &File, const CSG_Array_Int &Classes, bool bStream)
{
    pdal::StageFactory Factory; std::string ReaderDriver = Factory.inferReaderDriver(File.b_str());

    if( ReaderDriver.empty() )
    {
        Message_Fmt("\n%s, %s: %s", _TL("Warning"), _TL("could not infer input file type"), File.c_str());

        return( false );
    }
    
    pdal::Stage *pReader = Factory.createStage(ReaderDriver);

    if( !pReader )
    {
        Message_Fmt("\n%s, %s: %s", _TL("Warning"), _TL("PDAL reader creation failed"), File.c_str());

        return( false );
    }
    
    pdal::Options Options; Options.add(pdal::Option("filename", File.b_str())); pReader->setOptions(Options);

	//-----------------------------------------------------
    if( bStream && pReader->pipelineStreamable() )
    {
        pdal::StreamCallbackFilter StreamFilter; StreamFilter.setInput(*pReader);
        pdal::FixedPointTable Table(10000); StreamFilter.prepare(Table);

		pdal::SpatialReference SpatialRef = Table.spatialReference();

		if( !SpatialRef.empty() && !m_pGrid->Get_Projection().is_Okay() )
		{
			m_pGrid->Get_Projection().Create(SpatialRef.getWKT().c_str());
		}

		bool bClasses = Classes.Get_Size() && Table.layout()->hasDim(pdal::Dimension::Id::Classification);

        //-------------------------------------------------
		auto CallbackReadPoint = [=](pdal::PointRef &point)->bool
		{
			double x = point.getFieldAs<double>(pdal::Dimension::Id::X);
			double y = point.getFieldAs<double>(pdal::Dimension::Id::Y);

			if( m_pGrid->Get_Extent().Contains(x, y) && (!bClasses || _Find_Class(Classes, point.getFieldAs<int>(pdal::Dimension::Id::Classification))) )
			{
				_Add_Point(x, y, point.getFieldAs<double>(pdal::Dimension::Id::Z));
			}

			return( true );
		};
    
        StreamFilter.setCallback(CallbackReadPoint);
        StreamFilter.execute(Table);
    }

	//-----------------------------------------------------
	else    // not streamable
    {
        pdal::PointTable   Table;    pReader->prepare(Table);
        pdal::PointViewSet ViewSet = pReader->execute(Table);
        pdal::PointViewPtr pView   = *ViewSet.begin();

        if( pView->size() < 1 )
        {
            Message_Fmt("\n%s, %s: %s", _TL("Warning"), _TL("invalid or empty file"), File.c_str());

			return( false );
        }

        pdal::SpatialReference SpatialRef = Table.spatialReference();

		if( !SpatialRef.empty() && !m_pGrid->Get_Projection().is_Okay() )
		{
			m_pGrid->Get_Projection().Create(SpatialRef.getWKT().c_str());
		}

		bool bClasses = Classes.Get_Size() && Table.layout()->hasDim(pdal::Dimension::Id::Classification);

        //-----------------------------------------------------
        for(pdal::PointId i=0; i<pView->size() && Set_Progress((sLong)i, (sLong)pView->size()); i++)
        {
			double x = pView->getFieldAs<double>(pdal::Dimension::Id::X, i);
			double y = pView->getFieldAs<double>(pdal::Dimension::Id::Y, i);

			if( m_pGrid->Get_Extent().Contains(x, y) && (!bClasses || _Find_Class(Classes, pView->getFieldAs<int>(pdal::Dimension::Id::Classification, i))) )
			{
				_Add_Point(x, y, pView->getFieldAs<double>(pdal::Dimension::Id::Z, i));
			}
        }
    }

    //-----------------------------------------------------
    return( true );
}


///////////////////////////////////////////////////////////
//                                                       //
///////////////////////////////////////////////////////////

//---------------------------------------------------------
bool CPDAL_to_Grid::_Get_Extent(const CSG_Strings &Files, CSG_Rect &Extent, bool bStream)
{
	bool haveExtent = false;

	for(int i=0; i<Files.Get_Count(); i++)
	{
		CSG_Rect r;

		if( _Get_Extent(Files[i], r, bStream) )
		{
			if( haveExtent == false )
			{
				haveExtent = true; Extent = r;
			}
			else
			{
				Extent.Union(r);
			}
		}
	}

	return( haveExtent );
}

//---------------------------------------------------------
bool CPDAL_to_Grid::_Get_Extent(const CSG_String &File, CSG_Rect &Extent, bool bStream)
{
	pdal::StageFactory Factory; std::string Driver = Factory.inferReaderDriver(File.b_str());

	if( Driver.empty() )
	{
		return( false );
	}

	pdal::Stage *pReader = Factory.createStage(Driver);

	if( !pReader )
	{
		return( false );
	}

	pdal::Options Options; Options.add(pdal::Option("filename", File.b_str())); pReader->setOptions(Options);

	//-----------------------------------------------------
	if( bStream && pReader->pipelineStreamable() )
	{
		CSG_Rect *pExtent = &Extent; Extent.xMin = 0.; Extent.xMax = -1.;

		auto CallbackReadPoint = [=](pdal::PointRef &point)->bool
		{
			double x = point.getFieldAs<double>(pdal::Dimension::Id::X);
			double y = point.getFieldAs<double>(pdal::Dimension::Id::Y);

			if( pExtent->xMin > pExtent->xMax ) // first point
			{
				pExtent->xMin = pExtent->xMax = x;
				pExtent->yMin = pExtent->yMax = y;
			}
			else
			{
				if( pExtent->xMin > x ) { pExtent->xMin = x; } else if( pExtent->xMax < x ) { pExtent->xMax = x; }
				if( pExtent->yMin > y ) { pExtent->yMin = y; } else if( pExtent->yMax < y ) { pExtent->yMax = y; }
			}

			return( true );
		};

		pdal::StreamCallbackFilter Stream; Stream.setInput(*pReader);
		pdal::FixedPointTable Table(10000); Stream.prepare(Table);

		Stream.setCallback(CallbackReadPoint);
		Stream.execute(Table);
	}

	//-----------------------------------------------------
	else    // not streamable
	{
		pdal::PointTable   Table;    pReader->prepare(Table);
		pdal::PointViewSet ViewSet = pReader->execute(Table);
		pdal::PointViewPtr pView   = *ViewSet.begin();

		if( pView->size() < 1 )
		{
			return( false );
		}

		//-------------------------------------------------
		Extent.Create(
			pView->getFieldAs<double>(pdal::Dimension::Id::X,                 0),
			pView->getFieldAs<double>(pdal::Dimension::Id::Y,                 0),
			pView->getFieldAs<double>(pdal::Dimension::Id::X, pView->size() - 1),
			pView->getFieldAs<double>(pdal::Dimension::Id::Y, pView->size() - 1)
		);

		pdal::point_count_t Step = pView->size() / 1000; if( Step < 1 ) { Step = 1; }

		for(pdal::PointId i=1; i<pView->size(); i+=Step)
		{
			Extent.Union(
				pView->getFieldAs<double>(pdal::Dimension::Id::X, i),
				pView->getFieldAs<double>(pdal::Dimension::Id::Y, i)
			);
		}
	}

	return( true );
}


///////////////////////////////////////////////////////////
//                                                       //
//                                                       //
//                                                       //
///////////////////////////////////////////////////////////

//---------------------------------------------------------

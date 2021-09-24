
///////////////////////////////////////////////////////////
//                                                       //
//                         SAGA                          //
//                                                       //
//      System for Automated Geoscientific Analyses      //
//                                                       //
//                     Tool Library                      //
//                   Projection_Proj4                    //
//                                                       //
//-------------------------------------------------------//
//                                                       //
//                 gcs_rotated_grid.cpp                  //
//                                                       //
//                 Olaf Conrad (c) 2021                  //
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
#include "gcs_rotated_grid.h"


///////////////////////////////////////////////////////////
//														 //
//														 //
//														 //
///////////////////////////////////////////////////////////

//---------------------------------------------------------
CGCS_Rotated_Grid::CGCS_Rotated_Grid(void)
{
	Set_Name		(_TL("Rotated to Regular Grid"));

	Set_Author		("O.Conrad (c) 2021");

	Set_Description	(_TW(
		"This tool projects grids using rotated-pole coordinates "
		"to regular geographic grids.\n"
		"Rotated-pole coordinates are used by the CORDEX project. "

		"<hr><h4>CORDEX Domains</h4><table border=\"1\">"
		"<tr><td>CORDEX Area</td><td>Name</td><td>Resolution</td><td>N-Pole/Lon.</td><td>N-Pole/Lat.</td></tr>"
		"<tr><td>South America   </td><td>SAM-44</td><td>0.44</td><td>  -56.06</td><td> 70.60</td></tr>"
		"<tr><td>Central America </td><td>CAM-44</td><td>0.44</td><td>  113.98</td><td> 75.74</td></tr>"
		"<tr><td>North America   </td><td>NAM-44</td><td>0.44</td><td>   83.00</td><td> 42.50</td></tr>"
		"<tr><td>Europe          </td><td>EUR-44</td><td>0.44</td><td> -162.00</td><td> 39.25</td></tr>"
		"<tr><td>Africa          </td><td>AFR-44</td><td>0.44</td><td>     N/A</td><td> 90.00</td></tr>"
		"<tr><td>South Asia      </td><td>WAS-44</td><td>0.44</td><td> -123.34</td><td> 79.95</td></tr>"
		"<tr><td>East Asia       </td><td>EAS-44</td><td>0.44</td><td>  -64.78</td><td> 77.61</td></tr>"
		"<tr><td>Central Asia    </td><td>CAS-44</td><td>0.44</td><td> -103.39</td><td> 43.48</td></tr>"
		"<tr><td>Australasia     </td><td>AUS-44</td><td>0.44</td><td>  141.38</td><td> 60.31</td></tr>"
		"<tr><td>Antarctica      </td><td>ANT-44</td><td>0.44</td><td> -166.92</td><td>  6.08</td></tr>"
		"<tr><td>Arctic          </td><td>ARC-44</td><td>0.44</td><td>    0.00</td><td>  6.55</td></tr>"
		"<tr><td>Mediterranean   </td><td>MED-44</td><td>0.44</td><td>  198.00</td><td> 39.25</td></tr>"
		"<tr><td>M-East, N-Africa</td><td>MNA-44</td><td>0.44</td><td>     N/A</td><td> 90.00</td></tr>"
		"<tr><td>M-East, N-Africa</td><td>MNA-22</td><td>0.22</td><td>     N/A</td><td> 90.00</td></tr>"
		"<tr><td>Europe          </td><td>EUR-11</td><td>0.11</td><td> -162.00</td><td> 39.25</td></tr>"
		"<tr><td>South East Asia </td><td>SEA-22</td><td>0.22</td><td>     N/A</td><td> 90.00</td></tr>"
		"</table>"
	));

	Add_Reference("https://is-enes-data.github.io/cordex_archive_specifications.pdf",
		SG_T("CORDEX Archive Specifications")
	);

	// CORDEX Area      | Name   | Res. | NP/Lon  | NP/Lat
	// -----------------+--------+------+---------+--------
	// South America    | SAM-44 | 0.44 |  -56.06 |  70.60
	// Central America  | CAM-44 | 0.44 |  113.98 |  75.74
	// North America    | NAM-44 | 0.44 |   83.00 |  42.50
	// Europe           | EUR-44 | 0.44 | -162.00 |  39.25
	// Africa           | AFR-44 | 0.44 |     N/A |  90.00
	// South Asia       | WAS-44 | 0.44 | -123.34 |  79.95
	// East Asia        | EAS-44 | 0.44 |  -64.78 |  77.61
	// Central Asia     | CAS-44 | 0.44 | -103.39 |  43.48
	// Australasia      | AUS-44 | 0.44 |  141.38 |  60.31
	// Antarctica       | ANT-44 | 0.44 | -166.92 |   6.08
	// Arctic           | ARC-44 | 0.44 |    0.00 |   6.55
	// Mediterranean    | MED-44 | 0.44 |  198.00 |  39.25
	// M-East, N-Africa | MNA-44 | 0.44 |     N/A |  90.00
	// M-East, N-Africa | MNA-22 | 0.22 |     N/A |  90.00
	// Europe           | EUR-11 | 0.11 | -162.00 |  39.25
	// South East Asia  | SEA-22 | 0.22 |     N/A |  90.00

	// proj supports this operation principally with 'ob_tran':
	// +proj=ob_tran +o_proj=longlat +o_lon_p=-162 +o_lat_p=39.25 +lon_0=180
	// but seems to have some problems (i.e. latitudes/longitudes in radians)

	//-----------------------------------------------------
	Parameters.Add_Grid_List("",
		"SOURCE"	, _TL("Grids"),
		_TL(""),
		PARAMETER_INPUT
	);

	Parameters.Add_Grid_List("",
		"TARGET"	, _TL("Grids"),
		_TL(""),
		PARAMETER_OUTPUT, false
	);

	Parameters.Add_Shapes("",
		"EXTENT"	, _TL("Extent"),
		_TL(""),
		PARAMETER_OUTPUT_OPTIONAL, SHAPE_TYPE_Polygon
	);

	Parameters.Add_Node("", "ROT_POLE", _TL("Rotated Pole"), _TL(""));
	Parameters.Add_Double("ROT_POLE", "ROT_POLE_LON", _TL("Longitude"), _TL(""), -162.00);
	Parameters.Add_Double("ROT_POLE", "ROT_POLE_LAT", _TL("Latitude" ), _TL(""),   39.25);

	m_Grid_Target.Create(&Parameters, false, "TARGET_NODE", "TARGET_");
}


///////////////////////////////////////////////////////////
//														 //
///////////////////////////////////////////////////////////

//---------------------------------------------------------
int CGCS_Rotated_Grid::On_Parameter_Changed(CSG_Parameters *pParameters, CSG_Parameter *pParameter)
{
	if( pParameter->Get_Type() == PARAMETER_TYPE_Grid_System
	||  pParameter->Cmp_Identifier("ROT_POLE_LON")
	||  pParameter->Cmp_Identifier("ROT_POLE_LAT") )
	{
		CSG_Grid_System	System = *pParameters->Get_Grid_System();

		if( System.is_Valid() )
		{
			double dLon = -(180. + (*pParameters)("ROT_POLE_LON")->asDouble()) * M_DEG_TO_RAD;
			double dLat = -( 90. - (*pParameters)("ROT_POLE_LAT")->asDouble()) * M_DEG_TO_RAD;

			CSG_Shapes Extent;

			if( Get_Projected(dLon, dLat, System, &Extent) )
			{
				System.Create(System.Get_Cellsize(), Extent.Get_Extent());

				m_Grid_Target.Set_User_Defined(pParameters, System);
			}
		}
	}
	else
	{
		m_Grid_Target.On_Parameter_Changed(pParameters, pParameter);
	}

	return( CSG_Tool_Grid::On_Parameter_Changed(pParameters, pParameter) );
}

//---------------------------------------------------------
int CGCS_Rotated_Grid::On_Parameters_Enable(CSG_Parameters *pParameters, CSG_Parameter *pParameter)
{
	m_Grid_Target.On_Parameters_Enable(pParameters, pParameter);

	return( CSG_Tool_Grid::On_Parameters_Enable(pParameters, pParameter) );
}


///////////////////////////////////////////////////////////
//														 //
///////////////////////////////////////////////////////////

//---------------------------------------------------------
bool CGCS_Rotated_Grid::On_Execute(void)
{
	if( !m_Grid_Target.Get_System().is_Valid() )
	{
		return( false );
	}

	double dLon = -(180. + Parameters("ROT_POLE_LON")->asDouble()) * M_DEG_TO_RAD;
	double dLat = -( 90. - Parameters("ROT_POLE_LAT")->asDouble()) * M_DEG_TO_RAD;

	Get_Projected(dLon, dLat, Get_System(), Parameters("EXTENT")->asShapes());

	CSG_Grid_System	System(m_Grid_Target.Get_System());

	CSG_Parameter_Grid_List	*pSources = Parameters("SOURCE")->asGridList();
	CSG_Parameter_Grid_List	*pTargets = Parameters("TARGET")->asGridList();

	//-----------------------------------------------------
	pTargets->Del_Items();

	for(int i=0; i<pSources->Get_Grid_Count(); i++)
	{
		CSG_Grid	*pSource	= pSources->Get_Grid(i);
		CSG_Grid	*pTarget	= SG_Create_Grid(System, pSource->Get_Type());

		pTarget->Set_Name        (pSource->Get_Name());
		pTarget->Set_Description (pSource->Get_Description());
		pTarget->Set_Unit        (pSource->Get_Unit());
		pTarget->Set_Scaling     (pSource->Get_Scaling(), pSource->Get_Offset());
		pTarget->Set_NoData_Value(pSource->Get_NoData_Value());

		if( 1 )
		{
			pTarget->Get_Projection().Set_GCS_WGS84();
		}

		pTargets->Add_Item(pTarget);
	}

	//-----------------------------------------------------
	for(int y=0; y<System.Get_NY() && Set_Progress(y, System.Get_NY()); y++)
	{
		double yWorld = System.Get_YMin() + y * System.Get_Cellsize();

		#pragma omp parallel for
		for(int x=0; x<System.Get_NX(); x++)
		{
			double xWorld = System.Get_XMin() + x * System.Get_Cellsize();

			TSG_Point p = Get_Projected(dLon, dLat, false, xWorld, yWorld); double Value;

			for(int i=0; i<pSources->Get_Grid_Count(); i++)
			{
				if( pSources->Get_Grid(i)->Get_Value(p.x, p.y, Value) )
				{
					pTargets->Get_Grid(i)->Set_Value(x, y, Value);
				}
				else
				{
					pTargets->Get_Grid(i)->Set_NoData(x, y);
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
inline TSG_Point CGCS_Rotated_Grid::Get_Projected(double dLon, double dLat, bool bForward, double Lon, double Lat)
{
	Lon *= M_DEG_TO_RAD; Lat *= M_DEG_TO_RAD;

	if( bForward == false )
	{
		Lon  += dLon;
		dLat = -dLat;
	}

	TSG_Point	p;

	p.y  = asin(cos(dLat) * sin(Lat) - cos(Lon) * sin(dLat) * cos(Lat));
	p.x  = atan2(sin(Lon), tan(Lat) * sin(dLat) + cos(Lon) * cos(dLat));

	if( bForward == true )
	{
		p.x -= dLon;
	}

	p.y *= M_RAD_TO_DEG; p.x *= M_RAD_TO_DEG;

	return( p );
}


///////////////////////////////////////////////////////////
//														 //
///////////////////////////////////////////////////////////

//---------------------------------------------------------
bool CGCS_Rotated_Grid::Get_Projected(double dLon, double dLat, const CSG_Grid_System &System, CSG_Shapes *pExtent)
{
	if( pExtent )
	{
		pExtent->Create(SHAPE_TYPE_Polygon, _TL("Extent"));
		pExtent->Add_Field("ID", SG_DATATYPE_Int);
		pExtent->Get_Projection().Set_GCS_WGS84();
		CSG_Shape &Extent = *pExtent->Add_Shape();

		for(int y=0; y<System.Get_NY(); y++) { Extent.Add_Point(Get_Projected(dLon, dLat, true, System.Get_XMin(), System.Get_YMin() + y * System.Get_Cellsize())); }
		for(int x=0; x<System.Get_NX(); x++) { Extent.Add_Point(Get_Projected(dLon, dLat, true, System.Get_XMin() + x * System.Get_Cellsize(), System.Get_YMax())); }
		for(int y=0; y<System.Get_NY(); y++) { Extent.Add_Point(Get_Projected(dLon, dLat, true, System.Get_XMax(), System.Get_YMax() - y * System.Get_Cellsize())); }
		for(int x=0; x<System.Get_NX(); x++) { Extent.Add_Point(Get_Projected(dLon, dLat, true, System.Get_XMax() - x * System.Get_Cellsize(), System.Get_YMin())); }

		return( pExtent->Get_Extent().Get_Area() > 0. );
	}

	return( false );
}


///////////////////////////////////////////////////////////
//														 //
//														 //
//														 //
///////////////////////////////////////////////////////////

//---------------------------------------------------------


///////////////////////////////////////////////////////////
//                                                       //
//                         SAGA                          //
//                                                       //
//      System for Automated Geoscientific Analyses      //
//                                                       //
//                     Tool Library                      //
//                                                       //
//                       io_pdal                         //
//                                                       //
//-------------------------------------------------------//
//                                                       //
//                   pdal_driver.cpp                     //
//                                                       //
//                 Copyright (C) 2021 by                 //
//                    Volker Wichmann                    //
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
//    e-mail:     wichmann@laserdata                     //
//                                                       //
//    contact:    Volker Wichmann                        //
//                LASERDATA GmbH                         //
//                Innsbruck, Austria                     //
//                                                       //
///////////////////////////////////////////////////////////

//---------------------------------------------------------
#include "pdal_driver.h"

#include <pdal/pdal_config.hpp>
#include <pdal/PluginManager.hpp>
#include <pdal/StageFactory.hpp>


///////////////////////////////////////////////////////////
//                                                       //
//                                                       //
//                                                       //
///////////////////////////////////////////////////////////

//---------------------------------------------------------
CSG_PDAL_Drivers  gSG_PDAL_Drivers;

//---------------------------------------------------------
const CSG_PDAL_Drivers &  SG_Get_PDAL_Drivers (void)
{
    return( gSG_PDAL_Drivers );
}


///////////////////////////////////////////////////////////
//                                                       //
//                                                       //
//                                                       //
///////////////////////////////////////////////////////////

//---------------------------------------------------------
CSG_PDAL_Drivers::CSG_PDAL_Drivers(void)
{
    pdal::PluginManager<pdal::Stage>::loadAll();
}

//---------------------------------------------------------
CSG_PDAL_Drivers::~CSG_PDAL_Drivers(void)
{
    // do we have to clean-up something?
}


///////////////////////////////////////////////////////////
//                                                       //
///////////////////////////////////////////////////////////

//---------------------------------------------------------
CSG_String CSG_PDAL_Drivers::Get_Version(void) const
{
#if defined(_DEBUG) && defined(_SAGA_MSW)
	return( "" );
#endif

	return( pdal::Config::fullVersionString().c_str() );
}

//---------------------------------------------------------
int CSG_PDAL_Drivers::Get_Count(void) const
{
#if defined(_DEBUG) && defined(_SAGA_MSW)
	return( 0 );
#endif

	return( (int)pdal::PluginManager<pdal::Stage>::names().size() );
}


///////////////////////////////////////////////////////////
//                                                       //
///////////////////////////////////////////////////////////

//---------------------------------------------------------
CSG_String CSG_PDAL_Drivers::Get_Description(bool bReader) const
{
	CSG_String Description;

	Description += CSG_String::Format("\n<table border=\"1\"><tr><th>%s</th><th>%s</th><th>%s</th></tr>",
		_TL("Name"), _TL("Extension"), _TL("Description")
	);

	for(int i=0; i<Get_Count(); i++)
	{
		if( is_Working(i) && ((bReader && is_Reader(i)) || (!bReader && is_Writer(i))) )
		{
			CSG_Strings Extension = Get_Extensions(i); CSG_String Extensions;

			for(int j=0; j<Extension.Get_Count(); j++)
			{
				if( !Extension[j].is_Empty() )
				{
					Extensions += (j ? ";" : "") + Extension[j];
				}
			}

			Description += "<tr><td>" + Get_Name(i) + "</td><td>" + Extensions + "</td><td>" + Get_Description(i) + "</td></tr>";
		}
	}

	Description += "</table>";

	return( Description );
}

//---------------------------------------------------------
CSG_String CSG_PDAL_Drivers::Get_Filter(bool bReader) const
{
	CSG_String Filter, Recognized;

	for(int i=0; i<Get_Count(); i++)
	{
		if( is_Working(i) && ((bReader && is_Reader(i)) || (!bReader && is_Writer(i))) )
		{
			CSG_Strings Extension = Get_Extensions(i);

			for(int j=0; j<Extension.Get_Count(); j++)
			{
				if( !Extension[j].is_Empty() )
				{
					Filter     += "*." + Extension[j] + "|*." + Extension[j] + "|";
					Recognized += (Recognized.is_Empty() ? "*." : ";*.") + Extension[j];
				}
			}
		}
	}

	Filter.Prepend(CSG_String::Format("%s|%s|", _TL("Recognized Files"), Recognized.c_str()));
	Filter.Append (CSG_String::Format("%s|*.*", _TL("All Files")));

	return( Filter );
}


///////////////////////////////////////////////////////////
//                                                       //
///////////////////////////////////////////////////////////

//---------------------------------------------------------
CSG_String CSG_PDAL_Drivers::Get_Name(int Index) const
{
    return( pdal::PluginManager<pdal::Stage>::names().at(Index).c_str() );
}

//---------------------------------------------------------
CSG_String CSG_PDAL_Drivers::Get_Description(int Index) const
{
    return( pdal::PluginManager<pdal::Stage>::description(std::string(Get_Name(Index))).c_str() );
}

//---------------------------------------------------------
CSG_Strings CSG_PDAL_Drivers::Get_Extensions(int Index) const
{
    pdal::StageExtensions& StageExtensions = pdal::PluginManager<pdal::Stage>::extensions();
    
    pdal::StringList Extensions = StageExtensions.extensions(std::string(Get_Name(Index)));

    CSG_Strings Exts;

    for(auto Ext : Extensions)
    {
        Exts.Add(Ext.c_str());    
    }

    return( Exts );
}


///////////////////////////////////////////////////////////
//                                                       //
///////////////////////////////////////////////////////////

//---------------------------------------------------------
bool CSG_PDAL_Drivers::is_Working(int Index) const
{
	CSG_String Name(Get_Name(Index));

	for(int j=0; !g_Non_Working_Drivers[j].Name.is_Empty(); j++)
	{
		if( Name.Cmp(g_Non_Working_Drivers[j].Name) == 0 )
		{
			return( false );
		}
	}

	return( true );
}

//---------------------------------------------------------
bool CSG_PDAL_Drivers::is_Reader(int Index) const
{
    CSG_String Prefix = Get_Name(Index).BeforeFirst('.');

    if( Prefix.Cmp("readers") == 0 )
    {
        return( true );
    }

    return( false );
}

//---------------------------------------------------------
bool CSG_PDAL_Drivers::is_Writer(int Index) const
{
    CSG_String Prefix = Get_Name(Index).BeforeFirst('.');

    if( Prefix.Cmp("writers") == 0 )
    {
        return( true );
    }

    return( false );
}

//---------------------------------------------------------
bool CSG_PDAL_Drivers::is_Filter(int Index) const
{
    CSG_String Prefix = Get_Name(Index).BeforeFirst('.');

    if( Prefix.Cmp("filters") == 0 )
    {
        return( true );
    }

    return( false );
}


///////////////////////////////////////////////////////////
//                                                       //
//                                                       //
//                                                       //
///////////////////////////////////////////////////////////

//---------------------------------------------------------

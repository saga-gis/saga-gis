
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
    return( pdal::Config::fullVersionString().c_str() );
}

//---------------------------------------------------------
int CSG_PDAL_Drivers::Get_Count(void) const
{
    return( (int)pdal::PluginManager<pdal::Stage>::names().size() );
}

//---------------------------------------------------------
CSG_String CSG_PDAL_Drivers::Get_Driver_Name(int iIndex) const
{
    return( pdal::PluginManager<pdal::Stage>::names().at(iIndex).c_str() );
}

//---------------------------------------------------------
CSG_String CSG_PDAL_Drivers::Get_Driver_Description(int iIndex) const
{
    return( pdal::PluginManager<pdal::Stage>::description(std::string(Get_Driver_Name(iIndex))).c_str() );
}

//---------------------------------------------------------
CSG_Strings CSG_PDAL_Drivers::Get_Driver_Extensions(int iIndex) const
{
    pdal::StageExtensions& StageExtensions = pdal::PluginManager<pdal::Stage>::extensions();
    
    pdal::StringList Extensions = StageExtensions.extensions(std::string(Get_Driver_Name(iIndex)));

    CSG_Strings Exts;

    for(auto Ext : Extensions)
    {
        Exts.Add(Ext.c_str());    
    }

    return( Exts );
}

//---------------------------------------------------------
bool CSG_PDAL_Drivers::is_Reader(int Index) const
{
    CSG_String Prefix = Get_Driver_Name(Index).BeforeFirst('.');

    if( Prefix.Cmp(SG_T("readers")) == 0 )
    {
        return( true );
    }

    return( false );
}

//---------------------------------------------------------
bool CSG_PDAL_Drivers::is_Writer(int Index) const
{
    CSG_String Prefix = Get_Driver_Name(Index).BeforeFirst('.');

    if( Prefix.Cmp(SG_T("writers")) == 0 )
    {
        return( true );
    }

    return( false );
}

//---------------------------------------------------------
bool CSG_PDAL_Drivers::is_Filter(int Index) const
{
    CSG_String Prefix = Get_Driver_Name(Index).BeforeFirst('.');

    if( Prefix.Cmp(SG_T("filters")) == 0 )
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

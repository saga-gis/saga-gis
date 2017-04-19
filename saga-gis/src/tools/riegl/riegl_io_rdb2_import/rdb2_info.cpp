/* This Source Code Form is subject to the terms of the Mozilla Public
 * License, v. 2.0. If a copy of the MPL was not distributed with this
 * file, You can obtain one at http://mozilla.org/MPL/2.0/. */

//---------------------------------------------------------


///////////////////////////////////////////////////////////
//														 //
//														 //
//														 //
///////////////////////////////////////////////////////////

//---------------------------------------------------------
#include "rdb2_info.h"

#include <algorithm> // std::find
#include <array>
#include <vector>
#include <sstream>
#include <riegl/rdb.hpp>
#include "rapidjson/document.h"
#include "rapidjson/pointer.h"
#include "rapidjson/stringbuffer.h"

///////////////////////////////////////////////////////////
//														 //
//														 //
//														 //
///////////////////////////////////////////////////////////


//---------------------------------------------------------
#define	ADD_FIELD(id, var, name, type)	if( Parameters(id)->asBool() ) { iField[var] = nFields++; pPoints->Add_Field(name, type); } else { iField[var] = -1; }


///////////////////////////////////////////////////////////
//														 //
//														 //
//														 //
///////////////////////////////////////////////////////////

//---------------------------------------------------------
CRDB2_Info::CRDB2_Info(void)
{
	//-----------------------------------------------------
	// 1. Info...

	Set_Name		(_TL("Info about RDB2 Files"));

	Set_Author		(SG_T("R.Gschweicher (c) 2016, Riegl GmbH"));

	CSG_String		Description(_TW(
		"Print info about a Riegl RDB 2 file. "
		"This is a work in progress."
	));

	Set_Description	(Description);

	//-----------------------------------------------------
	// 2. Parameters...
	Parameters.Add_FilePath(
		NULL	, "FILES"		, _TL("Input Files"),
		_TL(""),
		_TL("RDB2 Files (*.rdbx)|*.rdbx|RDB2 Files (*.RDBX)|*.RDBX|All Files|*.*"),
		NULL, false, false, true
	);
}


///////////////////////////////////////////////////////////
//														 //
//														 //
//														 //
///////////////////////////////////////////////////////////

namespace
{
bool parse_json_int(rapidjson::Document &json, const char *pointer, int &target)
{
	if (rapidjson::Value *obj = rapidjson::Pointer(pointer).Get(json))
	{
		if (obj->IsInt())
		{
			target = obj->GetInt();
		}
		else
		{
			SG_UI_Msg_Add_Error(CSG_String::Format(_TL("json_keys_int: key '%s' cannot be parsed as integer"), pointer));
			return false;
		}
	}
	return true;
};
}
//---------------------------------------------------------
bool CRDB2_Info::On_Execute(void)
{
	CSG_Strings		Files;

	//-----------------------------------------------------
	if( !Parameters("FILES")->asFilePath()->Get_FilePaths(Files) )
	{
		return( false );
	}

	//-----------------------------------------------------


	try {
		for(int i=0; i<Files.Get_Count(); i++)
		{
			SG_UI_Msg_Add(CSG_String::Format(_TL("Parsing %s ... "), SG_File_Get_Name(Files[i], true).c_str()), true);

			// New RDB library context
			riegl::rdb::Context context;

			// Access existing database
			riegl::rdb::Pointcloud rdb(context);
			riegl::rdb::pointcloud::OpenSettings settings(context);
			rdb.open(Files[i].b_str(), settings);

			// Get index graph root node
			riegl::rdb::pointcloud::QueryStat stat = rdb.stat();
			riegl::rdb::pointcloud::GraphNode root = stat.index();
			//root.pointCountTotal

			// print point count
			SG_UI_Msg_Add(CSG_String::Format(_TL("number of points: %d"), root.pointCountTotal), true);

			// Get list of point attributes
			std::vector<std::string> attributes = rdb.pointAttribute().list();

			// check for existance of specific rdb attribute
			auto listContains = [] (std::vector<std::string> &attributes, const char *att)
			{
				return std::find(attributes.begin(),attributes.end(), att) != attributes.end();
			};

			SG_UI_Msg_Add(CSG_String::Format(_TL("check for known attributes")), true);
			auto printIfListContains = [this, listContains] (std::vector<std::string> &attributes, const char *att)
			{
				if (listContains(attributes, att))
				{
					std::ostringstream ss;
					ss << "  RDB2 found attribute " << att;
					SG_UI_Msg_Add(ss.str().c_str(), true);
// 					SG_UI_Msg_Add(CSG_String::Format(_TL("  RDB2 found attribute %s"), att), true);
				}
			};
			printIfListContains(attributes, "riegl.id");
			printIfListContains(attributes, "riegl.xyz");
			printIfListContains(attributes, "riegl.timestamp");
			printIfListContains(attributes, "riegl.amplitude");
			printIfListContains(attributes, "riegl.reflectance");
			printIfListContains(attributes, "riegl.deviation");
			printIfListContains(attributes, "riegl.pulse_width");
			printIfListContains(attributes, "riegl.target_index");
			printIfListContains(attributes, "riegl.target_count");
			printIfListContains(attributes, "riegl.class");
			printIfListContains(attributes, "riegl.rgba");
			// get geotag from rdbx-meta information (if any)
			riegl::rdb::pointcloud::MetaData meta = rdb.metaData();
			if (meta.exists("riegl.geo_tag"))
			{
				std::string geo_tag = meta.get("riegl.geo_tag");
				rapidjson::Document json;
				json.Parse(geo_tag.c_str());
				int epsg;
				if (parse_json_int(json, "/crs/epsg", epsg))
				{
					SG_UI_Msg_Add(CSG_String::Format(_TL("RDB2 found epsg: %d"), epsg), true);
				}
			}

		}
	} catch (std::exception &e)
	{
		SG_UI_Msg_Add_Error(CSG_String::Format(_TL("Unable to open RDB2 file! Exception: %s"), e.what()));
		return false;
	}
	catch(...) {
		SG_UI_Msg_Add_Error(CSG_String::Format(_TL("Unknown RDB2 reader exception!")));
		return false;
	}

	return true;
}


///////////////////////////////////////////////////////////
//														 //
//														 //
//														 //
///////////////////////////////////////////////////////////

//---------------------------------------------------------


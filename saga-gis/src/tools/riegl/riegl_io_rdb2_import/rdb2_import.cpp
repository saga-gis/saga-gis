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
#include "rdb2_import.h"

#include <algorithm> // std::find
#include <array>
#include <vector>
#include <sstream>
#include <riegl/rdb.hpp>
#include "rapidjson/document.h"
#include "rapidjson/pointer.h"
#include "rapidjson/stringbuffer.h"
#include "rapidjson/writer.h"
#include "rapidjson/prettywriter.h"

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
CRDB2_Import::CRDB2_Import(void)
{
	CSG_Parameter	*pNode;

	//-----------------------------------------------------
	// 1. Info...

	Set_Name		(_TL("Import RDB2 Files"));

	Set_Author		(SG_T("R.Gschweicher (c) 2016, Riegl GmbH"));

	CSG_String		Description(_TW(
		"Import a pointcloud from Riegl RDB 2 format. "
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

	Parameters.Add_PointCloud_List(
		NULL	, "POINTS"		, _TL("Point Clouds"),
		_TL(""),
		PARAMETER_OUTPUT
	);

	pNode	= Parameters.Add_Node(
		NULL	, "NODE_VARS"	, _TL("Attributes to import besides x,y,z ..."),
		_TL("")
	);

	Parameters.Add_Value(pNode, "id",   _TL("point source ID")      , _TL(""), PARAMETER_TYPE_Bool, false);
	Parameters.Add_Value(pNode, "ts",   _TL("timestamp")            , _TL(""), PARAMETER_TYPE_Bool, false);
	Parameters.Add_Value(pNode, "ampl", _TL("amplitude")            , _TL(""), PARAMETER_TYPE_Bool, false);
	Parameters.Add_Value(pNode, "refl", _TL("reflectance")          , _TL(""), PARAMETER_TYPE_Bool, false);
	Parameters.Add_Value(pNode, "dev",  _TL("deviation")            , _TL(""), PARAMETER_TYPE_Bool, false);
	Parameters.Add_Value(pNode, "pw",   _TL("pulse_width")          , _TL(""), PARAMETER_TYPE_Bool, false);
	Parameters.Add_Value(pNode, "t_idx",_TL("target_index")         , _TL(""), PARAMETER_TYPE_Bool, false);
	Parameters.Add_Value(pNode, "t_cnt",_TL("target_count")         , _TL(""), PARAMETER_TYPE_Bool, false);
	Parameters.Add_Value(pNode, "class",_TL("class")                , _TL(""), PARAMETER_TYPE_Bool, false);
	Parameters.Add_Value(pNode, "rgba", _TL("rgb color")            , _TL(""), PARAMETER_TYPE_Bool, false);

	Parameters.Add_Value(
		NULL	, "epsg" , _TL("Load epsg geo-tag"),
		_TL(""),
		PARAMETER_TYPE_Bool, false
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
bool CRDB2_Import::On_Execute(void)
{
	const size_t BUFFER_SIZE = 100000;

	CSG_Parameter_PointCloud_List	*pPointsList;
	CSG_Strings		Files;

	//-----------------------------------------------------
	if( !Parameters("FILES")->asFilePath()->Get_FilePaths(Files) )
	{
		return( false );
	}

	//-----------------------------------------------------
	pPointsList	= Parameters("POINTS")->asPointCloudList();
	pPointsList	->Del_Items();

	bool param_id    = Parameters("id")->asBool();
	bool param_ts    = Parameters("ts")->asBool();
	bool param_ampl  = Parameters("ampl")->asBool();
	bool param_refl  = Parameters("refl")->asBool();
	bool param_dev   = Parameters("dev")->asBool();
	bool param_pw    = Parameters("pw")->asBool();
	bool param_class = Parameters("class")->asBool();
	bool param_t_idx = Parameters("t_idx")->asBool();
	bool param_t_cnt = Parameters("t_cnt")->asBool();
	bool param_rgba  = Parameters("rgba")->asBool();
	bool param_epsg  = Parameters("epsg")->asBool();

	try {
		for(int i=0; i<Files.Get_Count(); i++)
		{
			SG_UI_Msg_Add(CSG_String::Format(_TL("Parsing %s ... "), SG_File_Get_Name(Files[i], true).c_str()), true);

			// New RDB library context
			riegl::rdb::Context context;

			SG_UI_Msg_Add(CSG_String::Format(_TL("access database")), true);
			// Access existing database
			riegl::rdb::Pointcloud rdb(context);
			riegl::rdb::pointcloud::OpenSettings settings(context);
			rdb.open(Files[i].b_str(), settings);

			SG_UI_Msg_Add(CSG_String::Format(_TL("graph and root node")), true);
			// Get index graph root node
			riegl::rdb::pointcloud::QueryStat stat = rdb.stat();
			riegl::rdb::pointcloud::GraphNode root = stat.index();
			//root.pointCountTotal

			// Get list of point attributes
			std::vector<std::string> attributes = rdb.pointAttribute().list();

			// check for existance of specific rdb attribute
			auto listContains = [] (std::vector<std::string> &attributes, const char *att)
			{
				return std::find(attributes.begin(),attributes.end(), att) != attributes.end();
			};
			auto checkAttribute = [listContains] (bool param, std::vector<std::string> &attributes, const char *att) -> bool
			{
				if (param && !listContains(attributes, att))
				{
					std::ostringstream ss;
					ss << "RDB2 has no attribute " << att << ". Aborting";
					SG_UI_Msg_Add_Error(ss.str().c_str());
					return false;
				}
				return true;
			};

			SG_UI_Msg_Add(CSG_String::Format(_TL("check for known attributes")), true);
			if (!checkAttribute(param_id,    attributes, "riegl.id"))           return false;
			if (!checkAttribute(true,        attributes, "riegl.xyz"))          return false;
			if (!checkAttribute(param_ts,    attributes, "riegl.timestamp"))    return false;
			if (!checkAttribute(param_ampl,  attributes, "riegl.amplitude"))    return false;
			if (!checkAttribute(param_refl,  attributes, "riegl.reflectance"))  return false;
			if (!checkAttribute(param_dev,   attributes, "riegl.deviation"))    return false;
			if (!checkAttribute(param_pw,    attributes, "riegl.pulse_width"))  return false;
			if (!checkAttribute(param_t_idx, attributes, "riegl.target_index")) return false;
			if (!checkAttribute(param_t_cnt, attributes, "riegl.target_count")) return false;
			if (!checkAttribute(param_class, attributes, "riegl.class"))        return false;
			if (!checkAttribute(param_rgba,  attributes, "riegl.rgba"))         return false;

			SG_UI_Msg_Add(CSG_String::Format(_TL("assign buffers")), true);
			// assign buffers for reading from rdbx
			std::vector< std::array<double, 3> >  buffer_xyz(BUFFER_SIZE);
			std::vector<uint64_t>                 buffer_id;
			std::vector<double>                   buffer_ts;
			std::vector<float>                    buffer_ampl;
			std::vector<float>                    buffer_refl;
			std::vector<int16_t>                  buffer_dev;
			std::vector<float>                    buffer_pw;
			std::vector<uint8_t>                  buffer_t_idx;
			std::vector<uint8_t>                  buffer_t_cnt;
			std::vector<uint16_t>                 buffer_class;
			std::vector< std::array<uint8_t, 4> > buffer_rgba;
			if (param_id)    buffer_id.resize(BUFFER_SIZE);
			if (param_ts)    buffer_ts.resize(BUFFER_SIZE);
			if (param_ampl)  buffer_ampl.resize(BUFFER_SIZE);
			if (param_refl)  buffer_refl.resize(BUFFER_SIZE);
			if (param_dev)   buffer_dev.resize(BUFFER_SIZE);
			if (param_pw)    buffer_pw.resize(BUFFER_SIZE);
			if (param_t_idx) buffer_t_idx.resize(BUFFER_SIZE);
			if (param_t_cnt) buffer_t_cnt.resize(BUFFER_SIZE);
			if (param_class) buffer_class.resize(BUFFER_SIZE);
			if (param_rgba)  buffer_rgba.resize(BUFFER_SIZE);

			//-----------------------------------------------------
			SG_UI_Msg_Add(CSG_String::Format(_TL("create empty PointCloud")), true);
			CSG_PointCloud	*pPoints	= SG_Create_PointCloud();
			pPoints->Set_Name(SG_File_Get_Name(Files[i], false));

			// get geotag from rdbx-meta information (if any)
			if (param_epsg)
			{
				riegl::rdb::pointcloud::MetaData meta = rdb.metaData();
				if (!meta.exists("riegl.geo_tag"))
				{
					SG_UI_Msg_Add_Error(_TL("RDB2 no meta tag 'riegl.geo_tag'. Aborting!"));
					return false;
				}
				std::string geo_tag = meta.get("riegl.geo_tag");
				rapidjson::Document json;
				json.Parse(geo_tag.c_str());
				int epsg;
				if (!parse_json_int(json, "/crs/epsg", epsg))
				{
					SG_UI_Msg_Add_Error(_TL("RDB2 meta tag 'riegl.geo_tag' does not contain field '/crs/epsg'. Aborting!"));
					return false;
				}
				SG_UI_Msg_Add(CSG_String::Format(_TL("found epsg: %d"), epsg), true);
				if (!pPoints->Get_Projection().Assign(epsg))
				{
					SG_UI_Msg_Add_Error(_TL("Unable to assign projection information!"));
					return false;
				}
			}

			SG_UI_Msg_Add(CSG_String::Format(_TL("bind buffers for RDB2 import")), true);
			int att_id_cnt   = 0;
			int att_id_id    = 0;
			int att_id_ts    = 0;
			int att_id_ampl  = 0;
			int att_id_refl  = 0;
			int att_id_dev   = 0;
			int att_id_pw    = 0;
			int att_id_t_idx = 0;
			int att_id_t_cnt = 0;
			int att_id_class = 0;
			int att_id_rgba   = 0;
			// create rdb query
			riegl::rdb::pointcloud::QuerySelect select = rdb.select();
			//select.bind("riegl.id",          riegl::rdb::pointcloud::UINT64, &bufferPointNumber[0]);
			select.bind("riegl.xyz",                riegl::rdb::pointcloud::DOUBLE, buffer_xyz.data());
			auto create_and_bind_attribute = [&att_id_cnt, &select, pPoints] (
					const char *att, int &att_id, void *buffer,
					const riegl::rdb::pointcloud::DataType rdb_type,
					ESG_Data_Type saga_type
					)
			{
				select.bind(att,        rdb_type, buffer);
				pPoints->Add_Field(att, saga_type);
				att_id = att_id_cnt;
				att_id_cnt++;
				std::ostringstream ss;
				ss << "created attribute " << att;
				SG_UI_Msg_Add(ss.str().c_str(), true);
			};

			if (param_id)
				create_and_bind_attribute(
							"riegl.id", att_id_id, buffer_id.data(),
							riegl::rdb::pointcloud::UINT64,
							SG_DATATYPE_ULong);
			if (param_ts)
				create_and_bind_attribute(
							"riegl.timestamp", att_id_ts, buffer_ts.data(),
							riegl::rdb::pointcloud::DOUBLE,
							SG_DATATYPE_Double);
			if (param_ampl)
				create_and_bind_attribute(
							"riegl.amplitude", att_id_ampl, buffer_ampl.data(),
							riegl::rdb::pointcloud::SINGLE,
							SG_DATATYPE_Float);
			if (param_refl)
				create_and_bind_attribute(
							"riegl.reflectance", att_id_refl, buffer_refl.data(),
							riegl::rdb::pointcloud::SINGLE,
							SG_DATATYPE_Float);
			if (param_dev)
				create_and_bind_attribute(
							"riegl.deviation", att_id_dev, buffer_dev.data(),
							riegl::rdb::pointcloud::INT16,
							SG_DATATYPE_Short);
			if (param_pw)
				create_and_bind_attribute(
							"riegl.pulse_width", att_id_pw, buffer_pw.data(),
							riegl::rdb::pointcloud::SINGLE,
							SG_DATATYPE_Float);
			if (param_t_idx)
				create_and_bind_attribute(
							"riegl.target_index", att_id_t_idx, buffer_t_idx.data(),
							riegl::rdb::pointcloud::UINT8,
							SG_DATATYPE_Byte);
			if (param_t_cnt)
				create_and_bind_attribute(
							"riegl.target_count", att_id_t_cnt, buffer_t_cnt.data(),
							riegl::rdb::pointcloud::UINT8,
							SG_DATATYPE_Byte);
			if (param_class)
				create_and_bind_attribute(
							"riegl.class", att_id_class, buffer_class.data(),
							riegl::rdb::pointcloud::UINT16,
							SG_DATATYPE_Word);
			if (param_rgba)
				create_and_bind_attribute(
							"riegl.rgba", att_id_rgba, buffer_rgba.data(),
							riegl::rdb::pointcloud::UINT8,
							SG_DATATYPE_Int);

			SG_UI_Msg_Add(CSG_String::Format(_TL("read points. Total: %d"), root.pointCountTotal), true);
			int iPoint = 0;
			SG_UI_Process_Set_Progress(iPoint, root.pointCountTotal);
			// Read and process all points block-wise
			while (const uint32_t count = select.next(BUFFER_SIZE))
			{
				for (uint32_t i = 0; i < count; i++)
				{
					pPoints->Add_Point(buffer_xyz[i][0], buffer_xyz[i][1], buffer_xyz[i][2]);
					if (param_id)
						pPoints->Set_Attribute(att_id_id, buffer_id[i]);
					if (param_ts)
						pPoints->Set_Attribute(att_id_ts, buffer_ts[i]);
					if (param_ampl)
						pPoints->Set_Attribute(att_id_ampl, buffer_ampl[i]);
					if (param_refl)
						pPoints->Set_Attribute(att_id_refl, buffer_refl[i]);
					if (param_dev)
						pPoints->Set_Attribute(att_id_dev, buffer_dev[i]);
					if (param_pw)
						pPoints->Set_Attribute(att_id_pw, buffer_pw[i]);
					if (param_t_idx)
						pPoints->Set_Attribute(att_id_t_idx, buffer_t_idx[i]);
					if (param_t_cnt)
						pPoints->Set_Attribute(att_id_t_cnt, buffer_t_cnt[i]);
					if (param_class)
						pPoints->Set_Attribute(att_id_class, buffer_class[i]);
					if (param_rgba)
					{
						auto &color = buffer_rgba[i];
						pPoints->Set_Attribute(att_id_rgba, SG_GET_RGB(color[0], color[1], color[2]));
					}

					iPoint++;
				}
				SG_UI_Process_Set_Progress(iPoint, root.pointCountTotal);
			}
			SG_UI_Msg_Add(CSG_String::Format(_TL("finished read points")), true);
			if (iPoint <= 0)
			{
				SG_UI_Msg_Add_Error(CSG_String::Format(_TL("RDB2: empty input file")));
				return false;
			}
			SG_UI_Msg_Add(CSG_String::Format(_TL("add PC to PCList")), true);
			pPointsList->Add_Item(pPoints);

			DataObject_Add(pPoints);
			//-----------------------------------------------------
			SG_UI_Msg_Add(CSG_String::Format(_TL("set Parameter")), true);
			CSG_Parameters		sParms;

			DataObject_Get_Parameters(pPoints, sParms);

			if (sParms("METRIC_ATTRIB")	&& sParms("COLORS_TYPE") && sParms("METRIC_COLORS")
				&& sParms("METRIC_ZRANGE") && sParms("DISPLAY_VALUE_AGGREGATE"))
			{
				sParms("DISPLAY_VALUE_AGGREGATE")->Set_Value(3);		// highest z
				sParms("COLORS_TYPE")->Set_Value(2);                    // graduated color
				sParms("METRIC_COLORS")->asColors()->Set_Count(255);    // number of colors
				sParms("METRIC_ATTRIB")->Set_Value(2);					// z attrib
				sParms("METRIC_ZRANGE")->asRange()->Set_Range(pPoints->Get_Minimum(2),pPoints->Get_Maximum(2));
			}

			DataObject_Set_Parameters(pPoints, sParms);

			SG_UI_Msg_Add(_TL("okay"), false);
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


/**********************************************************
 * Version $Id$
 *********************************************************/

///////////////////////////////////////////////////////////
//                                                       //
//                         SAGA                          //
//                                                       //
//      System for Automated Geoscientific Analyses      //
//                                                       //
//                    Module Library:                    //
//                       Shapes_IO                       //
//                                                       //
//-------------------------------------------------------//
//                                                       //
//             pointcloud_from_text_file.cpp             //
//                                                       //
//                 Copyright (C) 2009 by                 //
//                    Volker Wichmann                    //
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
//    e-mail:     wichmann@laserdata.at                  //
//                                                       //
//    contact:    LASERDATA GmbH                         //
//                Management and Analysis of             //
//                Laserscanning Data                     //
//                Technikerstr. 21a                      //
//                6020 Innsbruck                         //
//                Austria                                //
//                www.laserdata.at                       //
//                                                       //
///////////////////////////////////////////////////////////

//---------------------------------------------------------


///////////////////////////////////////////////////////////
//														 //
//														 //
//														 //
///////////////////////////////////////////////////////////

//---------------------------------------------------------
#include "pointcloud_from_text_file.h"


///////////////////////////////////////////////////////////
//														 //
//														 //
//														 //
///////////////////////////////////////////////////////////

//---------------------------------------------------------
CPointCloud_From_Text_File::CPointCloud_From_Text_File(void)
{
	//-----------------------------------------------------
	Set_Name		(_TL("Import Point Cloud from Text File"));

	Set_Author		(SG_T("V. Wichmann, LASERDATA GmbH (c) 2009"));

	Set_Description	(_TW(
					"Creates a point cloud from a text file.\n"
					"The input file must have at least three columns holding the "
					"x, y, z coordinates of each point. You must specify the field "
					"numbers of these. In case you like to import additional attributes, "
					"you have to provide the number of attribute fields. After module "
					"execution, you will be prompted to provide their field numbers, "
					"names and datatypes.\n"
					"You have also to decide on which field separator to use and if "
					"the first line of the input file should be skipped (in case it "
					"contains column headings).\n"
					"The columns in the input file can be in any order, and you can "
					"omit columns, but you have to provide the correct field numbers "
					"of those you like to import.\n\n"
	));


	//-----------------------------------------------------
	Parameters.Add_PointCloud_Output(
		NULL	, "POINTS"		, _TL("Point Cloud"),
		_TL("")
	);

	Parameters.Add_FilePath(
		NULL	, "FILE"		, _TL("Text File"),
		_TL("")
	);

	Parameters.Add_Value(
		NULL	, "XFIELD"	, _TL("X is Column ..."),
		_TL("The column holding the X-coordinate."),
		PARAMETER_TYPE_Int, 1, 1, true
	);
	Parameters.Add_Value(
		NULL	, "YFIELD"	, _TL("Y is Column ..."),
		_TL("The column holding the Y-coordinate."),
		PARAMETER_TYPE_Int, 2, 1, true
	);
	Parameters.Add_Value(
		NULL	, "ZFIELD"	, _TL("Z is Column ..."),
		_TL("The column holding the Z-coordinate."),
		PARAMETER_TYPE_Int, 3, 1, true
	);

	Parameters.Add_Value(
		NULL	, "ATTRIBS"	, _TL("Number of Attributes"),
		_TL("Number of additional attributes to import."),
		PARAMETER_TYPE_Int, 0, 0, true
	);

	Parameters.Add_Value(
        NULL	, "SKIP_HEADER" , _TL("Skip first line"),
        _TL("Skip first line as it contains column names."),
        PARAMETER_TYPE_Bool, false
    );

    Parameters.Add_Choice(
        NULL	, "FIELDSEP" , _TL("Field Separator"),
        _TL("Field Separator"),
        CSG_String::Format(SG_T("%s|%s|%s|"),
            _TL("tabulator"),
            _TL("space"),
            _TL("comma")
        ), 0
    );
}

//---------------------------------------------------------
CPointCloud_From_Text_File::~CPointCloud_From_Text_File(void)
{}


///////////////////////////////////////////////////////////
//														 //
//														 //
//														 //
///////////////////////////////////////////////////////////

//---------------------------------------------------------
bool CPointCloud_From_Text_File::On_Execute(void)
{
	CSG_String				fileName;
	int						iField, iType;
	TSG_Data_Type			Type;
	CSG_String				Name, Types, s;
	CSG_PointCloud			*pPoints;
	CSG_Parameters			P;
	CSG_Parameter			*pNode;
	int						xField, yField, zField, nAttribs, max_iField;
	bool					bSkipHeader;
	char					fieldSep;
	std::vector<int>		vCol;
	std::ifstream			tabStream;
    std::string				tabLine;
	double					lines;
	long					cntPt, cntInvalid;
	double					x, y, z, value;


	//-----------------------------------------------------
	fileName	= Parameters("FILE")		->asString();
	xField		= Parameters("XFIELD")		->asInt() - 1;
	yField		= Parameters("YFIELD")		->asInt() - 1;
	zField		= Parameters("ZFIELD")		->asInt() - 1;
	nAttribs	= Parameters("ATTRIBS")		->asInt();
	bSkipHeader	= Parameters("SKIP_HEADER")	->asBool();

	switch (Parameters("FIELDSEP")->asInt())
    {
	default:
    case 0: fieldSep = '\t';	break;
    case 1: fieldSep = ' ';		break;
    case 2: fieldSep = ',';		break;
    }

	Types.Printf(SG_T("%s|%s|%s|%s|%s|"),
		_TL("1 byte integer"),
		_TL("2 byte integer"),
		_TL("4 byte integer"),
		_TL("4 byte floating point"),
		_TL("8 byte floating point")
	);

	P.Set_Name(_TL("Attribute Field Properties"));

	for(iField=1; iField<=nAttribs; iField++)
	{
		s.Printf(SG_T("NODE_%03d") , iField);
		pNode	= P.Add_Node(NULL, s, CSG_String::Format(SG_T("%d. %s"), iField, _TL("Field")), _TL(""));

		s.Printf(SG_T("FIELD_%03d"), iField);
		P.Add_String(pNode, s, _TL("Name"), _TL(""), s);

		s.Printf(SG_T("COLUMN_%03d"), iField);
		P.Add_Value(pNode, s, _TL("Attribute is Column ..."), _TL(""), PARAMETER_TYPE_Int, iField+3, 1, true);

		s.Printf(SG_T("TYPE_%03d") , iField);
		P.Add_Choice(pNode, s, _TL("Type"), _TL(""), Types, 3);
	}

	//-----------------------------------------------------
	if( nAttribs > 0 )
	{
		if( Dlg_Parameters(&P, _TL("Field Properties")) )
		{
			pPoints	= SG_Create_PointCloud();
			pPoints->Create();
			pPoints->Set_Name(SG_File_Get_Name(fileName, false));
			Parameters("POINTS")->Set_Value(pPoints);
			DataObject_Add(pPoints);

			for(iField=0; iField<nAttribs; iField++)
			{

				Name		 = P(CSG_String::Format(SG_T("FIELD_%03d" ), iField + 1).c_str())->asString();
				iType		 = P(CSG_String::Format(SG_T("TYPE_%03d"  ), iField + 1).c_str())->asInt();
				vCol.push_back(P(CSG_String::Format(SG_T("COLUMN_%03d"), iField + 1).c_str())->asInt() - 1);

				switch( iType )
				{
				default:
				case 0:	Type	= SG_DATATYPE_Char;		break;
				case 1:	Type	= SG_DATATYPE_Short;	break;
				case 2:	Type	= SG_DATATYPE_Int;		break;
				case 3:	Type	= SG_DATATYPE_Float;	break;
				case 4:	Type	= SG_DATATYPE_Double;	break;
				}

				pPoints->Add_Field(Name, Type);
			}
		}
		else
			return( false );
	}
	else
	{
		pPoints	= SG_Create_PointCloud();
		pPoints->Create();
		pPoints->Set_Name(SG_File_Get_Name(fileName, false));
		Parameters("POINTS")->Set_Value(pPoints);
		DataObject_Add(pPoints);
	}

	max_iField = M_GET_MAX(xField, yField);
    max_iField = M_GET_MAX(max_iField, zField);
	for( unsigned int i=0; i<vCol.size(); i++ )
	{
		if( max_iField < vCol.at(i) )
			max_iField = vCol.at(i);
	}

	// open input stream
    //---------------------------------------------------------
    tabStream.open(fileName.b_str(), std::ifstream::in);
    if( !tabStream )
    {
        SG_UI_Msg_Add_Error(CSG_String::Format(_TL("Unable to open input file!")));
        return (false);
    }

	tabStream.seekg(0, std::ios::end);	// get length of file
    lines = (double)tabStream.tellg();
    tabStream.seekg(0, std::ios::beg);

    std::getline(tabStream, tabLine);	// as a workaround we assume the number of lines from the length of the first line
    lines = lines / (double)tabStream.tellg();

	if( !bSkipHeader )
    {
        tabStream.clear();                      // let's forget we may have reached the EOF
        tabStream.seekg(0, std::ios::beg);      // and rewind to the beginning
    }

    
	// import
    //---------------------------------------------------------
	cntPt = cntInvalid = 0;

	SG_UI_Process_Set_Text(CSG_String::Format(_TL("Importing data ...")));

    while( std::getline(tabStream, tabLine) )
    {
        std::istringstream stream(tabLine);
        std::vector<std::string> tabCols;
        std::string tabEntry;

        if( cntPt%10000 == 0 )
            SG_UI_Process_Set_Progress((double)cntPt, lines);

        cntPt++;

        while( std::getline(stream, tabEntry, fieldSep) )      // read every column in this line and fill vector
        {
            if (tabEntry.length() == 0)
                continue;
            tabCols.push_back(tabEntry);
        }

        if ((int)tabCols.size() < max_iField - 1 )
        {
            SG_UI_Msg_Add(CSG_String::Format(_TL("WARNING: Skipping misformatted line (%.0f)!"), cntPt), true);
            cntInvalid++;
            continue;
        }

		x = strtod(tabCols[xField].c_str(), NULL);
        y = strtod(tabCols[yField].c_str(), NULL);
        z = strtod(tabCols[zField].c_str(), NULL);

		pPoints->Add_Point(x, y, z);

		for( int i=0; i<nAttribs; i++ )
		{
			value = strtod(tabCols.at(vCol.at(i)).c_str(), NULL);
			pPoints->Set_Attribute(i, value);
		}
	}

	// finalize
    //---------------------------------------------------------
	tabStream.close();

	CSG_Parameters	sParms;
	DataObject_Get_Parameters(pPoints, sParms);
	if (sParms("COLORS_ATTRIB")	&& sParms("COLORS_TYPE") && sParms("METRIC_COLORS")
		&& sParms("METRIC_ZRANGE") && sParms("COLORS_AGGREGATE"))
		{
			sParms("COLORS_AGGREGATE")->Set_Value(3);				// highest z
			sParms("COLORS_TYPE")->Set_Value(2);                    // graduated color
			sParms("METRIC_COLORS")->asColors()->Set_Count(255);    // number of colors
			sParms("COLORS_ATTRIB")->Set_Value(2);					// z attrib
			sParms("METRIC_ZRANGE")->asRange()->Set_Range(pPoints->Get_Minimum(2),pPoints->Get_Maximum(2));
			DataObject_Set_Parameters(pPoints, sParms);
			DataObject_Update(pPoints);
		}

	if (cntInvalid > 0)
        SG_UI_Msg_Add(CSG_String::Format(SG_T("%s: %d %s"), _TL("WARNING"), cntInvalid, _TL("invalid points have been skipped")), true);

    SG_UI_Msg_Add(CSG_String::Format(SG_T("%d %s"), (cntPt-cntInvalid), _TL("points have been imported with success")), true);

	return( true );
}


///////////////////////////////////////////////////////////
//														 //
//														 //
//														 //
///////////////////////////////////////////////////////////

//---------------------------------------------------------

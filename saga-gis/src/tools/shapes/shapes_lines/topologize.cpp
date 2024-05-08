#include "topologize.h"
#include <map>

CLine_Topology::CLine_Topology(void)
{
    Set_Name(_TL("Topologize Polylines"));

    Set_Author(_TL("Copyright (c) 2018-2022 by Johan Van de Wauw"));

    Set_Description(_TL(
        "The topology module will convert a shapefile containing lines to a topological network of lines: the direction of every line (i.e., the point order) is used to create a network from and to nodes."
        "<ul>"
        "<li>It will convert start and endpoints of every line to nodes</li>"
        "<li>a new line shape is created with three attributes</li>"
        "<li>the start_id and end_id will refer to the node_id, the length is the original length</li>"
        "<li>the shape is either the original shape, or a simplified straight line between the nodes if “SIMPLIFY” is selected</li>"
        "<li>if the original shapefile contains multiparts (e.g., e in the example above) each part will be exported as a separate line</li>"
        "</ul>")
    ); 

    //----------------------------------------------------

    Parameters.Add_Shapes(
        NULL, "INPUTLINES"	, _TL("Input Lines"),
        _TL(""),
        PARAMETER_INPUT,SHAPE_TYPE_Line
    );
    Parameters.Add_Shapes(
        NULL, "OUTPUTLINES"	, _TL("Output Lines"),
        _TL(""),
        PARAMETER_OUTPUT, SHAPE_TYPE_Line
    );
    Parameters.Add_Shapes(
        NULL, "OUTPUTPOINTS"	, _TL("Output Nodes"),
        _TL(""),
        PARAMETER_OUTPUT_OPTIONAL, SHAPE_TYPE_Point
    );
    Parameters.Add_Value(
        NULL, "TOLERANCE"	, _TL("Tolerance Distance"),
        _TL(""),
        PARAMETER_TYPE_Double,
        0.01
    );
    Parameters.Add_Grid(
        NULL, "SYSTEM"	, _TL("Target grid (for grid system)"),
        _TL(""), PARAMETER_INPUT_OPTIONAL
    );
    Parameters.Add_Value(
        NULL, "SIMPLIFY", _TL("Simplify"),
        _TL(""),
        PARAMETER_TYPE_Bool,
        0);

}

CLine_Topology::~CLine_Topology(void)
{}

bool CLine_Topology::On_Execute(void)
{
    CSG_Shapes *pInLines, *pOutLines, *pOutPoints;
    CSG_Shape_Line *pInLine;

    CSG_Grid *grid = Parameters("SYSTEM")->asGrid();

    CSG_Grid_System system;
    if (grid != NULL) system = grid->Get_System();

    int iPart, MaxNodeID=0;
    double tolerance;
    std::map<Vertex, int> vertices;

    pInLines	= Parameters("INPUTLINES")->asShapes();
    pOutLines = Parameters("OUTPUTLINES")->asShapes();
    pOutPoints = Parameters("OUTPUTPOINTS")->asShapes();
    tolerance = Parameters("TOLERANCE")->asDouble();
    bool simplify = Parameters("SIMPLIFY")->asBool();

    pOutLines->Set_Name(CSG_String::Format(_TL("Topology of %s"),pInLines->Get_Name()));

    if (pOutLines->Get_Field("line_id")==-1) pOutLines->Add_Field("line_id", SG_DATATYPE_Double);
    if (pOutLines->Get_Field("startpt_id")==-1) pOutLines->Add_Field("startpt_id", SG_DATATYPE_Int);
    if (pOutLines->Get_Field("endpt_id")==-1) pOutLines->Add_Field("endpt_id", SG_DATATYPE_Int);
    if (pOutLines->Get_Field("length")==-1) pOutLines->Add_Field("length", SG_DATATYPE_Double);

    if (pOutPoints != 0) {
        pOutPoints->Set_Name(CSG_String::Format(_TL("Vertices of %s"), pInLines->Get_Name()));
        if (pOutPoints->Get_Field("point_id")==-1) pOutPoints->Add_Field("point_id", SG_DATATYPE_Int);
        pOutPoints->Del_Records();
    }


    pOutLines->Del_Records();

    int line_id=0;

    for (sLong iLine = 0; iLine < pInLines->Get_Count() && SG_UI_Process_Set_Progress(iLine, pInLines->Get_Count()); iLine++)
    {
        pInLine = (CSG_Shape_Line*) pInLines->Get_Shape(iLine);

        for (iPart = 0; iPart < pInLine->Get_Part_Count(); iPart++)
        {
            // Copy the shape
            CSG_Shape *pOut = pOutLines->Add_Shape();
            pOut->Set_Value("line_id", ++line_id);
            if (simplify)
            {
                pOut->Add_Point(pInLine->Get_Point(0, iPart));
                pOut->Add_Point(pInLine->Get_Point(pInLine->Get_Point_Count(iPart)-1, iPart));
            }
            else
            {
                for (int iPoint = 0; iPoint < pInLine->Get_Point_Count(iPart); iPoint++)
                {
                    pOut->Add_Point(pInLine->Get_Point(iPoint, iPart));
                }
            }

            // Starting point of the line
            Vertex start, end;
            start =  pInLine->Get_Point(0, iPart);
            end = pInLine->Get_Point(pInLine->Get_Point_Count(iPart)-1, iPart);

            if (grid != NULL && system.is_Valid())
            {
                tolerance = 0; // we force tolerance to 0 as we convert to cell center anyway
                start.x = system.Get_xGrid_to_World(system.Get_xWorld_to_Grid(start.x));
                start.y = system.Get_yGrid_to_World(system.Get_yWorld_to_Grid(start.y));
                end.x = system.Get_xGrid_to_World(system.Get_xWorld_to_Grid(end.x));
                end.y = system.Get_yGrid_to_World(system.Get_yWorld_to_Grid(end.y));
            }
            if (tolerance != 0)
            {
                start.x = floor(start.x / tolerance + 0.5) * tolerance;
                start.y = floor(start.y / tolerance + 0.5) * tolerance;
                end.x = floor(end.x / tolerance + 0.5) * tolerance;
                end.y = floor(end.y / tolerance + 0.5) * tolerance;
            }


            auto start_it = vertices.emplace(std::map<Vertex, int>::value_type(start, -1));
            if (start_it.second)
                (*start_it.first).second = ++MaxNodeID;
            pOut->Set_Value("startpt_id", (*start_it.first).second);
            auto it = vertices.emplace(std::map<Vertex, int>::value_type(end, -1));
            if (it.second)
                (*it.first).second = ++MaxNodeID;
            pOut->Set_Value("endpt_id", (*it.first).second);
            pOut->Set_Value("length", pInLine->Get_Length(iPart));
        }
    }

    if (pOutPoints != 0) {

        for (auto iVertex = vertices.begin(); iVertex != vertices.end(); iVertex++)
        {
            CSG_Shape * pOut = pOutPoints->Add_Shape();
            pOut->Set_Value("point_id", iVertex->second);
            pOut->Add_Point(iVertex->first.x + tolerance/2, iVertex->first.y + tolerance/2);
        }
    }

    return true;
}

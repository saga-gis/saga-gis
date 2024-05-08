#ifndef UPSTREAM_EDGES_H
#define UPSTREAM_EDGES_H


#include "MLB_Interface.h"
#include <list>
#include <vector>
#include <map>


struct Node
{
    std::vector<int> from;
    std::vector<int> to;
    std::vector<int> to_edge;
    std::vector<int> from_edge;
};

struct Edge
{
    int start_node;
    int end_node;
    std::vector<int> from;
    std::vector<int> to;
    bool finished;
    std::map<int, double> proportion;
    int shreve_order;
    int sort_order;
};


class CLine_UpstreamEdges : public CSG_Tool
{
public:
    CLine_UpstreamEdges(void);
    virtual ~CLine_UpstreamEdges(void);
    virtual CSG_String		Get_MenuPath			(void)	{	return( _TL("Topology") );	}

protected:
    virtual bool			On_Execute(void);

private:
    std::map<int, Node> nodes;
    std::map<int, Edge> edges;
    void break_cycles(int edge_id, std::vector<int> upstream, int depth);
};



#endif // UPSTREAM_EDGES_H

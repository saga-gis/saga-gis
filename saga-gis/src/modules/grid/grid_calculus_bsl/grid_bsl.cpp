
#include <string>

#include "grid_bsl.h"

using namespace std;

GridWerte::GridWerte(void)
{
	xanz	= 0;
	yanz	= 0;
}

GridWerte::~GridWerte(void)
{
	freeMem();
}

void GridWerte::getMem(void)
{
	Create(SG_DATATYPE_Float, xanz, yanz, dxy, xll, yll);
}

void GridWerte::freeMem(void)
{
 	Destroy();
}

GridWerte& GridWerte::operator =(const GridWerte& W)
{
	dxy		= W.dxy;
	xll		= W.xll;
	yll		= W.yll;
	xanz	= W.xanz;
	yanz	= W.yanz;
	maxy	= W.maxy;
	miny	= W.miny;

	return( *this );
}

void GridWerte::calcMinMax(void)
{
	maxy = (*this)(0,0);
	miny = (*this)(0,0);

	for (int i=0; i<yanz; i++)
	{
		for (int j=0; j<xanz; j++)
		{
			maxy = M_GET_MAX((*this) (j,i), maxy);
			miny = M_GET_MIN((*this) (j,i), miny);
		}
	}
}

//++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++

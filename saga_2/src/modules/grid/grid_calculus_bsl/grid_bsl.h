#ifndef __GRID_H 
#define __GRID_H 


#include "gr_def.h"

#include "MLB_Interface.h"

class GridWerte :public CSG_Grid 
{
public:

	GridWerte(void);
	virtual ~GridWerte(void);

	double			dxy, xll, yll;
	long			xanz, yanz;

	void			getMem		(void);
	void			freeMem		(void);

	GridWerte &		operator =	(const GridWerte& W);

	void			calcMinMax	(void);
	double			getMax		(void) const	{ return( maxy ); };
	double			getMin		(void) const	{ return( miny ); };

	bool			getHoeheRechtsHoch	(double x, double y, double& erg) const;


private:

	double			maxy, miny;

};


#endif


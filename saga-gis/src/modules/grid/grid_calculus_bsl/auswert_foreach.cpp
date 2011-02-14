/**********************************************************
 * Version $Id$
 *********************************************************/

#include "auswert_foreach.h"

#include  "bsl_interpreter.h"


void ausfuehren_foreach(BBForEach& f)
{
	int x, y;
	y = f.M->M->yanz;
	x = f.M->M->xanz;
	if (f.type == BBForEach::Point)
	{
		// foreach p in M do
		for (f.P->v.y=0; f.P->v.y<y; (f.P->v.y)++)
		{
			if( g_Set_Progress(f.P->v.y, y) == false )
			{
				throw BBFehlerUserbreak("User Break");
			}
			for (f.P->v.x=0; f.P->v.x<x; (f.P->v.x)++)
			{
				// P setzen
				//f.P->v.x = j;
				//f.P->v.y = i;

				// Anweisungen ausführen
				ausfuehren_anweisung(f.z);
			}
		}
	}
	else 
	{
		// foreachn n of p in M do
		for (int i=-1; i<=1; i++)
		{
			for (int j=-1; j<=1; j++)
			{
				if (i != 0 || j != 0)
				{
					int vx, vy;
					vx = j+f.P->v.x;
					vy = i+f.P->v.y;
					if (vx < 0 || vx >= f.M->M->xanz ||
						vy < 0 || vy >= f.M->M->yanz)
						continue;

					// n setzen
					f.N->v.x = vx;
					f.N->v.y = vy;

					// Anweisungen ausführen
					ausfuehren_anweisung(f.z);
				}
			}
		}
	}
}

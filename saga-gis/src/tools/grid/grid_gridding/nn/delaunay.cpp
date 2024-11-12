/******************************************************************************
*
* File:           delaunay.c
*
* Created:        04/08/2000
*
* Author:         Pavel Sakov
*                 CSIRO Marine Research
*
* Purpose:        Delaunay triangulation - a wrapper to triangulate()
*
* Description:    None
*
* Revisions:      10/06/2003 PS: delaunay_build(); delaunay_destroy();
*                   struct delaunay: from now on, only shallow copy of the
*                   input data is contained in struct delaunay. This saves
*                   memory and is consistent with libcsa.
*                 30/10/2007 PS: added delaunay_addflag() and
*                   delaunay_resetflags(); modified delaunay_circles_find()
*                   to reset the flags to 0 on return. This is very important
*                   for large datasets, many thanks to John Gerschwitz,
*                   Petroleum Geo-Services, for identifying the problem.
*
*****************************************************************************/

#include <stdlib.h>
#include <stdio.h>
#include <assert.h>
#include <math.h>
#include <string.h>
#include <limits.h>
#include <float.h>
extern "C"
{
	#include "istack.h"
	#include "nan.h"
	#include "delaunay.h"
	#include "nn.h"
	#include "nn_internal.h"
}

/*
* This parameter is used in search of tricircles containing a given point:
*   if there are no more triangles than N_SEARCH_TURNON
*     do linear search
*   else
*     do more complicated stuff
*/
#define N_SEARCH_TURNON 20
#define N_FLAGS_TURNON 1000
#define N_FLAGS_INC 100

static delaunay* delaunay_create()
{
	delaunay *d = (delaunay*)malloc(sizeof(delaunay));

	d->npoints = 0;
	d->points = NULL;
	d->xmin = DBL_MAX;
	d->xmax = -DBL_MAX;
	d->ymin = DBL_MAX;
	d->ymax = -DBL_MAX;
	d->ntriangles = 0;
	d->triangles = NULL;
	d->circles = NULL;
	d->neighbours = NULL;
	d->n_point_triangles = NULL;
	d->point_triangles = NULL;
	d->nedges = 0;
	d->edges = NULL;
	d->flags = NULL;
	d->first_id = -1;
	d->t_in = NULL;
	d->t_out = NULL;
	d->nflags = 0;
	d->nflagsallocated = 0;
	d->flagids = NULL;

	return d;
}

//---------------------------------------------------------
// returns 1 if triangle nodes a,b,c are clockwise ordered
static int cw(delaunay *d, triangle *t)
{
	point *pa = &d->points[t->vids[0]];
	point *pb = &d->points[t->vids[1]];
	point *pc = &d->points[t->vids[2]];

	return( (pb->x - pa->x)*(pc->y - pa->y) < (pc->x - pa->x)*(pb->y - pa->y) );
}

//---------------------------------------------------------
#if !defined(HAVE_LIBQHULL_R_QHULL_RA_H) && !defined(HAVE_LIBQHULL_QHULL_A_H) && !defined(HAVE_QHULL_QHULL_A_H)

//---------------------------------------------------------
#include <saga_api/saga_api.h>

//---------------------------------------------------------
delaunay *delaunay_build(int np, point points[], int ns, int segments[], int nh, double holes[])
{
	CSG_TIN TIN; TIN.Add_Field("z", SG_DATATYPE_Double);

	for(int i=0; i<np; i++)
	{
		TIN.Add_Node(CSG_Point(points[i].x, points[i].y), NULL, false)->Set_Value(0, points[i].z);
	}

	TIN.Update();

	if( !TIN.is_Valid() )
	{
		return( NULL );
	}

	//-----------------------------------------------------
	delaunay *d = delaunay_create(); // if( false ) { free(d); d = NULL; }

	d->xmin = DBL_MAX; d->xmax = -DBL_MAX;
	d->ymin = DBL_MAX; d->ymax = -DBL_MAX;

	d->npoints = TIN.Get_Node_Count();
	d->points  = (point *)malloc(d->npoints * sizeof(point));

	for(int i=0; i<d->npoints; ++i)
	{
		CSG_TIN_Node &Node = *TIN.Get_Node(i);

		point *p = &d->points[i];

		p->x = Node.Get_X(); p->y = Node.Get_Y(); p->z = Node.asDouble(0);

		if( d->npoints < np ) // CSG_TIN might have removed duplicate points!!!
		{
			points[i].x = p->x; points[i].y = p->y; points[i].z = p->z;
		}

		if( p->x < d->xmin ) { d->xmin = p->x; } else if( p->x > d->xmax ) { d->xmax = p->x; }
		if( p->y < d->ymin ) { d->ymin = p->y; } else if( p->y > d->ymax ) { d->ymax = p->y; }
	}

	d->ntriangles = TIN.Get_Triangle_Count();
	d->triangles  = (triangle            *)malloc(d->ntriangles * sizeof(triangle           ));
	d->neighbours = (triangle_neighbours *)malloc(d->ntriangles * sizeof(triangle_neighbours));
	d->circles    = (circle              *)malloc(d->ntriangles * sizeof(circle             ));

	for(sLong iTriangle=0; iTriangle<TIN.Get_Triangle_Count(); iTriangle++)
	{
		CSG_TIN_Triangle &Triangle = *TIN.Get_Triangle(iTriangle);

		triangle            *t = &d->triangles [iTriangle];
		triangle_neighbours *n = &d->neighbours[iTriangle];
		circle              *c = &d->circles   [iTriangle];

		t->vids[0] = (int)Triangle.Get_Node(0)->Get_Index();
		t->vids[1] = (int)Triangle.Get_Node(1)->Get_Index();
		t->vids[2] = (int)Triangle.Get_Node(2)->Get_Index();

		n->tids[0] = -1;
		n->tids[1] = -1;
		n->tids[2] = -1;

		if( cw(d, t) )
		{
			{ int tmp = t->vids[1]; t->vids[1] = t->vids[2]; t->vids[2] = tmp; }
			{ int tmp = n->tids[1]; n->tids[1] = n->tids[2]; n->tids[2] = tmp; }
		}

		circle_build1(c, &d->points[t->vids[0]], &d->points[t->vids[1]], &d->points[t->vids[2]]);
	}

	d->flags = (int *)calloc(d->ntriangles, sizeof(int));

	d->n_point_triangles = (int *)calloc(d->npoints, sizeof(int));
	for(int i=0; i<d->ntriangles; ++i)
	{
		triangle *t = &d->triangles[i];

		for(int j=0; j<3; ++j)
			d->n_point_triangles[t->vids[j]]++;
	}

	d->point_triangles = (int **)malloc(d->npoints * sizeof(int *));
	for(int i=0; i<d->npoints; ++i)
	{
		if (d->n_point_triangles[i] > 0)
			d->point_triangles[i] = (int *)malloc(d->n_point_triangles[i] * sizeof(int));
		else
			d->point_triangles[i] = NULL;
		d->n_point_triangles[i] = 0;
	}

	for(int i=0; i<d->ntriangles; ++i)
	{
		triangle *t = &d->triangles[i];

		for(int j=0; j<3; ++j)
		{
			int vid = t->vids[j];

			d->point_triangles[vid][d->n_point_triangles[vid]] = i;
			d->n_point_triangles[vid]++;
		}
	}

	d->nedges   = 0;
	d->edges    = NULL;
	d->t_in     = NULL;
	d->t_out    = NULL;
	d->first_id = -1;

	return( d );
}

//---------------------------------------------------------
#else // #if defined(HAVE_LIBQHULL_R_QHULL_RA_H) || defined(HAVE_LIBQHULL_QHULL_A_H) || defined(HAVE_QHULL_QHULL_A_H)

//---------------------------------------------------------
#ifdef HAVE_LIBQHULL_R_QHULL_RA_H
#include <libqhull_r/qhull_ra.h>
#elif HAVE_LIBQHULL_QHULL_A_H
#include <libqhull/qhull_a.h>
#else
#ifdef HAVE_QHULL_QHULL_A_H
#include <qhull/qhull_a.h>
#else
#error Failed to include qhull_a.h. Is Qhull installed?
#endif
#endif

delaunay* delaunay_build(int np, point points[], int ns, int segments[], int nh, double holes[])
{
	delaunay* d = delaunay_create();

	coordT *qpoints;                     /* array of coordinates for each point */
	boolT ismalloc = False;              /* True if qhull should free points */
	char flags[64] = "qhull d Qbb Qt";   /* option flags for qhull */
	facetT *facet,*neighbor,**neighborp; /* variables to walk through facets */
	vertexT *vertex, **vertexp;          /* variables to walk through vertex */

	int curlong, totlong;                /* memory remaining after qh_memfreeshort */
	FILE *outfile = stdout;
	FILE *errfile = stderr;              /* error messages from qhull code */

	int i, j;
	int exitcode;
	int dim, ntriangles;
	int numfacets, numsimplicial, numridges, totneighbors, numcoplanars, numtricoplanars;	 

	dim = 2;

	assert(sizeof(realT) == sizeof(double)); /* Qhull was compiled with doubles? */

	if (np == 0 || ns > 0 || nh > 0) {
		fprintf(stderr, "segments=%d holes=%d\n, aborting Qhull implementation, use 'triangle' instead.\n", ns, nh);
		free(d);
		return NULL;
	}

	qpoints = (coordT *) malloc(np * (dim+1) * sizeof(coordT));

	for (i=0; i<np; i++) {
		qpoints[i*dim] = points[i].x;
		qpoints[i*dim+1] = points[i].y;
	}

	if (!nn_verbose)
		outfile = NULL;
	if (nn_verbose)
		strcat(flags, " s");
	if (nn_verbose > 1)
		strcat(flags, " Ts");

	if (nn_verbose)
		fflush(stderr);

	/*
	* climax 
	*/

	#ifdef HAVE_LIBQHULL_R_QHULL_RA_H
	qhT qh_qh;
	qhT *qh= &qh_qh;

	QHULL_LIB_CHECK

		qh_zero(qh, errfile);

	exitcode = qh_new_qhull (qh, dim, np, qpoints, ismalloc,
		flags, outfile, errfile);
	#else
	exitcode = qh_new_qhull (dim, np, qpoints, ismalloc,
		flags, outfile, errfile);
	#endif

	if(!exitcode) {

		if (nn_verbose)
			fflush(stderr);

		d->xmin = DBL_MAX;
		d->xmax = -DBL_MAX;
		d->ymin = DBL_MAX;
		d->ymax = -DBL_MAX;

		d->npoints = np;
		d->points = (point *)malloc(np * sizeof(point));
		for (i = 0; i < np; ++i) {
			point* p = &d->points[i];

			p->x = points[i].x;
			p->y = points[i].y;
			p->z = points[i].z;

			if (p->x < d->xmin)
				d->xmin = p->x;
			if (p->x > d->xmax)
				d->xmax = p->x;
			if (p->y < d->ymin)
				d->ymin = p->y;
			if (p->y > d->ymax)
				d->ymax = p->y;
		}

		if (nn_verbose) {
			fprintf(stderr, "input:\n");
			for (i = 0; i < np; ++i) {
				point* p = &d->points[i];

				fprintf(stderr, "  %d: %15.7g %15.7g %15.7g\n",
					i, p->x, p->y, p->z);
			}
		}

		#ifdef HAVE_LIBQHULL_R_QHULL_RA_H
		qh_findgood_all (qh, qh->facet_list);
		qh_countfacets (qh, qh->facet_list, NULL, !qh_ALL, &numfacets,
			&numsimplicial, &totneighbors, &numridges,
			&numcoplanars, &numtricoplanars);
		#else
		qh_findgood_all (qh facet_list);
		qh_countfacets (qh facet_list, NULL, !qh_ALL, &numfacets,
			&numsimplicial, &totneighbors, &numridges,
			&numcoplanars, &numtricoplanars);
		#endif

		ntriangles = 0;
		FORALLfacets {
			if (!facet->upperdelaunay && facet->simplicial)
			ntriangles++;
		}

		d->ntriangles = ntriangles;
		d->triangles  = (triangle            *)malloc(d->ntriangles * sizeof(triangle));
		d->neighbours = (triangle_neighbours *)malloc(d->ntriangles * sizeof(triangle_neighbours));
		d->circles    = (circle              *)malloc(d->ntriangles * sizeof(circle));

		if (nn_verbose)
			fprintf(stderr, "triangles:\tneighbors:\n");

		i = 0;      
		FORALLfacets {
			if (!facet->upperdelaunay && facet->simplicial) {
				triangle* t = &d->triangles[i];        
				triangle_neighbours* n = &d->neighbours[i];
				circle* c = &d->circles[i];

				j = 0;
				FOREACHvertex_(facet->vertices)

					#ifdef HAVE_LIBQHULL_R_QHULL_RA_H
					t->vids[j++] = qh_pointid(qh, vertex->point);
				#else
					t->vids[j++] = qh_pointid(vertex->point);
				#endif

				j = 0;
				FOREACHneighbor_(facet)
					n->tids[j++] = neighbor->visitid ? neighbor->visitid - 1 : - 1;

				/* Put triangle vertices in counterclockwise order, as
				* 'triangle' do.
				* The same needs to be done with the neighbors.
				*
				* The following works, i.e., it seems that Qhull maintains a
				* relationship between the vertices and the neighbors
				* triangles, but that is not said anywhere, so if this stop
				* working in a future Qhull release, you know what you have
				* to do, reorder the neighbors.
				*/

				if(cw(d, t)) {
					int tmp = t->vids[1];
					t->vids[1] = t->vids[2];
					t->vids[2] = tmp;

					tmp = n->tids[1];
					n->tids[1] = n->tids[2];
					n->tids[2] = tmp;
				}

				circle_build1(c, &d->points[t->vids[0]], &d->points[t->vids[1]],
					&d->points[t->vids[2]]);

				if (nn_verbose)
					fprintf(stderr, "  %d: (%d,%d,%d)\t(%d,%d,%d)\n",
						i, t->vids[0], t->vids[1], t->vids[2], n->tids[0],
						n->tids[1], n->tids[2]);

				i++;
			}
		}

		d->flags = (int *)calloc(d->ntriangles, sizeof(int));

		d->n_point_triangles = (int *)calloc(d->npoints, sizeof(int));
		for (i = 0; i < d->ntriangles; ++i) {
			triangle* t = &d->triangles[i];

			for (j = 0; j < 3; ++j)
				d->n_point_triangles[t->vids[j]]++;
		}
		d->point_triangles = (int **)malloc(d->npoints * sizeof(int*));
		for (i = 0; i < d->npoints; ++i) {
			if (d->n_point_triangles[i] > 0)
				d->point_triangles[i] = (int *)malloc(d->n_point_triangles[i] * sizeof(int));
			else
				d->point_triangles[i] = NULL;
			d->n_point_triangles[i] = 0;
		}
		for (i = 0; i < d->ntriangles; ++i) {
			triangle* t = &d->triangles[i];

			for (j = 0; j < 3; ++j) {
				int vid = t->vids[j];

				d->point_triangles[vid][d->n_point_triangles[vid]] = i;
				d->n_point_triangles[vid]++;
			}
		}

		d->nedges = 0;
		d->edges = NULL;

		d->t_in = NULL;
		d->t_out = NULL;
		d->first_id = -1;

	} else {
		free(d);
		d = NULL;
	}

	free(qpoints);

	#ifdef HAVE_LIBQHULL_R_QHULL_RA_H
	qh_freeqhull(qh, !qh_ALL);                 /* free long memory */
	qh_memfreeshort (qh, &curlong, &totlong);  /* free short memory and memory allocator */
	#else
	qh_freeqhull(!qh_ALL);                 /* free long memory */
	qh_memfreeshort (&curlong, &totlong);  /* free short memory and memory allocator */
	#endif

	if (curlong || totlong) 
		fprintf (errfile,
			"qhull: did not free %d bytes of long memory (%d pieces)\n",
			totlong, curlong);

	return d;
}
#endif // #if defined(HAVE_LIBQHULL_R_QHULL_RA_H) || defined(HAVE_LIBQHULL_QHULL_A_H) || defined(HAVE_QHULL_QHULL_A_H)


/* Destroys Delaunay triangulation.
*
* @param d Structure to be destroyed
*/
void delaunay_destroy(delaunay* d)
{
	if (d == NULL)
		return;

	if (d->point_triangles != NULL) {
		int i;

		for (i = 0; i < d->npoints; ++i)
			if (d->point_triangles[i] != NULL)
				free(d->point_triangles[i]);
		free(d->point_triangles);
	}
	if (d->nedges > 0)
		free(d->edges);
	#if defined(HAVE_LIBQHULL_R_QHULL_RA_H) || defined(HAVE_LIBQHULL_QHULL_A_H) || defined(HAVE_QHULL_QHULL_A_H)
	/* This is a shallow copy if we're not using qhull so we don't
	* need to free it */
	if (d->points != NULL)
		free(d->points);
	#endif
	if (d->n_point_triangles != NULL)
		free(d->n_point_triangles);
	if (d->flags != NULL)
		free(d->flags);
	if (d->circles != NULL)
		free(d->circles);
	if (d->neighbours != NULL)
		free(d->neighbours);
	if (d->triangles != NULL)
		free(d->triangles);
	if (d->t_in != NULL)
		istack_destroy(d->t_in);
	if (d->t_out != NULL)
		istack_destroy(d->t_out);
	if (d->flagids != NULL)
		free(d->flagids);
	free(d);
}

/* Returns whether the point p is on the right side of the vector (p0, p1).
*/
static int onrightside(point* p, point* p0, point* p1)
{
	return (p1->x - p->x) * (p0->y - p->y) > (p0->x - p->x) * (p1->y - p->y);
}

/* Finds triangle specified point belongs to (if any).
*
* @param d Delaunay triangulation
* @param p Point to be mapped
* @param seed Triangle index to start with
* @return Triangle id if successful, -1 otherwise
*/
int delaunay_xytoi(delaunay* d, point* p, int id)
{
	triangle* t;
	int i;

	if (p->x < d->xmin || p->x > d->xmax || p->y < d->ymin || p->y > d->ymax)
		return -1;

	if (id < 0 || id > d->ntriangles)
		id = 0;
	t = &d->triangles[id];
	do {
		for (i = 0; i < 3; ++i) {
			int i1 = (i + 1) % 3;

			if (onrightside(p, &d->points[t->vids[i]], &d->points[t->vids[i1]])) {
				id = d->neighbours[id].tids[(i + 2) % 3];
				if (id < 0)
					return id;
				t = &d->triangles[id];
				break;
			}
		}
	} while (i < 3);

	return id;
}

static void delaunay_addflag(delaunay* d, int i)
{
	if (d->nflags == d->nflagsallocated) {
		d->nflagsallocated += N_FLAGS_INC;
		d->flagids = (int *)realloc(d->flagids, d->nflagsallocated * sizeof(int));
	}
	d->flagids[d->nflags] = i;
	d->nflags++;
}

static void delaunay_resetflags(delaunay* d)
{
	int i;

	for (i = 0; i < d->nflags; ++i)
		d->flags[d->flagids[i]] = 0;
	d->nflags = 0;
}

/* Finds all tricircles specified point belongs to.
*
* @param d Delaunay triangulation
* @param p Point to be mapped
* @param n Pointer to the number of tricircles within `d' containing `p'
*          (output)
* @param out Pointer to an array of indices of the corresponding triangles 
*            [n] (output)
*
* There is a standard search procedure involving search through triangle
* neighbours (not through vertex neighbours). It must be a bit faster due to
* the smaller number of triangle neighbours (3 per triangle) but may fail
* for a point outside convex hall.
*
* We may wish to modify this procedure in future: first check if the point
* is inside the convex hall, and depending on that use one of the two
* search algorithms. It not 100% clear though whether this will lead to a
* substantial speed gains because of the check on convex hall involved.
*/
void delaunay_circles_find(delaunay* d, point* p, int* n, int** out)
{
	/*
	* This flag was introduced as a hack to handle some degenerate cases. It 
	* is set to 1 only if the triangle associated with the first circle is
	* already known to contain the point. In this case the circle is assumed 
	* to contain the point without a check. In my practice this turned
	* useful in some cases when point p coincided with one of the vertices
	* of a thin triangle. 
	*/
	int contains = 0;
	int i;

	if (d->t_in == NULL) {
		d->t_in = istack_create();
		d->t_out = istack_create();
	}

	/*
	* if there are only a few data points, do linear search
	*/
	if (d->ntriangles <= N_SEARCH_TURNON) {
		istack_reset(d->t_out);

		for (i = 0; i < d->ntriangles; ++i) {
			if (circle_contains(&d->circles[i], p)) {
				istack_push(d->t_out, i);
			}
		}

		*n = d->t_out->n;
		*out = d->t_out->v;

		return;
	}
	/*
	* otherwise, do a more complicated stuff
	*/

	/*
	* It is important to have a reasonable seed here. If the last search
	* was successful -- start with the last found tricircle, otherwise (i)
	* try to find a triangle containing p; if fails then (ii) check
	* tricircles from the last search; if fails then (iii) make linear
	* search through all tricircles 
	*/
	if (d->first_id < 0 || !circle_contains(&d->circles[d->first_id], p)) {
		/*
		* if any triangle contains p -- start with this triangle 
		*/
		d->first_id = delaunay_xytoi(d, p, d->first_id);
		contains = (d->first_id >= 0);

		/*
		* if no triangle contains p, there still is a chance that it is
		* inside some of circumcircles 
		*/
		if (d->first_id < 0) {
			int nn = d->t_out->n;
			int tid = -1;

			/*
			* first check results of the last search 
			*/
			for (i = 0; i < nn; ++i) {
				tid = d->t_out->v[i];
				if (circle_contains(&d->circles[tid], p))
					break;
			}
			/*
			* if unsuccessful, search through all circles 
			*/
			if (tid < 0 || i == nn) {
				double nt = d->ntriangles;

				for (tid = 0; tid < nt; ++tid) {
					if (circle_contains(&d->circles[tid], p))
						break;
				}
				if (tid == nt) {
					istack_reset(d->t_out);
					*n = 0;
					*out = NULL;
					return;     /* failed */
				}
			}
			d->first_id = tid;
		}
	}

	istack_reset(d->t_in);
	istack_reset(d->t_out);

	istack_push(d->t_in, d->first_id);
	d->flags[d->first_id] = 1;
	delaunay_addflag(d, d->first_id);

	/*
	* main cycle 
	*/
	while (d->t_in->n > 0) {
		int tid = istack_pop(d->t_in);
		triangle* t = &d->triangles[tid];

		if (contains || circle_contains(&d->circles[tid], p)) {
			istack_push(d->t_out, tid);
			for (i = 0; i < 3; ++i) {
				int vid = t->vids[i];
				int nt = d->n_point_triangles[vid];
				int j;

				for (j = 0; j < nt; ++j) {
					int ntid = d->point_triangles[vid][j];

					if (d->flags[ntid] == 0) {
						istack_push(d->t_in, ntid);
						d->flags[ntid] = 1;
						delaunay_addflag(d, ntid);
					}
				}
			}
		}
		contains = 0;
	}

	*n = d->t_out->n;
	*out = d->t_out->v;
	delaunay_resetflags(d);
}

/**********************************************************
 * Version $Id$
 *********************************************************/

///////////////////////////////////////////////////////////
//                                                       //
//                         SAGA                          //
//                                                       //
//      System for Automated Geoscientific Analyses      //
//                                                       //
//                     Tool Library                      //
//                      Grid_Filter                      //
//                                                       //
//-------------------------------------------------------//
//                                                       //
//                      storeorg.h                       //
//                                                       //
//                 Copyright (C) 2013 by                 //
//                     HfT Stuttgart                     //
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
//    e-mail:     johannes.engels@hft-stuttgart.de       //
//                                                       //
//    contact:    Johannes Engels                        //
//                Hochschule fuer Technik Stuttgart      //
//                Schellingstr. 24                       //
//                70174 Stuttgart                        //
//                Germany                                //
//                                                       //
///////////////////////////////////////////////////////////

//---------------------------------------------------------


///////////////////////////////////////////////////////////
//														 //
//                                                       //
//														 //
///////////////////////////////////////////////////////////

//---------------------------------------------------------
//#ifdef _cplusplus
//extern "C" {
//#endif

#include <stdio.h>
#include <stddef.h>
#include <stdlib.h>

#define NR_END 1

#define BASIS_MALLOC  malloc
#define BASIS_CALLOC  calloc
#define BASIS_FREE    free
#define BASIS_REALLOC realloc

#if 0
#define BASIS_MALLOC  basis_malloc
#define BASIS_CALLOC  basis_calloc
#define BASIS_FREE    basis_free
#define BASIS_REALLOC basis_realloc
#endif

#define ANZ_MEMPOOL   10

void *chain_malloc (long size, short mempool);

void chain_free (void *palt);

void *chain_realloc (void *palt, long newsize, short mempool);

void chain_integrity (short mempool);

void chain_all_free (short mempool);


size_t element_length (char type);

void *vector_alloc (long n, char type);


/**********************************************************************/
/* einen double-Vektor mit Indexbereich   0 ... n   allokieren und    */
/* initialisieren. Es wird also immer ein Element mehr allokiert als  */
/* im Prinzip noetig waere.                                           */
/**********************************************************************/

double *dvector_alloc (long n);





/**********************************************************************/
/* Double-Matrix mit Indexbereich                                     */
/*                   indexing, ... num_rows + indexing - 1 (Zeilen),  */
/*                   indexing, ... num_cols + indexing - 1 (Spalten)  */
/* allokieren                                                         */
/* Hier wird nur der Vektor von Zeigern auf die Zeilen allokiert und  */
/* gesetzt. indexing kann 0 oder 1 sein.                              */
/* Der erste Zeiger m [indexing] zeigt auf den mitgegebenen Vektor a, */
/* so dass m [indexing][indexing] = a [indexing].                     */
/**********************************************************************/

double **dmatrix_pointer_alloc (double *a,
                                long num_rows,
                                long num_cols,
                                short indexing);




/**********************************************************************/
/* Double-Matrix mit Indexbereich                                     */
/*                   indexing, ... num_rows + indexing - 1 (Zeilen),  */
/*                   indexing, ... num_cols + indexing - 1 (Spalten)  */
/* allokieren                                                         */
/* Hier werden der double-Vektor a fuer die Matrixelemente (gesamte   */
/* Groesse der Matrix) und ein Vektor m von Zeigern auf die Zeilen    */
/* der Matrix allokiert. Der double-Vektor a wird initialisiert, die  */
/* Elemente des Zeiger-Vektors m werden auf die "Zeilenanfaenge"      */
/* (also die entsprechenden Elemente von a) gesetzt. indexing gibt    */
/* an, ob die Indizierung der Matrix bei 0 oder bei 1 beginnen soll.  */
/* indexing kann 0 oder 1 sein.                                       */
/* Der erste Zeiger m [indexing] zeigt auf den mitgegebenen Vektor a, */
/* so dass m [indexing][indexing] = a [indexing].                     */
/**********************************************************************/

double **dmatrix_all_alloc (long zn, long sn, short indexing);

/**********************************************************************/
/* Matrix mit Indexbereich                                            */
/*                   indexing, ... num_rows + indexing - 1 (Zeilen),  */
/*                   indexing, ... num_cols + indexing - 1 (Spalten)  */
/* allokieren                                                         */
/* Hier wird nur der Vektor von Zeigern auf die Zeilen allokiert und  */
/* gesetzt. indexing kann 0 oder 1 sein.                              */
/* Der erste Zeiger m [indexing] zeigt auf den mitgegebenen Vektor a, */
/* so dass m [indexing][indexing] = a [indexing].                     */
/*                                                                    */
/* Folgende Typen sind moeglich:  D  -- double                        */
/*                                F  -- float                         */
/*                                I  -- int                           */
/*                                S  -- short                         */
/*                                L  -- long                          */
/*                                C  -- char                          */
/*                                U  -- unsigned char                 */
/**********************************************************************/

void **matrix_pointer_alloc (void *a,
                             long num_rows,
                             long num_cols,
                             char type,
                             short indexing);

void ***array_3_pointer_alloc (void *a,
                               long num_rows,
                               long num_cols,
                               long num_piles,
                               char type,
                               short indexing);

void **matrix_all_alloc (long zn, long sn, char type, short indexing);

void ***array_3_all_alloc (long zn,
                           long sn,
                           long hn,
                           char type,
                           short indexing);

void ****array_4_pointer_alloc (void *a,
                                long num_rows,
                                long num_cols,
                                long num_levs,
                                long num_elems,
                                char type,
                                short indexing);

void *basis_malloc (long size);

void integritaet_pruefen (void);

int adr_in_kette_finden (void *adr);

void *basis_calloc (long length, int obj_size);

void basis_free (void *palt);

void integritaet_speziell (void *palt);

void *basis_realloc (void *palt, long newsize);




/***********************************************************/
/* einen Vektor mit Indexbereich 1 ... n allokieren        */
/* und initialisieren                                      */
/***********************************************************/

double *basis_dvector_alloc (long n);


#if 0
float *fvector_alloc (long n);

float *fvector_alloc_1 (long n);

/***********************************************************/
/* einen Vektor mit Indexbereich 0 ... n - 1 allokieren    */
/* und initialisieren                                      */
/***********************************************************/


/***********************************************************/
/* einen Vektor mit Indexbereich 1 ... n allokieren        */
/* und initialisieren                                      */
/***********************************************************/

int *ivector_alloc (long n);

int *ivector_alloc_1 (long n);
#endif


/***********************************************************/
/* einen Short-Vektor mit Indexbereich 1 ... n allokieren   */
/* und initialisieren                                      */
/***********************************************************/


void *vector_realloc (void *v, long n_alt, long n_neu, char type);

double *dvector_realloc (double *v, long n_alt, long n_neu);

double *dvector_realloc_1 (double *v, long n_alt, long n_neu);


/************************************/
/* einen int-Vektor freigeben       */
/************************************/

void ivector_free (int *v);



/************************************/
/* einen double-Vektor freigeben    */
/************************************/

void dvector_free (double *v);

void fvector_free (float *v);


/***************************************************************/
/* einen Vektor freigeben                                      */
/***************************************************************/

void vector_free (void *v);


/***********************************************************/
/* eine Matrix mit Indexbereich 1, ... zn (Zeilen)         */
/*                              1, ... sn (Spalten)        */
/* allokieren                                              */
/* Hier wird nur der Vektor von Zeigern allokiert und      */
/* gesetzt. Der erste Zeiger m [1] zeigt auf den           */
/* mitgegebenen Vektor a, so dass m [1][1] = a [1].        */
/***********************************************************/

double **dmatrix_zeiger_alloc (double *a, long zn, long sn);

double **dmatrix_zeiger_alloc_1 (double *a, long zn, long sn);

#if 0
int **imatrix_zeiger_alloc (int *a, long zn, long sn);

int **imatrix_zeiger_alloc_1 (int *a, long zn, long sn);
#endif


double **basis_dmatrix_zeiger_alloc (double *a, long zn, long sn);

double **basis_dmatrix_zeiger_alloc_1 (double *a, long zn, long sn);

#if 0
float **fmatrix_zeiger_alloc (float *a, long zn, long sn);


/***********************************************************/
/* eine Matrix mit Indexbereich i = 1, ... n (Zeilen)      */
/*                              j = i, ... n (Spalten)     */
/* allokieren                                              */
/* Hier wird nur der Vektor von Zeigern allokiert und      */
/* gesetzt. Der erste Zeiger m [1] zeigt auf den           */
/* mitgegebenen Vektor a, so dass m [1][1] = a [1].        */
/***********************************************************/

float **fmatrix_zeiger_alloc_1 (float *a, long zn, long sn);
#endif

double **doberes_dreieck_zeiger_alloc (double *a, long n);



/**********************************************************************/
/* Matrix mit Indexbereich                                            */
/*               i = indexing, ... num_rows + indexing - 1 (Zeilen),  */
/*               j =        i, ... num_cols + indexing - 1 (Spalten)  */
/* allokieren                                                         */
/* Hier wird nur der Vektor von Zeigern auf die Zeilen allokiert und  */
/* gesetzt. indexing kann 0 oder 1 sein.                              */
/* Der erste Zeiger m [indexing] zeigt auf den mitgegebenen Vektor a, */
/* so dass m [indexing][indexing] = a [indexing].                     */
/*                                                                    */
/* Folgende Typen sind moeglich:  D  -- double                        */
/*                                F  -- float                         */
/*                                I  -- int                           */
/*                                S  -- short                         */
/*                                L  -- long                          */
/*                                C  -- char                          */
/*                                U  -- unsigned char                 */
/**********************************************************************/

void **upper_triangle_pointer_alloc (void *a,
                                     long num_rows,
                                     char type,
                                     short indexing);


void **upper_triangle_all_alloc (long num_rows,
                                 char type,
                                 short indexing);




/***********************************************************/
/* eine Matrix mit Indexbereich 0, ... zn - 1 (Zeilen)     */
/*                              0, ... sn - 1 (Spalten)    */
/* allokieren                                              */
/* Hier wird nur der Vektor von Zeigern allokiert und      */
/* gesetzt. Der erste Zeiger m [0] zeigt auf den           */
/* mitgegebenen Vektor a, so dass m [0][0] = a [0].        */
/***********************************************************/

double **basis_dmatrix_zeiger_alloc_0 (double *a, long zn, long sn);





void dmatrix_zeiger_free (double **m);

void matrix_pointer_free (void **m);

#if 0
float **fmatrix_all_alloc (long zn, long sn);

float **fmatrix_all_alloc_1 (long zn, long sn);
#endif

double **basis_dmatrix_all_alloc (long zn, long sn);

double **basis_dmatrix_all_alloc_1 (long zn, long sn);


/******************************************************************/
/* obere Dreiecks Matrix mit Indexbereich i = 1, ... n (Zeilen)   */
/*                                        j = i, ... n (Spalten)  */
/* allokieren                                                     */
/******************************************************************/

double **doberes_dreieck_all_alloc (long n);

double **doberes_dreieck_all_alloc_1 (long n);

/***********************************************************/
/* eine Matrix mit Indexbereich 0, ... zn - 1 (Zeilen)     */
/*                              0, ... sn - 1 (Spalten)    */
/* allokieren                                              */
/* Hier werden der double-Vektor a für die Matrixelemente  */
/* (gesamte Größe der Matrix) und ein Vektor m von Zeigern */
/* allokiert. Der double Vektor a wird initialisiert, die  */
/* Elemente des Zeiger-vektors m werden auf die "Zeilen-   */
/* anfänge" (entsprechende Elemente von a) gesetzt,        */
/* so dass m [0][0] = a [0].                               */
/***********************************************************/

double **dmatrix_all_alloc_0 (long zn, long sn);

double **basis_dmatrix_all_alloc_0 (long zn, long sn);



void **matrix_realloc_zeilen (void **m,
                              long zn_alt,
                              long zn_neu,
                              long sn,
                              char type,
                              short indexing);
#if 0
int **imatrix_all_alloc (long zn, long sn);

int **imatrix_all_alloc_1 (long zn, long sn);
#endif

/***********************************************************/
/* den Vektor mit Zeigern auf die Zeilen der Matrix        */
/* sowie den Vektor der Matrixelemente freigeben           */
/* (Gegenstueck zu dmatrix_all_alloc)                      */
/***********************************************************/

void dmatrix_all_free (double **m);

void fmatrix_all_free (float **m);

void smatrix_all_free (short **m);


void matrix_all_free (void **m);

/**********************************************************/
/* Speicherplatz fuer Legendre-Funktionen bis zum         */
/* Entwicklungsgrad maxdeg allokieren und initialisieren  */
/* Es wird sowohl der double-Vektor der Elemente          */
/* allokiert und mit Nullen vorbesetzt als auch der       */
/* Zeigervektor (Zeiger auf die Zeilen der Legendre-      */
/* Matrix (Grade)) allokiert und gesetzt.                 */
/**********************************************************/

int legendre_dreieck_alloc (int maxdeg, double ***lf_anker);


double **legendre_dreieck_alloc_neu (int maxdeg);


   /***********************************************************/
   /* den Vektor mit Zeigern auf die Zeilen der Legendre-     */
   /* Matrix sowie den Vektor der Matrixelemente freigeben    */
   /* (Gegenstueck zu legendre_dreieck_alloc)                 */
   /***********************************************************/


int legendre_dreieck_free (double ***lf_anker);




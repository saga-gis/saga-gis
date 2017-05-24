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
//              Grid_Filter / Grid_Calculus              //
//                                                       //
//-------------------------------------------------------//
//                                                       //
//                      spezfunc.h                       //
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
#ifndef SPEZ
#define SPEZ



typedef int (* daten_funktion) (int anz_mer,
                                double t_sin_phi,
                                double **lf,
                                char spiegel,
                                double *ergebnis,
                                void *arg1,
                                void *arg2,
                                void *arg3,
                                void *arg4,
                                void *arg5,
                                void *arg6);




/**********************************************************/
/* Prototypen der externen Funktionen in spezfunc.c       */
/**********************************************************/

int read_coefficients (const char *filename,
	                   int mindegree,
                       int maxdegree,
                       double ***c_lm,
                       double ***s_lm);



/*******************************************************************/
/* Nicht normierte Legendre-Polynome bis zum angegebenen           */
/* Entwicklungsgrad maxdeg rechnen                                 */
/* t = sin (phi) muss mitgegeben werden                            */
/* Der Vektor lp muss im aufrufenden Programm genügend groß        */
/* allokiert sein (z.B. durch Funktion dvector_alloc in speiorg.c) */
/*******************************************************************/

int leg_pol_berechnen (double t,
                       int maxdeg,
                       double *lp);


/*******************************************************************/
/* Vollständig normierte Legendre-Funktionen bis zum angegebenen   */
/* Entwicklungsgrad maxdeg rechnen                                 */
/* t = sin (phi) muss mitgegeben werden                            */
/* Die Matrix lf muss im aufrufenden Programm genügend groß        */
/* allokiert sein (z.B. durch Funktion legendre_dreieck_alloc      */
/* in speiorg.c)                                                   */
/*******************************************************************/

int leg_func_berechnen (double t,
                        int maxdeg,
                        double **lf);


/*******************************************************************/
/* Ableitungen der vollständig normierten Legendre-Funktionen nach */
/* dem Argument t = sin (phi) bis zum angegebenen Entwicklungsgrad */
/* maxdeg rechnen                                                  */
/* t = sin (phi) sowie die Matrix lf der Legendre-Funktionen       */
/* selbst müssen mitgegeben werden                                 */
/* Die Matrix lf_der muss im aufrufenden Programm genügend groß    */
/* allokiert sein (z.B. durch Funktion legendre_dreieck_alloc      */
/* in speiorg.c)                                                   */
/*******************************************************************/

int leg_func_deriv (double t,
                    int maxdeg,
                    double **lf,
                    double **lf_der);




/*******************************************************************/
/* Harmonische Synthese in einem Einzelpunkt rechnen               */
/*                                                                 */
/* f = sum_{l=mindeg}^maxdeg  *                                    */
/*           sum_{m=0}^l leg_func_{l,m} (c_lm * cos (m*lambdar) +  */
/*                                       s_lm * sin (m*lambdar))   */
/*                                                                 */
/*                                                                 */
/* Winkelmass = 'A': lambda muss in Altgrad eingegeben werden      */
/* Winkelmass = 'R': lambda muss im Bogenmaß eingegeben werden     */
/*                                                                 */
/* mindeg muss >= 0 sein, andernfalls wird es gewaltsam auf 0      */
/* gesetzt                                                         */
/*******************************************************************/

int kff_synthese_einzelpunkt (double lambda,
                              char winkelmass,
                              double **leg_func,
                              int mindeg,
                              int maxdeg,
                              double **c_lm,
                              double **s_lm,
                              double *f);




/*******************************************************************/
/* Harmonische Synthese in einem Einzelpunkt rechnen               */
/*                                                                 */
/* wie kff_synthese_einzelpunkt_S, aber es wird das Ergebnis nicht */
/* für die Breite phi gewünscht, mit der die Legendre-Funktionen   */
/* berechnet wurden, sondern für die Breite -phi                   */
/* ("Wert im symmetrisch zur Äquatorebene liegenden Punkt")        */
/*                                                                 */
/* Winkelmass = 'A': lambda muss in Altgrad eingegeben werden      */
/* Winkelmass = 'R': lambda muss im Bogenmaß eingegeben werden     */
/*                                                                 */
/* mindeg muss >= 0 sein, andernfalls wird es gewaltsam auf 0      */
/* gesetzt                                                         */
/*******************************************************************/

int kff_synthese_einzelpunkt_s (double lambda,
                                char winkelmass,
                                double **leg_func,
                                int mindeg,
                                int maxdeg,
                                double **c_lm,
                                double **s_lm,
                                double *f);







/*******************************************************************/
/* Harmonische Synthese auf einem Breitenkreis rechnen             */
/*                                                                 */
/* f = sum_{l=mindeg}^maxdeg  *                                    */
/*           sum_{m=0}^l leg_func_{l,m} (c_lm * cos (m*lambdar) +  */
/*                                       s_lm * sin (m*lambdar))   */
/*                                                                 */
/* Winkelmass = 'A': lambda_inc, lambda_anf und lambda_end müssen  */
/*                   in Altgrad eingegeben werden                  */
/* Winkelmass = 'R': lambda_inc, lambda_anf und lambda_end müssen  */
/*                   im Bogenmaß eingegeben werden                 */
/*                                                                 */
/* Es wird ab lambda_anf bis lambda_end mit lambda_inc als         */
/* Inkrement gerechnet.                                            */
/* Der Vektor f muss im aufrufenden Modul genügend groß allokiert  */
/* sein: Mindestgröße ist                                          */
/*        (lambda_end - lambda_anf) / lambda_inc + 1               */
/*                                                                 */
/* mindeg muss >= 0 sein, andernfalls wird es gewaltsam auf 0      */
/* gesetzt                                                         */
/*******************************************************************/

int kff_synthese_breitenkreis (double lambda_inc,
                               double lambda_anf,
                               double lambda_end,
                               char winkelmass,
                               double **leg_func,
                               int mindeg,
                               int maxdeg,
                               double **c_lm,
                               double **s_lm,
                               double *f);




/*******************************************************************/
/* Harmonische Synthese auf einem Breitenkreis rechnen             */
/*                                                                 */
/* Im Gegensatz zu kff_synthese_breitenkreis wird hier voraus-     */
/* gesetzt, dass die Länge lambda aller Gitterpunkte jeweils ein   */
/* ganzzahliges Vielfaches des Inkrementes lambda_inc              */
/* 2 * pi / anz_mer und dass lambda = 360 Grad ein ganzzahliges    */
/* Vielfaches von lambda_inc ist.                                  */
/* Damit vereinfacht sich die Berechnung.                          */
/*                                                                 */
/* f = sum_{l=mindeg}^maxdeg  *                                    */
/*           sum_{m=0}^l leg_func_{l,m} (c_lm * cos (m*lambdar) +  */
/*                                       s_lm * sin (m*lambdar))   */
/*                                                                 */
/* Es wird von 0 <= lambda < 360 Grad gerechnet                    */
/* Der Vektor f muss im aufrufenden Modul genügend groß allokiert  */
/* sein: Mindestgröße ist anz_mer                                  */
/*                                                                 */
/* Die Vektoren cos_la und sin_la müssen bereits die               */
/* Werte                                                           */
/*          cos_la [k] = cos (k * lambda_inc)                      */
/*          sin_la [k] = sin (k * lambda_inc)                      */
/*                                                                 */
/* für 0 <= k < anz_mer enthalten.                                 */
/*                                                                 */
/* mindeg muss >= 0 sein.                                          */
/*                                                                 */
/* Ist spiegel = 'S', so wird das Ergebnis nicht für die Breite    */
/* phi berechnet, mit der die Legendre-Funktionen                  */
/* berechnet wurden, sondern für die Breite -phi                   */
/* ("Wert im symmetrisch zur Äquatorebene liegenden Punkt")        */
/*                                                                 */
/*******************************************************************/

int kff_synthese_bk_ng (int anz_mer,
                        double **leg_func,
                        double *cos_la,
                        double *sin_la,
                        int mindeg,
                        int maxdeg,
                        char spiegel,
                        double **c_lm,
                        double **s_lm,
                        double *f);




/*******************************************************************/
/* Harmonische Synthese auf einem regelmäßigen Gitter rechnen      */
/*                                                                 */
/* f = sum_{l=mindeg}^maxdeg  *                                    */
/*           sum_{m=0}^l leg_func_{l,m} (c_lm * cos (m*lambdar) +  */
/*                                       s_lm * sin (m*lambdar))   */
/*                                                                 */
/* Winkelmass = 'A':                                               */
/* winkel_inc, lambda_anf, lambda_end, phi_anf und phi_end müssen  */
/* in Altgrad mitgegeben werden                                    */
/*                                                                 */
/* Winkelmass = 'R':                                               */
/* winkel_inc, lambda_anf, lambda_end, phi_anf und phi_end müssen  */
/* im Bogenmaß mitgegeben werden                                   */
/*                                                                 */
/* Die Matrix f muss im aufrufenden Modul genügend groß allokiert  */
/* sein: Mindestgröße ist                                          */
/*       [(phi_end - phi_anf) / winkel_inc + 1] *                  */
/*       [(lambda_end - lambda_anf) / winkel_inc + 1]              */
/*                                                                 */
/* mindeg muss >= 0 sein, andernfalls wird es gewaltsam auf 0      */
/* gesetzt                                                         */
/*******************************************************************/

int kff_synthese_regel_gitter (double winkel_inc,
                               double phi_anf,
                               double phi_end,
                               double lambda_anf,
                               double lambda_end,
                               char winkelmass,
                               int mindeg,
                               int maxdeg,
                               double **c_lm,
                               double **s_lm,
                               double **f,
                               char **error_liste);

int kff_synthese_regel_gitter_m (double winkel_inc,
                                 double phi_anf,
                                 double phi_end,
                                 double lambda_anf,
                                 double lambda_end,
								 int numlat,
								 int numlong,
                                 char winkelmass,
                                 int mindeg,
                                 int maxdeg,
                                 double **c_lm,
                                 double **s_lm,
                                 double **f,
                                 char **error_liste);


/**************************************************************/
/* Programm zur harmonischen Analyse mit Hilfe einer          */
/* Gauss-Quadratur. Die Nullstellen des Legendre-Polynoms     */
/* vom Grad lmax (Entwicklungsgrad) sowie die Gewichte werden */
/* vom Eingabefile inpfile gelesen. lmax muss gerade sein !!  */
/**************************************************************/

int harm_ana_gauss (FILE *inpfile,
                    FILE *outfile,
                    int lmax,
                    daten_funktion dat_funk,
                    void *arg1,
                    void *arg2,
                    void *arg3,
                    void *arg4,
                    void *arg5,
                    void *arg6,
                    char **error_liste);

#endif


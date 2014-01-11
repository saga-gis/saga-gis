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
//              Grid_Filter / Grid_Calculus              //
//                                                       //
//-------------------------------------------------------//
//                                                       //
//                      spezfunc.c                       //
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
// 51 Franklin Street, 5th Floor, Boston, MA 02110-1301, //
// USA.                                                  //
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
#include <string.h>
#include <stdio.h>
#include <stdlib.h>
#include <math.h>

#define M_PI 3.141592653589793

#include "storeorg.h"
#include "spezfunc.h"
#include "rho.h"
#include "errfunc.h"


static fehlerinfo finfo [] =
        { { 1001, 'E', "    Fehler in legendre_dreieck_alloc, "
                       "lmax = %d\n"                           },
          { 1002, 'E', "    Fehler beim Einlesen der "
                       "Nullstellen, Zeile %d \n" },
          { 1003, 'E', "    Fehlerhafte Indizierung im Nullstellenfile,"
                       " Grad (ist)  = %d,\n"
                       "                                               "
                       " Grad (soll) = %d,\n" },
          { 1004, 'F', "    Fehler in der Funktion zur "
                       "Berechnung der Gitterwerte \n" },
          { 1005, 'E', "    Testausgabe: "
                       "lmax = %d, name = %s \n"  },
          {    0, 'F', ""  } };



int read_coefficients (const char *filename,
	                   int mindegree,
                       int maxdegree,
                       double ***c_lm,
                       double ***s_lm)
{
	FILE *fh;
    int l;
    int m;
    int degree;
    int order;
    double c;
    double s;


	fh = fopen (filename, "r");

    legendre_dreieck_alloc (maxdegree, c_lm);
	legendre_dreieck_alloc (maxdegree, s_lm);

	for (l = mindegree; l <= maxdegree; l ++)
	{
	    fscanf (fh, "%d %d %lf %lf", &degree, &order, &c, &s);

        if (degree != l || order != 0)
           printf ("Error: Wrong order of coefficients in input file\n");

	    (*c_lm) [l][0] = c;

		for (m = 1; m <= l; m ++)
		{
	        fscanf (fh, "%d %d %lf %lf", &degree, &order, &c, &s);

	        if (degree != l || order != m)
               printf ("Error: Wrong order of coefficients in input file\n");

       	    (*c_lm) [l][m] = c;
	        (*s_lm) [l][m] = s;
	    }
    }

	fclose (fh);

	return 0;
}




int leg_pol_berechnen (double t,
                       int maxdeg,
                       double *lp)
{
   /*******************************************************************/
   /* Nicht normierte Legendre-Polynome bis zum angegebenen           */
   /* Entwicklungsgrad maxdeg rechnen                                 */
   /* t = sin (phi) muss mitgegeben werden                            */
   /* Der Vektor lp muss im aufrufenden Programm genügend groß        */
   /* allokiert sein (z.B. durch Funktion dvector_alloc in speiorg.c) */
   /*******************************************************************/

   short n;


   lp [0] = 1.0;
   lp [1] = t;

   for (n = 2; n <= maxdeg; n ++)
   {
      lp [n] = (-lp [n - 2] * (n - 1) +
                 t * lp [n - 1] * (2 * n - 1)) / n;
   }

   return 0;
}




int leg_func_berechnen (double t,
                        int maxdeg,
                        double **lf)
{
   /*******************************************************************/
   /* Vollständig normierte Legendre-Funktionen bis zum angegebenen   */
   /* Entwicklungsgrad maxdeg rechnen                                 */
   /* t = sin (phi) muss mitgegeben werden                            */
   /* Die Matrix lf muss im aufrufenden Programm genügend groß        */
   /* allokiert sein (z.B. durch Funktion legendre_dreieck_alloc      */
   /* in speiorg.c)                                                   */
   /*******************************************************************/

   short grad;
   short ord;
   double cf;
   double *wurz;
   short i;
   short ip;
   short im;

   wurz = (double *) malloc ((2 * maxdeg + 4) * sizeof (double));

   for (i = 0; i < 2 * maxdeg + 4; i ++)
      wurz [i] = sqrt ((double ) (i));


   cf = sqrt (1.0 - t * t);

   lf [0][0] = 1.0;
   lf [1][1] = wurz [3] * cf;

   for (grad = 1; grad < maxdeg; grad ++)
   {
      /*****************************************/
      /* Hauptdiagonalelemente berechnen       */
      /*****************************************/

      lf [grad + 1][grad + 1] =
              wurz [2 * grad + 3] / wurz [2 * grad + 2] *
              cf * lf [grad][grad];
   }

   for (ord = 0; ord < maxdeg; ord ++)
   {
      lf [ord + 1][ord] = wurz [2 * ord + 3] * t * lf [ord][ord];

      for (grad = ord + 1; grad < maxdeg; grad ++)
      {
         i = 2 * grad;
         ip = grad + ord;
         im = grad - ord;

         lf [grad + 1][ord] =
               wurz [i + 3] / wurz [ip + 1] / wurz [im + 1] *
               (wurz [i + 1] * t * lf [grad][ord] -
                wurz [ip] * wurz [im] / wurz [i - 1] *
                                                 lf [grad - 1][ord]);
      }
   }

   free (wurz);

   return 0;
}




int leg_func_deriv (double t,
                    int maxdeg,
                    double **lf,
                    double **lf_der)
{
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

   short grad;
   short ord;
   double cf;
   double *wurz;
   short i;
   short ip;
   short im;

   wurz = (double *) malloc ((2 * maxdeg + 4) * sizeof (double));

   for (i = 0; i < 2 * maxdeg + 4; i ++)
      wurz [i] = sqrt ((double ) (i));


   cf = sqrt (1.0 - t * t);

   lf_der [0][0] = 0.0;
   lf_der [1][1] = -wurz [3] * t;

   for (grad = 1; grad < maxdeg; grad ++)
   {
      /*****************************************/
      /* Hauptdiagonalelemente berechnen       */
      /*****************************************/

      lf_der [grad + 1][grad + 1] =
              wurz [2 * grad + 3] / wurz [2 * grad + 2] *
              (cf * lf_der [grad][grad] - t * lf [grad][grad]);
   }

   for (ord = 0; ord < maxdeg; ord ++)
   {
      lf_der [ord + 1][ord] =
                       wurz [2 * ord + 3] * (t * lf_der [ord][ord] +
                                             cf * lf [ord][ord]);

      for (grad = ord + 1; grad < maxdeg; grad ++)
      {
         i = 2 * grad;
         ip = grad + ord;
         im = grad - ord;

         lf_der [grad + 1][ord] =
               wurz [i + 3] / wurz [ip + 1] / wurz [im + 1] *
               (wurz [i + 1] * (t * lf_der [grad][ord] +
                                cf * lf [grad][ord]) -
                wurz [ip] * wurz [im] / wurz [i - 1] *
                                              lf_der [grad - 1][ord]);
      }
   }

   free (wurz);

   return 0;
}



int kff_synthese_einzelpunkt (double lambda,
                              char winkelmass,
                              double **leg_func,
                              int mindeg,
                              int maxdeg,
                              double **c_lm,
                              double **s_lm,
                              double *f)
{
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

   double fzw;
   int l;
   int m;

   *f = 0.;

   mindeg = (mindeg < 0) ? 0 : mindeg;

   if (winkelmass == 'A')
      lambda *= RHO_REZ;

   for (l = mindeg; l <= maxdeg; l ++)
   {
      fzw = leg_func [l][0] * c_lm [l][0];

      for (m = 1; m <= l; m ++)
      {
         fzw += (leg_func [l][m] *
                 (c_lm [l][m] * cos (m * lambda) +
                  s_lm [l][m] * sin (m * lambda)));
      }

      *f += fzw;
   }

   return 0;
}





int kff_synthese_einzelpunkt_s (double lambda,
                                char winkelmass,
                                double **leg_func,
                                int mindeg,
                                int maxdeg,
                                double **c_lm,
                                double **s_lm,
                                double *f)
{
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

   double fzw;
   int l;
   int m;
   int sig_alt;
   int sig_alt_st;

   *f = 0.;

   mindeg = (mindeg < 0) ? 0 : mindeg;

   if (winkelmass == 'A')
      lambda *= RHO_REZ;

   if (mindeg % 2 == 0)
      sig_alt_st = -1;
   else
      sig_alt_st = 1;

   for (l = mindeg; l <= maxdeg; l ++)
   {
      sig_alt_st = -sig_alt_st;
      sig_alt = sig_alt_st;

      if (sig_alt == -1)
      {
         fzw = -leg_func [l][0] * c_lm [l][0];
      }
      else
      {
         fzw = leg_func [l][0] * c_lm [l][0];
      }

      for (m = 1; m <= l; m ++)
      {
         sig_alt = -sig_alt;

         if (sig_alt == -1)
         {
            fzw -= (leg_func [l][m] *
                    (c_lm [l][m] * cos (m * lambda) +
                     s_lm [l][m] * sin (m * lambda)));
         }
         else
         {
            fzw += (leg_func [l][m] *
                    (c_lm [l][m] * cos (m * lambda) +
                     s_lm [l][m] * sin (m * lambda)));
         }
      }

      *f += fzw;
   }

   return 0;
}





int kff_synthese_breitenkreis (double lambda_inc,
                               double lambda_anf,
                               double lambda_end,
                               char winkelmass,
                               double **leg_func,
                               int mindeg,
                               int maxdeg,
                               double **c_lm,
                               double **s_lm,
                               double *f)
{
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

   double fzw;
   double lambda;
   int l;
   int m;
   int i;


   mindeg = (mindeg < 0) ? 0 : mindeg;

   if (winkelmass == 'A')
   {
      lambda_inc *= RHO_REZ;
      lambda_anf *= RHO_REZ;
      lambda_end *= RHO_REZ;
   }


   for (i = 0, lambda = lambda_anf;
        lambda <= lambda_end;
        i ++ ,lambda += lambda_inc)
   {
      f [i] = 0.;

      for (l = mindeg; l <= maxdeg; l ++)
      {
         fzw = leg_func [l][0] * c_lm [l][0];

         for (m = 1; m <= l; m ++)
         {
            fzw += (leg_func [l][m] *
                    (c_lm [l][m] * cos (m * lambda) +
                     s_lm [l][m] * sin (m * lambda)));
         }

         f [i] += fzw;
      }
   }

   return 0;
}





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
                               char **error_liste)
{
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

   double fzw;
   double lambda;
   double phi;
   double sphi;
   double **lf;
   int l;
   int m;
   int i;
   int j;
   int rc;
   char *funk_name = "kff_synthese_regel_gitter";


   mindeg = (mindeg < 0) ? 0 : mindeg;

   if (winkelmass == 'A')
   {
      winkel_inc *= RHO_REZ;
      lambda_anf *= RHO_REZ;
      lambda_end *= RHO_REZ;
      phi_anf *= RHO_REZ;
      phi_end *= RHO_REZ;
   }

   rc = legendre_dreieck_alloc (maxdeg, &lf);
   ERRCHECK2_RC(1001,finfo,&maxdeg,NULL)

   for (i = 0, phi = phi_anf;
        phi <= phi_end;
        i ++, phi += winkel_inc)
   {
      sphi = sin (phi);

      leg_func_berechnen (sphi, maxdeg, lf);


      for (j = 0, lambda = lambda_anf;
           lambda <= lambda_end;
           j ++, lambda += winkel_inc)
      {
         f [i][j] = 0.;

         for (l = mindeg; l <= maxdeg; l ++)
         {
            fzw = lf [l][0] * c_lm [l][0];

            for (m = 1; m <= l; m ++)
            {
               fzw += (lf [l][m] *
                       (c_lm [l][m] * cos (m * lambda) +
                        s_lm [l][m] * sin (m * lambda)));
            }

            f [i][j] += fzw;
         }
      }
   }

   legendre_dreieck_free (&lf);

   return 0;
}





static int trigfuncs_gitter (double winkel_inc,
                             double lambda_anf,
                             double lambda_end,
	        			     int maxdeg,
                             double **cosmla,
                             double **sinmla)
{
   /*******************************************************************/
   /* Trigonometrische Funktionen cos (m lambda), sin (m lambda)      */
   /* mit Additionstheoremen berechnen                                */
   /*******************************************************************/

   double lambda;
   int m;
   int j;
   double cosl1;
   double sinl1;

   char *funk_name = "trigfuncs_gitter";

   for (j = 0, lambda = lambda_anf;
        lambda <= lambda_end;
        j ++, lambda += winkel_inc)
   {
	   cosmla [j][0] = 1.0;
	   sinmla [j][0] = 0.0;
	   cosmla [j][1] = cos (lambda);
	   sinmla [j][1] = sin (lambda);
	   cosl1 = cosmla [j][1];
	   sinl1 = sinmla [j][1];

	   for (m = 2; m <= maxdeg; m ++)
	   {
		   cosmla [j][m] = cosmla [j][m-1] * cosl1 - sinmla [j][m-1] * sinl1;
  		   sinmla [j][m] = sinmla [j][m-1] * cosl1 + cosmla [j][m-1] * sinl1;
	   }
   }

   return 0;
}





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
                                 char **error_liste)
{
   /**************************************************************************/
   /* Wie kff_synthese_regel_gitter, aber die cos (m lambda), sin (m lambda) */
   /* werden vorab gerechnet und gespeichert. Geht schneller und braucht     */
   /* mehr Speicherplatz.                                                    */
   /**************************************************************************/

   double fzw;
   double lambda;
   double phi;
   double sphi;
   double **lf;
   double **cosmla;
   double **sinmla;
   int l;
   int m;
   int i;
   int j;
   int rc;
   char *funk_name = "kff_synthese_regel_gitter_m";


   mindeg = (mindeg < 0) ? 0 : mindeg;

   if (winkelmass == 'A')
   {
      winkel_inc *= RHO_REZ;
      lambda_anf *= RHO_REZ;
      lambda_end *= RHO_REZ;
      phi_anf *= RHO_REZ;
      phi_end *= RHO_REZ;
   }

   rc = legendre_dreieck_alloc (maxdeg, &lf);
   ERRCHECK2_RC(1001,finfo,&maxdeg,NULL)

   cosmla = (double **) matrix_all_alloc (numlong, maxdeg + 1, 'D', 0);
   sinmla = (double **) matrix_all_alloc (numlong, maxdeg + 1, 'D', 0);

   trigfuncs_gitter (winkel_inc,
                     lambda_anf,
                     lambda_end,
	        		 maxdeg,
                     cosmla,
                     sinmla);


   for (i = 0, phi = phi_anf;
        phi <= phi_end;
        i ++, phi += winkel_inc)
   {
      sphi = sin (phi);

      leg_func_berechnen (sphi, maxdeg, lf);


      for (j = 0, lambda = lambda_anf;
           lambda <= lambda_end;
           j ++, lambda += winkel_inc)
      {
         f [i][j] = 0.;

         for (l = mindeg; l <= maxdeg; l ++)
         {
            fzw = lf [l][0] * c_lm [l][0];

            for (m = 1; m <= l; m ++)
            {
               fzw += (lf [l][m] *
                       (c_lm [l][m] * cosmla [j][m] +
                        s_lm [l][m] * sinmla [j][m]));
            }

            f [i][j] += fzw;
         }
      }
   }

   legendre_dreieck_free (&lf);
   matrix_all_free ((void **) cosmla);
   matrix_all_free ((void **) sinmla);

   return 0;
}













int kff_synthese_bk_ng (int anz_mer,
                        double **leg_func,
                        double *cos_la,
                        double *sin_la,
                        int mindeg,
                        int maxdeg,
                        char spiegel,
                        double **c_lm,
                        double **s_lm,
                        double *f)
{
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

   int l;
   int m;
   int k;
   int tri_ind;
   int sig_alt;
   int sig_alt_st;

   double cfac;
   double sfac;


   for (k = 0; k < anz_mer; k ++)
      f [k] = 0.0;


   if (spiegel == 'S')
   {
      if (mindeg % 2 == 0)
         sig_alt_st = -1;
      else
         sig_alt_st = 1;


      for (l = mindeg; l <= maxdeg; l ++)
      {
         sig_alt_st = -sig_alt_st;
         sig_alt = sig_alt_st;

         for (m = 0; m <= l; m ++)
         {
            if (sig_alt > 0)
            {
               cfac = leg_func [l][m] * c_lm [l][m];
               sfac = leg_func [l][m] * s_lm [l][m];
            }
            else
            {
               cfac = -leg_func [l][m] * c_lm [l][m];
               sfac = -leg_func [l][m] * s_lm [l][m];
            }

            for (k = 0, tri_ind = 0; k < anz_mer; k ++)
            {
               f [k] += (cfac * cos_la [tri_ind] +
                         sfac * sin_la [tri_ind]);

               tri_ind += m;
               tri_ind %= anz_mer;
            }

            sig_alt = -sig_alt;
         }
      }
   }
   else
   {
      for (l = mindeg; l <= maxdeg; l ++)
      {
         for (m = 0; m <= l; m ++)
         {
            cfac = leg_func [l][m] * c_lm [l][m];
            sfac = leg_func [l][m] * s_lm [l][m];

            for (k = 0, tri_ind = 0; k < anz_mer; k ++)
            {
               f [k] += (cfac * cos_la [tri_ind] +
                         sfac * sin_la [tri_ind]);

               tri_ind += m;
               tri_ind %= anz_mer;
            }
         }
      }
   }

   return 0;
}





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
                    char **error_liste)
{
   /**************************************************************/
   /* Programm zur harmonischen Analyse mit Hilfe einer          */
   /* Gauss-Quadratur. Die Nullstellen des Legendre-Polynoms     */
   /* vom Grad lmax (Entwicklungsgrad) sowie die Gewichte werden */
   /* vom Eingabefile inpfile gelesen. lmax muss gerade sein!!   */
   /**************************************************************/

   int rc;
   int l;
   int m;
   int j;
   int k;
   int dummy;
   int anz_m;
   int tri_ind;
   int sig_alt;
   int sig_alt_st;

   double **lf;
   double **clm;
   double **slm;
   double alpha;
   double t_nullst;
   double lambda;
   double la_inc;
   double *f_nord;
   double *f_sued;
   double *cos_la;
   double *sin_la;
   double *c_sum_nord;
   double *s_sum_nord;
   double *c_sum_sued;
   double *s_sum_sued;

   char zeile [82];
   char *cp;
   char *cp5;
   char *funk_name = "harm_ana_gauss";


   cp5 = funk_name;

   anz_m = 2 * lmax;
   la_inc = M_PI / lmax;


   /***************************/
   /* Speicher allokieren     */
   /***************************/

   cos_la = (double *) malloc ((size_t) (anz_m) * sizeof (double));
   sin_la = (double *) malloc ((size_t) (anz_m) * sizeof (double));

   f_nord = (double *) malloc ((size_t) (anz_m) * sizeof (double));
   f_sued = (double *) malloc ((size_t) (anz_m) * sizeof (double));

   c_sum_nord = (double *) malloc ((size_t) (lmax + 1) *
                                            sizeof (double));
   s_sum_nord = (double *) malloc ((size_t) (lmax + 1) *
                                            sizeof (double));
   c_sum_sued = (double *) malloc ((size_t) (lmax + 1) *
                                            sizeof (double));
   s_sum_sued = (double *) malloc ((size_t) (lmax + 1) *
                                            sizeof (double));

   rc = legendre_dreieck_alloc (lmax, &lf);
   ERRCHECK2_RC(1001,finfo,&lmax,NULL)

   rc = legendre_dreieck_alloc (lmax, &clm);
   ERRCHECK2_RC(1001,finfo,&lmax,NULL)

   rc = legendre_dreieck_alloc (lmax, &slm);
   ERRCHECK2_RC(1001,finfo,&lmax,NULL)


   /******************************************************/
   /* Berechnung von cos lambda, sin lambda auf den      */
   /* Gittermeridianen                                   */
   /******************************************************/

   for (k = 0, lambda = 0.0; k < anz_m; k ++, lambda += la_inc)
   {
      cos_la [k] = cos (lambda);
      sin_la [k] = sin (lambda);
   }


   /******************************************************/
   /* Schleife über die Breitenkreise                    */
   /******************************************************/


   for (j = 1; j <= lmax / 2; j ++)
   {
      /****************************************/
      /* Nullstelle und Gewicht einlesen      */
      /****************************************/

      cp = fgets (zeile, 80, inpfile);
      ERRCHECK2_NULL(cp,1002,finfo,&j,NULL)

#if 0
      if (j == 2)
      {
         rc = 5;
         ERRCHECK2_RC_R(1005,finfo,&lmax,cp5)
      }


      if ((cp = fgets (zeile, 80, inpfile)) == NULL)
      {
         printf ("Fehler beim Einlesen der Nullstellen\n");
         exit (20);
      }
#endif

      sscanf (zeile, "%d %lf %lf", &dummy, &t_nullst, &alpha);

#if 0
      dummy = 7;

      printf ("Achtung, j = %d", j);
#endif

      if (dummy != j)
      {
#if 0
         fehler2 (__LINE__,1003,__FILE__,funk_name,stdout,finfo,
                  dummy, j, NULL, NULL, NULL, NULL, NULL);
#endif
         error_message (__LINE__,1003,__FILE__,funk_name,error_liste,
                        finfo, &dummy, &j, NULL, NULL, NULL, NULL, NULL);
      }


      /****************************************/
      /* Legendre-Funktionen berechnen        */
      /****************************************/

      rc =  leg_func_berechnen (t_nullst, lmax, lf);


      for (m = 0; m <= lmax; m ++)
      {
         c_sum_nord [m] = 0.0;
         s_sum_nord [m] = 0.0;
         c_sum_sued [m] = 0.0;
         s_sum_sued [m] = 0.0;
      }


      /****************************************/
      /* Funktionswerte berechnen (auf Nord-  */
      /* und Südhalbkugel)                    */
      /****************************************/

      rc = (*dat_funk) (anz_m,
                        t_nullst,
                        lf,
                        'N',
                        f_nord,
                        arg1,
                        arg2,
                        arg3,
                        arg4,
                        arg5,
                        arg6);

      ERRCHECK2_RC_R(1004,finfo,NULL,NULL)

      rc = (*dat_funk) (anz_m,
                        -t_nullst,
                        lf,
                        'S',
                        f_sued,
                        arg1,
                        arg2,
                        arg3,
                        arg4,
                        arg5,
                        arg6);

      ERRCHECK2_RC_R(1004,finfo,NULL,NULL)

      /***************************************************/
      /* Schleife über die Meridiane                     */
      /***************************************************/

      for (k = 0; k < anz_m; k ++)
      {
         /****************************************/
         /* Schleife über die Ordnung der        */
         /* Kff-Koeffiezienten                   */
         /****************************************/

         c_sum_nord [0] += f_nord [k];
         c_sum_sued [0] += f_sued [k];


         for (m = 1, tri_ind = 0; m <= lmax; m ++)
         {
            /***************************************************/
            /* Aufsummieren:                                   */
            /*                                                 */
            /*     anz_m - 1                     cos m*lambda  */
            /* Summe          f (lambda, phi) *                */
            /*     k=0                           sin m*lambda  */
            /*                                                 */
            /***************************************************/

            tri_ind += k;
            tri_ind %= anz_m;

            c_sum_nord [m] += (cos_la [tri_ind] * f_nord [k]);
            s_sum_nord [m] += (sin_la [tri_ind] * f_nord [k]);
            c_sum_sued [m] += (cos_la [tri_ind] * f_sued [k]);
            s_sum_sued [m] += (sin_la [tri_ind] * f_sued [k]);
         }
      }


      /*****************************************/
      /* Schleife über den Grad der            */
      /* Kff-Koeffiezienten                    */
      /*****************************************/

      sig_alt_st = -1;

      for (l = 0; l <= lmax; l ++)
      {
         sig_alt_st = -sig_alt_st;
         sig_alt = sig_alt_st;

         clm [l][0] += (lf [l][0] * alpha *
                          (c_sum_nord [0] + sig_alt * c_sum_sued [0]));


         for (m = 1; m <= l; m ++)
         {
            /***********************************/
            /* Gewicht auf Legendre-Funktionen */
            /* draufmultiplizieren             */
            /***********************************/

            sig_alt = -sig_alt;

            clm [l][m] += (lf [l][m] * alpha *
                           (c_sum_nord [m] + sig_alt * c_sum_sued [m]));
            slm [l][m] += (lf [l][m] * alpha *
                           (s_sum_nord [m] + sig_alt * s_sum_sued [m]));
         }
      }
   }


   for (l = 0; l <= lmax; l ++)
   {
      clm [l][0] /= (2. * (double) (anz_m));

      for (m = 1; m <= l; m ++)
      {
         clm [l][m] /= (2. * (double) (anz_m));
         slm [l][m] /= (2. * (double) (anz_m));
      }
   }


   /*************************************************/
   /* Berechnete KFF-Koeffizienten ausgeben         */
   /*************************************************/

   for (l = 0; l <= lmax; l ++)
   {
      for (m = 0; m <= l; m ++)
      {
         fprintf (outfile, "%3d%3d%19.12e%19.12e\n",
                           l, m, clm [l][m], slm [l][m]);
      }
   }


   free (c_sum_nord);
   free (s_sum_nord);
   free (c_sum_sued);
   free (s_sum_sued);

   free (cos_la);
   free (sin_la);

   legendre_dreieck_free (&clm);
   legendre_dreieck_free (&slm);
   legendre_dreieck_free (&lf);

   return 0;
}



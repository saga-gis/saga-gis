// interplation.cc
//#include "../stdafx.h"

#include <iostream>
#include <math.h>
#include <stdlib.h>

#include "diverses.h"
#include "interpolation.h"


#define false 0
#define true 1


double LinPol(double x0, double y0, double x1, double y1, double x)
{
	if (x1 == x0)
		return y0;
	return (y1-y0)/(x1-x0)*(x-x0) + y0;
}


Interpolation::Interpolation( GridWerte& I) : W(I)
{
}

void Interpolation::setParams(double minx, double miny, double dx, int anzx, int anzy)
{
	MinX = minx;
	MinY = miny;
	DX = dx;
	AnzahlX = anzx;
	AnzahlY = anzy;
}

void Interpolation::setZwischenwerte(void)
{
	MinX = 0.5f;
	MinY = 0.5f;
	DX = 1.0f;
	AnzahlX = W.xanz -1;
	AnzahlY = W.yanz -1;
}

void Interpolation::setDoppeltewerte(void)
{
	MinX = 0.0f;
	MinY = 0.0f;
	DX = 0.5f;
	AnzahlX = 2*W.xanz -1;
	AnzahlY = 2*W.yanz -1;
}

bool Interpolation::IsOk(void)
{
	return (	(MinX + AnzahlX*DX <= W.xanz)
			&&  (MinY + AnzahlY*DX <= W.yanz)
			&&  (MinX > 0)
			&&  (MinY > 0)
			&&  (DX > 0)
			&&  (AnzahlX > 0)
			&&  (AnzahlY > 0) );

}
//++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
//   Spline    + Splinefloat
//++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++

/*
void QSpline::spline(double x[],double y[],int n, double yp1,double ypn,double y2[], double tmp_u[])
{
	int i,k;
	double p,qn,sig,un, *u = tmp_u;

	
	if (yp1 > 0.99e30)
		y2[1]=u[1]=0.0f;
	else {
		y2[1] = -0.5f;
		u[1]=(3.0f/(x[2]-x[1]))*((y[2]-y[1])/(x[2]-x[1])-yp1);
	}
	for (i=2;i<=n-1;i++) {
		sig=(x[i]-x[i-1])/(x[i+1]-x[i-1]);
		p=sig*y2[i-1]+2.0f;
		y2[i]=(sig-1.0f)/p;
		u[i]=(y[i+1]-y[i])/(x[i+1]-x[i]) - (y[i]-y[i-1])/(x[i]-x[i-1]);
		u[i]=(6.0f*u[i]/(x[i+1]-x[i-1])-sig*u[i-1])/p;
	}
	if (ypn > 0.99e30f)
		qn=un=0.0f;
	else {
		qn=0.5f;
		un=(3.0f/(x[n]-x[n-1]))*(ypn-(y[n]-y[n-1])/(x[n]-x[n-1]));
	}
	y2[n]=(un-qn*u[n-1])/(qn*y2[n-1]+1.0f);
	for (k=n-1;k>=1;k--)
		y2[k]=y2[k]*y2[k+1]+u[k];
}

void QSpline::splint(double xa[],double ya[],double y2a[],int n, double x,double *y)
{
	int klo,khi,k;
	double h,b,a;

	klo=1;
	khi=n;
	while (khi-klo > 1) {
		k=(khi+klo) >> 1;
		if (xa[k] > x) khi=k;
		else klo=k;
	}
	h=xa[khi]-xa[klo];
	if (h == 0.0) 
		throw InterpolError("Bad XA input to routine SPLINT");
	a=(xa[khi]-x)/h;
	b=(x-xa[klo])/h;
	*y=a*ya[klo]+b*ya[khi]+((a*a*a-a)*y2a[klo]+(b*b*b-b)*y2a[khi])*(h*h)/6.0f;
}

void QSpline::splie2(double x1a[], double x2a[], double **ya, int m, int n, double **y2a) 
{
	int j;
	double *tmp_u = new double [n];
	for (j=1;j<=m;j++)
		spline(x2a,ya[j],n,1.0e30f,1.0e30f,y2a[j], tmp_u);
	delete[] tmp_u;
}


void QSpline::splin2(double x1a[], double x2a[], double **ya, double **y2a,
					 int m, int n, double x1, double x2, double  *y,
					 double *ytmp, double *yytmp, double *tmp_u)
{
	int j;
	for (j=1;j<=m;j++)
		splint(x2a,ya[j],y2a[j],n,x2,&yytmp[j]);

	spline(x1a,yytmp,m,1.0e30f,1.0e30f,ytmp, tmp_u);
	splint(x1a,yytmp,ytmp,m,x1,y);
}

void QSpline::mysplin(	double x1a[], double x2a[], double **ya, double **y2a,
						 GridWerte& Erg, int mm, int nn, 
						double *ytmp, double *yytmp, double *tmp_u)
{
	int j, m, n;
	double y, x1, x2;

	//m = Erg.yanz+1;  // vorher G.yanz
	//n = Erg.xanz+1;  // vorher G.xanz

	m = AnzahlY+1; 
	n = AnzahlX+1; 

	for (int n2=1; n2<n; n2++)
	{
		//x2 = double(n2)-0.5; // x
		x2 = double(n2-1)*DX + MinX; // x

		for (j=1;j<=mm;j++)
			splint(x2a,ya[j],y2a[j],nn,x2,&yytmp[j]);

		spline(x1a,yytmp,mm,1.0e30f,1.0e30f,ytmp, tmp_u);
		for (int m2=1; m2<m; m2++)
		{
			//x1 = double(m2)-0.5; // y 
			x1 = double(m2-1)*DX+MinY; // y 

			splint(x1a,yytmp,ytmp,mm,x1,&y);
			Erg(n2-1,m2-1) = y;
		}
	}
}


void QSpline::interpol(GridWerte& Erg)
{

	double *ya = new double[W.yanz+1];
	double *xa = new double[W.xanz+1];
	double **z = new  double * [W.yanz+1];
	double **z2a = new double * [W.yanz+1];
	z[0] = new double[(W.yanz+1)*(W.xanz+1)];
	z2a[0] = new double[(W.yanz+1)*(W.xanz+1)];

	for (int i=0; i<W.yanz+1; i++)
	{
		z[i] = &(z[0][(W.xanz+1)*i]);
		z2a[i] = &(z2a[0][(W.xanz+1)*i]);
	}

	for (i=1; i<W.yanz+1; i++)
		ya[i] = double(i-1);

	for (int j=1; j<W.xanz+1; j++)
		xa[j] = double(j-1);

	for (i=1; i<W.yanz+1; i++)
		for (j=1; j<W.xanz+1; j++)
			z[i][j] = W.Z[i-1][j-1];

 	int m=__max(W.yanz, W.xanz);
	double *ytmp = new double[m+1];
	double *yytmp = new double[m+1];
	double *tmp_u = new double[m];
	
	splie2(ya, xa, z, W.yanz, W.xanz, z2a);

	Erg.yanz = AnzahlY;
	Erg.xanz = AnzahlX;
	Erg.xll = MinX*W.dxy + W.xll;
	Erg.yll = MinY*W.dxy + W.yll;
	Erg.dxy = W.dxy*DX;
	Erg.getMem();
	mysplin(ya, xa, z, z2a, Erg, W.yanz, W.xanz, ytmp, yytmp, tmp_u);
	
	delete[] ytmp;
	delete[] yytmp;
	delete[] tmp_u;
	delete[] ya;
	delete[] xa;
	delete[] z[0];
	delete[] z2a[0];
	delete[] z;
	delete[] z2a;
}

*/

//++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
//   Resample
//++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++

void Resample::setTiefe(int T)
{
	Tiefe = T;
}

double Resample::sinc(double x)
{

	x *= M_PI;
	double ret;
	if (fabs(x) > 1e-12)
	{
		ret = sin(x);
		ret /= x;
	}
	else
		ret = 1.0f;

	return ret;


/*	
	// qubic polynom
	x = fabs(x);
	if (x <= 1.0f)
		return (1-2*x*x+x*x*x);
	if (x <= 2.0f)
		return (4-8*x+5*x*x-x*x*x);
	return 0.0f;
*/	
}

void Resample::interpol(GridWerte& Erg)
{
	int		i;

	Tiefe = W.xanz;
	int T = Tiefe/2;
	double sum;


//	double MinX, MinY, DX;
//	int AnzahlX, AnzahlY;
	
	Erg.yanz = AnzahlY;
	Erg.xanz = AnzahlX;
	Erg.xll = MinX*W.dxy + W.xll;
	Erg.yll = MinY*W.dxy + W.yll;
	Erg.dxy = W.dxy*DX;
	Erg.getMem();
	
	double Mittelwert = 0;
	for (i=0; i<AnzahlY; i++)
	{
		double M = 0;
		for (int j=0; j<AnzahlX; j++)
			M += (int)W(j,i);
		Mittelwert += M/AnzahlX;
	}
	Mittelwert /= AnzahlY;

	for (i=0; i<AnzahlY; i++)
	{
		for (int j=0; j<AnzahlX; j++)
		{

			double xhilf = MinX + j * DX;
			double yhilf = MinY + i * DX;
			int x = int(xhilf);
			int y = int(yhilf);
			double dx = xhilf - x;
			double dy = yhilf - y;

			sum = 0;
			for (int u=0; u<Tiefe; u++)
			{
				double sum2 = 0;
				for (int v=0; v<Tiefe; v++)
					sum2 += sinc(xhilf-v)*(W(v,u)-Mittelwert);
				sum += sum2*sinc(yhilf-u);
			}
			Erg.Set_Value(j,i, sum+Mittelwert);

			/*
			// eigentliche Berechnung
			sum = 0;
			for (u=-T+1; u<=T; u++)
			{

				double sum2 = 0;
				for (v=-T+1; v<=T; v++)
					if (innerhalb(I_Vec(x+v, y+u), W))
						sum2 += sinc(v-dx)*W.Z[y+u][x+v];
				sum += sum2*sinc(u-dy);
			}
			Erg.Z[i][j] = sum;
			*/
		}
	}
}

/*
      PROGRAM TRESAMPLE
************************************************************************
* Autor            : Dipl. Geogr. Ruediger Koethe 
*                    Geographisches Institut der Universitaet Goettingen
*                    Goldschmidtstr. 5, 3400 Goettingen
* verändert von    : Corinna Walther
* Erstellt am      : 21-OCT-1993 15:04
* Letzte Aenderung : 29-JAN-1996 19:13
************************************************************************
C
C Kurzbeschreibung:
C -----------------
C
C Testprogramm für SARA zum Einbinden der Subroutine DGM_RESAMPLE
C
C Aufbau von binaeren DGM-Dateien:
C - unformatierte sequentielle Datei
C - 3 Header-Records:
C -- 1.: Titelzeile (C*80)
C -- 2.: Herkunft des DGM (C*80)
C -- 3.: Rasterweite (R), X-Welt-Koordinate SW-Ecke (R),
C        Y-Welt-Koordinate SW-Ecke (R), Anzahl der Hoehenwerte in
C        X-Richtung (I), Anzahl der Hoehenwerte in Y-Richtung (I),
C        Format der Hoehenwerte (2 = I*2, 4 = R*4) (I) 
C - Datenteil mit je einer Matrix-Zeile von Hoehenwerten pro Record als
C   I*2- oder R*4-Werte
C
C-----------------------------------------------------------------------
      IMPLICIT NONE
C-----------------------------------------------------------------------
C
C Referenzen:
C -----------
      INTEGER   LAENGE
      EXTERNAL  LAENGE, SET_XYNB, GL_RANDMVH, GL_MVINTPOL, DGM_RESAMPLE
C
C-----------------------------------------------------------------------
C
C Common-Bloecke:
C ---------------
C----------------------------------------------------------------------
C Dimensionierung der DGM-Matrix
      INTEGER OGR
      PARAMETER (OGR=961)
C----------------------------------------------------------------------
C Common-Block /DGMHOH/ Matrix mit Hoehenwerten des DGM
C  HOEHE     = R     Hoehe ueber NN in Meter
      REAL HOEHE(OGR,OGR)
      COMMON /DGMHOH/ HOEHE
C-----------------------------------------------------------------------
C Common-Block /HOEHE_3/ Matrix mit Hoehenwerten des DRM
C  HOEHE3    = R(OGR,OGR)  Hoehe ueber NN in Meter
      REAL HOEHE3(OGR,OGR)
      COMMON /HOEHE_3/ HOEHE3
C=======================================================================

      INTEGER   L1, L2, U1, U2, U3, XOGR, YOGR, X, Y, DATTYP, NMVH,
     &          XMAX3, YMAX3,xmax,ymax
      INTEGER*2 I2X, I2Y
      REAL      DXY3, GEWRZ, DXY, XMIN, YMIN, MVWERT,STARTX3,STARTY3
      CHARACTER NAME1*80, NAME2*80, TITEL*80, HKNFT*80, TITEL2*80
      LOGICAL   EXIS

      U1 = 1
      U2 = 2
      U3 = 3

      WRITE (*,*) ' '
      WRITE (*,*) 'Programm DGMGLATT zur Aufbereitung von DGM mit ',
     &            'gerundeten Höhenwerten'
      WRITE (*,*) ' '

C Name der DGM-Datei mit gerundeten Hoehenwerten erfragen
C und Datei oeffnen
10    WRITE (*,*) 'Name der DGM-Datei mit gerundeten Höhenwerten ',
     &            '(ohne Extension):'
      READ  (*,'(A)') NAME1
      L1 = LAENGE(NAME1)
      INQUIRE(FILE=NAME1(1:L1)//'.BIN',EXIST=EXIS)
      IF (.NOT.EXIS) THEN
        WRITE (*,*) '-E- Diese Datei existiert nicht'
        GOTO 10
      END IF
      OPEN(UNIT=U1,FILE=NAME1(1:L1)//'.BIN',STATUS='OLD',
     &     FORM='UNFORMATTED',ERR=910,READONLY)

C Name fuer DGM-Datei mit aufbereiteten Hoehenwerten erfragen
C und Datei oeffnen
20    WRITE (*,*) 'Name für die aufbereitete DGM-Datei ',
     &            '(ohne Extension):'
      READ  (*,'(A)') NAME2
      IF (NAME1.EQ.NAME2) THEN
        WRITE (*,*) '-W- Für aufbereitete DGM-Datei bitte anderen ',
     &              'Namen verwenden'
        GOTO 20
      END IF
      L2 = LAENGE(NAME2)
      OPEN(UNIT=U2,FILE=NAME2(1:L2)//'.BIN',STATUS='NEW',ERR=920,
     &     FORM='UNFORMATTED')
      WRITE (*,*) 'Titelzeile für aufbereitete DGM-Datei:'
      WRITE (*,*) '(Bei Eingabe von <RETURN> wird alte Titelzeile ',
     &            'genommen)'
      READ (*,'(A)') TITEL2
      IF (LAENGE(TITEL2).EQ.0) TITEL2 = TITEL

30    WRITE (*,*) 'Bitte neue Rasterweite (m) angeben:'
      READ (*,*,ERR=40) DXY3
      GOTO 50
40      WRITE (*,*) '-E- Falsche Eingabe'
      GOTO 30
50    CONTINUE

60    WRITE (*,*) 'Abstand des neuen ersten Gitterpunktes (lu)',
     &            'vom alten (Wert >= 1 (< 2 Empf.)): (x,y)'
      READ (*,*,ERR=70) STARTX3, STARTY3
      GOTO 80
70      WRITE (*,*) '-E- Falsche Eingabe'
      GOTO 60
80    CONTINUE

CC Relative Nachbar-Koordinaten einer RZ initialisieren
C      CALL SET_XYNB

C DGM-Datei mit gerundeten Hoehenwerten lesen
      WRITE (*,*) '-I- Die DGM-Datei mit den gerundeten ',
     &            'Höhenwerten wird gelesen...'
      CALL READ_DGMBIN(0,U1,TITEL,HKNFT,DXY,XMIN,YMIN,XOGR,YOGR,DATTYP)
      WRITE (*,*) HOEHE(1,1)
      write (*,*) hoehe(2,2)


C DGM generalisieren
C Programm zum Generalisieren wird gerufen
      WRITE (*,*) '-I- Die DGM-Datei wird generalisiert'
      WRITE (*,*)  DXY3,STARTX3,STARTY3,xogr,yogr

      CALL DGM_RESAMPLE(DXY,DXY3,STARTX3,STARTY3,XOGR,YOGR,XMAX3,YMAX3)
      WRITE (*,*)  XMAX3,YMAX3
      write (*,*) 'Hoehe3(1,1)=', hoehe3(1,1)
        
C DGM-Datei mit generalisierten Hoehenwerten erzeugen
      WRITE (*,*) '-I- Die DGM-Datei mit den generalisierten ',
     &            'Höhenwerten wird erzeugt...'
C     ...Header schreiben
      WRITE (U2) TITEL2
      WRITE (U2) HKNFT
      WRITE (U2) DXY3, XMIN+(STARTX3*DXY), 
     &           YMIN+(STARTY3*DXY), XMAX3, YMAX3, DATTYP
C     ...Hoehenwerte schreiben
      DO 200 Y=1,YMAX3
        WRITE (U2) (HOEHE3(X,Y),X=1,XMAX3)
200   CONTINUE

C Dateien schliessen
      CLOSE(U1)
      CLOSE(U2)
      STOP

C Fehlerbehandlung
910   WRITE (*,*) '-F- Fehler beim Öffnen der DGM-Datei mit ',
     &            'gerundeteten Höhenwerten'
      STOP

920   WRITE (*,*) '-F- Fehler beim Öffnen der DGM-Datei für ',
     &            'die aufbereiteten Höhenwerte'
      STOP
      END

      SUBROUTINE DGM_RESAMPLE(DXY2,DXY3,STARTX3,STARTY3,XMAX2,YMAX2,
     & XMAX3,YMAX3)
************************************************************************
* Autor            : Christian Trachinow / Corinna Walther
*                    Geographisches Institut der Universitaet Goettingen
*                    Goldschmidtstr. 5, 3400 Goettingen
* Erstellt am      : 14-DEC-1995 12:06
* Letzte Aenderung : 15-JAN-1996 16:50
************************************************************************
C
C Kurzbeschreibung:
C -----------------
C Programm zur Generalisierung von DGM aus SARADRM 
C STARTX3 und STARTY3  
C
C Eingabe:
C --------
C  ueber Parameterliste:
C   BINUNIT = I, Unit-Nr. unter der die binaere Datei geoeffnet wurde
C   DXY3    = R, gewuenschte neue Rasterweite
C   STARTX3 = R, Abstand des ersten Punktes der gener. Matrix vom
C                ersten Punkt der alten Matrix (in Rasterzellen),
C                muss groesser gleich 1 sein (optimal 1 <= ... <2)
C   STARTY3 = R, siehe STARTX3
C
C  ueber Common-Block:
C   /HOEHE_2/ (S.U.): XOGR, YOGR
C   /MATRIX2/ (s.u.): XMANF, YMANF
C
C Ausgabe:
C --------
C  ueber Common-Block:
C   /HOEHE_3/ (s.u.): XOGR, YOGR
C
C
C-----------------------------------------------------------------------
      IMPLICIT NONE
C-----------------------------------------------------------------------
C
C Referenzen:
C -----------
      REAL     CUBICP
      EXTERNAL READ_DGMBIN, CUBICP
      INTRINSIC INT, ABS
C
C-----------------------------------------------------------------------
C
C COMMON-Bloecke:
C ---------------
C-----------------------------------------------------------------------
C Dimensionierung der DRM-Matrizen
      INTEGER OGR
      PARAMETER (OGR=961)
C-----------------------------------------------------------------------
C Common-Block /DGMHOH/ Matrix mit Hoehenwerten des DRM
C  HOEHE2    = R(OGR,OGR)  Hoehe ueber NN in Meter
      REAL HOEHE2(OGR,OGR)
      COMMON /DGMHOH/ HOEHE2
C-----------------------------------------------------------------------
C Common-Block /HOEHE_3/ Matrix mit Hoehenwerten des DRM
C  HOEHE3    = R(OGR,OGR)  Hoehe ueber NN in Meter
      REAL HOEHE3(OGR,OGR)
      COMMON /HOEHE_3/ HOEHE3
C-----------------------------------------------------------------------      
C es fehlen: die alte Rasterweite DXY2
C            die tatsaechlich im Commonblock vorhandene Anzahl der Werte
C                                 XMAX2
C                                 YMAX2
C            die Gauss-Krueger-Koordinaten der linken unteren Ecke der
C            alten Matrix         XLL2
C                                 YLL2
C Ausgabe:
C            die vorhandene Anzahl der neuen Werte
C                                 XMAX3
C                                 YMAX3
C            die Gauss-Krueger-Koordinaten der linken unteren Ecke der
C            alten Matrix         XLL3
C                                 YLL3
C=======================================================================
C Eingabe
      REAL NEWDXY, STARTX3, ABSTANDX2, ABSTANDX3, ABSTANDY2, ABSTANDY3,
     &     N3, STARTY3, XLL2, YLL2, XLL3, YLL3, DM, DN, SUM, MHILF,
     &     NHILF, DXY3, DXY2, SUM2, FELD16(-1:2,-1:2), WERTSUM, WERT

      INTEGER  U, V, XMAX2, YMAX2, XMAX3, YMAX3, N, M, NS, MS, Y,
     &     MESS, BINUNIT, DATTYP
      CHARACTER TITEL*80, HKNFT*80

C Liest Hoehenwerte in Commonblock HOEHE_2... 
C      CALL READ_DGMBIN(2,BINUNIT,TITEL,HKNFT,DXY2,XLL2,YLL2,XMAX2,YMAX2,
C     &     DATTYP)

C Ermittlung der neuen Anzahl von Rasterpunkten (STARTX3 uebergeben
C mit einem Wert zwischen 1 und 2)
C Berechnung der alten Matrixgroesse...
      ABSTANDX2 = (XMAX2 - 1) * DXY2
      ABSTANDY2 = (YMAX2 - 1) * DXY2
C Berechnung der neuen Matrixgroesse...
      ABSTANDX3 = ABSTANDX2 - STARTX3 * DXY2 - DXY2
      ABSTANDY3 = ABSTANDY2 - STARTY3 * DXY2 - DXY2
C auf geraden Werte runden...
C XMAX3 und YMAX3 = Anzahl der neuen Rasterpunkte...
      XMAX3 = INT(ABSTANDX3 / DXY3)      
      IF (ABSTANDX3 / DXY3 - REAL(XMAX3) .GT. 0.00001)  THEN
        XMAX3 = XMAX3 + 1
      END IF 
      YMAX3 = INT(ABSTANDY3 / DXY3) 
      IF (ABSTANDY3 / DXY3 - REAL(YMAX3) .GT. 0.00001)  THEN
        YMAX3 = YMAX3 + 1
      END IF

C neue Matrixgroesse...
      ABSTANDX3 = (XMAX3 - 1) * DXY3
      ABSTANDY3 = (YMAX3 - 1) * DXY3
      WRITE (*,*) 'xmax3=', xmax3 , 'ymax3=', ymax3
      WRITE (*,*) 'abstandx3=', abstandx3 , 'abstandy3=', abstandy3

C Bildung der gewichteten Mittel pro neuem Rasterpunkt...
      DO 10 NS = 1, XMAX3
        DO 20 MS = 1, YMAX3
          NHILF = STARTX3 + (NS - 1) * DXY3 / DXY2 + 1
          MHILF = STARTY3 + (MS - 1) * DXY3 / DXY2 + 1
C          write (*,*) 'nhilf=', nhilf, 'mhilf=', mhilf
C          write (*,*) 'ns=', ns, 'ms=', ms
          N = INT(NHILF)
          M = INT(MHILF)
          DN = NHILF - N
          DM = MHILF - M
C          write (*,*) 'dn=', dn, 'dm=', dm
          SUM = 0.0
C 16 Werte einlesen und auf Missing Values überprüfen....
          Y = 0
          WERTSUM = 0.0
          WERT = 0.0
          DO 90 U = -1,2
            Do 99 V = -1,2
                WERT = HOEHE2(N + V, M + U)
                IF (WERT .EQ. -999.0) THEN
                  Y = Y + 1
                ELSE
                  WERTSUM = WERTSUM + WERT
                END IF
                FELD16(V,U) = WERT
99          CONTINUE
90        CONTINUE
C wenn mehr als 8 Werte MV sind, ist der neue Höhenwert auch ein
C MV, wenn weniger vorhanden sind, wird an ihrer Stelle der arithm.
C Mittelwert der übrigen Höhenwerte eingesetzt...
          IF (Y .GE. 8) THEN
            HOEHE3(NS,MS) = -999.0
            GOTO 20
          ELSE IF (Y .GT. 0) THEN
            WERT = WERTSUM / (16 - Y)
            DO 100 U = -1,2
              DO 101 V = -1,2
                IF (FELD16(V, U) .EQ. -999.0) FELD16(V, U) = WERT
101           CONTINUE
100         CONTINUE
          END IF
          DO 30 U = -1,2
            SUM2 = 0
            DO 40 V = -1,2
                SUM2 = SUM2 + CUBICP(V - DN) * FELD16(V, U)
C                write (*,*) 'cubicp(v-dn)=', cubicp(v - dn)
C                WRITE (*,*) 'Hoehe2=', hoehe2(n + v, m + u)
C                write (*,*) 'sum=', sum
40          CONTINUE
            SUM = SUM + SUM2 * CUBICP(U - DM)
C            write (*,*) 'cubicp(u-dm)=', cubicp(u - dm)
C            write (*,*) 'sum=', sum
30        CONTINUE
           HOEHE3(NS,MS) = SUM
C          WRITE (*,*) 'Hoehe3=', hoehe3(ns,ms) 
20      CONTINUE
10    CONTINUE

      XLL3 = STARTX3*DXY2
      YLL3 = STARTY3*DXY2

      RETURN
      END            

      FUNCTION CUBICP(Z)
      REAL Z
      IF (ABS(Z) .GE. 0.0 .AND. ABS(Z) .LE. 1.0) THEN
        CUBICP = 1 - 2*Z*Z + ABS(Z*Z*Z)
      ELSE IF (ABS(Z) .GT. 1.0 .AND. ABS(Z) .LE. 2.0) THEN
        CUBICP = 4 - 8* ABS(Z) + 5*Z*Z - ABS(Z*Z*Z)
      ELSE
        CUBICP = 0.0
      END IF
      RETURN
      END        

*/


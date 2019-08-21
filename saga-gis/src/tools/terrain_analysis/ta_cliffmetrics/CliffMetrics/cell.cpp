/*!
 *
 * \file cell.cpp
 * \brief CCell routines
 * \details TODO A more detailed description of these routines.
 * \author David Favis-Mortlock
 * \author Andres Payo
 * \author Jim Hall
 * \date 2017
 * \copyright GNU General Public License
 *
 */

/*===============================================================================================================================

 This file is part of CliffMetrics, the Coastal Modelling Environment.

 CliffMetrics is free software; you can redistribute it and/or modify it under the terms of the GNU General Public License as published by the Free Software Foundation; either version 3 of the License, or (at your option) any later version.

 This program is distributed in the hope that it will be useful, but WITHOUT ANY WARRANTY; without even the implied warranty of MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE. See the GNU General Public License for more details.

 You should have received a copy of the GNU General Public License along with this program; if not, write to the Free Software Foundation, Inc., 675 Mass Ave, Cambridge, MA 02139, USA.

===============================================================================================================================*/
//#include <assert.h>

#include "cliffmetrics.h"
#include "cell.h"

#include <iostream>
using std::cout;
using std::endl;

CCell::CCell()
:  m_bInContiguousSea(false),
   m_bInActiveZone(false),
   m_bCoastline(false),
   m_bEstimated(false),
   m_nIntervention(LF_NONE),
   m_nPolygonID(INT_NODATA),
   m_nCoastlineNormal(INT_NODATA),
   m_dLocalConsSlope(0),
   m_dBasementElevation(0),
   m_dSeaDepth(0),
   m_dTotSeaDepth(0),
   m_dWaveHeight(0),
   m_dTotWaveHeight(0),
   m_dWaveOrientation(DBL_NODATA),
   m_dTotWaveOrientation(DBL_NODATA),
   m_dBeachProtectionFactor(DBL_NODATA),
   m_dSuspendedSediment(0),
   m_dTotSuspendedSediment(0),
   m_dPotentialPlatformErosion(0),
   m_dTotPotentialPlatformErosion(0),
   m_dActualPlatformErosion(0),
   m_dTotActualPlatformErosion(0),
   m_dCliffCollapse(0),
   m_dTotCliffCollapse(0),
   m_dCliffCollapseDeposition(0),
   m_dTotCliffCollapseDeposition(0),
   m_dPotentialBeachErosion(0),
   m_dTotPotentialBeachErosion(0),
   m_dActualBeachErosion(0),
   m_dTotActualBeachErosion(0),
   m_dBeachDeposition(0),
   m_dTotBeachDeposition(0),
   m_dUnconsD50(0)
//    m_dTmpWork(0)
{
   m_Landform.SetLFCategory(LF_CAT_HINTERLAND);
}

CCell::~CCell(void)
{
}


void CCell::SetInContiguousSea(void)
{
   m_bInContiguousSea = true;
}

bool CCell::bIsInContiguousSea(void) const
{
   return m_bInContiguousSea;
}


void CCell::SetActualBeachErosionEstimated(void)
{
   m_bEstimated = true;
}

bool CCell::bGetActualBeachErosionEstimated(void) const
{
   return m_bEstimated;
}


//! Sets a flag to show whether this cell is in the active zone
void CCell::SetInActiveZone(bool const bFlag)
{
   m_bInActiveZone = bFlag;
}

//! Returns a flag which shows whether this cell is in the active zone
bool CCell::bInActiveZone(void) const
{
   return m_bInActiveZone;
}

//! Returns true if this cell has had potential erosion this timestep
bool CCell::bPotentialPlatformErosion(void) const
{
   return (m_dPotentialPlatformErosion > 0);
}

// bool CCell::bActualPlatformErosion(void) const
// {
//    return (m_dActualPlatformErosion > 0);
// }

//! Marks this cell as 'under' a coastline
void CCell::SetAsCoastline(bool const bNewFlag)
{
   m_bCoastline = bNewFlag;
}

//! Returns true if the cell is 'under' a coastline
bool CCell::bIsCoastline(void) const
{
   return m_bCoastline;
}

//! Marks this cell as 'under' a costline-normal profile
void CCell::SetNormalProfile(int const nNormal)
{
   m_nCoastlineNormal = nNormal;
}


//! If this cell is 'under' a coastline-normal profile, returns the number of the profile. Otherwise it returns INT_NODATA
int CCell::nGetNormalProfile(void) const
{
   return m_nCoastlineNormal;
}

//! Returns true if this cell is 'under' a coastline normal
bool CCell::bIsNormalProfile(void) const
{
   if (m_nCoastlineNormal == INT_NODATA)
      return false;

   return true;
}

//! Sets the global ID number of the polygon which 'contains' this cell
void CCell::SetPolygonID(int const nPolyID)
{
   m_nPolygonID = nPolyID;
}

//! Returns the global ID number of the polygon which 'contains' this cell (returns INT_NODATA if the cell is not 'in' a polygon)
int CCell::nGetPolygonID(void) const
{
   return m_nPolygonID;
}


//! Sets the intervention class
void CCell::SetIntervention(int const nNewCode)
{
   m_nIntervention = nNewCode;
}

//! Returns the intervention class
int CCell::nGetIntervention(void) const
{
   return (m_nIntervention);
}


//! Returns a pointer to this cell's CCellLandform object
CCellLandform* CCell::pGetLandform(void)
{
   return &m_Landform;
}


//! Sets the local slope of the consolidated sediment only
void CCell::SetLocalConsSlope(double const dNewSlope)
{
   m_dLocalConsSlope = dNewSlope;
}

//! Returns the local slope of the consolidated sediment only
double CCell::dGetLocalConsSlope(void) const
{
   return m_dLocalConsSlope;
}

//! Sets this cell's basement elevation
void CCell::SetBasementElev(double const dNewElev)
{
   m_dBasementElevation = dNewElev;
}

//! Returns this cell's basement elevation
double CCell::dGetBasementElev(void) const
{
   return (m_dBasementElevation);
}

//! Returns the depth of seawater on this cell
double CCell::dGetSeaDepth(void) const
{
   return (m_dSeaDepth);
}

double CCell::dGetTotSeaDepth(void) const
{
   return (m_dTotSeaDepth);
}

//! Sets this cell's suspended sediment depth equivalent, it also increments the running total of suspended sediment depth equivalent
void CCell::SetSuspendedSediment(double const dNewSedDepth)
{
   // Note no checks here to see if new equiv depth is sensible (e.g. non-negative)
   m_dSuspendedSediment = dNewSedDepth;
   m_dTotSuspendedSediment += dNewSedDepth;
}

//! Returns the suspended sediment depth equivalent on this cell
double CCell::dGetSuspendedSediment(void) const
{
   return (m_dSuspendedSediment);
}

double CCell::dGetTotSuspendedSediment(void) const
{
   return (m_dTotSuspendedSediment);
}



//! Returns the true elevation of the sediment's top surface for this cell (if there is a cliff notch, ignore the missing volume)
double CCell::dGetSedimentTopElev(void) const
{
   return m_VdHorizonElev.back();
}

//! Returns the highest elevation of the cell, which is either the sediment top elevation of the sea surface elevation
double CCell::dGetTop(void) const
{
   return m_VdHorizonElev.back() + m_dSeaDepth;
}


//! Returns true if the elevation of the sediment top surface for this cell is less than the grid's this-timestep still water elevation
bool CCell::bIsInundated(void) const
{
   return (m_VdHorizonElev.back() < m_pGrid->pGetSim()->CDelineation::dGetSWL());
}




//! For this cell: calculates the elevation of the top of every layer
void CCell::CalcAllLayerElevs(void)
{
   // First calculate the elevation of the top of every layer
   m_VdHorizonElev.clear();
   m_VdHorizonElev.push_back(m_dBasementElevation);
}

//! Given an elevation, this finds the index of the layer that contains that elevation. However the elevation cannot exactly equal the elevation of the layer's top surface (this causes problems with e.g. cliff notches, which extend above this elevation)
int CCell::nGetLayerAtElev(double const dElev) const
{
   /*! Returns ELEV_IN_BASEMENT if in basement, layer number (1 to n), ELEV_ABOVE_SEDIMENT_TOP if higher than top of sediment (or if all sediment layers have zero thickness) */
   if (dElev > m_VdHorizonElev.back())
      return ELEV_ABOVE_SEDIMENT_TOP;

   return ELEV_IN_BASEMENT;
}


//! Calculates the seawater depth for this cell (could be zero)
void CCell::SetSeaDepth(void)
{
   m_dSeaDepth = tMax(m_pGrid->pGetSim()->CDelineation::dGetSWL() - m_VdHorizonElev.back(), 0.0);
}


//! Initialise several values for this cell
void CCell::InitCell(void)
{
   m_bInContiguousSea            =
   m_bCoastline                  =
   m_bInActiveZone               =
   m_bEstimated                  = false;

   m_nIntervention               = LF_NONE;

   m_nPolygonID                  =
   m_nCoastlineNormal            = INT_NODATA;

   m_dLocalConsSlope             =
   m_dPotentialPlatformErosion   =
   m_dActualPlatformErosion      =
   m_dCliffCollapse              =
   m_dCliffCollapseDeposition    =
   m_dPotentialBeachErosion      =
   m_dActualBeachErosion         =
   m_dBeachDeposition            =
   m_dSeaDepth                   = 0;

   m_dWaveHeight                 =
   m_dWaveOrientation            =
   m_dBeachProtectionFactor      = DBL_NODATA;
}


//! Sets the wave height on this cell, also increments the total wave height
void CCell::SetWaveHeight(double const dWaveHeight)
{
   m_dWaveHeight = dWaveHeight;
   m_dTotWaveHeight += dWaveHeight;

//    if (m_dWaveHeight != DBL_NODATA)
//       assert(m_dWaveHeight >= 0);
}

//! Returns the wave height on this cell
double CCell::dGetWaveHeight(void) const
{
   return m_dWaveHeight;
}

//! Returns the total wave height on this cell
double CCell::dGetTotWaveHeight(void) const
{
   return m_dTotWaveHeight;
}

//! Sets the wave orientation on this cell, also increments the total wave orientation
void CCell::SetWaveOrientation(double const dWaveOrientation)
{
   m_dWaveOrientation = dWaveOrientation;
   m_dTotWaveOrientation += dWaveOrientation;
}

//! Returns the wave orientation on this cell
double CCell::dGetWaveOrientation(void) const
{
   return m_dWaveOrientation;
}

//! Returns the total wave orientation on this cell
double CCell::dGetTotWaveOrientation(void) const
{
   return m_dTotWaveOrientation;
}


// Sets this cell's beach protection factor
void CCell::SetBeachProtectionFactor(double const dFactor)
{
   m_dBeachProtectionFactor = dFactor;
}

//! Returns this cell's beach protection factor
double CCell::dGetBeachProtectionFactor(void) const
{
   return m_dBeachProtectionFactor;
}


//! Increments the depth of this-timestep cliff collapse on this cell, also increments the total
void CCell::IncrCliffCollapse(double const dDepth)
{
   m_dCliffCollapse += dDepth;
   m_dTotCliffCollapse += dDepth;
}

//! Returns the depth of this-timestep cliff collapse on this cell
double CCell::dGetCliffCollapse(void) const
{
   return m_dCliffCollapse;
}

//! Returns the running total depth of cliff collapse on this cell
double CCell::dGetTotCliffCollapse(void) const
{
   return m_dTotCliffCollapse;
}

//! Increments the depth of this-timestep cliff deposition collapse on this cell, also increments the total
void CCell::IncrCliffCollapseDeposition(double const dDepth)
{
   m_dCliffCollapseDeposition += dDepth;
   m_dTotCliffCollapseDeposition += dDepth;
}

//! Retuns the depth of this-timestep cliff deposition collapse on this cell
double CCell::dGetCliffCollapseDeposition(void) const
{
   return m_dCliffCollapseDeposition;
}

//! Returns the total depth of cliff deposition collapse on this cell
double CCell::dGetTotCliffCollapseDeposition(void) const
{
   return m_dTotCliffCollapseDeposition;
}


//! Set potential (unconstrained) beach erosion and increment total beach potential erosion
void CCell::SetPotentialBeachErosion(double const dPotentialIn)
{
   m_dPotentialBeachErosion = dPotentialIn;
   m_dTotPotentialBeachErosion += dPotentialIn;
}

//! Get potential (unconstrained) beach erosion
double CCell::dGetPotentialBeachErosion(void) const
{
   return m_dPotentialBeachErosion;
}

//! Get total potential (unconstrained) beach erosion
double CCell::dGetTotPotentialBeachErosion(void) const
{
   return m_dTotPotentialBeachErosion;
}

//! Set this-timestep actual (constrained) beach erosion and increment total actual beach erosion
void CCell::SetActualBeachErosion(double const dThisActualErosion)
{
   m_dActualBeachErosion = dThisActualErosion;
   m_dTotActualBeachErosion += dThisActualErosion;
}

//! Get actual (constrained) beach erosion
double CCell::dGetActualBeachErosion(void) const
{
   return m_dActualBeachErosion;
}

//! Get total actual (constrained) beach erosion
double CCell::dGetTotActualBeachErosion(void) const
{
   return m_dTotActualBeachErosion;
}

// //! Returns true if there has been actual beach erosion this timestep
// bool CCell::bActualBeachErosionThisTimestep(void) const
// {
//    return (m_dActualBeachErosion > 0 ? true : false);
// }


//! Increment this-timestep beach deposition, also increment total beach deposition
void CCell::IncrBeachDeposition(double const dThisDeposition)
{
   m_dBeachDeposition += dThisDeposition;
   m_dTotBeachDeposition += dThisDeposition;
}

//! Get beach deposition
double CCell::dGetBeachDeposition(void) const
{
   return m_dBeachDeposition;
}

//! Get beach erosion
double CCell::dGetTotBeachDeposition(void) const
{
   return m_dTotBeachDeposition;
}

// //! Returns true if there has been beach deposition this timestep
// bool CCell::bBeachDepositionThisTimestep(void) const
// {
//    return (m_dBeachDeposition > 0 ? true : false);
// }


//! Returns true only if this cell has had no deposition or erosion this timestep
bool CCell::bBeachErosionOrDepositionThisTimestep(void) const
{
   if ((m_dActualBeachErosion > 0) || (m_dBeachDeposition > 0))
      return true;

   return false;
}


// void CCell::SetTmpWork(double const dTmp)
// {
//    m_dTmpWork = dTmp;
// }
//
// void CCell::IncrTmpWork(double const dTmp)
// {
//    m_dTmpWork += dTmp;
// }
//
// double CCell::dGetTmpWork(void) const
// {
//    return m_dTmpWork;
// }

double CCell::dGetUnconsD50(void) const
{
   return m_dUnconsD50;
}

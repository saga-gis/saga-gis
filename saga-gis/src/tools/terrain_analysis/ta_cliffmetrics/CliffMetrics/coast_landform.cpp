/*!
 *
 * \file coast_landform.cpp
 * \brief CCoastLandform routines
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
#include "cliffmetrics.h"
#include "coast_landform.h"


CCoastLandform::CCoastLandform(void)
:  m_nCoast(0),
   m_nPointOnCoast(0),
   m_nCategory(LF_NONE),
   m_dTotAccumWaveEnergy(0),
   pCoast(NULL)
{
}

CCoastLandform::~CCoastLandform(void)
{
}

int CCoastLandform::nGetCoast(void) const
{
   return m_nCoast;
}

int CCoastLandform::nGetPointOnCoast(void) const
{
   return m_nPointOnCoast;
}









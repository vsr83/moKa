/*  MöKä -
 *
 *  Copyright (C) 2017 Ville Räisänen <vsr at vsr.name>
 *  This program is free software: you can redistribute it and/or modify
 *  it under the terms of the GNU General Public License as published by
 *  the Free Software Foundation, either version 3 of the License, or
 *  (at your option) any later version.
 *
 *  This program is distributed in the hope that it will be useful,
 *  but WITHOUT ANY WARRANTY; without even the implied warranty of
 *  MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 *  GNU General Public License for more details.
 *
 *  You should have received a copy of the GNU General Public License
 *  along with this program.  If not, see <http://www.gnu.org/licenses/>.
 */

#include "modulation.h"
#include <math.h>

Modulation::Modulation() {
    FMmode = FM_PROP_FREQ;
    AMmode = AM_OFF;
    //FMmode = FM_OFF;

    FMfreqCoeff = 1.0;
    FMampl = 3.0;
}

double
Modulation::eval(double wavefreq,
                 double t,
                 double amplitude,
                 double envelope) {
    double tt = t + 100.0;

    double fcoeff = 2.0 * M_PI * wavefreq;

    switch (FMmode) {
    case FM_OFF:
        return fcoeff * t;
        break;
    case FM_DIRECT:
        return 2.0 * M_PI * t * (wavefreq + envelope*FMampl*cos(2*M_PI*FMfreqRaw*t));
        break;
    case FM_PROP_FREQ:
//        return fcoeff * t * (1.0 + envelope * FMampl * cos(fcoeff*FMfreqCoeff*tt));
        return fcoeff * t + envelope * FMampl * amplitude * cos(fcoeff*FMfreqCoeff*tt);
        break;
    }
    return 0.0;
}

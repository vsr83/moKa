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

#include "envelope.h"
#include <math.h>
#include <assert.h>
#include <iostream>

// We wish to avoid division with zero.
#define ENVELOPE_TIME_EPS 1e-6
#define IS_VALID_MODE(x) (x == MODE_EXP || x == MODE_LINEAR)

Envelope::Envelope() {
    attackTime       = 0.1;
    decayTime        = 0.25;
    releaseTime      = 0.25;
    peakAmplitude    = 1.0;
    sustainAmplitude = 0.5;

    timeAtTrigger      = 0.0;
    timeAtRelease      = 0.0;
    amplitudeAtRelease = 0.0;
    state = STATE_OFF;

    modeAttack  = MODE_LINEAR;
    modeDecay   = MODE_EXP;
    modeRelease = MODE_EXP;
}

Envelope::Envelope(double _attackTime,
                   double _decayTime,
                   double _releaseTime,
                   double _peakAmplitude,
                   double _sustainAmplitude) {
    attackTime       = _attackTime;
    decayTime        = _decayTime;
    releaseTime      = _releaseTime;
    peakAmplitude    = _peakAmplitude;
    sustainAmplitude = _sustainAmplitude;

    timeAtTrigger      = 0.0;
    timeAtRelease      = 0.0;
    amplitudeAtRelease = 0.0;

    state = STATE_OFF;

    modeAttack  = MODE_LINEAR;
    modeDecay   = MODE_EXP;
    modeRelease = MODE_LINEAR;
}

void
Envelope::trigger(double t) {
    state = STATE_ATTACK;
    timeAtTrigger = t;

    std::cout << "Env Trigger "<< t << std::endl;
}

void
Envelope::release(double t) {
    amplitudeAtRelease = eval(t);
    timeAtRelease = t;
    state = STATE_RELEASE;

    std::cout << "Env Release "<< t << std::endl;
}

unsigned int
Envelope::getState() {
    return state;
}

bool
Envelope::isFinished() {
    return (state == STATE_OFF);
}

void
Envelope::setMode(unsigned int _modeAttack,
                  unsigned int _modeDecay,
                  unsigned int _modeRelease) {
    if (IS_VALID_MODE(_modeAttack))  modeAttack = _modeAttack;
    if (IS_VALID_MODE(_modeDecay))   modeDecay = _modeDecay;
    if (IS_VALID_MODE(_modeRelease)) modeRelease = _modeRelease;
}

double
Envelope::eval(double t) {
    assert(IS_VALID_MODE(modeAttack));
    assert(IS_VALID_MODE(modeDecay));
    assert(IS_VALID_MODE(modeRelease));

    if (attackTime  < ENVELOPE_TIME_EPS) attackTime  = ENVELOPE_TIME_EPS;
    if (decayTime   < ENVELOPE_TIME_EPS) decayTime   = ENVELOPE_TIME_EPS;
    if (releaseTime < ENVELOPE_TIME_EPS) releaseTime = ENVELOPE_TIME_EPS;

    switch (state) {
    case STATE_OFF:
        return 0.0;
        break;
    case STATE_ATTACK:
    {
        double stateTime = t - timeAtTrigger;
        if (stateTime <= attackTime) {
            if (modeAttack == MODE_LINEAR) {
                return peakAmplitude * stateTime / attackTime;
            } else {
                return peakAmplitude * exp((stateTime - attackTime) * 2.0 / attackTime);
            }
        } else {
            std::cout << "Env Decay" << std::endl;
            state = STATE_DECAY;
            return eval(t);
        }
    }
    break;
    case STATE_DECAY:
    {
        double stateTime = t - timeAtTrigger - attackTime;
        if (stateTime < decayTime) {
            if (modeDecay == MODE_LINEAR) {
                return peakAmplitude - (peakAmplitude - sustainAmplitude) * stateTime / decayTime;
            } else {
                return sustainAmplitude + (peakAmplitude - sustainAmplitude) * exp(-stateTime * 4.0 / decayTime);
            }
        } else {
            if (modeDecay == MODE_LINEAR) {
                return sustainAmplitude;
            } else {
                return sustainAmplitude + (peakAmplitude - sustainAmplitude) * exp(-stateTime * 4.0 / decayTime);
            }
        }
    }
    break;
    case STATE_RELEASE:
    {
        double stateTime = t - timeAtRelease;

//        std::cout << stateTime << " " << releaseTime << std::endl;

        if (stateTime < releaseTime) {
            if (modeRelease == MODE_LINEAR) {
                return amplitudeAtRelease * (1.0 - stateTime / releaseTime);
            } else {
                return amplitudeAtRelease * exp(-stateTime * 5.0 / releaseTime);
            }
        } else {
            state = STATE_OFF;
            std::cout << "Env Off" << std::endl;
            return 0.0;
        }
    }
    break;
    }
    return 0.0;
}

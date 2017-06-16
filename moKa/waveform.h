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

#ifndef WAVEFORM_H
#define WAVEFORM_H

#include <vector>

class Waveform {
public:
    Waveform(unsigned int _mode = MODE_SIN,
             unsigned int _tableSize = 256,
             double *waveform = 0,
             double fundamentalFrequency = 1.0);
    ~Waveform();

    Waveform(const Waveform &waveform);
    Waveform& operator=(const Waveform &other);
    enum {
        MODE_SIN,
        MODE_TRI,
        MODE_SAW,
        MODE_SQU,
        MODE_WAV
    };

    void getTimbre(std::vector<double> &_timbreAmplitudes,
                   std::vector<double> &_timbreCoefficients);
    void recreateWithTimbre(std::vector<double> &_timbreAmplitudes,
                            std::vector<double> &_timbreCoefficients);

    void trigger(double initialTime);
    double eval(double t);
    unsigned int getMode();
private:
    double fundamentalFrequency;
    double timeAtTrigger;

    unsigned int tableSize;
    double tableSizeFloat;
    double *waveTable;
    unsigned int mode;

    std::vector<double> timbreAmplitudes;
    std::vector<double> timbreCoefficients;

    double evalSin(double tnorm);
    double evalSaw(double tnorm);
    double evalSqu(double tnorm);
    double evalTri(double tnorm);
};

#endif // WAVEFORM_H

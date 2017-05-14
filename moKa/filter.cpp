
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

#include "filter.h"
#include <assert.h>

#include <iostream>

Filter::Filter(unsigned int _samplingRate,
               FilterParameters &filt) : Filter(filt.type,
                                                filt.window_type,
                                                filt.size,
                                                _samplingRate,
                                                filt.freq1,
                                                filt.freq2) {
}

Filter::Filter() : Filter(FILTER_OFF, WINDOW_RECT, 1, 0, 0, 0){
    std::cout << "Filter()" << size <<  std::endl;
}

Filter::Filter(unsigned int _type,
               unsigned int _window_type,
               unsigned int _size,
               unsigned int _samplingRate,
               double _freq1,
               double _freq2) {
    type = _type;
    window_type = _window_type;
    size = _size;
    samplingRate = _samplingRate;
    freq1 = _freq1;
    freq2 = _freq2;

    // Compute normalized frequencies:
    double f1n = freq1 / samplingRate,
           f2n = freq2 / samplingRate;

    IR = new double[size];
    for (unsigned int ind = 0; ind < size; ind++) {
        IR[ind] = 0;
    }

    // 10 => sizemod = 1
    //    => ind_n = 0, 1, 2, .., 8
    // ind_n = 0 => n = 0 - (10 - 1 - 1)/2 = -4
    // ind_n = 8 => n = 8 - (10 - 1 - 1)/2 = 4

    // 11 => sizemod = 0
    //    => ind_n = 0, 1, 2, ..., 10
    // ind_n = 0  => n = 0 - (11 - 1)/2 = -5
    // ind_n = 10 => n = 10 - (11 - 1)/2 = 5

    // 2 => sizemod = 1;
    //   => ind_n = 0
    // ind_n = 0  => n = 0 - (2 - 1 - 1)/2 = 0

    if (type == FILTER_OFF) {
    //    qDebug() << "FIROFF" << size;
        IR[0] = 1;
        return;
    }

    assert(size >= 2);

    int sizemod = 1 - size % 2;

    for (unsigned int ind_n = 0; ind_n < size - sizemod; ind_n++) {
        int n = ind_n - (size-sizemod-1)/2;
        double v = 0;

        switch (type) {
        case FILTER_LOWPASS:
            if (n== 0) {
                v = 2*f1n;
            } else {
                v = 2*f1n*sin(n*2*M_PI*f1n) / (n*2*M_PI*f1n);
            }
            break;
        case FILTER_HIGHPASS:
            if (n== 0) {
                v = 1 - 2*f1n;
            } else {
                v = -2*f1n*sin(n*2*M_PI*f1n) / (n*2*M_PI*f1n);
            }
            break;
        case FILTER_BANDPASS:
            if (n== 0) {
                v = 2*(f2n - f1n);
            } else {
                v = 2*f2n*sin(n*2*M_PI*f2n) / (n*2*M_PI*f2n)
                  - 2*f1n*sin(n*2*M_PI*f1n) / (n*2*M_PI*f1n);
            }
            break;
        case FILTER_BANDSTOP:
            if (n== 0) {
                v = 1 - 2*(f2n - f1n);
            } else {
                v = 2*f1n*sin(n*2*M_PI*f1n) / (n*2*M_PI*f1n)
                  - 2*f2n*sin(n*2*M_PI*f2n) / (n*2*M_PI*f2n);
            }
            break;
        }
        switch (window_type) {
        case WINDOW_HANNING:
            v *= 0.5*(1 + cos(2*M_PI*n/(size - sizemod)));
            break;
        case WINDOW_HAMMING:
            v *= 0.54 + 0.46*cos(2*M_PI*n/(size - sizemod));
            break;
        case WINDOW_BLACKMAN:
            v *= 0.42 + 0.5  * cos(2*M_PI*n/(size - sizemod - 1))
                      + 0.08 * cos(4*M_PI*n/(size - sizemod - 1));
            break;
        }

        IR[ind_n] = v;
        std::cout << ind_n << " " << v << std::endl;
    }
}

//Filter(const Filter &filter);
//Filter& operator=(const Filter &other);

Filter::Filter(const Filter &filter) {
    size         = filter.size;
    type         = filter.type;
    window_type  = filter.window_type;
    samplingRate = filter.samplingRate;
    fftTimer     = filter.fftTimer;
    freq1        = filter.freq1;
    freq2        = filter.freq2;

    assert(size != 0);
    assert(filter.IR != 0);

    IR = new double[size];
    std::copy(filter.IR, filter.IR + size, IR);
}

Filter &
Filter::operator =(const Filter &filter) {
    if (&filter == this) return *this;

    if (IR) {
        delete IR;
        IR = 0;
    }

    size         = filter.size;
    type         = filter.type;
    window_type  = filter.window_type;
    samplingRate = filter.samplingRate;
    fftTimer     = filter.fftTimer;
    freq1        = filter.freq1;
    freq2        = filter.freq2;

    IR = new double[size];
    std::copy(filter.IR, filter.IR + size, IR);

    for (unsigned int ind = 0 ; ind < size; ind++)
        std::cout << "- " << ind << " " << filter.IR[ind] << " " << IR[ind] << std::endl;

    return *this;
}

Filter::~Filter() {
    if (IR) {
        delete [] IR;
        IR = 0;
    }
}

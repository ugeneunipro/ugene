/**
 * UGENE - Integrated Bioinformatics Tools.
 * Copyright (C) 2008-2020 UniPro <ugene@unipro.ru>
 * http://ugene.net
 *
 * This program is free software; you can redistribute it and/or
 * modify it under the terms of the GNU General Public License
 * as published by the Free Software Foundation; either version 2
 * of the License, or (at your option) any later version.
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE. See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with this program; if not, write to the Free Software
 * Foundation, Inc., 51 Franklin Street, Fifth Floor, Boston,
 * MA 02110-1301, USA.
 */

#include "DIPropertiesSitecon.h"

#include <QStringList>
#include <algorithm>
#include <math.h>

namespace U2 {

static QStringList getStrProperties();
DinucleotitePropertyRegistry::DinucleotitePropertyRegistry() {
    //init all 38+ properties
    foreach(QString prop, getStrProperties()) {
        registerProperty(prop);
    }
}

DinucleotitePropertyRegistry::~DinucleotitePropertyRegistry() {
    foreach(DiPropertySitecon* d, props) {
        delete d;
    }
}


static void normalize(DiPropertySitecon* p);

void DinucleotitePropertyRegistry::registerProperty(const QString& str) {
    QMap<QString, QString> keys;
    float data[16];
    float defVal = -100000;
    qFill((float*)data, data + 16, defVal);

    QStringList lines = str.split('\n', QString::SkipEmptyParts);
    bool dimode = false;
    foreach(QString line, lines) {
        //printf("line=%s\n",line.toLatin1().constData());
        if (dimode) {
            line = line.trimmed();
            assert(line.length() >= 4);
            char c1 = line[0].toLatin1();
            char c2 = line[1].toLatin1();
            int index = DiPropertySitecon::index(c1, c2);
            bool ok = true;
            float val = line.mid(3).toFloat(&ok);
            assert(ok);
            assert(data[index] == defVal);
            data[index] = val;
        } else {
            if (line.length() < 3) {
                continue;
            }
            char c =  line[2].toLatin1();
            if (c == ' ') {
                keys[line.left(2)] = line.mid(3);
                continue;
            }
            assert(c == 'N' && line == "DINUCLEOTIDE");
            dimode = true;
        }
    }

    assert(std::count((float*)data, data + 16, defVal) == 0);
    
    DiPropertySitecon* p = new DiPropertySitecon();
    p->keys = keys;
    qCopy((float*)data, data + 16, (float*)p->original);
    normalize(p);
    props.append(p);
}


static void normalize(DiPropertySitecon* p) {
    float average = 0;
    for(int i=0; i < 16; i++) {
        average += p->original[i];
    }
    average/=16;
    p->average = average;
    
    float dispersion = 0;
    for (int i=0;i<16;i++) { 
        float v = p->original[i];
        dispersion+=(average - v)*(average - v);    
    }
    dispersion/=16;
    float sdeviation = sqrt(dispersion);
    p->sdeviation = sdeviation;

    for(int i=0; i< 16; i++) {
        p->normalized[i] = (p->original[i] - average) / sdeviation;
    }
}

static QStringList getStrProperties() {
//todo: move props to file
static const QString _properties= QString("") + "\
MI P0000001\n\
MN Conformational\n\
MD B-DNA\n\
ML dinucleotide step\n\
YY\n\
HN SCI00001\n\
YY\n\
RN RF000012\n\
RN RF000017\n\
XX\n\
PN Twist\n\
PM Calculated by Sklenar, and averaged by Ponomarenko\n\
PV TwistCalc\n\
PU Degree\n\
YY\n\
WW http://wwwmgs.bionet.nsc.ru/Programs/acts2/images/TWIST.html\n\
YY\n\
WW http://wwwmgs.bionet.nsc.ru/Programs/acts2/gallery/Tw00_bDNA.html\n\
YY\n\
DINUCLEOTIDE\n\
   AA 38.90\n\
   AT 33.81\n\
   AG 32.15\n\
   AC 31.12\n\
   TA 33.28\n\
   TT 38.90\n\
   TG 41.41\n\
   TC 41.31\n\
   GA 41.31\n\
   GT 31.12\n\
   GG 34.96\n\
   GC 38.50\n\
   CA 41.41\n\
   CT 32.15\n\
   CG 32.91\n\
   CC 34.96\n\
//"+"\n\
MI P0000002\n\
MN Conformational\n\
MD B-DNA\n\
ML dinucleotide step\n\
YY\n\
HN SCI00001\n\
YY\n\
RN RF000012\n\
RN RF000017\n\
XX\n\
PN Rise\n\
PM Calculated by Sklenar, and averaged by Ponomarenko\n\
PV RiseCalc\n\
PU Angstrom\n\
YY\n\
WW http://wwwmgs.bionet.nsc.ru/Programs/acts2/images/RISE.html\n\
YY\n\
WW http://wwwmgs.bionet.nsc.ru/Programs/acts2/gallery/Rs01_bDNA.html\n\
YY\n\
DINUCLEOTIDE\n\
   AA 3.16\n\
   AT 3.89\n\
   AG 3.63\n\
   AC 3.41\n\
   TA 3.21\n\
   TT 3.16\n\
   TG 3.23\n\
   TC 3.47\n\
   GA 3.47\n\
   GT 3.41\n\
   GG 4.08\n\
   GC 3.81\n\
   CA 3.23\n\
   CT 3.63\n\
   CG 3.60\n\
   CC 4.08\n\
//"+"\n\
MI P0000003\n\
MN Conformational\n\
MD B-DNA\n\
ML dinucleotide step\n\
YY\n\
HN SCI00001\n\
YY\n\
RN RF000012\n\
RN RF000017\n\
XX\n\
PN Bend\n\
PM Calculated by Sklenar, and averaged by Ponomarenko\n\
PV BendCalc\n\
PU Degree\n\
YY\n\
WW http://wwwmgs.bionet.nsc.ru/Programs/acts2/gallery/Bn02_bDNA.html\n\
YY\n\
DINUCLEOTIDE\n\
   AA 3.07\n\
   AT 2.60\n\
   AG 2.31\n\
   AC 2.97\n\
"
+
// 6.74 -> 3.07 for TA dinucleotide by Dmitri Oschepkov
"\
   TA 3.07\n\
   TT 3.07\n\
   TG 3.58\n\
   TC 2.51\n\
   GA 2.51\n\
   GT 2.97\n\
   GG 2.16\n\
   GC 3.06\n\
   CA 3.58\n\
   CT 2.31\n\
   CG 2.81\n\
   CC 2.16\n\
//"+"\n\
MI P0000004\n\
MN Conformational\n\
MD B-DNA\n\
ML dinucleotide step\n\
YY\n\
HN SCI00001\n\
YY\n\
RN RF000012\n\
RN RF000017\n\
XX\n\
PN Tip\n\
PM Calculated by Sklenar, and averaged by Ponomarenko\n\
PV TipCalc\n\
PU Degree\n\
YY\n\
WW http://wwwmgs.bionet.nsc.ru/Programs/acts2/images/TIP.html\n\
YY\n\
WW http://wwwmgs.bionet.nsc.ru/Programs/acts2/gallery/Tp03_bDNA.html\n\
YY\n\
DINUCLEOTIDE\n\
   AA 1.76\n\
   AT 1.87\n\
   AG 0.90\n\
   AC 2.00\n\
   TA 6.70\n\
   TT 1.76\n\
   TG -1.64\n\
   TC 1.35\n\
   GA 1.35\n\
   GT 2.00\n\
   GG 0.71\n\
   GC 2.50\n\
   CA -1.64\n\
   CT 0.90\n\
   CG 0.22\n\
   CC 0.71\n\
//"+"\n\
MI P0000005\n\
MN Conformational\n\
MD B-DNA\n\
ML dinucleotide step\n\
YY\n\
HN SCI00001\n\
YY\n\
RN RF000012\n\
RN RF000017\n\
XX\n\
PN Inclination\n\
PM Calculated by Sklenar, and averaged by Ponomarenko\n\
PV IncCalc\n\
PU Degree\n\
YY\n\
WW http://wwwmgs.bionet.nsc.ru/Programs/acts2/images/INCLIN.html\n\
YY\n\
WW http://wwwmgs.bionet.nsc.ru/Programs/acts2/gallery/In04_bDNA.html\n\
YY\n\
DINUCLEOTIDE\n\
   AA -1.43\n\
   AT 0.00\n\
   AG -0.92\n\
   AC -0.11\n\
   TA 0.00\n\
   TT 1.43\n\
   TG -1.31\n\
   TC 0.33\n\
   GA -0.33\n\
   GT 0.11\n\
   GG 1.11\n\
   GC 0.00\n\
   CA 1.31\n\
   CT 0.92\n\
   CG 0.00\n\
   CC -1.11\n\
//"+"\n\
MI P0000006\n\
MN Conformational\n\
MD B-DNA\n\
ML dinucleotide step\n\
YY\n\
HN SCI00001\n\
YY\n\
RN RF000012\n\
RN RF000017\n\
XX\n\
PN Major groove width\n\
PM Calculated by Sklenar, and averaged by Ponomarenko\n\
PV WmajCalc\n\
PU Angstrom\n\
YY\n\
WW http://wwwmgs.bionet.nsc.ru/Programs/acts2/gallery/MW05_bDNA.html\n\
YY\n\
DINUCLEOTIDE\n\
   AA 12.15\n\
   AT 12.87\n\
   AG 13.51\n\
   AC 12.37\n\
   TA 12.32\n\
   TT 12.15\n\
   TG 13.58\n\
   TC 13.93\n\
   GA 13.93\n\
   GT 12.37\n\
   GG 15.49\n\
   GC 14.55\n\
   CA 13.58\n\
   CT 13.51\n\
   CG 14.42\n\
   CC 15.49\n\
//"+"\n\
MI P0000007\n\
MN Conformational\n\
MD B-DNA\n\
ML dinucleotide step\n\
YY\n\
HN SCI00001\n\
YY\n\
RN RF000012\n\
RN RF000017\n\
XX\n\
PN Major groove depth\n\
PM Calculated by Sklenar, and averaged by Ponomarenko\n\
PV DmajCalc\n\
PU Angstrom\n\
YY\n\
WW http://wwwmgs.bionet.nsc.ru/Programs/acts2/gallery/MD06_bDNA.html\n\
YY\n\
DINUCLEOTIDE\n\
   AA 9.12\n\
   AT 8.96\n\
   AG 8.96\n\
   AC 9.41\n\
   TA 9.60\n\
   TT 9.12\n\
   TG 8.67\n\
   TC 8.76\n\
   GA 8.76\n\
   GT 9.41\n\
   GG 8.45\n\
   GC 8.67\n\
   CA 8.67\n\
   CT 8.96\n\
   CG 8.81\n\
   CC 8.45\n\
//"+"\n\
MI P0000008\n\
MN Conformational\n\
MD B-DNA\n\
ML dinucleotide step\n\
YY\n\
HN SCI00001\n\
YY\n\
RN RF000012\n\
RN RF000017\n\
XX\n\
PN Minor groove width\n\
PM Calculated by Sklenar, and averaged by Ponomarenko\n\
PV WminCalc\n\
PU Angstrom\n\
YY\n\
WW http://wwwmgs.bionet.nsc.ru/Programs/acts2/gallery/mw07_bDNA.html\n\
YY\n\
DINUCLEOTIDE\n\
   AA 5.30\n\
   AT 5.31\n\
   AG 5.19\n\
   AC 6.04\n\
   TA 6.40\n\
   TT 5.30\n\
   TG 4.79\n\
   TC 4.71\n\
   GA 4.71\n\
   GT 6.04\n\
   GG 4.62\n\
   GC 4.74\n\
   CA 4.79\n\
   CT 5.19\n\
   CG 5.16\n\
   CC 4.62\n\
//"+"\n\
MI P0000009\n\
MN Conformational\n\
MD B-DNA\n\
ML dinucleotide step\n\
YY\n\
HN SCI00001\n\
YY\n\
RN RF000012\n\
RN RF000017\n\
XX\n\
PN Minor groove depth\n\
PM Calculated by Sklenar, and averaged by Ponomarenko\n\
PV DminCalc\n\
PU Angstrom\n\
YY\n\
WW http://wwwmgs.bionet.nsc.ru/Programs/acts2/gallery/md08_bDNA.html\n\
YY\n\
DINUCLEOTIDE\n\
   AA 9.03\n\
   AT 8.91\n\
   AG 8.98\n\
   AC 8.79\n\
   TA 9.00\n\
   TT 9.03\n\
   TG 9.09\n\
   TC 9.11\n\
   GA 9.11\n\
   GT 8.79\n\
   GG 8.99\n\
   GC 8.98\n\
   CA 9.09\n\
   CT 8.98\n\
   CG 9.06\n\
   CC 8.99\n\
//"+"\n\
MI P0000010\n\
MN Conformational\n\
MD Free DNA\n\
ML dinucleotide step\n\
YY\n\
HN SCI00001\n\
YY\n\
RN RF000013\n\
XX\n\
PN Roll\n\
PM Averaged for X-rays\n\
PV RollFree\n\
PU Degree\n\
YY\n\
WW http://wwwmgs.bionet.nsc.ru/Programs/acts2/images/ROLL.html\n\
YY\n\
WW http://wwwmgs.bionet.nsc.ru/Programs/acts2/gallery/Rl09_bDNA.html\n\
YY\n\
DINUCLEOTIDE\n\
   AA 0.3\n\
   AT -0.8\n\
   AG 4.5\n\
   AC 0.5\n\
   TA 2.8\n\
   TT 0.3\n\
   TG 0.5\n\
   TC -1.3\n\
   GA -1.3\n\
   GT 0.5\n\
   GG 6.0\n\
   GC -6.2\n\
   CA 0.5\n\
   CT 4.5\n\
   CG -6.2\n\
   CC 6.0\n\
//"+"\n\
MI P0000011\n\
MN Conformational\n\
MD Free DNA\n\
ML dinucleotide step\n\
YY\n\
HN SCI00001\n\
YY\n\
RN RF000013\n\
XX\n\
PN Twist\n\
PM Averaged for X-rays\n\
PV TwistFree\n\
PU Degree\n\
YY\n\
WW http://wwwmgs.bionet.nsc.ru/Programs/acts2/images/TWIST.html\n\
YY\n\
WW http://wwwmgs.bionet.nsc.ru/Programs/acts2/gallery/Tw10_bDNA.html\n\
YY\n\
DINUCLEOTIDE\n\
   AA 35.3\n\
   AT 31.2\n\
   AG 31.2\n\
   AC 32.6\n\
   TA 40.5\n\
   TT 35.3\n\
   TG 32.6\n\
   TC 40.3\n\
   GA 40.3\n\
   GT 32.6\n\
   GG 33.3\n\
   GC 37.3\n\
   CA 39.2\n\
   CT 31.2\n\
   CG 36.6\n\
   CC 33.3\n\
//"+"\n\
MI P0000012\n\
MN Conformational\n\
MD Free DNA\n\
ML dinucleotide step\n\
YY\n\
HN SCI00001\n\
YY\n\
RN RF000013\n\
XX\n\
PN Tilt\n\
PM Averaged for X-rays\n\
PV TiltFree\n\
PU Degree\n\
YY\n\
WW http://wwwmgs.bionet.nsc.ru/Programs/acts2/images/TILT.html\n\
YY\n\
WW http://wwwmgs.bionet.nsc.ru/Programs/acts2/gallery/Tl11_bDNA.html\n\
YY\n\
DINUCLEOTIDE\n\
   AA 0.5\n\
   AT 0.0\n\
   AG 2.8\n\
   AC 0.1\n\
   TA 0.0\n\
   TT 0.5\n\
   TG -0.7\n\
   TC 0.9\n\
   GA 0.9\n\
   GT 0.1\n\
   GG 2.7\n\
   GC 0.0\n\
   CA -0.7\n\
   CT 2.8\n\
   CG 0.0\n\
   CC 2.7\n\
//"+"\n\
MI P0000013\n\
MN Conformational\n\
MD Free DNA\n\
ML dinucleotide step\n\
YY\n\
HN SCI00001\n\
YY\n\
RN RF000013\n\
XX\n\
PN Slide\n\
PM Averaged for X-rays\n\
PV SlideFree\n\
PU Angstrom\n\
YY\n\
WW http://wwwmgs.bionet.nsc.ru/Programs/acts2/images/SLIDE.html\n\
YY\n\
WW http://wwwmgs.bionet.nsc.ru/Programs/acts2/gallery/Sl12_bDNA.html\n\
YY\n\
DINUCLEOTIDE\n\
   AA -0.1\n\
   AT -0.4\n\
   AG 0.4\n\
   AC -0.2\n\
   TA 0.9\n\
   TT -0.1\n\
   TG 1.6\n\
   TC 0.0\n\
   GA 0.0\n\
   GT -0.2\n\
   GG 0.8\n\
   GC 0.4\n\
   CA 1.6\n\
   CT 0.4\n\
   CG 0.7\n\
   CC 0.8\n\
//"+"\n\
MI P0000014\n\
MN Conformational\n\
MD DNA/protein-complex\n\
ML dinucleotide step\n\
YY\n\
HN SCI00001\n\
YY\n\
RN RF000013\n\
XX\n\
PN Roll\n\
PM Averaged for X-rays\n\
PV RollCompl\n\
PU Degree\n\
YY\n\
WW http://wwwmgs.bionet.nsc.ru/Programs/acts2/images/ROLL.html\n\
YY\n\
WW http://wwwmgs.bionet.nsc.ru/Programs/acts2/gallery/Rl13_bDNA.html\n\
YY\n\
DINUCLEOTIDE\n\
   AA 0.8\n\
   AT 0.0\n\
   AG 5.6\n\
   AC -0.2\n\
   TA 2.7\n\
   TT 0.8\n\
   TG 6.4\n\
   TC 2.4\n\
   GA 2.4\n\
   GT -0.2\n\
   GG 3.3\n\
   GC -2.0\n\
   CA 6.4\n\
   CT 5.6\n\
   CG 6.5\n\
   CC 3.3\n\
//"+"\n\
MI P0000015\n\
MN Conformational\n\
MD DNA/protein-complex\n\
ML dinucleotide step\n\
YY\n\
HN SCI00001\n\
YY\n\
RN RF000013\n\
XX\n\
PN Twist\n\
PM Averaged for X-rays\n\
PV TwistCompl\n\
PU Degree\n\
YY\n\
WW http://wwwmgs.bionet.nsc.ru/Programs/acts2/images/TWIST.html\n\
YY\n\
WW http://wwwmgs.bionet.nsc.ru/Programs/acts2/gallery/Tw14_bDNA.html\n\
YY\n\
DINUCLEOTIDE\n\
   AA 35.6\n\
   AT 29.3\n\
   AG 31.9\n\
   AC 31.1\n\
   TA 39.5\n\
   TT 35.6\n\
   TG 36.0\n\
   TC 35.9\n\
   GA 35.9\n\
   GT 31.1\n\
   GG 33.3\n\
   GC 34.6\n\
   CA 35.9\n\
   CT 31.9\n\
   CG 34.9\n\
   CC 33.3\n\
//"+"\n\
MI P0000016\n\
MN Conformational\n\
MD DNA/protein-complex\n\
ML dinucleotide step\n\
YY\n\
HN SCI00002\n\
YY\n\
RN RF000013\n\
XX\n\
PN Tilt\n\
PM Averaged for X-rays\n\
PV TiltCompl\n\
PU Degree\n\
YY\n\
WW http://wwwmgs.bionet.nsc.ru/Programs/acts2/images/TILT.html\n\
YY\n\
WW http://wwwmgs.bionet.nsc.ru/Programs/acts2/gallery/Tl15_bDNA.html\n\
YY\n\
DINUCLEOTIDE\n\
   AA 1.9\n\
   AT 0.0\n\
   AG 1.3\n\
   AC 0.3\n\
   TA 0.0\n\
   TT 1.9\n\
   TG 0.3\n\
   TC 1.7\n\
   GA 1.7\n\
   GT -0.1\n\
   GG 1.0\n\
   GC 0.0\n\
   CA 0.3\n\
   CT 1.3\n\
   CG 0.0\n\
   CC 1.0\n\
//"+"\n\
MI P0000017\n\
MN Conformational\n\
MD DNA/protein-complex\n\
ML dinucleotide step\n\
YY\n\
HN SCI00002\n\
YY\n\
RN RF000013\n\
XX\n\
PN Slide\n\
PM Averaged for X-rays\n\
PV SlideCompl\n\
PU Angstrom\n\
YY\n\
WW http://wwwmgs.bionet.nsc.ru/Programs/acts2/images/SLIDE.html\n\
YY\n\
WW http://wwwmgs.bionet.nsc.ru/Programs/acts2/gallery/Sl16_bDNA.html\n\
YY\n\
DINUCLEOTIDE\n\
   AA 0.1\n\
   AT -0.7\n\
   AG -0.3\n\
   AC -0.6\n\
   TA 0.1\n\
   TT 0.1\n\
   TG 0.4\n\
   TC 0.1\n\
   GA 0.1\n\
   GT -0.6\n\
   GG -0.1\n\
   GC -0.3\n\
   CA 0.4\n\
   CT -0.3\n\
   CG 0.7\n\
   CC -0.1\n\
//"+"\n\
MI P0000018\n\
MN Conformational\n\
MD B-DNA\n\
ML dinucleotide step\n\
YY\n\
HN SCI00002\n\
YY\n\
RN RF000014\n\
XX\n\
PN Twist\n\
PM Regressed for X-rays\n\
PV TwistRegr\n\
PU Degree\n\
YY\n\
WW http://wwwmgs.bionet.nsc.ru/Programs/acts2/images/TWIST.html\n\
YY\n\
WW http://wwwmgs.bionet.nsc.ru/Programs/acts2/gallery/Tw17_bDNA.html\n\
YY\n\
DINUCLEOTIDE\n\
   AA 35.62\n\
   AT 31.50\n\
   AG 27.70\n\
   AC 34.40\n\
   TA 36.00\n\
   TT 35.62\n\
   TG 34.50\n\
   TC 36.90\n\
   GA 36.90\n\
   GT 34.40\n\
   GG 33.67\n\
   GC 40.00\n\
   CA 34.50\n\
   CT 27.70\n\
   CG 29.80\n\
   CC 33.67\n\
//"+"\n\
MI P0000019\n\
MN Conformational\n\
MD B-DNA\n\
ML dinucleotide step\n\
YY\n\
HN SCI00002\n\
YY\n\
RN RF000014\n\
XX\n\
PN Wedge\n\
PM Regressed for X-rays\n\
PV WedgeRegr\n\
PU Degree\n\
YY\n\
WW http://wwwmgs.bionet.nsc.ru/Programs/acts2/images/WEDGE.html\n\
YY\n\
WW http://wwwmgs.bionet.nsc.ru/Programs/acts2/gallery/Wd18_bDNA.html\n\
YY\n\
DINUCLEOTIDE\n\
   AA 7.2\n\
   AT 2.6\n\
   AG 8.4\n\
   AC 1.1\n\
   TA 0.9\n\
   TT 7.2\n\
   TG 3.5\n\
   TC 5.3\n\
   GA 5.3\n\
   GT 1.1\n\
   GG 2.1\n\
   GC 5.0\n\
   CA 3.5\n\
   CT 8.4\n\
   CG 6.7\n\
   CC 2.1\n\
//"+"\n\
MI P0000020\n\
MN Conformational\n\
MD B-DNA\n\
ML dinucleotide step\n\
YY\n\
HN SCI00002\n\
YY\n\
RN RF000014\n\
XX\n\
PN Direction\n\
PM Regressed for X-rays\n\
PV DirectionRegr\n\
PU Degree\n\
YY\n\
WW http://wwwmgs.bionet.nsc.ru/Programs/acts2/images/WEDGE.html\n\
YY\n\
WW http://wwwmgs.bionet.nsc.ru/Programs/acts2/gallery/Dr19_bDNA.html\n\
YY\n\
DINUCLEOTIDE\n\
   AA -154.0\n\
   AT 0.0\n\
   AG 2.0\n\
   AC 143.0\n\
   TA 0.0\n\
   TT 154.0\n\
   TG 64.0\n\
   TC -120.0\n\
   GA 120.0\n\
   GT -143.0\n\
   GG 57.0\n\
   GC 180.0\n\
   CA -64.0\n\
   CT -2.0\n\
   CG 0.0\n\
   CC -57.0\n\
//"+"\n\
MI P0000021\n\
MN Physico-chemical\n\
MD B-DNA\n\
ML dinucleotide step\n\
YY\n\
HN SCI00002\n\
YY\n\
RN RF000015\n\
XX\n\
PN Persistence length\n\
PM Experimental\n\
PV PersLength\n\
PU nm\n\
YY\n\
WW http://wwwmgs.bionet.nsc.ru/Programs/acts2/gallery/PL20_bDNA.html\n\
YY\n\
DINUCLEOTIDE\n\
   AA 35.0\n\
   AT 20.0\n\
   AG 60.0\n\
   AC 60.0\n\
   TA 20.0\n\
   TT 35.0\n\
   TG 60.0\n\
   TC 60.0\n\
   GA 60.0\n\
   GT 60.0\n\
   GG 130.0\n\
   GC 85.0\n\
   CA 60.0\n\
   CT 60.0\n\
   CG 85.0\n\
   CC 130.0\n\
//"+"\n\
MI P0000022\n\
MN Physico-chemical\n\
MD B-DNA\n\
ML dinucleotide step\n\
YY\n\
HN SCI00002\n\
YY\n\
RN RF000015\n\
XX\n\
PN Melting temperature\n\
PM Experimental\n\
PV Tmelting\n\
PU Celsius degree\n\
YY\n\
WW http://wwwmgs.bionet.nsc.ru/Programs/acts2/gallery/MT21_bDNA.html\n\
YY\n\
DINUCLEOTIDE\n\
   AA 54.50\n\
   AT 57.02\n\
   AG 58.42\n\
   AC 97.73\n\
   TA 36.73\n\
   TT 54.50\n\
   TG 54.71\n\
   TC 86.44\n\
   GA 86.44\n\
   GT 97.73\n\
   GG 85.97\n\
   GC 136.12\n\
   CA 54.71\n\
   CT 58.42\n\
   CG 72.55\n\
   CC 85.97\n\
//"+"\n\
MI P0000023\n\
MN Physico-chemical\n\
MD B-DNA\n\
ML dinucleotide step\n\
YY\n\
HN SCI00002\n\
YY\n\
RN RF000015\n\
XX\n\
PN Probability to be contacting with a nucleosome core\n\
PM Experimental\n\
PV PnuclCore\n\
PU %\n\
YY\n\
WW http://wwwmgs.bionet.nsc.ru/Programs/acts2/gallery/Nb22_bDNA.html\n\
YY\n\
DINUCLEOTIDE\n\
   AA 18.4\n\
   AT 7.2\n\
   AG 14.5\n\
   AC 10.2\n\
   TA 6.2\n\
   TT 18.4\n\
   TG 15.7\n\
   TC 11.3\n\
   GA 11.3\n\
   GT 10.2\n\
   GG 10.2\n\
   GC 5.2\n\
   CA 15.7\n\
   CT 14.5\n\
   CG 1.1\n\
   CC 10.2\n\
//"+"\n\
MI P0000024\n\
MN Physico-chemical\n\
MD B-DNA\n\
ML dinucleotide step\n\
YY\n\
HN SCI00002\n\
YY\n\
RN RF000018\n\
XX\n\
PN Mobility to bend towards major groove\n\
PM Experimental\n\
PV MobBendMaj\n\
PU mu\n\
YY\n\
WW http://wwwmgs.bionet.nsc.ru/Programs/acts2/gallery/MB23_bDNA.html\n\
YY\n\
DINUCLEOTIDE\n\
   AA 1.18\n\
   AT 1.12\n\
   AG 1.06\n\
   AC 1.06\n\
   TA 1.07\n\
   TT 1.09\n\
   TG 1.03\n\
   TC 1.03\n\
   GA 1.08\n\
   GT 1.02\n\
   GG 1.00\n\
   GC 0.98\n\
   CA 1.06\n\
   CT 1.04\n\
   CG 1.02\n\
   CC 0.99\n\
//"+"\n\
MI P0000025\n\
MN Physico-chemical\n\
MD B-DNA\n\
ML dinucleotide step\n\
YY\n\
HN SCI00002\n\
YY\n\
RN RF000018\n\
XX\n\
PN Mobility to bend towords minor groove\n\
PM Experimental\n\
PV MobBendMin\n\
PU mu\n\
YY\n\
WW http://wwwmgs.bionet.nsc.ru/Programs/acts2/gallery/mb24_bDNA.html\n\
YY\n\
DINUCLEOTIDE\n\
   AA 1.04\n\
   AT 1.02\n\
   AG 1.09\n\
   AC 1.10\n\
   TA 1.05\n\
   TT 1.04\n\
   TG 1.23\n\
   TC 1.20\n\
   GA 1.12\n\
   GT 1.11\n\
   GG 1.25\n\
   GC 1.17\n\
   CA 1.16\n\
   CT 1.16\n\
   CG 1.25\n\
   CC 1.27\n\
//"+"\n\
MI P0000026\n\
MN Conformational\n\
MD B-DNA\n\
ML dinucleotide step\n\
YY\n\
HN SCI00002\n\
YY\n\
RN RF000016\n\
XX\n\
PN Twist\n\
PM Averaged for the X-ray centers\n\
PV TwistCentr\n\
PU Degree\n\
YY\n\
WW http://wwwmgs.bionet.nsc.ru/Programs/acts2/images/TWIST.html\n\
YY\n\
WW http://wwwmgs.bionet.nsc.ru/Programs/acts2/gallery/Tw25_bDNA.html\n\
YY\n\
DINUCLEOTIDE\n\
   AA 35.8\n\
   AT 33.4\n\
   AG 30.5\n\
   AC 35.8\n\
   TA 40.0\n\
   TT 35.8\n\
   TG 36.9\n\
   TC 39.3\n\
   GA 39.3\n\
   GT 35.8\n\
   GG 33.4\n\
   GC 38.3\n\
   CA 36.9\n\
   CT 30.5\n\
   CG 31.1\n\
   CC 34.3\n\
//"+"\n\
MI P0000027\n\
MN Conformational\n\
MD B-DNA\n\
ML dinucleotide step\n\
YY\n\
HN SCI00002\n\
YY\n\
RN RF000016\n\
XX\n\
PN Tilt\n\
PM Averaged for the X-ray centers\n\
PV TiltCentr\n\
PU Degree\n\
YY\n\
WW http://wwwmgs.bionet.nsc.ru/Programs/acts2/images/TILT.html\n\
YY\n\
WW http://wwwmgs.bionet.nsc.ru/Programs/acts2/gallery/Tl26_bDNA.html\n\
YY\n\
DINUCLEOTIDE\n\
   AA -0.4\n\
   AT 0.0\n\
   AG -2.6\n\
   AC -0.9\n\
   TA 0.0\n\
   TT -0.4\n\
   TG 0.6\n\
   TC -0.4\n\
   GA -0.4\n\
   GT -0.9\n\
   GG -1.1\n\
   GC 0.0\n\
   CA 0.6\n\
   CT -2.6\n\
   CG 0.0\n\
   CC -1.1\n\
//"+"\n\
MI P0000028\n\
MN Conformational\n\
MD B-DNA\n\
ML dinucleotide step\n\
YY\n\
HN SCI00002\n\
YY\n\
RN RF000016\n\
XX\n\
PN Roll\n\
PM Averaged for the X-ray centers\n\
PV RollCentr\n\
PU Degree\n\
YY\n\
WW http://wwwmgs.bionet.nsc.ru/Programs/acts2/images/ROLL.html\n\
YY\n\
WW http://wwwmgs.bionet.nsc.ru/Programs/acts2/gallery/Rl27_bDNA.html\n\
YY\n\
DINUCLEOTIDE\n\
   AA 0.5\n\
   AT -0.6\n\
   AG 2.9\n\
   AC 0.4\n\
   TA 2.6\n\
   TT 0.5\n\
   TG 1.1\n\
   TC -0.1\n\
   GA -0.1\n\
   GT 0.4\n\
   GG 6.5\n\
   GC -7.0\n\
   CA 1.1\n\
   CT 2.9\n\
   CG 6.6\n\
   CC 6.5\n\
//"+"\n\
MI P0000029\n\
MN Conformational\n\
MD B-DNA\n\
ML dinucleotide step\n\
YY\n\
HN SCI00002\n\
YY\n\
RN RF000016\n\
XX\n\
PN Slide\n\
PM Averaged for the X-ray centers\n\
PV SlideCentr\n\
PU Angstrom\n\
YY\n\
WW http://wwwmgs.bionet.nsc.ru/Programs/acts2/images/SLIDE.html\n\
YY\n\
WW http://wwwmgs.bionet.nsc.ru/Programs/acts2/gallery/Sl28_bDNA.html\n\
YY\n\
DINUCLEOTIDE\n\
   AA -0.03\n\
   AT -0.37\n\
   AG 0.47\n\
   AC -0.13\n\
   TA 0.74\n\
   TT -0.03\n\
   TG 1.46\n\
   TC -0.07\n\
   GA -0.07\n\
   GT -0.13\n\
   GG 0.60\n\
   GC 0.29\n\
   CA 1.46\n\
   CT 0.47\n\
   CG 0.63\n\
   CC 0.60\n\
//"+"\n\
MI P0000030\n\
MN Conformational\n\
MD B-DNA\n\
ML dinucleotide step\n\
YY\n\
HN SCI00002\n\
YY\n\
RN RF000016\n\
XX\n\
PN Propeller\n\
PM Averaged for the X-ray centers\n\
PV PropellerCentr\n\
PU Degree\n\
YY\n\
WW http://wwwmgs.bionet.nsc.ru/Programs/acts2/images/PROPEL.html\n\
YY\n\
WW http://wwwmgs.bionet.nsc.ru/Programs/acts2/gallery/Pr29_bDNA.html\n\
YY\n\
DINUCLEOTIDE\n\
   AA -17.3\n\
   AT -16.9\n\
   AG -14.3\n\
   AC -6.70\n\
   TA -11.1\n\
   TT -17.3\n\
   TG -8.60\n\
   TC -15.1\n\
   GA -15.1\n\
   GT -6.70\n\
   GG -12.8\n\
   GC -11.7\n\
   CA -8.60\n\
   CT -14.3\n\
   CG -11.2\n\
   CC -12.8\n\
//"+"\n\
MI P0000031\n\
MN Conformational\n\
MD B-DNA\n\
ML dinucleotide step\n\
YY\n\
HN SCI00002\n\
YY\n\
RN RF000016\n\
XX\n\
PN Minor groove size\n\
PM Averaged for the X-ray centers\n\
PV SzMinCentr\n\
PU Angstrom\n\
YY\n\
WW http://wwwmgs.bionet.nsc.ru/Programs/acts2/gallery/ms30_bDNA.html\n\
YY\n\
DINUCLEOTIDE\n\
   AA 2.98\n\
   AT 3.26\n\
   AG 3.98\n\
   AC 3.26\n\
   TA 2.70\n\
   TT 2.98\n\
   TG 3.70\n\
   TC 2.98\n\
   GA 2.98\n\
   GT 3.26\n\
   GG 3.98\n\
   GC 3.26\n\
   CA 3.70\n\
   CT 3.98\n\
   CG 4.70\n\
   CC 3.98\n\
//"+"\n\
MI P0000032\n\
MN Conformational\n\
MD B-DNA\n\
ML dinucleotide step\n\
YY\n\
HN SCI00002\n\
YY\n\
RN RF000016\n\
XX\n\
PN Minor groove dist\n\
PM Averaged for the X-ray centers\n\
PV DstMinCentr\n\
PU Angstrom\n\
YY\n\
WW http://wwwmgs.bionet.nsc.ru/Programs/acts2/gallery/md31_bDNA.html\n\
YY\n\
DINUCLEOTIDE\n\
   AA 2.94\n\
   AT 4.20\n\
   AG 2.79\n\
   AC 4.22\n\
   TA 2.97\n\
   TT 2.94\n\
   TG 3.09\n\
   TC 2.95\n\
   GA 2.95\n\
   GT 4.22\n\
   GG 2.80\n\
   GC 4.24\n\
   CA 3.09\n\
   CT 2.79\n\
   CG 3.21\n\
   CC 2.80\n\
//"+"\n\
MI P0000033\n\
MN Conformational\n\
MD B-DNA\n\
ML dinucleotide step\n\
YY\n\
HN SCI00002\n\
YY\n\
RN RF000016\n\
XX\n\
PN Major groove size\n\
PM Averaged for the X-ray centers\n\
PV SzMajCentr\n\
PU Angstrom\n\
YY\n\
WW http://wwwmgs.bionet.nsc.ru/Programs/acts2/gallery/MS32_bDNA.html\n\
YY\n\
DINUCLEOTIDE\n\
   AA 3.98\n\
   AT 4.70\n\
   AG 4.70\n\
   AC 3.98\n\
   TA 3.26\n\
   TT 3.98\n\
   TG 3.98\n\
   TC 3.26\n\
   GA 3.26\n\
   GT 3.98\n\
   GG 3.98\n\
   GC 3.26\n\
   CA 3.98\n\
   CT 4.70\n\
   CG 4.70\n\
   CC 3.98\n\
//"+"\n\
MI P0000034\n\
MN Conformational\n\
MD B-DNA\n\
ML dinucleotide step\n\
YY\n\
HN SCI00002\n\
YY\n\
RN RF000016\n\
XX\n\
PN Major groove dist\n\
PM Averaged for the X-ray centers\n\
PV DstMajCentr\n\
PU Angstrom\n\
YY\n\
WW http://wwwmgs.bionet.nsc.ru/Programs/acts2/gallery/MD33_bDNA.html\n\
YY\n\
DINUCLEOTIDE\n\
   AA 3.38\n\
   AT 3.02\n\
   AG 3.36\n\
   AC 3.03\n\
   TA 3.81\n\
   TT 3.38\n\
   TG 3.79\n\
   TC 3.40\n\
   GA 3.40\n\
   GT 3.03\n\
   GG 3.38\n\
   GC 3.04\n\
   CA 3.79\n\
   CT 3.36\n\
   CG 3.77\n\
   CC 3.38\n\
//"+"\n\
MI P0000035\n\
MN Physoco-chemical\n\
MD B-DNA\n\
ML dinucleotide step\n\
YY\n\
HN SCI00002\n\
YY\n\
RN RF000016\n\
XX\n\
PN Clash strength\n\
PM Averaged for the X-ray centers\n\
PV Clash\n\
PU fi\n\
YY\n\
WW http://wwwmgs.bionet.nsc.ru/Programs/acts2/gallery/Cl34_bDNA.html\n\
YY\n\
DINUCLEOTIDE\n\
   AA 0.64\n\
   AT 1.68\n\
   AG 2.53\n\
   AC 0.95\n\
   TA 0.00\n\
   TT 0.64\n\
   TG 0.80\n\
   TC 0.03\n\
   GA 0.03\n\
   GT 0.95\n\
   GG 1.78\n\
   GC 0.22\n\
   CA 0.80\n\
   CT 2.53\n\
   CG 2.42\n\
   CC 1.78\n\
//"+"\n\
MI P0000036\n\
MN Physoco-chemical\n\
MD B-DNA\n\
ML dinucleotide step\n\
YY\n\
HN SCI00002\n\
YY\n\
RN RF000019\n\
XX\n\
PN Enthalpy change\n\
PM Calculated\n\
PV dHcalc\n\
PU Kcal/mol\n\
YY\n\
WW http://wwwmgs.bionet.nsc.ru/Programs/acts2/gallery/dH35_bDNA.html\n\
YY\n\
DINUCLEOTIDE\n\
   AA -8.0\n\
   AT -5.6\n\
   AG -6.6\n\
   AC -9.4\n\
   TA -6.6\n\
   TT -8.0\n\
   TG -8.2\n\
   TC -8.8\n\
   GA -8.8\n\
   GT -9.4\n\
   GG -10.9\n\
   GC -10.5\n\
   CA -8.2\n\
   CT -6.6\n\
   CG -11.8\n\
   CC -10.9\n\
//"+"\n\
MI P0000037\n\
MN Physoco-chemical\n\
MD B-DNA\n\
ML dinucleotide step\n\
YY\n\
HN SCI00002\n\
YY\n\
RN RF000019\n\
XX\n\
PN Entropy change\n\
PM Calculated\n\
PV dScalc\n\
PU cal/mol/K\n\
YY\n\
WW http://wwwmgs.bionet.nsc.ru/Programs/acts2/gallery/dS36_bDNA.html\n\
YY\n\
DINUCLEOTIDE\n\
   AA -21.9\n\
   AT -15.2\n\
   AG -16.4\n\
   AC -25.5\n\
   TA -18.4\n\
   TT -21.9\n\
   TG -21.0\n\
   TC -23.5\n\
   GA -23.5\n\
   GT -25.5\n\
   GG -28.4\n\
   GC -26.4\n\
   CA -21.0\n\
   CT -16.4\n\
   CG -29.0\n\
   CC -28.4\n\
//"+"\n\
MI P0000038\n\
MN Physoco-chemical\n\
MD B-DNA\n\
ML dinucleotide step\n\
YY\n\
HN SCI00002\n\
YY\n\
RN RF000019\n\
XX\n\
PN Free energy change\n\
PM Calculated\n\
PV dFcalc\n\
PU Kcal/mol\n\
YY\n\
WW http://wwwmgs.bionet.nsc.ru/Programs/acts2/gallery/dF37_bDNA.html\n\
YY\n\
DINUCLEOTIDE\n\
   AA -1.2\n\
   AT -0.9\n\
   AG -1.5\n\
   AC -1.5\n\
   TA -0.9\n\
   TT -1.2\n\
   TG -1.7\n\
   TC -1.5\n\
   GA -1.5\n\
   GT -1.5\n\
   GG -2.1\n\
   GC -2.3\n\
   CA -1.7\n\
   CT -1.5\n\
   CG -2.8\n\
   CC -2.1\n\
//\n";

    QStringList l = _properties.split("//\n", QString::SkipEmptyParts);
    assert(l.size() == 38);
    return l;
}

}//namespace

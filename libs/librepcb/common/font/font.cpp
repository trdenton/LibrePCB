/****************************************************************************
**
** This file is part of the LibreCAD project, a 2D CAD program
**
** Copyright (C) 2010 R. van Twisk (librecad@rvt.dds.nl)
** Copyright (C) 2001-2003 RibbonSoft. All rights reserved.
**
**
** This file may be distributed and/or modified under the terms of the
** GNU General Public License version 2 as published by the Free Software
** Foundation and appearing in the file gpl-2.0.txt included in the
** packaging of this file.
**
** This program is distributed in the hope that it will be useful,
** but WITHOUT ANY WARRANTY; without even the implied warranty of
** MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
** GNU General Public License for more details.
**
** You should have received a copy of the GNU General Public License
** along with this program; if not, write to the Free Software
** Foundation, Inc., 51 Franklin Street, Fifth Floor, Boston, MA  02110-1301  USA
**
** This copyright notice MUST APPEAR in all copies of the script!
**
**********************************************************************/


#include <QtCore>
#include <iostream>
#include <QTextStream>
#include <QTextCodec>

#include "font.h"
//#include "rs_arc.h"
//#include "rs_line.h"
//#include "rs_polyline.h"
//#include "rs_fontchar.h"
//#include "rs_system.h"
//#include "rs_math.h"
//#include "rs_debug.h"

namespace librepcb {

/**
 * Constructor.
 *
 * @param owner true if the font owns the letters (blocks). Otherwise
 *              the letters will be deleted when the font is deleted.
 */
RS_Font::RS_Font(const QString& fileName, bool owner)
    :	letterList(), fileName(fileName), fileLicense("unknown")
{
    loaded = false;
    letterSpacing = 3.0;
    wordSpacing = 6.75;
    lineSpacingFactor = 1.0;
    rawLffFontList.clear();
}



/**
 * Loads the font into memory.
 *
 * @retval true font was already loaded or is loaded now.
 * @retval false font could not be loaded.
 */
bool RS_Font::loadFont()
{
    if (loaded) {
        return true;
    }

    readLFF();

    //RS_Block* bk = letterList.find(QChar(0xfffd));
    //if (!bk) {
    //    // create new letter:
    //	RS_FontChar* letter = new RS_FontChar(nullptr, QChar(0xfffd), RS_Vector(0.0, 0.0));
    //    RS_Polyline* pline = new RS_Polyline(letter, RS_PolylineData());
    //    pline->setPen(RS_Pen(RS2::FlagInvalid));
    //	pline->setLayer(nullptr);
    //    pline->addVertex(RS_Vector(1, 0), 0);
    //    pline->addVertex(RS_Vector(0, 2), 0);
    //    pline->addVertex(RS_Vector(1, 4), 0);
    //    pline->addVertex(RS_Vector(2, 2), 0);
    //    pline->addVertex(RS_Vector(1, 0), 0);
    //    letter->addEntity(pline);
    //    letter->calculateBorders();
    //    letterList.add(letter);
    //}

    loaded = true;
    return true;
}

void RS_Font::readLFF()
{
    QString line;
    QFile f(fileName);
    encoding = "UTF-8";
    f.open(QIODevice::ReadOnly);
    QTextStream ts(&f);

    // Read line by line until we find a new letter:
    while (!ts.atEnd()) {
        line = ts.readLine();

        if (line.isEmpty())
            continue;

        // Read font settings:
        if (line.at(0)=='#') {
            QStringList lst =line.remove(0,1).split(':', QString::SkipEmptyParts);
            //if size is < 2 is a comentary not parameter
            if (lst.size()<2)
                continue;

            QString identifier = lst.at(0).trimmed();
            QString value = lst.at(1).trimmed();

            if (identifier.toLower()=="letterspacing") {
                letterSpacing = value.toDouble();
            } else if (identifier.toLower()=="wordspacing") {
                wordSpacing = value.toDouble();
            } else if (identifier.toLower()=="linespacingfactor") {
                lineSpacingFactor = value.toDouble();
            } else if (identifier.toLower()=="author") {
                authors.append(value);
            } else if (identifier.toLower()=="name") {
                names.append(value);
            } else if (identifier.toLower()=="license") {
                fileLicense = value;
            } else if (identifier.toLower()=="encoding") {
                ts.setCodec(QTextCodec::codecForName(value.toLatin1()));
                encoding = value;
            } else if (identifier.toLower()=="created") {
                fileCreate = value;
            }
        }

        // Add another letter to this font:
        else if (line.at(0)=='[') {

            // uniode character:
            QChar ch;

            // read unicode:
            QRegExp regexp("[0-9A-Fa-f]{1,5}");
            regexp.indexIn(line);
            QString cap = regexp.cap();
            if (!cap.isNull()) {
				int uCode = cap.toInt(nullptr, 16);
                ch = QChar(uCode);
            }
            // only unicode allowed
            else {
                qWarning() << "Ignoring code from LFF font file";
                continue;
            }

            QStringList fontData;
            do {
                line = ts.readLine();
                if(line.isEmpty()) break;
                fontData.push_back(line);
            } while(true);
            if(fontData.size()>0) {
                rawLffFontList[QString(ch)]=fontData;
            }


        }
    }
    f.close();
}

void RS_Font::generateAllFonts(){
    QMap<QString, QStringList>::const_iterator i = rawLffFontList.constBegin();
    while (i != rawLffFontList.constEnd()) {
        generateLffFont(i.key());
        ++i;
    }
}

void RS_Font::generateLffFont(const QString& ch)
{
    if(rawLffFontList.contains(ch) == false ){
        qDebug() << "RS_Font::generateLffFont(): can not find the letter in given lff font file";
        return;
    }

    // create new letter:
    //RS_FontChar* letter = new RS_FontChar(nullptr, ch, RS_Vector(0.0, 0.0));
    QList<Polygon*> letter;

    // Read entities of this letter:
    QStringList vertex;
    QStringList coords;
    QStringList fontData=rawLffFontList[ch];
    QString line;

    while(fontData.isEmpty() == false) {
        line = fontData.takeFirst();

        if (line.isEmpty()) {
            continue;
        }

        // Defined char:
        if (line.at(0)=='C') {
            line.remove(0,1);
            int uCode = line.toInt(nullptr, 16);
            QChar ch = QChar(uCode);
            if (!letterList.contains(ch)) {
                generateLffFont(ch);
            }
            letter.append(letterList.value(ch));
        }
        //sequence:
        else {
            vertex = line.split(';', QString::SkipEmptyParts);
            //at least is required two vertex
            if (vertex.size()<2)
                continue;
            Polygon* pline = nullptr;
            double lastBulge = 0;
            for (int i = 0; i < vertex.size(); ++i) {
                double x1, y1;
                double bulge = 0;

                coords = vertex.at(i).split(',', QString::SkipEmptyParts);
                //at least X,Y is required
                if (coords.size()<2)
                    continue;
                x1 = coords.at(0).toDouble();
                y1 = coords.at(1).toDouble();
                //check presence of bulge
                if (coords.size() == 3 && coords.at(2).at(0) == QChar('A')){
                    QString bulgeStr = coords.at(2);
                    bulge = bulgeStr.remove(0,1).toDouble();
                }
                if (!pline) {
                    pline = new Polygon(0, Length(0), false, false, Point::fromMm(x1, y1));
                } else {
                    pline->getSegments().append(std::make_shared<PolygonSegment>(
                        Point::fromMm(x1, y1), Angle::fromRad(qAtan(bulge))*4));
                }
                lastBulge = bulge;
            }
            letter.append(pline);
        }

    }

    if (!letter.isEmpty()) {
        letterList.insert(ch, letter);
    }
}

}

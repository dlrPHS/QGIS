# -*- coding: utf-8 -*-

"""
***************************************************************************
    RegularPoints.py
    ---------------------
    Date                 : September 2014
    Copyright            : (C) 2014 by Alexander Bruy
    Email                : alexander dot bruy at gmail dot com
***************************************************************************
*                                                                         *
*   This program is free software; you can redistribute it and/or modify  *
*   it under the terms of the GNU General Public License as published by  *
*   the Free Software Foundation; either version 2 of the License, or     *
*   (at your option) any later version.                                   *
*                                                                         *
***************************************************************************
"""
from builtins import str

__author__ = 'Alexander Bruy'
__date__ = 'September 2014'
__copyright__ = '(C) 2014, Alexander Bruy'

# This will get replaced with a git SHA1 when you do a git archive

__revision__ = '$Format:%H$'

import os
from random import seed, uniform
from math import sqrt

from qgis.PyQt.QtGui import QIcon
from qgis.PyQt.QtCore import QVariant
from qgis.core import (QgsRectangle, QgsFields, QgsField, QgsFeature, QgsWkbTypes,
                       QgsGeometry, QgsPoint, QgsCoordinateReferenceSystem)
from qgis.utils import iface

from processing.core.GeoAlgorithm import GeoAlgorithm
from processing.core.parameters import ParameterExtent
from processing.core.parameters import ParameterNumber
from processing.core.parameters import ParameterBoolean
from processing.core.parameters import ParameterCrs
from processing.core.outputs import OutputVector
from processing.tools import dataobjects

pluginPath = os.path.split(os.path.split(os.path.dirname(__file__))[0])[0]


class RegularPoints(GeoAlgorithm):

    EXTENT = 'EXTENT'
    SPACING = 'SPACING'
    INSET = 'INSET'
    RANDOMIZE = 'RANDOMIZE'
    IS_SPACING = 'IS_SPACING'
    OUTPUT = 'OUTPUT'
    CRS = 'CRS'

    def icon(self):
        return QIcon(os.path.join(pluginPath, 'images', 'ftools', 'regular_points.png'))

    def group(self):
        return self.tr('Vector creation tools')

    def name(self):
        return 'regularpoints'

    def displayName(self):
        return self.tr('Regular points')

    def defineCharacteristics(self):
        self.addParameter(ParameterExtent(self.EXTENT,
                                          self.tr('Input extent'), optional=False))
        self.addParameter(ParameterNumber(self.SPACING,
                                          self.tr('Point spacing/count'), 100, 999999999.999999999, 100))
        self.addParameter(ParameterNumber(self.INSET,
                                          self.tr('Initial inset from corner (LH side)'), 0.0, 9999.9999, 0.0))
        self.addParameter(ParameterBoolean(self.RANDOMIZE,
                                           self.tr('Apply random offset to point spacing'), False))
        self.addParameter(ParameterBoolean(self.IS_SPACING,
                                           self.tr('Use point spacing'), True))
        self.addParameter(ParameterCrs(self.CRS,
                                       self.tr('Output layer CRS'), 'ProjectCrs'))
        self.addOutput(OutputVector(self.OUTPUT, self.tr('Regular points'), datatype=[dataobjects.TYPE_VECTOR_POINT]))

    def processAlgorithm(self, feedback):
        extent = str(self.getParameterValue(self.EXTENT)).split(',')

        spacing = float(self.getParameterValue(self.SPACING))
        inset = float(self.getParameterValue(self.INSET))
        randomize = self.getParameterValue(self.RANDOMIZE)
        isSpacing = self.getParameterValue(self.IS_SPACING)
        crsId = self.getParameterValue(self.CRS)
        crs = QgsCoordinateReferenceSystem()
        crs.createFromUserInput(crsId)

        extent = QgsRectangle(float(extent[0]), float(extent[2]),
                              float(extent[1]), float(extent[3]))

        fields = QgsFields()
        fields.append(QgsField('id', QVariant.Int, '', 10, 0))

        writer = self.getOutputFromName(self.OUTPUT).getVectorWriter(
            fields, QgsWkbTypes.Point, crs)

        if randomize:
            seed()

        area = extent.width() * extent.height()
        if isSpacing:
            pSpacing = spacing
        else:
            pSpacing = sqrt(area / spacing)

        f = QgsFeature()
        f.initAttributes(1)
        f.setFields(fields)

        count = 0
        total = 100.0 / (area / pSpacing)
        y = extent.yMaximum() - inset

        extent_geom = QgsGeometry.fromRect(extent)
        extent_engine = QgsGeometry.createGeometryEngine(extent_geom.geometry())
        extent_engine.prepareGeometry()

        while y >= extent.yMinimum():
            x = extent.xMinimum() + inset
            while x <= extent.xMaximum():
                if randomize:
                    geom = QgsGeometry().fromPoint(QgsPoint(
                        uniform(x - (pSpacing / 2.0), x + (pSpacing / 2.0)),
                        uniform(y - (pSpacing / 2.0), y + (pSpacing / 2.0))))
                else:
                    geom = QgsGeometry().fromPoint(QgsPoint(x, y))

                if extent_engine.intersects(geom.geometry()):
                    f.setAttribute('id', count)
                    f.setGeometry(geom)
                    writer.addFeature(f)
                    x += pSpacing
                    count += 1
                    feedback.setProgress(int(count * total))
            y = y - pSpacing
        del writer

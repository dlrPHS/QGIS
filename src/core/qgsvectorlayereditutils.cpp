/***************************************************************************
    qgsvectorlayereditutils.cpp
    ---------------------
    begin                : Dezember 2012
    copyright            : (C) 2012 by Martin Dobias
    email                : wonder dot sk at gmail dot com
 ***************************************************************************
 *                                                                         *
 *   This program is free software; you can redistribute it and/or modify  *
 *   it under the terms of the GNU General Public License as published by  *
 *   the Free Software Foundation; either version 2 of the License, or     *
 *   (at your option) any later version.                                   *
 *                                                                         *
 ***************************************************************************/
#include "qgsvectorlayereditutils.h"

#include "qgsvectordataprovider.h"
#include "qgsfeatureiterator.h"
#include "qgsgeometrycache.h"
#include "qgsvectorlayereditbuffer.h"
#include "qgslinestring.h"
#include "qgslogger.h"
#include "qgspointv2.h"
#include "qgsgeometryfactory.h"
#include "qgis.h"
#include "qgswkbtypes.h"
#include "qgsvectorlayerutils.h"

#include <limits>


QgsVectorLayerEditUtils::QgsVectorLayerEditUtils( QgsVectorLayer *layer )
  : L( layer )
{
}

bool QgsVectorLayerEditUtils::insertVertex( double x, double y, QgsFeatureId atFeatureId, int beforeVertex )
{
  if ( !L->hasGeometryType() )
    return false;

  QgsGeometry geometry;
  if ( !cache()->geometry( atFeatureId, geometry ) )
  {
    // it's not in cache: let's fetch it from layer
    QgsFeature f;
    if ( !L->getFeatures( QgsFeatureRequest().setFilterFid( atFeatureId ).setSubsetOfAttributes( QgsAttributeList() ) ).nextFeature( f ) || !f.hasGeometry() )
      return false; // geometry not found

    geometry = f.geometry();
  }

  geometry.insertVertex( x, y, beforeVertex );

  L->editBuffer()->changeGeometry( atFeatureId, geometry );
  return true;
}

bool QgsVectorLayerEditUtils::insertVertex( const QgsPointV2 &point, QgsFeatureId atFeatureId, int beforeVertex )
{
  if ( !L->hasGeometryType() )
    return false;

  QgsGeometry geometry;
  if ( !cache()->geometry( atFeatureId, geometry ) )
  {
    // it's not in cache: let's fetch it from layer
    QgsFeature f;
    if ( !L->getFeatures( QgsFeatureRequest().setFilterFid( atFeatureId ).setSubsetOfAttributes( QgsAttributeList() ) ).nextFeature( f ) || !f.hasGeometry() )
      return false; // geometry not found

    geometry = f.geometry();
  }

  geometry.insertVertex( point, beforeVertex );

  L->editBuffer()->changeGeometry( atFeatureId, geometry );
  return true;
}

bool QgsVectorLayerEditUtils::moveVertex( double x, double y, QgsFeatureId atFeatureId, int atVertex )
{
  QgsPointV2 p( x, y );
  return moveVertex( p, atFeatureId, atVertex );
}

bool QgsVectorLayerEditUtils::moveVertex( const QgsPointV2 &p, QgsFeatureId atFeatureId, int atVertex )
{
  if ( !L->hasGeometryType() )
    return false;

  QgsGeometry geometry;
  if ( !cache()->geometry( atFeatureId, geometry ) )
  {
    // it's not in cache: let's fetch it from layer
    QgsFeature f;
    if ( !L->getFeatures( QgsFeatureRequest().setFilterFid( atFeatureId ).setSubsetOfAttributes( QgsAttributeList() ) ).nextFeature( f ) || !f.hasGeometry() )
      return false; // geometry not found

    geometry = f.geometry();
  }

  geometry.moveVertex( p, atVertex );

  L->editBuffer()->changeGeometry( atFeatureId, geometry );
  return true;
}


QgsVectorLayer::EditResult QgsVectorLayerEditUtils::deleteVertex( QgsFeatureId featureId, int vertex )
{
  if ( !L->hasGeometryType() )
    return QgsVectorLayer::InvalidLayer;

  QgsGeometry geometry;
  if ( !cache()->geometry( featureId, geometry ) )
  {
    // it's not in cache: let's fetch it from layer
    QgsFeature f;
    if ( !L->getFeatures( QgsFeatureRequest().setFilterFid( featureId ).setSubsetOfAttributes( QgsAttributeList() ) ).nextFeature( f ) || !f.hasGeometry() )
      return QgsVectorLayer::FetchFeatureFailed; // geometry not found

    geometry = f.geometry();
  }

  if ( !geometry.deleteVertex( vertex ) )
    return QgsVectorLayer::EditFailed;

  if ( geometry.geometry() && geometry.geometry()->nCoordinates() == 0 )
  {
    //last vertex deleted, set geometry to null
    geometry.setGeometry( nullptr );
  }

  L->editBuffer()->changeGeometry( featureId, geometry );
  return !geometry.isNull() ? QgsVectorLayer::Success : QgsVectorLayer::EmptyGeometry;
}

int QgsVectorLayerEditUtils::addRing( const QList<QgsPoint> &ring, const QgsFeatureIds &targetFeatureIds, QgsFeatureId *modifiedFeatureId )
{
  QgsLineString *ringLine = new QgsLineString( ring );
  return addRing( ringLine, targetFeatureIds,  modifiedFeatureId );
}

int QgsVectorLayerEditUtils::addRing( QgsCurve *ring, const QgsFeatureIds &targetFeatureIds, QgsFeatureId *modifiedFeatureId )
{
  if ( !L->hasGeometryType() )
  {
    delete ring;
    return 5;
  }

  int addRingReturnCode = 5; //default: return code for 'ring not inserted'
  QgsFeature f;

  QgsFeatureIterator fit;
  if ( !targetFeatureIds.isEmpty() )
  {
    //check only specified features
    fit = L->getFeatures( QgsFeatureRequest().setFilterFids( targetFeatureIds ) );
  }
  else
  {
    //check all intersecting features
    QgsRectangle bBox = ring->boundingBox();
    fit = L->getFeatures( QgsFeatureRequest().setFilterRect( bBox ).setFlags( QgsFeatureRequest::ExactIntersect ) );
  }

  //find first valid feature we can add the ring to
  while ( fit.nextFeature( f ) )
  {
    if ( !f.hasGeometry() )
      continue;

    //add ring takes ownership of ring, and deletes it if there's an error
    QgsGeometry g = f.geometry();

    addRingReturnCode = g.addRing( static_cast< QgsCurve * >( ring->clone() ) );
    if ( addRingReturnCode == 0 )
    {
      L->editBuffer()->changeGeometry( f.id(), g );
      if ( modifiedFeatureId )
        *modifiedFeatureId = f.id();

      //setModified( true, true );
      break;
    }
  }

  delete ring;
  return addRingReturnCode;
}

int QgsVectorLayerEditUtils::addPart( const QList<QgsPoint> &points, QgsFeatureId featureId )
{
  QgsPointSequence l;
  for ( QList<QgsPoint>::const_iterator it = points.constBegin(); it != points.constEnd(); ++it )
  {
    l <<  QgsPointV2( *it );
  }
  return addPart( l, featureId );
}

int QgsVectorLayerEditUtils::addPart( const QgsPointSequence &points, QgsFeatureId featureId )
{
  if ( !L->hasGeometryType() )
    return 6;

  QgsGeometry geometry;
  bool firstPart = false;
  if ( !cache()->geometry( featureId, geometry ) ) // maybe it's in cache
  {
    // it's not in cache: let's fetch it from layer
    QgsFeature f;
    if ( !L->getFeatures( QgsFeatureRequest().setFilterFid( featureId ).setSubsetOfAttributes( QgsAttributeList() ) ).nextFeature( f ) )
      return 6; //not found

    if ( !f.hasGeometry() )
    {
      //no existing geometry, so adding first part to null geometry
      firstPart = true;
    }
    else
    {
      geometry = f.geometry();
    }
  }

  int errorCode = geometry.addPart( points,  L->geometryType() ) ;
  if ( errorCode == 0 )
  {
    if ( firstPart && QgsWkbTypes::isSingleType( L->wkbType() )
         && L->dataProvider()->doesStrictFeatureTypeCheck() )
    {
      //convert back to single part if required by layer
      geometry.convertToSingleType();
    }
    L->editBuffer()->changeGeometry( featureId, geometry );
  }
  return errorCode;
}

int QgsVectorLayerEditUtils::addPart( QgsCurve *ring, QgsFeatureId featureId )
{
  if ( !L->hasGeometryType() )
    return 6;

  QgsGeometry geometry;
  bool firstPart = false;
  if ( !cache()->geometry( featureId, geometry ) ) // maybe it's in cache
  {
    // it's not in cache: let's fetch it from layer
    QgsFeature f;
    if ( !L->getFeatures( QgsFeatureRequest().setFilterFid( featureId ).setSubsetOfAttributes( QgsAttributeList() ) ).nextFeature( f ) )
      return 6; //not found

    if ( !f.hasGeometry() )
    {
      //no existing geometry, so adding first part to null geometry
      firstPart = true;
    }
    else
    {
      geometry = f.geometry();
    }
  }

  int errorCode = geometry.addPart( ring, L->geometryType() ) ;
  if ( errorCode == 0 )
  {
    if ( firstPart && QgsWkbTypes::isSingleType( L->wkbType() )
         && L->dataProvider()->doesStrictFeatureTypeCheck() )
    {
      //convert back to single part if required by layer
      geometry.convertToSingleType();
    }
    L->editBuffer()->changeGeometry( featureId, geometry );
  }
  return errorCode;
}


int QgsVectorLayerEditUtils::translateFeature( QgsFeatureId featureId, double dx, double dy )
{
  if ( !L->hasGeometryType() )
    return 1;

  QgsGeometry geometry;
  if ( !cache()->geometry( featureId, geometry ) ) // maybe it's in cache
  {
    // it's not in cache: let's fetch it from layer
    QgsFeature f;
    if ( !L->getFeatures( QgsFeatureRequest().setFilterFid( featureId ).setSubsetOfAttributes( QgsAttributeList() ) ).nextFeature( f ) || !f.hasGeometry() )
      return 1; //geometry not found

    geometry = f.geometry();
  }

  int errorCode = geometry.translate( dx, dy );
  if ( errorCode == 0 )
  {
    L->editBuffer()->changeGeometry( featureId, geometry );
  }
  return errorCode;
}


int QgsVectorLayerEditUtils::splitFeatures( const QList<QgsPoint> &splitLine, bool topologicalEditing )
{
  if ( !L->hasGeometryType() )
    return 4;

  QgsFeatureList newFeatures; //store all the newly created features
  double xMin, yMin, xMax, yMax;
  QgsRectangle bBox; //bounding box of the split line
  int returnCode = 0;
  int splitFunctionReturn; //return code of QgsGeometry::splitGeometry
  int numberOfSplitFeatures = 0;

  QgsFeatureIterator features;
  const QgsFeatureIds selectedIds = L->selectedFeatureIds();

  if ( !selectedIds.isEmpty() ) //consider only the selected features if there is a selection
  {
    features = L->selectedFeaturesIterator();
  }
  else //else consider all the feature that intersect the bounding box of the split line
  {
    if ( boundingBoxFromPointList( splitLine, xMin, yMin, xMax, yMax ) == 0 )
    {
      bBox.setXMinimum( xMin );
      bBox.setYMinimum( yMin );
      bBox.setXMaximum( xMax );
      bBox.setYMaximum( yMax );
    }
    else
    {
      return 1;
    }

    if ( bBox.isEmpty() )
    {
      //if the bbox is a line, try to make a square out of it
      if ( bBox.width() == 0.0 && bBox.height() > 0 )
      {
        bBox.setXMinimum( bBox.xMinimum() - bBox.height() / 2 );
        bBox.setXMaximum( bBox.xMaximum() + bBox.height() / 2 );
      }
      else if ( bBox.height() == 0.0 && bBox.width() > 0 )
      {
        bBox.setYMinimum( bBox.yMinimum() - bBox.width() / 2 );
        bBox.setYMaximum( bBox.yMaximum() + bBox.width() / 2 );
      }
      else
      {
        //If we have a single point, we still create a non-null box
        double bufferDistance = 0.000001;
        if ( L->crs().isGeographic() )
          bufferDistance = 0.00000001;
        bBox.setXMinimum( bBox.xMinimum() - bufferDistance );
        bBox.setXMaximum( bBox.xMaximum() + bufferDistance );
        bBox.setYMinimum( bBox.yMinimum() - bufferDistance );
        bBox.setYMaximum( bBox.yMaximum() + bufferDistance );
      }
    }

    features = L->getFeatures( QgsFeatureRequest().setFilterRect( bBox ).setFlags( QgsFeatureRequest::ExactIntersect ) );
  }

  QgsFeature feat;
  while ( features.nextFeature( feat ) )
  {
    if ( !feat.hasGeometry() )
    {
      continue;
    }
    QList<QgsGeometry> newGeometries;
    QList<QgsPoint> topologyTestPoints;
    QgsGeometry featureGeom = feat.geometry();
    splitFunctionReturn = featureGeom.splitGeometry( splitLine, newGeometries, topologicalEditing, topologyTestPoints );
    if ( splitFunctionReturn == 0 )
    {
      //change this geometry
      L->editBuffer()->changeGeometry( feat.id(), featureGeom );

      //insert new features
      for ( int i = 0; i < newGeometries.size(); ++i )
      {
        QgsFeature f = QgsVectorLayerUtils::createFeature( L, newGeometries.at( i ), feat.attributes().toMap() );
        L->editBuffer()->addFeature( f );
      }

      if ( topologicalEditing )
      {
        QList<QgsPoint>::const_iterator topol_it = topologyTestPoints.constBegin();
        for ( ; topol_it != topologyTestPoints.constEnd(); ++topol_it )
        {
          addTopologicalPoints( *topol_it );
        }
      }
      ++numberOfSplitFeatures;
    }
    else if ( splitFunctionReturn > 1 ) //1 means no split but also no error
    {
      returnCode = splitFunctionReturn;
    }
  }

  if ( numberOfSplitFeatures == 0 && !selectedIds.isEmpty() )
  {
    //There is a selection but no feature has been split.
    //Maybe user forgot that only the selected features are split
    returnCode = 4;
  }

  return returnCode;
}

int QgsVectorLayerEditUtils::splitParts( const QList<QgsPoint> &splitLine, bool topologicalEditing )
{
  if ( !L->hasGeometryType() )
    return 4;

  double xMin, yMin, xMax, yMax;
  QgsRectangle bBox; //bounding box of the split line
  int returnCode = 0;
  int splitFunctionReturn; //return code of QgsGeometry::splitGeometry
  int numberOfSplitParts = 0;

  QgsFeatureIterator fit;

  if ( L->selectedFeatureCount() > 0 ) //consider only the selected features if there is a selection
  {
    fit = L->selectedFeaturesIterator();
  }
  else //else consider all the feature that intersect the bounding box of the split line
  {
    if ( boundingBoxFromPointList( splitLine, xMin, yMin, xMax, yMax ) == 0 )
    {
      bBox.setXMinimum( xMin );
      bBox.setYMinimum( yMin );
      bBox.setXMaximum( xMax );
      bBox.setYMaximum( yMax );
    }
    else
    {
      return 1;
    }

    if ( bBox.isEmpty() )
    {
      //if the bbox is a line, try to make a square out of it
      if ( bBox.width() == 0.0 && bBox.height() > 0 )
      {
        bBox.setXMinimum( bBox.xMinimum() - bBox.height() / 2 );
        bBox.setXMaximum( bBox.xMaximum() + bBox.height() / 2 );
      }
      else if ( bBox.height() == 0.0 && bBox.width() > 0 )
      {
        bBox.setYMinimum( bBox.yMinimum() - bBox.width() / 2 );
        bBox.setYMaximum( bBox.yMaximum() + bBox.width() / 2 );
      }
      else
      {
        //If we have a single point, we still create a non-null box
        double bufferDistance = 0.000001;
        if ( L->crs().isGeographic() )
          bufferDistance = 0.00000001;
        bBox.setXMinimum( bBox.xMinimum() - bufferDistance );
        bBox.setXMaximum( bBox.xMaximum() + bufferDistance );
        bBox.setYMinimum( bBox.yMinimum() - bufferDistance );
        bBox.setYMaximum( bBox.yMaximum() + bufferDistance );
      }
    }

    fit = L->getFeatures( QgsFeatureRequest().setFilterRect( bBox ).setFlags( QgsFeatureRequest::ExactIntersect ) );
  }

  int addPartRet = 0;

  QgsFeature feat;
  while ( fit.nextFeature( feat ) )
  {
    QList<QgsGeometry> newGeometries;
    QList<QgsPoint> topologyTestPoints;
    QgsGeometry featureGeom = feat.geometry();
    splitFunctionReturn = featureGeom.splitGeometry( splitLine, newGeometries, topologicalEditing, topologyTestPoints );
    if ( splitFunctionReturn == 0 )
    {
      //add new parts
      if ( !newGeometries.isEmpty() )
        featureGeom.convertToMultiType();

      for ( int i = 0; i < newGeometries.size(); ++i )
      {
        addPartRet = featureGeom.addPart( newGeometries.at( i ) );
        if ( addPartRet )
          break;
      }

      // For test only: Exception already thrown here...
      // feat.geometry()->asWkb();

      if ( !addPartRet )
      {
        L->editBuffer()->changeGeometry( feat.id(), featureGeom );
      }
      else
      {
        // Test addPartRet
        switch ( addPartRet )
        {
          case 1:
            QgsDebugMsg( "Not a multipolygon" );
            break;

          case 2:
            QgsDebugMsg( "Not a valid geometry" );
            break;

          case 3:
            QgsDebugMsg( "New polygon ring" );
            break;
        }
      }
      L->editBuffer()->changeGeometry( feat.id(), featureGeom );

      if ( topologicalEditing )
      {
        QList<QgsPoint>::const_iterator topol_it = topologyTestPoints.constBegin();
        for ( ; topol_it != topologyTestPoints.constEnd(); ++topol_it )
        {
          addTopologicalPoints( *topol_it );
        }
      }
      ++numberOfSplitParts;
    }
    else if ( splitFunctionReturn > 1 ) //1 means no split but also no error
    {
      returnCode = splitFunctionReturn;
    }
  }

  if ( numberOfSplitParts == 0 && L->selectedFeatureCount() > 0  && returnCode == 0 )
  {
    //There is a selection but no feature has been split.
    //Maybe user forgot that only the selected features are split
    returnCode = 4;
  }

  return returnCode;
}


int QgsVectorLayerEditUtils::addTopologicalPoints( const QgsGeometry &geom )
{
  if ( !L->hasGeometryType() )
    return 1;

  if ( geom.isNull() )
  {
    return 1;
  }

  int returnVal = 0;

  QgsWkbTypes::Type wkbType = geom.wkbType();

  switch ( wkbType )
  {
    //line
    case QgsWkbTypes::LineString25D:
    case QgsWkbTypes::LineString:
    {
      QgsPolyline line = geom.asPolyline();
      QgsPolyline::const_iterator line_it = line.constBegin();
      for ( ; line_it != line.constEnd(); ++line_it )
      {
        if ( addTopologicalPoints( *line_it ) != 0 )
        {
          returnVal = 2;
        }
      }
      break;
    }

    //multiline
    case QgsWkbTypes::MultiLineString25D:
    case QgsWkbTypes::MultiLineString:
    {
      QgsMultiPolyline multiLine = geom.asMultiPolyline();
      QgsPolyline currentPolyline;

      for ( int i = 0; i < multiLine.size(); ++i )
      {
        QgsPolyline::const_iterator line_it = currentPolyline.constBegin();
        for ( ; line_it != currentPolyline.constEnd(); ++line_it )
        {
          if ( addTopologicalPoints( *line_it ) != 0 )
          {
            returnVal = 2;
          }
        }
      }
      break;
    }

    //polygon
    case QgsWkbTypes::Polygon25D:
    case QgsWkbTypes::Polygon:
    {
      QgsPolygon polygon = geom.asPolygon();
      QgsPolyline currentRing;

      for ( int i = 0; i < polygon.size(); ++i )
      {
        currentRing = polygon.at( i );
        QgsPolyline::const_iterator line_it = currentRing.constBegin();
        for ( ; line_it != currentRing.constEnd(); ++line_it )
        {
          if ( addTopologicalPoints( *line_it ) != 0 )
          {
            returnVal = 2;
          }
        }
      }
      break;
    }

    //multipolygon
    case QgsWkbTypes::MultiPolygon25D:
    case QgsWkbTypes::MultiPolygon:
    {
      QgsMultiPolygon multiPolygon = geom.asMultiPolygon();
      QgsPolygon currentPolygon;
      QgsPolyline currentRing;

      for ( int i = 0; i < multiPolygon.size(); ++i )
      {
        currentPolygon = multiPolygon.at( i );
        for ( int j = 0; j < currentPolygon.size(); ++j )
        {
          currentRing = currentPolygon.at( j );
          QgsPolyline::const_iterator line_it = currentRing.constBegin();
          for ( ; line_it != currentRing.constEnd(); ++line_it )
          {
            if ( addTopologicalPoints( *line_it ) != 0 )
            {
              returnVal = 2;
            }
          }
        }
      }
      break;
    }
    default:
      break;
  }
  return returnVal;
}


int QgsVectorLayerEditUtils::addTopologicalPoints( const QgsPoint &p )
{
  if ( !L->hasGeometryType() )
    return 1;

  double segmentSearchEpsilon = L->crs().isGeographic() ? 1e-12 : 1e-8;

  //work with a tolerance because coordinate projection may introduce some rounding
  double threshold =  0.0000001;
  if ( L->crs().mapUnits() == QgsUnitTypes::DistanceMeters )
  {
    threshold = 0.001;
  }
  else if ( L->crs().mapUnits() == QgsUnitTypes::DistanceFeet )
  {
    threshold = 0.0001;
  }

  QgsRectangle searchRect( p.x() - threshold, p.y() - threshold,
                           p.x() + threshold, p.y() + threshold );
  double sqrSnappingTolerance = threshold * threshold;

  QgsFeature f;
  QgsFeatureIterator fit = L->getFeatures( QgsFeatureRequest()
                           .setFilterRect( searchRect )
                           .setFlags( QgsFeatureRequest::ExactIntersect )
                           .setSubsetOfAttributes( QgsAttributeList() ) );

  QMap<QgsFeatureId, QgsGeometry> features;
  QMap<QgsFeatureId, int> segments;

  while ( fit.nextFeature( f ) )
  {
    int afterVertex;
    QgsPoint snappedPoint;
    double sqrDistSegmentSnap = f.geometry().closestSegmentWithContext( p, snappedPoint, afterVertex, nullptr, segmentSearchEpsilon );
    if ( sqrDistSegmentSnap < sqrSnappingTolerance )
    {
      segments[f.id()] = afterVertex;
      features[f.id()] = f.geometry();
    }
  }

  if ( segments.isEmpty() )
    return 2;

  for ( QMap<QgsFeatureId, int>::const_iterator it = segments.constBegin(); it != segments.constEnd(); ++it )
  {
    QgsFeatureId fid = it.key();
    int segmentAfterVertex = it.value();
    QgsGeometry geom = features[fid];

    int atVertex, beforeVertex, afterVertex;
    double sqrDistVertexSnap;
    geom.closestVertex( p, atVertex, beforeVertex, afterVertex, sqrDistVertexSnap );

    if ( sqrDistVertexSnap < sqrSnappingTolerance )
      continue;  // the vertex already exists - do not insert it

    if ( !L->insertVertex( p.x(), p.y(), fid, segmentAfterVertex ) )
    {
      QgsDebugMsg( "failed to insert topo point" );
    }
  }

  return 0;
}


int QgsVectorLayerEditUtils::boundingBoxFromPointList( const QList<QgsPoint> &list, double &xmin, double &ymin, double &xmax, double &ymax ) const
{
  if ( list.size() < 1 )
  {
    return 1;
  }

  xmin = std::numeric_limits<double>::max();
  xmax = -std::numeric_limits<double>::max();
  ymin = std::numeric_limits<double>::max();
  ymax = -std::numeric_limits<double>::max();

  for ( QList<QgsPoint>::const_iterator it = list.constBegin(); it != list.constEnd(); ++it )
  {
    if ( it->x() < xmin )
    {
      xmin = it->x();
    }
    if ( it->x() > xmax )
    {
      xmax = it->x();
    }
    if ( it->y() < ymin )
    {
      ymin = it->y();
    }
    if ( it->y() > ymax )
    {
      ymax = it->y();
    }
  }

  return 0;
}

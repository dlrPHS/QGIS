/***************************************************************************
    qgsmaptoolzoom.cpp  -  map tool for zooming
    ----------------------
    begin                : January 2006
    copyright            : (C) 2006 by Martin Dobias
    email                : wonder.sk at gmail dot com
 ***************************************************************************
 *                                                                         *
 *   This program is free software; you can redistribute it and/or modify  *
 *   it under the terms of the GNU General Public License as published by  *
 *   the Free Software Foundation; either version 2 of the License, or     *
 *   (at your option) any later version.                                   *
 *                                                                         *
 ***************************************************************************/

#include "qgsmaptoolzoom.h"
#include "qgsmapcanvas.h"
#include "qgsmaptopixel.h"
#include "qgscursors.h"
#include "qgsrubberband.h"

#include <QMouseEvent>
#include <QRect>
#include <QColor>
#include <QCursor>
#include <QPixmap>
#include "qgslogger.h"


QgsMapToolZoom::QgsMapToolZoom( QgsMapCanvas *canvas, bool zoomOut )
  : QgsMapTool( canvas )
  , mZoomOut( zoomOut )
  , mDragging( false )
  , mRubberBand( nullptr )
{
  mToolName = tr( "Zoom" );
  // set the cursor
  QPixmap myZoomQPixmap = QPixmap( ( const char ** )( zoomOut ? zoom_out : zoom_in ) );
  mCursor = QCursor( myZoomQPixmap, 7, 7 );
}

QgsMapToolZoom::~QgsMapToolZoom()
{
  delete mRubberBand;
}


void QgsMapToolZoom::canvasMoveEvent( QgsMapMouseEvent *e )
{
  if ( !( e->buttons() & Qt::LeftButton ) )
    return;

  if ( !mDragging )
  {
    mDragging = true;
    delete mRubberBand;
    mRubberBand = new QgsRubberBand( mCanvas, QgsWkbTypes::PolygonGeometry );
    QColor color( Qt::blue );
    color.setAlpha( 63 );
    mRubberBand->setColor( color );
    mZoomRect.setTopLeft( e->pos() );
  }
  mZoomRect.setBottomRight( e->pos() );
  if ( mRubberBand )
  {
    mRubberBand->setToCanvasRectangle( mZoomRect );
    mRubberBand->show();
  }
}


void QgsMapToolZoom::canvasPressEvent( QgsMapMouseEvent *e )
{
  if ( e->button() != Qt::LeftButton )
    return;

  mZoomRect.setRect( 0, 0, 0, 0 );
}


void QgsMapToolZoom::canvasReleaseEvent( QgsMapMouseEvent *e )
{
  if ( e->button() != Qt::LeftButton )
    return;

  bool zoomOut = mZoomOut;
  if ( e->modifiers() & Qt::AltModifier )
    zoomOut = !zoomOut;

  // We are not really dragging in this case. This is sometimes caused by
  // a pen based computer reporting a press, move, and release, all the
  // one point.
  if ( mDragging && ( mZoomRect.topLeft() == mZoomRect.bottomRight() ) )
  {
    mDragging = false;
    delete mRubberBand;
    mRubberBand = nullptr;
  }

  if ( mDragging )
  {
    mDragging = false;
    delete mRubberBand;
    mRubberBand = nullptr;

    // store the rectangle
    mZoomRect.setRight( e->pos().x() );
    mZoomRect.setBottom( e->pos().y() );

    //account for bottom right -> top left dragging
    mZoomRect = mZoomRect.normalized();

    // set center and zoom
    const QSize &zoomRectSize = mZoomRect.size();
    const QgsMapSettings &mapSettings = mCanvas->mapSettings();
    const QSize &canvasSize = mapSettings.outputSize();
    double sfx = ( double )zoomRectSize.width() / canvasSize.width();
    double sfy = ( double )zoomRectSize.height() / canvasSize.height();
    double sf = qMax( sfx, sfy );

    const QgsMapToPixel *m2p = mCanvas->getCoordinateTransform();
    QgsPoint c = m2p->toMapCoordinates( mZoomRect.center() );

    mCanvas->zoomByFactor( zoomOut ? 1.0 / sf : sf, &c );

    mCanvas->refresh();
  }
  else // not dragging
  {
    // change to zoom in/out by the default multiple
    mCanvas->zoomWithCenter( e->x(), e->y(), !zoomOut );
  }
}

void QgsMapToolZoom::deactivate()
{
  delete mRubberBand;
  mRubberBand = nullptr;

  QgsMapTool::deactivate();
}

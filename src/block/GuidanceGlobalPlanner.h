// Copyright( C ) 2019 Christian Riggenbach
//
// This program is free software:
// you can redistribute it and / or modify
// it under the terms of the GNU General Public License as published by
// the Free Software Foundation, either version 3 of the License, or
// ( at your option ) any later version.
//
// This program is distributed in the hope that it will be useful,
//      but WITHOUT ANY WARRANTY;
// without even the implied warranty of
// MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
//
// GNU General Public License for more details.
//
// You should have received a copy of the GNU General Public License
// along with this program.  If not, see < https : //www.gnu.org/licenses/>.

#ifndef GLOBALPLANNER_H
#define GLOBALPLANNER_H

#include <QObject>

#include <QQuaternion>
#include <QVector3D>
#include <QPointF>
#include <QPolygonF>
#include <QLineF>

#include <Qt3DCore/QEntity>
#include <Qt3DCore/QTransform>
#include <Qt3DExtras/QSphereMesh>
#include <Qt3DExtras/QPhongMaterial>
#include <Qt3DExtras/QDiffuseSpecularMaterial>
#include <Qt3DExtras/QExtrudedTextMesh>

#include <QDebug>

#include "BlockBase.h"

#include "qneblock.h"
#include "qneport.h"

#include "../kinematic/Tile.h"
#include "../kinematic/PoseOptions.h"
#include "../kinematic/PathPrimitive.h"

#include <QVector>
#include <QSharedPointer>
#include <utility>


class GlobalPlanner : public BlockBase {
    Q_OBJECT

  public:
    explicit GlobalPlanner( Tile* tile, Qt3DCore::QEntity* rootEntity )
      : BlockBase(),
        rootEntity( rootEntity ) {
      this->tile = tile->getTileForOffset( 0, 0 );
      this->tile00 = tile->getTileForOffset( 0, 0 );

      // a point marker -> orange
      {
        aPointEntity = new Qt3DCore::QEntity( tile->tileEntity );

        aPointMesh = new Qt3DExtras::QSphereMesh();
        aPointMesh->setRadius( .2f );
        aPointMesh->setSlices( 20 );
        aPointMesh->setRings( 20 );

        aPointTransform = new Qt3DCore::QTransform();

        Qt3DExtras::QPhongMaterial* material = new Qt3DExtras::QPhongMaterial();
        material->setDiffuse( QColor( "orange" ) );

        aPointEntity->addComponent( aPointMesh );
        aPointEntity->addComponent( material );
        aPointEntity->addComponent( aPointTransform );
        aPointEntity->setEnabled( false );

        aTextEntity = new Qt3DCore::QEntity( aPointEntity );
        Qt3DExtras::QExtrudedTextMesh* aTextMesh = new Qt3DExtras::QExtrudedTextMesh();
        aTextMesh->setText( "A" );
        aTextMesh->setDepth( 0.05f );

        aTextEntity->setEnabled( true );
        aTextTransform = new Qt3DCore::QTransform();
        aTextTransform->setRotation( QQuaternion::fromAxisAndAngle( QVector3D( 0, 0, 1 ), -90 ) );
        aTextTransform->setScale( 2.0f );
        aTextTransform->setTranslation( QVector3D( 0, -.2f, 0 ) );
        aTextEntity->addComponent( aTextTransform );
        aTextEntity->addComponent( aTextMesh );
        aTextEntity->addComponent( material );
      }

      // b point marker -> purple
      {
        bPointMesh = new Qt3DExtras::QSphereMesh();
        bPointMesh->setRadius( .2f );
        bPointMesh->setSlices( 20 );
        bPointMesh->setRings( 20 );

        bPointTransform = new Qt3DCore::QTransform();

        Qt3DExtras::QPhongMaterial* material = new Qt3DExtras::QPhongMaterial();
        material->setDiffuse( QColor( "purple" ) );

        bPointEntity = new Qt3DCore::QEntity( tile->tileEntity );
        bPointEntity->addComponent( bPointMesh );
        bPointEntity->addComponent( material );
        bPointEntity->addComponent( bPointTransform );
        bPointEntity->setEnabled( false );

        bTextEntity = new Qt3DCore::QEntity( bPointEntity );
        Qt3DExtras::QExtrudedTextMesh* bTextMesh = new Qt3DExtras::QExtrudedTextMesh();
        bTextMesh->setText( "B" );
        bTextMesh->setDepth( 0.05f );

        bTextEntity->setEnabled( true );
        bTextTransform = new Qt3DCore::QTransform();
        bTextTransform->setRotation( QQuaternion::fromAxisAndAngle( QVector3D( 0, 0, 1 ), -90 ) );
        bTextTransform->setScale( 2.0f );
        bTextTransform->setTranslation( QVector3D( 0, -.2f, 0 ) );
        bTextEntity->addComponent( bTextTransform );
        bTextEntity->addComponent( bTextMesh );
        bTextEntity->addComponent( material );
      }

      // line marker
      {
        pathEntity = new Qt3DCore::QEntity( tile00->tileEntity );
        pathEntity->setEnabled( false );

        pathTransform = new Qt3DCore::QTransform();
        pathEntity->addComponent( pathTransform );

        pathMesh = new LineMesh();
        pathEntity->addComponent( pathMesh );

        pathMaterial = new Qt3DExtras::QDiffuseSpecularMaterial( pathEntity );
        pathMaterial->setAmbient( Qt::red );
        pathEntity->addComponent( pathMaterial );
      }
    }

  public slots:
    void setPose( Tile* tile, QVector3D position, QQuaternion orientation, PoseOption::Options options ) {
      if( !options.testFlag( PoseOption::CalculateLocalOffsets ) ) {
        this->tile = tile;
        this->position = position;
        this->orientation = orientation;

        aPointTransform->setRotation( orientation );
        bPointTransform->setRotation( orientation );
      }
    }

    void setPoseLeftEdge( Tile*, QVector3D position, QQuaternion, PoseOption::Options options ) {
      if( options.testFlag( PoseOption::CalculateLocalOffsets ) &&
          options.testFlag( PoseOption::CalculateWithoutTiling ) &&
          options.testFlag( PoseOption::CalculateWithoutOrientation ) ) {
        positionLeftEdgeOfImplement = position;
        implementLine.setP1( QPointF( double( position.x() ), double( position.y() ) ) );
      }
    }

    void setPoseRightEdge( Tile*, QVector3D position, QQuaternion, PoseOption::Options options ) {
      if( options.testFlag( PoseOption::CalculateLocalOffsets ) &&
          options.testFlag( PoseOption::CalculateWithoutTiling ) &&
          options.testFlag( PoseOption::CalculateWithoutOrientation ) ) {
        positionRightEdgeOfImplement = position;
        implementLine.setP2( QPointF( double( position.x() ), double( position.y() ) ) );
      }
    }

    void a_clicked() {
      aPointEntity->setParent( tile->tileEntity );
      aPointTransform->setTranslation( position );

      aPointEntity->setEnabled( true );
      bPointEntity->setEnabled( false );
      pathEntity->setEnabled( false );

      aPoint = QPointF( double( position.x() ) + tile->x, double( position.y() ) + tile->y );

      qDebug() << "a_clicked()" << aPoint;
    }

    void b_clicked() {
      bPointEntity->setParent( tile->tileEntity );
      bPointTransform->setTranslation( position );
      bPointEntity->setEnabled( true );

      bPoint = QPointF( double( position.x() ) + tile->x, double( position.y() ) + tile->y );

      abLine.setPoints( aPoint, bPoint );

      QVector<QVector3D> linePoints;

      // extend the points 200m in either direction

      qreal headingOfABLine = abLine.angle();

      QLineF lineExtensionFromA = QLineF::fromPolar( -200, headingOfABLine );
      lineExtensionFromA.translate( aPoint );

      QLineF lineExtensionFromB = QLineF::fromPolar( 200, headingOfABLine );
      lineExtensionFromB.translate( bPoint );

      QLineF pathLine( lineExtensionFromA.p2(), lineExtensionFromB.p2() );

//      pathLine = abLine;

      QVector<QSharedPointer<PathPrimitive>> plan;

      for( uint16_t i = 0; i < pathsToGenerate; ++i ) {
        plan.append( QSharedPointer<PathPrimitive>(
                             new PathPrimitiveLine(
                                     pathLine.translated(
                                             QLineF::fromPolar( i * implementLine.dy() +
                                                 implementLine.center().y(),
                                                 headingOfABLine - 90 ).p2() ), false, true ) ) );
      }

      for( uint16_t i = 0; i < pathsToGenerate; ++i ) {
        plan.append( QSharedPointer<PathPrimitive>(
                             new PathPrimitiveLine(
                                     pathLine.translated(
                                             QLineF::fromPolar( i * implementLine.dy() +
                                                 implementLine.center().y(),
                                                 headingOfABLine - 270 ).p2() ), false, true ) ) );
      }

      // display paths
      {
        for( const auto& primitive : plan ) {
          auto* line =  qobject_cast<PathPrimitiveLine*>( primitive.data() );

          if( line ) {
            linePoints.append( QVector3D( float( line->line.x1() ), float( line->line.y1() ), position.z() ) );
            linePoints.append( QVector3D( float( line->line.x2() ), float( line->line.y2() ), position.z() ) );
          }
        }

        pathMesh->posUpdate( linePoints );
        pathEntity->setEnabled( true );
      }

      qDebug() << "b_clicked()" << "abLine" << abLine << "pathLine" << pathLine << "heading" << headingOfABLine;

      emit planChanged( plan );
    }

    void snap_clicked() {
      qDebug() << "snap_clicked()";
    }

    void turnLeft_clicked() {
      qDebug() << "turnLeft_clicked()";
    }
    void turnRight_clicked() {
      qDebug() << "turnRight_clicked()";
    }

    void setPlannerSettings( int pathsToGenerate, int pathsInReserve ) {
      this->pathsToGenerate = pathsToGenerate;
      this->pathsInReserve = pathsInReserve;
    }

  signals:
    void planChanged( QVector<QSharedPointer<PathPrimitive>> );

  public:
    Tile* tile = nullptr;
    Tile* tile00 = nullptr;
    QVector3D position = QVector3D();
    QQuaternion orientation = QQuaternion();

    int pathsToGenerate = 5;
    int pathsInReserve = 3;

    QPointF aPoint = QPointF();
    QPointF bPoint = QPointF();
    QLineF abLine = QLineF();

    QLineF implementLine = QLineF();

    QVector3D positionLeftEdgeOfImplement = QVector3D();
    QVector3D positionRightEdgeOfImplement = QVector3D();

  private:
    Qt3DCore::QEntity* rootEntity = nullptr;

    // markers
    Qt3DCore::QEntity* aPointEntity = nullptr;
    Qt3DExtras::QSphereMesh* aPointMesh = nullptr;
    Qt3DCore::QTransform* aPointTransform = nullptr;
    Qt3DCore::QEntity* aTextEntity = nullptr;
    Qt3DCore::QTransform* aTextTransform = nullptr;

    Qt3DCore::QEntity* bPointEntity = nullptr;
    Qt3DExtras::QSphereMesh* bPointMesh = nullptr;
    Qt3DCore::QTransform* bPointTransform = nullptr;
    Qt3DCore::QEntity* bTextEntity = nullptr;
    Qt3DCore::QTransform* bTextTransform = nullptr;

    // path
    Qt3DCore::QEntity* pathEntity = nullptr;
    LineMesh* pathMesh = nullptr;
    Qt3DCore::QTransform* pathTransform = nullptr;
    Qt3DExtras::QDiffuseSpecularMaterial* pathMaterial = nullptr;
};

class GlobalPlannerFactory : public BlockFactory {
    Q_OBJECT

  public:
    GlobalPlannerFactory( Tile* tile, Qt3DCore::QEntity* rootEntity )
      : BlockFactory(),
        tile( tile ),
        rootEntity( rootEntity ) {}

    QString getNameOfFactory() override {
      return QStringLiteral( "Global Planner" );
    }

    virtual void addToCombobox( QComboBox* combobox ) override {
      combobox->addItem( getNameOfFactory(), QVariant::fromValue( this ) );
    }

    virtual BlockBase* createNewObject() override {
      return new GlobalPlanner( tile, rootEntity );
    }

    virtual QNEBlock* createBlock( QGraphicsScene* scene, QObject* obj ) override {
      auto* b = createBaseBlock( scene, obj, true );

      b->addInputPort( "Pose", SLOT( setPose( Tile*, QVector3D, QQuaternion, PoseOption::Options ) ) );
      b->addInputPort( "Pose Left Edge", SLOT( setPoseLeftEdge( Tile*, QVector3D, QQuaternion, PoseOption::Options ) ) );
      b->addInputPort( "Pose Right Edge", SLOT( setPoseRightEdge( Tile*, QVector3D, QQuaternion, PoseOption::Options ) ) );
      b->addInputPort( "A clicked", SLOT( a_clicked() ) );
      b->addInputPort( "B clicked", SLOT( b_clicked() ) );
      b->addInputPort( "Snap clicked", SLOT( snap_clicked() ) );
      b->addInputPort( "Turn Left", SLOT( turnLeft_clicked() ) );
      b->addInputPort( "Turn Right", SLOT( turnRight_clicked() ) );

      b->addOutputPort( "Plan", SIGNAL( planChanged( QVector<QSharedPointer<PathPrimitive>> ) ) );

      return b;
    }

  private:
    Tile* tile = nullptr;
    Qt3DCore::QEntity* rootEntity = nullptr;
};

#endif // GLOBALPLANNER_H


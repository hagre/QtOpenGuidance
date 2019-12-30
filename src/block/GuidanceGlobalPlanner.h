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

#include "../3d/linemesh.h"

#include "../gui/FieldsOptimitionToolbar.h"

#include "../cgal.h"
#include "../kinematic/PoseOptions.h"
#include "../kinematic/PathPrimitive.h"

#include "../kinematic/TransverseMercatorWrapper.h"

#include <QVector>
#include <QSharedPointer>
#include <utility>

class GlobalPlanner : public BlockBase {
    Q_OBJECT

  public:
    explicit GlobalPlanner( Qt3DCore::QEntity* rootEntity, TransverseMercatorWrapper* tmw )
      : BlockBase(),
        tmw( tmw ) {
      // a point marker -> orange
      {
        aPointMesh = new Qt3DExtras::QSphereMesh();
        aPointMesh->setRadius( .2f );
        aPointMesh->setSlices( 20 );
        aPointMesh->setRings( 20 );

        aPointTransform = new Qt3DCore::QTransform();

        Qt3DExtras::QPhongMaterial* material = new Qt3DExtras::QPhongMaterial();
        material->setDiffuse( QColor( "orange" ) );

        aPointEntity = new Qt3DCore::QEntity( rootEntity );
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

        bPointEntity = new Qt3DCore::QEntity( rootEntity );
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

      // test for recording
      {
        m_baseEntity = new Qt3DCore::QEntity( rootEntity );
        m_baseTransform = new Qt3DCore::QTransform();
        m_baseEntity->addComponent( m_baseTransform );

        m_pointsEntity = new Qt3DCore::QEntity( m_baseEntity );
        m_segmentsEntity = new Qt3DCore::QEntity( m_baseEntity );
        m_segmentsEntity2 = new Qt3DCore::QEntity( m_baseEntity );
        m_segmentsEntity3 = new Qt3DCore::QEntity( m_baseEntity );
        m_segmentsEntity4 = new Qt3DCore::QEntity( m_baseEntity );

        m_pointsMesh = new LineMesh();
        m_pointsMesh->setPrimitiveType( Qt3DRender::QGeometryRenderer::Points );
        m_pointsEntity->addComponent( m_pointsMesh );

        m_segmentsMesh = new LineMesh();
        m_segmentsEntity->addComponent( m_segmentsMesh );

        m_segmentsMesh2 = new LineMesh();
        m_segmentsEntity2->addComponent( m_segmentsMesh2 );

        m_segmentsMesh3 = new LineMesh();
        m_segmentsEntity3->addComponent( m_segmentsMesh3 );

        m_segmentsMesh4 = new LineMesh();
        m_segmentsEntity4->addComponent( m_segmentsMesh4 );

        m_pointsMaterial = new Qt3DExtras::QPhongMaterial( m_pointsEntity );
        m_segmentsMaterial = new Qt3DExtras::QPhongMaterial( m_segmentsEntity );
        m_segmentsMaterial2 = new Qt3DExtras::QPhongMaterial( m_segmentsEntity2 );
        m_segmentsMaterial3 = new Qt3DExtras::QPhongMaterial( m_segmentsEntity3 );
        m_segmentsMaterial4 = new Qt3DExtras::QPhongMaterial( m_segmentsEntity4 );

        m_pointsMaterial->setAmbient( Qt::yellow );
        m_segmentsMaterial->setAmbient( Qt::white );
        m_segmentsMaterial2->setAmbient( Qt::green );
        m_segmentsMaterial3->setAmbient( Qt::blue );
        m_segmentsMaterial4->setAmbient( Qt::red );

        m_pointsEntity->addComponent( m_pointsMaterial );
        m_segmentsEntity->addComponent( m_segmentsMaterial );
        m_segmentsEntity2->addComponent( m_segmentsMaterial2 );
        m_segmentsEntity3->addComponent( m_segmentsMaterial3 );
        m_segmentsEntity4->addComponent( m_segmentsMaterial4 );
      }
    }

    void alphaShape();

  public slots:
    void setPose( Point_3 position, QQuaternion orientation, PoseOption::Options options ) {
      if( !options.testFlag( PoseOption::CalculateLocalOffsets ) ) {
        this->position = position;
        this->orientation = orientation;

        aPointTransform->setRotation( orientation );
        bPointTransform->setRotation( orientation );
      }
    }

    void setPoseLeftEdge( Point_3 position, QQuaternion, PoseOption::Options options ) {
      if( options.testFlag( PoseOption::CalculateLocalOffsets ) &&
          options.testFlag( PoseOption::CalculateWithoutOrientation ) ) {
        positionLeftEdgeOfImplement = position;

        Point_2 point2D( double( position.x() ), double( position.y() ) );

        if( implementLine.source() != point2D ) {
          implementLine = Segment_2( point2D, implementLine.target() );
//          createPlanAB();
        }
      } else {
        if( recordOnRightEdgeOfImplement == false ) {
          if( recordContinous ) {
            points.push_back( position );
            recordNextPoint = false;
          }

          if( recordNextPoint ) {
            points.push_back( position );
            recordNextPoint = false;
          }
        }
      }
    }

    void setPoseRightEdge( Point_3 position, QQuaternion, PoseOption::Options options ) {
      if( options.testFlag( PoseOption::CalculateLocalOffsets ) &&
          options.testFlag( PoseOption::CalculateWithoutOrientation ) ) {
        positionRightEdgeOfImplement = position;

        Point_2 point2D( double( position.x() ), double( position.y() ) );

        if( implementLine.target() != point2D ) {
          implementLine = Segment_2( implementLine.source(), point2D );
//          createPlanAB();
        }
      } else {
        if( recordOnRightEdgeOfImplement == true ) {
          if( recordContinous ) {
            points.push_back( position );
            recordNextPoint = false;
          }

          if( recordNextPoint ) {
            points.push_back( position );
            recordNextPoint = false;
          }
        }
      }
    }

    void openField() {}
    void newField() {
      points.clear();
    }
    void saveField() {}

    void setContinousRecord( bool enabled ) {
      if( recordContinous == true && enabled == false ) {
        recalculateField();
      }

      recordContinous = enabled;
    }
    void recordPoint() {
      recordNextPoint = true;
    }

    void recordOnEdgeOfImplementChanged( bool right ) {
      recordOnRightEdgeOfImplement = right;
    }

    void recalculateField() {
      alphaShape();
    }

    void setRecalculateFieldSettings( FieldsOptimitionToolbar::AlphaType alphaType, double customAlpha, double maxDeviation ) {
      this->alphaType = alphaType;
      this->customAlpha = customAlpha;
      this->maxDeviation = maxDeviation;
      recalculateField();
    }

    void a_clicked() {
      aPointTransform->setTranslation( convertPoint3ToQVector3D( position ) );

      aPointEntity->setEnabled( true );
      bPointEntity->setEnabled( false );

      aPoint = position;

      points.clear();

      qDebug() << "a_clicked()"/* << aPoint*/;
    }

    void createPlanAB();

    // form polygons from alpha shape
    void alphaToPolygon( const Alpha_shape_2& A,
                         Polygon_with_holes_2& out_poly );


    void b_clicked() {
      bPointTransform->setTranslation( convertPoint3ToQVector3D( position ) );
      bPointEntity->setEnabled( true );

      bPoint = position;

      abLine = Segment_3( aPoint, bPoint );

      createPlanAB();


//      optimalTransportationReconstruction();
//      alphaShape();
    }

    void snap_clicked() {
//      optimalTransportationReconstruction();
      alphaShape();

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

      createPlanAB();
    }

    void setPassSettings( int forwardPasses, int reversePasses, bool startRight, bool mirror ) {
      if( ( forwardPasses == 0 || reversePasses == 0 ) ) {
        this->forwardPasses = 0;
        this->reversePasses = 0;
      } else {
        this->forwardPasses = forwardPasses;
        this->reversePasses = reversePasses;
      }

      this->startRight = startRight;
      this->mirror = mirror;

      createPlanAB();
    }

    void setPassNumberTo( int /*passNumber*/ ) {}

  signals:
    void alphaChanged( double optimal, double solid );
    void fieldStatisticsChanged( size_t pointsRecorded, size_t pointsInPolygon );
    void planChanged( QVector<QSharedPointer<PathPrimitive>> );

  public:
    Point_3 position = Point_3();
    QQuaternion orientation = QQuaternion();

    int pathsToGenerate = 5;
    int pathsInReserve = 3;
    int forwardPasses = 0;
    int reversePasses = 0;
    bool startRight = false;
    bool mirror = false;

    Point_3 aPoint = Point_3();
    Point_3 bPoint = Point_3();
    Segment_3 abLine = Segment_3();

    Segment_2 implementLine = Segment_2();

    Point_3 positionLeftEdgeOfImplement = Point_3();
    Point_3 positionRightEdgeOfImplement = Point_3();

  private:
    Qt3DCore::QEntity* rootEntity = nullptr;
    TransverseMercatorWrapper* tmw = nullptr;

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

    std::vector<K::Point_3> points;
    bool recordContinous = false;
    bool recordNextPoint = false;
    bool recordOnRightEdgeOfImplement = false;

    FieldsOptimitionToolbar::AlphaType alphaType = FieldsOptimitionToolbar::AlphaType::Optimal;
    double customAlpha = 10;
    double maxDeviation = 0.01;

    Qt3DCore::QEntity* m_baseEntity = nullptr;
    Qt3DCore::QTransform* m_baseTransform = nullptr;

    Qt3DCore::QEntity* m_pointsEntity = nullptr;
    Qt3DCore::QEntity* m_segmentsEntity = nullptr;
    Qt3DCore::QEntity* m_segmentsEntity2 = nullptr;
    Qt3DCore::QEntity* m_segmentsEntity3 = nullptr;
    Qt3DCore::QEntity* m_segmentsEntity4 = nullptr;
    LineMesh* m_pointsMesh = nullptr;
    LineMesh* m_segmentsMesh = nullptr;
    LineMesh* m_segmentsMesh2 = nullptr;
    LineMesh* m_segmentsMesh3 = nullptr;
    LineMesh* m_segmentsMesh4 = nullptr;
    Qt3DExtras::QPhongMaterial* m_pointsMaterial = nullptr;
    Qt3DExtras::QPhongMaterial* m_segmentsMaterial = nullptr;
    Qt3DExtras::QPhongMaterial* m_segmentsMaterial2 = nullptr;
    Qt3DExtras::QPhongMaterial* m_segmentsMaterial3 = nullptr;
    Qt3DExtras::QPhongMaterial* m_segmentsMaterial4 = nullptr;
};

class GlobalPlannerFactory : public BlockFactory {
    Q_OBJECT

  public:
    GlobalPlannerFactory( Qt3DCore::QEntity* rootEntity, TransverseMercatorWrapper* tmw )
      : BlockFactory(),
        rootEntity( rootEntity ),
        tmw( tmw ) {}

    QString getNameOfFactory() override {
      return QStringLiteral( "Global Planner" );
    }

    virtual void addToCombobox( QComboBox* combobox ) override {
      combobox->addItem( getNameOfFactory(), QVariant::fromValue( this ) );
    }

    virtual BlockBase* createNewObject() override {
      return new GlobalPlanner( rootEntity, tmw );
    }

    virtual QNEBlock* createBlock( QGraphicsScene* scene, QObject* obj ) override {
      auto* b = createBaseBlock( scene, obj, true );

      b->addInputPort( "Pose", SLOT( setPose( Point_3, QQuaternion, PoseOption::Options ) ) );
      b->addInputPort( "Pose Left Edge", SLOT( setPoseLeftEdge( Point_3, QQuaternion, PoseOption::Options ) ) );
      b->addInputPort( "Pose Right Edge", SLOT( setPoseRightEdge( Point_3, QQuaternion, PoseOption::Options ) ) );
      b->addInputPort( "A clicked", SLOT( a_clicked() ) );
      b->addInputPort( "B clicked", SLOT( b_clicked() ) );
      b->addInputPort( "Snap clicked", SLOT( snap_clicked() ) );
      b->addInputPort( "Turn Left", SLOT( turnLeft_clicked() ) );
      b->addInputPort( "Turn Right", SLOT( turnRight_clicked() ) );

      b->addOutputPort( "Plan", SIGNAL( planChanged( QVector<QSharedPointer<PathPrimitive>> ) ) );

      return b;
    }

  private:
    Qt3DCore::QEntity* rootEntity = nullptr;
    TransverseMercatorWrapper* tmw = nullptr;
};

#endif // GLOBALPLANNER_H


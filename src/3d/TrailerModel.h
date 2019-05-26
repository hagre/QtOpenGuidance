
#ifndef TRAILERENTITY_H
#define TRAILERENTITY_H

#include <QtCore/QObject>

#include <Qt3DCore/qentity.h>
#include <Qt3DCore/qtransform.h>

#include <Qt3DRender/QGeometry>
#include <Qt3DRender/QGeometryRenderer>

#include <Qt3DExtras/QTorusMesh>
#include <Qt3DExtras/QConeMesh>
#include <Qt3DExtras/QCylinderMesh>
#include <Qt3DExtras/QCuboidMesh>
#include <Qt3DExtras/QPlaneMesh>
#include <Qt3DExtras/QSphereMesh>
#include <Qt3DExtras/QPhongMaterial>

#include "../GuidanceBase.h"

class TrailerModel : public GuidanceBase {
    Q_OBJECT

  public:
    explicit TrailerModel( Qt3DCore::QEntity* rootEntity );
    ~TrailerModel();

    QVector3D position();
    QQuaternion rotation();

  public slots:
    void setPoseHookPoint( QVector3D, QQuaternion );
    void setPoseTowPoint( QVector3D, QQuaternion );
    void setPosePivotPoint( QVector3D, QQuaternion );

    void setOffsetHookPointPosition( QVector3D position );

  signals:
    void positionChanged( QVector3D position );
    void rotationChanged( QQuaternion rotation );

  private:
    Qt3DCore::QEntity* m_rootEntity;

    Qt3DCore::QEntity* m_hitchEntity;
    Qt3DCore::QEntity* m_axleEntity;
    Qt3DCore::QEntity* m_wheelLeftEntity;
    Qt3DCore::QEntity* m_wheelRightEntity;

    Qt3DExtras::QCylinderMesh* m_hitchMesh;
    Qt3DExtras::QCylinderMesh* m_axleMesh;
    Qt3DExtras::QCylinderMesh* m_wheelMesh;

    Qt3DCore::QTransform* m_rootEntityTransform;
    Qt3DCore::QTransform* m_hitchTransform;
    Qt3DCore::QTransform* m_axleTransform;
    Qt3DCore::QTransform* m_wheelLeftTransform;
    Qt3DCore::QTransform* m_wheelRightTransform;


    Qt3DCore::QEntity* m_towHookEntity;
    Qt3DCore::QEntity* m_pivotPointEntity;
    Qt3DCore::QEntity* m_towPointEntity;

    Qt3DExtras::QSphereMesh*  m_towHookMesh;
    Qt3DExtras::QSphereMesh* m_pivotPointMesh;
    Qt3DExtras::QSphereMesh* m_towPointMesh;

    Qt3DCore::QTransform* m_towHookTransform;
    Qt3DCore::QTransform* m_pivotPointTransform;
    Qt3DCore::QTransform* m_towPointTransform;

    QVector3D m_offsetHookPoint;
    float m_wheelbase;

    void setProportions();
};

class TrailerModelFactory : public GuidanceFactory {
    Q_OBJECT

  public:
    TrailerModelFactory( Qt3DCore::QEntity* rootEntity )
      : GuidanceFactory(),
        rootEntity( rootEntity ) {}
    ~TrailerModelFactory() {}

    virtual void addToCombobox( QComboBox* combobox ) override {
      combobox->addItem( QStringLiteral( "Trailer Model" ), QVariant::fromValue( this ) );
    }

    virtual GuidanceBase* createNewObject() override {
      return new TrailerModel( rootEntity );
    }

    virtual void createBlock( QGraphicsScene* scene, GuidanceBase* obj ) override {
      QNEBlock* b = new QNEBlock( obj );
      scene->addItem( b );

      b->addPort( "Trailer", "", 0, QNEPort::NamePort );
      b->addPort( "Trailer Model", "", 0, QNEPort::TypePort );

      b->addInputPort( "Pose Hook Point", SLOT( setPoseHookPoint( QVector3D, QQuaternion ) ) );
      b->addInputPort( "Pose Pivot Point", SLOT( setPosePivotPoint( QVector3D, QQuaternion ) ) );
      b->addInputPort( "Pose Tow Point", SLOT( setPoseTowPoint( QVector3D, QQuaternion ) ) );
    }

  private:
    Qt3DCore::QEntity* rootEntity;
};

#endif // TRAILERENTITY_H

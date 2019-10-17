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

#ifndef SETTINGSDIALOG_H
#define SETTINGSDIALOG_H

#include <QObject>

#include <QDialog>
#include <QVector3D>
#include <Qt3DCore/QEntity>

#include <QGraphicsScene>
#include <QSortFilterProxyModel>

#include "../block/PoseSynchroniser.h"

#include "VectorBlockModel.h"
#include "NumberBlockModel.h"
#include "StringBlockModel.h"
#include "ImplementBlockModel.h"
#include "ImplementSectionModel.h"

#include "../kinematic/Tile.h"

namespace Ui {
  class SettingsDialog;
}

class SettingsDialog : public QDialog {
    Q_OBJECT

  public:
    explicit SettingsDialog( Qt3DCore::QEntity* rootEntity, QWidget* parent = nullptr );
    ~SettingsDialog();

    QGraphicsScene* getSceneOfConfigGraphicsView();

    void emitAllConfigSignals();

  signals:
    void setGrid( bool );
    void setGridValues( float, float, float, float, float, float, float, QColor, QColor );
    void plannerSettingsChanged( int, int );

  public slots:
    void toggleVisibility();

    void loadConfigOnStart();
    void saveConfigOnExit();

    void loadDefaultConfig();
    void saveDefaultConfig();

    void allModelsReset();

  private slots:
    void on_cbValues_currentIndexChanged( int index );
    void on_pbSetStringToFilename_clicked();

    void on_pbAddBlock_clicked();
    void on_pbSaveSelected_clicked();
    void on_pbLoad_clicked();

    void on_pbZoomOut_clicked();
    void on_pbZoomIn_clicked();
    void on_pbDeleteSelected_clicked();


    void on_cbGridVisible_stateChanged( int arg1 );
    void on_dsbGridXStep_valueChanged( double arg1 );
    void on_dsbGridYStep_valueChanged( double arg1 );
    void on_dsbGridSize_valueChanged( double arg1 );
    void on_dsbGridXStepCoarse_valueChanged( double arg1 );
    void on_dsbGridYStepCoarse_valueChanged( double arg1 );
    void on_dsbGridCameraThreshold_valueChanged( double arg1 );
    void on_dsbGridCameraThresholdCoarse_valueChanged( double arg1 );
    void on_pbColor_clicked();
    void on_pbColorCoarse_clicked();

    void on_cbSaveConfigOnExit_stateChanged( int arg1 );
    void on_cbLoadConfigOnStart_stateChanged( int arg1 );
    void on_cbOpenSettingsDialogOnStart_stateChanged( int arg1 );
    void on_cbShowCameraToolbarOnStart_stateChanged( int arg1 );
    void on_cbRunSimulatorOnStart_stateChanged( int arg1 );

    void on_pbSaveAsDefault_clicked();
    void on_pbLoadSavedConfig_clicked();
    void on_pbDeleteSettings_clicked();

    void on_pbComPortRefresh_clicked();
    void on_pbBaudrateRefresh_clicked();

    void on_pbComPortSet_clicked();
    void on_pbBaudrateSet_clicked();

    void on_pbClear_clicked();

    void on_pbTileColor_clicked();
    void on_gbShowTiles_toggled( bool enabled );

    void on_cbImplements_currentIndexChanged( int index );

    void implementModelReset();

    void on_btnSectionAdd_clicked();

    void on_btnSectionRemove_clicked();

    void on_pbSetSelectedCellsToNumber_clicked();

    void on_btnSectionMoveUp_clicked();

    void on_btnSectionMoveDown_clicked();

    void on_cbGlobalPlanner_stateChanged( int arg1 );
    void on_dsbGlobalPlannerVisibleAreaX_valueChanged( double arg1 );
    void on_dsbGlobalPlannerVisibleAreaY_valueChanged( double arg1 );
    void on_dsbGlobalPlannerArrowSize_valueChanged( double arg1 );
    void on_dsbGlobalPlannerArrowDistance_valueChanged( double arg1 );
    void on_pbGlobalPlannerArrowColor_clicked();
    void on_pbGlobalPlannerBackgroundColor_clicked();
    void on_slGlobalPlannerTransparency_valueChanged( int value );

    void on_cbLocalPlannerVisible_stateChanged( int arg1 );
    void on_dsbLocalPlannerArrowSize_valueChanged( double arg1 );
    void on_dsbLocalPlannerLineWidth_valueChanged( double arg1 );
    void on_pbLocalPlannerArrowColor_clicked();
    void on_pbLocalPlannerLineColor_clicked();
    void on_slLocalPlannerTransparency_valueChanged( int value );

    void on_sbPathsToGenerate_valueChanged( int arg1 );

    void on_sbPathsInReserve_valueChanged( int arg1 );

  private:
    void saveGridValuesInSettings();
    void saveTileValuesInSettings();
    void savePlannerValuesInSettings();
    void savePathPlannerValuesInSettings();

    void setPlannerColorLabels();

    void saveConfigToFile( QFile& file );
    void loadConfigFromFile( QFile& file );

  public:
    QComboBox* getCbNodeType();

    RootTile tileRoot;

  public:
    BlockBase* poseSimulation = nullptr;

    BlockBase* plannerGui = nullptr;
    BlockBase* globalPlanner = nullptr;
    BlockBase* localPlanner = nullptr;
    BlockBase* stanleyGuidance = nullptr;
    BlockBase* xteGuidance = nullptr;

  private:
    Ui::SettingsDialog* ui = nullptr;

//    Qt3DCore::QEntity* rootEntity = nullptr;

    BlockFactory* poseSimulationFactory = nullptr;

    BlockFactory* transverseMercatorConverterFactory = nullptr;
    BlockFactory* poseSynchroniserFactory = nullptr;
    BlockFactory* tractorModelFactory = nullptr;
    BlockFactory* trailerModelFactory = nullptr;
    BlockFactory* fixedKinematicFactory = nullptr;
    BlockFactory* trailerKinematicFactory = nullptr;
    BlockFactory* debugSinkFactory = nullptr;
    BlockFactory* printLatencyFactory = nullptr;
    BlockFactory* udpSocketFactory = nullptr;

#ifdef SERIALPORT_ENABLED
    BlockFactory* serialPortFactory = nullptr;
#endif

    BlockFactory* fileStreamFactory = nullptr;
    BlockFactory* ackermannSteeringFactory = nullptr;
    BlockFactory* nmeaParserFactory = nullptr;
    BlockFactory* communicationPgn7ffeFactory = nullptr;
    BlockFactory* communicationJrkFactory = nullptr;

    BlockFactory* plannerGuiFactory = nullptr;
    BlockFactory* globalPlannerFactory = nullptr;
    BlockFactory* localPlannerFactory = nullptr;
    BlockFactory* stanleyGuidanceFactory = nullptr;
    BlockFactory* xteGuidanceFactory = nullptr;

    BlockFactory* vectorFactory = nullptr;
    BlockFactory* numberFactory = nullptr;
    BlockFactory* stringFactory = nullptr;
    QSortFilterProxyModel* filterModelValues = nullptr;
    VectorBlockModel* vectorBlockModel = nullptr;
    NumberBlockModel* numberBlockModel = nullptr;
    StringBlockModel* stringBlockModel = nullptr;

    BlockFactory* implementFactory = nullptr;
    QSortFilterProxyModel* filterModelImplements = nullptr;
    ImplementBlockModel* implementBlockModel = nullptr;
    ImplementSectionModel* implementSectionModel = nullptr;

  private:
    QNEBlock* getBlockWithId( int id );
    QNEBlock* getBlockWithName( const QString& name );

    bool blockSettingsSaving = false;

    QColor gridColor = QColor( 0x6b, 0x96, 0xa8 );
    QColor gridColorCoarse = QColor( 0xa2, 0xe3, 0xff );
    QColor tileColor = QColor( 0xff, 0x00, 0x00 );

    QColor globalPlannerArrowColor = QColor( 0xff, 0xff, 0 );
    QColor globalPlannerBackgroundColor = QColor( 0xf5, 0x9f, 0xbd );
    QColor localPlannerArrowColor = QColor( 0x90, 0x90, 0 );
    QColor localPlannerLineColor = QColor( 0x9a, 0x64, 0x77 );
};

#endif // SETTINGSDIALOG_H

// Copyright( C ) 2020 Christian Riggenbach
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

#pragma once

#include <QObject>

#include <QQuaternion>
#include <QVector3D>

#include <QDateTime>

#include "BlockBase.h"

#include "qneblock.h"
#include "qneport.h"

#include <QElapsedTimer>

#include <QDebug>

class PrintLatency : public BlockBase {
    Q_OBJECT

  public:
    explicit PrintLatency()
      : BlockBase(),
        block( nullptr ) {}

  public slots:
    void input1() {
      timer.start();
    }

    void input2() {
      if( timer.isValid() ) {
        qDebug() << block->getName() << timer.nsecsElapsed() << "ns";
        timer.invalidate();
      }
    }

  public:
    virtual void emitConfigSignals() override {
    }

  public:
    QNEBlock* block = nullptr;

  private:
    QElapsedTimer timer;
};

class PrintLatencyFactory : public BlockFactory {
    Q_OBJECT

  public:
    PrintLatencyFactory()
      : BlockFactory() {}

    QString getNameOfFactory() override {
      return QStringLiteral( "Print Latency" );
    }

    virtual QNEBlock* createBlock( QGraphicsScene* scene, int id ) override {
      auto* obj = new PrintLatency;
      auto* b = createBaseBlock( scene, obj, id );

      auto printLatency = qobject_cast<PrintLatency*>( obj );

      if( printLatency ) {
        printLatency->block = b;
      }

      b->addInputPort( QStringLiteral( "Input1" ), QLatin1String( SLOT( input1() ) ) );
      b->addInputPort( QStringLiteral( "Input2" ), QLatin1String( SLOT( input2() ) ) );

      return b;
    }
};

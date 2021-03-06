/* Copyright (c) 2012, STANISLAW ADASZEWSKI
 * Copyright (c) 2020, Christian Riggenbach
 * All rights reserved.
 *
 * Redistribution and use in source and binary forms, with or without
 * modification, are permitted provided that the following conditions are met:
 *     * Redistributions of source code must retain the above copyright
 *       notice, this list of conditions and the following disclaimer.
 *     * Redistributions in binary form must reproduce the above copyright
 *       notice, this list of conditions and the following disclaimer in the
 *       documentation and/or other materials provided with the distribution.
 *     * Neither the name of STANISLAW ADASZEWSKI nor the
 *       names of its contributors may be used to endorse or promote products
 *       derived from this software without specific prior written permission.
 *
 * THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS "AS IS" AND
 * ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE IMPLIED
 * WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE ARE
 * DISCLAIMED. IN NO EVENT SHALL THE AUTHORS BE LIABLE FOR ANY
 * DIRECT, INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR CONSEQUENTIAL DAMAGES
 * (INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS OR SERVICES;
 * LOSS OF USE, DATA, OR PROFITS; OR BUSINESS INTERRUPTION) HOWEVER CAUSED AND
 * ON ANY THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT LIABILITY, OR TORT
 * (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT OF THE USE OF THIS
 * SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.
 */

#include "qnodeseditor.h"

#include <QObject>

#include <QGraphicsView>
#include <QGraphicsScene>
#include <QEvent>
#include <QGraphicsSceneMouseEvent>
#include <QKeyEvent>
#include <QScrollBar>

#include <QDebug>

#include "qneport.h"
#include "qneconnection.h"
#include "qneblock.h"
#include "qnegestures.h"

#include "../gui/SettingsDialog.h"

#include "../block/BlockBase.h"

QNodesEditor::QNodesEditor( QObject* parent ) :
  QObject( parent ) {
}

void QNodesEditor::install( QGraphicsScene* s ) {
  s->installEventFilter( this );
  scene = s;

  const auto& constRefOfList = scene->views();

  for( const auto& it : constRefOfList ) {
    new QNEGestureEventFilter( it );
    it->grabGesture( Qt::PinchGesture );
  }
}

QGraphicsItem* QNodesEditor::itemAt( QPointF pos ) {
  QList<QGraphicsItem*> items = scene->items( QRectF( pos - QPointF( 1, 1 ), QSize( 3, 3 ) ) );

  for( auto item :  qAsConst( items ) ) {
    if( item->type() > QGraphicsItem::UserType ) {
      return item;
    }
  }

  return nullptr;
}

bool QNodesEditor::eventFilter( QObject* o, QEvent* e ) {
  const auto mouseEvent = static_cast< QGraphicsSceneMouseEvent*>( e );
  const auto keyEvent = static_cast<QKeyEvent*>( e );

  switch( int( e->type() ) ) {
    case QEvent::GraphicsSceneMousePress: {

        switch( int( mouseEvent->button() ) ) {
          case Qt::LeftButton: {
              const auto item = qgraphicsitem_cast<QNEPort*>( itemAt( mouseEvent->scenePos() ) );

              if( item != nullptr ) {
                const auto& constRefOfList = scene->views();

                for( const auto& it : constRefOfList ) {
                  it->setDragMode( QGraphicsView::NoDrag );
                }

                currentConnection = new QNEConnection( nullptr );
                scene->addItem( currentConnection );
                currentConnection->setPort1( item );
                currentConnection->setPos1( item->scenePos() );
                currentConnection->setPos2( mouseEvent->scenePos() );
                currentConnection->updatePath();

                return true;
              }

            }
            break;

          case Qt::RightButton: {
              isInPaningState = false;
            }
            break;
        }

        break;

      case QEvent::KeyRelease: {

          if( keyEvent->matches( QKeySequence::Delete ) ) {
            {
              const auto& constRefOfList = scene->selectedItems();

              for( const auto& item : constRefOfList ) {
                delete qgraphicsitem_cast<QNEConnection*>( item );
              }
            }

            {
              const auto& constRefOfList = scene->selectedItems();
              SettingsDialog* settingsDialog = nullptr;

              for( const auto& item : constRefOfList ) {
                const auto block = qgraphicsitem_cast<QNEBlock*>( item );

                if( block != nullptr ) {
                  if( !block->systemBlock ) {
                    delete block;

                    const auto dialog = qobject_cast<SettingsDialog*>( parent() );

                    if( dialog != nullptr ) {
                      settingsDialog = dialog;
                    }
                  }
                }
              }

              if( settingsDialog ) {
                settingsDialog->allModelsReset();
              }
            }
          }
        }
        break;
      }
      break;


    case QEvent::GraphicsSceneMouseMove: {
        const auto m = static_cast<QGraphicsSceneMouseEvent*>( e );

        if( currentConnection != nullptr ) {
          currentConnection->setPos2( mouseEvent->scenePos() );
          currentConnection->updatePath();
          return true;
        }

        if( m->buttons() & Qt::RightButton ) {
          QPointF delta = m->lastScreenPos() - m->screenPos();

          {
            const auto& constRefOfList = scene->views();

            for( const auto& view : constRefOfList ) {
              double newX = view->horizontalScrollBar()->value() + delta.x();
              double newY = view->verticalScrollBar()->value() + delta.y();
              view->horizontalScrollBar()->setValue( int( newX ) );
              view->verticalScrollBar()->setValue( int( newY ) );
            }
          }

          {
            const auto& constRefOfList = scene->views();

            for( const auto& view : constRefOfList ) {
              view->setDragMode( QGraphicsView::ScrollHandDrag );
            }
          }

          isInPaningState = true;
          return true;
        }


        break;
      }

    case QEvent::GraphicsSceneMouseRelease: {
        if( ( currentConnection != nullptr ) && mouseEvent->button() == Qt::LeftButton ) {
          auto* port = qgraphicsitem_cast<QNEPort*>( itemAt( mouseEvent->scenePos() ) );

          if( ( port != nullptr ) && port != currentConnection->port1() ) {
            if( currentConnection->setPort2( port ) ) {
              currentConnection->updatePosFromPorts();
              currentConnection->updatePath();
              auto* block = qobject_cast<BlockBase*> ( currentConnection->port1()->block()->object );

              if( block != nullptr ) {
                block->emitConfigSignals();
              }

              currentConnection = nullptr;
              return true;
            }

            QNEPort* port1 = currentConnection->port1();
            currentConnection->setPort1( port );

            if( currentConnection->setPort2( port1 ) ) {
              currentConnection->updatePosFromPorts();
              currentConnection->updatePath();
              auto* block = qobject_cast<BlockBase*> ( currentConnection->port1()->block()->object );

              if( block != nullptr ) {
                block->emitConfigSignals();
              }

              currentConnection = nullptr;
              return true;
            }

          }

          delete currentConnection;
          currentConnection = nullptr;
          return true;
        }

        if( mouseEvent->button() == Qt::RightButton ) {
          if( !isInPaningState )  {
            QGraphicsItem* item = itemAt( mouseEvent->scenePos() );

            if( item != nullptr ) {
              auto* block = qgraphicsitem_cast<QNEBlock*>( item );

              if( block != nullptr ) {
                if( !block->systemBlock ) {
                  delete block;

                  auto* dialog = qobject_cast<SettingsDialog*>( parent() );

                  if( dialog != nullptr ) {
                    dialog->allModelsReset();
                  }
                }
              } else {
                auto* connection = qgraphicsitem_cast<QNEConnection*>( item );

                if( connection != nullptr ) {
                  delete connection;
                }
              }
            }
          }

          const auto& constRefOfList = scene->views();

          for( const auto& it : constRefOfList ) {
            it->setDragMode( QGraphicsView::RubberBandDrag );
          }
        }


        break;
      }
  }

  return QObject::eventFilter( o, e );
}

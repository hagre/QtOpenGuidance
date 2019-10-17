/* Copyright (c) 2012, STANISLAW ADASZEWSKI
 * Copyright (c) 2019, Christian Riggenbach
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
 * DISCLAIMED. IN NO EVENT SHALL STANISLAW ADASZEWSKI BE LIABLE FOR ANY
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

#include "../gui/SettingsDialog.h"

#include "../block/BlockBase.h"

QNodesEditor::QNodesEditor( QObject* parent ) :
  QObject( parent ) {
}

void QNodesEditor::install( QGraphicsScene* s ) {
  s->installEventFilter( this );
  scene = s;
}

QGraphicsItem* QNodesEditor::itemAt( const QPointF& pos ) {
  QList<QGraphicsItem*> items = scene->items( QRectF( pos - QPointF( 1, 1 ), QSize( 3, 3 ) ) );

  foreach( QGraphicsItem* item, items ) {
    if( item->type() > QGraphicsItem::UserType ) {
      return item;
    }
  }

  return nullptr;
}

bool QNodesEditor::eventFilter( QObject* o, QEvent* e ) {
  QGraphicsSceneMouseEvent* mouseEvent = ( QGraphicsSceneMouseEvent* ) e;
  QKeyEvent* keyEvent = ( QKeyEvent* ) e;

  switch( ( int ) e->type() ) {
    case QEvent::GraphicsSceneMousePress: {

        switch( ( int ) mouseEvent->button() ) {
          case Qt::LeftButton: {
              QNEPort* item = qgraphicsitem_cast<QNEPort*>( itemAt( mouseEvent->scenePos() ) );

              if( item ) {
                foreach( QGraphicsView* it, scene->views() ) {
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
            foreach( QGraphicsItem* item, scene->selectedItems() ) {
              QNEConnection* connection = qgraphicsitem_cast<QNEConnection*>( item );

              if( connection != nullptr ) {
                delete connection;
              }
            }

            foreach( QGraphicsItem* item, scene->selectedItems() ) {
              QNEBlock* block = qgraphicsitem_cast<QNEBlock*>( item );

              if( block != nullptr ) {
                if( !block->systemBlock ) {
                  delete block;

                  SettingsDialog* dialog = qobject_cast<SettingsDialog*>( parent() );

                  if( dialog ) {
                    dialog->allModelsReset();
                  }
                }
              }
            }
          }
        }
        break;
      }
      break;


    case QEvent::GraphicsSceneMouseMove: {
        QGraphicsSceneMouseEvent* m = static_cast<QGraphicsSceneMouseEvent*>( e );

        if( currentConnection ) {
          currentConnection->setPos2( mouseEvent->scenePos() );
          currentConnection->updatePath();
          return true;
        } else {
          if( m->buttons() & Qt::RightButton ) {
            QPointF delta = m->lastScreenPos() - m->screenPos();

            foreach( QGraphicsView* view, scene->views() ) {
              double newX = view->horizontalScrollBar()->value() + delta.x();
              double newY = view->verticalScrollBar()->value() + delta.y();
              view->horizontalScrollBar()->setValue( int( newX ) );
              view->verticalScrollBar()->setValue( int( newY ) );
            }

            foreach( QGraphicsView* it, scene->views() ) {
              it->setDragMode( QGraphicsView::ScrollHandDrag );
            }

            isInPaningState = true;
            return true;
          }
        }

        break;
      }

    case QEvent::GraphicsSceneMouseRelease: {
        if( currentConnection && mouseEvent->button() == Qt::LeftButton ) {
          QNEPort* port = qgraphicsitem_cast<QNEPort*>( itemAt( mouseEvent->scenePos() ) );

          if( port && port != currentConnection->port1() ) {
            if( currentConnection->setPort2( port ) ) {
              currentConnection->updatePosFromPorts();
              currentConnection->updatePath();
              BlockBase* block = qobject_cast<BlockBase*> ( currentConnection->port1()->block()->object );

              if( block ) {
                block->emitConfigSignals();
              }

              currentConnection = nullptr;
              return true;
            } else {
              QNEPort* port1 = currentConnection->port1();
              currentConnection->setPort1( port );

              if( currentConnection->setPort2( port1 ) ) {
                currentConnection->updatePosFromPorts();
                currentConnection->updatePath();
                BlockBase* block = qobject_cast<BlockBase*> ( currentConnection->port1()->block()->object );

                if( block ) {
                  block->emitConfigSignals();
                }

                currentConnection = nullptr;
                return true;
              }
            }
          }

          delete currentConnection;
          currentConnection = nullptr;
          return true;
        }

        if( mouseEvent->button() == Qt::RightButton ) {
          if( isInPaningState == false )  {
            QGraphicsItem* item = itemAt( mouseEvent->scenePos() );

            if( item ) {
              QNEBlock* block = qgraphicsitem_cast<QNEBlock*>( item );

              if( block ) {
                if( !block->systemBlock ) {
                  delete block;

                  SettingsDialog* dialog = qobject_cast<SettingsDialog*>( parent() );

                  if( dialog ) {
                    dialog->allModelsReset();
                  }
                }
              } else {
                QNEConnection* connection = qgraphicsitem_cast<QNEConnection*>( item );

                if( connection ) {
                  delete connection;
                }
              }
            }
          }

          foreach( QGraphicsView* it, scene->views() ) {
            it->setDragMode( QGraphicsView::RubberBandDrag );
          }
        }


        break;
      }
  }

  return QObject::eventFilter( o, e );
}

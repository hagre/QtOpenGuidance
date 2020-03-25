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

#include "../block/NumberObject.h"

#include "SliderBlockModel.h"

#include <QGraphicsItem>
#include <QGraphicsScene>

#include "../qnodeseditor/qneblock.h"

#include "../block/SliderDockBlock.h"

SliderBlockModel::SliderBlockModel( QGraphicsScene* scene )
  : scene( scene ) {
}

int SliderBlockModel::columnCount( const QModelIndex& /*parent*/ ) const {
  return 9;
}

QVariant SliderBlockModel::headerData( int section, Qt::Orientation orientation, int role ) const {
  if( role == Qt::DisplayRole && orientation == Qt::Orientation::Horizontal ) {
    switch( section ) {
      case 0:
        return QStringLiteral( "Name" );
        break;

      case 1:
        return QStringLiteral( "Value" );
        break;

      case 2:
        return QStringLiteral( "Decimals" );
        break;

      case 3:
        return QStringLiteral( "Maximum" );
        break;

      case 4:
        return QStringLiteral( "Minimum" );
        break;

      case 5:
        return QStringLiteral( "DefaultValue" );
        break;

      case 6:
        return QStringLiteral( "Unit" );
        break;

      case 7:
        return QStringLiteral( "Reset on Start?" );
        break;

      case 8:
        return QStringLiteral( "Inverted?" );
        break;

      default:
        return QString();
        break;
    }
  }

  return QVariant();
}

bool SliderBlockModel::setHeaderData( int section, Qt::Orientation orientation, const QVariant& value, int role ) {
  if( value != headerData( section, orientation, role ) ) {
    emit headerDataChanged( orientation, section, section );
    return true;
  }

  return false;
}

int SliderBlockModel::rowCount( const QModelIndex& /*parent*/ ) const {
  return countBuffer;
}

QVariant SliderBlockModel::data( const QModelIndex& index, int role ) const {
  if( !index.isValid() || ( role != Qt::DisplayRole &&  role != Qt::EditRole &&  role != Qt::CheckStateRole ) ) {
    return QVariant();
  }

  if( role == Qt::CheckStateRole ) {
    int countRow = 0;

    const auto& constRefOfList = scene->items();

    for( const auto& item : constRefOfList ) {
      auto* block = qgraphicsitem_cast<QNEBlock*>( item );

      if( block != nullptr ) {
        if( auto* object = qobject_cast<SliderDockBlock*>( block->object ) ) {
          if( countRow++ == index.row() ) {
            switch( index.column() ) {
              case 7:
                return object->widget->resetOnStart ? Qt::Checked : Qt::Unchecked;

              case 8:
                return object->widget->getSliderInverted();
            }
          }
        }
      }
    }
  }

  if( role == Qt::DisplayRole || role == Qt::EditRole ) {

    int countRow = 0;

    const auto& constRefOfList = scene->items();

    for( const auto& item : constRefOfList ) {
      auto* block = qgraphicsitem_cast<QNEBlock*>( item );

      if( block != nullptr ) {
        if( auto* object = qobject_cast<SliderDockBlock*>( block->object ) ) {
          if( countRow++ == index.row() ) {
            switch( index.column() ) {
              case 0:
                return block->getName();

              case 1:
                return object->widget->getValue();

              case 2:
                return object->widget->getDecimals();

              case 3:
                return object->widget->getMaximum();

              case 4:
                return object->widget->getMinimum();

              case 5:
                return object->widget->getDefaultValue();

              case 6:
                return object->widget->getUnit();
            }
          }
        }
      }
    }
  }

  return QVariant();
}

bool SliderBlockModel::setData( const QModelIndex& index, const QVariant& value, int role ) {
  int countRow = 0;

  const auto& constRefOfList = scene->items();

  for( const auto& item : constRefOfList ) {
    auto* block = qgraphicsitem_cast<QNEBlock*>( item );

    if( block != nullptr ) {
      if( auto* object = qobject_cast<SliderDockBlock*>( block->object ) ) {
        if( countRow++ == index.row() ) {
          switch( index.column() ) {
            case 0:
              block->setName( qvariant_cast<QString>( value ) );
              emit dataChanged( index, index, QVector<int>() << role );
              return true;

            case 1:
              object->widget->setValue( value.toString().toDouble() );
              emit dataChanged( index, index, QVector<int>() << role );
              return true;

            case 2:
              object->widget->setDecimals( value.toString().toInt() );
              emit dataChanged( index, index, QVector<int>() << role );
              return true;

            case 3:
              object->widget->setMaximum( value.toString().toDouble() );
              emit dataChanged( index, index, QVector<int>() << role );
              return true;

            case 4:
              object->widget->setMinimum( value.toString().toDouble() );
              emit dataChanged( index, index, QVector<int>() << role );
              return true;

            case 5:
              object->widget->setDefaultValue( value.toString().toDouble() );
              emit dataChanged( index, index, QVector<int>() << role );
              return true;

            case 6:
              object->widget->setUnit( value.toString() );
              emit dataChanged( index, index, QVector<int>() << role );
              return true;

            case 7:
              object->widget->resetOnStart = value.toBool();
              emit dataChanged( index, index, QVector<int>() << role );
              return true;

            case 8:
              object->widget->setSliderInverted( value.toBool() );
              emit dataChanged( index, index, QVector<int>() << role );
              return true;
          }
        }
      }
    }
  }

  return false;
}

Qt::ItemFlags SliderBlockModel::flags( const QModelIndex& index ) const {
  if( !index.isValid() ) {
    return Qt::NoItemFlags;
  }

  if( index.column() == 7 || index.column() == 8 ) {
    return Qt::ItemIsEnabled | Qt::ItemIsSelectable | Qt::ItemIsUserCheckable;
  }

  return Qt::ItemIsEnabled | Qt::ItemIsSelectable | Qt::ItemIsEditable;
}

void SliderBlockModel::addToCombobox( QComboBox* combobox ) {
  combobox->addItem( QStringLiteral( "Slider Dock" ), QVariant::fromValue( this ) );
}

void SliderBlockModel::resetModel() {
  beginResetModel();
  countBuffer = 0;

  const auto& constRefOfList = scene->items();

  for( const auto& item : constRefOfList ) {
    auto* block = qgraphicsitem_cast<QNEBlock*>( item );

    if( block != nullptr ) {
      if( qobject_cast<SliderDockBlock*>( block->object ) != nullptr ) {
        ++countBuffer;
      }
    }
  }

  endResetModel();
}

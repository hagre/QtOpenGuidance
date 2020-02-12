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

#ifndef MYMAINWINDOW_H
#define MYMAINWINDOW_H

#include <QGuiApplication>
#include <QMainWindow>
#include <QSettings>
#include <QStandardPaths>
#include <QEvent>
#include <QWidget>
#include <QObject>
#include <QCloseEvent>

#include <kddockwidgets/MainWindow.h>

class MyMainWindow : public KDDockWidgets::MainWindow {
    Q_OBJECT

  public:
    explicit MyMainWindow( const QString& uniqueName, KDDockWidgets::MainWindowOptions options,
                           const QString& affinityName = {}, // Usually not needed. Just here to show the feature.
                           QWidget* parent = nullptr )
      : MainWindow( uniqueName, options, parent ) {
      setAffinityName( affinityName );
    }

  protected:
    void closeEvent( QCloseEvent* event ) override {
      emit closed();

      event->accept();
    }

  signals:
    void closed();
};

#endif // MYMAINWINDOW_H
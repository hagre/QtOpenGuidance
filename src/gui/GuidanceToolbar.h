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

#ifndef GUIDANCETOOLBAR_H
#define GUIDANCETOOLBAR_H

#include <QGroupBox>

namespace Ui {
  class GuidanceToolbar;
}

class GuidanceToolbar : public QGroupBox {
    Q_OBJECT

  public:
    explicit GuidanceToolbar( QWidget* parent = nullptr );
    ~GuidanceToolbar();

  private slots:
    void on_checkBox_stateChanged( int arg1 );

    void on_btn_settings_clicked();

    void on_cbCamera_stateChanged( int arg1 );

  signals:
    void simulatorChanged( bool );
    void toggleSettings();
    void cameraChanged( bool );

  private:
    Ui::GuidanceToolbar* ui;
};

#endif // GUIDANCETOOLBAR_H
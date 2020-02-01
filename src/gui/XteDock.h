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

#ifndef XTEDOCK_H
#define XTEDOCK_H

#include <QGroupBox>

namespace Ui {
  class XteDock;
}

class XteDock : public QGroupBox {
    Q_OBJECT

  public:
    explicit XteDock( QWidget* parent = nullptr );
    ~XteDock();

  public slots:
    void setXte( float xte );
    void setName( const QString& name );

  private:
    Ui::XteDock* ui = nullptr;
};

#endif // XTEDOCK_H
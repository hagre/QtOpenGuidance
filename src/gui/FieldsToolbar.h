#pragma once

#include <QGroupBox>
#include <QBoxLayout>
#include <QDockWidget>

namespace Ui {
  class FieldsToolbar;
}

class FieldsToolbar : public QGroupBox {
    Q_OBJECT

  public:
    explicit FieldsToolbar( QWidget* parent = nullptr );
    ~FieldsToolbar();

  signals:
    void openField();
    void newField();
    void saveField();

    void continousRecordToggled( bool );
    void recordPoint();

    void recordOnEdgeOfImplementChanged( bool right );

  public slots:
    void setDockLocation( Qt::DockWidgetArea area );

  private:
    Ui::FieldsToolbar* ui = nullptr;
    QBoxLayout* boxLayout = nullptr;
};


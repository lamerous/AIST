#pragma once

#include <QWidget>
#include "busstop.h"

namespace Ui {
class StopWidget;
}

class StopWidget : public QWidget {
    Q_OBJECT

public:
    explicit StopWidget(const BusStop& busstop, QWidget* parent = nullptr);
    ~StopWidget();

    void updateStopData(const BusStop& busstop);
    
    BusStop getStop() const;

signals:
    void delButtonClicked(const BusStop& busStop);

private slots:
    void on_delButton_clicked();

private:
    void setupUI();

private:
    Ui::StopWidget* ui;
    BusStop currentStop;
};
#pragma once

#include <QWidget>
#include "path.h"

namespace Ui {
class PathWidget;
}

class PathWidget : public QWidget {
    Q_OBJECT

public:
    explicit PathWidget(const Path& path, QWidget* parent = nullptr);
    ~PathWidget();

    void updatePathData(const Path& path);
    
    Path getPath() const;

signals:
    void editButtonClicked(Path& path);
    void delButtonClicked(const Path& path);

private slots:
    void on_editButton_clicked();
    void on_delButton_clicked();

private:
    void setupUI();

private:
    Ui::PathWidget* ui;
    Path currentPath;
};
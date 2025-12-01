#include "pathwidget.h"
#include "ui_path_widget.h"

#include <QLabel>
#include <QDebug>

PathWidget::PathWidget(const Path& path, QWidget* parent)
    : QWidget(parent)
    , ui(new Ui::PathWidget)
    , currentPath(path)
{
    ui->setupUi(this);
    setupUI();
    updatePathData(path);
}

PathWidget::~PathWidget() {
    delete ui;
}

void PathWidget::setupUI() {

}

void PathWidget::updatePathData(const Path& path) {
    currentPath = path;
    
    ui->pathNumber->setText(path.getNumber());
}

Path PathWidget::getPath() const {
    return currentPath;
}

void PathWidget::on_editButton_clicked() {
    emit editBittonClicked(currentPath);
}

void PathWidget::on_delButton_clicked() {
    emit delButtonClicked(currentPath);
    qDebug() << "Удалена остановка" << currentPath.getNumber();
}
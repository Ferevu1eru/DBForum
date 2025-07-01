#ifndef UTILGUI_H
#define UTILGUI_H
#include <QPushButton>
#include <QLineEdit>
#include <QTableWidgetItem>
#include <QTableWidget>
#include <QLabel>
#include <QWidget>
#include <QTextEdit>
#include <QComboBox>

namespace utilgui
{
    QPushButton *createButton(QWidget* parent, QString objectName, QString text, QRect geom, bool isOn=false, QString property="", QString propertyVal="");
    QLineEdit *createLineEdit(QWidget* parent, QString objectName, QRect geom, QString placeholder, QString property="", QString propertyVal="");
    QLabel *createLabel(QWidget* parent, QString objectName, QRect geom, QString text,  QString property="", QString propertyVal="");
    QTableWidget *createTableWidget(QWidget* parent, QString objectName, QRect geom, QString property="", QString propertyVal="");
    QTableWidgetItem *createTWItem(QString text, bool isEditable, bool isHeader);
    QTextEdit *createTextEdit(QWidget* parent, QString objectName, QString text, QRect geom, bool isEditable, QString property="", QString propertyVal="");
    QComboBox *createComboBox(QWidget* parent, QString objectName, QRect geom, QString property="", QString propertyVal="");
};

#endif // UTILGUI_H

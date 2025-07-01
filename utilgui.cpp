#include "utilgui.h"

// Создать кнопку
QPushButton *utilgui::createButton(QWidget *parent, QString objectName, QString text, QRect geom, bool isOn, QString property, QString propertyVal)
{
    QPushButton* temp = new QPushButton(parent);
    temp->setObjectName(objectName);
    temp->setGeometry(geom);
    temp->setEnabled(isOn);
    temp->setText(text);
    if (property!="")temp->setProperty(property.toUtf8(),propertyVal);
    temp->show();
    return temp;
}

// Создать строку
QLineEdit *utilgui::createLineEdit(QWidget *parent, QString objectName, QRect geom, QString placeholder, QString property, QString propertyVal)
{
    QLineEdit* temp = new QLineEdit(parent);
    temp->setObjectName(objectName);
    temp->setGeometry(geom);
    temp->setPlaceholderText(placeholder);
    if (property!="")temp->setProperty(property.toUtf8(),propertyVal);
    temp->show();
    return temp;
}

// Создать подпись
QLabel *utilgui::createLabel(QWidget *parent, QString objectName, QRect geom, QString text, QString property, QString propertyVal)
{
    QLabel* temp = new QLabel(parent);
    temp->setObjectName(objectName);
    temp->setGeometry(geom);
    temp->setText(text);
    if (property!="")temp->setProperty(property.toUtf8(),propertyVal);
    temp->show();
    return temp;
}

// Создать таблицу
QTableWidget *utilgui::createTableWidget(QWidget *parent, QString objectName, QRect geom, QString property, QString propertyVal)
{
    QTableWidget* temp = new QTableWidget(parent);
    temp->setObjectName(objectName);
    temp->setGeometry(geom);
    if (property!="")temp->setProperty(property.toUtf8(),propertyVal);
    temp->show();
    return temp;
}

// Создать ячейку
QTableWidgetItem *utilgui::createTWItem(QString text, bool isEditable, bool isHeader)
{
    QTableWidgetItem* temp = new QTableWidgetItem(text);
    temp->setTextAlignment(Qt::AlignCenter);
    if (!isEditable) temp->setFlags(temp->flags() ^ Qt::ItemIsEditable);
    if (isHeader)
    {
        QFont font = temp->font();
        font.setBold(true);
        temp->setFont(font);
    }
    return temp;
}

// Создать окно для текста
QTextEdit *utilgui::createTextEdit(QWidget *parent, QString objectName, QString text, QRect geom, bool isEditable, QString property, QString propertyVal)
{
    QTextEdit* temp = new QTextEdit(parent);
    temp->setObjectName(objectName);
    temp->setText(text);
    temp->setReadOnly(!isEditable);
//    temp->setEnabled(isEditable);
    temp->setGeometry(geom);
    if (property!="")temp->setProperty(property.toUtf8(),propertyVal);
    temp->show();
    return temp;
}

// Создать комбобокс
QComboBox *utilgui::createComboBox(QWidget *parent, QString objectName, QRect geom, QString property, QString propertyVal)
{
    QComboBox* temp = new QComboBox(parent);
    temp->setObjectName(objectName);
    temp->setGeometry(geom);
    temp->setEnabled(true);
    temp->show();
    if (property!="")temp->setProperty(property.toUtf8(),propertyVal);
    return temp;
}

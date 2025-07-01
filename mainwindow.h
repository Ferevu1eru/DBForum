#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <QMainWindow>
#include <utilgui.h>
#include <QDebug>
#include <QtSql/QSqlDatabase>
#include <QtSql/QSqlError>
#include <QtSql/QSqlQuery>
#include <QMessageBox>
#include <QDateTime>
#include <QComboBox>
#include <QSqlRecord>
#include <QHeaderView>
#include <QMessageBox>
#include <QDialog>

namespace Ui {
class MainWindow;
}

class MainWindow : public QMainWindow
{
    Q_OBJECT

public:
    explicit MainWindow(QWidget *parent = nullptr);
    ~MainWindow();

private slots:
    bool EstablishDBConnection();
    void Enter();
    void registration();
    void LogIn();
    void loadUserInt();
    void createPost();
    void viewMyPosts();
    void viewAllPosts();
    void userExit();
    void loadTable(QTableWidget* table, QString query);
    void adminCB(int);
    void showPost();
    void deletePost();
    void addTagToTableToShowPost();
    void deleteOneChosenTagInTableForSearchAllPostInDataBase();
    void showPostByTags();
    void showOnePostByTag();
    void deleteDataAdmin();
    void addDataAdmin();
    void writeData();
    void backAdmin();

private:
    Ui::MainWindow *ui;
    QSqlDatabase *db = nullptr;
    QSqlQuery qw;
    QString userId;
    QList<QString> tagsList;
};

#endif // MAINWINDOW_H

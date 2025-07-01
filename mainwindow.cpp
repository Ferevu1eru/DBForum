#include "mainwindow.h"
#include "ui_mainwindow.h"

MainWindow::MainWindow(QWidget *parent) : // Конструктор
    QMainWindow(parent),
    ui(new Ui::MainWindow)
{
    ui->setupUi(this);
    setWindowTitle("MonkeyForum");
    QPushButton *temp = utilgui::createButton(this,"login","Войти",QRect(10,10,150,50),true);
    connect(temp,SIGNAL(clicked()),this,SLOT(Enter()));
    EstablishDBConnection();
}

MainWindow::~MainWindow() // Дестурктор
{
    delete ui;
}

bool MainWindow::EstablishDBConnection() // Установить соединение с БД
{
    db = new QSqlDatabase(QSqlDatabase::addDatabase("QPSQL"));
    db->setDatabaseName("MonkeyForum");
    db->setHostName("127.0.0.1");
    db->setPort(5432);
    if (!db->open("postgres","changeme"))
    {
        QMessageBox::warning(this,"Error", db->lastError().text());
        exit(0);
    }
    qw = QSqlQuery(*db);
    return true;
}

void MainWindow::Enter() //Входим в аккаунт(джунгли)
{
    qDeleteAll(this->findChildren<QWidget*>(QRegExp(""),Qt::FindDirectChildrenOnly));
    utilgui::createLineEdit(this,"LElogin",QRect(10,10,150,50),"Введите логин");
    utilgui::createLineEdit(this,"LEpwd",QRect(10,70,150,50),"Введите пароль");
    this->findChild<QLineEdit*>("LEpwd")->setEchoMode(QLineEdit::EchoMode::Password);
    QPushButton *temp1 = utilgui::createButton(this,"PBlogin","Войти",QRect(10,130,150,50),true);
    connect(temp1,SIGNAL(clicked()),this,SLOT(LogIn()));
    QPushButton *temp2 = utilgui::createButton(this,"PBreg","Регистрация",QRect(10,190,150,50),true,"state","0");
    connect(temp2,SIGNAL(clicked()),this,SLOT(registration()));
}

void MainWindow::registration() //Регистрация
{
    if(sender()->property("state") == "0")
    { // Начать регистраицю
        this->findChild<QPushButton*>("PBlogin")->hide();
        this->findChild<QPushButton*>("PBreg")->setProperty("state","1");
        QPushButton *temp1 = utilgui::createButton(this,"PBlogin","Назад",QRect(10,130,150,50),true);
        connect(temp1,SIGNAL(clicked()),this, SLOT(Enter()));
        this->findChild<QPushButton*>("PBreg")->setText("Зарегистрироваться");
    }
    else
    {// Закончить регистрацию
        if (this->findChild<QLineEdit*>("LElogin")->text().trimmed().length()==0 ||
                this->findChild<QLineEdit*>("LEpwd")->text().trimmed().length()==0)
        {QMessageBox::warning(this,"Предупреждение", "Логин или пароль не могут быть пустыми");return;}
        qw.prepare(R"(INSERT INTO public."Users"(login, password) VALUES(:login, :password))");
        qw.bindValue(":login", this->findChild<QLineEdit*>("LElogin")->text());
        qw.bindValue(":password", this->findChild<QLineEdit*>("LEpwd")->text());

        if(qw.exec())
        {
            this->findChild<QPushButton*>("PBreg")->setProperty("state","0");
            QMessageBox::information(this,"Успешно","Аккаунт создан");
            qDeleteAll(this->findChildren<QWidget*>(QRegExp(""),Qt::FindDirectChildrenOnly));
            Enter();
        }
        else
            QMessageBox::warning(this,"",qw.lastError().text());
    }
}

void MainWindow::LogIn() //Слот для входа в аккаунт/админа
{
    QString login = this->findChild<QLineEdit*>("LElogin")->text(), password = this->findChild<QLineEdit*>("LEpwd")->text();
    if(login == "ADMIN" && password == "ADMIN")
    {
        qDeleteAll(this->findChildren<QWidget*>(QRegExp(""),Qt::FindDirectChildrenOnly));
        QPushButton* adminExit = utilgui::createButton(this,"adminExit","Выйти",QRect(10,10,100,30),true);
        connect(adminExit, &QPushButton::clicked, this, &MainWindow::userExit);
        QPushButton* adminAddData = utilgui::createButton(this,"adminAddData","Добавить",QRect(120,10,100,30),true);
        connect(adminAddData, &QPushButton::clicked, this, &MainWindow::addDataAdmin);
        QComboBox* tables = utilgui::createComboBox(this,"tables",QRect(10,50,150,30));
        tables->addItems({"Пользователи", "Посты", "Тэги", "Типы постов"});
        connect(tables, SIGNAL(currentIndexChanged(int)), this, SLOT(adminCB(int)));
        utilgui::createTableWidget(this, "adminData", QRect(10,100,550,400));
        adminAddData->hide();
        adminCB(0);
        return;
    }
    qw.exec(R"(SELECT password, id_user FROM public."Users" WHERE login = ')" + login+"'");
    qw.next();
    if(qw.size()>=1 && password == qw.value(0).toString())
    {
        qDeleteAll(this->findChildren<QWidget*>(QRegExp(""),Qt::FindDirectChildrenOnly));
        QMessageBox::information(this,"Успех","Успешно");
        userId = qw.value(1).toString();
        loadUserInt();
    }
    else QMessageBox::warning(this,"Ошибка","Неверный логин или пароль");
}

void MainWindow::loadUserInt() // Загрузка юзерского интерфейса
{
    qDeleteAll(this->findChildren<QWidget*>(QRegExp(""),Qt::FindDirectChildrenOnly));
    QPushButton* create = utilgui::createButton(this,"create","Создать пост",QRect(10,10,100,30),true);
    connect(create, &QPushButton::clicked, this, &MainWindow::createPost);
    QPushButton* view = utilgui::createButton(this,"view","Мои посты",QRect(10,40,100,30),true);
    connect(view, &QPushButton::clicked, this, &MainWindow::viewMyPosts);
    QPushButton* view2 = utilgui::createButton(this,"view","Все посты",QRect(10,70,100,30),true);
    connect(view2, &QPushButton::clicked, this, &MainWindow::viewAllPosts);
    QPushButton* userExit = utilgui::createButton(this,"userExit","Выйти",QRect(10,120,100,30),true);
    connect(userExit, &QPushButton::clicked, this, &MainWindow::userExit);
    tagsList.clear();
}

void MainWindow::createPost() // Создать пост
{
    qw.clear();
    if (sender()->property("state")!=1)
    {// Начать создание
        qDeleteAll(this->findChildren<QWidget*>(QRegExp(""),Qt::FindDirectChildrenOnly));
        utilgui::createTextEdit(this, "text","",QRect(10,10,400,400),true);
        QPushButton* finish = utilgui::createButton(this,"finish","Создать",QRect(10,420,100,30),true,"state","1");
        connect(finish, &QPushButton::clicked, this, &MainWindow::createPost);
        QPushButton* back= utilgui::createButton(this,"back","Назад",QRect(120,420,100,30),true);
        connect(back, &QPushButton::clicked, this, &MainWindow::loadUserInt);
        QLineEdit* tags = utilgui::createLineEdit(this, "tags", QRect(10,460,300,30),"Введите не более 10 тэгов через ','");
        tags->setMaxLength(100);
        QComboBox* types= utilgui::createComboBox(this, "types",QRect(10,500,150,30));
        qw.exec(R"(SELECT id_type, name FROM public."Types" ORDER BY id_type)");
        while (qw.next()) types->addItem(qw.value(1).toString(),qw.value(0).toString());
    }
    else
    {// Закончить создание
        if (this->findChild<QTextEdit*>("text")->toPlainText().trimmed().length()==0)
        {QMessageBox::information(this,"Уведомление","Пост не может быть пустым");return;}
        if (this->findChild<QLineEdit*>("tags")->text().split(",").count()>10)
        {QMessageBox::information(this,"Уведомление","Слишком много тэгов");return;}
        if (this->findChild<QComboBox*>("types")->currentText()=="")
        {QMessageBox::information(this,"Уведомление","Не выбран тип поста. Обратитесь к администратору, если доступных типов нет.");return;}
        QStringList tags4db = this->findChild<QLineEdit*>("tags")->text().split(",");
        for (int i = 0 ; i < tags4db.count(); i++) tags4db[i] = tags4db[i].trimmed().toUpper();
        tags4db = tags4db.toSet().toList();
        tags4db.removeAll(QString(""));
        QString type = this->findChild<QComboBox*>("types")->currentData().toString();
        QString text = this->findChild<QTextEdit*>("text")->toPlainText().trimmed();

        // Добавляем теги
        for (int i = 0; i < tags4db.count(); i++)
            qw.exec(R"(INSERT INTO public."Tags" (name) VALUES (')"+tags4db[i]+R"(') ON CONFLICT (name) DO UPDATE SET name = public."Tags".name)");

        // Добавляем пост
        qw.exec(R"(INSERT INTO public."Posts" (id_post_type, text) VALUES(')" +type+"', '"+text+"')");

        // Добавляем юзер+пост
        qw.exec(R"(SELECT MAX(id_post)FROM public."Posts";)");
        qw.next();
        QString postId = qw.value(0).toString();
        QDateTime date = QDateTime::currentDateTime();
        QString formattedDate = date.toString("dd.MM.yyyy");
        qw.exec(R"(INSERT INTO public."Users-Posts" (id_user, id_post, date_post) VALUES (')"+userId+"', '"+postId+"', '"+formattedDate+"');");

        // Добавляем пост + теги
        QStringList tagsIds;
        for (int i = 0; i < tags4db.count(); i++)
        {
            qw.exec(R"(SELECT id_tag FROM public."Tags" WHERE name = ')"+tags4db[i]+"';");
            qw.next();
            tagsIds.append(qw.value(0).toString());
        }
        for (int i = 0; i <tagsIds.count(); i++)
            qw.exec(R"(INSERT INTO public."Posts-Tags" (id_post, id_tag) VALUES (')"+postId+"', '"+tagsIds[i]+"');");

        QMessageBox::information(this,"Успех","Пост успешно создан.");
        loadUserInt();
    }
}

void MainWindow::viewMyPosts() // Просмотреть свои посты
{
    qDeleteAll(this->findChildren<QWidget*>(QRegExp(""),Qt::FindDirectChildrenOnly));
    QPushButton* back= utilgui::createButton(this,"back","Назад",QRect(10,10,100,30),true);
    connect(back, &QPushButton::clicked, this, &MainWindow::loadUserInt);
    QTableWidget* table = utilgui::createTableWidget(this,"table",QRect(10,40,502,400));

    QString query = (R"(SELECT p.id_post as "Номер поста в базе", STRING_AGG(t.name, ', ') as "Теги" FROM public."Posts" p JOIN public."Users-Posts" u ON u.id_post = p.id_post
                     JOIN public."Posts-Tags" pt ON pt.id_post = p.id_post
                     JOIN public."Tags" t ON pt.id_tag = t.id_tag
                     WHERE id_user = )" + userId + " GROUP BY p.id_post");
    loadTable(table,query);

    table->setColumnCount(table->columnCount()+2);
    table->setItem(0,table->columnCount()-2, utilgui::createTWItem("Действие с постом",false,true));
    table->setItem(0,table->columnCount()-1, utilgui::createTWItem("Удалить",false,true));
    for (int i=1;i<table->rowCount();i++)
    {
        QPushButton *temp = utilgui::createButton(table,"ShowPost","Просмотреть",QRect(0,0,0,0),true,"ID",table->item(i,0)->text());
        connect(temp,SIGNAL(clicked()),this,SLOT(showPost()));
        table->setCellWidget(i,table->columnCount()-2, temp);
        QPushButton *temp3 = utilgui::createButton(table,"DeletePost","Удалить",QRect(0,0,0,0),true,"ID",table->item(i,0)->text());
        connect(temp3,SIGNAL(clicked()),this,SLOT(deletePost()));
        table->setCellWidget(i,table->columnCount()-1, temp3);
    }
}

void MainWindow::viewAllPosts() // Слот для юзера, в котором смотрим все существующие посты
{
    qDeleteAll(this->findChildren<QWidget*>(QRegExp(""),Qt::FindDirectChildrenOnly));
    qw.exec(R"(SELECT name, id_tag FROM public."Tags")");
    QComboBox *cb = utilgui::createComboBox(this,"cbTags",QRect(10,10,150,30));
    while (qw.next()) cb->addItem(qw.value(0).toString(), qw.value(1).toString());
    QPushButton *cancel = utilgui::createButton(this,"Cancel","Назад",QRect(10,90,150,30),true);
    connect(cancel, &QPushButton::clicked, this, &MainWindow::loadUserInt);
    QPushButton *addTag = utilgui::createButton(this,"addTag","Добавить тег",QRect(10,50,150,30),true);
    connect(addTag, &QPushButton::clicked, this, &MainWindow::addTagToTableToShowPost);
    utilgui::createTableWidget(this,"tableTags",QRect(10,140,272,400));
    utilgui::createTableWidget(this,"tablePosts",QRect(310,140,402,400));
}

void MainWindow::addTagToTableToShowPost()// Добавляем в первую табличку тег, по которому будем искать посты
{
    QComboBox *cb = this->findChild<QComboBox*>("cbTags");
    QTableWidget *tb = this->findChild<QTableWidget*>("tableTags");
    tb->setColumnCount(2);
    tb->setRowCount(tb->rowCount()+1);
    tb->setItem(tb->rowCount()-1,0,utilgui::createTWItem(cb->currentText(),false,false));
    QPushButton *temp = utilgui::createButton(tb,"deleteTag","Удалить",QRect(0,0,0,0),true,"ID",QString::number(tb->rowCount()-1));
    connect(temp,SIGNAL(clicked()),this,SLOT(deleteOneChosenTagInTableForSearchAllPostInDataBase()));
    tb->setCellWidget(tb->rowCount()-1,1,temp);
    tagsList.append(cb->currentData().toString());
    cb->removeItem(cb->currentIndex());
    if(cb->count()==0) this->findChild<QPushButton*>("addTag")->setEnabled(false);
    showPostByTags();
}

void MainWindow::deleteOneChosenTagInTableForSearchAllPostInDataBase() // Удаляем из первой табличке тег, который используем для поиска
{
    int id = sender()->property("ID").toInt();
    this->findChild<QPushButton*>("addTag")->setEnabled(true);
    QTableWidget *tb = this->findChild<QTableWidget*>("tableTags");
    this->findChild<QComboBox*>("cbTags")->addItem(tb->item(id,0)->text(),tagsList[id]);
    tagsList.removeAt(id);
    tb->removeRow(id);
    for (int i=0;i<tb->rowCount();i++) tb->cellWidget(i,1)->setProperty("ID",i);
    showPostByTags();
}

void MainWindow::showPostByTags() // Динамически обновляем таблицу с поиском постов по тегам
{
    QString query = (R"(SELECT pt.id_post as "Номер поста в базе", u.login as "Name автора", t.name as "Тип поста" FROM public."Posts-Tags" pt JOIN public."Posts" p ON pt.id_post = p.id_post LEFT JOIN public."Users-Posts" pu ON pt.id_post =
                     pu.id_post JOIN public."Users" u ON u.id_user = pu.id_user JOIN public."Types" t ON p.id_post_type = t.id_type WHERE id_tag IN
                     ()" + tagsList.join(", ") + ") GROUP BY pt.id_post, u.login, t.name Order BY pt.id_post");

    qw.exec(query);
    QTableWidget *tb = this->findChild<QTableWidget*>("tablePosts");
    loadTable(tb,query);
    tb->setColumnCount(4);
    tb->setItem(0,3,utilgui::createTWItem("Действие",false,true));
    for (int i=1;i<tb->rowCount();i++) {
        QPushButton *temp = utilgui::createButton(tb,"showPost","Открыть",QRect(0,0,0,0),true,"IDpost",tb->item(i,0)->text());
        tb->setCellWidget(i,3,temp);
        connect(temp,SIGNAL(clicked()),this,SLOT(showOnePostByTag()));
    }
}

void MainWindow::showOnePostByTag() // Вызов диалога с данными о посте
{
    QString str = sender()->property("IDpost").toString();
    qw.exec(R"(SELECT text FROM public."Posts" WHERE id_post = ')"+ str +"'");
    qw.next();
    QDialog *dialog = new QDialog(this);
    utilgui::createTextEdit(dialog,"",qw.value(0).toString(), QRect(10,10,400,250),false);
    qw.exec(R"(SELECT t.name FROM public."Posts-Tags" pt JOIN public."Tags" t ON t.id_tag = pt.id_tag WHERE pt.id_post = ')" + str + "'");
    QLineEdit *le = utilgui::createLineEdit(dialog,"",QRect(10,270,400,50),"");
    QStringList list;
    while (qw.next()) list.append(qw.value(0).toString());
    le->setText(list.join(", "));
    le->setReadOnly(true);
    dialog->exec();
}

void MainWindow::deleteDataAdmin() // Слот обрабатывает удаление данных для админа
{
    QString deleteid = sender()->property("ID").toString();
    int index = this->findChild<QComboBox*>("tables")->currentIndex();
    switch (index)
    {
    case 0://User
    {
        qw.exec(R"(SELECT id_post FROM public."Users-Posts" WHERE id_user = )" + deleteid + "");
        if(qw.next())
        {
            QString postToDelete = qw.value(0).toString();
            db->transaction();
            qw.exec(R"(DELETE FROM public."Users-Posts" WHERE id_user = )" + deleteid);
            qw.exec(R"(DELETE FROM public."Posts-Tags" WHERE id_post = )"+ postToDelete);
            qw.exec(R"(DELETE FROM public."Posts" WHERE id_post = )" + postToDelete);
        }
        qw.exec(R"(DELETE FROM public."Users" WHERE id_user = )" + deleteid);
        if(db->commit())
        {
            QMessageBox::information(this,"Успех", "Успешно удалили пользователя под номером: " + deleteid);
            adminCB(0);
        }
        else QMessageBox::warning(this,"Не успех", db->lastError().text());
        break;
    }
    case 1://Post
    {
        db->transaction();
        qw.exec(R"(DELETE FROM public."Posts-Tags" WHERE id_post = )"+ deleteid);
        qw.exec(R"(DELETE FROM public."Users-Posts" WHERE id_post = )" + deleteid);
        qw.exec(R"(DELETE FROM public."Posts" WHERE id_post = )" + deleteid);
        if(db->commit())
        {
            QMessageBox::information(this,"Успех", "Успешно удалили пост под номером: " + deleteid);
            adminCB(1);
        }
        else QMessageBox::warning(this,"Не успех", db->lastError().text());
        break;
    }
    case 2://Tag
    {
        db->transaction();
        qw.exec(R"(DELETE FROM public."Posts-Tags" WHERE id_tag = )"+ deleteid);
        qw.exec(R"(DELETE FROM public."Tags" WHERE id_tag = )" + deleteid);
        if(db->commit())
        {
            QMessageBox::information(this,"Успех", "Успешно удалили тег под номером: " + deleteid);
            adminCB(2);
        }
        else QMessageBox::warning(this,"Не успех", db->lastError().text());
        break;
    }
    case 3://Type
    {
        qw.exec(R"(SELECT id_post FROM public."Posts" WHERE id_post_type = ')" + deleteid + "'");
        db->transaction();
        if(qw.next()){
            QString postToDelete = qw.value(0).toString();
            qw.exec(R"(DELETE FROM public."Users-Posts" WHERE id_post = )"+ postToDelete);
            qw.exec(R"(DELETE FROM public."Posts-Tags" WHERE id_post = )"+ postToDelete);
            qw.exec(R"(DELETE FROM public."Posts" WHERE id_post = )" + postToDelete);
        }
        qw.exec(R"(DELETE FROM public."Types" WHERE id_type = )" + deleteid);
        if(db->commit())
        {
            QMessageBox::information(this,"Успех", "Успешно удалили тип под номером: " + deleteid);
            adminCB(3);
        }
        else QMessageBox::warning(this,"Не успех", db->lastError().text());
        break;
    }
    }
}

void MainWindow::addDataAdmin() // Слот для кнопки добавления данных
{
    qDeleteAll(this->findChildren<QWidget*>(QRegExp(""),Qt::FindDirectChildrenOnly));
    utilgui::createLineEdit(this,"LENewData",QRect(10,10,150,50),"Новые данные");
    QPushButton *PB = utilgui::createButton(this,"PBaddData","Добавить",QRect(170,10,150,50),true);
    connect(PB, SIGNAL(clicked()),this,SLOT(writeData()));
    QPushButton *PB2 = utilgui::createButton(this,"Cancel","Назад",QRect(10,70,150,50),true);
    connect(PB2, SIGNAL(clicked()),this,SLOT(backAdmin()));
}

void MainWindow::writeData()//Записать данные в бд
{
    QString index = this->property("indexData").toString();
    QString data = this->findChild<QLineEdit*>("LENewData")->text().trimmed();
    if(data.size() == 0){
        backAdmin();
        return;
    }
    if(index == "2")//теги
        qw.prepare(R"(INSERT INTO public."Tags" (name) VALUES (')" + data + R"(') ON CONFLICT (name) DO UPDATE SET name = public."Tags".name)");
    else//Тип
        qw.prepare(R"(INSERT INTO public."Types"(name) VALUES(')" + data + "')");
    if(!qw.exec())
        QMessageBox::warning(this,"Неудача",qw.lastError().text());
    else
    {
        QMessageBox::information(this,"Удача","Получилось добавить данные");
        backAdmin();
    }
}

void MainWindow::backAdmin()//Слот для возвращения интерфейса в случае отказа добавлять данные
{
    qDeleteAll(this->findChildren<QWidget*>(QRegExp(""),Qt::FindDirectChildrenOnly));
    QPushButton* adminExit = utilgui::createButton(this,"adminExit","Выйти",QRect(10,10,100,30),true);
    connect(adminExit, &QPushButton::clicked, this, &MainWindow::userExit);
    QPushButton* adminAddData = utilgui::createButton(this,"adminAddData","Добавить",QRect(120,10,100,30),true);
    connect(adminAddData, &QPushButton::clicked, this, &MainWindow::addDataAdmin);
    this->setProperty("indexData","2");
    QComboBox* tables = utilgui::createComboBox(this,"tables",QRect(10,50,150,30));
    tables->addItems({"Пользователи", "Посты", "Тэги", "Типы постов"});
    connect(tables, SIGNAL(currentIndexChanged(int)), this, SLOT(adminCB(int)));
    utilgui::createTableWidget(this, "adminData", QRect(10,100,550,400));
    adminAddData->hide();
    adminCB(0);
}

void MainWindow::showPost() // Слот для отображение постов и/или обновления данных в них
{
    if(sender()->property("state").toString() != "1")
    {
        QString id = sender()->property("ID").toString();
        qDeleteAll(this->findChildren<QWidget*>(QRegExp(""),Qt::FindDirectChildrenOnly));
        QTextEdit*te = utilgui::createTextEdit(this, "text","",QRect(10,10,400,400),true);
        QPushButton* finish = utilgui::createButton(this,"Save","Сохранить",QRect(10,420,100,30),true,"state","1");
        finish->setProperty("post_id", id);
        connect(finish, &QPushButton::clicked, this, &MainWindow::showPost);
        QPushButton* back= utilgui::createButton(this,"back","Назад",QRect(120,420,100,30),true);
        connect(back, &QPushButton::clicked, this, &MainWindow::loadUserInt);
        QComboBox* types= utilgui::createComboBox(this, "types",QRect(10,500,150,30));
        qw.exec(R"(SELECT id_type, name FROM public."Types" Order BY id_type)");
        while (qw.next()) types->addItem(qw.value(1).toString(),qw.value(0).toString());
        qw.exec(R"(SELECT p.text, t.name FROM public."Posts" p JOIN public."Types" t ON p.id_post_type = t.id_type WHERE id_post = ')" + id +"'");
        qw.next();
        te->append(qw.value(0).toString());
        types->setCurrentText(qw.value(1).toString());
    }
    else
    {
        if (this->findChild<QTextEdit*>("text")->toPlainText().trimmed().length()==0)
        {QMessageBox::information(this,"Уведомление","Пост не может быть пустым");return;}
        if (this->findChild<QComboBox*>("types")->currentText()=="")
        {QMessageBox::information(this,"Уведомление","Не выбран тип поста. Обратитесь к администратору, если доступных типов нет.");return;}
        QString type = this->findChild<QComboBox*>("types")->currentData().toString();
        QString text = this->findChild<QTextEdit*>("text")->toPlainText().trimmed();

        //Обновляем пост
        qw.exec(R"(UPDATE public."Posts" SET text = ')"+ text +"', id_post_type = '"+ type +"' WHERE id_post = '" + sender()->property("post_id").toString() +"'");
        QMessageBox::information(this,"Успех","Пост успешно обновлен.");
        viewMyPosts();
    }
}

void MainWindow::deletePost() // Удалить пост
{
    QString id_post = sender()->property("ID").toString();
    if(QMessageBox::question(this,"Удалить","Удалить пост под номером: " + id_post,QMessageBox::Yes,QMessageBox::No) == QMessageBox::No) return;
    db->transaction();
    qw.exec(R"(DELETE FROM public."Users-Posts" WHERE id_post = ')" + id_post + "'");
    qw.exec(R"(DELETE FROM public."Posts-Tags" WHERE id_post = ')" + id_post + "'");
    qw.exec(R"(DELETE FROM public."Comments" WHERE id_post = ')" + id_post + "'");
    qw.exec(R"(DELETE FROM public."Posts" WHERE id_post = ')" + id_post + "'");
    if(!db->commit())
        QMessageBox::warning(this,"Ошибка","Не удалось удалить, так как:\n" + db->lastError().text(),QMessageBox::Ok);
    viewMyPosts();
}

void MainWindow::userExit() // Выход для юзера
{
    tagsList.clear();
    qDeleteAll(this->findChildren<QWidget*>(QRegExp(""),Qt::FindDirectChildrenOnly));
    userId.clear();
    Enter();
}

void MainWindow::loadTable(QTableWidget *table, QString query) // Заполнение таблицы данными из запроса
{
    qw = db->exec(query);
    table->clear();
    table->setRowCount(qw.size()+1);
    table->setColumnCount(qw.record().count());
    table->verticalHeader()->hide();
    table->horizontalHeader()->hide();
    for (int i = 0; i < qw.record().count(); i++)
    {
        table->setItem(0,i,utilgui::createTWItem(qw.record().fieldName(i), false, true));
        for (int j = 0; j < qw.size(); j++)
        {
            qw.seek(j);
            table->setItem(j+1,i,utilgui::createTWItem(qw.value(i).toString(), false, false));
        }
    }
}

void MainWindow::adminCB(int index) // Изменение комбобокса с типом данных для админа
{
    QTableWidget *tb = this->findChild<QTableWidget*>("adminData");
    QStringList queries={
        R"(SELECT id_user as "Номер user в базе", login as "Логин пользователя" from public."Users")",
        R"(SELECT p.id_post as "Номер поста в базе", u.login as "Автор поста", t.name as "Тип поста" from public."Posts" p JOIN public."Types" t ON p.id_post_type =
        t.id_type LEFT JOIN public."Users-Posts" up ON up.id_post = p.id_post JOIN public."Users" u ON up.id_user = u.id_user)",
        R"(SELECT id_tag as "Номер тега в базе", name as "Название тега" from public."Tags" ORDER BY id_tag)",
        R"(SELECT id_type as "Номер типа в базе", name as "Возможный тип поста" from public."Types")",
    };
    loadTable(tb,queries[index]);
    if (index == 1)
    {
        tb->setColumnCount(tb->columnCount()+2);
        tb->setItem(0,tb->columnCount()-2,utilgui::createTWItem("Просмотреть",false,true));
        tb->setItem(0,tb->columnCount()-1,utilgui::createTWItem("Действие",false,true));
        for (int i=1;i<tb->rowCount();i++)
        {
            QPushButton *tempShow = utilgui::createButton(tb,"showPost","Просмотреть",QRect(0,0,0,0),true);
            tempShow->setProperty("IDpost",tb->item(i,0)->text());
            connect(tempShow,SIGNAL(clicked()),this,SLOT(showOnePostByTag()));
            tb->setCellWidget(i,tb->columnCount()-2,tempShow);
            QPushButton *tempDel = utilgui::createButton(tb,"deletePost","Удалить",QRect(0,0,0,0),true);
            tempDel->setProperty("ID",tb->item(i,0)->text());
            connect(tempDel,SIGNAL(clicked()),this,SLOT(deleteDataAdmin()));
            tb->setCellWidget(i,tb->columnCount()-1,tempDel);
        }
    }
    else
    {
        tb->setColumnCount(tb->columnCount()+1);
        tb->setItem(0,tb->columnCount()-1,utilgui::createTWItem("Действие",false,true));
        for (int i=1;i<tb->rowCount();i++)
        {
            QPushButton *tempDel = utilgui::createButton(tb,"deletePost","Удалить",QRect(0,0,0,0),true);
            tempDel->setProperty("ID",tb->item(i,0)->text());
            connect(tempDel,SIGNAL(clicked()),this,SLOT(deleteDataAdmin()));
            tb->setCellWidget(i,tb->columnCount()-1,tempDel);
        }
    }
    if (index < 2)
        this->findChild<QPushButton*>("adminAddData")->hide();
    else
        this->findChild<QPushButton*>("adminAddData")->show();
}

#include "widget.h"
#include "ui_widget.h"

// DB 경로
#define PATH_DATABASE "C:/Users/iot/Documents/develop/DB/danawha"

Widget::Widget(QWidget *parent)
    : QWidget(parent)
    , ui(new Ui::Widget)
    , currentBrand("")
{
    ui->setupUi(this);

    myDB = QSqlDatabase::addDatabase("QSQLITE");
    myDB.setDatabaseName(PATH_DATABASE);
    if(!myDB.open())
    {
        qDebug() << myDB.lastError();
        return;
    }

    ui->stackedWidget->setCurrentIndex(0);
    ui->stackedWidget_2->setCurrentIndex(0);
    ui->stackedWidget_3->setCurrentIndex(0);
    ui->stackedWidget_4->setCurrentIndex(0);
    ui->stackedWidget_5->setCurrentIndex(0);
    ui->tap_brand_model->setCurrentIndex(0);
    ui->tapMonth_6->setCurrentIndex(0);
    ui->tapBrand->setCurrentIndex(0);
    ui->tapModel->setCurrentIndex(0);

    // 메인 버튼 왼쪽 정렬
    ui->btn_topBrand->setStyleSheet("Text-align:left");
    ui->btn_topModel->setStyleSheet("Text-align:left");

    connect(ui->mainTap_all, SIGNAL(clicked()), this, SLOT(AllTap()));
    connect(ui->mainTap_model, SIGNAL(clicked()), this, SLOT(modelTap()));

    connect(ui->mainTap_brand, SIGNAL(clicked()), this, SLOT(on_mainTap_brand_clicked()));
    connect(ui->month_combo_2, QOverload<int>::of(&QComboBox::currentIndexChanged),
            this, &Widget::on_month_combo_2_currentIndexChanged);
    connect(ui->month_combo_2, SIGNAL(currentIndexChanged(int)), this, SLOT(updateKoreaDayLabel()));
    connect(ui->year_combo_2, SIGNAL(currentIndexChanged(int)), this, SLOT(updateKoreaDayLabel()));
    // 현대 버튼 클릭 시 람다 함수 연결
    connect(ui->hyundai, &QPushButton::clicked, [this]() {
        this->on_hyundai_clicked();
    });

    // BMW 버튼 클릭 시 람다 함수 연결
    connect(ui->BMW, &QPushButton::clicked, [this]() {
        this->on_BMWButton_clicked();
    });
    connect(ui->month_combo_6, QOverload<int>::of(&QComboBox::currentIndexChanged),
            this, &Widget::on_month_combo_6_currentIndexChanged);
    connect(ui->year_combo_2, SIGNAL(currentIndexChanged(int)), this, SLOT(updateincome_dayLabel()));
    connect(ui->month_combo_2, SIGNAL(currentIndexChanged(int)), this, SLOT(updateincome_dayLabel()));

    QMovie * Movie = new QMovie("C:/Users/iot/Desktop/danawa_AD.gif");
    ui -> label_17 -> setMovie(Movie);
    Movie -> setScaledSize(QSize(400,90));
    Movie -> start();
}

Widget::~Widget()
{
    delete ui;
}

// 회원가입 버튼
void Widget::go_joinPage()
{
    ui->stackedWidget->setCurrentIndex(1);
}

// 로그인
void Widget::LOGIN_SUCCESS()
{
    QString userID, userPW;
    QSqlQuery qry;

    userID = ui->input_login_ID->text();
    userPW = ui->input_login_PW->text();

    qry.prepare("SELECT ID, PW FROM USER_DATA WHERE ID = :id AND PW = :pw");
    qry.bindValue(":id", userID);
    qry.bindValue(":pw", userPW);

    if(qry.exec() && qry.next())
    {
        brand_top3_domestic();
        MonthTop();
        ui->stackedWidget->setCurrentIndex(3);
    }
    else
    {
        QMessageBox::critical(this, "알림", "아이디 또는 비밀번호를 다시 확인해주세요.");
    }
    input_clear();
}

// 아이디 중복 검사
void Widget::dupl_id()
{
    QString userID;
    QSqlQuery qry;

    userID = ui->input_Join_ID->text();
    qry.prepare("SELECT ID FROM USER_DATA WHERE ID = :id");
    qry.bindValue(":id", userID);

    if(qry.exec()&&qry.next())
    {
        QMessageBox::warning(this, "알림", "이미 사용 중인 아이디입니다.");
        ui->input_Join_ID->clear();
    }
    else if(userID == "")
    {
        QMessageBox::warning(this, "알림", "빈칸입니다.");
    }
    else
    {
        QMessageBox::information(this, "알림", "사용 가능한 아이디입니다.");
    }
}

// 이메일 중복 검사
void Widget::dupl_email()
{
    QString userE;
    QSqlQuery qry;

    userE = ui->input_Join_EMAIL->text();
    qry.prepare("SELECT EMAIL FROM USER_DATA WHERE EMAIL = :email");
    qry.bindValue(":email", userE);

    if(qry.exec()&&qry.next())
    {
        QMessageBox::warning(this, "알림", "이미 사용 중인 이메일입니다.");
        ui->input_Join_EMAIL->clear();
    }
    else if(userE == "")
    {
        QMessageBox::warning(this, "알림", "빈칸입니다.");
    }
    else
    {
        QMessageBox::information(this, "알림", "사용 가능한 이메일입니다.");
    }
}

// ID/PW 찾기 버튼
void Widget::btn_find()
{
    ui->stackedWidget->setCurrentIndex(2);
    ui->tabWidget->setCurrentIndex(0);
}

// 아이디 찾기
void Widget::find_ID()
{
    QString name, email;
    QSqlQuery qry;

    name = ui->input_findID_NAME->text();
    email = ui->input_findID_EMAIL->text();

    if(name.isEmpty() || email.isEmpty())
    {
        QMessageBox::warning(this, "알림", "빈칸을 채워주세요.");
    }
    else
    {
        QString id = "SELECT ID FROM USER_DATA WHERE NAME= :name AND EMAIL = :email";
        qry.prepare(id);
        qry.bindValue(":name", name);
        qry.bindValue(":email", email);

        if(qry.exec() && qry.next())
        {
            QString showID = qry.value(0).toString();
            QMessageBox::information(this, "알림", "회원님의 ID는 " + showID + " 입니다.");
            ui->stackedWidget->setCurrentIndex(0);
        }
        else
        {
            QMessageBox::critical(this, "알림", "일치하는 회원정보가 없습니다.");
        }
        input_clear();
    }
}

// 비밀번호 찾기
void Widget::find_PW()
{
    QString id, name, email;
    QSqlQuery qry;

    id = ui->input_findPW_ID->text();
    name = ui->input_findPW_NAME->text();
    email = ui->input_findPW_EMAIL->text();

    if(id.isEmpty() || name.isEmpty() || email.isEmpty())
    {
        QMessageBox::warning(this, "알림", "빈칸을 채워주세요.");
    }
    else
    {
        QString pw = "SELECT PW FROM USER_DATA WHERE ID = :id AND NAME= :name AND EMAIL = :email";
        qry.prepare(pw);
        qry.bindValue(":id", id);
        qry.bindValue(":name", name);
        qry.bindValue(":email", email);

        if(qry.exec() && qry.next())
        {
            QString showPW = qry.value(0).toString();
            QMessageBox::information(this, "알림", "회원님의 PW는 " + showPW + " 입니다.");
            ui->stackedWidget->setCurrentIndex(0);
        }
        else
        {
            QMessageBox::critical(this, "알림", "일치하는 회원정보가 없습니다.");
        }
        input_clear();
    }
}

// 회원가입 페이지_회원가입 버튼
void Widget::JOIN_SUCCESS()
{
    QString userNAME, userID, userPW, userEMAIL;
    QSqlQuery qry;

    userNAME = ui->input_Join_NAME->text();
    userID = ui->input_Join_ID->text();
    userPW = ui->input_Join_PW->text();
    userEMAIL = ui->input_Join_EMAIL->text();

    if(userNAME.isEmpty() || userID.isEmpty() || userPW.isEmpty() || userEMAIL.isEmpty())
    {
        QMessageBox::warning(this, "알림", "빈칸을 채워주세요.");
    }
    else
    {
        qry.prepare("INSERT INTO USER_DATA VALUES (:name, :id, :pw, :email)");
        qry.bindValue(":name", userNAME);
        qry.bindValue(":id", userID);
        qry.bindValue(":pw", userPW);
        qry.bindValue(":email", userEMAIL);
        qry.exec();

        QString welcom = "가입을 축하드립니다 " + userNAME + "님!";
        QMessageBox::information(this, "알림", welcom);
        ui->stackedWidget->setCurrentIndex(0);
        input_clear();
    }
}

// 로그인/회원가입 입력창 clear
void Widget::input_clear()
{
    ui->input_Join_NAME->clear();
    ui->input_Join_ID->clear();
    ui->input_Join_PW->clear();
    ui->input_Join_EMAIL->clear();

    ui->input_login_ID->clear();
    ui->input_login_PW->clear();

    ui->input_findID_NAME->clear();
    ui->input_findID_EMAIL->clear();
    ui->input_findPW_NAME->clear();
    ui->input_findPW_ID->clear();
    ui->input_findPW_EMAIL->clear();
}

// 시작 페이지 이동(ID,PW 찾기 / 회원가입 중 로고 누를시)
void Widget::go_startPage()
{
    ui->stackedWidget->setCurrentIndex(0);
    input_clear();
}




// 전체탭 선택
void Widget::AllTap()
{
    ui->stackedWidget->setCurrentIndex(3);
    ui->stackedWidget_2->setCurrentIndex(0);
    ui->stackedWidget_3->setCurrentIndex(0);
    ui->stackedWidget_4->setCurrentIndex(0);
    ui->stackedWidget_5->setCurrentIndex(0);

    ui->month_combo->setCurrentIndex(0);
    ui->period_month1->setCurrentIndex(0);
    ui->period_month2->setCurrentIndex(0);
    ui->tapMonth->setCurrentIndex(0);

    ui->tap_brand_model->setCurrentIndex(0);
    ui->tapMonth_6->setCurrentIndex(0);

    ui->tapBrand->setCurrentIndex(0);
    ui->tapModel->setCurrentIndex(0);

    brand_top3_domestic();      // 브랜드 TOP3
    MonthTop();                 // 모델 TOP10
}

// 브랜드 Top3
void Widget::brand_top3_domestic() //브랜드 TOP3
{
    //국산 수입 점유율 따로 구해서 넣기
    // 브랜드, 판매량 쿼리문
    // 국산, 수입 점유율 쿼리문
    //브랜드 -> 판매량 -> 점유율
    QStringList month = QStringList() << "January" << "February" << "March" << "April" << "May" << "June" << "July" << "August" <<
                        "September" << "October" << "November" << "December";
    int month_choice = ui -> month_combo -> currentIndex();
    QString brand, month_sales;

    QSqlQuery qry;
    int mon_sell, alltotal;
    double market_share; // 점유율 퍼센트용

    if(ui -> tapBrand -> currentIndex() == 1) // 수입차탭일때
    {
        QString income_share = "SELECT sum("+month[month_choice]+") as month_total FROM allCar WHERE domestic_income = 1";//수입 합계 쿼리문
        qry.exec(income_share);
        while(qry.next())
        {
            alltotal = qry.value("month_total").toInt(); //가져온 수입합계 변수에 저장
        }

        int row = 0;
        QString MONTHLY_BRAND = "SELECT brand,"+ month[month_choice] +" FROM month_total WHERE domestic_income = 1 ORDER BY 2 DESC LIMIT 3;"; // 브랜드별 국산, 수입 top3
        qry.exec(MONTHLY_BRAND);
        while(qry.next())
        {
            ui -> tableWidget_3 -> removeRow(row);//초기화용

            brand = qry.value(0).toString();
            mon_sell = qry.value(1).toInt();

            market_share = round((double(mon_sell)/alltotal)*100*10)/10;

            ui -> tableWidget_3 -> insertRow(row);
            ui -> tableWidget_3 -> setItem(row, 0, new QTableWidgetItem(brand));//브랜드명
            ui -> tableWidget_3 -> setItem(row, 1, new QTableWidgetItem(QString::number(mon_sell)));//판매량
            ui -> tableWidget_3 -> setItem(row, 2, new QTableWidgetItem(tr("%1%").arg(market_share,0,'f',1))); //점유율

            row++;
        }
    }
    else
    {
        QString domestic_share = "SELECT sum("+month[month_choice]+") as month_total FROM allCar WHERE domestic_income = 0";//국산 합계 쿼리문
        qry.exec(domestic_share);
        while(qry.next())
        {
            alltotal = qry.value("month_total").toInt(); //가져온 국산합계 변수에 저장
        }

        int row = 0;
        QString MONTHLY_BRAND = "SELECT brand, "+month[month_choice]+" FROM month_total WHERE domestic_income = 0 ORDER BY 2 DESC LIMIT 3;";
        qry.exec(MONTHLY_BRAND);
        while(qry.next())
        {
            ui -> tableWidget_3 -> removeRow(row);//초기화용
            brand = qry.value(0).toString();
            mon_sell = qry.value(1).toInt();
            market_share = round((double(mon_sell)/alltotal)*100*10)/10;

            ui -> tableWidget_3 -> insertRow(row);
            ui -> tableWidget_3 -> setItem(row, 0, new QTableWidgetItem(brand));//브랜드명
            ui -> tableWidget_3 -> setItem(row, 1, new QTableWidgetItem(QString::number(mon_sell)));//판매량
            ui -> tableWidget_3 -> setItem(row, 2, new QTableWidgetItem(tr("%1%").arg(market_share,0,'f',1))); //점유율
            row++;
        }
    }
}

// 브랜드 TOP3 기간조회
void Widget::find_brand() //브랜드 TOP3 기간조회
{
    int startMonth, endMonth, mon_sell, alltotal;
    QSqlQuery qry_model, qry;
    QString model;
    QStringList insertMonth;
    QStringList month = {"January", "February", "March", "April", "May", "June", "July", "August", "September", "October", "November", "December"};
    double market_share;

    startMonth = ui->period_month1->currentText().toInt();
    endMonth = ui->period_month2->currentText().toInt();

    // 범위를 insertMonth의 요소로 넣기
    if(startMonth > endMonth)
    {
        QMessageBox::warning(this, "알림", "범위를 다시 설정해주세요.");
    }
    else
    {
        // 테이블 초기화
        int row = 0;
        ui -> tableWidget_3 -> clearContents();
        ui -> tableWidget_3 -> setRowCount(0);

        QString income_share = "SELECT sum(";//수입 합계 쿼리문
        QString showModel = "SELECT brand, ";
        for(int i = startMonth-1; i < endMonth; i++)
        {
            showModel.append(month[i]);
            income_share.append(month[i]);

            // 마지막으로 뽑히는 거엔 + 되면 안되니께
            if(i != endMonth-1)
            {
                showModel.append("+");
                income_share.append("+");
            }
        }

        if(ui -> tapBrand -> currentIndex() == 1)   //수입
        {
            showModel.append(" FROM month_total WHERE domestic_income = 1 ORDER BY 2 DESC LIMIT 3");
            income_share.append(") as month_total FROM allCar WHERE domestic_income = 1");//점유율
            qry.exec(income_share);
            while(qry.next())
            {
                alltotal = qry.value("month_total").toInt(); //가져온 수입합계 변수에 저장
            }
        }
        else    // 국산
        {

            showModel.append(" FROM month_total WHERE domestic_income = 0 ORDER BY 2 DESC LIMIT 3");
            income_share.append(")as month_total FROM allCar WHERE domestic_income = 0");//점유율
            // qDebug() << income_share;
            qry.exec(income_share);
            while(qry.next())
            {
                alltotal = qry.value("month_total").toInt(); //가져온 합계 변수에 저장
            }
        }
        // qDebug() << showModel;
        qry_model.exec(showModel);
        while(qry_model.next())
        {
            model = qry_model.value(0).toString();
            mon_sell = qry_model.value(1).toInt();
            market_share = round((double(mon_sell)/alltotal)*100*10)/10;

            ui -> tableWidget_3 -> insertRow(row);
            ui -> tableWidget_3 -> setItem(row, 0, new QTableWidgetItem(model));        //브랜드명
            ui -> tableWidget_3 -> setItem(row, 1, new QTableWidgetItem(QString::number(mon_sell)));   //판매량
            ui -> tableWidget_3 -> setItem(row, 2, new QTableWidgetItem(tr("%1%").arg(market_share,0,'f',1))); //점유율
            row++;
        }
    }
}

//브랜드 국산 수입 선택 탭
void Widget::on_tapBrand_currentChanged(int index)
{
    //월별조회
    QStringList month = QStringList() << "January" << "February" << "March" << "April" << "May" << "June" << "July" << "August" <<
                        "September" << "October" << "November" << "December";
    int month_choice = ui -> month_combo -> currentIndex();
    QString brand;

    QSqlQuery qry;
    int mon_sell, alltotal;
    double market_share; // 점유율 퍼센트용

    if(ui -> tapBrand -> currentIndex() == 1) // 수입차탭일때
    {
        QString income_share = "SELECT sum("+month[month_choice]+") as month_total FROM allCar WHERE domestic_income = 1";//수입 합계 쿼리문
        qry.exec(income_share);
        while(qry.next())
        {
            alltotal = qry.value("month_total").toInt(); //가져온 수입합계 변수에 저장
        }

        int row = 0;
        QString MONTHLY_BRAND = "SELECT brand,"+month[month_choice]+" FROM month_total WHERE domestic_income = 1 ORDER BY 2 DESC LIMIT 3;";
        qry.exec(MONTHLY_BRAND);
        while(qry.next())
        {
            ui -> tableWidget_3 -> removeRow(row);//초기화용

            brand = qry.value(0).toString();
            mon_sell = qry.value(1).toInt();

            market_share = round((double(mon_sell)/alltotal)*100*10)/10;

            ui -> tableWidget_3 -> insertRow(row);
            ui -> tableWidget_3 -> setItem(row, 0, new QTableWidgetItem(brand));//브랜드명
            ui -> tableWidget_3 -> setItem(row, 1, new QTableWidgetItem(QString::number(mon_sell)));//판매량
            ui -> tableWidget_3 -> setItem(row, 2, new QTableWidgetItem(tr("%1%").arg(market_share,0,'f',1))); //점유율

            row++;
        }
    }
    else
    {
        QString domestic_share = "SELECT sum("+month[month_choice]+") as month_total FROM allCar WHERE domestic_income = 0";//국산 합계 쿼리문
        qry.exec(domestic_share);
        while(qry.next())
        {
            alltotal = qry.value("month_total").toInt(); //가져온 국산합계 변수에 저장
        }

        int row = 0;
        QString MONTHLY_BRAND = "SELECT brand, "+month[month_choice]+" FROM month_total WHERE domestic_income = 0 ORDER BY 2 DESC LIMIT 3;";
        qry.exec(MONTHLY_BRAND);
        while(qry.next())
        {
            ui -> tableWidget_3 -> removeRow(row);//초기화용
            brand = qry.value(0).toString();
            mon_sell = qry.value(1).toInt();
            market_share = round((double(mon_sell)/alltotal)*100*10)/10;

            ui -> tableWidget_3 -> insertRow(row);
            ui -> tableWidget_3 -> setItem(row, 0, new QTableWidgetItem(brand));//브랜드명
            ui -> tableWidget_3 -> setItem(row, 1, new QTableWidgetItem(QString::number(mon_sell)));//판매량
            ui -> tableWidget_3 -> setItem(row, 2, new QTableWidgetItem(tr("%1%").arg(market_share,0,'f',1))); //점유율
            row++;
        }
    }
}

// 모델별 선택
void Widget::modelTap()
{
    ui->stackedWidget_3->setCurrentIndex(3);
    ui->stackedWidget_4->setCurrentIndex(3);

    ui->tap_brand_model->setCurrentIndex(0);
    ui->month_combo_3->setCurrentIndex(0);
    ui->period_month1_3->setCurrentIndex(0);
    ui->period_month2_3->setCurrentIndex(0);

    MonthTop();
    selectMonth();
}

// 모델탭 별 월 선택
void Widget::selectMonth()
{
    int combo_Month, total, allTotal;
    double share;
    int row = 0;
    QString model;
    QSqlQuery qry;
    QStringList monthList = {"January", "February", "March", "April", "May", "June", "July", "August", "September", "October", "November", "December"};

    ui->tableWidget_10->setRowCount(0);

    combo_Month = ui->month_combo_3->currentText().toInt();

    QString showCount = "SELECT SUM(" + monthList[combo_Month-1] + ") as count FROM allCar";
    qry.exec(showCount);
    while(qry.next())
    {
        allTotal = qry.value("count").toInt();
    }

    QString showModel = "SELECT Model, " + monthList[combo_Month-1] + " FROM allCar ORDER BY 2 DESC";
    qry.exec(showModel);
    while(qry.next())
    {
        model = qry.value(0).toString();
        total = qry.value(1).toInt();
        share = round((double(total)/allTotal)*100*10)/10;

        ui->tableWidget_10->insertRow(row);
        ui->tableWidget_10->setItem(row, 0, new QTableWidgetItem(model));
        ui->tableWidget_10->setItem(row, 1, new QTableWidgetItem(QString::number(total)));
        ui->tableWidget_10->setItem(row, 2, new QTableWidgetItem(tr("%1%").arg(share,0,'f',1)));
        row++;
    }
}

// 모델탭 별 기간 선택
void Widget::selectPeriod()
{
    int startMonth, endMonth, total, allTotal;          // 시작월, 끝나는월, 월 범위, 월, 월별 총합
    double share;                                       // 점유율

    QString startPeriod, endPeriod, model;
    QSqlQuery qry;
    QStringList monthList = {"January", "February", "March", "April", "May", "June", "July", "August", "September", "October", "November", "December"};

    // 콤보박스 값 int로 가져오기
    startMonth = ui->period_month1_3->currentText().toInt();
    endMonth = ui->period_month2_3->currentText().toInt();

    if(startMonth > endMonth)
    {
        QMessageBox::warning(this, "알림", "범위를 다시 설정해주세요.");
    }
    else
    {
        // 컬럼별(월) 총합_점유율
        QString showCount = "SELECT SUM(";
        for(int i = startMonth-1; i < endMonth; i++)
        {
            showCount.append(monthList[i]);
            showCount.append(")");
            if(i != endMonth-1)
            {
                showCount.append("+ SUM(");
            }
        }
        showCount.append(" FROM allCar");

        qry.exec(showCount);
        while(qry.next())
        {
            allTotal = qry.value(0).toInt();
        }

        // 모델별 월 판매량
        QString showModel = "SELECT Model, ";
        for(int i = startMonth-1; i < endMonth; i++)
        {
            showModel.append(monthList[i]);
            if(i != endMonth-1)
            {
                showModel.append("+");
            }
        }
        showModel.append(" AS total FROM allCar ORDER BY total DESC");

        // 테이블 초기화
        ui->tableWidget_10->setRowCount(0);
        int row = 0;

        qry.exec(showModel);
        while(qry.next())
        {
            model = qry.value(0).toString();
            total = qry.value(1).toInt();
            share = round((double(total)/allTotal)*100*10)/10;

            ui->tableWidget_10->insertRow(row);
            ui->tableWidget_10->setItem(row, 0, new QTableWidgetItem(model));
            ui->tableWidget_10->setItem(row, 1, new QTableWidgetItem(QString::number(total)));
            ui->tableWidget_10->setItem(row, 2, new QTableWidgetItem(tr("%1%").arg(share,0,'f',1)));
            row++;
        }
    }
}

// 콤보박스의 currentIndexChanged, 모델탭의 currentChanged slots 둘 다 여기로 연결함
void Widget::MonthTop()
{
    ui->stackedWidget_5->setCurrentIndex(0);
    ui->tapMonth_3->setCurrentIndex(0);
    ui->month_combo_3->setCurrentIndex(0);
    ui->period_month1_3->setCurrentIndex(0);
    ui->period_month2_3->setCurrentIndex(0);

    ui->tableWidget_2->clearContents();
    ui->tableWidget_2->setRowCount(0);
    int row = 0;

    int total, model_sell, previous, abs, month_choice;     // 전월대비, 판매량, 전월판매량, 절대값, 콤보박스에서 선택한 달
    QSqlQuery qry_model;
    QString model, model_top;                               // 브랜드명, 쿼리문
    QStringList month = {"January", "February", "March", "April", "May", "June", "July", "August", "September", "October", "November", "December"};

    // 콤보박스 값 int로 받음
    month_choice = ui -> month_combo -> currentText().toInt();

    // 탭 구분(국산/수입 쿼리문만 다르고 뽑는 건 똑같음)
    if(ui -> tapModel -> currentIndex() == 1) //수입
    {
        // 만약 콤보박스로 선택한 달이 1월이면 month_choice가 1인데, month[1]의 요소는 February임. 그래서 -1 해갖고 month[0] 요소인 January 갖고옴
        if(month_choice == 1)
        {
            model_top = "SELECT Model, " + month[month_choice-1] +" FROM allCar WHERE domestic_income = 1 ORDER BY 2 DESC LIMIT 10;";
        }
        else
        {
            model_top = "SELECT Model, "+month[month_choice -2]+","+ month[month_choice-1] +" FROM allCar WHERE domestic_income = 1 ORDER BY 3 DESC LIMIT 10;";
        }
    }
    else //국산
    {
        if(month_choice == 1)
        {
            model_top = "SELECT Model, " + month[month_choice-1] +" FROM allCar WHERE domestic_income = 0 ORDER BY 2 DESC LIMIT 10;";
        }
        else
        {
            model_top = "SELECT Model, "+month[month_choice -2]+","+ month[month_choice-1] +" FROM allCar WHERE domestic_income = 0 ORDER BY 3 DESC LIMIT 10;";
        }
    }

    // 출력
    qry_model.exec(model_top);
    while(qry_model.next())
    {
        // 모델명
        model = qry_model.value(0).toString();
        ui -> tableWidget_2 -> insertRow(row);
        ui -> tableWidget_2 -> setItem(row, 0, new QTableWidgetItem(model));//브랜드명

        // 판매량
        if(month_choice == 1)       // 1월일 때
        {
            model_sell = qry_model.value(1).toInt();
            total = 0;

            ui -> tableWidget_2 -> setItem(row, 1, new QTableWidgetItem(QString::number(model_sell)));//판매량
        }
        else
        {
            previous = qry_model.value(1).toInt();
            model_sell = qry_model.value(2).toInt();
            total = model_sell - previous;

            ui -> tableWidget_2 -> setItem(row, 1, new QTableWidgetItem(QString::number(model_sell)));//판매량
        }

        // 전월대비
        if(total < 0)
        {
            abs = qAbs(total);

            QString insert_value = "▼ " + QString::number(abs);
            ui->tableWidget_2->setItem(row, 2, new QTableWidgetItem(insert_value));
            ui->tableWidget_2->item(row, 2)->setForeground(Qt::blue);
            row++;
        }
        else if(total == 0)
        {
            QString insert_value = "-";
            ui->tableWidget_2->setItem(row, 2, new QTableWidgetItem(insert_value));
            row++;
        }
        else
        {
            QString insert_value = "▲ " + QString::number(total);
            ui->tableWidget_2->setItem(row, 2, new QTableWidgetItem(insert_value));
            ui->tableWidget_2->item(row, 2)->setForeground(Qt::red);
            row++;
        }
    }
}

// 모델top10 기간선택
void Widget::MainSelectPeriod_Model()
{
    ui->stackedWidget_5->setCurrentIndex(1);

    int startMonth, endMonth;
    QSqlQuery qry_model;
    QString model, model_sell;
    QStringList insertMonth;
    QStringList month = {"January", "February", "March", "April", "May", "June", "July", "August", "September", "October", "November", "December"};

    startMonth = ui->period_month1->currentText().toInt();
    endMonth = ui->period_month2->currentText().toInt();

    qDebug() << startMonth << endMonth;
    // 범위를 insertMonth의 요소로 넣기
    if(!(startMonth > endMonth))
    {
        QString showModel = "SELECT Model, ";
        for(int i = startMonth-1; i < endMonth; i++)
        {
            showModel.append(month[i]);

            // 마지막으로 뽑히는 거엔 + 되면 안되니께
            if(i != endMonth-1)
            {
                showModel.append("+");
            }
        }

        if(ui -> tapModel_2 -> currentIndex() == 1)   //수입
        {
            showModel.append(" FROM allCar WHERE domestic_income = 1 ORDER BY 2 DESC LIMIT 10");
        }
        else    // 국산
        {
            showModel.append(" FROM allCar WHERE domestic_income = 0 ORDER BY 2 DESC LIMIT 10");
        }
        // 테이블 초기화
        int row = 0;
        ui->tableWidget_5->clearContents();
        ui->tableWidget_5->setRowCount(0);

        qry_model.exec(showModel);
        while(qry_model.next())
        {
            model = qry_model.value(0).toString();
            model_sell = qry_model.value(1).toString();

            ui -> tableWidget_5 -> insertRow(row);
            ui -> tableWidget_5 -> setItem(row, 0, new QTableWidgetItem(model));        //브랜드명
            ui -> tableWidget_5 -> setItem(row, 1, new QTableWidgetItem(model_sell));   //판매량
            row++;
        }
    }
}

// 차종별 클릭했을 때
void Widget::CarType()
{
    ui -> stackedWidget_3 -> setCurrentIndex(4);
    ui -> stackedWidget_4 -> setCurrentIndex(3);
    ui -> mainTap_model->setChecked(true);

    QPushButton * temp = qobject_cast<QPushButton*>(sender());  // QPushButton에 대한 포인터
    QString model_type = temp ->objectName();

    // 클릭된 버튼의 자동차 유형을 나타내는 문자열
    if (model_type == "light_car")
        car = "경차";
    else if(model_type == "small_car")
        car = "소형";
    else if(model_type == "semimidium_car")
        car = "준중형";
    else if(model_type == "midium_Car")
        car = "중형";
    else if(model_type == "semilarge_car")
        car = "준대형";
    else if(model_type == "large_Car")
        car = "대형";
    else if(model_type == "sports_car")
        car = "스포츠카";
    else if(model_type == "small_suv")
        car = "소형SUV";
    else if(model_type == "midium_suv")
        car = "중형SUV";
    else if(model_type == "large_suv")
        car = "대형SUV";
    else if(model_type == "RVMPV_btn")
        car = "RV";
    else
        car = "상용";

    CarType_Month();
}

// 차종별 월 선택
void Widget::CarType_Month()
{
    int combo_Month, total, allTotal;
    double share;
    int row = 0;
    QString model, light_car;
    QSqlQuery qry;
    QStringList monthList = {"January", "February", "March", "April", "May", "June", "July", "August", "September", "October", "November", "December"};

    ui->tableWidget_10->setRowCount(0);

    combo_Month = ui->month_combo_4->currentText().toInt();

    //차종별끼리의 합
    QString income_share = "SELECT sum("+monthList[combo_Month-1]+") as month_total FROM allCar WHERE size = '"+ car +"'"; //수입 합계 쿼리문
    qry.exec(income_share);
    while(qry.next())
    {
        allTotal = qry.value("month_total").toInt();    //가져온 수입합계 변수에 저장
    }


    if(car == "RV")
    {
        light_car = "SELECT Model, "+monthList[combo_Month-1]+" FROM allCar WHERE size LIKE '"+ car +"%' AND "+monthList[combo_Month-1]+" != 0 ORDER BY 2 DESC";
    }
    else
    {
        light_car = "SELECT Model, "+monthList[combo_Month-1]+" FROM allCar WHERE size = '"+ car +"' AND "+monthList[combo_Month-1]+" != 0 ORDER BY 2 DESC";
    }

    qry.exec(light_car);
    while(qry.next())
    {
        model = qry.value(0).toString();
        total = qry.value(1).toInt();
        share = round((double(total)/allTotal)*100*10)/10;

        ui->tableWidget_10->insertRow(row);
        ui->tableWidget_10->setItem(row, 0, new QTableWidgetItem(model));
        ui->tableWidget_10->setItem(row, 1, new QTableWidgetItem(QString::number(total)));
        ui->tableWidget_10->setItem(row, 2, new QTableWidgetItem(tr("%1%").arg(share,0,'f',1)));
        row++;
    }
}

// 차종별 기간 선택
void Widget::CarType_Period()
{
    int startMonth, endMonth, mon_sell, alltotal;
    QSqlQuery qry_model, qry;
    QString model;
    QStringList insertMonth;
    QStringList month = {"January", "February", "March", "April", "May", "June", "July", "August", "September", "October", "November", "December"};
    double market_share;

    // 테이블 초기화
    int row = 0;
    ui -> tableWidget_10 -> clearContents();
    ui -> tableWidget_10 -> setRowCount(0);

    startMonth = ui->period_month1_4->currentText().toInt();
    endMonth = ui->period_month2_4->currentText().toInt();

    // 범위를 insertMonth의 요소로 넣기
    if(startMonth > endMonth)
    {
        QMessageBox::warning(this, "알림", "범위를 다시 설정해주세요.");
    }
    else
    {
        QString income_share = "SELECT sum(";//합계 쿼리문
        QString showModel = "SELECT Model, ";
        for(int i = startMonth-1; i < endMonth; i++)
        {
            showModel.append(month[i]);
            income_share.append(month[i]);

            // 마지막으로 뽑히는 거엔 + 되면 안되니께
            if(i != endMonth-1)
            {
                showModel.append("+");
                income_share.append("+");
            }
        }

        showModel.append(" as zero FROM allCar WHERE size = '"+ car +"' AND zero !=0 ORDER BY 2 DESC");
        income_share.append(") as month_total FROM allCar WHERE size = '"+ car +"'");//점유율합계

        qDebug() << showModel;
        qDebug() << income_share;
        qry.exec(income_share);
        while(qry.next())
        {
            alltotal = qry.value("month_total").toInt(); //가져온 합계 변수에 저장
        }
        qry_model.exec(showModel);
        while(qry_model.next())
        {
            model = qry_model.value(0).toString();
            mon_sell = qry_model.value(1).toInt();
            market_share = round((double(mon_sell)/alltotal)*100*10)/10;

            ui -> tableWidget_10 -> insertRow(row);
            ui -> tableWidget_10 -> setItem(row, 0, new QTableWidgetItem(model));        //브랜드명
            ui -> tableWidget_10 -> setItem(row, 1, new QTableWidgetItem(QString::number(mon_sell)));   //판매량
            ui -> tableWidget_10 -> setItem(row, 2, new QTableWidgetItem(tr("%1%").arg(market_share,0,'f',1))); //점유율
            row++;
        }
    }
}

// 차종 1페이지
void Widget::on_type_pre_clicked()
{
    ui->stackedWidget_2->setCurrentIndex(0);
}

// 차종 2페이지
void Widget::on_type_next_clicked()
{
    ui->stackedWidget_2->setCurrentIndex(1);
}







// 브랜드별 버튼 클릭 처리
void Widget::on_mainTap_brand_clicked()
{
    // brand 위젯으로 전환
    ui->stackedWidget_4->setCurrentIndex(1);
    ui->stackedWidget_3->setCurrentIndex(2);

    on_month_combo_2_currentIndexChanged(0);
}

//브랜드별 월별 선택
void Widget::on_month_combo_2_currentIndexChanged(int index)
{


    QStringList monthNames = { "January", "February", "March", "April", "May", "June",
                              "July", "August", "September", "October", "November", "December" };
    QString selectedMonthName = monthNames[index]; // 선택된 월의 이름을 가져옵니다.

    // 국산차 브랜드 리스트
    QStringList domesticBrandTables = {"Chevrolet2023", "Genesis2023", "Hyundai2023", "KIA2023", "KG2023"};
    // 수입차 브랜드 리스트
    QStringList importBrandTables = {"Audi2023", "Benz2023", "BMW2023", "Honda2023", "Porsche2023"};

    ui->label_12->setText("판매량");
    ui->label_13->setText("전월 대비");

    // 국산차 데이터를 tableWidget_7에 로드
    loadBrandDataToTableWidget(ui->tableWidget_7,domesticBrandTables, selectedMonthName, ui->korea_total);

    // 수입차 데이터를 tableWidget_8에 로드
    loadBrandDataToTableWidget(ui->tableWidget_8,importBrandTables, selectedMonthName, ui->income_total);




}
//브랜드별 테이블위젯 업데이트함수
void Widget::loadBrandDataToTableWidget(QTableWidget* tableWidget,QStringList& brandTables, const QString& selectedMonthName, QLabel* totalSalesLabel)
{
    tableWidget->clearContents();
    tableWidget->setRowCount(0);
    tableWidget->setColumnCount(3); // 브랜드명, 판매량, 점유율 칼럼 추가

    // 칼럼 헤더 설정
    QStringList headers = {"브랜드명", "판매량", "점유율"};
    tableWidget->setHorizontalHeaderLabels(headers);

    qint64 totalSales = 0;
    QList<QPair<QString, qint64>> brandSalesList;

    foreach (const QString &brand, brandTables) {
        QSqlQuery query;
        QString queryStr = QString("SELECT SUM(\"%1\") FROM %2").arg(selectedMonthName, brand);
        query.exec(queryStr);

        if (query.lastError().type() != QSqlError::NoError) {
            qDebug() << "Error: Unable to execute query" << query.lastError();
            continue;
        }

        if (query.next()) {
            qint64 monthlyTotal = query.value(0).toInt();
            brandSalesList.append(qMakePair(brand.left(brand.length() - 4), monthlyTotal)); // 연도 제거
            totalSales += monthlyTotal;
        }
    }

    std::sort(brandSalesList.begin(), brandSalesList.end(), [](const QPair<QString, qint64> &a, const QPair<QString, qint64> &b) {
        return a.second > b.second;
    });

    for (int i = 0; i < brandSalesList.size(); ++i) {
        QString brandName = brandSalesList[i].first;
        qint64 sales = brandSalesList[i].second;
        double marketShare = static_cast<double>(sales) / totalSales * 100;

        QTableWidgetItem *brandItem = new QTableWidgetItem(brandName);
        QTableWidgetItem *salesItem = new QTableWidgetItem(QString::number(sales));
        QTableWidgetItem *marketShareItem = new QTableWidgetItem(QString::number(marketShare, 'f', 2) + "%");

        int row = tableWidget->rowCount();
        tableWidget->insertRow(row);
        tableWidget->setItem(row, 0, brandItem);
        tableWidget->setItem(row, 1, salesItem);
        tableWidget->setItem(row, 2, marketShareItem);
    }

    // 총합 대수를 해당 라벨에 표시
    totalSalesLabel->setText(QString::number(totalSales) + "대");
}
//korea_day라벨업데이트
void Widget::updateKoreaDayLabel()
{
    QString year = ui->year_combo_2->currentText();
    QString month = ui->month_combo_2->currentText();
    // 월이 한 자리 수일 경우 앞에 0을 붙입니다.
    if (month.length() == 1) {
        month.prepend("0");
    }
    QString dateText = year + "." + month;
    ui->korea_day->setText(dateText);
}
//income_day라벨업데이트
void Widget::updateincome_dayLabel() {
    QString year = ui->year_combo_2->currentText();
    QString month = ui->month_combo_2->currentText();
    // 월이 한 자리 수일 경우 앞에 0을 붙입니다.
    if (month.length() == 1) {
        month.prepend("0");
    }
    QString dateText = year + "." + month;
    ui->income_day->setText(dateText);
}
//브랜드별 기간별 선택 함수
void Widget::on_period_find_2_clicked()
{
    QString startYear = ui->period_year1_2->currentText();
    QString endYear = ui->period_year2_2->currentText();
    // 사용자가 선택한 시작 월과 종료 월의 인덱스를 계산합니다.
    int startMonthIndex = ui->period_month1_2->currentIndex(); // 0부터 시작
    int endMonthIndex = ui->period_month2_2->currentIndex(); // 0부터 시작

    if(startMonthIndex > endMonthIndex)
    {
        QMessageBox::warning(this, "알림", "범위를 다시 설정해주세요.");
    }
    else
    {
        loadPeriodDataToTableWidget(ui->tableWidget_7, startYear, startMonthIndex, endYear, endMonthIndex, true); // 국산차
        loadPeriodDataToTableWidget(ui->tableWidget_8, startYear, startMonthIndex, endYear, endMonthIndex, false); // 수입차
    }
}
//브랜드별 기간별 테이블위젯 업데이트함수
void Widget::loadPeriodDataToTableWidget(QTableWidget* tableWidget, const QString& startYear, int startMonthIndex, const QString& endYear, int endMonthIndex, bool isDomestic)
{
    QStringList monthNames = { "January", "February", "March", "April", "May", "June",
                              "July", "August", "September", "October", "November", "December" };

    QStringList brandTables = isDomestic ? QStringList{"Chevrolet2023", "Genesis2023", "Hyundai2023", "KIA2023", "KG2023"}
                                         : QStringList{"Audi2023", "Benz2023", "BMW2023", "Honda2023", "Porsche2023"};

    QMap<QString, qint64> brandSales;
    qint64 totalSales = 0;

    for (const QString& brandTable : brandTables) {
        qint64 brandTotalSales = 0;
        for (int monthIndex = startMonthIndex; monthIndex <= endMonthIndex; ++monthIndex) {
            QString month = monthNames[monthIndex];
            QString queryStr = QString("SELECT SUM(\"%1\") FROM %2").arg(month, brandTable);
            QSqlQuery query;
            if (!query.exec(queryStr)) {
                qDebug() << "Error executing query for table" << brandTable << ":" << query.lastError();
                continue;
            }
            if (query.next()) {
                brandTotalSales += query.value(0).toInt();
            }
        }
        brandSales[brandTable.left(brandTable.length() - 4)] = brandTotalSales;
        totalSales += brandTotalSales;
    }

    if (isDomestic) {
        ui->korea_total->setText(QString::number(totalSales) + "대");
        ui->korea_day->setText(""); // Clear
    } else {
        ui->income_total->setText(QString::number(totalSales) + "대");
        ui->income_day->setText(""); // Clear
    }

    tableWidget->clearContents();
    tableWidget->setRowCount(0);
    tableWidget->setHorizontalHeaderLabels(QStringList() << "브랜드명" << "판매량" << "점유율");

    QVector<QPair<QString, qint64>> sortableList;
    for (auto it = brandSales.constBegin(); it != brandSales.constEnd(); ++it) {
        sortableList.append(qMakePair(it.key(), it.value()));
    }

    std::sort(sortableList.begin(), sortableList.end(), [](const QPair<QString, qint64>& a, const QPair<QString, qint64>& b) {
        return a.second > b.second; // Descending order
    });

    int row = 0;
    for (const auto& pair : sortableList) {
        double marketShare = totalSales > 0 ? (100.0 * pair.second / totalSales) : 0;
        tableWidget->insertRow(row);
        tableWidget->setItem(row, 0, new QTableWidgetItem(pair.first));
        tableWidget->setItem(row, 1, new QTableWidgetItem(QString::number(pair.second)));
        tableWidget->setItem(row, 2, new QTableWidgetItem(QString::number(marketShare, 'f', 2) + "%"));
        ++row;
    }
}

void Widget::on_hyundai_clicked()
{

    currentBrand = "Hyundai"; // 현재 선택된 브랜드를 "Hyundai"로 설정
    ui->stackedWidget_4->setCurrentIndex(2);
    ui->stackedWidget_3->setCurrentIndex(1);
    QPixmap brandLogo("C:/Users/iot/Downloads/DB/danawa/HYUNDAI.png");
    ui->brand_logo->setPixmap(brandLogo.scaled(ui->brand_logo->size(), Qt::KeepAspectRatio, Qt::SmoothTransformation));
    loadBrandData(currentBrand);
    // 브랜드 로고 이미지 설정

    // 오타 수정: 함수 이름을 정확하게 맞춤
}
void Widget::on_BMWButton_clicked()
{
    currentBrand = "BMW";
    ui->stackedWidget_4->setCurrentIndex(2); // BMW에 맞는 페이지 인덱스로 설정
    ui->stackedWidget_3->setCurrentIndex(1);
    QPixmap brandLogo("C:/Users/iot/Downloads/DB/danawa/bmw.jpg");
    ui->brand_logo->setPixmap(brandLogo.scaled(ui->brand_logo->size(), Qt::KeepAspectRatio, Qt::SmoothTransformation));    // 필요에 따라 조정
    loadBrandData(currentBrand);

}
void Widget::on_genesis_clicked()
{
    currentBrand = "Genesis";
    ui->stackedWidget_4->setCurrentIndex(2); // BMW에 맞는 페이지 인덱스로 설정
    ui->stackedWidget_3->setCurrentIndex(1); // 필요에 따라 조정
    // 브랜드 로고 이미지 설정
    QPixmap brandLogo("C:/Users/iot/Downloads/DB/danawa/GENESIS.png");
    ui->brand_logo->setPixmap(brandLogo.scaled(ui->brand_logo->size(), Qt::KeepAspectRatio, Qt::SmoothTransformation));
    loadBrandData(currentBrand); // BMW 데이터 로딩 함수 호출
}
void Widget::on_KG_clicked()
{
    currentBrand = "KG";
    ui->stackedWidget_4->setCurrentIndex(2); // BMW에 맞는 페이지 인덱스로 설정
    ui->stackedWidget_3->setCurrentIndex(1); // 필요에 따라 조정
    // 브랜드 로고 이미지 설정
    QPixmap brandLogo("C:/Users/iot/Downloads/DB/danawa/KG_MOBILITY.png");
    ui->brand_logo->setPixmap(brandLogo.scaled(ui->brand_logo->size(), Qt::KeepAspectRatio, Qt::SmoothTransformation));
    loadBrandData(currentBrand); // BMW 데이터 로딩 함수 호출
}
void Widget::on_chevolet_clicked()
{
    currentBrand = "Chevrolet";
    ui->stackedWidget_4->setCurrentIndex(2); // BMW에 맞는 페이지 인덱스로 설정
    ui->stackedWidget_3->setCurrentIndex(1); // 필요에 따라 조정
    QPixmap brandLogo("C:/Users/iot/Downloads/DB/danawa/CHEVROLET.png");
    ui->brand_logo->setPixmap(brandLogo.scaled(ui->brand_logo->size(), Qt::KeepAspectRatio, Qt::SmoothTransformation));
    loadBrandData(currentBrand); // BMW 데이터 로딩 함수 호출
}
void Widget::on_benz_clicked()
{
    currentBrand = "Benz";
    ui->stackedWidget_4->setCurrentIndex(2); // BMW에 맞는 페이지 인덱스로 설정
    ui->stackedWidget_3->setCurrentIndex(1); // 필요에 따라 조정
    QPixmap brandLogo("C:/Users/iot/Downloads/DB/danawa/benz.png");
    ui->brand_logo->setPixmap(brandLogo.scaled(ui->brand_logo->size(), Qt::KeepAspectRatio, Qt::SmoothTransformation));
    loadBrandData(currentBrand); // BMW 데이터 로딩 함수 호출

}

void Widget::on_porsche_clicked()
{
    currentBrand = "porsche";
    ui->stackedWidget_4->setCurrentIndex(2); // BMW에 맞는 페이지 인덱스로 설정
    ui->stackedWidget_3->setCurrentIndex(1); // 필요에 따라 조정
    QPixmap brandLogo("C:/Users/iot/Downloads/DB/danawa/prosche.jpg");
    ui->brand_logo->setPixmap(brandLogo.scaled(ui->brand_logo->size(), Qt::KeepAspectRatio, Qt::SmoothTransformation));
    loadBrandData(currentBrand); // BMW 데이터 로딩 함수 호출
}
void Widget::on_honda_clicked()
{
    currentBrand = "Honda";
    ui->stackedWidget_4->setCurrentIndex(2); // BMW에 맞는 페이지 인덱스로 설정
    ui->stackedWidget_3->setCurrentIndex(1); // 필요에 따라 조정
    QPixmap brandLogo("C:/Users/iot/Downloads/DB/danawa/honda.png");
    ui->brand_logo->setPixmap(brandLogo.scaled(ui->brand_logo->size(), Qt::KeepAspectRatio, Qt::SmoothTransformation));
    loadBrandData(currentBrand); // BMW 데이터 로딩 함수 호출
}


void Widget::on_audi_clicked()
{
    currentBrand = "Audi";
    ui->stackedWidget_4->setCurrentIndex(2); // BMW에 맞는 페이지 인덱스로 설정
    ui->stackedWidget_3->setCurrentIndex(1); // 필요에 따라 조정
    QPixmap brandLogo("C:/Users/iot/Downloads/DB/danawa/AUDI.png");
    ui->brand_logo->setPixmap(brandLogo.scaled(ui->brand_logo->size(), Qt::KeepAspectRatio, Qt::SmoothTransformation));
    loadBrandData(currentBrand); // BMW 데이터 로딩 함수 호출
}
//세부 브랜드별 월별 선택 함수
void Widget::on_month_combo_6_currentIndexChanged(int index)
{
    ui->stackedWidget_4->setCurrentIndex(2);
    qDebug() << "on_month_combo_6_currentIndexChanged start";
    ui->label_12->setText("판매량");
    ui->label_13->setText("전월 대비");
    // 현재 선택된 브랜드에 따라 적절한 데이터 로딩 함수를 호출
    if (currentBrand == "Hyundai") {
        QPixmap brandLogo("C:/Users/iot/Downloads/DB/danawa/HYUNDAI.png");
        ui->brand_logo->setPixmap(brandLogo.scaled(ui->brand_logo->size(), Qt::KeepAspectRatio, Qt::SmoothTransformation));
        loadBrandData(currentBrand);


    } else if (currentBrand == "BMW") {
        QPixmap brandLogo("C:/Users/iot/Downloads/DB/danawa/bmw.jpg");
        ui->brand_logo->setPixmap(brandLogo.scaled(ui->brand_logo->size(), Qt::KeepAspectRatio, Qt::SmoothTransformation));
        loadBrandData(currentBrand);
            // 브랜드 로고 이미지 설정

    } else if (currentBrand == "KIA") {
        QPixmap brandLogo("C:/Users/iot/Downloads/DB/danawa/KIA.png");
        ui->brand_logo->setPixmap(brandLogo.scaled(ui->brand_logo->size(), Qt::KeepAspectRatio, Qt::SmoothTransformation));
        loadBrandData(currentBrand);

    } else if (currentBrand == "Genesis") {
        QPixmap brandLogo("C:/Users/iot/Downloads/DB/danawa/GENESIS.png");
        ui->brand_logo->setPixmap(brandLogo.scaled(ui->brand_logo->size(), Qt::KeepAspectRatio, Qt::SmoothTransformation));
        loadBrandData(currentBrand);


    } else if (currentBrand == "KG") {
        QPixmap brandLogo("C:/Users/iot/Downloads/DB/danawa/KG_MOBILITY.png");
        ui->brand_logo->setPixmap(brandLogo.scaled(ui->brand_logo->size(), Qt::KeepAspectRatio, Qt::SmoothTransformation));
        loadBrandData(currentBrand);

    } else if (currentBrand == "Chevrolet") {
        QPixmap brandLogo("C:/Users/iot/Downloads/DB/danawa/CHEVROLET.png");
        ui->brand_logo->setPixmap(brandLogo.scaled(ui->brand_logo->size(), Qt::KeepAspectRatio, Qt::SmoothTransformation));
        loadBrandData(currentBrand);

    } else if (currentBrand == "Benz") {
        QPixmap brandLogo("C:/Users/iot/Downloads/DB/danawa/benz.png");
        ui->brand_logo->setPixmap(brandLogo.scaled(ui->brand_logo->size(), Qt::KeepAspectRatio, Qt::SmoothTransformation));
        loadBrandData(currentBrand);

    } else if (currentBrand == "Honda") {
        QPixmap brandLogo("C:/Users/iot/Downloads/DB/danawa/honda.png");
        ui->brand_logo->setPixmap(brandLogo.scaled(ui->brand_logo->size(), Qt::KeepAspectRatio, Qt::SmoothTransformation));
        loadBrandData(currentBrand);

    } else if (currentBrand == "porsche") {
        QPixmap brandLogo("C:/Users/iot/Downloads/DB/danawa/prosche.jpg");
        ui->brand_logo->setPixmap(brandLogo.scaled(ui->brand_logo->size(), Qt::KeepAspectRatio, Qt::SmoothTransformation));
        loadBrandData(currentBrand);

    } else if (currentBrand == "Audi") {
        QPixmap brandLogo("C:/Users/iot/Downloads/DB/danawa/AUDI.png");
        ui->brand_logo->setPixmap(brandLogo.scaled(ui->brand_logo->size(), Qt::KeepAspectRatio, Qt::SmoothTransformation));
        loadBrandData(currentBrand);

    }
    qDebug() << "on_month_combo_4_currentIndexChanged end";
}
void Widget::loadBrandData(const QString BrandName)
{
    QStringList monthNames = {"January", "February", "March", "April", "May", "June",
                              "July", "August", "September", "October", "November", "December"};
    int currentIndex = ui->month_combo_6->currentIndex();
    QString selectedMonth = monthNames[currentIndex];
    QString prevMonth = currentIndex > 0 ? monthNames[currentIndex - 1] : "";

    // 현재 및 전월 데이터 조회
    QMap<QString, qint64> currentMonthSales;
    QMap<QString, qint64> prevMonthSales;
    qint64 totalSales = 0, totalPrevSales = 0;

    QSqlQuery query;
    QString queryString = QString("SELECT Model, \"%1\", \"%2\" FROM allCar WHERE BrandName='%3'")
                              .arg(selectedMonth, prevMonth, BrandName); // %3로 BrandName을 포함시킴
    if (!query.exec(queryString)) {
        qDebug() << "쿼리 실행 에러:" << query.lastError();
        return;
    }

    while (query.next()) {
        QString model = query.value(0).toString();
        qint64 sales = query.value(1).toInt();
        qint64 prevSales = query.value(2).toInt();
        currentMonthSales[model] = sales;
        prevMonthSales[model] = prevSales;
        totalSales += sales;
        totalPrevSales += prevSales;
    }

    // 판매량으로 내림차순 정렬
    QVector<QPair<QString, qint64>> sortedSales;
    for (auto it = currentMonthSales.begin(); it != currentMonthSales.end(); ++it) {
        sortedSales.append(qMakePair(it.key(), it.value()));
    }
    std::sort(sortedSales.begin(), sortedSales.end(), [](const QPair<QString, qint64>& a, const QPair<QString, qint64>& b) {
        return a.second > b.second;
    });

    // 테이블 위젯에 데이터 추가
    ui->tableWidget_9->clearContents();
    ui->tableWidget_9->setRowCount(0);
    for (auto &pair : sortedSales) {
        if (pair.second > 0) { // 판매량이 0보다 클 때만 표시
            double marketShare = totalSales > 0 ? static_cast<double>(pair.second) / totalSales * 100 : 0;
            int row = ui->tableWidget_9->rowCount();
            ui->tableWidget_9->insertRow(row);
            ui->tableWidget_9->setItem(row, 0, new QTableWidgetItem(pair.first));
            ui->tableWidget_9->setItem(row, 1, new QTableWidgetItem(QString::number(pair.second)));
            ui->tableWidget_9->setItem(row, 2, new QTableWidgetItem(QString::number(marketShare, 'f', 2) + "%"));
        }
    }


    // 현재 월의 총 판매량 업데이트
    ui->sales_rate->setText(QString::number(totalSales) + "대"); // 현재 월의 총 판매량 표시

    // 전월 대비 판매량 차이 계산 및 표시
    qint64 salesDifference = totalSales - totalPrevSales;

    // 1월 선택 시 이전 월 데이터가 없으므로 "데이터 없음" 표시
    if (currentIndex == 0) { // 1월이 선택되었을 경우
        ui->pre_month->setText("데이터 없음");
    } else {
        // 전월 판매량과 증가 또는 감소 기호, 차이량을 표시하는 문자열 생성
        QString preMonthText = QString::number(totalPrevSales) + "대 "; // 전월의 총 판매량을 먼저 표시

        // 차이가 0보다 크면 증가(▲), 작으면 감소(▼) 표시
        if (salesDifference > 0) {
            // 증가량 추가 (파란색 문구로 표시)
            preMonthText += "<span style=\"color:blue;\">▲ " + QString::number(salesDifference) + "</span>";
        } else if (salesDifference < 0) {
            // 감소량 추가 (빨간색 문구로 표시, 양수로 변환하여 표시)
            preMonthText += "<span style=\"color:red;\">▼ " + QString::number(-salesDifference) + "</span>";
        }

        ui->pre_month->setText(preMonthText);

    }
}

//세부브랜드별 기간별 선택 함수
void Widget::on_period_find_6_clicked() {

    QString startYear = ui->period_year1_6->currentText();
    QString endYear = ui->period_year2_6->currentText();
    int startMonthIndex = ui->period_month1_6->currentIndex(); // 시작 월 인덱스
    int endMonthIndex = ui->period_month2_6->currentIndex(); // 종료 월 인덱스

    if(startMonthIndex > endMonthIndex)
    {
        QMessageBox::warning(this, "알림", "범위를 다시 설정해주세요.");
    }
    else
    {
        // 선택된 기간에 대한 데이터 로드 및 표시 함수 호출
        loadPeriodModelDataToTableWidget(ui->tableWidget_10, startYear, startMonthIndex, endYear, endMonthIndex);

        // 라벨을 숨기는 대신 가시성을 변경
        ui->label_12->clear();
        ui->label_13->clear();
        ui->brand_logo->clear();
        ui->sales_rate->clear();
        ui->pre_month->clear();
    }
}
//세부브랜드별 기간별 테이블위젯 업데이트 함수
void Widget::loadPeriodModelDataToTableWidget(QTableWidget* tableWidget, const QString& startYear, int startMonthIndex, const QString& endYear, int endMonthIndex) {

    ui->stackedWidget_4->setCurrentIndex(3);
    QStringList monthNames = {"January", "February", "March", "April", "May", "June",
                              "July", "August", "September", "October", "November", "December"};

    QMap<QString, qint64> modelSales;
    for (int monthIndex = startMonthIndex; monthIndex <= endMonthIndex; ++monthIndex) {
        QString month = monthNames[monthIndex];
        QString queryStr = QString("SELECT Model, SUM(\"%1\") FROM allCar WHERE BrandName = '%2' GROUP BY Model")
                               .arg(month, currentBrand);
        QSqlQuery query;
        if (!query.exec(queryStr)) {
            qDebug() << "쿼리 실행 에러:" << query.lastError();
            return;
        }

        while (query.next()) {
            QString model = query.value(0).toString();
            qint64 sales = query.value(1).toInt();
            modelSales[model] += sales;
        }
    }
    qint64 totalSales = 0;
    QVector<QPair<QString, qint64>> sortedModels;
    for (auto it = modelSales.begin(); it != modelSales.end(); ++it) {
        totalSales += it.value();
        sortedModels.append(qMakePair(it.key(), it.value()));
    }

    // 판매량에 따라 모델을 내림차순으로 정렬
    std::sort(sortedModels.begin(), sortedModels.end(), [](const QPair<QString, qint64>& a, const QPair<QString, qint64>& b) {
        return a.second > b.second;
    });

    tableWidget->clearContents();
    tableWidget->setRowCount(0);
    tableWidget->setColumnCount(3); // 모델명, 판매량, 점유율

    QStringList headers = {"모델명", "판매량", "점유율"};
    tableWidget->setHorizontalHeaderLabels(headers);

    // 정렬된 데이터를 테이블 위젯에 추가
    for (const auto& pair : sortedModels) {
        double marketShare = totalSales > 0 ? (100.0 * pair.second / totalSales) : 0;
        int row = tableWidget->rowCount();
        tableWidget->insertRow(row);
        tableWidget->setItem(row, 0, new QTableWidgetItem(pair.first));
        tableWidget->setItem(row, 1, new QTableWidgetItem(QString::number(pair.second)));
        tableWidget->setItem(row, 2, new QTableWidgetItem(QString::number(marketShare, 'f', 2) + "%"));
    }


}

void Widget::on_kia_clicked()
{
    currentBrand = "KIA"; // 현재 선택된 브랜드를 "Hyundai"로 설정
    ui->stackedWidget_4->setCurrentIndex(2);
    ui->stackedWidget_3->setCurrentIndex(1);
    // 브랜드 로고 이미지 설정
    QPixmap brandLogo("C:/Users/iot/Downloads/DB/danawa/KIA.png");
    ui->brand_logo->setPixmap(brandLogo.scaled(ui->brand_logo->size(), Qt::KeepAspectRatio, Qt::SmoothTransformation));
    loadBrandData(currentBrand); // 오타 수정: 함수 이름을 정확하게 맞춤
}



void Widget::on_link_clicked()
{
    QDesktopServices::openUrl(QUrl("https://mauto.danawa.com/newcar/?Work=record&Tab=Top10&Month=2024-01-00&MonthTo="));
}


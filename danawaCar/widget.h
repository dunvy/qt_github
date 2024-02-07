#ifndef WIDGET_H
#define WIDGET_H

#include <QWidget>
#include <QDebug>
#include <QtSql>
#include <QFileInfo>
#include <QMessageBox>
#include <QTableWidget>
#include <QMovie>

#include <QDesktopServices>
#include <QUrl>

#include <QSqlError> // QSqlError 사용을 위해 추가
#include <QTableWidgetItem> // QTableWidgetItem 사용을 위해 추가


QT_BEGIN_NAMESPACE
namespace Ui {
class Widget;
}
QT_END_NAMESPACE

class Widget : public QWidget
{
    Q_OBJECT

public:
    Widget(QWidget *parent = nullptr);
    ~Widget();

private slots:
    void go_joinPage();                 // 회원가입 페이지 이동

    void LOGIN_SUCCESS();               // 로그인 버튼

    void dupl_id();                     // 아이디 중복
    void dupl_email();                  // 이메일 중복

    void btn_find();                    // ID/PW 찾기 페이지 이동
    void find_ID();                     // ID 찾기 버튼
    void find_PW();                     // PW 찾기 버튼

    void JOIN_SUCCESS();                // 회원가입

    void go_startPage();                // 시작 페이지 이동


    void AllTap();                      // 전체탭 클릭

    void modelTap();                    // 모델별탭 클릭
    void selectMonth();                 // 모델별탭_월 선택탭
    void selectPeriod();                // 모델별탭_기간 선택탭

    void MonthTop();                    // 전월대비
    void MainSelectPeriod_Model();      // 기간 선택

    void CarType();                     // 차종별
    void CarType_Month();               // 차종별 월 선택
    void on_type_pre_clicked();         // 차종별 탭 1페이지
    void on_type_next_clicked();        // 차종별 탭 2페이지

    void brand_top3_domestic(); //브랜드 TOP3
    void find_brand(); // 브랜드 TOP3 기간조회
    void on_tapBrand_currentChanged(int index); // 브랜드 국산 수입 탭

    void CarType_Period();              // 차종별 기간 선택




    void on_mainTap_brand_clicked(); // 브랜드별 버튼 클릭
    void on_month_combo_2_currentIndexChanged(int index);
    void loadBrandDataToTableWidget(QTableWidget* tableWidget,QStringList& brandTables, const QString& selectedMonthName, QLabel* totalSalesLabel);
    void loadPeriodDataToTableWidget(QTableWidget* tableWidget, const QString& startYear, int startMonthIndex, const QString& endYear, int endMonthIndex, bool isDomestic);
    void on_period_find_2_clicked();
    void on_hyundai_clicked();
    void on_BMWButton_clicked();
    void loadBrandData(const QString BrandName);

    void updateKoreaDayLabel();
    void updateincome_dayLabel();
    void on_month_combo_6_currentIndexChanged(int index);

    void on_period_find_6_clicked();
    void loadPeriodModelDataToTableWidget(QTableWidget* tableWidget, const QString& startYear, int startMonthIndex, const QString& endYear, int endMonthIndex);


    void on_kia_clicked();

    void on_genesis_clicked();

    void on_KG_clicked();

    void on_chevolet_clicked();

    void on_benz_clicked();

    void on_honda_clicked();

    void on_porsche_clicked();

    void on_audi_clicked();

    void on_link_clicked();

private:
    Ui::Widget *ui;

    QSqlDatabase myDB;

    QString car, currentBrand;

    void input_clear();

};
#endif // WIDGET_H

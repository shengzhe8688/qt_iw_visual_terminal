#include "mainwindow.h"
#include "ui_mainwindow.h"
#include "qthread.h"
#include "qmessagebox.h"
#include "qdebug.h"
#include "dialog_about.h"
#include "communication.h"

MainWindow::MainWindow(QWidget *parent) :
    QMainWindow(parent),
    ui(new Ui::MainWindow)
{
    ui->setupUi(this);
    /*添加端口*/
    ui->port_list->addItem("COM1");
    ui->port_list->addItem("COM2");
    ui->port_list->addItem("COM3");
    ui->port_list->addItem("COM4");
    ui->port_list->addItem("COM5");
    ui->port_list->addItem("COM6");
    ui->port_list->addItem("COM7");
    ui->port_list->addItem("COM8");
    ui->port_list->addItem("COM9");
    ui->port_list->addItem("COM10");
    ui->port_list->addItem("COM11");
    ui->port_list->addItem("COM12");
    ui->port_list->addItem("COM13");
    ui->port_list->addItem("COM14");
    ui->port_list->setCurrentIndex(0);

    /*添加波特率*/
    ui->baudrate_list->addItem("115200");
    ui->baudrate_list->addItem("57600");
    ui->baudrate_list->addItem("19200");
    ui->baudrate_list->addItem("9600");
    ui->baudrate_list->setCurrentIndex(0);

    /*添加数据位*/
    ui->data_bits_list->addItem("8");
    ui->data_bits_list->addItem("7");
    ui->data_bits_list->setCurrentIndex(0);


    /*添加校验类型*/
    ui->parity_list->addItem("无校验");
    ui->parity_list->addItem("奇校验");
    ui->parity_list->addItem("偶校验");
    ui->parity_list->setCurrentIndex(0);

    ui->display_1->display("------");
    ui->display_2->display("------");
    ui->display_3->display("------");
    ui->display_4->display("------");

    QRegExp regExp("[0-9]{0,2}");
    ui->set_temperature_edit->setValidator(new QRegExpValidator(regExp, this));
    ui->set_temperature_edit->setText(QString::number(6));

    opened = false;

    QThread *comm_thread = new QThread(0);

    comm = new communication(0);

    comm->moveToThread(comm_thread);
    comm->m_serial->moveToThread(comm_thread);

    comm_thread->start();


     /*-从UI到comm--*/
    QObject::connect(this,SIGNAL(req_open_serial_port(QString ,int ,int ,int )),comm,SLOT(handle_open_serial_port_req(QString ,int ,int ,int )));
    QObject::connect(this,SIGNAL(req_close_serial_port(QString)),comm,SLOT(handle_close_serial_port_req(QString)));

    QObject::connect(this,SIGNAL(req_tare(int)),comm,SLOT(handle_tare_req(int)));
    QObject::connect(this,SIGNAL(req_calibration(int,int)),comm,SLOT(handle_calibration_req(int,int)));

    QObject::connect(this,SIGNAL(req_unlock()),comm,SLOT(handle_req_unlock()));
    QObject::connect(this,SIGNAL(req_lock()),comm,SLOT(handle_req_lock()));

    QObject::connect(this,SIGNAL(req_set_temperature(int)),comm,SLOT(handle_set_temperature(int)));



    /*-从comm到UI--*/
    QObject::connect(comm,SIGNAL(rsp_open_serial_port_result(int )),this,SLOT(handle_open_serial_port_result(int )));
    QObject::connect(comm,SIGNAL(rsp_close_serial_port_result(int )),this,SLOT(handle_close_serial_port_result(int )));

    QObject::connect(comm,SIGNAL(rsp_query_weight_result(int ,int ,int,int,int,int)),this,SLOT(handle_query_weight_result(int ,int,int,int,int ,int )));
    QObject::connect(comm,SIGNAL(rsp_tare_result(int,int)),this,SLOT(handle_tare_result(int,int)));
    QObject::connect(comm,SIGNAL(rsp_calibration_result(int,int,int)),this,SLOT(handle_calibration_result(int,int,int)));
    QObject::connect(comm,SIGNAL(rsp_set_temperature_result(int)),this,SLOT(handle_set_temperature_result(int)));

    QObject::connect(comm,SIGNAL(rsp_unlock_result(int)),this,SLOT(handle_unlock_result(int)));
    QObject::connect(comm,SIGNAL(rsp_lock_result(int)),this,SLOT(handle_lock_result(int)));

    QObject::connect(comm,SIGNAL(rsp_query_door_status(int,QString)),this,SLOT(handle_rsp_query_door_status(int,QString)));
    QObject::connect(comm,SIGNAL(rsp_query_lock_status(int,QString)),this,SLOT(handle_rsp_query_lock_status(int,QString)));
    QObject::connect(comm,SIGNAL(rsp_query_temperature_result(int,int,int)),this,SLOT(handle_rsp_query_temperature_result(int,int,int)));
    QObject::connect(comm,SIGNAL(rsp_query_weight_layer_result(int,int)),this,SLOT(handle_rsp_query_weight_layer_result(int,int)));
    QObject::connect(comm,SIGNAL(rsp_query_fw_vrersion_result(int,int)),this,SLOT(handle_rsp_query_fw_vrersion_result(int,int)));

}

MainWindow::~MainWindow()
{
    delete ui;
}

/*串口打开操作结果*/
void MainWindow::handle_open_serial_port_result(int result)
{
    if (result == 0) {
        opened = true;
        ui->open_button->setText("关闭");
        qDebug("串口打开成功！");
        ui->port_list->setEnabled(false);
        ui->data_bits_list->setEnabled(false);
        ui->baudrate_list->setEnabled(false);
        ui->parity_list->setEnabled(false);

    } else {
        QMessageBox::warning(this,"错误",ui->port_list->currentText() + "打开失败！",QMessageBox::Ok);
    }
}

/*串口关闭操作结果*/
void MainWindow::handle_close_serial_port_result(int result)
{
    if (result == 0) {
        opened = false;
        ui->open_button->setText("打开");
        qDebug("串口关闭成功！");

        ui->display_1->display("------");
        ui->display_2->display("------");
        ui->display_3->display("------");
        ui->display_4->display("------");

        ui->port_list->setEnabled(true);
        ui->data_bits_list->setEnabled(true);
        ui->baudrate_list->setEnabled(true);
        ui->parity_list->setEnabled(true);

        ui->door_status_display->setStyleSheet("color:black");
        ui->lock_status_display->setStyleSheet("color:black");
        ui->temperature_display->setStyleSheet("color:black");
        ui->temperature_setting_display->setStyleSheet("color:black");
        ui->weight_layer_display->setStyleSheet("color:black");

        ui->door_status_display->setText("未知");
        ui->lock_status_display->setText("未知");
        ui->temperature_display->setText("未知");
        ui->temperature_setting_display->setText("未知");
        ui->weight_layer_display->setText("未知");

    } else {
        QMessageBox::warning(this,"错误",ui->port_list->currentText() + "关闭失败！",QMessageBox::Ok);
    }
}

/*开锁*/
void MainWindow::handle_unlock_result(int result)
{
    if (result == 0) {
       QMessageBox::information(this,"成功","开锁成功！",QMessageBox::Ok);
    } else {
       QMessageBox::warning(this,"错误","开锁失败！",QMessageBox::Ok);
    }

}

/*关锁*/
void MainWindow::handle_lock_result(int result)
{
    if (result == 0) {
       QMessageBox::information(this,"成功","关锁成功！",QMessageBox::Ok);
    } else {
       QMessageBox::warning(this,"错误","关锁失败！",QMessageBox::Ok);
    }

}

/*净重结果显示*/
void MainWindow::handle_query_weight_result(int result,int level,int weight1,int weight2,int weight3,int weight4)
{
    /*成功读取*/
    if (level == 0 && result == 0) {
        if((int16_t)weight1 != -1) {
            ui->display_1->display((int16_t)weight1);
        } else {
            ui->display_1->display("err");
        }
        if((int16_t)weight2 != -1) {
            ui->display_2->display((int16_t)weight2);
        } else {
            ui->display_2->display("err");
        }
        if((int16_t)weight3 != -1) {
            ui->display_3->display((int16_t)weight3);
        } else {
            ui->display_3->display("err");
        }
        if((int16_t)weight4 != -1) {
            ui->display_4->display((int16_t)weight4);
        } else {
            ui->display_4->display("err");
        }
    } else {
        ui->display_1->display("err");
        ui->display_2->display("err");
        ui->display_3->display("err");
        ui->display_4->display("err");
    }


}


/*去皮结果显示*/

void MainWindow::handle_tare_result(int level,int result)
{
    if (result == 0) {
       QMessageBox::information(this,"成功","第" + QString::number( level)+ "层去皮成功！",QMessageBox::Ok);
    } else {
       QMessageBox::warning(this,"错误","第" + QString::number( level)+ "层去皮失败！",QMessageBox::Ok);
    }

}


/*校准结果显示*/
void MainWindow::handle_calibration_result(int level,int calibration_weight,int result)
{
    if (result == 0) {
       QMessageBox::information(this,"成功","第" + QString::number( level)+ "层" + QString::number(calibration_weight) +"校准成功！",QMessageBox::Ok);
    } else {
       QMessageBox::warning(this,"错误","第" + QString::number( level)+ "层" + QString::number(calibration_weight) +"校准失败！",QMessageBox::Ok);
    }

}



/*点击打开或者关闭按键*/
void MainWindow::on_open_button_clicked()
{
    if (opened) {
        /*发送关闭串口信号*/
        emit req_close_serial_port(ui->port_list->currentText());
    } else {

        /*发送打开串口信号*/
        emit req_open_serial_port(ui->port_list->currentText(),ui->baudrate_list->currentText().toInt(),ui->data_bits_list->currentText().toInt(),ui->parity_list->currentIndex());
    }
}

/*第4层去皮*/
void MainWindow::on_tare_button_4_clicked()
{
    if (opened == false) {
        QMessageBox::warning(this,"错误","串口没有打开！",QMessageBox::Ok);
        return;
    }

    /*发送去皮信号*/
    emit req_tare(4);

}
/*第3层去皮*/
void MainWindow::on_tare_button_3_clicked()
{
    if (opened == false) {
        QMessageBox::warning(this,"错误","串口没有打开！",QMessageBox::Ok);
        return;
    }
    /*发送去皮信号*/
    emit req_tare(3);
}

/*第2层去皮*/
void MainWindow::on_tare_button_2_clicked()
{
    if (opened == false) {
        QMessageBox::warning(this,"错误","串口没有打开！",QMessageBox::Ok);
        return;
    }
    /*发送去皮信号*/
    emit req_tare(2);
}
/*第1层去皮*/
void MainWindow::on_tare_button_1_clicked()
{
    if (opened == false) {
        QMessageBox::warning(this,"错误","串口没有打开！",QMessageBox::Ok);
        return;
    }
    /*发送去皮信号*/
    emit req_tare(1);
}


/*第4层0点校准*/
void MainWindow::on_calibration_zero_button_4_clicked()
{
    if (opened == false) {
        QMessageBox::warning(this,"错误","串口没有打开！",QMessageBox::Ok);
        return;
    }
    /*发送校准信号*/
    emit req_calibration(4,0);
}

/*第3层0点校准*/
void MainWindow::on_calibration_zero_button_3_clicked()
{
    if (opened == false) {
        QMessageBox::warning(this,"错误","串口没有打开！",QMessageBox::Ok);
        return;
    }
    /*发送校准信号*/
    emit req_calibration(3,0);
}

/*第2层0点校准*/
void MainWindow::on_calibration_zero_button_2_clicked()
{
    if (opened == false) {
        QMessageBox::warning(this,"错误","串口没有打开！",QMessageBox::Ok);
        return;
    }
    /*发送校准信号*/
    emit req_calibration(2,0);
}

/*第1层0点校准*/
void MainWindow::on_calibration_zero_button_1_clicked()
{
    if (opened == false) {
        QMessageBox::warning(this,"错误","串口没有打开！",QMessageBox::Ok);
        return;
    }
    /*发送校准信号*/
    emit req_calibration(1,0);
}

/*第4层2000g校准*/
void MainWindow::on_calibration_2000_button_4_clicked()
{
    if (opened == false) {
        QMessageBox::warning(this,"错误","串口没有打开！",QMessageBox::Ok);
        return;
    }
    /*发送校准信号*/
    emit req_calibration(4,2000);
}

/*第3层2000g校准*/
void MainWindow::on_calibration_2000_button_3_clicked()
{
    if (opened == false) {
        QMessageBox::warning(this,"错误","串口没有打开！",QMessageBox::Ok);
        return;
    }
    /*发送校准信号*/
    emit req_calibration(3,2000);
}
/*第2层2000g校准*/
void MainWindow::on_calibration_2000_button_2_clicked()
{
    if (opened == false) {
        QMessageBox::warning(this,"错误","串口没有打开！",QMessageBox::Ok);
        return;
    }
    /*发送校准信号*/
    emit req_calibration(2,2000);
}

/*第1层2000g校准*/
void MainWindow::on_calibration_2000_button_1_clicked()
{
    if (opened == false) {
        QMessageBox::warning(this,"错误","串口没有打开！",QMessageBox::Ok);
        return;
    }
    /*发送校准信号*/
    emit req_calibration(1,2000);
}

/*第4层5000g校准*/
void MainWindow::on_calibration_5000_button_4_clicked()
{
    if (opened == false) {
        QMessageBox::warning(this,"错误","串口没有打开！",QMessageBox::Ok);
        return;
    }
    /*发送校准信号*/
    emit req_calibration(4,5000);
}
/*第3层5000g校准*/
void MainWindow::on_calibration_5000_button_3_clicked()
{
    if (opened == false) {
        QMessageBox::warning(this,"错误","串口没有打开！",QMessageBox::Ok);
        return;
    }
    /*发送校准信号*/
    emit req_calibration(3,5000);
}
/*第2层5000g校准*/
void MainWindow::on_calibration_5000_button_2_clicked()
{
    if (opened == false) {
        QMessageBox::warning(this,"错误","串口没有打开！",QMessageBox::Ok);
        return;
    }
    /*发送校准信号*/
    emit req_calibration(2,5000);
}
/*第1层5000g校准*/
void MainWindow::on_calibration_5000_button_1_clicked()
{
    if (opened == false) {
        QMessageBox::warning(this,"错误","串口没有打开！",QMessageBox::Ok);
        return;
    }
    /*发送校准信号*/
    emit req_calibration(1,5000);
}

void MainWindow::on_all_on_top_check_button_stateChanged(int arg1)
{
    (void)arg1;
    if (ui->all_on_top_check_button->isChecked()) {
        setWindowFlags(windowFlags() | Qt::WindowStaysOnTopHint);
    } else {
        setWindowFlags(windowFlags() & ~Qt::WindowStaysOnTopHint);
    }
    this->show();
}


/*处理回应的查询门状态*/
void MainWindow::handle_rsp_query_door_status(int rc,QString status)
{
    if (rc == 0) {
        ui->door_status_display->setStyleSheet("color:blue");
    } else {
        ui->door_status_display->setStyleSheet("color:red");
    }

    ui->door_status_display->setText(status);
}

/*处理回应的查询锁状态*/
void MainWindow::handle_rsp_query_lock_status(int rc,QString status)
{
    if (rc == 0) {
        ui->lock_status_display->setStyleSheet("color:blue");
    } else {
        ui->lock_status_display->setStyleSheet("color:red");
    }

    ui->lock_status_display->setText(status);
}

/*处理回应的查询温度状态*/
void MainWindow::handle_rsp_query_temperature_result(int rc,int temperature_setting,int temperature)
{
    if (rc == 0) {
        if (temperature != 0x7f) {
            ui->temperature_display->setStyleSheet("color:blue");
            ui->temperature_display->setNum(temperature);
        } else {
            ui->temperature_display->setStyleSheet("color:red");
            ui->temperature_display->setText("错误");
        }

        ui->temperature_setting_display->setStyleSheet("color:blue");
        ui->temperature_setting_display->setNum(temperature_setting);

    } else {
        ui->temperature_display->setStyleSheet("color:red");
        ui->temperature_setting_display->setStyleSheet("color:red");
        ui->temperature_display->setText("错误");
        ui->temperature_setting_display->setText("错误");
    }

}
/*处理回应的查询称重单元数量状态*/
void MainWindow::handle_rsp_query_weight_layer_result(int rc,int weight_layer)
{
    if (rc == 0) {
        ui->weight_layer_display->setStyleSheet("color:blue");
        ui->weight_layer_display->setNum(weight_layer);
    } else {
        ui->weight_layer_display->setStyleSheet("color:red");
        ui->weight_layer_display->setText("错误");
    }
}

/*处理回应的查询称重单元数量状态*/
void MainWindow::handle_rsp_query_fw_vrersion_result(int rc,int fw_version)
{
    if (rc == 0) {
        ui->fw_version_display->setStyleSheet("color:blue");
        ui->fw_version_display->setText(" v" + QString::number((fw_version >> 16) & 0xff) + "."+ QString::number((fw_version >> 8) & 0xff) + "." + QString::number(fw_version & 0xff));
    } else {
        ui->fw_version_display->setStyleSheet("color:red");
        ui->fw_version_display->setText("错误");
    }
}


void MainWindow::on_open_lock_button_clicked()
{
    if (opened == false) {
        QMessageBox::warning(this,"错误","串口没有打开！",QMessageBox::Ok);
        return;
    }
    emit req_unlock();
}

void MainWindow::on_close_lock_button_clicked()
{
    if (opened == false) {
        QMessageBox::warning(this,"错误","串口没有打开！",QMessageBox::Ok);
        return;
    }
    emit req_lock();
}



void MainWindow::on_actionabout_triggered()
{
    Dialog_about about(this);
    about.setWindowTitle("关于");
    about.exec();
}

void MainWindow::on_set_temperature_button_clicked()
{
    int temperature;
    if (opened == false) {
        QMessageBox::warning(this,"错误","串口没有打开！",QMessageBox::Ok);
        return;
    }

    if (!ui->set_temperature_edit->text().isEmpty()) {
       temperature = ui->set_temperature_edit->text().toInt();
    }

    emit req_set_temperature(temperature);
}

void MainWindow::handle_set_temperature_result(int rc)
{
    if (rc == 0) {
       QMessageBox::information(this,"成功","设置温度成功！",QMessageBox::Ok);
    } else {
       QMessageBox::warning(this,"失败","设置温度失败！",QMessageBox::Ok);
    }

}

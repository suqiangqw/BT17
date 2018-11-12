#include "dialog.h"
#include "ui_dialog.h"
#include <QMessageBox>

#include "UserDefine.h"

void CALLBACK DisConnectFunc(LLONG lLoginID, char *pchDVRIP, LONG nDVRPort, LDWORD dwUser)
{
    Dialog * pThis = (Dialog *)dwUser;
    if(NULL == pThis)
    {
        return;
    }
    //deal
    QEvent*  eventDisconnect = new QEvent(QEvent::User);
    QApplication::postEvent((QObject*)pThis, eventDisconnect);
    return;
}

Dialog::Dialog(QWidget *parent) :
    QDialog(parent),
    ui(new Ui::Dialog)
{
    ui->setupUi(this);
    InitData();
}

Dialog::~Dialog()
{
    delete ui;
    CLIENT_Cleanup();
}



bool Dialog::InitData()
{
    CLIENT_Init(DisConnectFunc, (LDWORD)this);
    installEventFilter(this);//install filter
    m_counter = new QTimer( this );
    m_lLoginHandle = 0;
    m_nCurDiskType = 0;
    memset(&m_DiskInfo, 0, sizeof(DH_HARDDISK_STATE));
    ui->lineEdit3->setEchoMode(QLineEdit::Password);
    ui->lineEdit3->setMaxLength(8);
    ui->lineEdit4->setMaxLength(8);
    QDate date = QDate::currentDate();
    QTime time = QTime::currentTime();
    QDateTime curDateTime(date, time);
    ui->DVRdateTime->setDisplayFormat("yyyy.MM.dd hh:mm:ss");
    ui->DVRdateTime->setDateTime(curDateTime);

    ui->Login->setEnabled( TRUE );
    ui->Logout->setEnabled( FALSE );
    ui->ButBoot->setEnabled( FALSE );
    ui->ButAlarmInStart->setEnabled( FALSE );
    ui->ButAlarmInStop->setEnabled( FALSE );
    ui->ButAlarmOutStart->setEnabled( FALSE );
    ui->ButAlarmOutStop->setEnabled( FALSE );
    ui->ButGetTime->setEnabled( FALSE );
    ui->ButSetTime->setEnabled( FALSE );
    ui->ButDiskControl->setEnabled( FALSE );
    return true;
}


bool Dialog::UnInitData()
{
    if(m_counter)
         delete m_counter;
    CLIENT_Cleanup();
    return true;
}
void Dialog::on_Login_clicked()
{
        QString strIp;
        QString strUserName;
        QString strDvrPort;
        QString strPassword;

        strIp = ui->lineEdit1->text();
        strUserName = ui->lineEdit4->text();
        strPassword = ui->lineEdit3->text();
        strDvrPort = ui->lineEdit2->text();

        NET_DEVICEINFO deviceInfo = {0};
        int error = 0;

        //get nunber
        bool numok = false;
        long lPort = strDvrPort.toLong(&numok);
        if(numok)
        {
            m_lLoginHandle = CLIENT_Login((char*)strIp.toUtf8().data(), lPort, (char*)strUserName.toUtf8().data(),(char*)strPassword.toUtf8().data(),  &deviceInfo, &error);
            if(m_lLoginHandle == 0)
            {
                if(error != 255)
                {
                    QMessageBox::about(this, tr("Prompt"), tr("Login failed!"));
                }
                else
                {
                    error = CLIENT_GetLastError();
                    if(error ==NET_ERROR_MAC_VALIDATE_FAILED )
                    {
                        QMessageBox::about(this, tr("Prompt"), tr("Login failed! bad mac address"));
                    }
                    else if(error ==NET_ERROR_SENIOR_VALIDATE_FAILED)
                    {
                        QMessageBox::about(this, tr("Prompt"), tr("Login failed! senior validate failed"));
                    }
                }

            }
            else
            {
                ui->Login->setEnabled( FALSE );
                ui->Logout->setEnabled( TRUE );
                ui->ButBoot->setEnabled( TRUE );
                ui->ButAlarmInStart->setEnabled( TRUE );
                ui->ButAlarmInStop->setEnabled( TRUE );
                ui->ButAlarmOutStart->setEnabled( TRUE );
                ui->ButAlarmOutStop->setEnabled( TRUE );
                ui->ButGetTime->setEnabled( TRUE );
                ui->ButSetTime->setEnabled( TRUE );
                ui->ButDiskControl->setEnabled( TRUE );

                ui->ComboxChannelIn->clear();
                for(int i = 1; i <= deviceInfo.byAlarmInPortNum; i++)
                {
                    QString strdata = QString::number(i, 10);
                    int nCount = ui->ComboxChannelIn->count();
                    ui->ComboxChannelIn->insertItem(nCount,strdata);
                }

                ui->ComboxChannelIn->setCurrentIndex(0);

                ui->ComboxChannelOut->clear();
                for(int j = 1; j <= deviceInfo.byAlarmOutPortNum; j++)
                {
                    QString strdata = QString::number(j, 10);
                    int nCount = ui->ComboxChannelOut->count();
                    ui->ComboxChannelOut->insertItem(nCount, strdata);
                }
                ui->ComboxChannelOut->setCurrentIndex(0);

                GetDiskInformation();
            }
        }
        else
        {
            int nRet = QMessageBox::warning(this, "Input Error",
                      "could not convert the port to numbers!\n"
                      "please try again!\n\n ",
                      "Retry",
                      "Quit",0,0,1);
            if (nRet == QMessageBox::Retry)
            {

            }
        }
}

void Dialog::on_Logout_clicked()
{
    if (m_lLoginHandle != 0)
    {
            BOOL bSuccess = CLIENT_Logout(m_lLoginHandle);
            if(bSuccess)
            {
                m_lLoginHandle = 0;
                m_nCurDiskType = 0;
                ui->Login->setEnabled( TRUE );
                ui->Logout->setEnabled( FALSE );
                ui->ButBoot->setEnabled( FALSE );
                ui->ButAlarmInStart->setEnabled( FALSE );
                ui->ButAlarmInStop->setEnabled( FALSE );
                ui->ButAlarmOutStart->setEnabled( FALSE );
                ui->ButAlarmOutStop->setEnabled( FALSE );
                ui->ButGetTime->setEnabled( FALSE );
                ui->ButSetTime->setEnabled( FALSE );
                ui->ButDiskControl->setEnabled( FALSE );

                ui->ComboxChannelIn->clear();
                ui->ComboxChannelOut->clear();
            }
            else
            {
                QMessageBox::about(this, tr("Prompt"), tr("Logout failed!"));
            }
     }
}

/// Net Trigger Alarm In
void Dialog::on_ButAlarmInStart_clicked()
{
    if(0 != m_lLoginHandle)
    {
           ALARMCTRL_PARAM alarmParam = {0};
           alarmParam.dwSize = sizeof(ALARMCTRL_PARAM);
           alarmParam.nAction = 1;
           alarmParam.nAlarmNo = ui->ComboxChannelIn->currentIndex();

           BOOL bSuccess = CLIENT_ControlDevice(m_lLoginHandle, DH_TRIGGER_ALARM_IN, &alarmParam);
           if(bSuccess)
           {
               //ButAlarmInStart->setEnabled( FALSE );
               //ButAlarmInStop->setEnabled( TRUE );
           }
           else
           {
               QMessageBox::about(this, tr("Promt"), tr("Trigger Alarm In Failed!"));
           }
    }
}

void Dialog::on_ButAlarmInStop_clicked()
{
    if(0 != m_lLoginHandle)
    {
            ALARMCTRL_PARAM alarmParam = {0};
            alarmParam.dwSize = sizeof(ALARMCTRL_PARAM);
            alarmParam.nAction = 0;
            alarmParam.nAlarmNo = ui->ComboxChannelIn->currentIndex();
            BOOL bSuccess = CLIENT_ControlDevice(m_lLoginHandle, DH_TRIGGER_ALARM_IN, &alarmParam);
            if(bSuccess)
            {
                //ButAlarmInStart->setEnabled( TRUE );
                //ButAlarmInStop->setEnabled( FALSE );
            }
            else
            {
                QMessageBox::about(this, tr("Promt"), tr("Stop Trigger Alarm In Failed!"));
            }
     }
}

///Reboot
void Dialog::on_ButBoot_clicked()
{
    if(0 != m_lLoginHandle)
    {
            BOOL bSuccess = CLIENT_ControlDevice(m_lLoginHandle, DH_CTRL_REBOOT, NULL);
            if(bSuccess)
            {
                QMessageBox::about(this, tr("Promt"), tr("Reboot Successful!"));
            }
            else
            {
                QMessageBox::about(this, tr("Promt"), tr("Reboot Failed!"));
            }
    }
}

///Calibrate DVR Time

void Dialog::on_ButGetTime_clicked()
{
    if(0 != m_lLoginHandle)
    {
            NET_TIME curDateTime;
            DWORD dwRet = 0;

            BOOL bSuccess = CLIENT_GetDevConfig(m_lLoginHandle, DH_DEV_TIMECFG, -1, &curDateTime, sizeof(NET_TIME), &dwRet);
            if(bSuccess && (dwRet == sizeof(NET_TIME)))
            {
                QDate date;
                QTime time;

                ConvertTimeToStr(curDateTime, &date, &time);
                QDateTime curDateTime(date, time);
                ui->DVRdateTime->setDateTime(curDateTime);
            }
            else
            {
                QMessageBox::about(this, tr("Promt"), tr("Get DateTime Failed!"));
            }
    }
}

void Dialog::on_ButSetTime_clicked()
{
    if(0 != m_lLoginHandle)
    {
           NET_TIME curDateTime;

           ConvertTime(ui->DVRdateTime->dateTime().date(), ui->DVRdateTime->dateTime().time(), &curDateTime);

           BOOL bSuccess = CLIENT_SetDevConfig(m_lLoginHandle, DH_DEV_TIMECFG, -1, &curDateTime, sizeof(NET_TIME), 1500);
           if(bSuccess)
           {
               QMessageBox::about(this, tr("Promt"), tr("Set DateTime Successful!"));
           }
           else
           {
               QMessageBox::about(this, tr("Promt"), tr("Set DateTime Failed!"));
           }
   }
}

/// Net Trigger Alarm Out

void Dialog::on_ButAlarmOutStart_clicked()
{
    if(0 != m_lLoginHandle)
    {
            ALARMCTRL_PARAM alarmParam = {0};
            alarmParam.dwSize = sizeof(ALARMCTRL_PARAM);
            alarmParam.nAction = 1;
            alarmParam.nAlarmNo = ui->ComboxChannelOut->currentIndex();

            BOOL bSuccess = CLIENT_ControlDevice(m_lLoginHandle, DH_TRIGGER_ALARM_OUT, &alarmParam);
            if(bSuccess)
            {
                //ButAlarmOutStart->setEnabled( FALSE );
                //ButAlarmOutStop->setEnabled( TRUE );
            }
            else
            {
                QMessageBox::about(this, tr("Promt"), tr("Trigger Alarm Out Failed!"));
            }
    }
}

void Dialog::on_ButAlarmOutStop_clicked()
{
    if(0 != m_lLoginHandle)
    {
            ALARMCTRL_PARAM alarmParam = {0};
            alarmParam.dwSize = sizeof(ALARMCTRL_PARAM);
            alarmParam.nAction = 0;
            alarmParam.nAlarmNo = ui->ComboxChannelOut->currentIndex();

            BOOL bSuccess = CLIENT_ControlDevice(m_lLoginHandle, DH_TRIGGER_ALARM_OUT, &alarmParam);
            if(bSuccess)
            {
                //ButAlarmOutStart->setEnabled( TRUE );
                //ButAlarmOutStop->setEnabled( FALSE );
            }
            else
            {
                QMessageBox::about(this, tr("Promt"), tr("Stop Trigger Alarm Out Failed!"));
            }
    }
}

/// Disk Control
void Dialog::on_ButDiskControl_clicked()
{
    if(0 != m_lLoginHandle)
    {
           DISKCTRL_PARAM diskParam;
           diskParam.dwSize = sizeof(DISKCTRL_PARAM);
           diskParam.nIndex = ui->ComboxDiskNo->currentIndex();

           BOOL bSuccess = 0;
           if (diskParam.nIndex >= 0 && diskParam.nIndex < m_DiskInfo.dwDiskNum)
          {
               diskParam.ctrlType = ui->ComboxOperateType->currentIndex();
               memcpy(&diskParam.stuDisk, &m_DiskInfo.stDisks[diskParam.nIndex], sizeof(diskParam.stuDisk));

               if(m_nCurDiskType == 0)
               {
                       bSuccess =  CLIENT_ControlDevice(m_lLoginHandle, DH_CTRL_DISK, &diskParam, 10000);
               }
               else
               {
                       bSuccess = CLIENT_ControlDevice(m_lLoginHandle, DH_CTRL_SDCARD, &diskParam, 10000);
               }

               if(bSuccess)
               {
                   QMessageBox::about(this, tr("Promt"), tr("Operate Successfully!"));
               }
               else
               {
                   QMessageBox::about(this, tr("Promt"), tr("Operate Failed!"));
               }
           }
   }
}


void Dialog::on_ComboxDiskNo_activated(int index)
{
        QString strdata = QString::number((int)m_DiskInfo.stDisks[index].dwVolume, 10);
        QString strVolume = QString("%1 %2").arg(tr("Volume:")).arg(strdata);

        strdata = QString::number((int)m_DiskInfo.stDisks[index].dwFreeSpace, 10);
        QString strFreeSpace = QString("%1 %2").arg(tr("Free Space:")).arg(strdata);

        QString strStatus = "";

        switch((int)m_DiskInfo.stDisks[index].dwStatus&0xF)
        {
        case 0:
            strStatus = QString("%1 %2").arg(tr("Status:")).arg(tr("Sleep"));
            break;
        case 1:
            strStatus = QString("%1 %2").arg(tr("Status:")).arg(tr("Active"));
            break;
        case 2:
            strStatus = QString("%1 %2").arg(tr("Status:")).arg(tr("Error"));
            break;
        default:
            break;
        }

        ui->TextDiskInfo ->setText(QString("%1 %2 %3").arg(strVolume).arg(strFreeSpace).arg(strStatus));
}

/// keyboard
void Dialog::on_ButNUM0_clicked()
{
    OperateKeyboard(DH_KEYBOARD_BTN0);
}

void Dialog::on_ButNUM1_clicked()
{
    OperateKeyboard(DH_KEYBOARD_BTN1);
}

void Dialog::on_ButPower_clicked()
{
    OperateKeyboard(DH_KEYBOARD_POWER);
}

void Dialog::on_ButNUM7_clicked()
{
    OperateKeyboard(DH_KEYBOARD_BTN7);
}

void Dialog::on_ButNUM2_clicked()
{
    OperateKeyboard(DH_KEYBOARD_BTN2);
}

void Dialog::on_ButNUM3_clicked()
{
    OperateKeyboard(DH_KEYBOARD_BTN3);
}

void Dialog::on_ButNUM4_clicked()
{
    OperateKeyboard(DH_KEYBOARD_BTN4);
}

void Dialog::on_ButNUM5_clicked()
{
    OperateKeyboard(DH_KEYBOARD_BTN5);
}

void Dialog::on_ButNUM6_clicked()
{
    OperateKeyboard(DH_KEYBOARD_BTN6);
}

void Dialog::on_ButNUM8_clicked()
{
    OperateKeyboard(DH_KEYBOARD_BTN8);
}

void Dialog::on_ButEsc_clicked()
{
    OperateKeyboard(DH_KEYBOARD_ESC);
}

void Dialog::on_ButNUM9_clicked()
{
    OperateKeyboard(DH_KEYBOARD_BTN9);
}

void Dialog::on_ButNUM10_clicked()
{
    OperateKeyboard(DH_KEYBOARD_BTN10);
}

void Dialog::on_ButNUM11_clicked()
{
    OperateKeyboard(DH_KEYBOARD_BTN11);
}

void Dialog::on_ButNUM12_clicked()
{
    OperateKeyboard(DH_KEYBOARD_BTN12);
}

void Dialog::on_ButNUM13_clicked()
{
    OperateKeyboard(DH_KEYBOARD_BTN13);
}

void Dialog::on_ButNUM14_clicked()
{
    OperateKeyboard(DH_KEYBOARD_BTN14);
}

void Dialog::on_ButNUM15_clicked()
{
    OperateKeyboard(DH_KEYBOARD_BTN15);
}

void Dialog::on_ButNUM16_clicked()
{
    OperateKeyboard(DH_KEYBOARD_BTN16);
}

void Dialog::on_ButFn1_clicked()
{
    OperateKeyboard(DH_KEYBOARD_FN1);
}

void Dialog::on_ButRec_clicked()
{
    OperateKeyboard(DH_KEYBOARD_REC);
}

void Dialog::on_ButPrew_clicked()
{
    OperateKeyboard(DH_KEYBOARD_PREW);
}

void Dialog::on_ButSlow_clicked()
{
    OperateKeyboard(DH_KEYBOARD_SLOW);
}

void Dialog::on_ButPlay_clicked()
{
    OperateKeyboard(DH_KEYBOARD_PLAY);
}

void Dialog::on_ButFast_clicked()
{
    OperateKeyboard(DH_KEYBOARD_FAST);
}

void Dialog::on_ButNext_clicked()
{
    OperateKeyboard(DH_KEYBOARD_NEXT);
}

void Dialog::on_ButStop_clicked()
{
    OperateKeyboard(DH_KEYBOARD_STOP);
}

void Dialog::on_ButJmpUp_clicked()
{
    OperateKeyboard(DH_KEYBOARD_JMPUP);
}

void Dialog::on_ButJmpDown_clicked()
{
    OperateKeyboard(DH_KEYBOARD_JMPDOWN);
}

void Dialog::on_ButFn2_clicked()
{
    OperateKeyboard(DH_KEYBOARD_FN2);
}

void Dialog::on_ButUP_clicked()
{
    OperateKeyboard(DH_KEYBOARD_UP);
}

void Dialog::on_ButLeft_clicked()
{
    OperateKeyboard(DH_KEYBOARD_LEFT);
}

void Dialog::on_ButEnter_clicked()
{
    OperateKeyboard(DH_KEYBOARD_ENTER);
}

void Dialog::on_ButRight_clicked()
{
    OperateKeyboard(DH_KEYBOARD_RIGHT);
}

void Dialog::on_ButDown_clicked()
{
    OperateKeyboard(DH_KEYBOARD_DOWN);
}

void Dialog::on_ButNine_clicked()
{
    OperateKeyboard(DH_KEYBOARD_NINE);
}

void Dialog::on_ButOne_clicked()
{
    OperateKeyboard(DH_KEYBOARD_ONE);
}

void Dialog::on_ButSplit_clicked()
{
    OperateKeyboard(DH_KEYBOARD_SPLIT);
}

void Dialog::on_ButInfo_clicked()
{
    OperateKeyboard(DH_KEYBOARD_INFO);
}

void Dialog::on_ButAddr_clicked()
{
    OperateKeyboard(DH_KEYBOARD_ADDR);
}

void Dialog::GetDiskInformation()
{
    if( 0 != m_lLoginHandle)
    {
        DH_HARDDISK_STATE diskInfo = {0};
        DH_SDCARD_STATE sdInfo = {0};
        int nRetLen = 0;
        DWORD  dwDiskNum = 0;
        BOOL bSuccess = CLIENT_QueryDevState(m_lLoginHandle, DH_DEVSTATE_DISK, (char *)&diskInfo, sizeof(DH_HARDDISK_STATE), &nRetLen);
        if(bSuccess && (nRetLen == sizeof(DH_HARDDISK_STATE)))
        {
                m_nCurDiskType = 0;
        }
        else
        {
                bSuccess =  CLIENT_QueryDevState(m_lLoginHandle, DH_DEVSTATE_SD_CARD, (char*)&sdInfo, sizeof(DH_SDCARD_STATE), &nRetLen, 1000);
                if(bSuccess && (nRetLen == sizeof(DH_HARDDISK_STATE)))
                {
                        m_nCurDiskType = 1;
                }
                else
                {
                        m_nCurDiskType = 0;
                }

        }
        if(bSuccess)
        {
            if(m_nCurDiskType == 0)
            {
                dwDiskNum = diskInfo.dwDiskNum;
            }
            else
            {
                dwDiskNum = sdInfo.dwDiskNum;
            }


           ui->ComboxDiskNo->clear();

            DWORD dwDiskNum = diskInfo.dwDiskNum;
            for(int i=0; i<(int)dwDiskNum; i++)
            {
                QString str = QString::number(i+1, 10);
                int nCount = ui->ComboxDiskNo->count();
                ui->ComboxDiskNo->insertItem(nCount, str);
            }
            ui->ComboxDiskNo->setCurrentIndex(0);

            memcpy(&m_DiskInfo, &diskInfo, sizeof(DH_HARDDISK_STATE));
            on_ComboxDiskNo_activated(0);
        }
        else
        {
            QMessageBox::about(this, tr("Promt"), tr("Fail to get disk information!"));
        }
    }
}

void Dialog::ConvertTime( QDate date, QTime time, NET_TIME * nettime)
{
    NET_TIME netTime = {0};
    netTime.dwYear = date.year();
    netTime.dwMonth = date.month();
    netTime.dwDay = date.day();
    netTime.dwHour = time.hour();
    netTime.dwMinute = time.minute();
    netTime.dwSecond =time.second();

    *nettime = netTime;
}


void Dialog::ConvertTimeToStr(NET_TIME nettime, QDate *date, QTime *time)
{
    //date->setYMD(nettime.dwYear, nettime.dwMonth, nettime.dwDay);
    //time->setHMS(nettime.dwHour, nettime.dwMinute, nettime.dwSecond);
}

void Dialog::OperateKeyboard( CtrlType nType)
{
    if(0 != m_lLoginHandle)
    {
        BOOL bSuccess = CLIENT_ControlDevice(m_lLoginHandle, nType, NULL);
        if(!bSuccess)
        {
            QMessageBox::about(this, tr("Promt"), tr("Operate Failed!"));
        }
    }
}

bool Dialog::eventFilter(QObject *obj, QEvent *event)//message filter
{
    if(event->type() == QEvent::User)
    {
        QString strDevInfo;
        strDevInfo.append("Disconnect!");
        QMessageBox::information(0, "Promt", strDevInfo);
    }
    return QWidget::eventFilter(obj, event);//don't eat event
}

void Dialog::on_ButBoot_2_clicked()
{
    int nVer = CLIENT_GetSDKVersion();
	QString strTmpVer = QString::number(nVer, 10);
	QString strVer = strTmpVer.left(1) + "." + strTmpVer.right(strTmpVer.length()-1).left(2) + "." + strTmpVer.right(strTmpVer.length()-3);
	QMessageBox::information(0, "Promt", strVer);
}

#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <QMainWindow>
#include <QDateTime>
#include "MQTTClient.h"

#define ADDRESS     "tcp://192.168.0.177:1883"
#define CLIENTID    "QTClient"
#define AUTHMETHOD  "rageobi"
#define AUTHTOKEN   "llll"
#define TOPIC       "ee513/#"
#define PAYLOAD     "Hello World!"
#define QOS         1
#define TIMEOUT     10000L

namespace Ui {
class MainWindow;
}

class MainWindow : public QMainWindow
{
    Q_OBJECT
public:
    explicit MainWindow(QWidget *parent = 0);
    ~MainWindow();
private slots:
    void on_connectButton_clicked();
    void on_disconnectButton_clicked();
    void on_MQTTmessage(QString message);
    void on_comboBox_currentTextChanged(const QString &arg1);
    void on_topicEdit_editingFinished();

signals:
    void messageSignal(QString message);

private:
    Ui::MainWindow *ui;
    void update();
    void setGraph();
    double pitch,roll,temp,yData;
    int flag=0;
    QString timeData,dataSelected="Tempreature",yLabel="Tempreature (celsius)";
    MQTTClient client;

    volatile MQTTClient_deliveryToken deliveredtoken;

    friend void delivered(void *context, MQTTClient_deliveryToken dt);
    friend int msgarrvd(void *context, char *topicName, int topicLen, MQTTClient_message *message);
    friend void connlost(void *context, char *cause);
};

void delivered(void *context, MQTTClient_deliveryToken dt);
int  msgarrvd(void *context, char *topicName, int topicLen, MQTTClient_message *message);
void connlost(void *context, char *cause);

#endif // MAINWINDOW_H


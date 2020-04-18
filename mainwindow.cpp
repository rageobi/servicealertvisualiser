#include "mainwindow.h"
#include "ui_mainwindow.h"
#include<QDebug>

MainWindow *handle;
MainWindow::MainWindow(QWidget *parent) :
    QMainWindow(parent),
    ui(new Ui::MainWindow)
{       ui->setupUi(this);
        this->timeData="00:00:00";
        this->setWindowTitle("EE513 Assignment 2");
        setGraph();
        ui->topicEdit->setText(TOPIC);
        QObject::connect(this, SIGNAL(messageSignal(QString)),
                         this, SLOT(on_MQTTmessage(QString)));
        ::handle = this;

}

MainWindow::~MainWindow()
{
    delete ui;
}
void MainWindow::setGraph(){

    this->ui->customPlot->setInteraction(QCP::iRangeZoom,true);
    this->ui->customPlot->setInteraction(QCP::iRangeDrag,true);
    //this->ui->customPlot->axisRect()->setRangeDrag(Qt::Horizontal);
    this->ui->customPlot->axisRect()->setRangeDrag(Qt::Vertical);
     this->ui->customPlot->axisRect()->setRangeZoom(Qt::Horizontal);
    this->ui->customPlot->addGraph()->setName("IsLine");

    if(this->dataSelected=="Tempreature"){
        this->yLabel="Tempreature (celsius)";
    }
    else if(this->dataSelected=="Pitch"){
        this->yLabel="Pitch (degrees)";
    }
    else{
        this->yLabel="Roll (degrees)";
    }
    qDebug()<<"YLabel is"<<yLabel;
    this->ui->customPlot->yAxis->setLabel(yLabel);

    QSharedPointer<QCPAxisTickerTime> timeTicker(new QCPAxisTickerTime);
    timeTicker->setTimeFormat("%h:%m:%s");
    this->ui->customPlot->xAxis->setTicker(timeTicker);
    this->ui->customPlot->yAxis->setRange(-5,5);
    ui->customPlot->replot();

}
void MainWindow::update(){
    // For more help on real-time plots, see: http://www.qcustomplot.com/index.php/demos/realtimedatademo

    QTime time =QTime::fromString(timeData,"hh:mm:ss");
    if(this->flag ==0){
        double tSeconds= time.second()+time.minute()*60+time.hour()*3600;
        this->ui->customPlot->xAxis->setRange(tSeconds-500,tSeconds+3000);
        this->ui->customPlot->yAxis->setRange(yData*3/5,yData*1.5);this->flag=1;
    }
    qDebug()<<"Debug time is"<<time;
    double xData = time.second()+time.minute()*60+time.hour()*3600; // time elapsed since start of demo, in seconds
    ui->customPlot->graph(0)->addData(xData,yData);
    ui->customPlot->graph(0)->rescaleKeyAxis(true);
    ui->customPlot->replot();
    QString text = QString("The %2 value is %1").arg(QString::number(this->yData),this->dataSelected);
    ui->outputEdit->setText(text);
}


void MainWindow::on_connectButton_clicked()
{   if(ui->topicEdit->text()==""){
        ui->topicEdit->setFocus(Qt::FocusReason::OtherFocusReason);
        QPalette pal = ui->topicEdit->palette();
        pal.setColor(QPalette::Text,Qt::red);
        ui->topicEdit->setPalette(pal);
        ui->topicEdit->setText("Please fill this section");
    }
    else{
    MQTTClient_connectOptions opts = MQTTClient_connectOptions_initializer;
    int rc;this->yData = 0.00;
    MQTTClient_create(&client, ADDRESS, CLIENTID, MQTTCLIENT_PERSISTENCE_NONE, NULL);
    opts.keepAliveInterval = 20;
    opts.cleansession = 1;
    opts.username = AUTHMETHOD;
    opts.password = AUTHTOKEN;

    if (MQTTClient_setCallbacks(client, NULL, connlost, msgarrvd, delivered)==0){
        ui->outputText->appendPlainText(QString("Callbacks set correctly"));
    }
    if ((rc = MQTTClient_connect(client, &opts)) != MQTTCLIENT_SUCCESS) {
        ui->outputText->appendPlainText(QString("Failed to connect, return code %1").arg(rc));
    }
    ui->outputText->appendPlainText(QString("Subscribing to topic %1 for client " CLIENTID).arg(ui->topicEdit->text()));
    int x = MQTTClient_subscribe(client, ui->topicEdit->text().toLatin1().data(), QOS);
    ui->outputText->appendPlainText(QString("Result of subscribe is %1 (0=success)").arg(x));
    }
}

void delivered(void *context, MQTTClient_deliveryToken dt) {
    (void)context;
    // Please don't modify the Window UI from here
    qDebug() << "Message delivery confirmed";
    handle->deliveredtoken = dt;
}

/* This is a callback function and is essentially another thread. Do not modify the
 * main window UI from here as it will cause problems. Please see the Slot method that
 * is directly below this function. To ensure that this method is thread safe I had to
 * get it to emit a signal which is received by the slot method below */
int msgarrvd(void *context, char *topicName, int topicLen, MQTTClient_message *message) {
    (void)context; (void)topicLen;
    qDebug() << "Message arrived (topic is " << topicName << ")";
    qDebug() << "Message payload length is " << message->payloadlen;
    QString payload;
    payload.sprintf("%s", (char *) message->payload).truncate(message->payloadlen);
    emit handle->messageSignal(payload);
    MQTTClient_freeMessage(&message);
    MQTTClient_free(topicName);
    return 1;
}

/** This is the slot method. Do all of your message received work here. It is also safe
 * to call other methods on the object from this point in the code */
void MainWindow::on_MQTTmessage(QString payload){

    ui->outputText->appendPlainText(payload);
    ui->outputText->ensureCursorVisible();
    QJsonDocument json = QJsonDocument::fromJson(payload.toUtf8());
    QJsonObject obj = json.object();
    QJsonValue value=obj.value(QString("d"));
    QJsonObject data = value.toObject();

     this->pitch=data["pitch"].toDouble();
     this->roll=data["roll"].toDouble();
     this->temp=data["temp"].toDouble();
     this->timeData=data["time"].toString();

     qDebug()<<"time"<<timeData;
     qDebug()<<"pitch"<<pitch;
     qDebug()<<"roll"<<roll;
     qDebug()<<"temp"<<temp;
     if(dataSelected=="Tempreature"){
        this->yData=this->temp;
     }
     else if(dataSelected=="Pitch"){
       this->yData=this->pitch;
     }
     else{
       this->yData=this->roll;
}
     this->update();
}

void connlost(void *context, char *cause) {
    (void)context; (void)*cause;
    // Please don't modify the Window UI from here
    qDebug() << "Connection Lost" << endl;
}

void MainWindow::on_disconnectButton_clicked()
{
    qDebug() << "Disconnecting from the broker" << endl;
    MQTTClient_disconnect(client, 10000);
    //MQTTClient_destroy(&client);
}

void MainWindow::on_comboBox_currentTextChanged(const QString &arg1)
{
    qDebug()<<"Combobox val is:"<<arg1;
    dataSelected=arg1;
    this->flag=0;
    this->ui->customPlot->graph(0)->data().data()->clear();
    this->setGraph();
}

void MainWindow::on_topicEdit_editingFinished()
{
    ui->topicEdit->setFocus(Qt::FocusReason::OtherFocusReason);
    QPalette pal = ui->topicEdit->palette();
    pal.setColor(QPalette::Text,Qt::black);
    ui->topicEdit->setPalette(pal);
}


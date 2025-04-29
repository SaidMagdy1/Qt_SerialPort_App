#include "mainwindow.h"
#include "ui_mainwindow.h"

MainWindow::MainWindow(QWidget *parent)
    : QMainWindow(parent), ui(new Ui::MainWindow), serial(new QSerialPort(this)), settings("MyCompany", "SerialPortConfig") {
    ui->setupUi(this);

    // Load last used port
    // QString lastPort = settings.value("lastPort", "").toString();
    // if (!lastPort.isEmpty()) {
    //     ui->portComboBox->addItem(lastPort);
    // }

    updatePortList();
    connect(serial, &QSerialPort::readyRead, this, &MainWindow::readData);
    connect(serial, &QSerialPort::errorOccurred, this, &MainWindow::handleError);
}

MainWindow::~MainWindow() {
    delete ui;
}

void MainWindow::updatePortList() {
    const auto ports = QSerialPortInfo::availablePorts();
    for (const QSerialPortInfo &portInfo : ports) {
        ui->portComboBox->addItem(portInfo.portName());
    }
}

void MainWindow::on_connectButton_clicked() {
    if (serial->isOpen()) {
        serial->close();
        ui->connectButton->setText("Connect");
        ui->connectButton->setIcon(QIcon(":/Images/link_Black.svg"));
        ui->statusLabel->setText("Disconnected");
    } else {
        serial->setPortName(ui->portComboBox->currentText());
        if (serial->open(QIODevice::ReadWrite)) {
            settings.setValue("lastPort", serial->portName());
            ui->connectButton->setText("Disconnect");
            ui->connectButton->setIcon(QIcon(":/Images/link_off.svg"));
            ui->statusLabel->setText("Connected");
        } else {
            ui->lsLog->addItem("Failed to connect: " + serial->errorString());
        }
    }
}

void MainWindow::on_sendButton_clicked() {
    if (serial->isOpen()) {
        serial->write(ui->leSend->text().toUtf8());
    }
}

void MainWindow::readData()
{
    if (serial->isOpen()) {
        ui->lsLog->addItem( QString(serial->readAll()));
    }
}

void MainWindow::handleError(QSerialPort::SerialPortError error) {
    if (error == QSerialPort::ResourceError) {
        ui->lsLog->addItem("Serial port resource error: " + serial->errorString());
        serial->close();
        ui->connectButton->setText("Connect");
        ui->statusLabel->setText("Disconnected");
    }
}

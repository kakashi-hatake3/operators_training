#include "mainwindow.h"
#include <QPushButton>
#include <QLineEdit>
#include <QComboBox>
#include <QRadioButton>
#include <QTableWidget>
#include <QMenu>
#include <QMenuBar>
#include <QAction>
#include <QDialog>
#include <QLabel>
#include <QVBoxLayout>
#include <QInputDialog>
#include <QMessageBox>
#include <QXmlStreamWriter>
#include <QXmlStreamReader>
#include <QFile>
#include <QDateTime>
#include <QTimer>
#include <QDir>
#include <QCoreApplication>

MainWindow::MainWindow(QWidget *parent)
    : QMainWindow(parent)
{

    // Создание элементов управления
    testSelector = new QComboBox(this);
    testSelector->addItem("Test 1");
    testSelector->addItem("Test 2");
    testSelector->addItem("Test 3");

    button = new QPushButton("Button", this);
    lineEdit = new QLineEdit(this);
    comboBox = new QComboBox(this);
    radioButton = new QRadioButton("Option", this);
    tableWidget = new QTableWidget(3, 3, this);

    // Размещение элементов управления
    QWidget *centralWidget = new QWidget(this);
    QVBoxLayout *layout = new QVBoxLayout(centralWidget);
    layout->addWidget(testSelector);
    layout->addWidget(button);
    layout->addWidget(lineEdit);
    layout->addWidget(comboBox);
    layout->addWidget(radioButton);
    layout->addWidget(tableWidget);
    setCentralWidget(centralWidget);

    // Создание меню
    menu = menuBar()->addMenu("Menu");
    startAction = menu->addAction("Начать тренировку");
    finishAction = menu->addAction("Завершить тренировку");
    checkAction = menu->addAction("Проверить результат тренировки");
    speedAction = menu->addAction("Изменить скорость воспроизведения");

    // Подключение слотов к сигналам
    connect(startAction, &QAction::triggered, this, &MainWindow::startTraining);
    connect(finishAction, &QAction::triggered, this, &MainWindow::finishTraining);
    connect(checkAction, &QAction::triggered, this, &MainWindow::checkTrainingResult);

    connect(button, &QPushButton::clicked, [this]() {
        saveAction("Button clicked");
    });

    connect(lineEdit, &QLineEdit::textChanged, [this](const QString &text) {
        saveAction("LineEdit text changed: " + text);
    });

    connect(comboBox, QOverload<int>::of(&QComboBox::currentIndexChanged), [this](int index) {
        saveAction("ComboBox index changed: " + QString::number(index));
    });

    connect(radioButton, &QRadioButton::toggled, [this](bool checked) {
        saveAction("RadioButton toggled: " + QString::number(checked));
    });

    connect(tableWidget, &QTableWidget::cellChanged, [this](int row, int column) {
        QString cellValue = tableWidget->item(row, column)->text();
        saveAction("TableWidget cell changed: (" + QString::number(row) + ", " + QString::number(column) + ") " + cellValue);
    });

    connect(testSelector, QOverload<int>::of(&QComboBox::currentIndexChanged), this, &MainWindow::updateTestLayout);

    connect(speedAction, &QAction::triggered, this, &MainWindow::changePlaybackSpeed);

    setWidgetsEnabled(false);

}

void MainWindow::changePlaybackSpeed()
{
    bool ok;
    double speed = QInputDialog::getDouble(this, "Изменить скорость воспроизведения", "Введите коэффициент ускорения:", playbackSpeed, 0.1, 10.0, 1, &ok);
    if (ok) {
        playbackSpeed = speed;
    }
    clearFields();
}

void MainWindow::startTraining()
{
    clearFields();
    bool ok;
    QString user = QInputDialog::getText(this, "Начать тренировку", "Введите фамилию:", QLineEdit::Normal, "", &ok);
    if (ok && !user.isEmpty()) {
        setWidgetsEnabled(true);

        currentUser = user;
        actions.clear();
        timestamps.clear();
    }
}

void MainWindow::finishTraining()
{
    if (!currentUser.isEmpty()) {
        QString testName = testSelector->currentText();
        QString xmlFolderPath = QCoreApplication::applicationDirPath() + "/../training_results/" + testName + "/";
        QDir xmlFolder(xmlFolderPath);
        if (!xmlFolder.exists()) {
            xmlFolder.mkpath(".");
        }

        QString filename = xmlFolderPath + currentUser + ".xml";
        saveActionsToXml(filename);
        QMessageBox::information(this, "Завершить тренировку", "Тренировка завершена. Результаты сохранены в файл " + filename);
        currentUser.clear();
        clearFields();
        setWidgetsEnabled(false);
    }
}

void MainWindow::checkTrainingResult()
{
    QString testName = testSelector->currentText();
    QString xmlFolderPath = QCoreApplication::applicationDirPath() + "/../training_results/" + testName + "/";
    QDir xmlFolder(xmlFolderPath);
    if (!xmlFolder.exists()) {
        QMessageBox::warning(this, "Проверить результат тренировки", "Папка " + testName + " не найдена.");
        return;
    }

    QStringList xmlFiles = xmlFolder.entryList(QStringList() << "*.xml", QDir::Files);
    QStringList users;
    foreach (const QString &file, xmlFiles) {
        QString user = file.left(file.length() - 4);
        users.append(user);
    }

    bool ok;
    QString selectedUser = QInputDialog::getItem(this, "Проверить результат тренировки", "Выберите студента:", users, 0, false, &ok);
    if (ok && !selectedUser.isEmpty()) {
        QString filename = xmlFolderPath + selectedUser + ".xml";
        replayActions(filename);
    }
}

void MainWindow::saveActionsToXml(const QString &filename)
{
    QFile file(filename);
    if (file.open(QIODevice::WriteOnly)) {
        QXmlStreamWriter writer(&file);
        writer.setAutoFormatting(true);
        writer.writeStartDocument();
        writer.writeStartElement("actions");

        for (int i = 0; i < actions.size(); ++i) {
            writer.writeStartElement("action");
            writer.writeAttribute("timestamp", timestamps[i]);
            writer.writeCharacters(actions[i]);
            writer.writeEndElement();
        }

        writer.writeEndElement();
        writer.writeEndDocument();
        file.close();
    }
}

void MainWindow::replayActions(const QString &filename)
{
    QFile file(filename);
    if (file.open(QIODevice::ReadOnly)) {
        QXmlStreamReader reader(&file);
        QStringList replayActions;
        QStringList replayTimestamps;

        while (!reader.atEnd()) {
            reader.readNext();
            if (reader.isStartElement() && reader.name().toString() == "action") {
                QString timestamp = reader.attributes().value("timestamp").toString();
                QString action = reader.readElementText();
                replayActions.append(action);
                replayTimestamps.append(timestamp);
            }
        }

        file.close();

        int totalDuration = 0;
        for (int i = 0; i < replayActions.size(); ++i) {
            QDateTime currentTimestamp = QDateTime::fromString(replayTimestamps[i], Qt::ISODate);
            QDateTime prevTimestamp = (i > 0) ? QDateTime::fromString(replayTimestamps[i - 1], Qt::ISODate) : currentTimestamp;
            int duration = prevTimestamp.msecsTo(currentTimestamp) / playbackSpeed;
            totalDuration += duration;


            QTimer::singleShot(totalDuration, [=]() {
                // Воспроизведение действия
                if (replayActions[i].startsWith("Button clicked")) {
                    button->animateClick();
                } else if (replayActions[i].startsWith("LineEdit text changed:")) {
                    QString text = replayActions[i].mid(22).trimmed();
                    lineEdit->setText(text);
                } else if (replayActions[i].startsWith("ComboBox index changed:")) {
                    QString indexStr = replayActions[i].mid(24).trimmed();
                    int index = indexStr.toInt();
                    comboBox->setCurrentIndex(index);
                } else if (replayActions[i].startsWith("RadioButton toggled:")) {
                    QString checkedStr = replayActions[i].mid(21).trimmed();
                    bool checked = (checkedStr == "1");
                    radioButton->setChecked(checked);
                } else if (replayActions[i].startsWith("TableWidget cell changed:")) {
                    QString cellStr = replayActions[i].mid(27).trimmed();
                    QList<QString> cellCoords = cellStr.split(")")[0].split(", ");

                    if (cellCoords.size() == 2) {
                        int row = cellCoords[0].trimmed().toInt();
                        int column = cellCoords[1].trimmed().toInt();
                        QString cellValue = cellStr.split(") ")[1];
                        tableWidget->setItem(row, column, new QTableWidgetItem(cellValue));
                    }
                }
            });
        }
        // Вызов диалогового окна после завершения воспроизведения
        QTimer::singleShot(totalDuration, [=]() {
            QMessageBox::information(this, "Воспроизведение завершено", "Воспроизведение записи завершено.");
        });
    }
}

void MainWindow::saveAction(const QString &action)
{
    if (!currentUser.isEmpty()) {
        QString timestamp = QDateTime::currentDateTime().toString(Qt::ISODate);
        actions.append(action);
        timestamps.append(timestamp);
    }
}

void MainWindow::clearFields()
{
    lineEdit->clear();
    comboBox->setCurrentIndex(0);
    radioButton->setChecked(false);
    tableWidget->clearContents();
}

void MainWindow::updateTestLayout(int index)
{
    setWidgetsEnabled(true);

    clearFields();

    // Очистка текущего расположения виджетов
    QWidget *centralWidget = this->centralWidget();
    QLayout *layout = centralWidget->layout();
    while (layout->count() > 1) {
        QLayoutItem *item = layout->takeAt(1);
        if (item->widget()) {
            item->widget()->setParent(nullptr);
        }
        delete item;
    }// Обновление расположения виджетов в соответствии с выбранным тестом
    switch (index) {
    case 0:
        // Тест 1
        layout->addWidget(button);
        layout->addWidget(lineEdit);
        layout->addWidget(comboBox);
        layout->addWidget(radioButton);
        layout->addWidget(tableWidget);
        break;
    case 1:
        // Тест 2
        layout->addWidget(lineEdit);
        layout->addWidget(button);
        layout->addWidget(tableWidget);
        layout->addWidget(comboBox);
        layout->addWidget(radioButton);
        break;
    case 2:
        // Тест 3
        layout->addWidget(radioButton);
        layout->addWidget(tableWidget);
        layout->addWidget(lineEdit);
        layout->addWidget(comboBox);
        layout->addWidget(button);
        break;
    }
    setWidgetsEnabled(false);
}

void MainWindow::setWidgetsEnabled(bool enabled)
{
    button->setEnabled(enabled);
    lineEdit->setEnabled(enabled);
    comboBox->setEnabled(enabled);
    radioButton->setEnabled(enabled);
    tableWidget->setEnabled(enabled);
}

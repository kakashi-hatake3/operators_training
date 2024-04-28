// Подключение заголовочных файлов
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
#include <QHeaderView>

// Конструктор MainWindow
MainWindow::MainWindow(QWidget *parent)
    : QMainWindow(parent)
{
    // Установка размеров окна
    setFixedSize(900, 500);

    // Установка названия программы
    setWindowTitle("Тренажер");

    // Создание элементов управления
    testSelector = new QComboBox(this);
    testSelector->addItem("Test 1");
    testSelector->addItem("Test 2");
    testSelector->addItem("Test 3");

    button1 = new QPushButton("Button", this);
    button2 = new QPushButton("Button", this);
    lineEdit = new QLineEdit(this);
    comboBox = new QComboBox(this);
    radioButton1 = new QRadioButton("Option", this);
    radioButton2 = new QRadioButton("Option", this);
    radioButton3 = new QRadioButton("Option", this);
    tableWidget = new QTableWidget(3, 2, this);

    // Размещение элементов управления
    QWidget *centralWidget = new QWidget(this);
    QVBoxLayout *layout = new QVBoxLayout(centralWidget);
    layout->addWidget(testSelector);
    layout->addWidget(comboBox);

    layout->addWidget(button1);
    layout->addWidget(button2);

    layout->addWidget(lineEdit);
    layout->addWidget(radioButton1);
    layout->addWidget(radioButton2);
    layout->addWidget(radioButton3);

    layout->addWidget(tableWidget);
    setCentralWidget(centralWidget);

    // Установка одинаковой ширины для всех столбцов
    int columnCount = tableWidget->columnCount();
    for (int i = 0; i < columnCount; i++) {
        tableWidget->setColumnWidth(i, 440);
    }

    // Установка одинаковой высоты для всех строк
    int rowCount = tableWidget->rowCount();
    for (int i = 0; i < rowCount; i++) {
        tableWidget->setRowHeight(i, 50);
    }

    // Создание меню
    menu = menuBar()->addMenu("Menu");
    startAction = menu->addAction("Начать тренировку");
    finishAction = menu->addAction("Завершить тренировку");
    checkAction = menu->addAction("Проверить результат тренировки");
    speedAction = menu->addAction("Изменить скорость воспроизведения");
    // Создание кнопки выхода
    exitAction = menu->addAction("Выход");
    connect(exitAction, &QAction::triggered, this, &QMainWindow::close);

    // Подключение слотов к сигналам
    connect(startAction, &QAction::triggered, this, &MainWindow::startTraining);
    connect(finishAction, &QAction::triggered, this, &MainWindow::finishTraining);
    connect(checkAction, &QAction::triggered, this, &MainWindow::checkTrainingResult);

    connect(button1, &QPushButton::clicked, [this]() {
        saveAction("Button1 clicked");
    });

    connect(button2, &QPushButton::clicked, [this]() {
        saveAction("Button2 clicked");
    });

    connect(lineEdit, &QLineEdit::textChanged, [this](const QString &text) {
        saveAction("LineEdit text changed: " + text);
    });

    connect(comboBox, QOverload<int>::of(&QComboBox::currentIndexChanged), [this](int index) {
        saveAction("ComboBox index changed: " + QString::number(index));
    });

    connect(radioButton1, &QRadioButton::toggled, [this](bool checked) {
        saveAction("RadioButton1 toggled: " + QString::number(checked));
    });

    connect(radioButton2, &QRadioButton::toggled, [this](bool checked) {
        saveAction("RadioButton2 toggled: " + QString::number(checked));
    });

    connect(radioButton3, &QRadioButton::toggled, [this](bool checked) {
        saveAction("RadioButton3 toggled: " + QString::number(checked));
    });

    connect(tableWidget, &QTableWidget::cellChanged, [this](int row, int column) {
        QString cellValue = tableWidget->item(row, column)->text();
        saveAction("TableWidget cell changed: |" + QString::number(row) + "+" + QString::number(column) + "|" + cellValue);
    });

    connect(testSelector, QOverload<int>::of(&QComboBox::currentIndexChanged), this, &MainWindow::updateTestLayout);

    connect(speedAction, &QAction::triggered, this, &MainWindow::changePlaybackSpeed);

    updateTestLayout(0);

    setWidgetsEnabled(false);
}

// Слот для изменения скорости воспроизведения
void MainWindow::changePlaybackSpeed()
{
    bool ok;
    double speed = QInputDialog::getDouble(this, "Изменить скорость воспроизведения", "Введите коэффициент ускорения:", playbackSpeed, 0.1, 10.0, 1, &ok);
    if (ok) {
        playbackSpeed = speed;
    }
    clearFields();
}

// Слот для начала тренировки
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

// Слот для завершения тренировки
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

// Слот для проверки результатов тренировки
void MainWindow::checkTrainingResult()
{
    QString testName = testSelector->currentText();
    QString xmlFolderPath

 = QCoreApplication::applicationDirPath() + "/../training_results/" + testName + "/";
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

// Метод для сохранения действий в XML-файл
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

// Метод для воспроизведения действий из XML-файла
void MainWindow::replayActions(const QString &filename)
{
    clearFields();
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
        // Блокирует меню и выбор тестов на время повтора
        menu->setEnabled(false);
        testSelector->setEnabled(false);
        int totalDuration = 0;
        for (int i = 0; i < replayActions.size(); ++i) {
            QDateTime currentTimestamp = QDateTime::fromString(replayTimestamps[i], Qt::ISODate);
            QDateTime prevTimestamp = (i > 0) ? QDateTime::fromString(replayTimestamps[i - 1], Qt::ISODate) : currentTimestamp;
            int duration = prevTimestamp.msecsTo(currentTimestamp) / playbackSpeed;
            totalDuration += duration;

            QTimer::singleShot(totalDuration, [=]() {
                // Воспроизведение действия
                if (replayActions[i].startsWith("Button1 clicked")) {
                    button1->setStyleSheet("background-color: green; color: white;");
                }
                else if (replayActions[i].startsWith("Button2 clicked")) {
                    button2->setStyleSheet("background-color: red; color: white;");
                }else if (replayActions[i].startsWith("LineEdit text changed:")) {
                    QString text = replayActions[i].mid(22).trimmed();
                    lineEdit->setText(text);
                } else if (replayActions[i].startsWith("ComboBox index changed:")) {
                    QString indexStr = replayActions[i].mid(24).trimmed();
                    int index = indexStr.toInt();
                    comboBox->setCurrentIndex(index);
                } else if (replayActions[i].startsWith("RadioButton1 toggled:")) {
                    QString checkedStr = replayActions[i].mid(21).trimmed();
                    bool checked = (checkedStr == "1");
                    radioButton1->setChecked(checked);
                } else if (replayActions[i].startsWith("RadioButton2 toggled:")) {
                    QString checkedStr = replayActions[i].mid(21).trimmed();
                    bool checked = (checkedStr == "1");
                    radioButton2->setChecked(checked);
                } else if (replayActions[i].startsWith("RadioButton3 toggled:")) {
                    QString checkedStr = replayActions[i].mid(21).trimmed();
                    bool checked = (checkedStr == "1");
                    radioButton3->setChecked(checked);
                } else if (replayActions[i].startsWith("TableWidget cell changed:")) {
                    QString cellStr = replayActions[i].mid(27).trimmed();
                    QList<QString> cellCoords = cellStr.split("|")[0].split("+");

                    if (cellCoords.size() == 2) {
                        int row = cellCoords[0].trimmed().toInt();
                        int column = cellCoords[1].trimmed().toInt();
                        QString cellValue = cellStr.split("|")[1];
                        tableWidget->setItem(row, column, new QTableWidgetItem(cellValue));
                    }
                }
            });
        }
        // Вызов диалогового окна после завершения воспроизведения
        QTimer::singleShot(totalDuration, [=]() {
            QMessageBox::information(this, "Воспроизведение завершено", "Воспроизведение записи завершено.");
            menu->setEnabled(true);
            testSelector->setEnabled(true);
        });
    }
}

// Метод для сохранения действия
void MainWindow::saveAction(const QString &action)
{
    if (!currentUser.isEmpty()) {
        QString timestamp = QDateTime::currentDateTime().toString(Qt::ISODate);
        actions.append(action);
        timestamps.append(timestamp);
    }
}

// Метод для очистки полей
void MainWindow::clearFields()
{
    lineEdit->clear();
    comboBox->setCurrentIndex(0);
    button1->setStyleSheet("background-color: white; color: green;");
    button2->setStyleSheet("background-color: white; color: red;");
    radioButton1->setAutoExclusive(false);
    radioButton1->setChecked(false);
    radioButton1->setAutoExclusive(true);
    radioButton2->setAutoExclusive(false);
    radioButton2->setChecked(false);
    radioButton2->setAutoExclusive(true);
    radioButton3->setAutoExclusive(false);
    radioButton3->setChecked(false);
    radioButton3->setAutoExclusive(true);

    // Очистка ответов из таблицы
    for (int i = 0; i < tableWidget->rowCount(); ++i) {
        QTableWidgetItem *item = tableWidget->item(i, 1);
        if (item) {
            item->setText("");
        }
    }
}

// Метод для обновления макета теста
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
            item->widget()->setParent(nullptr

);
        }
        delete item;
    }

    // Очистка выпадающего списка
    comboBox->clear();

    // Обновление расположения виджетов в соответствии с выбранным тестом
    switch (index) {
    case 0:{
        // Тест 1
        comboBox->addItem("Математический анализ");
        comboBox->addItem("Программирование");
        comboBox->addItem("Линейная алгебра");
        comboBox->addItem("Экономика");
        layout->addWidget(new QLabel("Какой ваш любимый предмет?"));
        layout->addWidget(comboBox);

        QWidget *buttonWidget1 = new QWidget;
        QHBoxLayout *buttonLayout1 = new QHBoxLayout(buttonWidget1);
        button1->setText("ДА");
        button1->setStyleSheet("background-color: white; color: green;");
        button2->setText("НЕТ");
        button2->setStyleSheet("background-color: white; color: red;");
        buttonLayout1->addWidget(button1);
        buttonLayout1->addWidget(button2);
        layout->addWidget(new QLabel("Вам нравится ходить на пары?"));
        layout->addWidget(buttonWidget1);

        layout->addWidget(new QLabel("Как зовут вашего любимого преподавателя?"));
        layout->addWidget(lineEdit);

        QWidget *radioWidget1 = new QWidget;
        QHBoxLayout *radioLayout1 = new QHBoxLayout(radioWidget1);
        layout->addWidget(new QLabel("Выберите как вы добираетесь до универа?"));
        radioButton1->setText("Метро");
        radioButton2->setText("Автобус");
        radioButton3->setText("Автомобиль");
        radioLayout1->addWidget(radioButton1);
        radioLayout1->addWidget(radioButton2);
        radioLayout1->addWidget(radioButton3);
        layout->addWidget(radioWidget1);

        layout->addWidget(new QLabel("Напишите, что вы думаете о каждом явлении."));
        tableWidget->setHorizontalHeaderLabels(QStringList() << "Вопрос" << "Ответ");
        tableWidget->verticalHeader()->hide();

        QTableWidgetItem *item1_1 = new QTableWidgetItem("Как вам еда в столовой универа?");
        item1_1->setFlags(Qt::ItemIsEditable);
        tableWidget->setItem(0, 0, item1_1);

        QTableWidgetItem *item2_1 = new QTableWidgetItem("Как вам состояние универа?");
        item2_1->setFlags(Qt::ItemIsEditable);
        tableWidget->setItem(1, 0, item2_1);

        QTableWidgetItem *item3_1 = new QTableWidgetItem("Как вам качество материала программы?");
        item3_1->setFlags(Qt::ItemIsEditable);
        tableWidget->setItem(2, 0, item3_1);

        layout->addWidget(tableWidget);
        break;
    }
    case 1:{
        // Тест 2
        comboBox->addItem("Блины");
        comboBox->addItem("Омлет");
        comboBox->addItem("Вафли");
        comboBox->addItem("Каша");
        layout->addWidget(new QLabel("Какой ваш любимый завтрак?"));
        layout->addWidget(comboBox);

        QWidget *buttonWidget2 = new QWidget;
        QHBoxLayout *buttonLayout2 = new QHBoxLayout(buttonWidget2);
        button1->setText("ДА");
        button1->setStyleSheet("background-color: white; color: green;");
        button2->setText("НЕТ");
        button2->setStyleSheet("background-color: white; color: red;");
        buttonLayout2->addWidget(button1);
        buttonLayout2->addWidget(button2);
        layout->addWidget(new QLabel("Вы бы съели всю пиццу в одиночку?"));
        layout->addWidget(buttonWidget2);

        layout->addWidget(new QLabel("Сколько яиц вы добавляете в омлет?"));
        layout->addWidget(lineEdit);

        QWidget *radioWidget2 = new QWidget;
        QHBoxLayout *radioLayout2 = new QHBoxLayout(radioWidget2);
        layout->addWidget(new QLabel("Выбирите, что вы бы никогда не съели?"));
        radioButton1->setText("Яблоко");
        radioButton2->setText("Оливки");
        radioButton3->setText("Чеснок");
        radioLayout2->addWidget(radioButton1);
        radioLayout2->addWidget(radioButton2);
        radioLayout2->addWidget(radioButton3);
        layout->addWidget(radioWidget2);

        layout->addWidget(new QLabel("Напишите, что вы думаете о каждом блюде."));
        tableWidget->setHorizontalHeaderLabels(QStringList() << "Блюдо" << "Ответ");
        tableWidget->verticalHeader()->hide();

        QTableWidgetItem *item1_2 = new QTableWidgetItem("Пицца");
        item1_2->setFlags(Qt::ItemIsEditable);
        tableWidget->setItem(0, 0, item1_2);

        QTableWidgetItem *item2_2 = new QTableWidgetItem("Гребешок");
        item2_2->setFlags(Qt::ItemIsEditable);
        tableWidget->setItem(1, 0, item2_2);

        QTableWidgetItem *item3_2 = new QTableWidgetItem("Холодец");
        item3_2->setFlags(Qt::ItemIsEditable);
        tableWidget->setItem(2, 0, item3_2);

        layout->addWidget(tableWidget);
        break;
    }
    case 2:{
        // Тест 3
        comboBox->addItem("Сидеть дома");
        comboBox->addItem("Гулять в парке");
        comboBox->addItem("Сходить в кино");
        comboBox->addItem("Посетить музей");
        layout->addWidget(new QLabel("Какой ваш любимый досуг?"));
        layout->addWidget(comboBox);

        QWidget *buttonWidget3 = new QWidget;
        QHBoxLayout *buttonLayout3 = new QHBoxLayout(buttonWidget3);
        button1->setText("ДА");
        button1->setStyleSheet("background-color: white; color: green;");
        button2->setText("НЕТ");
        button2->setStyleSheet("background-color: white; color: red;");
        buttonLayout3->addWidget(button1);
        buttonLayout3->addWidget(button2);
        layout->addWidget(new QLabel("Вам нравится выходить на улицу?"));
        layout->addWidget(buttonWidget3);

        layout->addWidget(new QLabel("Какое место на Земле вы бы хотели посетить больше всего?"));
        layout->addWidget(lineEdit);

        QWidget *radioWidget3 = new QWidget;
        QHBoxLayout *radioLayout3 = new QHBoxLayout(radioWidget3);
        layout->addWidget(new QLabel("Выберите на что вы бы точно не тратили свое время?"));
        radioButton1->setText("Футбол");
        radioButton2->setText("Книга");
        radioButton3->setText("Сериал");
        radioLayout3->addWidget(radioButton1);
        radioLayout3->addWidget(radioButton2);
        radioLayout3->addWidget(radioButton3);
        layout->addWidget(radioWidget3);

        layout->addWidget(new QLabel("Напишите, чем вы занимаетесь в свободное время."));
        tableWidget->setHorizontalHeaderLabels(QStringList() << "Вид деятельности" << "Ответ");
        tableWidget->verticalHeader()->hide();

        QTableWidgetItem *item1_3 = new QTableWidgetItem("Чтение");
        item1_3->setFlags(Qt::ItemIsEditable);
        tableWidget->setItem(0, 0, item1_3);

        QTableWidgetItem *item2_3 = new QTableWidgetItem("Футбол");
        item2_3->setFlags(Qt::ItemIsEditable);
        tableWidget->setItem(1, 0, item2_3);

        QTableWidgetItem *item3_3 = new QTableWidgetItem("Кино");
        item3_3->setFlags(Qt::ItemIsEditable);
        tableWidget->setItem(2, 0, item3_3);

        layout->addWidget(tableWidget);
        break;
    }
    default:
        break;
    }

    // Отключение элементов управления в начале тренировки
    setWidgetsEnabled(false);
}

// Метод для установки активности элементов управления
void MainWindow::setWidgetsEnabled(bool enabled)
{
    testSelector->setEnabled(enabled);
    comboBox->setEnabled(enabled);
    button1->setEnabled(enabled);
    button2->setEnabled(enabled);
    lineEdit->setEnabled(enabled);
    radioButton1->setEnabled(enabled);
    radioButton2->setEnabled(enabled);
    radioButton3->setEnabled(enabled);
    tableWidget->setEnabled(enabled);
}

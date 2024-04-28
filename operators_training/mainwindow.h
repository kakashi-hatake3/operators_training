#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <QMainWindow>
#include <QStringList>

class QPushButton;      // Объявление класса QPushButton для создания кнопок
class QLineEdit;        // Объявление класса QLineEdit для создания поля ввода
class QComboBox;        // Объявление класса QComboBox для создания комбо-бокса
class QRadioButton;     // Объявление класса QRadioButton для создания переключателей
class QTableWidget;     // Объявление класса QTableWidget для создания таблицы
class QMenu;            // Объявление класса QMenu для создания меню
class QAction;          // Объявление класса QAction для создания действий меню

class MainWindow : public QMainWindow  // Определение класса MainWindow, который является подклассом QMainWindow
{
    Q_OBJECT  // Макрос Q_OBJECT для мета-объектной системы Qt

public:
    MainWindow(QWidget *parent = nullptr);  // Конструктор класса

private slots:
    void startTraining();           // Слот для начала тренировки
    void finishTraining();          // Слот для завершения тренировки
    void checkTrainingResult();     // Слот для проверки результатов тренировки
    void saveAction(const QString &action);  // Слот для сохранения действия
    void changePlaybackSpeed();     // Слот для изменения скорости воспроизведения

private:
    QPushButton *button1;       // Указатель на кнопку 1
    QPushButton *button2;       // Указатель на кнопку 2
    QLineEdit *lineEdit;        // Указатель на поле ввода
    QComboBox *comboBox;        // Указатель на комбо-бокс
    QRadioButton *radioButton1; // Указатель на переключатель 1
    QRadioButton *radioButton2; // Указатель на переключатель 2
    QRadioButton *radioButton3; // Указатель на переключатель 3
    QTableWidget *tableWidget;  // Указатель на таблицу
    QMenu *menu;                // Указатель на меню
    QAction *startAction;       // Указатель на действие начала тренировки
    QAction *exitAction;        // Указатель на действие выхода
    QAction *finishAction;      // Указатель на действие завершения тренировки
    QAction *checkAction;       // Указатель на действие проверки результатов тренировки
    QComboBox *testSelector;    // Указатель на комбо-бокс выбора теста
    QAction *speedAction;       // Указатель на действие изменения скорости воспроизведения

    QString currentUser;        // Текущий пользователь
    QStringList actions;        // Список действий
    QStringList timestamps;     // Список меток времени

    void saveActionsToXml(const QString &filename);   // Метод для сохранения действий в XML-файл
    void replayActions(const QString &filename);      // Метод для воспроизведения действий из XML-файла
    void clearFields();                              // Метод для очистки полей
    void updateTestLayout(int index);                // Метод для обновления расположения элементов теста
    void setWidgetsEnabled(bool isEnable);           // Метод для установки активности элементов управления

    double playbackSpeed = 1.0;  // Скорость воспроизведения по умолчанию
};

#endif // MAINWINDOW_H

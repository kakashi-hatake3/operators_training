#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <QMainWindow>
#include <QStringList>

class QPushButton;
class QLineEdit;
class QComboBox;
class QRadioButton;
class QTableWidget;
class QMenu;
class QAction;

class MainWindow : public QMainWindow
{
    Q_OBJECT

public:
    MainWindow(QWidget *parent = nullptr);

private slots:
    void startTraining();
    void finishTraining();
    void checkTrainingResult();
    void saveAction(const QString &action);
    void changePlaybackSpeed();

private:
    QPushButton *button1;
    QPushButton *button2;
    QLineEdit *lineEdit;
    QComboBox *comboBox;
    QRadioButton *radioButton1;
    QRadioButton *radioButton2;
    QRadioButton *radioButton3;
    QTableWidget *tableWidget;
    QMenu *menu;
    QAction *startAction;
    QAction *exitAction;
    QAction *finishAction;
    QAction *checkAction;
    QComboBox *testSelector;
    QAction *speedAction;

    QString currentUser;
    QStringList actions;
    QStringList timestamps;

    void saveActionsToXml(const QString &filename);
    void replayActions(const QString &filename);
    void clearFields();
    void updateTestLayout(int index);
    void setWidgetsEnabled(bool isEnable);

    double playbackSpeed = 1.0;
};

#endif // MAINWINDOW_H

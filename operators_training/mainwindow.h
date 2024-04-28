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

private:
    QPushButton *button;
    QLineEdit *lineEdit;
    QComboBox *comboBox;
    QRadioButton *radioButton;
    QTableWidget *tableWidget;
    QMenu *menu;
    QAction *startAction;
    QAction *finishAction;
    QAction *checkAction;

    QString currentUser;
    QStringList actions;
    QStringList timestamps;

    void saveActionsToXml(const QString &filename);
    void replayActions(const QString &filename);
    void clearFields();
};

#endif // MAINWINDOW_H

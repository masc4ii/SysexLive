/*!
 * \file MainWindow.h
 * \author masc4ii
 * \copyright 2018
 * \brief The main window
 */

#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <QMainWindow>
#include "qmidiout.h"
#include "qmidimapper.h"
#include <QTableWidget>
#include <QRecentFilesMenu.h>

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
    void on_actionSearchInterfaces_triggered();
    void on_actionAboutSysexLive_triggered();
    void on_actionAboutQt_triggered();
    void on_actionAddEntry_triggered();
    void on_actionDeleteEntry_triggered();
    void on_tableWidget_doubleClicked(const QModelIndex &index);
    void on_actionSendPatches_triggered();
    void on_actionNew_triggered();
    void on_actionOpen_triggered();
    void on_actionSave_triggered();
    void on_comboBoxSynth1_activated(const QString &arg1);
    void on_comboBoxSynth2_activated(const QString &arg1);
    void on_actionMoveUp_triggered();
    void on_actionMoveDown_triggered();
    void loadFile(const QString &fileName);

private:
    Ui::MainWindow *ui;
    void getPorts(void);
    void searchSynths(void);
    void moveRow( bool up );
    void readSettings(void);
    void writeSettings(void);
    QList<QTableWidgetItem*>  takeRow( int row );
    void setRow(int row, const QList<QTableWidgetItem *> &rowItems);

    QRecentFilesMenu *m_recentFilesMenu;
    QString m_lastSaveFileName;
    QString m_synth1;
    QString m_synth2;
    QMidiOut *m_midiOut;
};

#endif // MAINWINDOW_H

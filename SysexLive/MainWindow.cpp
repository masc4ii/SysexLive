/*!
 * \file MainWindow.cpp
 * \author masc4ii
 * \copyright 2018
 * \brief The main window
 */

#include "MainWindow.h"
#include "ui_MainWindow.h"
#include <QDebug>
#include <QMessageBox>
#include <QBuffer>
#include <QFileInfo>
#include <QDir>
#include <QFileDialog>
#include <QXmlStreamWriter>
#include "DarkStyle.h"

#define APPNAME "SysexLive"
#define VERSION "0.2"

//Constructor
MainWindow::MainWindow(QWidget *parent) :
    QMainWindow(parent),
    ui(new Ui::MainWindow)
{
    ui->setupUi(this);

    //We dont want a context menu which could disable the menu bar
    setContextMenuPolicy(Qt::NoContextMenu);

    //Apply DarkStyle
#if QT_VERSION >= 0x050000
    CDarkStyle::assign();
#endif

    m_midiIn = new QMidiIn( this );
    m_midiOut = new QMidiOut( this );

    getPorts();

    m_lastSaveFileName = QDir::homePath();

    ui->plainTextEdit->setEnabled( false );

    //AutoResize for table columns
#if QT_VERSION >= 0x050000
    ui->tableWidget->horizontalHeader()->setSectionResizeMode(QHeaderView::Stretch);
#else
    ui->tableWidget->horizontalHeader()->setResizeMode( QHeaderView::Stretch );
#endif
    ui->tableWidget->hideColumn( 1 );
    ui->tableWidget->hideColumn( 2 );
    ui->tableWidget->hideColumn( 3 );
    ui->tableWidget->hideColumn( 4 );

    //Number of synths as group
    m_actionGroupSynths = new QActionGroup( this );
    m_actionGroupSynths->setExclusive( true );
    m_actionGroupSynths->addAction( ui->action2Synths );
    m_actionGroupSynths->addAction( ui->action4Synths );

    //RecentFilesMenu
    m_recentFilesMenu = new QRecentFilesMenu( tr( "Recent Files" ), ui->menuFile );
    readSettings();
    connect( m_recentFilesMenu, SIGNAL(recentFileTriggered(const QString &)), this, SLOT(loadFile(const QString &)) );
    ui->menuFile->insertMenu( ui->actionSave, m_recentFilesMenu );

    //Keyfilter on Table
    m_eventFilter = new EventReturnFilter( this );
    ui->tableWidget->installEventFilter( m_eventFilter );
}

//Destructor
MainWindow::~MainWindow()
{
    writeSettings();
    delete m_eventFilter;
    delete m_midiOut;
    if( ui->pushButtonListen->isChecked() ) ui->pushButtonListen->setChecked( false );
    delete m_midiIn;
    delete ui;
}

//Get Ports and write into combobox
void MainWindow::getPorts( void )
{
    bool portAvailable = true;
    ui->comboBoxInput->addItems( m_midiIn->getPorts() );
    ui->comboBoxSynth1->addItems( m_midiOut->getPorts() );
    ui->comboBoxSynth2->addItems( m_midiOut->getPorts() );
    ui->comboBoxSynth3->addItems( m_midiOut->getPorts() );
    ui->comboBoxSynth4->addItems( m_midiOut->getPorts() );

    //Block GUI if no port available
    if( ui->comboBoxSynth1->count() == 0 )
    {
        portAvailable = false;
        statusBar()->showMessage( tr( "No MIDI port found." ), 0 );
    }
    else
    {
        statusBar()->showMessage( tr( "" ), 0 );
    }
    ui->comboBoxInput->setEnabled( portAvailable );
    ui->comboBoxSynth1->setEnabled( portAvailable );
    ui->comboBoxSynth2->setEnabled( portAvailable );
    ui->comboBoxSynth3->setEnabled( portAvailable );
    ui->comboBoxSynth4->setEnabled( portAvailable );
    ui->pushButtonListen->setEnabled( portAvailable );
    ui->labelSynth1->setEnabled( portAvailable );
    ui->labelSynth2->setEnabled( portAvailable );
    ui->labelSynth3->setEnabled( portAvailable );
    ui->labelSynth4->setEnabled( portAvailable );
    ui->actionSendPatches->setEnabled( portAvailable );
}

//Connect ports which were saved in file
void MainWindow::searchSynths( void )
{
    //Close input port, if opened
    if( ui->pushButtonListen->isChecked() ) ui->pushButtonListen->setChecked( false );

    for( int i = 0; i < ui->comboBoxInput->count(); i++ )
    {
        if( ui->comboBoxInput->itemText( i ) == m_midiInput )
        {
            ui->comboBoxInput->setCurrentIndex( i );
            break;
        }
    }

    for( int i = 0; i < ui->comboBoxSynth1->count(); i++ )
    {
        if( ui->comboBoxSynth1->itemText( i ) == m_synth1 )
        {
            ui->comboBoxSynth1->setCurrentIndex( i );
            break;
        }
    }

    for( int i = 0; i < ui->comboBoxSynth2->count(); i++ )
    {
        if( ui->comboBoxSynth2->itemText( i ) == m_synth2 )
        {
            ui->comboBoxSynth2->setCurrentIndex( i );
            break;
        }
    }

    for( int i = 0; i < ui->comboBoxSynth3->count(); i++ )
    {
        if( ui->comboBoxSynth3->itemText( i ) == m_synth3 )
        {
            ui->comboBoxSynth3->setCurrentIndex( i );
            break;
        }
    }

    for( int i = 0; i < ui->comboBoxSynth4->count(); i++ )
    {
        if( ui->comboBoxSynth4->itemText( i ) == m_synth4 )
        {
            ui->comboBoxSynth4->setCurrentIndex( i );
            break;
        }
    }
}

//Find the ports
void MainWindow::on_actionSearchInterfaces_triggered()
{
    ui->comboBoxInput->clear();
    ui->comboBoxSynth1->clear();
    ui->comboBoxSynth2->clear();
    ui->comboBoxSynth3->clear();
    ui->comboBoxSynth4->clear();
    getPorts();
    searchSynths();
}

//About Box
void MainWindow::on_actionAboutSysexLive_triggered()
{
    QMessageBox::about( this, QString( "About %1" ).arg( APPNAME ),
                                QString(
                                  "<html>"
                                  "<body><h3>%1</h3>"
                                  " <p>%1 v%2</p>"
                                  " <p>%3</p>"
                                  " <p>See <a href='%4'>this site</a> for more information.</p>"
                                  " <p>Darkstyle Copyright (c) 2017, <a href='%5'>Juergen Skrotzky</a></p>"
                                  " <p>Icons by <a href='%6'>Double-J Design</a> under <a href='%7'>CC4.0</a></p>"
                                  " </body></html>" )
                                 .arg( APPNAME )
                                 .arg( VERSION )
                                 .arg( "by masc." )
                                 .arg( "https://github.com/masc4ii/SysexLive" )
                                 .arg( "https://github.com/Jorgen-VikingGod" )
                                 .arg( "http://www.doublejdesign.co.uk/" )
                                 .arg( "https://creativecommons.org/licenses/by/4.0/" ) );
}

//Qt Box
void MainWindow::on_actionAboutQt_triggered()
{
    QMessageBox::aboutQt(this);
}

//Add entry to table
void MainWindow::on_actionAddEntry_triggered()
{
    ui->tableWidget->insertRow( ui->tableWidget->rowCount() );
    for( int i = 0; i < ui->tableWidget->columnCount(); i++ )
    {
        QTableWidgetItem *item = new QTableWidgetItem( "" );
        if( i > 0 && i < 9 ) item->setFlags(item->flags() & ~Qt::ItemIsEditable);
        ui->tableWidget->setItem( ui->tableWidget->rowCount()-1, i, item );
    }
    ui->plainTextEdit->setEnabled( true );
}

//Delete current entry in table
void MainWindow::on_actionDeleteEntry_triggered()
{
    ui->tableWidget->removeRow( ui->tableWidget->currentRow() );
}

//Doubleclick in table
void MainWindow::on_tableWidget_doubleClicked(const QModelIndex &index)
{
    if( index.column() >= 5 && index.column() <= 8 )
    {
        QString path = QFileInfo( m_lastSaveFileName ).absolutePath();
        if( !QDir( path ).exists() ) path = QDir::homePath();

        QString fileName = QFileDialog::getOpenFileName(this,
                                               tr("Open sysex file"), path,
                                               tr("Sysex files (*.syx)"));

        //Abort selected
        if( fileName.count() == 0 ) return;

        m_lastSaveFileName = fileName;

        QTableWidgetItem *item = ui->tableWidget->item( index.row(), index.column() - 4 );
        item->setText( fileName );
        item = ui->tableWidget->item( index.row(), index.column() );
        item->setText( QFileInfo( fileName ).fileName() );
    }
}

//Send current selected
void MainWindow::on_actionSendPatches_triggered()
{
    //Nothing there? End!
    if( ui->tableWidget->rowCount() == 0 ) return;

    //Get current row
    int row = ui->tableWidget->currentRow();

    //No row selected
    if( row < 0 )
    {
        QMessageBox::information( this, APPNAME, tr( "Please select an entry first!" ) );
        return;
    }

    for( int i = 0; i < 4; i++ )
    {
        //Get filename
        QString fileName = ui->tableWidget->item( row, i + 1 )->text();

        //Get & check file
        QFile file( fileName );
        if( !file.exists() ) continue;

        //Read from file
        file.open( QIODevice::ReadOnly );
        QByteArray syxData = file.readAll();
        file.close();

        //Send to synth 1
        if( i == 0 && ui->comboBoxSynth1->count() != 0 && ui->comboBoxSynth1->currentIndex() < m_midiOut->getPorts().size() )
        {
            m_midiOut->openPort( ui->comboBoxSynth1->currentIndex() );
            std::vector<unsigned char> message( syxData.begin(), syxData.end() );
            m_midiOut->sendRawMessage( message );
            m_midiOut->closePort();
        }
        //Send to synth 2
        if( i == 1 && ui->comboBoxSynth2->count() != 0 && ui->comboBoxSynth2->currentIndex() < m_midiOut->getPorts().size() )
        {
            m_midiOut->openPort( ui->comboBoxSynth2->currentIndex() );
            std::vector<unsigned char> message( syxData.begin(), syxData.end() );
            m_midiOut->sendRawMessage( message );
            m_midiOut->closePort();
        }
        //Send to synth 3
        if( i == 2 && ui->comboBoxSynth3->count() != 0 && ui->action4Synths->isChecked() && ui->comboBoxSynth3->currentIndex() < m_midiOut->getPorts().size() )
        {
            m_midiOut->openPort( ui->comboBoxSynth3->currentIndex() );
            std::vector<unsigned char> message( syxData.begin(), syxData.end() );
            m_midiOut->sendRawMessage( message );
            m_midiOut->closePort();
        }
        //Send to synth 4
        if( i == 3 && ui->comboBoxSynth4->count() != 0 && ui->action4Synths->isChecked() && ui->comboBoxSynth4->currentIndex() < m_midiOut->getPorts().size() )
        {
            m_midiOut->openPort( ui->comboBoxSynth4->currentIndex() );
            std::vector<unsigned char> message( syxData.begin(), syxData.end() );
            m_midiOut->sendRawMessage( message );
            m_midiOut->closePort();
        }
    }
}

//Delete table
void MainWindow::on_actionNew_triggered()
{
    while( ui->tableWidget->rowCount() > 0 )
    {
        ui->tableWidget->removeRow( 0 );
    }
    ui->plainTextEdit->setEnabled( false );
}

//Open table
void MainWindow::on_actionOpen_triggered()
{
    QString path = QFileInfo( m_lastSaveFileName ).absolutePath();
    QString fileName = QFileDialog::getOpenFileName(this,
                                           tr("Open syxml"), path,
                                           tr("sysex table xml (*.syxml)"));

    //Abort selected
    if( fileName.count() == 0 ) return;

    m_recentFilesMenu->addRecentFile( fileName );
    m_lastSaveFileName = fileName;

    loadFile( fileName );
}

//Load file
void MainWindow::loadFile(const QString & fileName)
{
    //Open file
    QFile file(fileName);
    if( !file.exists() )
    {
        m_recentFilesMenu->removeFile( fileName );
        return;
    }

    //Clear table
    on_actionNew_triggered();

    QXmlStreamReader Rxml;
    file.open(QIODevice::ReadOnly | QFile::Text);

    Rxml.setDevice(&file);
    while( !Rxml.atEnd() && !Rxml.isEndDocument() )
    {
        Rxml.readNext();
        //qDebug()<<"In while!";

        if( Rxml.isStartElement() && Rxml.name() == "settings" )
        {
            //Read name string, if there is one
            if( ui->action4Synths->isChecked() && Rxml.attributes().count() >= 5 )
            {
                m_midiInput = Rxml.attributes().at(0).value().toString();
                m_synth1 = Rxml.attributes().at(1).value().toString();
                m_synth2 = Rxml.attributes().at(2).value().toString();
                m_synth3 = Rxml.attributes().at(3).value().toString();
                m_synth4 = Rxml.attributes().at(4).value().toString();
                //qDebug() << "Ports:" << m_midiInput << m_synth1 << m_synth2 << m_synth3 << m_synth4;
                searchSynths();
            }
            else if( ui->action2Synths->isChecked() && Rxml.attributes().count() >= 3 )
            {
                m_midiInput = Rxml.attributes().at(0).value().toString();
                m_synth1 = Rxml.attributes().at(1).value().toString();
                m_synth2 = Rxml.attributes().at(2).value().toString();
                //qDebug() << "Ports:" << m_midiInput << m_synth1 << m_synth2;
                searchSynths();
            }
            else if( Rxml.attributes().count() >= 2 )
            {
                m_synth1 = Rxml.attributes().at(0).value().toString();
                m_synth2 = Rxml.attributes().at(1).value().toString();
                //qDebug() << "Ports:" << m_synth1 << m_synth2;
                searchSynths();
            }

            while( !Rxml.atEnd() && !Rxml.isEndElement() )
            {
                Rxml.readNext();
                if( Rxml.isStartElement() && Rxml.name() == "song" )
                {
                    //qDebug()<<"start!"<<Rxml.name();
                    on_actionAddEntry_triggered();

                    //Read name string, if there is one
                    if( Rxml.attributes().count() != 0 )
                    {
                        ui->tableWidget->item( ui->tableWidget->rowCount()-1, 0 )->setText( Rxml.attributes().at(0).value().toString() );
                    }

                    while( !Rxml.atEnd() && !Rxml.isEndElement() )
                    {
                        Rxml.readNext();
                        if( Rxml.isStartElement() && Rxml.name() == "synth1" )
                        {
                            QString fileName = Rxml.readElementText();
                            ui->tableWidget->item( ui->tableWidget->rowCount()-1, 1 )->setText( fileName );
                            ui->tableWidget->item( ui->tableWidget->rowCount()-1, 5 )->setText( QFileInfo( fileName ).fileName() );
                            Rxml.readNext();
                        }
                        else if( Rxml.isStartElement() && Rxml.name() == "synth2" )
                        {
                            QString fileName = Rxml.readElementText();
                            ui->tableWidget->item( ui->tableWidget->rowCount()-1, 2 )->setText( fileName );
                            ui->tableWidget->item( ui->tableWidget->rowCount()-1, 6 )->setText( QFileInfo( fileName ).fileName() );
                            Rxml.readNext();
                        }
                        else if( ui->action4Synths->isChecked() && Rxml.isStartElement() && Rxml.name() == "synth3" )
                        {
                            QString fileName = Rxml.readElementText();
                            ui->tableWidget->item( ui->tableWidget->rowCount()-1, 3 )->setText( fileName );
                            ui->tableWidget->item( ui->tableWidget->rowCount()-1, 7 )->setText( QFileInfo( fileName ).fileName() );
                            Rxml.readNext();
                        }
                        else if( ui->action4Synths->isChecked() && Rxml.isStartElement() && Rxml.name() == "synth4" )
                        {
                            QString fileName = Rxml.readElementText();
                            ui->tableWidget->item( ui->tableWidget->rowCount()-1, 4 )->setText( fileName );
                            ui->tableWidget->item( ui->tableWidget->rowCount()-1, 8 )->setText( QFileInfo( fileName ).fileName() );
                            Rxml.readNext();
                        }
                        else if( Rxml.isStartElement() && Rxml.name() == "info" )
                        {
                            QString text = Rxml.readElementText();
                            ui->tableWidget->item( ui->tableWidget->rowCount()-1, 9 )->setText( text );
                            Rxml.readNext();
                        }
                        else if( Rxml.isStartElement() ) //future features
                        {
                            Rxml.readElementText();
                            Rxml.readNext();
                        }
                    }
                    Rxml.readNext();
                }
                else if( Rxml.isEndElement() )
                {
                    //qDebug() << "EndElement! (song)";
                    Rxml.readNext();
                }
            }
        }
    }

    file.close();
}

//Save table
void MainWindow::on_actionSave_triggered()
{
    QString path = QFileInfo( m_lastSaveFileName ).absolutePath();
    QString fileName = QFileDialog::getSaveFileName(this,
                                           tr("Save syxml"), path,
                                           tr("sysex table xml (*.syxml)"));

    //Abort selected
    if( fileName.count() == 0 ) return;

    m_recentFilesMenu->addRecentFile( fileName );
    m_lastSaveFileName = fileName;

    QFile file(fileName);
    file.open(QIODevice::WriteOnly);

    //Open a XML writer
    QXmlStreamWriter xmlWriter(&file);
    xmlWriter.setAutoFormatting(true);
    xmlWriter.writeStartDocument();

    xmlWriter.writeStartElement( "settings" );
    xmlWriter.writeAttribute( "input", m_midiInput );
    xmlWriter.writeAttribute( "port1", m_synth1 );
    xmlWriter.writeAttribute( "port2", m_synth2 );
    if( ui->action4Synths->isChecked() )
    {
        xmlWriter.writeAttribute( "port3", m_synth3 );
        xmlWriter.writeAttribute( "port4", m_synth4 );
    }
    for( int i = 0; i < ui->tableWidget->rowCount(); i++ )
    {
        xmlWriter.writeStartElement( "song" );
        xmlWriter.writeAttribute( "name", ui->tableWidget->item(i, 0)->text() );
        xmlWriter.writeTextElement( "synth1", ui->tableWidget->item(i, 1)->text() );
        xmlWriter.writeTextElement( "synth2", ui->tableWidget->item(i, 2)->text() );
        if( ui->action4Synths->isChecked() )
        {
            xmlWriter.writeTextElement( "synth3", ui->tableWidget->item(i, 3)->text() );
            xmlWriter.writeTextElement( "synth4", ui->tableWidget->item(i, 4)->text() );
        }
        xmlWriter.writeTextElement( "info", ui->tableWidget->item(i, 9)->text() );
        xmlWriter.writeEndElement();
    }
    xmlWriter.writeEndElement();

    xmlWriter.writeEndDocument();

    file.close();
}

//Actively changed midi input
void MainWindow::on_comboBoxInput_activated(const QString &arg1)
{
    m_midiInput = arg1;
}

//Actively changed port 1
void MainWindow::on_comboBoxSynth1_activated(const QString &arg1)
{
    m_synth1 = arg1;
}

//Actively changed port 2
void MainWindow::on_comboBoxSynth2_activated(const QString &arg1)
{
    m_synth2 = arg1;
}

//Actively changed port 3
void MainWindow::on_comboBoxSynth3_activated(const QString &arg1)
{
    m_synth3 = arg1;
}

//Actively changed port 4
void MainWindow::on_comboBoxSynth4_activated(const QString &arg1)
{
    m_synth4 = arg1;
}

//Move row up
void MainWindow::on_actionMoveUp_triggered()
{
    moveRow( true );
}

//Move row down
void MainWindow::on_actionMoveDown_triggered()
{
    moveRow( false );
}

//Move row
void MainWindow::moveRow(bool up)
{
    if(ui->tableWidget->selectedItems().count() != 1) return;
    const int sourceRow = ui->tableWidget->row(ui->tableWidget->selectedItems().at(0));
    const int destRow = (up ? sourceRow-1 : sourceRow+1);
    if(destRow < 0 || destRow >= ui->tableWidget->rowCount()) return;

    // take whole rows
    QList<QTableWidgetItem*> sourceItems = takeRow(sourceRow);
    QList<QTableWidgetItem*> destItems = takeRow(destRow);

    // set back in reverse order
    setRow(sourceRow, destItems);
    setRow(destRow, sourceItems);

    ui->tableWidget->setCurrentCell( destRow, ui->tableWidget->currentColumn() );
}

//Read registry settings
void MainWindow::readSettings()
{
    QSettings set( QSettings::UserScope, "masc.SysexLive", "SysexLive" );
    restoreGeometry( set.value( "mainWindowGeometry" ).toByteArray() );
    ui->splitter->restoreState( set.value( "splitterState" ).toByteArray() );
    m_lastSaveFileName = set.value( "lastFileName", QDir::homePath() ).toString();
    m_recentFilesMenu->restoreState( set.value( "recentFiles" ).toByteArray() );
    QFont font = ui->plainTextEdit->font();
    font.setPointSize( set.value( "fontSize", font.pointSize() ).toInt() );
    ui->plainTextEdit->setFont( font );
    if( set.value( "4Synths", false ).toBool() )
    {
        ui->action4Synths->setChecked( true );
        on_action4Synths_triggered();
    }
    else
    {
        ui->action2Synths->setChecked( true );
        on_action2Synths_triggered();
    }
}

//Write registry settings
void MainWindow::writeSettings()
{
    QSettings set( QSettings::UserScope, "masc.SysexLive", "SysexLive" );
    set.setValue( "mainWindowGeometry", saveGeometry() );
    set.setValue( "splitterState", ui->splitter->saveState() );
    set.setValue( "lastFileName", m_lastSaveFileName );
    set.setValue( "recentFiles", m_recentFilesMenu->saveState() );
    set.setValue( "fontSize", ui->plainTextEdit->font().pointSize() );
    set.setValue( "4Synths", ui->action4Synths->isChecked() );
}

//Take row (for move)
QList<QTableWidgetItem*>  MainWindow::takeRow(int row)
{
    QList<QTableWidgetItem*> rowItems;
    for (int col = 0; col < ui->tableWidget->columnCount(); ++col)
    {
        rowItems << ui->tableWidget->takeItem(row, col);
    }
    return rowItems;
}

//Set row (for move)
void MainWindow::setRow(int row, const QList<QTableWidgetItem*>& rowItems)
{
    for (int col = 0; col < ui->tableWidget->columnCount(); ++col)
    {
        ui->tableWidget->setItem(row, col, rowItems.at(col));
    }
}

//Write accords from Edit to Table
void MainWindow::on_plainTextEdit_textChanged()
{
    if( ui->tableWidget->currentRow() >= 0 )
    {
        ui->tableWidget->item( ui->tableWidget->currentRow(), 9 )->setText( ui->plainTextEdit->toPlainText() );
    }
    else
    {
        ui->plainTextEdit->blockSignals( true );
        ui->plainTextEdit->setPlainText( "" );
        ui->plainTextEdit->blockSignals( false );
    }
}

//Current cell changed
void MainWindow::on_tableWidget_currentCellChanged(int currentRow, int currentColumn, int previousRow, int previousColumn)
{
    Q_UNUSED( currentColumn );
    Q_UNUSED( previousRow );
    Q_UNUSED( previousColumn );
    if( currentRow < 0 )
    {
        ui->plainTextEdit->setPlainText( QString( "" ) );
        ui->plainTextEdit->setEnabled( false );
    }
    else
    {
        ui->plainTextEdit->blockSignals( true );
        ui->plainTextEdit->setPlainText( ui->tableWidget->item( currentRow, 9 )->text() );
        ui->plainTextEdit->blockSignals( false );
        ui->plainTextEdit->setEnabled( true );
    }
}

//Zoom Text +
void MainWindow::on_actionZoomTextPlus_triggered()
{
    QFont font = ui->plainTextEdit->font();
    font.setPointSize( font.pointSize() + 1 );
    ui->plainTextEdit->setFont( font );
}

//Zoom Text -
void MainWindow::on_actionZoomTextMinus_triggered()
{
    QFont font = ui->plainTextEdit->font();
    font.setPointSize( font.pointSize() - 1 );
    ui->plainTextEdit->setFont( font );
}

//Listen to Midi In for program change
void MainWindow::on_pushButtonListen_toggled(bool checked)
{
    if( checked )
    {
        m_midiIn->openPort( ui->comboBoxInput->currentIndex() );
        connect(m_midiIn, SIGNAL(midiMessageReceived(QMidiMessage*)), this, SLOT(onMidiMessageReceive(QMidiMessage*)));
        //qDebug() << "Port opened";
    }
    else
    {
        disconnect(m_midiIn, SIGNAL(midiMessageReceived(QMidiMessage*)), this, SLOT(onMidiMessageReceive(QMidiMessage*)));
        m_midiIn->closePort();
        //qDebug() << "Port closed";
    }
}

//Get the midi message
void MainWindow::onMidiMessageReceive(QMidiMessage *message)
{
    unsigned int statusType = (message->getStatus());
    unsigned int programNumber = message->getValue();
    int theRowCount = ui->tableWidget->rowCount();

    //If program change, select row and send settings
    if( statusType == 192 )
    {
        qDebug() << "Received Program Change on MIDI Channel " << message->getChannel() << programNumber << statusType;
        if( (int)programNumber < theRowCount )
        {
            ui->tableWidget->selectRow( programNumber );
            on_actionSendPatches_triggered();
        }
    }
}

//Config GUI for 2 synths
void MainWindow::on_action2Synths_triggered()
{
    ui->comboBoxSynth3->setVisible( false );
    ui->comboBoxSynth4->setVisible( false );
    ui->labelSynth3->setVisible( false );
    ui->labelSynth4->setVisible( false );
    ui->tableWidget->hideColumn( 7 );
    ui->tableWidget->hideColumn( 8 );
}

//Config GUI for 4 synths
void MainWindow::on_action4Synths_triggered()
{
    ui->comboBoxSynth3->setVisible( true );
    ui->comboBoxSynth4->setVisible( true );
    ui->labelSynth3->setVisible( true );
    ui->labelSynth4->setVisible( true );
    ui->tableWidget->showColumn( 7 );
    ui->tableWidget->showColumn( 8 );
}

//Context menu for table
void MainWindow::on_tableWidget_customContextMenuRequested(const QPoint &pos)
{
    if( ui->tableWidget->rowCount() < 1 || ui->tableWidget->currentRow() < 0 ) return;

    // Handle global position
    QPoint globalPos = ui->tableWidget->mapToGlobal( pos );

    // Create menu and insert some actions
    QMenu myMenu;
    myMenu.addAction( ui->actionSendPatches );
    myMenu.addSeparator();
    myMenu.addAction( ui->actionMoveUp );
    myMenu.addAction( ui->actionMoveDown );
    myMenu.addSeparator();
    myMenu.addAction( ui->actionDeleteEntry );

    // Show context menu at handling position
    myMenu.exec( globalPos );
}

/*
 ____  __   __
(_  _)(  ) / _\
  )(   )( /    \
 (__) (__)\_/\_/

Copyright (C) 2014 mail@patwie.com (http://patwie.com)

This program is free software: you can redistribute it and/or modify
it under the terms of the GNU General Public License as published by
the Free Software Foundation, either version 2 of the License, or
(at your option) any later version.

This program is distributed in the hope that it will be useful,
but WITHOUT ANY WARRANTY; without even the implied warranty of
MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
GNU General Public License for more details.

You should have received a copy of the GNU General Public License
along with this program.  If not, see <http://www.gnu.org/licenses/>.

*/
#include "tia.h"
#include "ui_tia.h"

#include <QDockWidget>
TIA* TIA::m_TIAPtr = NULL;

TIA::TIA(QWidget *parent) :
    QMainWindow(parent),
    ui(new Ui::TIA)
{
    ui->setupUi(this);
    this->showMaximized();
    setDockNestingEnabled(true);

    m_TIAPtr = this;


    m_StatusBar = this->statusBar();
    m_Style = new HStyle;


    createModels();
    createWidgets();
    createViews();

    createActions();
    createConnections();
    createDialogs();

    m_VLog->start();

}

TIA::~TIA()
{
    delete m_Target;
    delete ui;
}

TIA *TIA::instance()
{
    return m_TIAPtr;
}

void TIA::createDialogs()
{
    m_DAbout = new DAbout();
    m_DAbout->setModal(true);
    m_DAbout->setFixedSize(m_DAbout->size());
}

void TIA::createModels()
{
    wLabels = new WLabels();
    wComments = new WComments();
    m_Target = new MTarget();
    m_StaticDisasm = new MStaticDisassembler();
    m_DB = new MDB;
}

void TIA::createWidgets()
{

    wBreakpoints = new WBreakpoints;

    wFunctions = new WFunctions;
    wFunctions->setWindowIcon(QIcon(":/icons/images/node-select-all.png"));

    wDisassembly = new WDisassembly();
    wDisassembly->setWindowIcon(QIcon(":/icons/images/application-detail.png"));

    wImports = new WImports();
    wImports->setWindowIcon(QIcon(":/icons/images/application-import.png"));

    wSegments = new WSegments();
    wSegments->setWindowIcon(QIcon(":/icons/images/application-block.png"));

    wDebugger = new WDebugger();


}

void TIA::createViews()
{
    wCentral = new QTabWidget(this);
    setCentralWidget(wCentral);



    m_VLog = new VLog();
    m_VLog->setWindowIcon(QIcon(":/icons/images/blue-folders-stack.png"));


    QLabel *lblRightStatusbar = new QLabel( tr("  EDIT  ") );

    lblRightStatusbar->setMinimumSize( lblRightStatusbar->sizeHint() );
    lblRightStatusbar->setAlignment( Qt::AlignCenter );
    lblRightStatusbar->setText( tr("v0.0.1alpha") );
    lblRightStatusbar->setToolTip( tr("The current working mode.") );

    m_StatusBar->addPermanentWidget(lblRightStatusbar);
    m_StatusBar->showMessage( tr("Ready"), 2000 );

}

void TIA::createActions()
{
    m_ActionOpen = new QAction(QIcon(":/icons/folder-open-document.png"), tr("&Open..."), this);
    m_ActionOpen->setShortcuts(QKeySequence::Open);
    m_ActionOpen->setStatusTip(tr("Open an existing file"));
    ui->menuFile->addAction(m_ActionOpen);

    m_ActionAbout = new QAction(QIcon(":/icons/folder-open-document.png"), tr("&About"), this);
    m_ActionAbout->setStatusTip(tr("Info"));
    ui->menuInfo->addAction(m_ActionAbout);

    m_ActionDebuggerStart = new QAction(QIcon(":/icons/folder-open-document.png"), tr("&Start"), this);
    m_ActionDebuggerStart->setStatusTip(tr("start the debugger"));
    ui->menuDebugger->addAction(m_ActionDebuggerStart);

    m_ActionDebuggerStop = new QAction(QIcon(":/icons/folder-open-document.png"), tr("&Stop"), this);
    m_ActionDebuggerStop->setStatusTip(tr("stop the debugger"));
    ui->menuDebugger->addAction(m_ActionDebuggerStop);
}

void TIA::createConnections()
{
    // connections from widgets ...
    connect(wDisassembly,SIGNAL(onDisassemblyViewReady()),this,SLOT(onDissassemblyViewReady()));
    connect(wFunctions,SIGNAL(gotoAddress(address_t)),wDisassembly,SLOT(onGotoAddress(address_t)));
    connect(wBreakpoints,SIGNAL(gotoAddress(address_t)),wDisassembly,SLOT(onGotoAddress(address_t)));

    // connections from main dialog ...
    connect(m_ActionOpen,SIGNAL(triggered()),this,SLOT(onMenuActionOpen()));
    connect(this,SIGNAL(newFileOpen(QString)),this,SLOT(onMenuActionOpenFile(QString)));
    connect(m_ActionAbout,SIGNAL(triggered()),this,SLOT(onMenuActionAbout()));
    connect(m_ActionDebuggerStart, SIGNAL(triggered()),this,SLOT(onMenuActionDebuggerStart()));
    connect(m_ActionDebuggerStop, SIGNAL(triggered()),this,SLOT(onMenuActionDebuggerStop()));

    // internal connections
    connect(m_Target,SIGNAL(targetLoaded()),this,SLOT(onTargetLoaded()));
    connect(m_Target,SIGNAL(targetUnloaded()),this,SLOT(onTargetUnloaded()));
    connect(m_StaticDisasm,SIGNAL(DisassemblyError(QString,QString,QString)),this,SLOT(onDisassemblyError(QString,QString,QString)));
    connect(this,SIGNAL(instructionAnalyticsFinish()),wDisassembly,SLOT(insertContent()));
    connect(this,SIGNAL(instructionAnalyticsFinish()),this,SLOT(onInstructionAnalyticsFinish()));
    connect(this,SIGNAL(log(QString)),m_VLog,SLOT(log(QString)));

    connect(m_VLog,SIGNAL(onLogViewReady()),this,SLOT(onLogViewReady()));
    connect(m_StaticDisasm,SIGNAL(fullStaticDisassembly()),this,SLOT(onFullStaticDisassembly()));




}




void TIA::log(QString str){
    m_TIAPtr->m_VLog->log(str);
}

void TIA::log(QString str, QString str2, QString col){
    m_TIAPtr->m_VLog->log(str,str2,col);
}

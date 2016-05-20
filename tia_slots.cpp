#include "tia.h"
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
#include "ui_tia.h"

#include <QFileDialog>
#include <QDockWidget>
#include <QMessageBox>

/********************** Menu Actions *****************************************/
void TIA::onMenuActionOpen(){

    if(m_Target->hasTarget()){
        QMessageBox::StandardButton reply = QMessageBox::information(this,"Load new Target","You currently have a target loaded. Do you really want to load a new one. All unsaves changes will lost.",QMessageBox::Yes|QMessageBox::No);
        if (reply == QMessageBox::No)
            return ;
        else
            m_Target->unloadFile();
    }


    // aks for filename of "exe" that should be debugged
    QString fileName = QFileDialog::getOpenFileName(NULL,"Please select a Target",QDir::currentPath(),"Executables (*.exe)");

    if(!fileName.isEmpty()){
        if(m_Target->loadFile(fileName)){
            if(m_Target->isValidPE()){
                emit newFileOpen(m_Target->name());
            }
        }
    }
}

void TIA::onMenuActionAbout(){

    m_DAbout->show();
}

void TIA::onMenuActionOpenFile(QString name)
{
    m_StatusBar->showMessage( QString("File '%1' was opened").arg(name), 2000 );
}

void TIA::onMenuActionDebuggerStart(){
    wDebugger->open(TIA::m_Target->fullname());
}

void TIA::onMenuActionDebuggerStop()
{
    wDebugger->stop();
}

/******************* internal events ****************************/
void TIA::onTargetUnloaded(){

    removeDockWidget(m_SegmentsDock);
    removeDockWidget(m_FunctionDock);

    m_VLog->clear();
    wCentral->clear();
    m_DB->clear();

    TIA::instance()->wComments->clear();
    TIA::instance()->wFunctions->clear();
    TIA::instance()->wLabels->clear();
    TIA::instance()->wImports->clear();
    qDebug() << "unload target ready";
}

void TIA::onFullStaticDisassembly(){
    m_Target->analyzeInstructions();
    emit instructionAnalyticsFinish();
}

void TIA::onTargetLoaded(){

    QList<PESectionData> Sections = m_Target->sections();

    m_StaticDisasm->setParameter(m_Target->data(),Sections.first().VirtualSize,Sections.first().PointerToRawData,m_Target->modulBase(),Sections.first().VirtualAddress);
    m_StaticDisasm->start();

}

void TIA::onInstructionAnalyticsFinish(){

    wFunctions->refresh();
    m_FunctionDock = new QDockWidget(tr("Functions"), this);
    m_FunctionDock->setWidget(wFunctions);
    m_FunctionDock->setStyle(new HDockWidgetIcon( QIcon(":/icons/images/node-select-all.png"), m_FunctionDock->style()));
    addDockWidget(Qt::LeftDockWidgetArea, m_FunctionDock);

    wSegments->refresh();
    m_SegmentsDock = new QDockWidget(tr("Program Segments"), this);
    m_SegmentsDock->setWidget(wSegments);
    m_SegmentsDock->setStyle(new HDockWidgetIcon( QIcon(":/icons/images/application-block.png"), m_SegmentsDock->style()));
    addDockWidget(Qt::LeftDockWidgetArea, m_SegmentsDock);

    wImports->refresh();
    wCentral->addTab(wImports,QIcon(":/icons/images/application-import.png"),"Imports");
    wBreakpoints->refresh();
    wCentral->addTab(wBreakpoints,QIcon(":/icons/images/application-import.png"),"Breakpoints");

}





void TIA::onLogViewReady(){
     m_LogDock = new QDockWidget(tr("Log"), this);
     m_LogDock->setWidget(m_VLog);
     m_LogDock->setStyle(new HDockWidgetIcon( QIcon(":/icons/images/blue-folders-stack.png"), m_LogDock->style()));
     addDockWidget(Qt::BottomDockWidgetArea, m_LogDock);
}

void TIA::onDissassemblyViewReady(){
     wCentral->addTab(wDisassembly,QIcon(":/icons/images/application-detail.png"),"Static Disassembly");
}

void TIA::onDisassemblyError(QString a,QString b,QString c){
    log(a,b,c);
    m_DB->clear();
    TIA::instance()->wComments->clear();
    TIA::instance()->wFunctions->clear();
    TIA::instance()->wLabels->clear();
    TIA::instance()->wImports->clear();
}

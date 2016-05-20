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
#include "WDisassembly.h"

#include "../Global.h"
#include "../tia.h"
#include "../Helpers/HString.h"

#include <QGridLayout>
#include <QDebug>
#include <QSplitter>
#include <QInputDialog>
#include <QMessageBox>

WDisassembly::WDisassembly(QWidget *parent) :
    QWidget(parent)
{
    // this widget joins two widgets: the asm-view with code and the jump-view with the arrows

    m_Jump = new VJump();
    m_Code = new VCode();

    connect(m_Code,SIGNAL(OnScroll(CodeIter)),m_Jump,SLOT(setTopInstr(CodeIter)));
    connect(m_Code,SIGNAL(OnVisibleLinesChange(int)),m_Jump,SLOT(setNumberOfVisibleLines(int)));
    connect(m_Code,SIGNAL(OnLineClicked(address_t)),m_Jump,SLOT(setClickedLine(address_t)));

    connect(m_Code->actionlabel(),SIGNAL(triggered()),this,SLOT(onEditLabel()));
    connect(m_Code->actioncomment(),SIGNAL(triggered()),this,SLOT(onEditComment()));
    connect(m_Code->actionGoto(),SIGNAL(triggered()),this,SLOT(onGotoDlg()));
    connect(m_Code->actionRenameVariable(),SIGNAL(triggered()),this,SLOT(onRenameVariable()));


    // why the hell the "signal mapper" does not work?
    connect(m_Code->actionConvertToBin(), SIGNAL(triggered()), this, SLOT(onToBin()));
    connect(m_Code->actionConvertToOct(), SIGNAL(triggered()), this, SLOT(onToOct()));
    connect(m_Code->actionConvertToDec(), SIGNAL(triggered()), this, SLOT(onToDec()));
    connect(m_Code->actionConvertToHex(), SIGNAL(triggered()), this, SLOT(onToHex()));
    connect(m_Code->actionConvertToAscii(), SIGNAL(triggered()), this, SLOT(onToAscii()));


    connect(m_Jump,SIGNAL(forceRepaint()),this,SLOT(onForceRepaint()));
    connect(this,SIGNAL(forceRepaint()),this,SLOT(onForceRepaint()));

    QGridLayout *layout = new QGridLayout(this);
    QSplitter *splitter = new QSplitter(this);

    splitter->addWidget(m_Jump);
    splitter->addWidget(m_Code);
    layout->addWidget(splitter,0,0);

}

void WDisassembly::insertContent()
{
    CodeIter it =TIA::instance()->m_Target->instructionsBegin();
    CodeIter end =TIA::instance()->m_Target->instructionsEnd();

    m_Code->setCodeLimits(it.key(),end.key());
    m_Code->insertCode(it,end,it);
    m_Jump->setTopInstr(it);

    emit onDisassemblyViewReady();
}

void WDisassembly::onGotoAddress(address_t addr){
    m_Code->setFocus(addr);
    m_Code->repaint();
}

void WDisassembly::onForceRepaint()
{
    m_Code->repaint();
    m_Jump->repaint();
}

void WDisassembly::onGotoDlg()
{
    bool ok;
    QString newAddress = HString::addrToString(m_Code->currentAddress());

    newAddress = QInputDialog::getText(this, tr("Goto"),tr("new address:"), QLineEdit::Normal ,newAddress, &ok);
    if(newAddress == "") return;


    if (ok && !newAddress.isEmpty()){
        address_t addr = QString(newAddress.trimmed()).toULong(&ok, 16);
        m_Code->setFocus(addr);
        emit forceRepaint();
    }

}

void WDisassembly::onRenameVariable()
{
    TIA::instance()->wLabels->setVariableByDlg(m_Code->variableToRename());
    emit forceRepaint();
}

void WDisassembly::onEditLabel()
{
    TIA::instance()->wLabels->setLabelByDlg(m_Code->currentAddress());
    emit forceRepaint();
}

void WDisassembly::onEditComment()
{
    TIA::instance()->wComments->set(m_Code->currentAddress());
    emit forceRepaint();
}

void WDisassembly::onConvertDigit(NUMBERFORMAT nf)
{
    address_t addr = m_Code->currentAddress() ;
    quint64 val = m_Code->activeToken().Info;

    TIA::instance()->m_DB->setNumberFormat(addr,val,nf);
    m_Code->repaint();
}

void WDisassembly::onToBin(){
    onConvertDigit(N_BIN);
}
void WDisassembly::onToOct(){
    onConvertDigit(N_OCT);
}
void WDisassembly::onToDec(){
    onConvertDigit(N_DEC);
}
void WDisassembly::onToHex(){
    onConvertDigit(N_HEX);
}
void WDisassembly::onToAscii(){
    onConvertDigit(N_ASCII); // ascii
}

void WDisassembly::Test(){
    qDebug() << "Test";
}


/*
    UNFORTUNATELY THIS DOES NOT WORK

    signalMapper = new QSignalMapper(this);

    signalMapper->setMapping(m_Code->m_actionConvertToBin, 2);
    signalMapper->setMapping(m_Code->m_actionConvertToOct, 8);
    signalMapper->setMapping(m_Code->m_actionConvertToDec, 10);
    signalMapper->setMapping(m_Code->m_actionConvertToHex, 16);

    connect(m_Code->m_actionConvertToBin, SIGNAL(triggered()), this, SLOT(Test()));
    connect(m_Code->m_actionConvertToBin, SIGNAL(triggered()), signalMapper, SLOT(map()));
    connect(m_Code->m_actionConvertToOct, SIGNAL(triggered()), signalMapper, SLOT(map()));
    connect(m_Code->m_actionConvertToDec, SIGNAL(triggered()), signalMapper, SLOT(map()));
    connect(m_Code->m_actionConvertToHex, SIGNAL(triggered()), signalMapper, SLOT(map()));



    connect(signalMapper, SIGNAL(mapped(int)), this, SIGNAL(OnConvertDigit(int)));
*/

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
#include "VCode.h"

#include "../Helpers/HString.h"
#include "../tia.h"
#include <QDebug>
#include <QMouseEvent>
#include <QMenu>



VCode::VCode(QWidget *parent )  :  QAbstractScrollArea(parent)
{
    // default styles
    m_Style = new HStyle;
    // how many lines are viewable given the fontheight and viewportsize?
    updateNumberOfVisibleLines();
    // change font
    QAbstractScrollArea::setFont(m_Style->font());
    // save the address of the last clicked line
    m_CurrentClickedAddr = -1;

    // some colors

    m_Colors = m_Style->colors();



    setVerticalScrollBarPolicy(Qt::ScrollBarAlwaysOn);
    setHorizontalScrollBarPolicy(Qt::ScrollBarAlwaysOn);
    // first instruction has offset 0
    verticalScrollBar()->setValue(0);
    // we will change this by "insertCode()"
    verticalScrollBar()->setMaximum(100);

    horizontalScrollBar()->setValue(0);
    horizontalScrollBar()->setMinimum(0);
    horizontalScrollBar()->setMaximum(30);

    // any change of the scrollbar will force a repaint!
    connect(verticalScrollBar(),SIGNAL(valueChanged(int)),this,SLOT(OnScroll(int)));
    connect(horizontalScrollBar(),SIGNAL(valueChanged(int)),this,SLOT(scrollHorizontal(int)));


    // just for the instruction
    m_SegmentNames << "" <<"es" << "ds" << "fs" << "gs" << "cs" << "ss";

    // lookup table for printer of asm code
    m_SizeNames = new QMap<int,QString>;
    m_SizeNames->insert(8,"byte");
    m_SizeNames->insert(16,"word");
    m_SizeNames->insert(32,"dword");
    m_SizeNames->insert(64,"qword");
    m_SizeNames->insert(80,"tword");
    m_SizeNames->insert(128,"dqword");
    m_SizeNames->insert(256,"yword");
    m_SizeNames->insert(512,"zword");

    m_horizontalDrawOffset = 0;


    // action to add a label at the last clicked address
    m_actionLabel = new QAction(QIcon(":/icons/images/tag--pencil.png"),tr("edit Label"),this);
    m_actionLabel->setShortcut(tr("N"));
    m_actionLabel->setStatusTip(tr("change or add a label to current address"));

    // action to add a comment at the last clicked address
    m_actionComment = new QAction(QIcon(":/icons/images/balloon--pencil.png"),tr("edit comment"),this);
    m_actionComment->setShortcut(tr(":"));
    m_actionComment->setStatusTip(tr("change or add a comment to current instruction"));

    m_actionGoto = new QAction(QIcon(":/icons/images/arrow-curve.png"),tr("goto address"),this);
    m_actionGoto->setStatusTip(tr("goto to address"));
    m_actionGoto->setShortcut(tr("G"));

    m_actionRenameVariable = new QAction(QIcon(":/icons/images/sticky-note.png"),tr("rename variable"),this);
    m_actionRenameVariable->setStatusTip(tr("edit the name of the variable"));
    m_actionRenameVariable->setShortcut(tr("V"));

    m_actionFollowJump = new QAction(QIcon(":/icons/images/arrow-turn-180.png"),tr("follow jump"),this);
    m_actionFollowJump->setStatusTip(tr("goto the jump target"));

    m_actionChangeJump = new QAction(QIcon(":/icons/images/arrow-switch.png"),tr("change jump type"),this);
    m_actionChangeJump->setStatusTip(tr("change jump type"));



    m_actionConvertToHex = new QAction(QIcon(":/icons/images/notification-counter-16.png"),"",this);
    m_actionConvertToHex->setStatusTip(tr("convert the number into hex"));

    m_actionConvertToDec = new QAction(QIcon(":/icons/images/notification-counter-10.png"),"",this);
    m_actionConvertToDec->setStatusTip(tr("convert the number into decimal"));

    m_actionConvertToBin = new QAction(QIcon(":/icons/images/notification-counter-02.png"),"",this);
    m_actionConvertToBin->setStatusTip(tr("convert the number into binary"));

    m_actionConvertToOct = new QAction(QIcon(":/icons/images/notification-counter-08.png"),"",this);
    m_actionConvertToOct->setStatusTip(tr("convert the number into octal format"));

    m_actionConvertToAscii = new QAction(QIcon(":/icons/images/notification-counter-ascii.png"),"",this);
    m_actionConvertToAscii->setStatusTip(tr("convert the constant in ascii symbol if possible"));



    m_JumpMenu = new QMenu();

    QStringList JmpModif;
    JmpModif << "JMP" << "JE"<<"JNE"<<"JZ"<<"JNZ"<<"JG"<<"JL"<<"JGE"<<"JLE";
    for(int i=0;i<JmpModif.count();i++){
        QAction* Q = new QAction("set "+JmpModif.at(i),this);
        m_JumpMenu->addAction(Q);
    }



    m_actionChangeJump->setMenu(m_JumpMenu);

    connect(m_actionFollowJump,SIGNAL(triggered()),this,SLOT(followJump()));


}

void VCode::painterColor(ELS t){
    painter->setPen(QPen(m_Colors.constFind(t).value()));
}

/**
 * @brief VCode::OnScroll tries to scroll a goven amount
 * @param val (steps to scroll)
 */
void VCode::OnScroll(int val){
    // how far should we scroll
    scrollSteps(m_TopLine - val);
}

bool VCode::goTo(address_t addr)
{
    if(addr <m_StartInstr.key() || addr >m_EndInstr.key()){
        return false;
    }

    CodeIter i = m_StartInstr;
    int offset=0;
    while(i!=m_EndInstr){
        if(i.key() == addr){
            scrollTo(offset);
            return true;
        }
        offset++;
        i++;
    }
    return false;
}

void VCode::setFocus(address_t addr){
    int line = absoluteLineOfAddress(addr);
    if(line > 4)
        line -=3;
    m_CurrentClickedAddr = addr;
    scrollTo(line);
}

void VCode::followJump()
{
    DISASM cur_instr = instruction(m_CurrentClickedAddr);
    if(HInstruction::isJump(&cur_instr)){
        setFocus(cur_instr.Instruction.AddrValue);

    }


    //scrollTo(12);
}

void VCode::scrollHorizontal(int value)
{
    m_horizontalDrawOffset = -1*value*m_Style->fontWidth();
    repaint();
}






/**
 * @brief VCode::paintEvent re-draw the viewport
 * @param event
 */
void VCode::paintEvent(QPaintEvent *)
{
    // save the programmers keyboard strokes
    DB = TIA::instance()->m_DB;
    // we start painting

    painter = new QPainter(viewport());
    painter->setRenderHint(QPainter::TextAntialiasing);
    painter->setBrush(QBrush(m_Colors[T_NORMALBACKGROUND]));
    painter->setPen(QPen(m_Colors[T_NORMALBACKGROUND]));
    painter->drawRect(this->rect());
    painter->setPen(QPen(QColor("black")));

    // remember topmost viewable instruction
    CodeIter it = m_TopInstr;

    // number of drawn lines
    unsigned int line = 0;

    // draw visible instructions
    QList<Token_t> lineTokens;
    while(line < m_visibleLines && it != m_EndInstr){

        if(it.key() == m_CurrentClickedAddr){
            const int y = m_Style->fontHeight()*(line)+0.2*m_Style->fontHeight();
            painter->save();
            painter->setPen(QPen(m_Colors[T_HIGHLIGHTEDBACKGROUND]));
            painter->setBrush(QBrush(m_Colors[T_HIGHLIGHTEDBACKGROUND]));
            painter->drawRect(0,y,viewport()->width(),m_Style->fontHeight());
            painter->restore();
        }else{

            if(TIA::instance()->wBreakpoints->exists(it.key())){
                const int y = m_Style->fontHeight()*(line)+0.2*m_Style->fontHeight();
                painter->save();
                painter->setPen(QPen(m_Colors[T_BREAKPOINTBACKGROUND]));
                painter->setBrush(QBrush(m_Colors[T_BREAKPOINTBACKGROUND]));
                painter->drawRect(0,y,viewport()->width(),m_Style->fontHeight());
                painter->restore();
            }


        }

        // add address
        if(TIA::instance()->wBreakpoints->exists(it.key())){
            painterColor(T_ACTIVEBREAKPOINT);
        }else{
            painterColor(T_ADDRESS);
        }

        drawText(line,0,HString::addrToString(it.key()));
        // add label
        if(TIA::instance()->wLabels->exists(it.key())){
            painterColor(T_LABEL);
            drawText(line,10, TIA::instance()->wLabels->get(it.key())+":");
            drawOverline(line);
        }

        HTokenizer::Tokenize(&it.value(),&lineTokens);
        drawInstruction(line,&lineTokens);

        if(TIA::instance()->wComments->exists(it.key())){
            painterColor(T_COMMENT);
            drawText(line,85,";"+TIA::instance()->wComments->get(it.key()));
        }

        line++;
        it++;
    }


    // save memory
    delete painter;
}

/**
 * @brief VCode::updateNumberOfVisibleLines
 * recalculates the number of visible lines in viewport
 */
void VCode::updateNumberOfVisibleLines(){
    // simple arithmetic and (dirty) rounding
    m_visibleLines = viewport()->height() / m_Style->fontHeight();
    // m_visibleCharsPerLine = viewport()->width() / m_Style->fontWidth();
    // tell everybody we need a repaint
    emit OnVisibleLinesChange(m_visibleLines);
}


/**
 * @brief VCode::repaint
 */
void VCode::repaint() {
    viewport()->repaint();
}


/************************************** EVENTS *******************************************/

/**
 * @brief VCode::resizeEvent
 */
void VCode::resizeEvent(QResizeEvent *)
{
    // viewportsize changes so recalculate
    updateNumberOfVisibleLines();
    repaint();
}
/**
 * @brief VCode::keyPressEvent
 * @param event
 */
void VCode::keyPressEvent(QKeyEvent *event) {
    switch(event->key()) {
    case Qt::Key_Home:
        if(scrollTo(0)){
            repaint();
        }
        break;
    case Qt::Key_End:
        if(scrollTo(m_NumberOfLines-m_visibleLines-1)){
            repaint();
        }
        break;
    case Qt::Key_Down:
        if(scrollSteps(-1)){
            repaint();
        }
        return;
    case Qt::Key_Up:
        if(scrollSteps(+1)){
            repaint();
        }
        return;
    case Qt::Key_N:
        if(m_CurrentClickedAddr != NULL)
            m_actionLabel->trigger();
        break;
    case Qt::Key_G:
        m_actionGoto->trigger();
        break;
    case ':':
        if(m_CurrentClickedAddr != NULL)
            m_actionComment->trigger();
        break;
    case Qt::Key_V:
        m_actionRenameVariable->trigger();
        break;

    default:
        QAbstractScrollArea::keyPressEvent(event);
    }
}
/**
 * @brief VCode::wheelEvent
 * @param event
 */
void VCode::wheelEvent(QWheelEvent *event)
{
    int numDegrees = event->delta() / 8;
    // one wheel click ==> 2 lines
    int numSteps = numDegrees / 15 *2 ;

    if (event->orientation() == Qt::Vertical) {
        if(scrollSteps(numSteps)){
            verticalScrollBar()->setValue(verticalScrollBar()->value()-numSteps);
            repaint();
        }

    }
    event->accept();
}

void VCode::contextMenuEvent(QContextMenuEvent *e)
{
    const int yy = (e->pos().y())/m_Style->fontHeight();   // line
    const int xx = (e->pos().x())/m_Style->fontWidth();    // character in line

    // do we have an active line?
    if(m_CurrentClickedAddr != NULL){
        // build menu
        QMenu *m = new QMenu();

        QAction *sep = new QAction(this);
        sep->setSeparator(true);

        // default actions (context independent)
        m->addAction(m_actionGoto);
        m->addAction(m_actionLabel);
        m->addAction(m_actionComment);
        m->addAction(sep);

        // is there a jump instruction in current line?
        if(HInstruction::isJump(&instruction(m_CurrentClickedAddr))){
            address_t target = ((CodeIter) (m_StartInstr + m_TopLine+yy)).value().Instruction.AddrValue;
            // is the target address within the disassembled memory?
            if(target >= m_StartInstr.key() && target < m_EndInstr.key()){
                m->addAction(m_actionFollowJump);
                m->addAction(m_actionChangeJump);
            }
        }

        // token-dependent actions
        switch(m_CurrentClickedToken.Type){
        default:
            break;
        case T_VARIABLE:
            m->addAction(m_actionRenameVariable);
            m_VariableToRename = m_CurrentClickedToken.Info;
            break;
        case T_CONSTANT:
            NUMBERFORMAT NF = DB->numberFormat(m_CurrentClickedAddr,m_CurrentClickedToken.Info);
            if(NF != N_HEX){
                m_actionConvertToHex->setText(HString::inFormat(m_CurrentClickedToken.Info,N_HEX));
                m->addAction(m_actionConvertToHex);
            }
            if(NF != N_DEC){
                m_actionConvertToDec->setText(HString::inFormat(m_CurrentClickedToken.Info,N_DEC));
                m->addAction(m_actionConvertToDec);
            }
            if(NF != N_BIN){
                m_actionConvertToBin->setText(HString::inFormat(m_CurrentClickedToken.Info,N_BIN));
                m->addAction(m_actionConvertToBin);
            }
            if(NF != N_OCT){
                m_actionConvertToOct->setText(HString::inFormat(m_CurrentClickedToken.Info,N_OCT));
                m->addAction(m_actionConvertToOct);
            }
            if(NF != N_ASCII){
                QChar c = QChar((char)m_CurrentClickedToken.Info);
                if(c.isPrint()){
                    m_actionConvertToAscii->setText(HString::inFormat(m_CurrentClickedToken.Info,N_ASCII));
                    m->addAction(m_actionConvertToAscii);
                }
            }
            break;
        }

        if(m_CurrentClickedToken.Type != T_UNKNOWN){
            // hopefully this will not happen
        }

        // display the menu
        m->exec(e->globalPos());
    }
}


/**
 * @brief VCode::mouseReleaseEvent
 * @param e
 */
void VCode::mouseReleaseEvent(QMouseEvent *e)
{
    // handle mouse clicking
    // clickline is line offset from topmost visible line in range 0,...,m_visibleLines
    const unsigned int clickline = (e->pos().y())/m_Style->fontHeight();
    const int yy = clickline;
    const int xx = (e->pos().x())/m_Style->fontWidth();
    // should not happen
    if(clickline <0 || clickline > m_visibleLines)
        return;

    // change clicked line
    if((*(m_TopInstr+clickline)).VirtualAddr != m_CurrentClickedAddr){
        m_CurrentClickedAddr = (*(m_TopInstr+clickline)).VirtualAddr ;
        repaint();
        emit OnLineClicked(m_CurrentClickedAddr);
    }

    Token_t clickedToken;
    if(token(yy,xx,&clickedToken)){
        m_CurrentClickedToken.Text = clickedToken.Text;
        m_CurrentClickedToken.Type = clickedToken.Type;
        m_CurrentClickedToken.Info = clickedToken.Info;
    }else{
        m_CurrentClickedToken.Text = "";
        m_CurrentClickedToken.Type = T_UNKNOWN;
        m_CurrentClickedToken.Info = -1;
    }


}


/********************************* CHANGE VIEW *************************************/

const bool VCode::token(const int line, const int offset, Token_t * token)
{
    CodeIter it = m_TopInstr + line;
    QList<Token_t> lineTokens;
    HTokenizer::Tokenize(&it.value(),&lineTokens);

    int offset2 = offset- 30;
    foreach(Token_t t, lineTokens){
        offset2 -= t.Text.length();
        if(offset2 <=0){
            token->Text = t.Text;
            token->Type = t.Type;
            token->Info = t.Info;
            return true;
        }


    }
    return false;
}
/**
 * @brief VCode::scrollTo
 * @param value (number of topmost visible line)
 * @return
 */

bool VCode::scrollTo(unsigned int value){
    // inside range?
    if(value < 0 || value > m_NumberOfLines)
        return false;

    if(value > m_NumberOfLines - m_visibleLines)
        value = m_NumberOfLines - m_visibleLines - 1;

    // set topmost visible line
    m_TopLine = value;
    m_TopInstr = m_StartInstr + m_TopLine;
    verticalScrollBar()->setValue(value);
    emit OnScroll(m_TopInstr);
    return true;
}
/**
 * @brief VCode::scrollSteps
 * @param steps
 * @return
 */
bool VCode::scrollSteps(int steps){
    // for easy thinking
    /* scroll up   = +1
     * scroll down = -1
     */
    steps *= -1;

    if(steps>0){
        // possible? (wihtin range?)
        if(m_TopLine+steps+m_visibleLines < m_NumberOfLines){
            m_TopLine += steps;
            m_TopInstr = m_TopInstr + steps;
            emit OnScroll(m_TopInstr);
            return true;
        }else{
            // cut the amount
            // the following is the topmost visible line if we scroll to the bottom
            m_TopLine = m_NumberOfLines-m_visibleLines-1;
            m_TopInstr = m_StartInstr + m_TopLine;
            emit OnScroll(m_TopInstr);
            return true;
        }
    }else if(steps <0){
        // possible to scroll?
        if(m_TopLine+steps>=0){
            m_TopLine += steps;
            m_TopInstr = m_TopInstr + steps;
            emit OnScroll(m_TopInstr);
            return true;
        }else{
            // chunk the scroll amount to arrive at "0"
            m_TopLine = 0;
            m_TopInstr = m_StartInstr;
            emit OnScroll(m_TopInstr);
            return true;
        }
    }
    // nothing todo (steps==0)
    return false;
}
/**
 * @brief VCode::updateTopInstr
 * @param New
 * @return
 */
bool VCode::updateTopInstr(CodeIter New){
    // set topmost visible instruction
    if(New.key() < m_StartInstr.key() || New.key() > m_EndInstr.key() || New.key() == m_TopInstr.key())
        return false;

    m_TopInstr = New;
    repaint();
    return true;
}


/**
 * @brief VCode::setCodeLimits
 * @param start
 * @param end
 */
void VCode::setCodeLimits(address_t start, address_t end){
    // planning for dynamic assemblies (from the debugger)
    // not in use yet
    m_PossibleEndAddr = end;
    m_PossibleStartAddr = start;
}

/**
 * @brief VCode::insertCode
 * @param Start  first instruction from given assembly
 * @param End    last  instruction  from given assembly
 * @param Top    topmost viewable instruction (for painting and init)
 */
void VCode::insertCode(CodeIter Start,CodeIter End,CodeIter Top)
{
    m_StartInstr = Start;
    m_EndInstr = End;
    m_TopInstr = Top;

    CodeIter it = Start;
    int lines_in_assembly=0;
    int lines_before_topmost=0;
    bool countTop = true;
    // iterate given assembly
    while(it!=End){
        lines_in_assembly++;
        // are we before the topmost visible instr?
        if(countTop && it!=Top){
            lines_before_topmost++;
        }
        // set flag, if we don't count the topmost offset anymore
        if(it == Top)
            countTop = false;
        // next instruction
        it++;

    }

    m_NumberOfLines = lines_in_assembly;
    m_TopLine = lines_before_topmost;

    // all lines - 1.5*viewport  <-- think about it
    verticalScrollBar()->setMaximum(m_NumberOfLines-1.5*m_visibleLines);

    // create spacing (hrizontal offset for instructions parts like: segment, addr, instr, arg1,...)
    m_Spacing.clear();
    // these spacing values look good
    m_Spacing.insert("segment",0);
    int off = 0;
    if(TIA::instance()->m_Target->peType() == _64){
        m_Spacing.insert("addr",10);
        off = 4;
    }else{
        m_Spacing.insert("addr",6);
    }
    m_Spacing.insert("label",10+off);
    m_Spacing.insert("instr",27+off);
    m_Spacing.insert("args",34+off);
    m_Spacing.insert("comment",91+off);

    repaint();
}

void VCode::drawInstruction(const int line, const QList<Token_t>* tokens){

    int offset = 30;

    bool hasprefix = false;

    foreach(Token_t token, *tokens){
        painterColor(token.Type);
        switch(token.Type){
        case T_PREFIX:
            offset = m_Spacing["instr"];
            drawText(line,offset, token.Text);
            offset += token.Text.length()+1;
            hasprefix = true;
            break;
        case T_INSTRUCTION:
            if(!hasprefix)
                offset = m_Spacing["instr"];
            drawText(line,offset, token.Text);
            offset = m_Spacing["args"];;
            break;
        default:
            drawText(line,offset, token.Text);
            offset += token.Text.length();
            break;
        case T_COMMENT:
            break;
        }


    }
}


/**
 * @brief VCode::drawOverline
 * @param line
 */
void VCode::drawOverline(int line){
    // draws a dashed line over the given line
    /*
     * "------------------------------------------"
     * ".text:5641 instr arg1, arg2               "
     */
    painter->save();
    painter->setPen(QPen(QColor("#ADACAC"),1,Qt::DashLine));
    const int y = m_Style->fontHeight()*(line)+0.2*m_Style->fontHeight();
    painter->drawLine(0,y,viewport()->width(),y);
    painter->restore();
}


const DISASM VCode::instruction(address_t addr) const
{
    return TIA::instance()->m_Target->instructions()->find(addr).value();
}


const int VCode::absoluteLineOfAddress(const address_t addr) const
{
    CodeIter i = m_StartInstr;
    int line = 0;
    while(i!=m_EndInstr){

        if(i.key() == addr)
            return line;
        line++;
        i++;
    }
    return 0;
}



/**
 * @brief VCode::drawText
 * @param line
 * @param charoffset
 * @param str
 */
void VCode::drawText(int line, int charoffset, QString str){
    // wraps the "painter->drawText"
    // attention!!! we change x and y in the args
    const int y = m_Style->fontHeight()*(line+1);
    const int x = m_Style->fontWidth()*(charoffset);
    painter->drawText(x+m_horizontalDrawOffset,y,str);
}

void VCode::drawRectangle(int line, int x, int width)
{
    painter->save();
    painter->setBrush(QBrush(QColor(0,0,0,0)));
    //painter->setPen(QPen(m_Colors["selection"]));
    const int yy = m_Style->fontHeight()*(line)+0.2*m_Style->fontHeight();
    const int xx = m_Style->fontWidth()*(x);
    painter->drawRect(xx,yy,width*m_Style->fontWidth(),m_Style->fontHeight());
    painter->restore();
}


const int VCode::visibleLines() const
{
    return m_visibleLines;
}

const CodeIter VCode::topInstr() const
{
    return m_TopInstr;
}

const address_t VCode::currentAddress() const
{
    return m_CurrentClickedAddr;
}

const address_t VCode::variableToRename() const
{
    return m_VariableToRename;
}

const Token_t VCode::activeToken() const
{
    return m_CurrentClickedToken;
}

const QAction *VCode::actionlabel() const
{
    return m_actionLabel;
}

const QAction *VCode::actioncomment() const
{
    return m_actionComment;
}

const QAction *VCode::actionJump() const
{
    return m_actionChangeJump;
}

const QAction *VCode::actionFollowJump() const
{
    return m_actionFollowJump;
}

const QAction *VCode::actionGoto() const
{
    return m_actionGoto;
}

const QAction *VCode::actionRenameVariable() const
{
    return m_actionRenameVariable;
}

const QAction *VCode::actionConvertToHex() const
{
    return m_actionConvertToHex;
}

const QAction *VCode::actionConvertToAscii() const
{
    return m_actionConvertToAscii;
}


const QAction *VCode::actionConvertToOct() const
{
    return m_actionConvertToOct;
}


const QAction *VCode::actionConvertToDec() const
{
    return m_actionConvertToDec;
}


const QAction *VCode::actionConvertToBin() const
{
    return m_actionConvertToBin;
}

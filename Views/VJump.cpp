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
#include "VJump.h"
#include "../tia.h"
#include <QDebug>
#include "../Helpers/HString.h"
#include "../Helpers/HArrow.h"

VJump::VJump(QWidget *parent)  :  QAbstractScrollArea(parent), m_Breakpoint_radius(8), m_Breakpoint_space(15), m_Style(new HStyle), m_Start(NULL),m_Lines(NULL),m_clickedaddress(0)
{

    QAbstractScrollArea::setFont(m_Style->font());
    m_Colors = m_Style->colors();
}


QSize VJump::sizeHint() const
{
    return QSize(50,this->viewport()->height());
}

void VJump::setTopInstr(CodeIter Start){
    m_Start = Start;
    repaint();
}

void VJump::setNumberOfVisibleLines(int lines)
{
    m_Lines = lines;
    repaint();
}

void VJump::setClickedLine(address_t line)
{
    if(m_clickedaddress != line){
        m_clickedaddress = line;
        repaint();
    }

}

void VJump::drawLabel(int Line, QString Text){

    const int LineCoordinate = m_Style->fontHeight()*(1+Line);
    int length = Text.length();


    painter->setBrush(QBrush(QColor("#4040ff")));
    painter->setPen(QPen(QColor("#4040ff")));
    int y = LineCoordinate-0.8*m_Style->fontHeight();

    painter->drawRect(1,y,length*m_Style->fontWidth(),m_Style->fontHeight());
    painter->setPen(QPen(QColor("#ffffff")));
    painter->drawText(2,LineCoordinate,Text);

    y = middleOfLine(Line);
    painter->setPen(QPen(QColor("#4040ff"),2));
    painter->setBrush(QBrush(QColor("#4040ff")));
    HArrow::drawStraightArrow(painter,(length+1)*m_Style->fontWidth()-4,y,this->viewport()->width()-2-m_Breakpoint_space,y);

}

const int VJump::middleOfLine(int Line) const{
    return m_Style->fontHeight()*(1+Line)-0.2*m_Style->fontHeight();
}

void VJump::drawBreakpoint(address_t StartAddress , CodeIter JumpInstruction, int Line){

    const int y = middleOfLine(Line)-5;
    const int x = this->viewport()->width() - 10;
    const int radius = 8;

    painter->save();

    if(TIA::instance()->wBreakpoints->exists(StartAddress)){
        painter->setBrush(m_Colors[T_ACTIVEBREAKPOINT]);
    }else{
        painter->setBrush(m_Colors[T_UNUSEDBREAKPOINT]);
    }

    painter->setRenderHint(QPainter::Antialiasing, true);
    painter->setPen(QPen(m_Colors[T_NORMALBACKGROUND]));
    painter->drawEllipse(x,y,radius,radius);
    painter->restore();

}

void VJump::drawJump(address_t StartAddress , CodeIter JumpInstruction, int Line){

    const int LineCoord = middleOfLine(Line);

    address_t target = DB->jumpTarget(StartAddress);
    QColor linecolor = m_Colors[T_INACTIVEARROW];

    if(JumpInstruction.key()==m_clickedaddress || target == m_clickedaddress){
        linecolor = m_Colors[T_ACTIVEARROW];
    }

    if((JumpInstruction.value().Instruction.Opcode == 0xEB) || JumpInstruction.value().Instruction.Opcode == 0xE9){
        painter->setPen(QPen(linecolor,1, Qt::SolidLine));  // jmp
    }else{
        painter->setPen(QPen(linecolor,1, Qt::DashLine));
    }

    if(JumpInstruction.value().Instruction.Opcode == 0xFF)  // api call
        return;

    // 3 cases

    int drawOffset = (DB->jumpDeep(StartAddress)+1)*(m_Style->fontWidth()+7);

    if(target >= m_Start.key() && target < End.key()){

        if(StartAddress >= m_Start.key() && StartAddress < End.key()){
            // within viewport
            int y_target;
            if(target > StartAddress){
                // target below
                int addOffset = 0;
                CodeIter it2 = JumpInstruction;
                while(it2.key()!=target){
                    it2++;
                    addOffset++;
                }
                y_target = middleOfLine(Line+addOffset);
            }else{
                // target above
                int addOffset = 0;
                CodeIter it2 = JumpInstruction;
                while(it2.key()!=target){
                    it2--;
                    addOffset--;
                }
                y_target = middleOfLine(Line+addOffset);
            }


            HArrow::drawJumpArrowInViewport(painter,this->viewport()->width()-m_Breakpoint_space,LineCoord,y_target,drawOffset);
        }else{
            // start is somewhere out of viewport
        int y_target= middleOfLine(Line);
            if(StartAddress < m_Start.key()){
                // start comes from above

                HArrow::drawJumpArrowEntryTopViewport(painter,this->viewport()->width()-m_Breakpoint_space,y_target,drawOffset);
            }else{
                HArrow::drawJumpArrowEntryBottomViewport(
                            painter,
                            this->viewport()->width()-m_Breakpoint_space,
                            LineCoord,
                            drawOffset,
                            this->viewport()->height()
                            );
            }

        }


    }else{
        if(target >= End.key()){
            HArrow::drawJumpArrowLeaveBottomViewport(
                        painter,
                        this->viewport()->width()-m_Breakpoint_space,
                        LineCoord,
                        drawOffset,
                        this->viewport()->height()
                        );
        }else if(target < m_Start.key()){
            HArrow::drawJumpArrowLeaveTopViewport(
                        painter,
                        this->viewport()->width()-m_Breakpoint_space,
                        LineCoord,
                        drawOffset
                        );
        }
    }
}

void VJump::paintEvent(QPaintEvent *event)
{
    painter = new QPainter(viewport());

    // reset background
    painter->setBrush(QBrush(m_Colors[T_NORMALBACKGROUND]));
    painter->setPen(QPen(m_Colors[T_NORMALBACKGROUND]));
    painter->drawRect(this->rect());

    // something to draw ?
    if(m_Start == NULL || m_Lines == NULL)
        return;

    // last visible instruction
    End = m_Start + m_Lines;

    // save keystrokes
    DB = TIA::instance()->m_DB;

    // iterate all visible instructions
    CodeIter it = m_Start;

    painter->setPen(QPen(m_Colors[T_DEFAULT]));

    // remember a jump of clicked line, so we can highlight it
    // preventing, that it will be overdrawn by other jumps
    address_t last_jump_address=NULL;
    CodeIter last_jump_instr = NULL;
    int last_jump_line = NULL;

    for(int i=0;i<m_Lines;i++){

        const address_t CurrentAddress = it.key();
        const int y = middleOfLine(i);

        drawBreakpoint(CurrentAddress,it,i);

        if(DB->existsJumpFrom(CurrentAddress)){

            if(it.key()==m_clickedaddress || DB->jumpTarget(CurrentAddress)==m_clickedaddress ){
                last_jump_address = CurrentAddress;
                last_jump_instr = it;
                last_jump_line = i;
            }
            drawJump(CurrentAddress,it,i);

        }

        if(DB->isJumpTarget(CurrentAddress)){
            // there are jumps going to this address
            QList<address_t> JumpStarts = DB->jumpsHasTarget(CurrentAddress);
            foreach(address_t ad,JumpStarts){
                if(ad < m_Start.key()){
                    drawJump(ad,TIA::instance()->m_Target->instruction(ad),i);
                }
            }

        }



        if(CurrentAddress == TIA::instance()->m_Target->oep()){
            drawLabel(i,"OEP");
        }

        it++;
    }

    if(last_jump_address != NULL)
        drawJump(last_jump_address,last_jump_instr,last_jump_line);



    delete painter;
}

void VJump::mouseReleaseEvent(QMouseEvent *e)
{
    const int yy = (e->pos().y())/m_Style->fontHeight();   // line
    const int xx = (e->pos().x())/m_Style->fontWidth();    // offset width


    //qDebug() << "clicked on" << yy << " width "<<xx;

    if(e->pos().x()>= viewport()->width() - m_Breakpoint_space){
        qDebug() << "clicked on" << yy;
        TIA::instance()->wBreakpoints->toggle((m_Start+yy).key());
        emit forceRepaint();
    }

}

void VJump::repaint(){
    viewport()->repaint();
}

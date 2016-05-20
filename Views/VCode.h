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
#ifndef VCODE_H
#define VCODE_H

#include <QAbstractScrollArea>
#include <QPainter>
#include <QStringList>
#include <QMap>
#include <QString>
#include <QScrollBar>
#include <QMapIterator>
#include <QContextMenuEvent>
#include <QMenu>
#include <QAbstractScrollArea>

#include "../Helpers/HStyle.h"
#include "../Global.h"
#include "../Models/MTarget.h"
#include "../Helpers/HInstruction.h"
#include "../Helpers/HTokenizer.h"




class VCode : public QAbstractScrollArea
{
    Q_OBJECT

    MDB* DB;



    QStringList m_SegmentNames;
    QMap<int,QString> *m_SizeNames;


    QMap<QString,unsigned int> m_Spacing;

    int m_horizontalDrawOffset;
    int m_TopLine;

    unsigned int m_NumberOfLines;
    unsigned int m_visibleLines;
    unsigned int m_visibleCharsPerLine;
    unsigned int m_maximumDrawnChars;

    QPainter* painter;
    HStyle* m_Style;


    address_t m_VariableToRename;
    address_t m_CurrentClickedAddr;
    Token_t m_CurrentClickedToken;

    address_t m_PossibleStartAddr;
    address_t m_PossibleEndAddr;

    CodeIter m_StartInstr;
    CodeIter m_EndInstr;
    CodeIter m_TopInstr;


    QAction *m_actionLabel;
    QAction *m_actionComment;
    QAction *m_actionFollowJump;
    QAction *m_actionGoto;
    QAction *m_actionRenameVariable;
    QAction *m_actionChangeJump;
    QAction *m_actionConvertToHex;
    QAction *m_actionConvertToDec;
    QAction *m_actionConvertToBin;
    QAction *m_actionConvertToOct;
    QAction *m_actionConvertToAscii;

    QMenu* m_JumpMenu;



     QMap<ELS, QColor>  m_Colors;
public:
    explicit VCode(QWidget *parent = 0);

    void drawText(int line, int charoffset, QString str);
    void drawRectangle(int line, int x, int width);

    const int visibleLines() const;
    const CodeIter topInstr() const;
    const address_t currentAddress() const;
    const address_t variableToRename() const;

    const Token_t activeToken() const;

    const QAction* actionlabel() const;
    const QAction* actioncomment() const;
    const QAction* actionJump() const;
    const QAction* actionFollowJump() const;
    const QAction* actionGoto() const;
    const QAction* actionRenameVariable() const;

    const QAction* actionConvertToHex() const;
    const QAction* actionConvertToAscii() const;
    const QAction* actionConvertToDec() const;
    const QAction* actionConvertToOct() const;
    const QAction* actionConvertToBin() const;

    void repaint();

public slots:

    void updateNumberOfVisibleLines();

    void insertCode(CodeIter Start, CodeIter End, CodeIter Top);
    void setCodeLimits(address_t start, address_t end);

    bool updateTopInstr(CodeIter New);
    bool scrollSteps(int steps);
    bool scrollTo(unsigned int value);
    void OnScroll(int val);
    bool goTo(address_t addr);
    void setFocus(address_t addr);

private slots:
    void followJump();
    void scrollHorizontal(int value);

protected:
    virtual void paintEvent(QPaintEvent *event);
    virtual void resizeEvent(QResizeEvent *event);
    virtual void mouseReleaseEvent(QMouseEvent *e);
    virtual void keyPressEvent(QKeyEvent *event);
    virtual void wheelEvent(QWheelEvent *event);
    virtual void contextMenuEvent ( QContextMenuEvent * e ) ;


    const bool token(const int line, const int offset, Token_t * token);

    const DISASM instruction(address_t addr) const;
    const int absoluteLineOfAddress(const address_t addr) const;


    void drawOverline(int line);
    void drawInstruction(const int line, const QList<Token_t> *tokens);
    void painterColor(ELS t);

signals:
    void OnScroll(const CodeIter Start);
    void OnVisibleLinesChange(const int visible);
    void OnLineClicked(const address_t line);
    void OnGoTo(const address_t addr);

protected slots:


};

#endif // VCODE_H

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
#ifndef TIA_H
#define TIA_H

#include <QMainWindow>
#include <QAction>
#include <QStatusBar>
#include <QProgressBar>
#include <QLabel>
#include <QTableWidget>
#include <QTabWidget>

#include "Helpers/HStyle.h"
#include "Helpers/HDockWidgetIcon.h"

#include "Models/MTarget.h"
#include "Models/MStaticDisassembler.h"
#include "Models/MPatches.h"
#include "Models/MDB.h"
#include "Models/MBreakpoints.h"

#include "Views/VImports.h"
#include "Views/VLog.h"


#include "Widgets/WBreakpoints.h"
#include "Widgets/WFunctions.h"
#include "Widgets/WDisassembly.h"
#include "Widgets/WImports.h"
#include "Widgets/WSegments.h"
#include "Widgets/WLabels.h"
#include "Widgets/WComments.h"
#include "Widgets/WDebugger.h"

#include "Dialogs/DAbout.h"

namespace Ui {
class TIA;
}

class TIA : public QMainWindow
{
    Q_OBJECT

    // contain itself
    static TIA* m_TIAPtr;

public:
    explicit TIA(QWidget *parent = 0);
    ~TIA();

    // single instance access
    static TIA* instance();
    static void log(QString str);
    static void log(QString str, QString str2, QString col);
    /******************* HELPERS *****************/
    HStyle* m_Style;

    /******************* MODELS  *****************/
    MTarget* m_Target;
    MStaticDisassembler* m_StaticDisasm;
    MDB* m_DB;

    /******************* VIEWS  *****************/


    VLog* m_VLog;

    /******************* WIDGETS  *****************/
    QTabWidget *wCentral;
    WBreakpoints *wBreakpoints;
    WFunctions *wFunctions;
    WDisassembly* wDisassembly;
    WImports* wImports;
    WSegments* wSegments;
    WLabels* wLabels;
    WComments* wComments;
    WDebugger* wDebugger;


    /******************* DIALOGS  *****************/
    DAbout* m_DAbout;


protected:

    void createModels();
    void createWidgets();
    void createViews();
    void createActions();
    void createConnections();
    void createDialogs();


private:
    Ui::TIA *ui;

    QAction *m_ActionOpen;
    QAction *m_ActionAbout;
    QAction *m_ActionDebuggerStart;
    QAction *m_ActionDebuggerStop;

    QStatusBar *m_StatusBar;
    QDockWidget *m_ImportsDock;
    QDockWidget *m_SegmentsDock;
    QDockWidget *m_DisassemblyDock;
    QDockWidget *m_LogDock;
    QDockWidget *m_FunctionDock;


signals:
    void newFileOpen(QString name);
    void instructionAnalyticsFinish();

public slots:
    void onMenuActionOpen();
    void onMenuActionAbout();
    void onMenuActionOpenFile(QString name);
    void onMenuActionDebuggerStart();
    void onMenuActionDebuggerStop();

    void onTargetLoaded();
    void onTargetUnloaded();
    void onLogViewReady();
    void onDissassemblyViewReady();
    void onFullStaticDisassembly();
    void onInstructionAnalyticsFinish();
    void onDisassemblyError(QString a, QString b, QString c);

protected slots:

};

#endif // TIA_H

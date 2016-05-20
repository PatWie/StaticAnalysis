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
#ifndef WDebugger_H
#define WDebugger_H

#include <QThread>
#define NOMINMAX
#include <Windows.h>

class WDebugger : public QThread
{
    Q_OBJECT

    bool m_isDebugging;
    bool m_stopDebugging;

    PROCESS_INFORMATION lpProcessInformation;

public:
    explicit WDebugger(QObject *parent = 0);

    const bool isDebugging() const;

    void detach();
    void open(QString filename);

protected:
    void waitForEvent();

signals:

public slots:

    void stop();
    void resume(DWORD status=DBG_EXCEPTION_NOT_HANDLED);
private slots:
    DWORD onCREATE_PROCESS_DEBUG_EVENT(DEBUG_EVENT *ev);
    DWORD onCREATE_THREAD_DEBUG_EVENT(DEBUG_EVENT *ev);
    DWORD onEXIT_THREAD_DEBUG_EVENT(DEBUG_EVENT *ev);
    DWORD onEXIT_PROCESS_DEBUG_EVENT(DEBUG_EVENT *ev);
    DWORD onLOAD_DLL_DEBUG_EVENT(DEBUG_EVENT *ev);
    DWORD onOUTPUT_DEBUG_STRING_EVENT(DEBUG_EVENT *ev);
    DWORD onEXCEPTION_DEBUG_EVENT(DEBUG_EVENT *ev);
    DWORD onUNLOAD_DLL_DEBUG_EVENT(DEBUG_EVENT *ev);
};

#endif // WDebugger_H

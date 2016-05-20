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
#include "WDebugger.h"
#include "../tia.h"

WDebugger::WDebugger(QObject *parent) :
    QThread(parent)
{
    m_isDebugging = false;
}

const bool WDebugger::isDebugging() const
{
    return m_isDebugging;
}

void WDebugger::detach()
{
    m_isDebugging = false;
}

void WDebugger::stop(){
    DebugActiveProcessStop(lpProcessInformation.dwProcessId);
    m_isDebugging = false;
}

void WDebugger::open(QString filename)
{
    // close all previous instances
    detach();

    /*
    BOOL WINAPI CreateProcess(
      _In_opt_     LPCTSTR lpApplicationName,
      _Inout_opt_  LPTSTR lpCommandLine,
      _In_opt_     LPSECURITY_ATTRIBUTES lpProcessAttributes,
      _In_opt_     LPSECURITY_ATTRIBUTES lpThreadAttributes,
      _In_         BOOL bInheritHandles,
      _In_         DWORD dwCreationFlags,
      _In_opt_     LPVOID lpEnvironment,
      _In_opt_     LPCTSTR lpCurrentDirectory,
      _In_         LPSTARTUPINFO lpStartupInfo,
      _Out_        LPPROCESS_INFORMATION lpProcessInformation
    );
    */
    STARTUPINFO         lpStartupInfo = { 0 };
    const DWORD dwCreationFlags = DEBUG_PROCESS | DEBUG_ONLY_THIS_PROCESS ;

    // create process
    bool process_created = CreateProcess(
                TIA::instance()->m_Target->fullname().toStdWString().c_str(),
                (LPWSTR)"",
                NULL,
                NULL,
                false,
                dwCreationFlags,
                NULL,
                NULL,
                &lpStartupInfo,
                &lpProcessInformation
                );

    if(process_created){
        m_isDebugging = true;
        waitForEvent();
    }else{
        m_isDebugging = false;
    }


}

void WDebugger::resume(DWORD status){
    ContinueDebugEvent(lpProcessInformation.dwProcessId,lpProcessInformation.dwThreadId,status);
}

void WDebugger::waitForEvent()
{
    DEBUG_EVENT currentEvent = {0};
    DWORD continueStatus=0;

    while(WaitForDebugEvent(&currentEvent, INFINITE) && m_isDebugging){
        switch(currentEvent.dwDebugEventCode)
        {
            default:
                break;
            case CREATE_PROCESS_DEBUG_EVENT:
                continueStatus = onCREATE_PROCESS_DEBUG_EVENT(&currentEvent);
                break;
            case CREATE_THREAD_DEBUG_EVENT:
                continueStatus = onCREATE_THREAD_DEBUG_EVENT(&currentEvent);
                break;
            case EXIT_THREAD_DEBUG_EVENT:
                continueStatus = onEXIT_THREAD_DEBUG_EVENT(&currentEvent);
                break;
            case EXIT_PROCESS_DEBUG_EVENT:
                continueStatus = onEXIT_PROCESS_DEBUG_EVENT(&currentEvent);
                return;
                break;
            case LOAD_DLL_DEBUG_EVENT:
                continueStatus = onLOAD_DLL_DEBUG_EVENT(&currentEvent);
                break;
            case UNLOAD_DLL_DEBUG_EVENT:
                continueStatus = onUNLOAD_DLL_DEBUG_EVENT(&currentEvent);
                break;
            case OUTPUT_DEBUG_STRING_EVENT:
                continueStatus = onOUTPUT_DEBUG_STRING_EVENT(&currentEvent);
                break;
            case EXCEPTION_DEBUG_EVENT:
                continueStatus = onEXCEPTION_DEBUG_EVENT(&currentEvent);
                //return;
                break;
        }
        resume(continueStatus);

    }


}
DWORD WDebugger::onCREATE_PROCESS_DEBUG_EVENT(DEBUG_EVENT* ev){
    qDebug() << "CREATE_PROCESS_DEBUG_EVENT";
    CloseHandle(lpProcessInformation.hProcess);
    return DBG_EXCEPTION_NOT_HANDLED;
}

DWORD WDebugger::onCREATE_THREAD_DEBUG_EVENT(DEBUG_EVENT* ev){
    qDebug() << "CREATE_THREAD_DEBUG_EVENT";
    return DBG_EXCEPTION_NOT_HANDLED;
}

DWORD WDebugger::onEXIT_THREAD_DEBUG_EVENT(DEBUG_EVENT* ev){
    qDebug() << "EXIT_THREAD_DEBUG_EVENT";
    return DBG_EXCEPTION_NOT_HANDLED;
}

DWORD WDebugger::onEXIT_PROCESS_DEBUG_EVENT(DEBUG_EVENT* ev){
    qDebug() << "EXIT_PROCESS_DEBUG_EVENT";
    stop();
    return DBG_CONTINUE;
}

DWORD WDebugger::onLOAD_DLL_DEBUG_EVENT(DEBUG_EVENT* ev){
    qDebug() << "LOAD_DLL_DEBUG_EVENT";
    CloseHandle(ev->u.LoadDll.hFile);
    return DBG_EXCEPTION_NOT_HANDLED;
}

DWORD WDebugger::onUNLOAD_DLL_DEBUG_EVENT(DEBUG_EVENT* ev){
    qDebug() << "UNLOAD_DLL_DEBUG_EVENT";
    return DBG_EXCEPTION_NOT_HANDLED;
}

DWORD WDebugger::onOUTPUT_DEBUG_STRING_EVENT(DEBUG_EVENT* ev){
    qDebug() << "OUTPUT_DEBUG_STRING_EVENT";
    return DBG_EXCEPTION_NOT_HANDLED;
}

DWORD WDebugger::onEXCEPTION_DEBUG_EVENT(DEBUG_EVENT* ev){
    qDebug() << "EXCEPTION_DEBUG_EVENT";
    return DBG_EXCEPTION_NOT_HANDLED;
}

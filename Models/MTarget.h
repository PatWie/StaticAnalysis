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
#ifndef MTARGET_H
#define MTARGET_H

#define NOMINMAX
#include <Windows.h>

#include "../Global.h"
#include "../Models/MDB.h"

#define BEA_ENGINE_STATIC
#define BEA_USE_STDCALL
#include "../BeaEngine/BeaEngine.h"

#include <QString>
#include <QCryptographicHash>
#include <QList>
#include <QWidget>
#include <QFile>
#include <QFileInfo>
#include <QMap>

enum PEType{_32,_64,_invalid};



struct PESectionData
{
    QString   SectionName;      // name of section (like ".text, .data, .rdata")
    quint64   VirtualSize;      // virtual size
    address_t VirtualAddress;   // address in virtual space
    quint64   SizeOfRawData;    // real data size
    quint64   PointerToRawData; // pointer to real data
    quint64   Characteristics;  // characteristic
};




class MTarget : public QWidget
{
    Q_OBJECT

public:
    MTarget();
    ~MTarget();

    void insertInstruction(address_t addr,DISASM Instr);
    void analyzeInstructions();

    bool loadFile(QString filename);
    bool unloadFile();
    bool isValidPE();

    byte_t* data();

    QMap<address_t,DISASM> m_Instructions;
    QMap<address_t,DISASM>::const_iterator instructionsBegin();
    QMap<address_t,DISASM>::const_iterator instructionsEnd();

    const bool hasTarget() const;

    const LPVOID buffer() const;
    const QString name() const;
    const QString fullname() const;

    const QString md5Checksum() const;
    const QString sha1Checksum() const;

    const address_t modulBase() const;
    const address_t oep() const;
    const address_t SegmentDataStart() const;

    QList<APIData_t> imports32();
    QList<APIData_t> imports64();
    QList<APIData_t> imports();

    QList<PESectionData> sections();

    PEType CheckPEType(LPVOID pBuffer);
    const PEType peType() const;

    QMap<address_t, DISASM> *instructions();
    const CodeIter instruction(address_t addr) const;
    void resolveArgsforCalls();
    void popagateCallArgs();
    void findVariables();
signals:
    void targetLoaded();
    void targetUnloaded();

public slots:

protected:

private:
    QString m_FullName;
    QString m_Name;
    LPVOID m_BufferPtr;
    quint64 m_Size;
    address_t m_OEP;

    QString m_MD5Checksum;
    QString m_SHA1Checksum;

    PEType m_PEType;
    bool m_hasTarget;

    bool m_is64Bit;
    address_t m_ModulBase;

    address_t m_SegmentDataVA;

    QList<PESectionData> m_SectionsOfFile;

    PIMAGE_DOS_HEADER m_DosHeader;
    PIMAGE_NT_HEADERS64 m_NTHeader64;
    PIMAGE_NT_HEADERS32 m_NTHeader32;

    DWORD64 calculateTableOffset64(int iTableEntryNr, PIMAGE_NT_HEADERS64 pINH, PIMAGE_DOS_HEADER pIDH, PBYTE pBuffer);
    DWORD calculateTableOffset32(int iTableEntryNr, PIMAGE_NT_HEADERS32 pINH, PIMAGE_DOS_HEADER pIDH, PBYTE pBuffer);
};

#endif // MTARGET_H

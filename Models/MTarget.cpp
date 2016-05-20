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
#include "MTarget.h"

#include <QDebug>
#include "../tia.h"
#include "../Helpers/HString.h"

/*
 * SOME PIECES ARE FROM NANOMITE
 */

/**
 * @brief MTarget::MTarget
 */
MTarget::MTarget()
{
    m_Name = "";
    m_hasTarget = false;
}

MTarget::~MTarget()
{

    UnmapViewOfFile(m_BufferPtr);
}

void MTarget::insertInstruction(address_t addr, DISASM Instr)
{
    m_Instructions.insert(addr,Instr);
}

void MTarget::resolveArgsforCalls(){

    TIA::log("insert imports fingerprints ... ");
    QMap<address_t, APIData_t > *imports = TIA::instance()->wFunctions->getApiCalls();

    QMap<address_t, APIData_t >::iterator import = imports->begin();
    while(import != imports->end()){
        //qDebug() << import.value().APIName;

        QString Dll = import.value().libraryName;
        Dll = Dll.replace(".dll","");
        QFile mFile(":/libs/Data/"+Dll+".txt");
        //qDebug() << "dll "<<":/libs/Data"+Dll+".txt";
        if(mFile.open(QFile::ReadOnly | QFile::Text)){

            //qDebug() << "can resolve Args for "<< import.value().APIName;

            QTextStream in(&mFile);
            while ( !in.atEnd() )
            {
                QString line = in.readLine();
                QStringList parts = line.split("  ");
                if(parts.count() <2)
                    continue;
                QString str_return = parts.at(0);

                QStringList funcs = parts.at(1).split("(");
                if(funcs.count()<2)
                    continue;

                QStringList args;
                QString str_name = funcs.at(0).trimmed();
                if(str_name == import.value().apiName){


                    QString tmp = funcs.at(1);
                    tmp = tmp.replace(")","");
                    tmp = tmp.replace(";","");
                    tmp = tmp.trimmed();
                    args = tmp.split(",");
                    //qDebug() << args;
                    import.value().Arguments = args;

                    QString g = parts.at(0);
                    g = g.replace("  "," ");
                    import.value().ReturnType = g;


                    break;



                }



            }
            mFile.close();

        }
        import++;

    }
    TIA::log("-> ","done","#00FF00");


}

void MTarget::analyzeInstructions()
{
    TIA::log("analyze instructions ... ");
    MDB* DB = TIA::instance()->m_DB;

    CodeIter it = m_Instructions.begin();
    while(it!=m_Instructions.end()){
        TIA::instance()->m_DB->analyze(it.value());
        it++;
    }
    TIA::log("-> ","done","#00FF00");
    DB->createJumpInformation();

    TIA::instance()->wComments->set(oep(),"OEP");
    TIA::log("found OEP on "+ HString::addrToString(oep()));
    address_t base = modulBase();

    // find Api-Calls
    TIA::log("resolve imports ... ");
    QList<APIData_t> imports_arr = TIA::instance()->m_Target->imports();
    it = m_Instructions.begin();
    while(it!=m_Instructions.end()){

        if(it.value().Instruction.Opcode == 0xFF){
            address_t target = it.value().Argument1.Memory.Displacement;;

            foreach(APIData_t Dat,imports_arr){
                if(Dat.idataAddress +base == target ){
                    TIA::instance()->wFunctions->addApiCall(it.key(),Dat);
                    TIA::instance()->wLabels->set(Dat.idataAddress +base, Dat.libraryName + ":"+Dat.apiName );
                    TIA::instance()->wLabels->set(it.key(),Dat.apiName);
                    break;
                }
            }

        }
        it++;
    }
    TIA::log("-> ","done","#00FF00");
    resolveArgsforCalls();
    popagateCallArgs();

    // find variables
    findVariables();

}

void MTarget::findVariables(){

    TIA::log("search for variables ... ");
    // find data segment
    address_t va_start,va_end;


    foreach(PESectionData segment, m_SectionsOfFile){
        QString name = QString("%1").arg(segment.SectionName).trimmed();
        if(name == ".data"){
            va_start = segment.VirtualAddress + modulBase();
            va_end = segment.VirtualAddress + segment.VirtualSize+ modulBase();
            break;
        }
    }

    TIA::log("data segment starts at: "+ HString::addrToString(va_start ));
    TIA::log("data segment ends   at: "+ HString::addrToString(va_end ));


    for(CodeIter i=m_Instructions.begin();i!=m_Instructions.end();i++){
        DISASM Instr = i.value();

        if(Instr.Instruction.Opcode == 0xFF)
            continue;

        if(Instr.Instruction.Immediat!=NULL){
            if(Instr.Instruction.Immediat >= va_start && Instr.Instruction.Immediat<va_end ){
                // var found
                TIA::instance()->m_DB->addVariable(Instr.Instruction.Immediat);
            }
        }

        for(int a = 1;a<=3;a++){
            ARGTYPE A;
            switch(a){
            case 1: A = Instr.Argument1; break;
            case 2: A = Instr.Argument2; break;
            case 3: A = Instr.Argument2; break;
            }

            if(A.ArgType == MEMORY_TYPE && A.SegmentReg == DSReg  ){
                if(A.Memory.Displacement >= va_start && A.Memory.Displacement<va_end ){
                    // var found
                    TIA::instance()->m_DB->addVariable(A.Memory.Displacement);
                }
            }
        }

    }
    TIA::log("-> ","done","#00FF00");

}


void MTarget::popagateCallArgs(){

    TIA::log("popagate imports arguments as comments ... ");
    CodeIter it = m_Instructions.begin();
    while(it!=m_Instructions.end()){

        if(it.value().Instruction.Opcode == 0xE8 ){
            //qDebug() << "found call "<<it.value().CompleteInstr;

            address_t target = it.value().Instruction.AddrValue;
            if(TIA::instance()->wFunctions->isApiCall(target)){
                const APIData_t cur = TIA::instance()->wFunctions->getApiCall(target);

                CodeIter it2 = it;
                QString rt = (cur.ReturnType == "") ? "_?" : cur.ReturnType;
                TIA::instance()->wComments->set(it2.key(),rt+" "+cur.apiName+"(...)");
                for(int j=0;j<cur.Arguments.count();j++){
                    it2--;

                    QString mne = it2.value().Instruction.Mnemonic;
                    mne = mne.trimmed();
                    mne = mne.toLower();
                    if(mne == "push"){
                    //if(it2.value().Instruction.Opcode == 0xFF ||it2.value().Instruction.Opcode == 0x55 || it2.value().Instruction.Opcode == 0x68 || it2.value().Instruction.Opcode == 0x6A){
                        TIA::instance()->wComments->set(it2.key(),cur.Arguments.at(j));
                    }else{
                        break;
                    }
                }


            }
        }
        it++;

    }
    TIA::log("-> ","done","#00FF00");
}


/**
 * @brief MTarget::loadFile
 * @param filename
 * @return
 */
bool MTarget::loadFile(QString filename)
{
    // does file exists ?
    TIA::log("try to load file "+filename);
    QFile wFile(filename);
    if(wFile.open(QIODevice::ReadOnly) == false)
        return false;

    QFileInfo fileInfo(wFile.fileName());

    m_Name = fileInfo.fileName();
    m_FullName = filename;

    // create checksums
    QByteArray FileContents = wFile.readAll();
    m_MD5Checksum = QString(QCryptographicHash::hash((FileContents), QCryptographicHash::Md5).toHex());
    m_SHA1Checksum = QString(QCryptographicHash::hash((FileContents), QCryptographicHash::Sha1).toHex());


    TIA::log("- md5: "+m_MD5Checksum);
    TIA::log("- sha1: "+m_SHA1Checksum);

    m_SectionsOfFile.clear();
    m_Instructions.clear();

    wFile.close();

    // load file
    TIA::log("read file ... ");
    HANDLE hFile = CreateFileW(m_FullName.toStdWString().c_str(),GENERIC_READ,FILE_SHARE_READ | FILE_SHARE_WRITE,NULL,OPEN_EXISTING,NULL,NULL);
    if(hFile == INVALID_HANDLE_VALUE)
        return false;

    // size of file
    m_Size = GetFileSize(hFile,NULL);
    TIA::log("- size: "+QString("%1 bytes").arg(m_Size));
    HANDLE hFileMap = CreateFileMapping(hFile,NULL,PAGE_READONLY,NULL,NULL,NULL);
    //if(m_Size < 0x2000)
    m_BufferPtr = MapViewOfFile(hFileMap,FILE_MAP_READ,NULL,NULL,NULL);
    //else
    //    m_BufferPtr = MapViewOfFile(hFileMap,FILE_MAP_READ,NULL,NULL,0x2000);

    if(m_BufferPtr == NULL)
    {
        CloseHandle(hFile);
        CloseHandle(hFileMap);
        return false;
    }

    m_ModulBase = 0x00400000;

    m_DosHeader = (PIMAGE_DOS_HEADER) m_BufferPtr;


    if(m_DosHeader->e_magic != IMAGE_DOS_SIGNATURE)
    {
        UnmapViewOfFile(m_BufferPtr);
        CloseHandle(hFile);
        CloseHandle(hFileMap);
        return false;
    }

    m_PEType = CheckPEType(m_BufferPtr);

    if(m_PEType == _invalid)
    {
        UnmapViewOfFile(m_BufferPtr);
        CloseHandle(hFile);
        CloseHandle(hFileMap);
        return false;
    }

    if(m_PEType == _64)
    {
        m_NTHeader64 = (PIMAGE_NT_HEADERS64)((DWORD)m_DosHeader + m_DosHeader->e_lfanew);
        if(m_NTHeader64 != NULL && m_NTHeader64->Signature != IMAGE_NT_SIGNATURE)
        {
            UnmapViewOfFile(m_BufferPtr);
            CloseHandle(hFile);
            CloseHandle(hFileMap);
            return false;
        }
        m_OEP = m_NTHeader64->OptionalHeader.AddressOfEntryPoint;
    }
    else
    {
        m_NTHeader32 = (PIMAGE_NT_HEADERS32)((DWORD)m_DosHeader + m_DosHeader->e_lfanew);
        if(m_NTHeader32 != NULL && m_NTHeader32->Signature != IMAGE_NT_SIGNATURE)
        {
            UnmapViewOfFile(m_BufferPtr);
            CloseHandle(hFile);
            CloseHandle(hFileMap);
            return false;
        }
        m_OEP = m_NTHeader32->OptionalHeader.AddressOfEntryPoint;
    }

    m_SectionsOfFile.clear();
    TIA::log("-> ","done","#00FF00");
    TIA::log("read sections ... ");
    TIA::log("- has " + QString("%1").arg(sections().count()) + " sections ");


    CloseHandle(hFile);
    CloseHandle(hFileMap);


    m_hasTarget = true;
    TIA::log("successfully loaded ... ");
    emit targetLoaded();

    return true;
}

bool MTarget::unloadFile()
{
    if(!m_hasTarget)
        return false;

    UnmapViewOfFile(m_BufferPtr);

    m_SectionsOfFile.clear();


    m_hasTarget = false;
    TIA::log("successfully unloaded ... ");
    emit targetUnloaded();
    return true;
}

PEType MTarget::CheckPEType(LPVOID pBuffer)
{
    PIMAGE_DOS_HEADER pIDH = (PIMAGE_DOS_HEADER)pBuffer;

    PIMAGE_NT_HEADERS32 pINH32 = (PIMAGE_NT_HEADERS32)((DWORD)pIDH + pIDH->e_lfanew);
    if(pINH32 != NULL && pINH32->Signature == IMAGE_NT_SIGNATURE)
    {
        if(pINH32->FileHeader.Machine == IMAGE_FILE_MACHINE_I386)
            return _32;
#ifdef _AMD64_
        else if(pINH32->FileHeader.Machine == IMAGE_FILE_MACHINE_AMD64)
            return _64;
#endif
    }
    // do something incase its invalid...
    return _invalid;
}

const PEType MTarget::peType() const
{
    return m_PEType;
}

bool MTarget::isValidPE()
{
    if(m_PEType == _invalid)
        return false;
    if(m_PEType == _64)
    {
        if(m_NTHeader64 == NULL || m_NTHeader64->FileHeader.Machine != IMAGE_FILE_MACHINE_AMD64)
        {
            return false;
        }
    }
    else
    {
        if(m_NTHeader32 == NULL || m_NTHeader32->FileHeader.Machine != IMAGE_FILE_MACHINE_I386)
        {
            return false;
        }

    }
    return true;
}
QList<APIData_t> MTarget::imports()
{
    QList<APIData_t> importsOfFile;

    if(!m_hasTarget)
        return importsOfFile;

    if(m_PEType == _invalid)
        return importsOfFile;
    if(m_PEType == _64)
        importsOfFile = imports64();
    else
        importsOfFile = imports32();

    return importsOfFile;
}

QList<PESectionData> MTarget::sections()
{
    if(!hasTarget())
        return QList<PESectionData>();

    if(m_SectionsOfFile.count() != 0)
        return m_SectionsOfFile;


    PIMAGE_SECTION_HEADER pSH = NULL;
    DWORD SectionCount = NULL;

    if(m_PEType == _64)
    {
        pSH = (PIMAGE_SECTION_HEADER)((quint64)m_BufferPtr + m_DosHeader->e_lfanew + sizeof(IMAGE_NT_HEADERS64));
        SectionCount = m_NTHeader64->FileHeader.NumberOfSections;
    }
    else
    {
        pSH = (PIMAGE_SECTION_HEADER)((quint64)m_BufferPtr + m_DosHeader->e_lfanew + sizeof(IMAGE_NT_HEADERS32));
        SectionCount = m_NTHeader32->FileHeader.NumberOfSections;
    }

    for (DWORD i = 0; i < SectionCount;i++)
    {
        PESectionData newSection;
        newSection.Characteristics = pSH->Characteristics;
        newSection.PointerToRawData = pSH->PointerToRawData;
        newSection.SectionName = (char*)pSH->Name;
        newSection.SizeOfRawData = pSH->SizeOfRawData;
        newSection.VirtualAddress = pSH->VirtualAddress;
        newSection.VirtualSize = pSH->Misc.VirtualSize;
        m_SectionsOfFile.push_back(newSection);

        if(QString("%1").arg((char*)pSH->Name).trimmed() == ".data"){
            m_SegmentDataVA = pSH->VirtualAddress + modulBase();
        }

        pSH++;
    }

    return m_SectionsOfFile;
}


QList<APIData_t> MTarget::imports32()
{
    QList<APIData_t> importsOfFile;

    DWORD dwVAOfImportSection = m_NTHeader32->OptionalHeader.DataDirectory[IMAGE_DIRECTORY_ENTRY_IMPORT].VirtualAddress;
    DWORD dwImportSectionOffset = calculateTableOffset32(IMAGE_DIRECTORY_ENTRY_IMPORT,m_NTHeader32,m_DosHeader,(PBYTE)m_BufferPtr);


    if(dwVAOfImportSection != 0)
    {

        PIMAGE_IMPORT_DESCRIPTOR pImportHeader = (PIMAGE_IMPORT_DESCRIPTOR)((BYTE*)dwImportSectionOffset);
        do
        {
            PIMAGE_THUNK_DATA32 pIAT = NULL;
            int p=0;

            if(pImportHeader->OriginalFirstThunk != 0)
                pIAT = (PIMAGE_THUNK_DATA32)((pImportHeader->OriginalFirstThunk - dwVAOfImportSection) + dwImportSectionOffset);
            else
                pIAT = (PIMAGE_THUNK_DATA32)((pImportHeader->FirstThunk - dwVAOfImportSection) + dwImportSectionOffset);



            while (pIAT->u1.Ordinal != 0 && !(pIAT->u1.Ordinal & IMAGE_ORDINAL_FLAG32))
            {
                PIMAGE_IMPORT_BY_NAME pImportName = (PIMAGE_IMPORT_BY_NAME)((pIAT->u1.Function - dwVAOfImportSection) + dwImportSectionOffset);
                if(pImportName != NULL && pImportName->Name != NULL)
                {

                    APIData_t newAPI;
                    newAPI.idataAddress = pImportHeader->FirstThunk+p*4;
                    p++;
                    newAPI.apiOffset = pIAT->u1.Function ;
                    newAPI.libraryName = QString::fromLatin1((const char*)((pImportHeader->Name - dwVAOfImportSection) + dwImportSectionOffset));
                    newAPI.apiName = QString::fromLatin1((const char*)pImportName->Name);
                    importsOfFile.push_back(newAPI);
                }

                pIAT++;

            }

            pImportHeader++;
        } while (pImportHeader->Name);
    }

    return importsOfFile;
}
QList<APIData_t> MTarget::imports64()
{
    QList<APIData_t> importsOfFile;

    DWORD64 dwVAOfImportSection = m_NTHeader64->OptionalHeader.DataDirectory[IMAGE_DIRECTORY_ENTRY_IMPORT].VirtualAddress;
    DWORD64 dwImportSectionOffset = calculateTableOffset64(IMAGE_DIRECTORY_ENTRY_IMPORT,m_NTHeader64,m_DosHeader,(PBYTE)m_BufferPtr);

    if(dwVAOfImportSection != 0)
    {
        PIMAGE_IMPORT_DESCRIPTOR pImportHeader = (PIMAGE_IMPORT_DESCRIPTOR)((BYTE*)dwImportSectionOffset);
        do
        {
            PIMAGE_THUNK_DATA64 pIAT = NULL;
            int p=0;

            if(pImportHeader->OriginalFirstThunk != 0)
                pIAT = (PIMAGE_THUNK_DATA64)((pImportHeader->OriginalFirstThunk - dwVAOfImportSection) + dwImportSectionOffset);
            else
                pIAT = (PIMAGE_THUNK_DATA64)((pImportHeader->FirstThunk - dwVAOfImportSection) + dwImportSectionOffset);

            while (pIAT->u1.Ordinal != 0 && !(pIAT->u1.Ordinal & IMAGE_ORDINAL_FLAG64))
            {
                PIMAGE_IMPORT_BY_NAME pImportName = (PIMAGE_IMPORT_BY_NAME)((pIAT->u1.Function - dwVAOfImportSection) + dwImportSectionOffset);
                if(pImportName != NULL && pImportName->Name != NULL)
                {
                    APIData_t newAPI;
                    newAPI.idataAddress = pImportHeader->FirstThunk+p*4;
                    p++;
                    newAPI.apiOffset = pIAT->u1.Function;
                    newAPI.libraryName = QString::fromLatin1((const char*)((pImportHeader->Name - dwVAOfImportSection) + dwImportSectionOffset));
                    newAPI.apiName = QString::fromLatin1((const char*)pImportName->Name);
                    importsOfFile.push_back(newAPI);
                }

                pIAT++;
            }

            pImportHeader++;
        } while (pImportHeader->Name);
    }

    return importsOfFile;
}



DWORD MTarget::calculateTableOffset32(int iTableEntryNr,PIMAGE_NT_HEADERS32 pINH,PIMAGE_DOS_HEADER pIDH,PBYTE pBuffer)
{
    DWORD tableVA = pINH->OptionalHeader.DataDirectory[iTableEntryNr].VirtualAddress;
    PIMAGE_SECTION_HEADER pSectionHeader = (PIMAGE_SECTION_HEADER)(pBuffer + pIDH->e_lfanew + sizeof(IMAGE_NT_HEADERS32));

    for (WORD i = 0; i < pINH->FileHeader.NumberOfSections; i++)
    {
        DWORD sectionVA = pSectionHeader->VirtualAddress;
        DWORD sectionSize = pSectionHeader->Misc.VirtualSize;

        if ((sectionVA <= tableVA) && (tableVA < (sectionVA + sectionSize)))
            return (DWORD)(pBuffer + pSectionHeader->PointerToRawData + (tableVA-sectionVA));
        pSectionHeader++;
    }
    return 0;
}
DWORD64 MTarget::calculateTableOffset64(int iTableEntryNr,PIMAGE_NT_HEADERS64 pINH,PIMAGE_DOS_HEADER pIDH,PBYTE pBuffer)
{
    DWORD64 tableVA = pINH->OptionalHeader.DataDirectory[iTableEntryNr].VirtualAddress;
    PIMAGE_SECTION_HEADER pSectionHeader = (PIMAGE_SECTION_HEADER)(pBuffer + pIDH->e_lfanew + sizeof(IMAGE_NT_HEADERS64));

    for (WORD i = 0; i < pINH->FileHeader.NumberOfSections; i++)
    {
        DWORD64 sectionVA = pSectionHeader->VirtualAddress;
        DWORD64 sectionSize = pSectionHeader->Misc.VirtualSize;

        if ((sectionVA <= tableVA) && (tableVA < (sectionVA + sectionSize)))
            return (DWORD64)(pBuffer + pSectionHeader->PointerToRawData + (tableVA-sectionVA));

        pSectionHeader++;
    }
    return 0;
}

byte_t* MTarget::data(){
    return (byte_t*) m_BufferPtr;
}
QMap<address_t,DISASM>* MTarget::instructions()
{
    return &m_Instructions;
}

const CodeIter MTarget::instruction(address_t addr) const
{
    return m_Instructions.constFind(addr);
}
QMap<address_t,DISASM>::const_iterator MTarget::instructionsBegin()
{
    return m_Instructions.begin();
}

QMap<address_t,DISASM>::const_iterator MTarget::instructionsEnd()
{
    return m_Instructions.end();
}

const bool MTarget::hasTarget() const
{
    return (m_Name != "");
}

const LPVOID MTarget::buffer() const
{
    return m_BufferPtr;
}

const QString MTarget::md5Checksum() const{
    return m_MD5Checksum;
}
const QString MTarget::sha1Checksum() const
{
    return m_SHA1Checksum;
}

const address_t MTarget::modulBase() const
{
    return m_ModulBase;
}

const address_t MTarget::oep() const
{
    return m_OEP+m_ModulBase;
}

const address_t MTarget::SegmentDataStart() const
{
    return m_SegmentDataVA;
}
const QString MTarget::name() const
{
    return m_Name;
}
const QString MTarget::fullname() const
{
    return m_FullName;
}

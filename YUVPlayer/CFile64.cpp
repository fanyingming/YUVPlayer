#include "stdafx.h"
#include "cfile64.h"

// http://jingyan.baidu.com/article/d3b74d648169411f77e60913.html

////////////////// Implementation //////////////////////////////
////////////////////////////////////////////////////////////////////////////
// CFile64 implementation
ULONGLONG CFile64::Seek(LONGLONG lOff, UINT nFrom)
{
    ASSERT_VALID(this);
    ASSERT((HANDLE)m_hFile != INVALID_HANDLE_VALUE);
    ASSERT(nFrom == begin || nFrom == end || nFrom == current);
    ASSERT(begin == FILE_BEGIN && end == FILE_END && current == FILE_CURRENT);
    LARGE_INTEGER liOff;
    liOff.QuadPart = lOff;
    liOff.LowPart = ::SetFilePointer((HANDLE)m_hFile, liOff.LowPart, &liOff.HighPart,
        (DWORD)nFrom);
    if (liOff.LowPart == (DWORD)-1)
        if (::GetLastError() != NO_ERROR)
            CFileException::ThrowOsError((LONG)::GetLastError(), m_strFileName);
    return liOff.QuadPart;
}

ULONGLONG CFile64::GetPosition()
{
    ASSERT_VALID(this);
    ASSERT((HANDLE)m_hFile != INVALID_HANDLE_VALUE);
    LARGE_INTEGER liPos;
    liPos.QuadPart = 0;
    liPos.LowPart = ::SetFilePointer((HANDLE)m_hFile, liPos.LowPart, &liPos.HighPart, FILE_CURRENT);
    if (liPos.LowPart == (DWORD)-1)
        if (::GetLastError() != NO_ERROR)
            CFileException::ThrowOsError((LONG)::GetLastError(), m_strFileName);
    return liPos.QuadPart;
}

void CFile64::LockRange(ULONGLONG dwPos, ULONGLONG dwCount)
{
    ASSERT_VALID(this);
    ASSERT((HANDLE)m_hFile != INVALID_HANDLE_VALUE);
    ULARGE_INTEGER liPos;
    ULARGE_INTEGER liCount;
    liPos.QuadPart = dwPos;
    liCount.QuadPart = dwCount;
    if (!::LockFile((HANDLE)m_hFile, liPos.LowPart, liPos.HighPart, liCount.LowPart,
        liCount.HighPart)) {
        CFileException::ThrowOsError((LONG)::GetLastError(), m_strFileName);
    }
}
void CFile64::UnlockRange(ULONGLONG dwPos, ULONGLONG dwCount)
{
    ASSERT_VALID(this);
    ASSERT((HANDLE)m_hFile != INVALID_HANDLE_VALUE);
    ULARGE_INTEGER liPos;
    ULARGE_INTEGER liCount;
    liPos.QuadPart = dwPos;
    liCount.QuadPart = dwCount;
    if (!::UnlockFile((HANDLE)m_hFile, liPos.LowPart, liPos.HighPart, liCount.LowPart,
        liCount.HighPart)) {
        CFileException::ThrowOsError((LONG)::GetLastError(), m_strFileName);
    }
}
void CFile64::SetLength(ULONGLONG dwNewLen)
{
    ASSERT_VALID(this);
    ASSERT((HANDLE)m_hFile != INVALID_HANDLE_VALUE);
    Seek(dwNewLen, (UINT)begin);
    if (!::SetEndOfFile((HANDLE)m_hFile))
        CFileException::ThrowOsError((LONG)::GetLastError(), m_strFileName);
}
ULONGLONG CFile64::GetLength()
{
    ASSERT_VALID(this);
    ULARGE_INTEGER liSize;
    liSize.LowPart = ::GetFileSize((HANDLE)m_hFile, &liSize.HighPart);
    if (liSize.LowPart == (DWORD)-1)
        if (::GetLastError() != NO_ERROR)
            CFileException::ThrowOsError((LONG)::GetLastError(), m_strFileName);
    return liSize.QuadPart;
}

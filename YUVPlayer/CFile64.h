#ifndef __CFILE64_H__
#define __CFILE64_H__


///////////////////// classes ////////////////////////////////////////
class CFile64 : public CFile {
public:
    // Attributes
    ULONGLONG GetPosition();
    // Overridables
    virtual ULONGLONG Seek(LONGLONG lOff, UINT nFrom);
    virtual void SetLength(ULONGLONG dwNewLen);
    ULONGLONG GetLength();
    virtual void LockRange(ULONGLONG dwPos, ULONGLONG dwCount);
    virtual void UnlockRange(ULONGLONG dwPos, ULONGLONG dwCount);
};

#endif // __CFILE64_H__


#ifndef FUNCTION__FUNCTION_H_
#define FUNCTION__FUNCTION_H_

#include <windows.h>
#include <stdio.h>
#include <string>
#include <vector>
#include <cctype>

typedef unsigned char uchar;
typedef unsigned int uint;

#define DWORD_L2H(x) ((((x) & 0xFF) << 24) + (((x) & 0xFF00) << 8) + (((x) & 0xFF0000000) >> 24) + (((x) & 0xFF0000) >> 8))
#define WODR_L2H(x) ((((x) & 0xFF) << 8) + (((x) & 0xFF00) >> 8))

namespace mk {
namespace func {

unsigned short GetCrc16(uchar *pucBuf, uint uiLen);
std::vector<BYTE> Xor(const uchar *pcOne, const uchar *pcTwe, size_t iLen);

int Bcd2Int(const uchar *pcBcd, uchar cBytes = 4);
std::string Bcd2Str(const uchar *pcBcd, uint iBytes);
std::vector<uchar> Str2Bcd(const char *szStr, uint iLen = (uint)-1, uint iFill = 0);
std::string Bcd2Hex(const uchar *pcBcd, uint iBytes);
std::vector<uchar> Hex2Bcd(const char *szHex, uint iLen = (uint)-1);
std::vector<uchar> Hex2rBcd(const char *szHex, uint iLen = (uint)-1);
//std::string G2U(const char *gb2312);
//std::string U2G(const char *utf8);
//std::wstring M2W(LPCSTR pcM);
//std::string  W2M(LPCWSTR pcW);

void sysprintA(LPCSTR format, ...);
void sysprintW(LPCWSTR format, ...);
void HexPrintA(uchar *pcBuf, int iLen);
void LogPrint(const char *psz, uchar *puc, int len, const char *pszFilename);
bool saveFile(const uchar *pcBuf, int iLen, const char *pszFilename, const char *pszMode);
bool readFile(const char *pszFilename, const char *pszMode, std::vector<uchar> &vData);

} // namespace func

class IsXxChar {
public:
    IsXxChar(int ch)
        : ch_(ch) { }
    bool operator()(int ch) {
        return ch == ch_;
    }
private:
    int ch_;
};

class IsOutOfHex {
public:
    IsOutOfHex() { };
    bool operator()(int ch) {
        int ch_ = toupper(ch);
        return !isdigit(ch_) && (ch_ < 'A' || ch_ > 'F');
    }
};

} // namespace mk

#endif // FUNCTION__FUNCTION_H_

#include "function.h"
#include <time.h>
#include <algorithm>
#include <QDebug>
#include <sstream>

using std::vector;
using std::string;
using std::wstring;
using std::ostringstream;

namespace mk {
namespace func {

static const unsigned short ccitt_crc_table[8 * 32] = {
            0x0000, 0x1021, 0x2042, 0x3063, 0x4084, 0x50a5, 0x60c6, 0x70e7,
            0x8108, 0x9129, 0xa14a, 0xb16b, 0xc18c, 0xd1ad, 0xe1ce, 0xf1ef,
            0x1231, 0x0210, 0x3273, 0x2252, 0x52b5, 0x4294, 0x72f7, 0x62d6,
            0x9339, 0x8318, 0xb37b, 0xa35a, 0xd3bd, 0xc39c, 0xf3ff, 0xe3de,
            0x2462, 0x3443, 0x0420, 0x1401, 0x64e6, 0x74c7, 0x44a4, 0x5485,
            0xa56a, 0xb54b, 0x8528, 0x9509, 0xe5ee, 0xf5cf, 0xc5ac, 0xd58d,
            0x3653, 0x2672, 0x1611, 0x0630, 0x76d7, 0x66f6, 0x5695, 0x46b4,
            0xb75b, 0xa77a, 0x9719, 0x8738, 0xf7df, 0xe7fe, 0xd79d, 0xc7bc,
            0x48c4, 0x58e5, 0x6886, 0x78a7, 0x0840, 0x1861, 0x2802, 0x3823,
            0xc9cc, 0xd9ed, 0xe98e, 0xf9af, 0x8948, 0x9969, 0xa90a, 0xb92b,
            0x5af5, 0x4ad4, 0x7ab7, 0x6a96, 0x1a71, 0x0a50, 0x3a33, 0x2a12,
            0xdbfd, 0xcbdc, 0xfbbf, 0xeb9e, 0x9b79, 0x8b58, 0xbb3b, 0xab1a,
            0x6ca6, 0x7c87, 0x4ce4, 0x5cc5, 0x2c22, 0x3c03, 0x0c60, 0x1c41,
            0xedae, 0xfd8f, 0xcdec, 0xddcd, 0xad2a, 0xbd0b, 0x8d68, 0x9d49,
            0x7e97, 0x6eb6, 0x5ed5, 0x4ef4, 0x3e13, 0x2e32, 0x1e51, 0x0e70,
            0xff9f, 0xefbe, 0xdfdd, 0xcffc, 0xbf1b, 0xaf3a, 0x9f59, 0x8f78,
            0x9188, 0x81a9, 0xb1ca, 0xa1eb, 0xd10c, 0xc12d, 0xf14e, 0xe16f,
            0x1080, 0x00a1, 0x30c2, 0x20e3, 0x5004, 0x4025, 0x7046, 0x6067,
            0x83b9, 0x9398, 0xa3fb, 0xb3da, 0xc33d, 0xd31c, 0xe37f, 0xf35e,
            0x02b1, 0x1290, 0x22f3, 0x32d2, 0x4235, 0x5214, 0x6277, 0x7256,
            0xb5ea, 0xa5cb, 0x95a8, 0x8589, 0xf56e, 0xe54f, 0xd52c, 0xc50d,
            0x34e2, 0x24c3, 0x14a0, 0x0481, 0x7466, 0x6447, 0x5424, 0x4405,
            0xa7db, 0xb7fa, 0x8799, 0x97b8, 0xe75f, 0xf77e, 0xc71d, 0xd73c,
            0x26d3, 0x36f2, 0x0691, 0x16b0, 0x6657, 0x7676, 0x4615, 0x5634,
            0xd94c, 0xc96d, 0xf90e, 0xe92f, 0x99c8, 0x89e9, 0xb98a, 0xa9ab,
            0x5844, 0x4865, 0x7806, 0x6827, 0x18c0, 0x08e1, 0x3882, 0x28a3,
            0xcb7d, 0xdb5c, 0xeb3f, 0xfb1e, 0x8bf9, 0x9bd8, 0xabbb, 0xbb9a,
            0x4a75, 0x5a54, 0x6a37, 0x7a16, 0x0af1, 0x1ad0, 0x2ab3, 0x3a92,
            0xfd2e, 0xed0f, 0xdd6c, 0xcd4d, 0xbdaa, 0xad8b, 0x9de8, 0x8dc9,
            0x7c26, 0x6c07, 0x5c64, 0x4c45, 0x3ca2, 0x2c83, 0x1ce0, 0x0cc1,
            0xef1f, 0xff3e, 0xcf5d, 0xdf7c, 0xaf9b, 0xbfba, 0x8fd9, 0x9ff8,
            0x6e17, 0x7e36, 0x4e55, 0x5e74, 0x2e93, 0x3eb2, 0x0ed1, 0x1ef0
};

unsigned short GetCrc16(uchar *pucBuf, uint uiLen)
{
    unsigned  short crc = 0;

    while (uiLen--)
    {
        crc = ccitt_crc_table[(crc >> 8 ^ (*pucBuf++)) & 0xFF] ^ (crc << 8);
    }

    return crc & 0xFFFF;
}

void sysprintA(LPCSTR format, ...)
{
    va_list ap;
    va_start(ap, format);
    int iNeed = _vscprintf(format, ap) + 1;
    char *pszOut = new char[iNeed];
    pszOut[iNeed - 1] = 0;
    vsprintf_s(pszOut, iNeed, format, ap);
    va_end(ap);
    qDebug() << pszOut;
    delete[] pszOut;
}

void sysprintW(LPCWSTR format, ...)
{
    va_list ap;
    va_start(ap, format);
    int iNeed = _vscwprintf(format, ap) + 1;
    WCHAR *pszOut = new WCHAR[iNeed];
    pszOut[iNeed - 1] = 0;
    vswprintf_s(pszOut, iNeed, format, ap);
    va_end(ap);
    qDebug() << pszOut;
    delete[] pszOut;
}

void HexPrintA(uchar *pcBuf, int iLen)
{
    qDebug() << Bcd2Hex(pcBuf, iLen).c_str();
}

void LogPrint(const char *psz, uchar *puc, int len, const char *pszFilename)
{
    FILE *fp = fopen(pszFilename, "wb");
    if (fp == NULL)
        return;

    if (psz != NULL) {
        fputs(psz, fp);
    }

    for (int i = 0; i < len; i++) {
        fprintf(fp, "%02x", puc[i]);
    }
    fputc('\r\n', fp);

    fclose(fp);
}

bool saveFile(const uchar *pcBuf, int iLen, const char *pszFilename, const char *pszMode)
{
    FILE *fp = fopen(pszFilename, pszMode);
    if (fp == NULL)
        return false;

    fwrite(pcBuf, iLen, 1, fp);

    fclose(fp);

    return true;
}

bool readFile(const char *pszFilename, const char *pszMode, std::vector<uchar> &vData)
{
    FILE *fp = fopen(pszFilename, pszMode);
    if (fp == NULL) return false;

    fseek(fp, 0, SEEK_END);
    long len = ftell(fp);
    fseek(fp, 0, SEEK_SET);

    vData.resize(len);
    fread(&vData[0], len, 1, fp);

    fclose(fp);
    return true;
}

vector<BYTE> Xor(const uchar *pcOne, const uchar *pcTwe, size_t iLen)
{
    vector<BYTE> vXor(iLen);

    for (size_t i = 0; i < iLen; ++i)
    {
        vXor[i] = pcOne[i] ^ pcTwe[i];
    }

    return vXor;
}

int Bcd2Int(const uchar *pcBcd, uchar cBytes)
{
    int iNum = 0;

    for (int i = 0; i < cBytes && i < 4; i++)
    {
        if ((pcBcd[i] & 0x0F) > 0x09) return -1;
        if ((pcBcd[i] & 0xF0) > 0x90) return -1;

        iNum *= 100;
        iNum += (pcBcd[i] & 0x0F) + (pcBcd[i] >> 4) * 10;
    }

    return iNum;
}

string Bcd2Str(const uchar *pcBcd, uint iBytes)
{
    string sRet;

    for (uint i = 0; i < iBytes; i++)
    {
        if ((pcBcd[i] & 0x0F) > 0x09) return string();
        if ((pcBcd[i] & 0xF0) > 0x90) return string();

        sRet.append(1, (pcBcd[i] >> 4) + 0x30);
        sRet.append(1, (pcBcd[i] & 0x0F) + 0x30);
    }

    string::size_type sit = sRet.find_first_not_of(0x30, 0);
    sRet.erase(0, sit);

    if (sRet.empty()) sRet.append(1, 0x30);

    return sRet;
}

vector<uchar> Str2Bcd(const char *szStr, uint iLen, uint iFill)
{
    vector<uchar> vecRet;

    iLen = strlen(szStr) < iLen ? strlen(szStr) : iLen;

    if (iLen % 2 == 1) return vector<uchar>();

    for (uint i = 0; i < iLen; i += 2)
    {
        char Hi = szStr[i];
        char Lo = szStr[i + 1];

        if (!isdigit(Hi)) return vector<uchar>();
        if (!isdigit(Lo)) return vector<uchar>();

        Hi -= 0x30;
        Lo -= 0x30;

        vecRet.push_back((Hi << 4) | Lo);
    }

    while (vecRet.size() < iFill) {
        vecRet.insert(vecRet.begin(), 0x00);
    }

    return vecRet;
}

string Bcd2Hex(const uchar *pcBcd, uint iBytes)
{
    string sRet;

    for (uint i = 0; i < iBytes; i++)
    {
        BYTE Hi = pcBcd[i] >> 4;
        BYTE Lo = pcBcd[i] & 0x0F;

        sRet.append(1, Hi + (Hi <= 0x09 ? 0x30 : 0x37));
        sRet.append(1, Lo + (Lo <= 0x09 ? 0x30 : 0x37));
    }

    return sRet;
}

vector<uchar> Hex2Bcd(const char *szHex, uint iLen)
{
    vector<uchar> vecRet;

    iLen = strlen(szHex) < iLen ? strlen(szHex) : iLen;

    if (iLen % 2 == 1) return vector<uchar>();

    for (uint i = 0; i < iLen; i += 2)
    {
        char Hi = toupper(szHex[i]);
        char Lo = toupper(szHex[i + 1]);

        if (isdigit(Hi)) {
            Hi -= 0x30;
        }
        else if (Hi >= 'A' || Hi <= 'F') {
            Hi -= 'A' - 10;
        }
        else {
            return vector<uchar>();
        }

        if (isdigit(Lo)) {
            Lo -= 0x30;
        }
        else if (Hi >= 'A' || Hi <= 'F') {
            Lo -= 'A' - 10;
        }
        else {
            return vector<uchar>();
        }

        vecRet.push_back((Hi << 4) | Lo);
    }

    return vecRet;
}

vector<uchar> Hex2rBcd(const char *szHex, uint iLen)
{
    vector<uchar> vecRet;

    iLen = strlen(szHex) < iLen ? strlen(szHex) : iLen;

    if (iLen % 2 == 1) return vector<uchar>();

    for (uint i = iLen - 2; i < iLen; i -= 2)
    {
        char Hi = toupper(szHex[i]);
        char Lo = toupper(szHex[i + 1]);

        if (isdigit(Hi)) {
            Hi -= 0x30;
        }
        else if (Hi >= 'A' || Hi <= 'F') {
            Hi -= 'A' - 10;
        }
        else {
            return vector<uchar>();
        }

        if (isdigit(Lo)) {
            Lo -= 0x30;
        }
        else if (Hi >= 'A' || Hi <= 'F') {
            Lo -= 'A' - 10;
        }
        else {
            return vector<uchar>();
        }

        vecRet.push_back((Hi << 4) | Lo);
    }

    return vecRet;
}

//string U2G(const char *utf8)
//{
//    int len = MultiByteToWideChar(CP_UTF8, 0, utf8, -1, NULL, 0);
//    if (len < 1) return string();

//    wchar_t* wstr = new wchar_t[len];
//    memset(wstr, 0, len);

//    MultiByteToWideChar(CP_UTF8, 0, utf8, -1, wstr, len);


//    len = WideCharToMultiByte(CP_ACP, 0, wstr, -1, NULL, 0, NULL, NULL);
//    if (len < 1) return string();
//    std::string str(len, 0);

//    WideCharToMultiByte(CP_ACP, 0, wstr, -1, &str[0], len, NULL, NULL);
//    if (wstr)
//        delete[] wstr;

//    str = str.c_str();
//    return str;
//}

//string G2U(const char *gb2312)
//{
//    int len = MultiByteToWideChar(CP_ACP, 0, gb2312, -1, NULL, 0);
//    if (len < 1) return string();

//    wchar_t* wstr = new wchar_t[len];
//    memset(wstr, 0, len);

//    MultiByteToWideChar(CP_ACP, 0, gb2312, -1, wstr, len);


//    len = WideCharToMultiByte(CP_UTF8, 0, wstr, -1, NULL, 0, NULL, NULL);
//    if (len < 1) return string();
//    std::string str(len, 0);

//    WideCharToMultiByte(CP_UTF8, 0, wstr, -1, &str[0], len, NULL, NULL);
//    if (wstr)
//        delete[] wstr;

//    if (str[str.size() - 1] == '\0')
//        str.erase(str.size() - 1);

//    return str;
//}

//wstring M2W(LPCSTR pcM)
//{
//    int len = MultiByteToWideChar(CP_ACP, 0, pcM, -1, NULL, 0);
//    wstring strW(len + 1, 0);
//    MultiByteToWideChar(CP_ACP, 0, pcM, -1, &strW[0], len);

//    strW = strW.c_str();
//    return strW;
//}

//string W2M(LPCWSTR pcW)
//{
//    int len = WideCharToMultiByte(CP_ACP, 0, pcW, -1, NULL, 0, NULL, NULL);
//    string strM(len + 1, 0);
//    WideCharToMultiByte(CP_ACP, 0, pcW, -1, &strM[0], len, NULL, NULL);

//    strM = strM.c_str();
//    return strM;
//}

} // namespace func
} // namespace mk

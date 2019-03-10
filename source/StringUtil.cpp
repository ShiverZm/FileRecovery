#include "StringUtil.h"
#include <tchar.h>
#include <stdio.h>
#include <iostream>
#include "StringUtil.h"


#include "commutil.h"

using namespace std;
using namespace commutil;

//�޲ι��캯��
CStringUtil::CStringUtil() :m_Buf(NULL), m_BufLen(0), m_Capacity(0)
{
	Assign(10);
}

//����Ϊchar* ��wchar_t*�͵��ַ���
CStringUtil::CStringUtil(LPCTSTR prmStr) : m_Buf(NULL), m_BufLen(0), m_Capacity(0)
{
	Assign(10);
	this->Append(prmStr);
}

//����Ϊchar*��wchar_t*����prmStr�ַ����ĵ�prmStart����
//����ʼ��ȡ������ȡprmLen���ַ�
CStringUtil::CStringUtil(LPCTSTR prmStr,int prmStart,int prmLen):m_Buf(0),m_BufLen(0),m_Capacity(0)
{
	Assign(10);
	int tmpLen = _tcslen(prmStr);
	if(prmStart + prmLen>tmpLen)
	{
		prmLen = m_BufLen-prmStart;
	}
	if(Assign(prmLen))
	{
		memcpy(m_Buf,((char*)prmStr+prmStart*sizeof(TCHAR)),prmLen*sizeof(TCHAR));
		m_BufLen = prmLen;
	}
}

//�������죬����һ��CStringUtil������Ϊ��ǰ�ַ���
CStringUtil::CStringUtil(const CStringUtil &prmObj) : m_Buf(NULL), m_BufLen(0), m_Capacity(0)
{
	Assign(10);
	this->Append(prmObj);
}

//��ֵ����
CStringUtil &CStringUtil::operator=(const CStringUtil &prmObj)
{
	if (this==&prmObj)
	{
		return *this;
	}
	if(m_Buf==NULL || m_Capacity==0)
	{
		this->Assign(10);
	}
	else
	{
		memset(m_Buf,0,m_BufLen*sizeof(TCHAR));
		m_BufLen = 0;
	}
	this->Append(prmObj);
	return *this;
}

//����
CStringUtil::~CStringUtil()
{
	free(m_Buf);
	m_Buf = NULL;
	m_BufLen = 0;
	m_Capacity = 0;
}

//��ǰ�ַ���������׷��һ��char*��wchar_t*��ʾ���ַ���
CStringUtil	&CStringUtil::Append(LPCTSTR prmStr)
{
	if (prmStr == NULL)
	{
		Assign(10);
		return *this;
	}
	size_t tmpLen = _tcslen(prmStr);
	if (tmpLen + m_BufLen > m_Capacity)
	{
		size_t tmpNewSize = m_BufLen;
		//ѡ�񳤶Ƚϴ����2�����Ƚ�����չ���������Ŀռ���ܲ���
		if (tmpLen > tmpNewSize)
		{
			tmpNewSize = tmpLen;
		}
		//�Խ������������������ַ�������
		tmpNewSize = tmpNewSize * 2;
		Assign(tmpNewSize);
	}
	//ȷ���ռ��㹻
	if (tmpLen + m_BufLen <= m_Capacity)
	{
		memcpy(((char*)m_Buf + m_BufLen * sizeof(TCHAR)), prmStr, tmpLen * sizeof(TCHAR));
		m_BufLen += tmpLen;
	}
	return *this;
}

//��ǰ�ַ���������׷��һ��char*��wchar_t��ʾ���ַ���
//@prmStartPos���ַ����ĵ�prmStartPos�ַ���ʼ��ȡ
//@prmLen����prmStartPos��ʼ��ȡprmLen���ַ�
CStringUtil	&CStringUtil::Append(LPCTSTR prmStr, UINT32 prmStartPos, UINT32 prmLen)
{
	//Ҫ��ȡ���ַ��������������ַ���ʵ�ʳ���
	if ( (prmLen + prmStartPos > _tcslen(prmStr)) || prmLen==-1) 
	{
		prmLen = _tcslen(prmStr) - prmStartPos;
	}
	if (prmLen >= m_Capacity-m_BufLen)
	{
		this->Assign( (m_Capacity+prmLen)*2);
		/*if (this->Assign(m_Capacity * 2))
		{
			memcpy(m_Buf + m_BufLen, prmStr + prmStartPos, prmLen * sizeof(TCHAR));
			m_BufLen += prmLen;
		}*/
	}
	if (m_BufLen + prmLen <= m_Capacity)
	{
		memcpy(m_Buf + m_BufLen, prmStr + prmStartPos, prmLen * sizeof(TCHAR));
		m_BufLen += prmLen;
	}
	return *this;
}

#ifdef _UNICODE
//�����ǰ����ѡ�����Unicode���룬������ǰ����׷��char*���ַ���
CStringUtil	&CStringUtil::Append(char *prmStr)
{
	//��ȡ��prmStr ANSI����ת��ΪUnicode������Ҫ���ٳ���
	int tmpLen = ::MultiByteToWideChar(CP_ACP, 0, prmStr, -1, NULL, 0);
	//tmpLen = (tmpLen + 1) * sizeof(TCHAR);
	TCHAR	*tmpBuf = (TCHAR*)malloc((tmpLen + 1) * sizeof(TCHAR));
	if (tmpBuf == NULL)
	{
		return *this;
	}
	memset(tmpBuf, 0, (tmpLen + 1) * sizeof(TCHAR));
	//��ANSIת��ΪUnicode�ַ���
	::MultiByteToWideChar(CP_ACP, 0, prmStr, -1, tmpBuf, tmpLen);
	//ת������ַ���׷�ӵ���ǰ������
	this->Append(tmpBuf);
	free(tmpBuf);
	return *this;
}
#else
//�����ǰѡ�����ANSI���룬֧����ǰ�������wchar_t*���ַ���
CStringUtil	&CStringUtil::Append(wchar_t *prmStr)
{
	//��ȡ��Unicodeת��ΪANSI�ַ�����Ҫ�����ַ�
	int tmpLen = ::WideCharToMultiByte(CP_ACP, 0, prmStr, -1, NULL, 0, 0, 0);
	char	*tmpBuf = (char*)malloc(tmpLen + 2);
	if (tmpBuf == NULL)
	{
		return *this;
	}
	memset(tmpBuf, 0, tmpLen + 2);
	//Unicodeת��ΪANSI�ַ���
	::WideCharToMultiByte(CP_ACP, 0, prmStr, -1, tmpBuf, tmpLen, 0, 0);
	//ת������ַ���׷�ӵ���ǰ������
	this->Append(tmpBuf);
	free(tmpBuf);
	return *this;
}
#endif

//����һ��CStringUtil��������ʾ�ַ���׷�ӵ���ǰ������
CStringUtil	&CStringUtil::Append(const CStringUtil &prmObj)
{
	if (this == &prmObj)
	{
		return *this;
	}
	this->Append(prmObj.m_Buf);
	return *this;
}

//��ǰ�ַ�������׷��һ���ַ�
CStringUtil	&CStringUtil::Append(TCHAR prmChar)
{
	TCHAR	tmpStr[2] = { prmChar };
	this->Append(tmpStr);
	return *this;
}

//��prmNum����ʾ��������Ϊ�ַ���׷�ӵ���ǰ������
CStringUtil	&CStringUtil::Append(int prmNum)
{
	char	tmpBuf[MAX_PATH] = { 0 };
	_snprintf_s(tmpBuf, MAX_PATH, "%d", prmNum);
	this->Append(tmpBuf);
	return *this;
}

CStringUtil &CStringUtil::operator+=(LPCTSTR prmStr)
{
	this->Append(prmStr);
	return *this;
	//return this->Append(prmStr);
}
#ifdef _UNICODE
CStringUtil	&CStringUtil::operator+=(char *prmStr)
{
	this->Append(prmStr);
	return *this;
}
#else
CStringUtil	&CStringUtil::operator+=(wchar_t *prmStr)
{
	this->Append(prmStr);
	return *this;
}
#endif
CStringUtil &CStringUtil::operator+=(const CStringUtil &prmObj)
{
	this->Append(prmObj);
	return *this;
}
CStringUtil &CStringUtil::operator+=(TCHAR prmChar)
{
	this->Append(prmChar);
	return *this;
}
CStringUtil &CStringUtil::operator+=(int prmNum)
{
	this->Append(prmNum);
	return *this;
}

CStringUtil CStringUtil::operator+(LPCTSTR prmStr)
{
	CStringUtil	tmpString = *this;
	tmpString.Append(prmStr);
	return tmpString;
}
#ifdef _UNICODE
CStringUtil	CStringUtil::operator+(char *prmStr)
{
	CStringUtil	tmpString = *this;
	tmpString.Append(prmStr);
	return tmpString;
}
#else
CStringUtil	CStringUtil::operator+(wchar_t *prmStr)
{
	CStringUtil	tmpString = *this;
	tmpString.Append(prmStr);
	return tmpString;
}
#endif
CStringUtil CStringUtil::operator+(const CStringUtil &prmObj)
{
	CStringUtil	tmpString = *this;
	tmpString.Append(prmObj);
	return tmpString;
}
CStringUtil CStringUtil::operator+(TCHAR prmChar)
{
	CStringUtil	tmpString = *this;
	tmpString.Append(prmChar);
	return tmpString;
}
CStringUtil CStringUtil::operator+(int prmNum)
{
	CStringUtil	tmpString = *this;
	tmpString.Append(prmNum);
	return tmpString;
}

//��յ�ǰ�����е��ַ�����Ϣ�������ͷ���������ڴ档
void CStringUtil::Empty()
{
	memset(m_Buf,0,sizeof(TCHAR)*m_BufLen);
	m_BufLen = 0;
	m_Capacity = 0;
}

//�ַ����Ƿ�Ϊ��
BOOL CStringUtil::IsEmpty()
{
	return m_BufLen==0;
}

//�ַ����Ƿ����
BOOL CStringUtil::operator ==(const CStringUtil &prmObj)
{
	return (Compare(m_Buf,prmObj.m_Buf)==0);
}

//�ַ����Ƿ񲻵�
BOOL CStringUtil::operator!=(const CStringUtil &prmObj)
{
	return (Compare(m_Buf,prmObj.m_Buf)!=0);
}

//��ǰ�ַ����Ƿ�С��prmObj����ʾ�ַ���
BOOL CStringUtil::operator<(const CStringUtil &prmObj)
{
	return (Compare(m_Buf,prmObj.m_Buf)<0);
}

//��ǰ�ַ����Ƿ�С�ڵ���prmObj����ʾ�ַ���
BOOL CStringUtil::operator <=(const CStringUtil &prmObj)
{
	return (Compare(m_Buf,prmObj.m_Buf)<=0);
}

//��ǰ�ַ����Ƿ����prmObj����ʾ�ַ���
BOOL CStringUtil::operator>(const CStringUtil &prmObj)
{
	return (Compare(m_Buf,prmObj.m_Buf)>0);
}

//��ǰ�ַ����Ƿ���ڵ���prmObj����ʾ�ַ���
BOOL CStringUtil::operator>=(const CStringUtil &prmObj)
{
	return (Compare(m_Buf,prmObj.m_Buf)>=0);
}

//�ַ��������ִ�Сд�Ƚ�
int		CStringUtil::CompareNoCase(const TCHAR *prmStr)
{
	return (_tcsicmp(m_Buf,prmStr));
}

//�ַ��������ִ�Сд�Ƚ�
int		CStringUtil::CompareNoCase(const CStringUtil &prmObj)
{
	return (_tcsicmp(m_Buf,prmObj.m_Buf));
}

//�ַ���ֻ�Ƚ�prmLen���ַ�,���ִ�Сд
int		CStringUtil::CompareNChar(const TCHAR *prmStr,size_t prmLen)
{
	return (_tcsncmp(m_Buf,prmStr,prmLen));
}

//�ַ���ֻ�Ƚ�prmLen���ַ�,���ִ�Сд
int		CStringUtil::CompareNChar(const CStringUtil &prmObj,size_t prmLen)
{
	return (_tcsncmp(m_Buf,prmObj.m_Buf,prmLen));
}

//�ַ���ֻ�Ƚ�prmLen���ַ��������ִ�Сд
int		CStringUtil::CompareNCharNoCase(const TCHAR *prmStr,size_t prmLen)
{
	return (_tcsnicmp(m_Buf,prmStr,prmLen));
}

//�ַ���ֻ�Ƚ�prmLen���ַ��������ִ�Сд
int		CStringUtil::CompareNCharNoCase(const CStringUtil &prmObj,size_t prmLen)
{
	return (_tcsnicmp(m_Buf,prmObj.m_Buf,prmLen));
}

//���ַ�����߿�ʼ��ȡprmLen���ַ�
CStringUtil CStringUtil::Left(int prmLen)
{
	size_t tmpLen = prmLen;
	if(tmpLen>m_BufLen || prmLen==-1)
	{
		tmpLen = m_BufLen;
	}
	return CStringUtil(m_Buf,0,prmLen);
}

//���ַ����ұ߿�ʼ��ȡprmLen���ַ�
CStringUtil	CStringUtil::Right(int prmLen)
{
	size_t tmpLen = prmLen;
	if(tmpLen<0 || tmpLen>m_BufLen)
	{
		tmpLen = m_BufLen;
	}
	return CStringUtil(m_Buf,m_BufLen-tmpLen,tmpLen);
}

//��ȡ��prmStart��ʼ��prmEnd�������ַ�������Χ[prmStart,prmEnd)
CStringUtil CStringUtil::Mid(int prmStart,int prmEnd)
{
	if(prmStart>prmEnd || prmStart>=(int)m_BufLen)
	{
		return CStringUtil(_T(""));
	}
	return CStringUtil(m_Buf,prmStart,prmEnd-prmStart+1);
}

//�����±����ã���ȡ��prmIndex���ַ�
TCHAR	&CStringUtil::operator[](int prmIndex)
{
	if(prmIndex<0 || prmIndex>=(int)m_BufLen)
	{
		//throw std::exception(_T("Out of Range"));
		throw std::out_of_range("Out of Range");
	}
	return (*(m_Buf + prmIndex));
}

//��ȡ��prmIndex���ַ�
TCHAR	CStringUtil::GetAt(int prmIndex)
{
	if(prmIndex<0 || prmIndex>=(int)m_BufLen)
	{
		throw std::out_of_range("Out of Range");
	}
	return (*(m_Buf + prmIndex));
}

//�ӵ�ǰ�ַ����в���prmStr�Ӵ�������ҵ����򷵻����ַ������±�ֵ�����򷵻�-1
//���ҷ�ʽΪ�������ҿ�ʼ��
int		CStringUtil::FindString(const TCHAR *prmStr)
{
	size_t tmpLen = _tcslen(prmStr);
	if (tmpLen > m_BufLen)
	{
		return -1;
	}
	for (size_t i = 0; i <= m_BufLen - tmpLen; i++)
	{
		if(memcmp(m_Buf+i,prmStr,tmpLen*sizeof(TCHAR)) == 0)
		{
			return i;
		}
		/*if (this->CompareNChar(prmStr, tmpLen) == 0)
		{
			return i;
		}*/
	}
	return -1;
}

//�ӵ�ǰ�ַ����в���prmObj����ʾ�Ӵ�������ҵ����򷵻����ַ������±�ֵ�����򷵻�-1
//���ҷ�ʽΪ�������ҿ�ʼ��
int		CStringUtil::FindString(const CStringUtil &prmObj)
{
	return this->FindString(prmObj.m_Buf);
}

//�ӵ�ǰ�ַ����в���PrmStr�Ӵ�������ҵ����򷵻����ַ������±�ֵ�����򷵻�-1
//���ҷ�ʽΪ�����������
int		CStringUtil::RFindString(const TCHAR *prmStr)
{
	size_t	tmpLen = _tcslen(prmStr);
	if (tmpLen > m_BufLen)
	{
		return -1;
	}
	for (int i = (int)(m_BufLen - tmpLen); i >= 0; i--)
	{
		/*if (this->CompareNChar(m_Buf + i, tmpLen) == 0)
		{
			return i;
		}*/
		if(memcmp(m_Buf+i,prmStr,tmpLen*sizeof(TCHAR)) ==0)
		{
			return i;
		}
	}
	return -1;
}

//�ӵ�ǰ�ַ����в���PrmStr�Ӵ�������ҵ����򷵻����ַ������±�ֵ�����򷵻�-1
//���ҷ�ʽΪ�����������
int		CStringUtil::RFindString(const CStringUtil &prmObj)
{
	return this->RFindString(prmObj.m_Buf);
}

//�������Ҳ��ҵ�ǰ�ַ������Ƿ����ַ�prmChar��������򷵻��ַ����ֵ�ַ�����򷵻�NULL
TCHAR	*CStringUtil::StrChar(const TCHAR prmChar)
{
	return _tcschr(m_Buf, prmChar);
}

//����������ҵ�ǰ�ַ������Ƿ����ַ�prmChar��������򷵻��ַ����ֵ�ַ�����򷵻�NULL
TCHAR	*CStringUtil::StrrChar(const TCHAR prmChar)
{
	return _tcsrchr(m_Buf, prmChar);
}

//��ǰ�ַ����Ƿ���prmStr��ʼ
BOOL	CStringUtil::StartWith(const TCHAR *prmStr)
{
	size_t tmpLen = _tcslen(prmStr);
	if (tmpLen > m_BufLen)
	{
		return FALSE;
	}
	return ( (this->CompareNChar(prmStr, tmpLen)) == 0);
}

//��ǰ�ַ����Ƿ���prmObj����ʾ�ַ�����ʼ
BOOL	CStringUtil::StartWith(const CStringUtil &prmObj)
{
	return this->StartWith(prmObj.m_Buf);
}

//��ǰ�ַ����Ƿ���prmStr����
BOOL	CStringUtil::EndWith(const TCHAR *prmStr)
{
	size_t tmpLen = _tcslen(prmStr);
	if (tmpLen > m_BufLen)
	{
		return FALSE;
	}
	return this->Compare(m_Buf + m_BufLen - tmpLen, prmStr)==0;
}

//��ǰ�ַ����Ƿ���prmObj����ʾ�ַ�������
BOOL	CStringUtil::EndWith(const CStringUtil &prmObj)
{
	return this->EndWith(prmObj.m_Buf);
}

//��ǰ�ַ����Ƿ���prmStr��ʼ�����Դ�Сд
BOOL	CStringUtil::StartWithNoCase(const TCHAR *prmStr)
{
	size_t tmpLen = _tcslen(prmStr);
	if (tmpLen > m_BufLen)
	{
		return FALSE;
	}
	return ((this->CompareNCharNoCase(prmStr, tmpLen)) == 0);
}

//��ǰ�ַ����Ƿ���prmObj����ʾ�ַ�����ʼ�����Դ�Сд
BOOL	CStringUtil::StartWithNoCase(const CStringUtil &prmObj)
{
	return this->StartWithNoCase(prmObj.m_Buf);
}

//��ǰ�ַ����Ƿ���prmStr���������Դ�Сд
BOOL	CStringUtil::EndWithNoCase(const TCHAR *prmStr)
{
	size_t tmpLen = _tcslen(prmStr);
	if (tmpLen > m_BufLen)
	{
		return FALSE;
	}
	//return this->CompareNoCase(m_Buf + m_BufLen - tmpLen, prmStr) == 0;
	return (_tcsicmp(m_Buf + m_BufLen - tmpLen, prmStr) == 0);
}

//��ǰ�ַ����Ƿ���prmObj����ʾ�ַ������������Դ�Сд
BOOL	CStringUtil::EndWithNoCase(const CStringUtil &prmObj)
{
	return this->EndWithNoCase(prmObj.m_Buf);
}

//
CStringUtil	&CStringUtil::Trim()
{
	UINT32 i = 0;
	for (i = 0; i < m_BufLen; i++)
	{
#ifdef _UNICODE
		INT16  *tmpVal = (INT16*)(m_Buf + i);
#else
		INT8	*tmpVal = (INT8*)(m_Buf + i);
#endif
		if (!this->IsSpaceChar(*tmpVal))
		{
			break;
		}
		//if(!isspace((m_Buf+m_BufLen)))
	}
	if (i > 0)
	{
		memcpy(m_Buf, m_Buf + i, (m_BufLen - i + 1)*sizeof(TCHAR));
		m_BufLen = m_BufLen - i;
	}
	for (i = m_BufLen - 1; i >= 0 && m_BufLen>0; i--)
	{
#ifdef _UNICODE
		INT16  *tmpVal = (INT16*)(m_Buf + i);
#else
		INT8	*tmpVal = (INT8*)(m_Buf + i);
#endif
		if (this->IsSpaceChar(*tmpVal))
		{
			*(m_Buf + i) = 0;
			m_BufLen--;
		}
		else
		{
			break;
		}
	}
	return *this;
}

void CStringUtil::ToUpper()
{
	for (UINT32 i = 0; i < m_BufLen; i++)
	{
#ifdef _UNICODE
		INT16  *tmpVal = (INT16*)(m_Buf + i);
#else
		INT8	*tmpVal = (INT8*)(m_Buf + i);
#endif
		if (*tmpVal >= _T('a') && *tmpVal <= _T('z'))
		{
			*tmpVal -= 32;
		}
	}
}

void CStringUtil::ToLower()
{
	for (UINT32 i = 0; i < m_BufLen; i++)
	{
#ifdef _UNICODE
		INT16  *tmpVal = (INT16*)(m_Buf + i);
#else
		INT8	*tmpVal = (INT8*)(m_Buf + i);
#endif
		if (*tmpVal >= _T('A') && *tmpVal <= _T('Z'))
		{
			*tmpVal += 32;
		}
	}
}

void CStringUtil::SplitString(vector<CStringUtil> &prmArray, LPCTSTR prmSplitStr)
{
	prmArray.clear();
	if (m_BufLen == 0)
	{
		return;
	}
	TCHAR *tmpBuf = (TCHAR*)malloc((m_BufLen + 2) * sizeof(TCHAR));
	memset(tmpBuf, 0, (m_BufLen + 2) * sizeof(TCHAR));
	memcpy(tmpBuf, m_Buf, m_BufLen * sizeof(TCHAR));
	TCHAR *tmpContext = NULL;
	TCHAR *tmpTok = _tcstok_s(tmpBuf, prmSplitStr,&tmpContext);
	while (tmpTok != NULL)
	{
		prmArray.push_back(tmpTok);
		tmpTok = _tcstok_s(NULL, prmSplitStr,&tmpContext);
	}
	free(tmpBuf);
}

CStringUtil CStringUtil::ReplaceStr(TCHAR *prmSrc, TCHAR *prmDst)
{
	CStringUtil	tmpResult;
	UINT32 tmpLastPos = 0;
	UINT32 tmpPos = 0;

	int		tmpLen = _tcslen(prmSrc);
	while (tmpPos <= m_BufLen)
	{
		BOOL	tmpFlag = TRUE;
		for(int i=0;i<tmpLen;i++)
		{
			if(*(m_Buf+tmpPos+i)!=*(prmSrc+i))
			{
				tmpFlag = FALSE;
				break;
			}
		}
		if(tmpFlag)
		{
			tmpResult.Append(m_Buf, tmpLastPos, tmpPos - tmpLastPos);
			tmpResult.Append(prmDst);
			tmpLastPos = tmpPos + tmpLen;
			tmpPos += tmpLen;
			continue;
		}
		/*if (_tcsncmp(m_Buf + tmpPos, prmSrc, _tcslen(prmSrc) * sizeof(TCHAR)) == 0)
		{
			tmpResult.Append(m_Buf, tmpLastPos, tmpPos - tmpLastPos);
			tmpResult.Append(prmDst);
			tmpLastPos = tmpPos + _tcslen(prmSrc);
			tmpPos += _tcslen(prmSrc);
			continue;
		}*/
		tmpPos++;
	}
	if (tmpLastPos < m_BufLen)
	{
		tmpResult.Append(m_Buf + tmpLastPos);
	}
	return tmpResult;
}

//�ַ�����ʽ��
void CStringUtil::Format(TCHAR *prmForamt, ...)
{
	int	tmpSize = 0;
	va_list		tmpArgs;
	va_start(tmpArgs, prmForamt);
#pragma warning(push)
#pragma warning(disable:4996)
	tmpSize = _vsntprintf(NULL, 0, prmForamt, tmpArgs);

	if (m_Capacity <= (UINT32)tmpSize)
	{
		this->Assign(tmpSize + 1);
	}
	m_BufLen = _vsntprintf(m_Buf, m_Capacity - 1, prmForamt, tmpArgs);
#pragma warning(pop)
	va_end(tmpArgs);
}

//���·���prmNewSize���ַ����ռ�
BOOL CStringUtil::Assign(size_t prmNewSize)
{
	//�����һ���ַ��ռ����ڴ���ַ���β'\0'
	TCHAR *tmpBuf = (TCHAR*)malloc( (prmNewSize+1) * sizeof(TCHAR));
	if (tmpBuf == NULL)
	{
		return FALSE;
	}
	memset(tmpBuf, 0, sizeof(TCHAR)*(prmNewSize + 1));
	if (m_Buf != NULL)
	{
		memcpy(tmpBuf, m_Buf, m_BufLen * sizeof(TCHAR));
	}
	free(m_Buf);
	//��ǰ�ַ���ָ���·���Ŀռ�
	m_Buf = tmpBuf;
	//�·���Ĵ�С
	m_Capacity = prmNewSize;
	return TRUE;
}

//�Ƚ������ַ�������ȷ���0��С�ڷ���-1�����ڷ���1
int CStringUtil::Compare(const TCHAR *prmStr1,const TCHAR *prmStr2)
{
	return (_tcscmp(prmStr1,prmStr2));
}

//�ж��Ƿ�Ϊ�ո��ַ�
BOOL CStringUtil::IsSpaceChar(int prmCharVal)
{
	if (prmCharVal < 255 && prmCharVal>0 && isspace(prmCharVal))
	{
		return TRUE;
	}
	return FALSE;
}
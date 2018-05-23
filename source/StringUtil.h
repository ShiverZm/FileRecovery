/***************************************************
*
*	@Version:	1.0
*	@Author:	Mengxl
*	@Date:		2017-8-27 22:11
*	@File:		StringUtil.h
*
****************************************************/

#ifndef STRING_UTIL_INCLUDE_H
#define STRING_UTIL_INCLUDE_H

#include <windows.h>
#include <tchar.h>
#include <vector>

using namespace std;

namespace	commutil
{
	class CStringUtil
	{
	public:
		//�޲ι��캯��
		CStringUtil();
		//����Ϊchar* ��wchar_t*�͵��ַ���
		CStringUtil(LPCTSTR prmStr);
		//����Ϊchar*��wchar_t*����prmStr�ַ����ĵ�prmStart����
		//����ʼ��ȡ������ȡprmLen���ַ�
		CStringUtil(LPCTSTR prmStr,int prmStart,int prmLen);
		//�������죬����һ��CStringUtil������Ϊ��ǰ�ַ���
		CStringUtil(const CStringUtil &prmObj);
		//��ֵ����
		CStringUtil & operator=(const CStringUtil &prmObj);
		//����
		~CStringUtil();

		//��ǰ�ַ���������׷��һ��char*��wchar_t*��ʾ���ַ���
		CStringUtil	&Append(LPCTSTR prmStr);

		//��ǰ�ַ���������׷��һ��char*��wchar_t��ʾ���ַ���
		//@prmStartPos���ַ����ĵ�prmStartPos�ַ���ʼ��ȡ
		//@prmLen����prmStartPos��ʼ��ȡprmLen���ַ�
		CStringUtil	&Append(LPCTSTR prmStr, UINT32 prmStartPos, UINT32 prmLen);
#ifdef _UNICODE
		//�����ǰ����ѡ�����Unicode���룬������ǰ����׷��char*���ַ���
		CStringUtil	&Append(char *prmStr);
#else
		//�����ǰѡ�����ANSI���룬֧����ǰ�������wchar_t*���ַ���
		CStringUtil	&Append(wchar_t *prmStr);
#endif
		//����һ��CStringUtil��������ʾ�ַ���׷�ӵ���ǰ������
		CStringUtil	&Append(const CStringUtil &prmObj);
		//��ǰ�ַ�������׷��һ���ַ�
		CStringUtil	&Append(TCHAR prmChar);
		//��prmNum����ʾ��������Ϊ�ַ���׷�ӵ���ǰ������
		CStringUtil	&Append(int prmNum);

		CStringUtil &operator+=(LPCTSTR prmStr);
#ifdef _UNICODE
		CStringUtil	&operator+=(char *prmStr);
#else
		CStringUtil	&operator+=(wchar_t *prmStr);
#endif
		CStringUtil &operator+=(const CStringUtil &prmObj);
		CStringUtil &operator+=(TCHAR prmChar);
		CStringUtil &operator+=(int prmNum);

		CStringUtil operator+(LPCTSTR prmStr);
#ifdef _UNICODE
		CStringUtil	operator+(char *prmStr);
#else
		CStringUtil	operator+(wchar_t *prmStr);
#endif
		CStringUtil operator+(const CStringUtil &prmObj);
		CStringUtil operator+(TCHAR prmChar);
		CStringUtil operator+(int prmNum);

		//��յ�ǰ�����е��ַ�����Ϣ�������ͷ���������ڴ档
		void	Empty();
		//�ַ����Ƿ�Ϊ��
		BOOL	IsEmpty();
		//�ַ����Ƿ����
		BOOL	operator==(const CStringUtil &prmObj);
		//�ַ����Ƿ񲻵�
		BOOL	operator!=(const CStringUtil &prmObj);
		//��ǰ�ַ����Ƿ�С��prmObj����ʾ�ַ���
		BOOL	operator<(const CStringUtil &prmObj);
		//��ǰ�ַ����Ƿ�С�ڵ���prmObj����ʾ�ַ���
		BOOL	operator<=(const CStringUtil &prmObj);
		//��ǰ�ַ����Ƿ����prmObj����ʾ�ַ���
		BOOL	operator>(const CStringUtil &prmObj);
		//��ǰ�ַ����Ƿ���ڵ���prmObj����ʾ�ַ���
		BOOL	operator>=(const CStringUtil &prmObj);

		//�ַ��������ִ�Сд�Ƚ�
		int		CompareNoCase(const TCHAR *prmStr);
		//�ַ��������ִ�Сд�Ƚ�
		int		CompareNoCase(const CStringUtil &prmObj);
		//�ַ���ֻ�Ƚ�prmLen���ַ�,���ִ�Сд
		int		CompareNChar(const TCHAR *prmStr,size_t prmLen);
		//�ַ���ֻ�Ƚ�prmLen���ַ�,���ִ�Сд
		int		CompareNChar(const CStringUtil &prmObj,size_t prmLen);
		//�ַ���ֻ�Ƚ�prmLen���ַ��������ִ�Сд
		int		CompareNCharNoCase(const TCHAR *prmStr,size_t prmLen);
		//�ַ���ֻ�Ƚ�prmLen���ַ��������ִ�Сд
		int		CompareNCharNoCase(const CStringUtil &prmObj,size_t prmLen);

		//���ַ�����߿�ʼ��ȡprmLen���ַ�
		CStringUtil Left(int prmLen);
		//���ַ����ұ߿�ʼ��ȡprmLen���ַ�
		CStringUtil	Right(int prmLen);
		//��ȡ��prmStart��ʼ��prmEnd�������ַ�������Χ[prmStart,prmEnd)
		CStringUtil Mid(int prmStart,int prmEnd);

		//�����±����ã���ȡ��prmIndex���ַ�
		TCHAR	&operator[](int prmIndex);
		//��ȡ��prmIndex���ַ�
		TCHAR	GetAt(int prmIndex);

		//��ȡ�ַ���ʵ�ʻ�������ַ
		LPCTSTR GetString()const {return m_Buf;}
		//��ȡ�ַ�������
		int		GetLength(){return m_BufLen;}

		//�ӵ�ǰ�ַ����в���prmStr�Ӵ�������ҵ����򷵻����ַ������±�ֵ�����򷵻�-1
		//���ҷ�ʽΪ�������ҿ�ʼ��
		int		FindString(const TCHAR *prmStr);
		//�ӵ�ǰ�ַ����в���prmObj����ʾ�Ӵ�������ҵ����򷵻����ַ������±�ֵ�����򷵻�-1
		//���ҷ�ʽΪ�������ҿ�ʼ��
		int		FindString(const CStringUtil &prmObj);
		//�ӵ�ǰ�ַ����в���PrmStr�Ӵ�������ҵ����򷵻����ַ������±�ֵ�����򷵻�-1
		//���ҷ�ʽΪ�����������
		int		RFindString(const TCHAR *prmStr);
		//�ӵ�ǰ�ַ����в���PrmStr�Ӵ�������ҵ����򷵻����ַ������±�ֵ�����򷵻�-1
		//���ҷ�ʽΪ�����������
		int		RFindString(const CStringUtil &prmObj);
		//�������Ҳ��ҵ�ǰ�ַ������Ƿ����ַ�prmChar��������򷵻��ַ����ֵ�ַ�����򷵻�NULL
		TCHAR	*StrChar(const TCHAR prmChar);
		//����������ҵ�ǰ�ַ������Ƿ����ַ�prmChar��������򷵻��ַ����ֵ�ַ�����򷵻�NULL
		TCHAR	*StrrChar(const TCHAR prmChar);

		//��ǰ�ַ����Ƿ���prmStr��ʼ
		BOOL	StartWith(const TCHAR *prmStr);
		//��ǰ�ַ����Ƿ���prmObj����ʾ�ַ�����ʼ
		BOOL	StartWith(const CStringUtil &prmObj);
		//��ǰ�ַ����Ƿ���prmStr����
		BOOL	EndWith(const TCHAR *prmStr);
		//��ǰ�ַ����Ƿ���prmObj����ʾ�ַ�������
		BOOL	EndWith(const CStringUtil &prmObj);

		//��ǰ�ַ����Ƿ���prmStr��ʼ�����Դ�Сд
		BOOL	StartWithNoCase(const TCHAR *prmStr);
		//��ǰ�ַ����Ƿ���prmObj����ʾ�ַ�����ʼ�����Դ�Сд
		BOOL	StartWithNoCase(const CStringUtil &prmObj);
		//��ǰ�ַ����Ƿ���prmStr���������Դ�Сд
		BOOL	EndWithNoCase(const TCHAR *prmStr);
		//��ǰ�ַ����Ƿ���prmObj����ʾ�ַ������������Դ�Сд
		BOOL	EndWithNoCase(const CStringUtil &prmObj);

		//
		CStringUtil	&Trim();

		//���ַ�������Сд�ַ�ת��Ϊ��д�ַ�
		void	ToUpper();

		//���ַ������д�д�ַ�ת��ΪСд�ַ�
		void	ToLower();

		void	SplitString(vector<CStringUtil> &prmArray,LPCTSTR prmSplitStr);

		CStringUtil	ReplaceStr(TCHAR *prmSrc, TCHAR	*prmDst);

		//��ʽ���ַ���
		void		Format(TCHAR *prmForamt, ...);
	private:
		//���·���prmNewSize���ַ����ռ�
		BOOL	Assign(size_t prmNewSize);
		//�Ƚ������ַ�������ȷ���0��С�ڷ���-1�����ڷ���1
		int		Compare(const TCHAR *prmStr1,const TCHAR *prmStr2);
		//�ж��Ƿ�Ϊ�ո��ַ�
		BOOL	IsSpaceChar(int prmCharVal);
	private:
		TCHAR		*m_Buf;//���ڴ��ʵ���ַ���
		UINT32		m_BufLen;//���ڱ�ʾ�ַ���ʵ�ʳ���
		UINT32		m_Capacity;//��ǰ������ַ�ʵ�ʿ����ɶ����ַ�
	};
}

#endif
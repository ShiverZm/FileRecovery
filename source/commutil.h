/***************************************************
*
*	@Version:	1.0
*	@Author:	Mengxl
*	@Date:		2017-8-27 22:08
*	@File:		commutil.h
*
****************************************************/

#ifndef COMM_UTIL_INCLUDE_H
#define COMM_UTIL_INCLUDE_H
#include <windows.h>
#include<stdlib.h>

#include "StringUtil.h"

using namespace commutil;
//���ڽ������ֽڽ��д�С��ת��
void	rev16bit(UCHAR *value);

//���ڽ��ĸ��ֽڽ��д�С��ת��
void	rev32bit(UCHAR *value);

//���ڽ�8���ֽڽ��д�С��ת��
void	rev64bit(UCHAR *value);

#pragma pack(push,1)

//�ļ�������ռ�õ����ݿ飬ÿ���ļ�����ռ�������Ĵأ�Ҳ����ռ�ò������Ķ���ء�
typedef struct _File_Content_Extent
{
	UINT64	startSector;//�ڷ����е���ʼ����
	UINT64	totalSector;//�����Ŀ��С����λΪ����
	UINT8	isPersist;//�ļ������Ƿ�Ϊ��פ���ԣ�ֻ��ntfs��Ч

	_File_Content_Extent *next;//ָ����һ���ļ���
	_File_Content_Extent()
	{
		isPersist = 0;
		startSector = 0;
		totalSector = 0;
		next = NULL;
	}
}File_Content_Extent_s;
#pragma pack(pop)

#pragma pack(push,1)

typedef struct _FAT32 {
	char BS_jmpBoot[3];
	char BS_OEMName[8];
	unsigned short BPB_BytsPerSec;
	unsigned char BPB_SecPerClus;
	unsigned short BPB_ResvdSecCnt;
	unsigned char BPB_NumFATs;
	unsigned short BPB_RootEntCnt;
	unsigned short BPB_TotSec16;
	char BPB_Media;
	unsigned short BPB_FATSz16;
	unsigned short BPB_SecPerTrk;
	unsigned short NumHeads;
	unsigned int BPB_HiddSec;
	unsigned int BPB_TotSec32;
	unsigned int BPB_FATSz32;
	unsigned short BPB_flags;
	unsigned short BPB_FSVer;
	unsigned int BPB_RootClus;
	unsigned short BPB_FSInfo;
	unsigned short BPB_BkBootSec;
	char BPB_Reserved[12];
	unsigned char BS_DrvNum;
	unsigned char BS_Reserved1;
	char BS_BootSig;
	unsigned int BS_VolID;
	char BS_VolLAB[11];
	char BS_FilSysType[8];
	char code[420];
	char bootsig[2];
}FAT32_s;

typedef struct dir_entry {
	unsigned char   name[11];/* name and extension */
	unsigned char   attr;           /* attribute bits */
	unsigned char   lcase;          /* Case for base and extension */
	unsigned char   ctime_cs;       /* Creation time, centiseconds (0-199) */
	unsigned short  ctime;          /* Creation time */
	unsigned short  cdate;          /* Creation date */
	unsigned short  adate;          /* Last access date */
	unsigned short  starthi;        /* High 16 bits of cluster in FAT32 */
	unsigned short  time, date, start;/* time, date and first cluster */
	unsigned int    size;           /* file size (in bytes) */
}DIR_ENTRY_s;

//�ļ������ر�־
#define EOC 0x0FFFFFFF

typedef struct dir_long_entry {
	unsigned char    id;             /* sequence number for slot */
	unsigned char    name0_4[10];    /* first 5 characters in name */
	unsigned char    attr;           /* attribute byte */
	unsigned char    reserved;       /* always 0 */
	unsigned char    alias_checksum; /* checksum for 8.3 alias */
	unsigned char    name5_10[12];   /* 6 more characters in name */
	unsigned short   start;         /* starting cluster number, 0 in long slots */
	unsigned char    name11_12[4];   /* last 2 characters in name */
}DIR_LONG_ENTRY_s;

#pragma pack(pop)

#pragma pack(push,1)

#define MFTREFMASK	0xFFFFFFFFFFFF

//ö������,ö��ntfs���е���������
enum NTFS_ATTRDEF
{
	ATTR_STANDARD = 0x10,//��׼��Ϣ����
	ATTR_ATTRIBUTE_LIST = 0x20,//�����б�
	ATTR_FILE_NAME = 0x30,//�ļ���
	ATTR_VOLUME_VERSION = 0x40,//��汾����
	ATTR_SECURITY_DESCRIPTOR = 0x50,//��ȫ��������
	ATTR_VOLUME_NAME = 0x60,//������
	ATTR_VOLUME_INFOMATION = 0x70,//����Ϣ
	ATTR_DATA = 0x80,//��������
	ATTR_INDEX_ROOT = 0x90,//������Ŀ¼����
	ATTR_INDEX_ALLOCATION = 0xA0,//�ⲿ������Ŀ¼
	ATTR_BITMAP = 0xB0,//λͼ
	ATTR_SYMLINK = 0xC0,//��������
	ATTR_HPFS_EXTENDED_INFO = 0xD0,//hpfs��չ��Ϣ
	ATTR_HPFS_EXTENDED = 0xE0,//hpfs��չ
	ATTR_PROPERTY = 0xF0,//����Ȩ��������
	ATTR_LOG_STREAM = 0x100//��־����������
};

typedef struct _NTFS_Data_Run
{
	UINT64				lcn;   //���������߼��غ�
	UINT64				vcn;	//������������غ�
	UINT64				length;	//���ȣ���ʾ��������ռ�ö��ٸ���
	_NTFS_Data_Run		*next;
	_NTFS_Data_Run()
	{
		lcn = 0;
		vcn = 0;
		length = 0;
		next = NULL;
	}
}Ntfs_Data_Run;
#pragma pack(pop)

#endif
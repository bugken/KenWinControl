#ifndef __CONFIGFILE_H__
#define __CONFIGFILE_H__

class CConfigFile
{
public:
	CConfigFile(const char *pszFilename);
	virtual ~CConfigFile();

	virtual bool GetShort(const char *secName,
		const char *keyName,
		short &retValue,
		short defValue);

	virtual bool GetShort(const char *secName,
		const char *keyName,
		unsigned short &retValue,
		unsigned short defValue);

	virtual bool GetInt(const char *secName,
		const char *keyName,
		int &retValue,
		int defValue);

	virtual bool GetInt(const char *secName,
		const char *keyName,
		unsigned int &retValue,
		unsigned int defValue);

	virtual bool GetLongLong(const char *secName,
		const char *keyName,
		long long &retValue,
		long long defValue);

	virtual bool GetLongLong(const char *secName,
		const char *keyName,
		unsigned long long &retValue,
		unsigned long long defValue);

	virtual bool GetString(const char *secName,
		const char *keyName,
		char *retValue,
		unsigned int nSize,
		const char *defValue);

	virtual void SetReadRelay()
	{
		mOldLocation = m_pszShadow;
	}

private:

	char    *m_pszFilename;     /* 存放需要读取的配置文件名 */

	unsigned int IsOpen();

	bool GetItemValueULL(const char *secName,
		const char *keyName,
		unsigned long long defValue);

	bool GetItemValueString(const char *secName,
		const char *keyName,
		char *retValue,
		unsigned int nSize);

	void CloseFile();

private:
	bool OpenFile(const char *pszFilename);

	unsigned int LocateSection(const char *secName,
		char * &pszSectionBegin,
		char * &pszSectionEnd);
	unsigned int LocateKeyRange(const char *keyName,
		const char *pszSectionBegin,
		const char *pszSectionEnd,
		char * &pszKeyBegin,
		char * &pszKeyEnd);
	unsigned int LocateKeyValue(const char *keyName,
		const char *pszSectionBegin,
		const char *pszSectionEnd,
		char * &pszValueBegin,
		char * &pszValueEnd);
	char *LocateStr(const char *pszCharSet,
		const char *pszBegin,
		const char *pszEnd);
	char *SearchMarchStr(const char *pszBegin, const char *pszCharSet);

	char *MapToContent(const char *p);
	char *MapToShadow(const char *p);

	void ToLower(char * pszSrc, size_t len);

private:
	char    *mOldLocation;
	char    *m_pszContent;      /* 配置文件的原始内容 */
	char    *m_pszShadow;       /* 配置文件的内容全部转换成小写 */
	size_t  m_nSize;            /* 配置文件内容的长度，不包括最后的NULL */
	short   m_bIsOpen;          /* 配置文件是否打开成功的标志 */

	unsigned long long mReadValue;
};

#endif


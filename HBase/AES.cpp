
#include "AES.h"
#include "rijndael/rijndael.h"

H_BNAMSP

/*
���һ���ʣ���ֽ���ȫ�����϶�����ֽ������������һ���СΪ16�ֽڣ�5�ֽ���Ч��
����������ֽ����11(16-5),��ʾ��11���ֽ���Ч���������ܺ��ȥ�ⲿ�־ͺ��ˡ�
���ԭʼ���ݳ��ȸպ���16����������������һ��block,����Ϊ16,
��ʾ��blockȫ��Ϊ��Ч���ݡ�
*/

CAES::CAES(void) : m_pEncodeRK(NULL), m_pDecodeRK(NULL), 
    m_iEncodeRounds(H_INIT_NUMBER), m_iDecodeRounds(H_INIT_NUMBER)
{
   
}

CAES::~CAES(void)
{
    H_SafeDelArray(m_pEncodeRK);
    H_SafeDelArray(m_pDecodeRK);
}

void CAES::setKey(const char *pszKey, const unsigned int uiKeyType)
{
    size_t iRKLens(RKLENGTH(uiKeyType));
    size_t iKeyLens(KEYLENGTH(uiKeyType));

    m_pEncodeRK = new(std::nothrow) unsigned long[iRKLens + 1];
    H_ASSERT(NULL != m_pEncodeRK, "malloc memory error.");
    H_Zero(m_pEncodeRK, iRKLens + 1);

    m_pDecodeRK = new(std::nothrow) unsigned long[iRKLens + 1];
    H_ASSERT(NULL != m_pDecodeRK, "malloc memory error.");
    H_Zero(m_pDecodeRK, iRKLens + 1);

    unsigned char * pKey = new(std::nothrow) unsigned char[iKeyLens + 1];
    H_ASSERT(NULL != pKey, "malloc memory error.");
    H_Zero(pKey, iKeyLens + 1);
    memcpy(pKey, pszKey, strlen(pszKey) > iKeyLens ? iKeyLens : strlen(pszKey));

    m_iEncodeRounds = rijndaelSetupEncrypt(m_pEncodeRK, pKey, uiKeyType);
    m_iDecodeRounds = rijndaelSetupDecrypt(m_pDecodeRK, pKey, uiKeyType);

    H_SafeDelArray(pKey);
}

std::string CAES::Encode(const char *pszPlaint, const size_t iLens)
{
    size_t iCopyLen(H_INIT_NUMBER);
    bool bFill(false);
    unsigned char uacPlain[AES_BlockSize];
    unsigned char uacCipher[AES_BlockSize];
    std::string strRtn;

    if (0 == iLens % AES_BlockSize)
    {
        bFill = true;
    }

    unsigned char *pPlain;
    for (size_t i = 0; i < iLens; i += AES_BlockSize)
    {
        iCopyLen = ((iLens - i) >= AES_BlockSize) ? (AES_BlockSize) : (iLens - i);         
        if (iCopyLen < AES_BlockSize)//����һ����Բ���ֽ������
        {
            memcpy(uacPlain, pszPlaint + i, iCopyLen);
            memset(uacPlain + iCopyLen, (int)(AES_BlockSize - iCopyLen), AES_BlockSize - iCopyLen);
            pPlain = uacPlain;
        }
        else
        {
            pPlain = (unsigned char*)(pszPlaint + i);
        }
        rijndaelEncrypt(m_pEncodeRK, m_iEncodeRounds, pPlain, uacCipher);
        
        strRtn.append((const char*)uacCipher, AES_BlockSize);
    }

    if (bFill)//���ȸպ�Ϊһ������������ټ�һ�����
    {
        memset(uacPlain, AES_BlockSize, AES_BlockSize);
        rijndaelEncrypt(m_pEncodeRK, m_iEncodeRounds, uacPlain, uacCipher);
        strRtn.append((const char*)uacCipher, AES_BlockSize);
    }

    return strRtn;
}

std::string CAES::Decode(const char *pszCipher, const size_t iLens)
{
    unsigned char m_uacPlain[AES_BlockSize];
    std::string strRtn;
    if (0 != (iLens % AES_BlockSize))
    {
        return "";
    }

    for (size_t i = 0; i < iLens; i += AES_BlockSize)
    {
        rijndaelDecrypt(m_pDecodeRK, m_iDecodeRounds, (const unsigned char*)(pszCipher + i), m_uacPlain);
        strRtn.append((const char*)m_uacPlain, AES_BlockSize);
    }

    strRtn.erase(strRtn.size() - (size_t)m_uacPlain[AES_BlockSize - 1], strRtn.size());

    return strRtn;
}

H_ENAMSP

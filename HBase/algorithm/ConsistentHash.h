
#ifndef H_CONSISTENTHASH_H_
#define H_CONSISTENTHASH_H_

#include "HObject.h"

H_BNAMSP

//consistenthashing
class CConHash : public CObject
{
public:
    CConHash(void);
    ~CConHash(void);

    //��ӽڵ� pszNode �ڵ���   iReplica ����ڵ���
    bool addNode(const char *pszNode, size_t iReplica);
    //ɾ���ڵ�
    void delNode(const char *pszNode);
    //���������Ǹ��ڵ�
    const char *findNode(const char *pszObject);
    //����ڵ���
    size_t getVNodeNum(void);

private:
#ifdef H_OS_WIN
    typedef std::unordered_map<std::string, void * >::iterator nodeit;
    typedef std::unordered_map<std::string, void * > node_map;
#else
    typedef std::tr1::unordered_map<std::string, void * >::iterator nodeit;
    typedef std::tr1::unordered_map<std::string, void * > node_map;
#endif

    void *m_pConHash;
    node_map m_mapNode;
};

H_ENAMSP

#endif//H_CONSISTENTHASH_H_

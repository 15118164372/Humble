
#ifndef H_CONSISTENTHASH_H_
#define H_CONSISTENTHASH_H_

#include "Macros.h"

H_BNAMSP

//consistenthashing
class CConHash
{
public:
    CConHash(void);
    ~CConHash(void);

    //添加节点 pszNode 节点名   iReplica 虚拟节点数
    bool addNode(const char *pszNode, size_t iReplica);
    //删除节点
    void delNode(const char *pszNode);
    //查找属于那个节点
    const char *findNode(const char *pszObject);
    //虚拟节点数
    size_t getVNodeNum(void);

private:
    H_DISALLOWCOPY(CConHash);
#ifdef H_OS_WIN
    #define nodeit std::unordered_map<std::string, void * >::iterator
    #define node_map std::unordered_map<std::string, void * >
#else
    #define nodeit std::tr1::unordered_map<std::string, void * >::iterator
    #define node_map std::tr1::unordered_map<std::string, void * >
#endif

private:
    void *m_pConHash;
    node_map m_mapNode;
};

H_ENAMSP

#endif//H_CONSISTENTHASH_H_

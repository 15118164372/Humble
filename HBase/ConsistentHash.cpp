
#include "ConsistentHash.h"
#include "Funcs.h"
#include "conhash/conhash.h"

H_BNAMSP

CConHash::CConHash(void)
{
    m_pConHash = conhash_init(H_HashStr);
}

CConHash::~CConHash(void)
{
    if (NULL != m_pConHash)
    {
        conhash_fini((struct conhash_s *)m_pConHash);
        m_pConHash = NULL;
    }

    struct node_s *pNode;
    for (nodeit itNode = m_mapNode.begin(); m_mapNode.end() != itNode; ++itNode)
    {
        pNode = (struct node_s *)itNode->second;
        H_SafeFree(pNode);
    }
    m_mapNode.clear();
}

bool CConHash::addNode(const char *pszNode, size_t iReplica)
{
    H_ASSERT(strlen(pszNode) < 64, "node name is too long.");

    struct node_s *pNode = (struct node_s *)malloc(sizeof(struct node_s));
    H_ASSERT(NULL != pNode, "malloc memory error.");

    conhash_set_node(pNode, pszNode, (u_int)iReplica);
    if (H_RTN_OK != conhash_add_node((struct conhash_s *)m_pConHash, pNode))
    {
        H_SafeFree(pNode);
        return false;
    }

    m_mapNode[pszNode] = pNode;

    return true;
}

void CConHash::delNode(const char *pszNode)
{
    nodeit itNode = m_mapNode.find(pszNode);
    if (m_mapNode.end() == itNode)
    {
        return;
    }

    struct node_s *pNode((struct node_s *)itNode->second);
    (void)conhash_del_node((struct conhash_s *)m_pConHash, pNode);

    m_mapNode.erase(itNode);
    H_SafeFree(pNode);
}

const char *CConHash::findNode(const char *pszObject)
{
    struct node_s *pNode = conhash_lookup((struct conhash_s *)m_pConHash, pszObject);
    if (NULL == pNode)
    {
        return NULL;
    }

    return pNode->iden;
}

size_t CConHash::getVNodeNum(void)
{
    return conhash_get_vnodes_num((struct conhash_s *)m_pConHash);
}

H_ENAMSP

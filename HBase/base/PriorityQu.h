
#ifndef H_PRIORITYQU_H_
#define H_PRIORITYQU_H_

#include "HMutex.h"

H_BNAMSP

//优先级队列 节点
class CPriQuNode : public CObject
{
public:
    CPriQuNode(void *pData, const uint64_t &ulMark, const uint64_t &ulId) :
        m_pData(pData), m_ulMark(ulMark), m_ulId(ulId)
    {};
    ~CPriQuNode(void)
    {};

    H_INLINE void *getData(void)
    {
        return m_pData;
    };
    H_INLINE const uint64_t &getId(void)
    {
        return m_ulId;
    };
    H_INLINE const uint64_t &getMark(void)
    {
        return m_ulMark;
    };

private:
    CPriQuNode(void);
    void *m_pData;
    uint64_t m_ulMark;
    uint64_t m_ulId;
};

struct PriQuNodeCMP
{
    bool operator () (CPriQuNode *stSrc, CPriQuNode *stTag)
    {
        return stSrc->getMark() > stTag->getMark();
    };
};

//优先级队列
class CPriorityQu : public CObject
{
public:
    CPriorityQu(void) 
    {};
    ~CPriorityQu() 
    {
        CPriQuNode *pNode;
        while (!m_priorityQu.empty())
        {
            pNode = m_priorityQu.top();
            m_priorityQu.pop();
            H_SafeDelete(pNode);
        }
    };

    void Push(CPriQuNode *pNode)
    {
        m_objLck.Lock();
        m_priorityQu.push(pNode);
        m_objLck.unLock();
    };
    void Push(void *pData, const uint64_t &ulMark, const uint64_t &ulId)
    {
        CPriQuNode *pNode = new(std::nothrow) CPriQuNode(pData, ulMark, ulId);
        H_ASSERT(NULL != pNode, H_ERR_MEMORY);

        Push(pNode);
    };
    //返回值需要delete
    CPriQuNode *Pop(const uint64_t &ulMark)
    {
        m_objLck.Lock();
        if (!m_priorityQu.empty())
        {
            CPriQuNode *pNode(m_priorityQu.top());
            if (ulMark >= pNode->getMark())
            {
                m_priorityQu.pop();
                m_objLck.unLock();

                return pNode;
            }
        }
        m_objLck.unLock();
        
        return NULL;
    };

private:
    typedef std::priority_queue<CPriQuNode *, std::vector<CPriQuNode *>, PriQuNodeCMP> priority_queue;

    priority_queue m_priorityQu;
    CMutex m_objLck;
};

H_ENAMSP

#endif//H_PRIORITYQU_H_

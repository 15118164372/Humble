
#ifndef H_PRIORITYQU_H_
#define H_PRIORITYQU_H_

#include "HMutex.h"

H_BNAMSP

//优先级队列 节点
class CPriQuNode : public CObject
{
public:
    CPriQuNode(class CWorker *pWork, const uint64_t &ulMark, const uint64_t &ulId) :
        m_pWork(pWork), m_ulMark(ulMark), m_ulId(ulId)
    {};
    ~CPriQuNode(void)
    {};

    class CWorker *getWork(void)
    {
        return m_pWork;
    };
    const uint64_t &getId(void)
    {
        return m_ulId;
    };
    const uint64_t &getMark(void)
    {
        return m_ulMark;
    };

private:
    CPriQuNode(void);
    class CWorker *m_pWork;
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
    void Push(class CWorker *pWork, const uint64_t &ulMark, const uint64_t &ulId)
    {
        CPriQuNode *pNode = new(std::nothrow) CPriQuNode(pWork, ulMark, ulId);
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

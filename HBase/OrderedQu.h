
#ifndef H_ORDEREDQU_H_
#define H_ORDEREDQU_H_

#include "Macros.h"

H_BNAMSP

class COrderedQu
{
private:
    struct OrderNode
    {
        uint64_t m_iMark;
        uint64_t m_iId;
        OrderNode(void): m_iMark(H_INIT_NUMBER), m_iId(H_INIT_NUMBER)
        {};
        OrderNode(uint64_t iMark, uint64_t iId) : m_iMark(iMark), m_iId(iId)
        {};
    };
    struct OrderNodeCMP
    {
        bool operator () (const OrderNode &stSrc, const OrderNode &stTag)
        {
            return stSrc.m_iMark > stTag.m_iMark;
        }
    };

public:
    COrderedQu(void) {};
    virtual ~COrderedQu(void) {};

    void pushNode(uint64_t iMark, uint64_t iId)
    {
        OrderNode stNode(iMark, iId);
        m_quOrdered.push(stNode);
    };

    std::vector<uint64_t> popNode(uint64_t iMark)
    {
        OrderNode stNode;
        std::vector<uint64_t> vcIds;

        while (!m_quOrdered.empty())
        {
            stNode = m_quOrdered.top();
            if (iMark >= stNode.m_iMark)
            {
                m_quOrdered.pop();
                vcIds.push_back(stNode.m_iId);
                continue;
            }
            break;
        }

        return vcIds;
    }

private:
    #define ordered_queue std::priority_queue<OrderNode, std::vector<OrderNode>, OrderNodeCMP>
    ordered_queue m_quOrdered;
};

H_ENAMSP

#endif//H_ORDEREDQU_H_

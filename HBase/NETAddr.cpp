
#include "NETAddr.h"

H_BNAMSP

CNETAddr::CNETAddr(void) : m_Addr_Type(IPV4)
{
    Clear();
}

CNETAddr::CNETAddr(bool ipv6)
{
    m_Addr_Type = (ipv6 ? IPV6 : IPV4);
    Clear();
}

CNETAddr::~CNETAddr(void)
{

}

void CNETAddr::Clear(void)
{
    H_Zero(&m_ipv4, sizeof(m_ipv4));
    H_Zero(&m_ipv6, sizeof(m_ipv6));

    m_ipv4.sin_family = AF_INET;
    m_ipv6.sin6_family = AF_INET6;
}

int CNETAddr::setAddr(const char *pszHostName, unsigned short usPort, bool bIpv6)
{
    struct addrinfo stAddrInfo = {0};
    struct addrinfo *pAddrInfo = NULL;
    int  iRtn = H_RTN_OK;

    Clear();

    if (!bIpv6)
    {
        m_Addr_Type = IPV4;
        stAddrInfo.ai_flags  = AI_PASSIVE;
        stAddrInfo.ai_family = AF_INET;
    }
    else
    {
        m_Addr_Type = IPV6;
        stAddrInfo.ai_flags  = AI_PASSIVE;
        stAddrInfo.ai_family = AF_INET6;
    }

    iRtn = getaddrinfo(pszHostName, NULL, &stAddrInfo, &pAddrInfo);
    if (H_RTN_OK != iRtn)
    {
        H_Printf("getaddrinfo error host %s. return code %d, message %s.", 
            pszHostName, iRtn, gai_strerror(iRtn));

        if (NULL != pAddrInfo)
        {
            freeaddrinfo(pAddrInfo);
        }

        return iRtn;
    }

    if (AF_INET == pAddrInfo->ai_family)
    {
        memcpy(&m_ipv4, pAddrInfo->ai_addr, pAddrInfo->ai_addrlen);
        m_ipv4.sin_port = htons(usPort);
    }
    else
    {
        memcpy(&m_ipv6, pAddrInfo->ai_addr, pAddrInfo->ai_addrlen);
        m_ipv6.sin6_port = htons(usPort);
    }

    freeaddrinfo(pAddrInfo);

    return H_RTN_OK;
}

int	CNETAddr::setAddr(const struct sockaddr *pAddr)
{
    if (NULL == pAddr)
    {
        return H_RTN_ERROR;
    }

    Clear();

    if (AF_INET == pAddr->sa_family)
    {
        m_Addr_Type = IPV4;
        memcpy(&m_ipv4, pAddr, sizeof(m_ipv4));
    }
    else
    {
        m_Addr_Type = IPV6;
        memcpy(&m_ipv6, pAddr, sizeof(m_ipv6));
    }

    return H_RTN_OK;
}

int CNETAddr::setRemoteAddr(const H_SOCK &fd)
{
    if (H_INVALID_SOCK == fd)
    {
        return H_RTN_FAILE;
    }

    sockaddr_storage stSockAddrStor;
    int iSockStorLens = sizeof(sockaddr_storage);
    H_Zero(&stSockAddrStor, sizeof(sockaddr_storage));

    Clear();

    int iRtn = getpeername(fd, (sockaddr*)&stSockAddrStor, (socklen_t*) &iSockStorLens);
    if (H_RTN_OK != iRtn)
    {
        iRtn = H_SockError();
        H_Printf("getpeername error. error code %d, message %s", iRtn, H_SockError2Str(iRtn));

        return iRtn;
    }

    return setAddr((sockaddr*)&stSockAddrStor);
}

int CNETAddr::setLocalAddr(const H_SOCK &fd)
{
    sockaddr_storage stSockAddrStor;
    int iSockAddrStorLen = sizeof(sockaddr_storage);
    H_Zero(&stSockAddrStor, sizeof(sockaddr_storage));

    Clear();

    int iRtn  = getsockname(fd, (sockaddr*)&stSockAddrStor, (socklen_t*)&iSockAddrStorLen);
    if (H_RTN_OK != iRtn)
    {
        iRtn = H_SockError();
        H_Printf("getsockname error. error code %d, message %s", iRtn, H_SockError2Str(iRtn));

        return iRtn;
    }

    return setAddr((sockaddr*)&stSockAddrStor);
}

const sockaddr *CNETAddr::getAddr(void) const
{
    if (IPV4 == m_Addr_Type)
    {
        return (const sockaddr*)&m_ipv4;
    }
    else
    {
        return (const sockaddr*)&m_ipv6;
    }
}

size_t CNETAddr::getAddrSize(void) const
{
    if (IPV4 == m_Addr_Type)
    {
        return sizeof(m_ipv4);
    }
    else
    {
        return sizeof(m_ipv6);
    }
}

std::string CNETAddr::getIp(void) const
{
    int iRtn = H_RTN_OK;
    char acTmp[200] = {0};
    int iLen = sizeof(acTmp);

    if (IPV4 == m_Addr_Type)
    {        
        iRtn = getnameinfo((const sockaddr*)&m_ipv4, (socklen_t)sizeof(m_ipv4), acTmp, iLen, NULL, 0, NI_NUMERICHOST);
    }
    else
    {
        iRtn = getnameinfo((const sockaddr*)&m_ipv6, (socklen_t)sizeof(m_ipv6), acTmp, iLen, NULL, 0, NI_NUMERICHOST);
    }

    if (H_RTN_OK != iRtn)
    {
        H_Printf("getnameinfo error. return code %d, message %s", iRtn, gai_strerror(iRtn));

        return std::string("");
    }

    return std::string(acTmp);
}

unsigned short CNETAddr::getPort(void)
{
    if (IPV4 == m_Addr_Type)
    {
        return ntohs(m_ipv4.sin_port);
    }
    else
    {
        return ntohs(m_ipv6.sin6_port);
    }
}

bool CNETAddr::is_ipv4() const
{
    return (IPV4 == m_Addr_Type);
}

bool CNETAddr::is_ipv6() const
{
    return (IPV6 == m_Addr_Type);
}

unsigned int CNETAddr::ipToNum(const char *pszIp)
{
    return (unsigned int)inet_addr(pszIp);
}

std::string CNETAddr::numToIp(const unsigned int &uiIp)
{
    char acIp[H_IPLENS] = {0};
    const unsigned char *pIp = (unsigned char *)&uiIp;

    H_Snprintf(acIp, sizeof(acIp) - 1, 
        "%u.%u.%u.%u", pIp[0], pIp[1], pIp[2], pIp[3]);

    return std::string(acIp);
}

H_ENAMSP

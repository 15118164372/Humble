
#ifndef H_NETADDR_H_
#define H_NETADDR_H_

#include "Macros.h"

H_BNAMSP

/*
网络地址
*/
class CNETAddr
{
public:
    CNETAddr(void);
    explicit CNETAddr(bool ipv6);
    ~CNETAddr(void);

    /*清空数据*/
    void Clear(void);

    /*设置sockaddr_in*/
    int setAddr(const char *pszHostName, unsigned short usPort, bool bIpv6 = false);
    /*设置sockaddr_in*/
    int	setAddr(const struct sockaddr *pAddr);
    /*根据socket句柄获取远端地址信息*/
    int setRemoteAddr(const H_SOCK &fd);
    /*根据socket句柄获取本地地址信息*/
    int setLocalAddr(const H_SOCK &fd);
    /*返回sockaddr句柄*/
    const sockaddr *getAddr(void) const;
    /*获取地址内存长度*/
    size_t getAddrSize(void) const;
    /*获取IP*/
    std::string getIp(void) const;
    /*获取端口*/
    unsigned short getPort(void);
    /*是否为ipv4*/
    bool is_ipv4() const;
    /*是否为ipv6*/
    bool is_ipv6() const;
    /*ip转数字*/
    static unsigned int ipToNum(const char *pszIp);
    static std::string numToIp(const unsigned int &uiIp);

private:
    H_DISALLOWCOPY(CNETAddr);
    enum NetAdrrType
    {
        IPV4,
        IPV6
    };

    NetAdrrType m_Addr_Type;
    sockaddr_in	m_ipv4;
    sockaddr_in6 m_ipv6;
};

H_ENAMSP

#endif//H_NETADDR_H_

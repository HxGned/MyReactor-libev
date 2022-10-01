#include <unistd.h>
#include <fcntl.h>

#include "Socket.hh"

using namespace std;
using namespace base;
using namespace net;

#ifndef BACKLOG_SIZE
#define BACKLOG_SIZE 1024
#endif

// namespace net class Socket
int Socket::Bind(INetAddr& listenAddr)
{
    sockaddr_in inAddr = *(listenAddr.GetSockAddr());
    socklen_t sockLen = sizeof(inAddr);

    if (bind(this->_sockFd, (sockaddr*)&inAddr, sockLen) < 0) {
        perror("bind");
        LOG_FATAL("Bind failed");
    }

    return 0;
}

void Socket::ReuseAddr()
{
    Socket::SetSocketReuseAddr(this->_sockFd, true);
}

int Socket::Listen()
{
    if (listen(this->_sockFd, BACKLOG_SIZE) < 0) {
        perror("listen");
        LOG_FATAL("listen");
    }
    return 0;
}

int Socket::Accept(INetAddr& peerAddr)
{
    sockaddr sAddr;
    socklen_t sAddrLen = sizeof(sAddr);
    int fd = 0; // peer socket fd

    fd = accept(_sockFd, &sAddr, &sAddrLen);
    if (fd < 0) {
        perror("accept");
        LOG_ERROR("accept failed!");
    }
    peerAddr.SetSockAddr(sAddr);
    return fd;
}

int Socket::Close()
{
    int ret = 0;
    ret = close(this->_sockFd);
    if (ret < 0) {
        perror("close");
        LOG_ERROR("close socket fd[%d] failed!", this->_sockFd);
    }
    return ret;
}

int Socket::ShutdownWrite()
{
    int ret = ::shutdown(this->_sockFd, SHUT_WR);
    if (ret < 0) {
        perror("shutdown");
        LOG_ERROR("ShutdownWrite failed!");
    }

    return ret;
}

// public socket related functions
SOCKET Socket::CreateNonBlockingSocket()
{
    int flags = 0;
    int ret = 0;
    SOCKET fd = 0;

    fd = socket(AF_INET, SOCK_STREAM, 0);
    if (fd < 0) {
        perror("socket");
        LOG_FATAL("call socket failed!");
    }
    flags = fcntl(fd, F_GETFL, 0);
    if (flags < 0) {
        perror("fcntl");
        LOG_FATAL("call fcntl GETFL failed!");
    }
    flags |= O_NONBLOCK;
    ret = fcntl(fd, F_SETFL, flags);
    if (ret < 0) {
        perror("fcntl");
        LOG_FATAL("call fcntl SETFL failed!");
    }
    return fd;
}

void Socket::SetSocketNonBlocking(SOCKET fd)
{
    int flags = 0;
    int ret = 0;

    flags = fcntl(fd, F_GETFL, 0);
    if (flags < 0) {
        perror("fcntl");
        LOG_FATAL("call fcntl GETFL failed!");
    }
    flags |= O_NONBLOCK;
    ret = fcntl(fd, F_SETFL, flags);
    if (ret < 0) {
        perror("fcntl");
        LOG_FATAL("call fcntl SETFL failed!");
    }
}

void Socket::SetSocketReuseAddr(SOCKET fd, bool on)
{
    int optval = on ? 1 : 0;
    #ifdef WIN32
        ::setsockopt(sockfd, SOL_SOCKET, SO_REUSEADDR, (char*)&optval, sizeof(optval));
    #else
        ::setsockopt(fd, SOL_SOCKET, SO_REUSEADDR, &optval, static_cast<socklen_t>(sizeof optval));
    #endif
        // FIXME CHECK
}

sockaddr Socket::GetLocalSockaddr(SOCKET sockfd)
{
    sockaddr addr;
    socklen_t addrLen;

    bzero(&addr, sizeof(addr));
    addrLen = sizeof(addr);

    if (::getsockname(sockfd, &addr, &addrLen) < 0) {
        perror("getsockname");
        LOG_ERROR("GetLocalSockaddr failed");
    }

    return addr;
}

sockaddr Socket::GetRemoteSockaddr(SOCKET sockfd)
{
    sockaddr addr;
    socklen_t addrLen;

    bzero(&addr, sizeof(addr));
    addrLen = sizeof(addr);

    if (::getpeername(sockfd, &addr, &addrLen) < 0) {
        perror("getpeername");
        LOG_ERROR("GetRemoteSockaddr failed");
    }

    return addr;
}

int Socket::GetSocketError(int sockfd)
{
    int optVal = 0;
    socklen_t optLen = 0;
    optLen = sizeof(optVal);

    if (::getsockopt(sockfd, SOL_SOCKET, SO_ERROR, &optVal, &optLen) < 0) {
        perror("getsockopt");
        return errno;
    }
    
    return optVal;
}

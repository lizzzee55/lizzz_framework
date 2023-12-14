#include <time.h>

class lizzz_socket
{

public:
	static int Connect(std::string host, int port = 80, int timeout = 10);
	static int connect_s(int sock, const char* ip, int port, int timeout = 10);

private:

};

#pragma once

#include "lizzz_dns.h"


#ifdef WIN32

inline int lizzz_socket::connect_s(int sock, const char* ip, int port, int timeout)
{
	TIMEVAL Timeout;
	Timeout.tv_sec = timeout;
	Timeout.tv_usec = 0;
	struct sockaddr_in address;  /* the libc network address data structure */


								 // printf("created %d\r\n", sock);

	address.sin_addr.s_addr = inet_addr(ip); /* assign the address */
	address.sin_port = htons(port);            /* translate int2port num */
	address.sin_family = AF_INET;

	lizzz_Log::Instance()->addLog("connect_s: " + to_string(sock));
	//set the socket in non-blocking
	unsigned long iMode = 1;
	int iResult = ioctlsocket(sock, FIONBIO, &iMode);
	if (iResult != 0)
	{
		lizzz_Log::Instance()->addLog("ioctlsocket failed with error: " + to_string(iResult));
		printf("ioctlsocket failed with error: %ld\n", iResult);
	}
	lizzz_Log::Instance()->addLog("Start conenct");
	if (connect(sock, (struct sockaddr*)&address, sizeof(address)) == false)
	{
		return false;
	}
	lizzz_Log::Instance()->addLog("Conenct ok!");
	// restart the socket mode
	iMode = 0;
	iResult = ioctlsocket(sock, FIONBIO, &iMode);
	if (iResult != 0)
	{
		printf("ioctlsocket failed with error: %ld\n", iResult);
	}

	fd_set Write, Err;
	FD_ZERO(&Write);
	FD_ZERO(&Err);
	FD_SET(sock, &Write);
	FD_SET(sock, &Err);
	
	lizzz_Log::Instance()->addLog("Start select");
	//return true;
	// check if the socket is ready
	select(0, NULL, &Write, &Err, &Timeout);
	if (FD_ISSET(sock, &Write))
	{
		lizzz_Log::Instance()->addLog("Select ok!");
		return true;
	}
	
	lizzz_Log::Instance()->addLog("Select error!");
	printf("Timeout socket %d\r\n", sock);

	return false;
}

#else

inline int lizzz_socket::connect_s(int sockfd, const char* ip, int port, int timeout)
{
	int rc = 0;
	timeout = timeout * 1000;

	struct sockaddr_in address;
	address.sin_addr.s_addr = inet_addr(ip); /* assign the address */
	address.sin_port = htons(port);            /* translate int2port num */
	address.sin_family = AF_INET;

	// Set O_NONBLOCK
	int sockfd_flags_before;
	if ((sockfd_flags_before = fcntl(sockfd, F_GETFL, 0) < 0)) return -1;
	if (fcntl(sockfd, F_SETFL, sockfd_flags_before | O_NONBLOCK) < 0) return -1;
	// Start connecting (asynchronously)
	do {
		if (connect(sockfd, (struct sockaddr*)&address, sizeof(address)) < 0) {
			// Did connect return an error? If so, we'll fail.
			if ((errno != EWOULDBLOCK) && (errno != EINPROGRESS)) {
				rc = -1;
			}
			// Otherwise, we'll wait for it to complete.
			else {
				// Set a deadline timestamp 'timeout' ms from now (needed b/c poll can be interrupted)
				struct timespec now;
				if (clock_gettime(CLOCK_MONOTONIC, &now) < 0) { rc = -1; break; }
				struct timespec deadline;

				deadline.tv_sec = now.tv_sec;
				deadline.tv_nsec = now.tv_nsec + timeout * 1000000l;

				// Wait for the connection to complete.
				do {
					// Calculate how long until the deadline
					if (clock_gettime(CLOCK_MONOTONIC, &now) < 0) { rc = -1; break; }
					int ms_until_deadline = (int)((deadline.tv_sec - now.tv_sec) * 1000l
						+ (deadline.tv_nsec - now.tv_nsec) / 1000000l);
					if (ms_until_deadline < 0) { rc = 0; break; }
					// Wait for connect to complete (or for the timeout deadline)
					struct pollfd pfds[] = { { .fd = sockfd,.events = POLLOUT } };
					rc = poll(pfds, 1, ms_until_deadline);
					// If poll 'succeeded', make sure it *really* succeeded
					if (rc > 0) {
						int error = 0; socklen_t len = sizeof(error);
						int retval = getsockopt(sockfd, SOL_SOCKET, SO_ERROR, &error, &len);
						if (retval == 0) errno = error;
						if (error != 0) rc = -1;
					}
				}
				// If poll was interrupted, try again.
				while (rc == -1 && errno == EINTR);
				// Did poll timeout? If so, fail.
				if (rc == 0) {
					errno = ETIMEDOUT;
					rc = -1;
				}
			}
		}
	} while (0);
	// Restore original O_NONBLOCK state
	if (fcntl(sockfd, F_SETFL, sockfd_flags_before) < 0) return -1;
	// Success
	return rc;
}

#endif

inline int lizzz_socket::Connect(std::string host, int port, int timeout)
{
	lizzz_Log::Instance()->addLog("---- network ----");
	lizzz_Log::Instance()->addLog("Connect to " + host + ":" + to_string(port));
	
	in_addr addr;
	if (!DNS::GetDns(host.c_str(), &addr))
	{
		lizzz_Log::Instance()->addLog("Error DNS Server " + host);
		return 0;
	}
	std::string real_ip = inet_ntoa(addr);

	lizzz_Log::Instance()->addLog("Target  " + host + " ip " + real_ip + ":" + to_string(port));

	int fd = socket(AF_INET, SOCK_STREAM, IPPROTO_TCP);
	
	lizzz_Log::Instance()->addLog("Socket created fd:  " + to_string(fd));
	
	int iResult = connect_s(fd, real_ip.c_str(), port, timeout);
	//lizzz_Log::Instance()->addLog("Connect:  ");
	if (iResult <= 0) {
		lizzz_Log::Instance()->addLog("Error connect host:" + host + " ip:" + real_ip + " port:" + to_string(port) + " fd:" + to_string(fd));
		return 0;
	}
	
	
	if (fd > 1000000) {
		lizzz_Log::Instance()->addLog("Error max fd: 1000000");
		//printf("Error fd > 1 000 000 (%d)\r\n", fd);
		exit(1);
	}
	
	lizzz_Log::Instance()->addLog("Success connect:" + host + " ip:" + real_ip + " port:" + to_string(port) + " fd:" + to_string(fd));
	
	return fd;

}
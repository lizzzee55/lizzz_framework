
#include <string>
#include <time.h>
#include <fstream>

namespace DNS
{
	bool InitDns();
	bool GetDns(const char* argv, in_addr* addr);
	std::string getIp(std::string hostname);

	int getIpWithPach(unsigned char* buf, int n, in_addr* addr);
};

static std::string g_dns = "192.168.1.254";

std::string DNS::getIp(std::string hostname)
{
	in_addr addr;

	DNS::GetDns(hostname.c_str(), &addr);

	return inet_ntoa(addr);
}

bool DNS::InitDns()
{
	std::fstream file;
	file.open("/etc/resolv.conf", std::ios::in);

	bool ret = false;

	if (file.is_open())
	{
		char* buf = (char*)malloc(5000);

		do {
			const char* flag = "nameserver";

			file.read(buf, 5000);

			std::string strOptions = buf;

			int pos = strOptions.find(flag);

			std::string ip = strOptions.substr(pos, strOptions.length() - pos).c_str();

			if (pos == -1)
				break;

			pos = ip.find("\n");
			int len = strlen(flag) + 1;

			if (pos == -1)
			{
				ip = ip.substr(len, ip.length() - len);
			}
			else
			{
				ip = ip.substr(len, pos - len);
			}

			g_dns = ip;

			//infoLog("DNS Server Address : %s", ip.c_str());

			ret = true;

		} while (false);

		if (buf)
			free(buf);

		file.close();
	}

	return ret;
}



bool DNS::GetDns(const char* argv, in_addr* addr)
{
	struct sockaddr_in sa;
	int result = lizzz_inet_pton(AF_INET, argv, &(sa.sin_addr));
	if (result != 0)
	{
		*addr = sa.sin_addr;
		lizzz_Log::Instance()->addLog("log_service.txt", "Error GetDns:" + to_string(result) + " argv:" + argv);
		//LOG("result %d %s\r\n", result, argv);
		return true;
	}


	time_t ident;
	int fd;
	int rc;
	socklen_t serveraddrlent;
	const char* q;
	unsigned char* p;
	unsigned char* countp;
	unsigned char reqBuf[512] = { 0 };
	unsigned char rplBuf[512] = { 0 };
	sockaddr_in serveraddr;

	lizzz_Log::Instance()->addLog("Do");
	bool ret = false;

	do
	{
		//udp
		fd = socket(AF_INET, SOCK_DGRAM, 0);
		//fd = socket(AF_INET, SOCK_STREAM, IPPROTO_TCP);
		//lizzz_Log::Instance()->console("Fd %d\r\n", fd);
		//set timeout
#ifdef LINUX
		timeval tv_out;
		tv_out.tv_sec = 5;
		tv_out.tv_usec = 0;
		setsockopt(fd, SOL_SOCKET, SO_RCVTIMEO, (char*)&tv_out, sizeof(timeval));

#else
		int outtime = 5000;
		setsockopt(fd, SOL_SOCKET, SO_RCVTIMEO, (char*)&outtime, sizeof(int));

#endif
		lizzz_Log::Instance()->addLog("Pointer");
		if (fd == -1)
		{
			/*	perror("error create udp socket");*/
			break;
		}
		
		time(&ident);
		//copy
		p = reqBuf;
		//Transaction ID
		*(p++) = ident;
		*(p++) = ident >> 8;
		//Header section
		//flag word = 0x0100
		*(p++) = 0x01;
		*(p++) = 0x00;
		//Questions = 0x0001
		//just one query
		*(p++) = 0x00;
		*(p++) = 0x01;
		//Answer RRs = 0x0000
		//no answers in this message
		*(p++) = 0x00;
		*(p++) = 0x00;
		//Authority RRs = 0x0000
		*(p++) = 0x00;
		*(p++) = 0x00;
		//Additional RRs = 0x0000
		*(p++) = 0x00;
		*(p++) = 0x00;
		//Query section
		countp = p;
		*(p++) = 0;
		for (q = argv; *q != 0; q++)
		{
			if (*q != '.')
			{
				(*countp)++;
				*(p++) = *q;
			}
			else if (*countp != 0)
			{
				countp = p;
				*(p++) = 0;
			}
		}
		if (*countp != 0)
			*(p++) = 0;

		//Type=1(A):host address
		*(p++) = 0;
		*(p++) = 1;
		//Class=1(IN):internet
		*(p++) = 0;
		*(p++) = 1;

		//	 printf("\nRequest:\n");
		//	 printmessage(reqBuf);
		/*
		int socket = lizzz_socket::connect_s(fd, g_dns.c_str(), 53);
		
		lizzz_Log::Instance()->addLog("Dns success connect fd:" + to_string(fd) + " server <" + g_dns + ":53>");
		
		int len = send(fd, (char*)reqBuf, p - reqBuf, MSG_NOSIGNAL);
		lizzz_Log::Instance()->addLog("Dns send bytes:" + to_string(len));
		
		char buffer[1024];
		int n = recv(fd, buffer, sizeof(buffer), MSG_NOSIGNAL);
		lizzz_Log::Instance()->addLog("Dns recv bytes:" + to_string(n));
		
		exit(1);
		*/
		//fill
		memset((void*)&serveraddr, 0, sizeof(serveraddr));
		serveraddr.sin_family = AF_INET;
		serveraddr.sin_port = htons(53);
		serveraddr.sin_addr.s_addr = inet_addr(g_dns.c_str());

		lizzz_Log::Instance()->addLog("GetDns send:");
		//send to DNS Serv
		if (sendto(fd, (char*)reqBuf, p - reqBuf, MSG_NOSIGNAL, (sockaddr*)&serveraddr, sizeof(serveraddr)) < 0)
		{
			/*		perror("error sending request");*/
			break;
		}

		//recev the reply
		memset((void*)&serveraddr, 0, sizeof(serveraddr));
		serveraddrlent = sizeof(serveraddr);
		lizzz_Log::Instance()->addLog("GetDns recv:");
		rc = recvfrom(fd, (char*)rplBuf, sizeof(rplBuf), MSG_NOSIGNAL, (sockaddr*)&serveraddr, &serveraddrlent);
		if (rc < 0)
		{
			lizzz_Log::Instance()->addLog("Error recv");
			/*		perror("error receiving request\n");*/
			break;
		}
		lizzz_Log::Instance()->addLog("GetDns recv ok:");

		//	printf("\nReply: %d %s\n", rc, rplBuf);
		ret = getIpWithPach(rplBuf, rc, addr);

	} while (false);

	closesocket(fd);

	lizzz_Log::Instance()->addLog("DNS success");
	return ret;
}

int DNS::getIpWithPach(unsigned char* buf, int n, in_addr* addr)
{
	unsigned char ip[5];
	//print out results
	int cnt = 0;
	for (int i = n; i != 0; i--)
	{
		//printf("%c = %d\r\n", buf[i], buf[i]);
		ip[3 - cnt] = buf[i - 1];
		//printf("%d ", ip[cnt]);
		cnt++;
		if (cnt == 4)
			break;
	}

	memcpy(&addr->s_addr, ip, 4);


	return true;
}


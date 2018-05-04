//AddMcast（套接字，本地IP（非“0.0.0.0“），多播IP，port（发送方使用的多播端口））
// Function    : struct sockaddr_in InitSocketAdd()
// Illustration: This function is used to create a address with the given ip and port,
// the address is the bind() parameter
// INput : struct sockaddr_in mcast_addr ,the NULL address
// INput : int socket_port_num,the port we want to bind to the socket
// INput : const char SocketIPNum[16] ,the ip we want to bind to the socket
// INput : int sockaddrin_len ,the length of the "struct sockaddr_in"
// Output : InitAddress,return a address with filled message
// Output : if fail, it will exit the hole program.

#include "add_ip_to_mul.h"

int SetLoopState(int client_sock)
{
	unsigned char loop = 0;
	if (setsockopt(client_sock, IPPROTO_IP, IP_MULTICAST_LOOP, &loop, sizeof(loop)) < 0)
	{
		debug_print(("setsockopt IP_MULTICAST_LOOP \n"));
		WriteNetworkErrorToLog("setsockopt IP_MULTICAST_LOOP"); 
		return -1;
	}
	return 0;
}

 int AddMcast(int socke_fd, char ClientIPNumber[16], char McastIPNumber[16], int port_num)
{
	int ret = 0;
	char BindingIP[16] = "0.0.0.0";
	struct ip_mreq Mreq = {{0}};
	struct sockaddr_in BindAddress = {0};

	if (SetLoopState(socke_fd) < 0)
	{
		debug_print(("ERROR on SetLoopState \n"));
		WriteNetworkErrorToLog("ERROR on SetLoopState\n"); 
		return(-1);
	}

	bzero(&Mreq, sizeof(struct ip_mreq));
	bzero(&BindAddress, sizeof(struct sockaddr_in));
	if (SocketBinding(socke_fd, BindAddress, BindingIP, port_num) < 0)
	{
		debug_print(("ERROR on SocketBinding \n"));
		WriteNetworkErrorToLog("ERROR on SocketBinding\n"); 
		return(-1);
	}
	if (inet_pton(AF_INET, ClientIPNumber, (char *)&Mreq.imr_interface.s_addr) <= 0)
	{
		debug_print((" ERROR in  transforming the IP add\n"));
		WriteNetworkErrorToLog("ERROR on SocketBinding\n"); 
		return(-1);
	 }
	if (inet_pton(AF_INET, McastIPNumber, (char *)&Mreq.imr_multiaddr.s_addr) <= 0)
	{
		debug_print((" ERROR in  transforming the IP add\n"));
		WriteNetworkErrorToLog(" ERROR in  transforming the IP add\n"); 
		return(-1);
	 }
	if ((ret=setsockopt(socke_fd, IPPROTO_IP, IP_ADD_MEMBERSHIP,
			&Mreq, sizeof(struct ip_mreq))) == -1)
	{
		debug_print(("the ret value is %d\n",ret));
		WriteNetworkErrorToLog(" ERROR in  transforming the IP add\n"); 
		debug_print(("stop on the setsockopt IP_ADD_MEMBERSHIP\n"));
		WriteNetworkErrorToLog(" stop on the setsockopt IP_ADD_MEMBERSHIP\n");
		return(-1);
	}
	debug_print(("the AddMcast fun \n"));
	return 0;
}

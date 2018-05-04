/*
 * add_ip_to_mul.h
 *
 *  Created on: 2012-8-30
 *      Author: benty
 */

#ifndef ADD_IP_TO_MUL_H_
#define ADD_IP_TO_MUL_H_

#include <sys/socket.h>
#include <netinet/in.h>
#include "udp_recv_fun.h"

int AddMcast(int socke_fd, char ClientIPNumber[16], char McastIPNumber[16], int port_num);
int SetLoopState(int client_sock);

#endif /* ADD_IP_TO_MUL_H_ */

/*
 * Copyright (c) 2012，MIT
 * All right reserved.
 *
 * 文件名称： zhw_use_ioctl_to_set_net.c
 * 文件标识：
 * 摘    要： 设置网络的ip地址
 *
 * 当前版本： V1.0.0
 * 作    者： 周魏
 * 完成日期：2012-09-14
 *
 * 取代版本：
 * 原作者  ：
 * 完成日期：
 */
#include "zhw_use_ioctl_to_set_net.h"

struct bootp eth0_bootp_data;

#ifndef IOCTL_TEST
#define IOCTL_TEST
#endif

/*
 *   	@brief	本函数的功能是进行具体的设置过程；
 *     @param intf 需要进行IP设置的以太网口
 *     @param set_ip 	将要设置的ip地址
 *     @return   1表示设置成功；0表示设置失败；
 */
int
ZhwSetIpToSys(net_info_t net_info)
{

    build_bootp_record(&eth0_bootp_data,
                       net_info.inf_name,
                       net_info.addrs_ip,
                       net_info.addrs_netmask,
                       net_info.addrs_broadcast,
                       net_info.addrs_gateway,
                       net_info.addrs_server);
    show_bootp(eth0_name, &eth0_bootp_data);

        if (!init_net(eth0_name, &eth0_bootp_data)) {
            diag_printf("Network initialization failed for eth0\n");
        }
        else {
        	diag_printf("Network initialization success for eth0\n");
        }
#ifdef CYGHWR_NET_DRIVER_ETH0_IPV6_PREFIX
        if (!init_net_IPv6(eth0_name, &eth0_bootp_data,
                           string(CYGHWR_NET_DRIVER_ETH0_IPV6_PREFIX))) {
            diag_printf("Static IPv6 network initialization failed for eth0\n");
        }
        else {
        	diag_printf("Static IPv6 network initialization success for eth0\n");
        }
#endif
}


int ZhwSetMacToSys( const char *interface, char mac_address[6] )
{
	net_debug_printf(("run into ZhwSetMacToSys() function!!!\n"));
    int s, i;
    struct ifreq ifr;

    s = socket(AF_INET, SOCK_DGRAM, 0);
    if (s < 0) {
    	net_debug_printf(("socket"));
        return false;
    }

    net_debug_printf(( "%s socket is %d:\n", interface, s ));

    strcpy(ifr.ifr_name, interface);

    for ( i = 0; i < 6 /*ETHER_ADDR_LEN */; i++ )
        ifr.ifr_hwaddr.sa_data[i] = mac_address[i];

    net_debug_printf(( "Mac addr %02x:%02x:%02x:%02x:%02x:%02x\n",
                 ifr.ifr_hwaddr.sa_data[0],
                 ifr.ifr_hwaddr.sa_data[1],
                 ifr.ifr_hwaddr.sa_data[2],
                 ifr.ifr_hwaddr.sa_data[3],
                 ifr.ifr_hwaddr.sa_data[4],
                 ifr.ifr_hwaddr.sa_data[5] ));

    if (ioctl(s, SIOCSIFHWADDR, &ifr)) {
    	net_debug_printf(("SIOCSIFHWADDR"));
        close( s );
        return false;
    }

    net_debug_printf(( "%s ioctl(SIOCSIFHWADDR) succeeded\n", interface ));

    close( s );

    net_debug_printf(("run out ZhwSetMacToSys() function!!!\n"));
    return true;
}

/*
 *   	@brief	本函数的功能是设置相应的以太网口的ip地址；
 *     @param addre_local_ip 将要设置的ip地址
 *     @param eth_name 	需要进行IP设置的以太网口
 *     @return   1表示设置成功；0表示设置失败；
 */
int ZhwSetAddToSys(net_info_t net_info,char addrs_local_mac[6])
{
	net_debug_printf(("I am success entry SetIpToSys function!\n"));
	net_debug_printf(("the ip of set is :%s,the eth name of set is :%s\n",net_info.addrs_ip,net_info.inf_name));

	net_debug_printf(( "the Mac addr of set is:%02x:%02x:%02x:%02x:%02x:%02x\n",
    		addrs_local_mac[0],
    		addrs_local_mac[1],
    		addrs_local_mac[2],
    		addrs_local_mac[3],
    		addrs_local_mac[4],
    		addrs_local_mac[5] ));

    if (!ZhwSetMacToSys(net_info.inf_name, addrs_local_mac)) {
    	net_debug_printf(("Network mac initialization failed for %s\n",net_info.inf_name));
        return -1;
    }
    else {
    	net_debug_printf(("Network mac initialization success for %s\n",net_info.inf_name));
    }
diag_printf("zhw &&&&&& test !\n");
    init_all_network_interfaces();
diag_printf("zhw ******* test !\n");
    if (!ZhwSetIpToSys(net_info)) {
    	net_debug_printf(("Network ip initialization failed for %s\n",net_info.inf_name));
        return -1;
    }
    else {
    	net_debug_printf(("Network ip initialization success for %s\n",net_info.inf_name));
    }

    return 0;
}

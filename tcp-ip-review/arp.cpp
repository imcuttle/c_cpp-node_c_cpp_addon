#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/socket.h>
#include <arpa/inet.h>
#include <net/if_dl.h>
#include <ifaddrs.h>
#include <sys/ioctl.h>
#include <net/if.h>
#include <netinet/if_ether.h>
#include <zconf.h>
#include <fcntl.h>
#include <net/bpf.h>


// osx
bool get_mac_address(unsigned char* mac_addr, const char* if_name = "en0")
{
    ifaddrs* iflist;
    bool found = false;
    if (getifaddrs(&iflist) == 0) {
        for (ifaddrs* cur = iflist; cur; cur = cur->ifa_next) {
            if ((cur->ifa_addr->sa_family == AF_LINK) &&
                (strcmp(cur->ifa_name, if_name) == 0) &&
                cur->ifa_addr) {
                sockaddr_dl* sdl = (sockaddr_dl*)cur->ifa_addr;
                memcpy(mac_addr, LLADDR(sdl), sdl->sdl_alen);
                found = true;
                break;
            }
        }

        freeifaddrs(iflist);
    }
    return found;
}


int main() {
    int sd;
    unsigned char src_mac[6];
    char interface[] = "en0", src_ip[] = "172.22.200.59", dest_ip[] = "172.22.200.23";

    u_char frame[42]; // 6+6+2+28
    struct ether_header ehead;
    struct ether_arp earp;
    struct arphdr arp_hdr = earp.ea_hdr;


//    if(!get_mac_address(src_mac)) {
//        perror("get_mac_address");
//        return 1;
//    }
    // f0:79:60:0a:bf:80
    src_mac[0] = 0xf0; src_mac[1] = 0x79; src_mac[2] = 0x60; src_mac[3] = 0x0a; src_mac[4] = 0xbf; src_mac[5] = 0x80;
    printf("SrcMac: %02x:%02x:%02x:%02x:%02x:%02x\n", src_mac[0], src_mac[1], src_mac[2], src_mac[3], src_mac[4], src_mac[5]);

    arp_hdr.ar_hrd = htons(ARPHRD_ETHER); // 以太网硬件类型
    arp_hdr.ar_pro = htons(0x0800); /* format of protocol address */
    arp_hdr.ar_hln = 6; /* length of hardware address */
    arp_hdr.ar_pln = 4; /* length of protocol address */
    arp_hdr.ar_op = htons(ARPOP_REQUEST); // 1
    memcpy(&earp.arp_sha, src_mac, arp_hdr.ar_hln);
    inet_pton (AF_INET, src_ip, &earp.arp_spa);
    memset(&earp.arp_tha, 0, arp_hdr.ar_hln);
    inet_pton (AF_INET, dest_ip, &earp.arp_tpa);

    printf("sizeOf earp: %lu\n", sizeof(earp));

    memcpy(&ehead.ether_shost, src_mac, 6); // source mac
    memset(&ehead.ether_dhost, -1, 6); // dest mac
    ehead.ether_type = htons(ETHERTYPE_ARP); //0x0806

    memcpy(&frame, &ehead, sizeof(ehead));
    memcpy(&frame + sizeof(ehead), &earp, sizeof(earp));


    // Linux   #include <netpacket/packet.h>

#ifdef __linux__
    sd = socket(AF_PACKET, SOCK_RAW, htons(ETH_P_ARP));
    struct sockaddr_ll destaddr;
    destaddr.sll_family = AF_PACKET;
    if((destaddr.sll_ifindex = if_nametoindex(INTERFACE)) == 0) {
        perror("if_nametoindex() failed");
        exit(EXIT_FAILURE);
    }
    destaddr.sll_halen = htons(ETHER_ADDR_LEN);
    // send packet to poison -- 将我们之前伪造好的arp响应包发送出去
    if(sendto(sd, frame, sizeof(frame), 0, (struct sockaddr *)&destaddr, sizeof(destaddr)) == -1) {
        perror("sendto() failed");
        exit(EXIT_FAILURE);
    }

    // close socket -- 完成后关闭socket
    close(soc);
#elif __APPLE__
    int bpf = -1;
    int devno = 0;
#define DEV_PLEN 12              //device path length, 设备路径长度。比如"/dev/bpf255"，最长11个字节加最后一位终止符，共12字节
    char dev[DEV_PLEN];

    // find available bpf device -- 找到空闲的bpf设备
    while(bpf < 0) {
        snprintf(dev, DEV_PLEN, "/dev/bpf%d", devno);
        bpf = open(dev, O_WRONLY);

        ++devno;
        if(devno > 255) {
            printf("/dev/bpf* full.\n");
            exit(EXIT_FAILURE);
        }
    }
    printf("* /dev/bpf%d available.\n", --devno);


    // bound bpf to an interface -- 通过ioctl将bpf与网络接口进行绑定
    struct ifreq boundif;
    strncpy(boundif.ifr_name, interface, strlen(interface));
    if(ioctl(bpf, BIOCSETIF, &boundif) < 0) {
        perror("ioctl() failed");
        close(bpf);
        exit(EXIT_FAILURE);
    }
    printf("* Interface %s bound.\n", interface);


    // write to bpf -- 直接写入bpf即可发送，因为arp帧的头部已经包含了目标地址信息
    if(write(bpf, frame, sizeof(frame)) < 0) {
        perror("write() failed");
        close(bpf);
        exit(EXIT_FAILURE);
    }
    printf("* Done write to bpf.\n");


    close(bpf);

#endif
    return 0;
}
#include <linux/kernel.h>
#include <linux/module.h>
#include <linux/netdevice.h>
#include <linux/skbuff.h>       /* Socket Kernel Buffers */
#include <linux/tcp.h>          /* TCP Header */
#include <linux/socket.h>
#include <net/sock.h>
#include <linux/proc_fs.h>
#include <linux/ip.h>
#ifndef __KERNEL__
	#define __KERNEL__
#endif
#ifndef MODULE
	#define MODULE
#endif

struct ipstats{
	short ipver;
	short iphlen;
	short ttl;
	short nextpro;
	unsigned long saddr,daddr;
	
};
static struct ipstats ips;
static int totalippacks; /*total ip packets captured*/
/*This will be called when a packet arrives*/
void handle_IP(struct sk_buff *sk)
{			
	 struct iphdr  *iph;
	 unsigned char sa[4],da[4];
	 iph =(struct iphdr *) (sk->data);
	 ips.ipver = iph->version;
	 ips.iphlen= iph->ihl;
	 ips.ttl = iph->ttl;
	 ips.nextpro = iph->protocol;
	 ips.saddr = iph->saddr;
	 ips.daddr = iph->daddr;
	 totalippacks++;
        memcpy(sa,(char*)&ips.saddr,4);
        memcpy(da,(char*)&ips.daddr,4);
	printk("\nIP_VER\tTTL\tTPROTOCOL\t   SADD\t\t   DADD\t\tNUMOFPACKS\n%4d\t%3d\t%3d\t\t%d.%d.%d.%d\t%d.%d.%d.%d\t%4d\n",ips.ipver,ips.ttl,ips.nextpro,sa[0],sa[1],sa[2],sa[3],da[0],da[1],da[2],da[3],totalippacks);
	 

}
void handle_ARP(struct sk_buff *sk)
{
	/*to be imple*/
	
}
/*This will be called when a packet arrives*/	
static int packlog_recv(struct sk_buff *skb, struct net_device *dev, struct packet_type *pt,struct net_device *realdev)
{
	
	
	if ((skb = skb_share_check(skb, GFP_ATOMIC)) == NULL)
		return 0;
	
	/*we dont capture loopback packets*/
	if (skb->pkt_type == PACKET_LOOPBACK)
		return 0;
	
	/*packet is to us ,capture it */ 
	if(skb->pkt_type == PACKET_HOST){
		switch(ntohs(skb->protocol)){
			case ETH_P_IP:
				handle_IP(skb);				
				break;
			case ETH_P_ARP:
				handle_ARP(skb);
				break;
		}
	}
	/*to be imple*/
	if(skb->pkt_type == PACKET_OUTGOING){
	}
	
	return 0;
}


static struct packet_type packlog_type = {
        .type = __constant_htons(ETH_P_ALL),
        .func = packlog_recv,//call back
};

int packlog_init(void)
{
	
	
	/*register with kernel to be notified when a packet comes*/
	dev_add_pack(&packlog_type);
	     
	
	return 0;

}

void packlog_exit(void)
{
	dev_remove_pack(&packlog_type);
}

module_init(packlog_init);
module_exit(packlog_exit);
MODULE_LICENSE("GPL");


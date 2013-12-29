/**********************************************************************
 * file:  sr_router.c 
 * date:  Mon Feb 18 12:50:42 PST 2002  
 * Contact: casado@stanford.edu 
 *
 * Description:
 * 
 * This file contains all the functions that interact directly
 * with the routing table, as well as the main entry method
 * for routing.
 *
 **********************************************************************/

#include <time.h>
#include <stdio.h>
#include <assert.h>
#include <string.h>
#include <stdlib.h>
#include <pthread.h>
#include <unistd.h>

#include "sr_if.h"
#include "sr_rt.h"
#include "sr_router.h"
#include "sr_protocol.h"
#include "rmutex.h"
#include "arp_caretaker.c"

/*--------------------------------------------------------------------- 
 * Method: sr_init(void)
 * Scope:  Global
 *
 * Initialize the routing subsystem
 * 
 *---------------------------------------------------------------------*/

void sr_init(struct sr_instance* sr) 
{
    pthread_t tid;
    /* REQUIRES */
    assert(sr);
    
    // init arp cache

    memset(&sr->arptable,0,ARP_TABLE_SIZE*sizeof(struct arptable_entry));
    memset(&sr->pq,0,OUTSTANDING_ARP_LIMIT * sizeof(struct pending_packet_queue_entry));
    memset(&sr->aq,0,OUTSTANDING_ARP_LIMIT * sizeof(struct arp_response_queue_entry));
    rmutex_init(&sr->arptable_lock);
    rmutex_init(&sr->arp_queue_lock);
    pthread_create(&tid,NULL,arp_queue_caretaker,(void *) sr);

} /* -- sr_init -- */

/* Method: ether_to_me:
 * returns true if i should process this ethernet packet
 * (if it is to this address or to broadcast)
 */
int ether_to_me(addr_mac_t  * my_address, addr_mac_t * addr_s){
    unsigned char * addr = (unsigned char *)addr_s;
    return ((memcmp(my_address,addr_s,ETHER_ADDR_LEN)==0) ||
          ((addr[0] & addr[1] & addr[2] & addr[3] & addr[4] & addr[5]) == 0xff));
}
/*---------------------------------------------------------------------
 * Method: sr_handlepacket(uint8_t* p,char* interface)
 * Scope:  Global
 *
 * This method is called each time the router receives a packet on the
 * interface.  The packet buffer, the packet length and the receiving
 * interface are passed in as parameters. The packet is complete with
 * ethernet headers.
 *
 * Note: Both the packet buffer and the character's memory are handled
 * by sr_vns_comm.c that means do NOT delete either.  Make a copy of the
 * packet instead if you intend to keep it around beyond the scope of
 * the method call.
 *
 *---------------------------------------------------------------------*/

enum arp_type{
    UNKNOWN = 0,
    ETHERNET_TYPE_ARP=0x0806,
    ETHERNET_TYPE_IP=0x0800
};


void sr_handlepacket(struct sr_instance* sr, 
        uint8_t * packet/* lent */,
        unsigned int len,
        char* interface/* lent */)
{
    /* REQUIRES */
    assert(sr);
    assert(packet);
    assert(interface);


    printf("*** -> Received ethernet frame of length %d on interface %s \n",len, interface);

    /* 
     * verify that the ethernet frame has a valid length 
     * must have at least an Ethernet header but not too big either 
     */
    if(len < ETH_HEADER_LEN || len > ETH_MAX_LEN){
        printf("Dropped ethernet frame: Not enough frame OR too big frame\n");
        return;
    }

    /* drop the packet if it does not apply to this interface */
    struct sr_if *intf = NULL;
    intf = sr_get_interface(sr,interface);
    if(intf == NULL || !ether_to_me(&(intf->addr), (addr_mac_t *)&packet[0]) ){
        printf("Dropped ethernet frame as it does not apply to this interface.\n");
        return;
    }


    
    /* determine whether the packet is IP or ARP */
    enum arp_type *arp_type = UNKNOWN;
    arp_type = ntohs(*(uint16_t *)&packet[12]); /* packet[12] and packet[13] are arp_type */
    printf("ETHERNET Type = %4x\n", arp_type);

    /* handle the payload with handle_ip() and handle_arp(), respectively */
    if(arp_type == ETHERNET_TYPE_IP){
        printf("Received IP Packet\n");
        handle_ip(sr, (packet + ETH_HEADER_LEN), (len - ETH_HEADER_LEN), interface);


    }
    else if(arp_type == ETHERNET_TYPE_ARP){
        printf("Received ARP Packet\n");
        handle_arp(sr, (packet + ETH_HEADER_LEN), (len - ETH_HEADER_LEN), interface);
    }
    else{
    /* if the packet is neither IP nor ARP, drop */
        printf("Dropped packet of type %d",arp_type);
        return;
    }

}/* end sr_ForwardPacket */

/* this is either an arp request and we are 
 * responsible for replying with our ethernet address
 * or this is an arp reply hopefully to a request
 * we have sent out, in which case we add the entry
 * to our cache.
 */

enum ARP_HTYPE {
    ARP_HTYPE_UNKNOWN = 0,
    ARP_HTYPE_ETHERNET = 0x01
};

enum ARP_PTYPE {
    ARP_PTYPE_UNKNOWN = 0,
    ARP_PTYPE_IP = 0x0800
};

#define IP_ADDR_LEN 4

int handle_arp(struct sr_instance * sr, uint8_t * packet, unsigned int len, char * interface)
{
    struct sr_arphdr * arp_header;
    arp_header = (struct sr_arphdr *) packet;
    uint32_t this_interface_ip;
    /* This function only works for IPv4 and MAC addresses, otherwise it will do nothing
     * Below checks are for the same.
     */
    if (ntohs(arp_header->ar_hrd) != ARP_HTYPE_ETHERNET ||
          ntohs(arp_header->ar_pro) != ARP_PTYPE_IP){
        printf("Dropped ARP packet : Either HTYPE not Ethernet or PTYPE not IP\n");
        return 1; /* Doing Nothing */
    }
    if( arp_header->ar_hln != ETHER_ADDR_LEN ||
        arp_header->ar_pln != IP_ADDR_LEN){
        printf("Dropped ARP packet : Either HLEN or PLEN incorrect\n");
        return 1; /* Doing Nothing */
    }
    // request or reply?
    switch (ntohs(arp_header->ar_op))
    {
        case ARP_REQUEST:
            printf("Received ARP Request\n");
            /* handle this ARP request by sending a reply if necessary.
             * This will entail:
             * verify that the request is for the correct interface(done in caller)
             * allocate space to store the response
             * set the fields in the ethernet header
             * set the fields in the arp header
             * call sr_send_packet() with the correct buffer, packet length,and interface
             */

            this_interface_ip = sr_get_interface(sr,interface)->ip;
            if(arp_header->ar_tip == this_interface_ip){
                printf("Sending ARP Reply to %x:%x:%x:%x:%x:%x...\n",arp_header->ar_sha.octet[0],
                                                                     arp_header->ar_sha.octet[1],
                                                                     arp_header->ar_sha.octet[2],
                                                                     arp_header->ar_sha.octet[3],
                                                                     arp_header->ar_sha.octet[4],
                                                                     arp_header->ar_sha.octet[5]);

                int out_packet_len = sizeof(struct sr_arphdr) + sizeof(struct sr_ethernet_hdr);
                char out_packet[out_packet_len];

                /* Fill ARP header */
                struct sr_arphdr *arp_reply = out_packet + sizeof(struct sr_ethernet_hdr);
                arp_reply->ar_hrd = arp_header->ar_hrd;
                arp_reply->ar_pro = arp_header->ar_pro;
                arp_reply->ar_hln = arp_header->ar_hln;
                arp_reply->ar_pln = arp_header->ar_pln;
                arp_reply->ar_op  = htons(ARP_REPLY);
                arp_reply->ar_sha = sr_get_interface(sr,interface)->addr;
                arp_reply->ar_sip = this_interface_ip;
                arp_reply->ar_tha = arp_header->ar_sha;
                arp_reply->ar_tip = arp_header->ar_sip;

                /* Fill ethernet header */
                struct sr_ethernet_hdr *eth_hdr = out_packet;
                eth_hdr->ether_dhost = arp_header->ar_sha;
                eth_hdr->ether_shost = sr_get_interface(sr,interface)->addr;
                eth_hdr->ether_type = htons(ETHERNET_TYPE_ARP);

                /*Send Ethernet packet - ARP Reply */
                sr_send_packet(sr, out_packet, out_packet_len, interface);
            }
            else{
                printf("ARP Request not for me.\n");
            }
            break;
        case ARP_REPLY:
            /* store the arp reply in the router's arp cache */
            arp_table_set_entry(sr,arp_header->ar_sip,arp_header->ar_sha);
            break;
        default:
            Debug("dropping unhandleable ARP packet\n");
    }
    return 0;
}

#define IPV4_VER 4

int handle_ip(struct sr_instance *sr, uint8_t * packet,unsigned int len,char * interface)
{

    struct ip * hdr = (struct ip *)packet;


    if(hdr->ip_v != IPV4_VER ||
        hdr->ip_hl*4 != IPV4_HEADER_LEN ||
         0 != ntohs(checksum(hdr, IPV4_HEADER_LEN))){
        printf("IP: Dropped IP packet due to malformed IP packet or not supported IP.\n");
        return 1; /* Doing nothing */
    }

    /*Decrement TTL*/
    hdr->ip_ttl--;
    

    if(0 == hdr->ip_ttl){
        printf("Sending ICMP TTL exceeded to %s for dest %s\n",inet_ntoa(hdr->ip_dst),inet_ntoa(hdr->ip_src));
        icmp_send(sr,
                  hdr->ip_src.s_addr, sr_get_interface(sr, interface)->ip,
                  packet, IPV4_HEADER_LEN + 8,
                  ICMP_TYPE_TIME_EXCEEDED , ICMP_CODE_TTL_EXPIRED, 0, 0);
        return 1;
    }

    /* Recalculate checksum based on new TTL, if it was not dropped */
    checksum_ip(hdr);
    
    if(TRUE == ip_to_me(sr, hdr->ip_dst.s_addr)){ 
        if(hdr->ip_p == IPPROTO_ICMP){ /* Router will only handle ICMP Echo Request */
            printf("IP: Received ICMP Packet of length %d\n",len - hdr->ip_hl * 4);
            icmp_handle_packet(sr, packet, len); /* IP Packet is sent up */
        }
        else{ /* All non ICMP messages to router, will receive an 'ICMP Protocol Unreachable' message */
            printf("IP: Router addressed non ICMP message. Sending ICMP Protocol Unreachable...\n");
            /* Send icmp protocol unreachable to source ip */
            icmp_send(sr,
                      hdr->ip_src.s_addr, hdr->ip_dst.s_addr,
                      packet, IPV4_HEADER_LEN + 8,
                      ICMP_TYPE_DEST_UNREACH, ICMP_CODE_PROTO_UNREACH, 0, 0);
        }
        return 0; /* Packet processed */
    }


 
    /* Forward the packet */
    if(!router_send_ethernet_frame(sr, hdr->ip_dst.s_addr, htons(ETHERTYPE_IP), packet,len))
    {
        Debug("Couldn't find a route to forward IP packet\n");
        
        /* send an ICMP DESTINATION UNREACHABLE */
        icmp_send(sr,
                  hdr->ip_src.s_addr, sr_get_interface(sr, interface)->ip,
                  packet, IPV4_HEADER_LEN + 8,
                  ICMP_TYPE_DEST_UNREACH , ICMP_CODE_HOST_UNREACH, 0, 0); 

        return 6;
    }
    return 0;

}

uint16_t checksum_ip( struct ip * hdr ) {
    hdr->ip_sum = 0;
    hdr->ip_sum = checksum( (uint16_t*)hdr, IPV4_HEADER_LEN );
    return hdr->ip_sum;
}



uint16_t checksum( uint16_t* buf, unsigned len ) {
    uint16_t answer;
    uint32_t sum;

    /* add all 16 bit pairs into the total */
    answer = sum = 0;
    while( len > 1 ) {
        sum += *buf++;
        len -= 2;
    }

    /* take care of the last lone uint8_t, if present */
    if( len == 1 ) {
        *(unsigned char *)(&answer) = *(unsigned char *)buf;
        sum += answer;
    }

    /* fold any carries back into the lower 16 bits */
    sum = (sum >> 16) + (sum & 0xFFFF);    /* add hi 16 to low 16 */
    sum += (sum >> 16);                    /* add carry           */
    answer = ~sum;                         /* truncate to 16 bits */

    return answer;
}

void icmp_send( struct sr_instance * router, uint32_t dst, uint32_t src, uint8_t* icmp_payload, /* or just the data to send back */
                unsigned len,
                uint8_t type,
                uint8_t code,
                uint16_t extra1,
                uint16_t extra2 ) {

    char icmp_packet[ICMP_HEADER_LEN + len];

    /* Populate ICMP Header fields */
    hdr_icmp_t * icmp_hdr = NULL;
    icmp_hdr = (hdr_icmp_t *)icmp_packet;
    icmp_hdr->type = type;
    icmp_hdr->code = code;
    icmp_hdr->short1 = extra1;
    icmp_hdr->short2 = extra2;

    /* If there is some ICMP data then add that to the packet */
    memcpy((char *)&icmp_hdr[1], (char *)icmp_payload, len);

    /* Fill ICMP checksum field */
    icmp_hdr->sum = checksum_icmp(icmp_hdr, len + ICMP_HEADER_LEN);
            
    if(!src){           
        /* Send ICMP header + ICMP payload as IP payload, along with destination IP to Network layer*/
        network_send_packet(router, dst, IP_PROTO_ICMP, icmp_packet, ICMP_HEADER_LEN + len);
    }
    else{
        /* Send ICMP header + ICMP payload as IP payload, along with destination IP and source IP to Network layer*/
        network_send_packet_from( router, dst, src, IP_PROTO_ICMP, icmp_packet , ICMP_HEADER_LEN + len);
    }
}

struct sr_if * router_lookup_interface_via_ip( struct sr_instance * sr, uint32_t dst )
{
    /* use rtable_find_route to find route, then look up sr_if based on route
     * interface's name
     */
    struct in_addr dst_addr;
    struct sr_rt * route = rtable_find_route(sr,dst);
    dst_addr.s_addr = dst;
    if(!route){
        printf("router_lookup_interface_via_ip(): route not found for %s",inet_ntoa(dst_addr));
    }
    return sr_get_interface(sr,route->interface);

}



int network_send_packet( struct sr_instance * router,
                     uint32_t dst,
                     uint8_t proto_id,
                     uint8_t* payload,
                     unsigned len ) {
    struct sr_if * intf;
    uint32_t found_src;
    uint8_t * quad = (uint8_t*)&dst;
    /* lookup the src address we'll send from to get to dst */
    // outgoing interface
    intf = router_lookup_interface_via_ip( router, dst );
    if( intf )
        // outgoing interface's source IP
        found_src = intf->ip;
        return network_send_packet_from( router, dst,found_src, proto_id, payload, len );

    /* couldn't route to dst */
    Debug( "Error: unable to find route in network_send_packet for %u.%u.%u.%u\n",quad[0],quad[1],quad[2],quad[3] );

    return FALSE;
}


// walk the list of interfaces and return true if dst == interface.ipaddr
int ip_to_me(struct sr_instance * sr, uint32_t dst)
{
    struct sr_if* if_walker = sr->if_list;
    while(if_walker)
    {
        if (if_walker->ip ==dst)
            return TRUE;
        if_walker = if_walker->next;
    }
    return FALSE;
}

void icmp_handle_packet(struct sr_instance * router,
                         uint8_t* ip_packet,
                         unsigned len ) 
{
    struct ip *  hdr_ip;
    hdr_icmp_t* hdr;
    unsigned headers_len;
    unsigned icmp_packet_len;
    uint16_t old_sum; 
            
    hdr = (hdr_icmp_t*)(ip_packet + IPV4_HEADER_LEN);
    icmp_packet_len = len - IPV4_HEADER_LEN;
            
    if( hdr->type != ICMP_TYPE_ECHO_REQUEST ) {
        Debug( "%s only Echo Request and Reply is handled (received type %u)",
                       "ICMP packet dropped:",
                       hdr->type );
        return;
    }       
                       
    old_sum = hdr->sum;
    if( old_sum != checksum_icmp(hdr, icmp_packet_len) ) {
        Debug( "%s checksum %u is incorrect:: should be %u",
                       "ICMP packet dropped:",
                       old_sum,
                       hdr->sum );
        return;
    }

    /* determine how much data came with the request */
    headers_len = IPV4_HEADER_LEN + ICMP_HEADER_LEN;

    /* send the reply to our sender from us (swapped dst/src fields) */
    printf("ICMP: Sending ICMP Echo reply\n");
    hdr_ip = (struct ip*)ip_packet;
    icmp_send( router,
               hdr_ip->ip_src.s_addr, hdr_ip->ip_dst.s_addr,
               ip_packet+headers_len, len - headers_len,
               ICMP_TYPE_ECHO_REPLY, 0, hdr->short1, hdr->short2 );
}

uint16_t checksum_icmp( hdr_icmp_t* icmp_hdr, unsigned total_len ) {
    icmp_hdr->sum = 0;
    icmp_hdr->sum = checksum( (uint16_t*)icmp_hdr, total_len );
    return icmp_hdr->sum;
}

int  network_send_packet_from( struct sr_instance* router,
                          uint32_t dst,
                          uint32_t src,
                          uint8_t proto_id,
                          uint8_t* buf,
                          unsigned len ) {
    static int ip_id = 1;
    int ret = 0;
    /* Allocate space for IP packet */
    char ip_packet[IPV4_HEADER_LEN + len];
    char ip_packet_len = IPV4_HEADER_LEN + len;
    memset(ip_packet, 0, ip_packet_len);

    /* Fill IP Header fields */
    struct ip * ip_hdr = NULL;
    ip_hdr = (struct ip *) ip_packet;
    ip_hdr->ip_v  = IPV4_VER;
    ip_hdr->ip_hl = IPV4_HEADER_LEN/4;
    ip_hdr->ip_len = htons(ip_packet_len);
    ip_hdr->ip_id = htons(ip_id++); 
    ip_hdr->ip_ttl = 64; 
    ip_hdr->ip_p = proto_id;
    ip_hdr->ip_src.s_addr = src;
    ip_hdr->ip_dst.s_addr = dst; 
    /* Calculate header checksum */
    checksum_ip(ip_hdr);

    /* Attach payload to IP header */
    memcpy((char *)&ip_hdr[1], buf, len);

    
    /* Send packet to link layer(ethernet) */
    ret = router_send_ethernet_frame(router, 
                                     dst,
                                     htons(ETHERNET_TYPE_IP),
                                     ip_packet,
                                     ip_packet_len);
    
    return ret;
}

// find outgoing interface based on routing table
struct sr_rt * rtable_find_route( struct sr_instance * sr,uint32_t dst_ip )
{
    struct sr_rt *route = NULL;
    struct sr_rt *best_route = NULL;

    route = sr->routing_table;

    /* Find the best route */
    while(route){
        uint32_t route_mask = route->mask.s_addr;
        uint32_t route_dest = route->dest.s_addr;
        if((dst_ip & route_mask) == (route_dest & route_mask)){
            if(best_route && best_route->mask.s_addr < route_mask){ /* If there are two matching routes, choose the lowest prefix route */
                best_route = route;
            }
            else if(best_route == NULL){
                best_route = route;
            }
        }
        route = route->next;
    }
    if(best_route){
        struct in_addr dst_addr;
        dst_addr.s_addr = dst_ip;
        printf("Best route for %s via interface %s\n",inet_ntoa(dst_addr),best_route->interface);
    }
    else{
        struct in_addr dst_addr;
        dst_addr.s_addr = dst_ip;
        printf("No route for %s\n",inet_ntoa(dst_addr));

    }
    return best_route; /* Will be NULL if no route found */
}

int router_send_ethernet_frame( struct sr_instance * router,
                                 uint32_t dst_ip,
                                 uint16_t type, /* needs to be in network order */
                                 uint8_t* payload,
                                 unsigned len ) {
    struct sr_rt * rti;
    struct sr_if * intf;
    /* lookup which route to use */
    rti = rtable_find_route( router, dst_ip );
    if( !rti ) {
        Debug("no route for this IP\n");
        return FALSE; /* don't have a route for this IP */
    }
    intf = sr_get_interface(router,rti->interface);
    router_queue_ethernet_frame(router, rti,intf, type, payload, len );
    return TRUE;
}

int router_queue_ethernet_frame(struct sr_instance * sr,
                                     struct sr_rt * rti,
                                     struct sr_if * intf,
                                     uint16_t type, /* needs to be in network order */
                                     uint8_t* payload,
                                     unsigned payload_len ) {


    addr_mac_t dst;
    if(1 && arp_table_get_entry(sr,rti->gw.s_addr,&dst))
    {
        /* ARP cache use */
        printf("ARP Cache Hit.\n");
        int out_packet_len = sizeof(struct sr_ethernet_hdr) + payload_len;
        char out_packet[out_packet_len];

        /* Fill ethernet header */
        struct sr_ethernet_hdr *eth_hdr = out_packet;
        eth_hdr->ether_dhost = dst;
        eth_hdr->ether_shost = sr_get_interface(sr,intf)->addr; 
        eth_hdr->ether_type = type;

        /* Ecapsulate IP packet */
        memcpy(out_packet + sizeof(struct sr_ethernet_hdr), payload, payload_len);
        
        sr_send_packet(sr, out_packet, out_packet_len, intf);
        
        return TRUE;
    }
    /* Else - queue_ethernet_wait_arp will send an ARP request
     * if necessary and send the packet once the dst MAC is found. 
     */
    queue_ethernet_wait_arp(sr,rti,intf,type,payload,payload_len);
    

    return FALSE;
}



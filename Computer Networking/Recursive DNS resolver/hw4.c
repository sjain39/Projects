#include <stdio.h>
#include <stdlib.h>
#include <stdint.h>
#include <unistd.h>
#include <string.h>
#include <sys/types.h>
#include <sys/uio.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include "dns.h"

#define IPV4_ADDR_LEN		0x0004
#define DNS_REPLY_FLAGS		0x8180
#define DNS_REPLY_REFUSED	0x8183
#define DNS_REPLY_NAME		0xC00C
#define DNS_REPLY_TTL		0x0005
#define DNS_CLASS_IN		0x0001
#define DNS_TYPE_A		0x0001
#define DNS_NUM_ANSWERS		0x0001

static int debug=0, nameserver_flag=0;
in_addr_t root_servers[255];	//shubham
int root_server_count=0;	//shubham

in_addr_t resolve_name(int, in_addr_t*, int, char*, int);

void usage() {
	printf("Usage: hw4 PORTNUM\n\n");
	exit(1);
}

//shubham
void read_root_file() {
	root_server_count=0;
	char addr[25];
	
	FILE *f = fopen("root-servers.txt","r");
	while(fscanf(f, "%s", addr) > 0)
		root_servers[root_server_count++]=inet_addr(addr);
}

/* constructs a DNS query message for the provided hostname */
int construct_query(uint8_t* query, int max_query, char* hostname)
{
	memset(query,0,max_query);

	in_addr_t rev_addr=inet_addr(hostname);
	if(rev_addr!=INADDR_NONE) {
		static char reverse_name[255];		
		sprintf(reverse_name,"%d.%d.%d.%d.in-addr.arpa",
						(rev_addr&0xff000000)>>24,
						(rev_addr&0xff0000)>>16,
						(rev_addr&0xff00)>>8,
						(rev_addr&0xff));
		hostname=reverse_name;
	}

	// first part of the query is a fixed size header
	struct dns_hdr *hdr = (struct dns_hdr*)query;

	// generate a random 16-bit number for session
	uint16_t query_id = (uint16_t) (random() & 0xffff);
	hdr->id = htons(query_id);
	// set header flags to request recursive query
	hdr->flags = htons(0x0000);	
	// 1 question, no answers or other records
	hdr->q_count=htons(1);

	// add the name
	int query_len = sizeof(struct dns_hdr); 
	int name_len=to_dns_style(hostname,query+query_len);
	query_len += name_len; 
	
	// now the query type: A or PTR. 
	uint16_t *type = (uint16_t*)(query+query_len);
	if(rev_addr!=INADDR_NONE)
		*type = htons(12);
	else
		*type = htons(1);
	query_len+=2;

	// finally the class: INET
	uint16_t *class = (uint16_t*)(query+query_len);
	*class = htons(1);
	query_len += 2;
 
	return query_len;	
}

int main(int argc, char** argv)
{
	if(argc!=2) usage();
	
	char *hostname=0;
	char *nameserver=0;
	read_root_file();

	struct sockaddr_in6 server_address; 
	struct sockaddr_in6 client_address;
	memset(&server_address,0,sizeof(server_address));
	memset(&client_address,0,sizeof(client_address));
        int port_number = atoi(argv[1]);
        
	int sock = socket(PF_INET6, SOCK_DGRAM, 0);
        if(sock < 0) {
                perror("Creating socket failed: ");
                exit(1);
        }
        server_address.sin6_family = AF_INET6;
        server_address.sin6_addr = in6addr_any;
        server_address.sin6_port = htons( port_number);
        if (bind( sock, (struct sockaddr *)&server_address, sizeof( server_address)) < 0)
        {
                perror("Unable to bind.");
                return -1;
        }
        printf("Bind successful.\n");
	uint8_t answerbuf[1500];
	char string_name[255];
	
	int cinlen = sizeof(client_address);

	while(1)
	{
		memset(answerbuf, 0, sizeof(answerbuf));
		int recv_count = recvfrom(sock, answerbuf, 1500, 0,(struct sockaddr *)&client_address,&cinlen);
		if(recv_count<0) { perror("Recieve failed");       exit(1); }
		
		struct dns_hdr *ans_hdr=(struct dns_hdr*)answerbuf;
		uint8_t *answer_ptr = answerbuf + sizeof(struct dns_hdr);
		from_dns_style(answerbuf,answer_ptr,string_name);
		printf("\n\nHostname sent by dig: %s\n", string_name);
		hostname = string_name;

		in_addr_t result;
		if(nameserver_flag)
		{
			in_addr_t nameserver_addr = inet_addr(nameserver);
			result = resolve_name(sock, &nameserver_addr, 1, hostname, 1);
		}
		else
			result = resolve_name(sock, root_servers, root_server_count, hostname, 1);


		in_addr_t rev_addr = inet_addr(hostname);
		char * request_packet = answerbuf;
		int request_packet_size = recv_count;
		if(rev_addr==INADDR_NONE)
		{
			if(!result)
			{
				printf("Host %s not found.\n", hostname);
				struct dns_header *reject_packet = NULL;
				int bytes_sent = 0;

				if(request_packet_size > (int) sizeof(struct dns_header))
				{

					reject_packet = (struct dns_header *) request_packet;
					reject_packet->num_answers = 0;
					reject_packet->flags = htons(DNS_REPLY_REFUSED);

					bytes_sent = sendto(sock,reject_packet,request_packet_size,0,(struct sockaddr *) &client_address, sizeof(client_address));
					if(bytes_sent != request_packet_size)
					{
						printf("Failed to send response DNS packet\n");
					}
				}
			}
			else
			{
				printf("%s resolves to %s\n", hostname, inet_ntoa(*(struct in_addr*)&result));
				int dns_type = htons(DNS_TYPE_A);
				char *reply_packet = NULL;
				struct dns_header *header = NULL;
				struct dns_answer_section answer;
				int reply_packet_size = 0;
				int answer_size = sizeof(struct dns_answer_section);
				int bytes_sent = 0;
				int memcpy_offset = 0;
				in_addr_t ip_address1 = result;

				memset(&answer,0,sizeof(struct dns_answer_section));

				if(request_packet_size > ((int) (sizeof(struct dns_header) + sizeof(struct dns_question_section)) + (int) strlen(hostname)))
				{
					answer.name = htons(DNS_REPLY_NAME);
					answer.type = dns_type;
					answer.class = htons(DNS_CLASS_IN);
					answer.ttl = htons(DNS_REPLY_TTL);

					answer.data_len = htons(IPV4_ADDR_LEN);

					reply_packet_size = request_packet_size + ((answer_size + IPV4_ADDR_LEN) * DNS_NUM_ANSWERS);
					if((reply_packet = malloc(reply_packet_size)) != NULL){

							memcpy(reply_packet,request_packet,request_packet_size);
							memcpy_offset += request_packet_size;
							memcpy(reply_packet+memcpy_offset,(void *) &answer,answer_size);
							memcpy_offset += answer_size;
							memcpy(reply_packet+memcpy_offset,(void *) &ip_address1,IPV4_ADDR_LEN);
							memcpy_offset += IPV4_ADDR_LEN;
							memcpy(reply_packet+memcpy_offset,(void *) &answer,answer_size);

					} else {
							perror("Malloc Failure");
							return 0;
					}

					header = (struct dns_header *) reply_packet;
					header->num_answers = htons(DNS_NUM_ANSWERS);
					header->flags = htons(DNS_REPLY_FLAGS);

					bytes_sent = sendto(sock,reply_packet,reply_packet_size,0,(struct sockaddr *) &client_address, sizeof(client_address));

					if(bytes_sent != reply_packet_size)
					{
						printf("Failed to send response DNS packet\n");
					}
				}
				else
				{
					printf("Failed to send DNS reply; DNS request packet appears to have an invalid length.\n");
				}
			}
		}
	}
	shutdown(sock, SHUT_RDWR);
	close(sock);
}

in_addr_t resolve_name(int sock, in_addr_t* nameservers, int nameservers_count, char* hostname, int persist)
{
	// construct the query message
	uint8_t answerbuf[1500];
	int rec_count = 0, i = 0;
	char recd_ns_names[20][255];
	in_addr_t recd_ns_ips[20];
	int recd_ns_count = 0;
	
	memset(recd_ns_ips, 0, sizeof(recd_ns_ips));
	
	do {
	uint8_t query[1500];
	int query_len=construct_query(query,1500,hostname);
	
	//in_addr_t chosen_ns = nameservers[random() % nameservers_count];
	in_addr_t chosen_ns = nameservers[nameservers_count/2];
	
	if(debug)
		printf("\nResolving %s using server %s\n", hostname, inet_ntoa(*(struct in_addr*)&chosen_ns));

	struct sockaddr_in addr; 	// internet socket address data structure
	addr.sin_family = AF_INET;
	addr.sin_port = htons(53); // port 53 for DNS
	addr.sin_addr.s_addr = chosen_ns; // destination address (any local for now)

	int send_count = sendto(sock, query, query_len, 0,(struct sockaddr*)&addr,sizeof(addr));
	if(send_count<0) { perror("Send failed");	exit(1); }	

	// await the response 
	//uint8_t answerbuf[1500];
	rec_count = recv(sock,answerbuf,1500,0);
	//rec_count = read(sock,answerbuf,1500);
	}
	while(persist && rec_count <= 0);
	
	// parse the response to get our answer
	struct dns_hdr *ans_hdr=(struct dns_hdr*)answerbuf;
	uint8_t *answer_ptr = answerbuf + sizeof(struct dns_hdr);
	
	// now answer_ptr points at the first question. 
	int question_count = ntohs(ans_hdr->q_count);
	int answer_count = ntohs(ans_hdr->a_count);
	int auth_count = ntohs(ans_hdr->auth_count);
	int other_count = ntohs(ans_hdr->other_count);

	// skip past all questions
	int q;
	for(q=0;q<question_count;q++) {
		char string_name[255];
		memset(string_name,0,255);
		int size=from_dns_style(answerbuf,answer_ptr,string_name);
		answer_ptr+=size;
		answer_ptr+=4; //2 for type, 2 for class
	}

	int a;
	//int got_answer=0;

	char last_auth_name[255];
	last_auth_name[0]=0;
	
	// now answer_ptr points at the first answer. loop through
	// all answers in all sections
	for(a=0;a<answer_count+auth_count+other_count;a++) 
	{
		// first the name this answer is referring to 
		char string_name[255];
		int dnsnamelen=from_dns_style(answerbuf,answer_ptr,string_name);
		answer_ptr += dnsnamelen;

		// then fixed part of the RR record
		struct dns_rr* rr = (struct dns_rr*)answer_ptr;
		answer_ptr+=sizeof(struct dns_rr);

		const uint8_t RECTYPE_A=1;
		const uint8_t RECTYPE_NS=2;
		const uint8_t RECTYPE_CNAME=5;
		const uint8_t RECTYPE_SOA=6;
		const uint8_t RECTYPE_PTR=12;
		const uint8_t RECTYPE_AAAA=28;

		if(htons(rr->type)==RECTYPE_A) {
			if(debug)
				printf("The hostname %s resolves to IP addr: %s\n", string_name, inet_ntoa(*((struct in_addr *)answer_ptr)));
			if(a<answer_count)
				return *((in_addr_t*)answer_ptr);
			else if(a>=answer_count+auth_count)
				{
					for(i=0; i<recd_ns_count; i++)
						if(strcmp(string_name, recd_ns_names[i]))
							recd_ns_ips[i]=*((in_addr_t*)answer_ptr);
				}
			//got_answer=1;
		}
		// NS record
		else if(htons(rr->type)==RECTYPE_NS) {
			//char ns_string[255];
			int ns_len=from_dns_style(answerbuf,answer_ptr,recd_ns_names[recd_ns_count]);
			if(debug)
				printf("The hostname %s can be resolved by NS: %s\n", string_name, recd_ns_names[recd_ns_count]);
			recd_ns_count++;
			//got_answer=1;
		}
		// CNAME record
		else if(htons(rr->type)==RECTYPE_CNAME) {
			char ns_string[255];
			int ns_len=from_dns_style(answerbuf,answer_ptr,ns_string);
			if(debug)
				printf("The hostname %s is also known as %s.\n", string_name, ns_string);
			if(a<answer_count)
				return resolve_name(sock, root_servers, root_server_count, ns_string, persist);
			//got_answer=1;
		}
		// PTR record
		else if(htons(rr->type)==RECTYPE_PTR) {
			char ns_string[255];
			int ns_len=from_dns_style(answerbuf,answer_ptr,ns_string);
			printf("The host at %s is also known as %s.\n", string_name, ns_string);
			return 0;
			//got_answer=1;
		}
		// SOA record
		else if(htons(rr->type)==RECTYPE_SOA) {
			if(debug)
				printf("Ignoring SOA record\n");
		}
		// AAAA record
		else if(htons(rr->type)==RECTYPE_AAAA)  {
			if(debug)
				printf("Ignoring IPv6 record\n");
		}
		else {
			if(debug)
				printf("got unknown record type %hu\n",htons(rr->type));
		} 

		answer_ptr+=htons(rr->datalen);
	}
	
	if(recd_ns_count>0)
	{
		for(i=0; i<recd_ns_count; i++)
		{
			if(recd_ns_ips[i]==0)
			{
				if(debug)
					printf("No A record for server %s, asking root servers.\n", recd_ns_names[i]);
				recd_ns_ips[i] = resolve_name(sock, root_servers, root_server_count, recd_ns_names[i], 0);
			}
		}
		return resolve_name(sock, recd_ns_ips, recd_ns_count, hostname, persist);
	}
	return 0;
}

#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <string.h>
#include <sys/types.h>
#include <sys/uio.h>
#include <sys/socket.h>
#include <sys/select.h>
#include <netinet/in.h>
#include "hw6.h"

#define ALPHA .75

FILE *fp = NULL;

int rtt_count;
int sequence_number;
unsigned int rtt;
enum user{
    UNKNOWN = 0,
    SENDER = 1,
    RECEIVER = 2
} user = UNKNOWN;

int timeval_to_msec(struct timeval *t) { 
	return t->tv_sec*1000+t->tv_usec/1000;
}

void msec_to_timeval(int millis, struct timeval *out_timeval) {
	out_timeval->tv_sec = millis/1000;
	out_timeval->tv_usec = (millis%1000)*1000;
}

int current_msec() {
	struct timeval t;
	gettimeofday(&t,0);
	return timeval_to_msec(&t);
}

int rel_connect(int socket,struct sockaddr_in *toaddr,int addrsize) {
        connect(socket,(struct sockaddr*)toaddr,addrsize);

		user = UNKNOWN; /* Extra safety */
		rtt = 100; /* 1200 msecs - Initial RTT */ 
		//rtt_count++;
		rtt_count = 0; /*Dont want to include initial RTT in Average */

}

int rel_rtt(int socket) {
		 return rtt;
}


void send_ack(int sock, int seq_no)
{
	char packet[1400];
	struct hw6_hdr *hdr = (struct hw6_hdr*)packet;
	hdr->sequence_number = htonl(sequence_number);
	hdr->ack_number = htonl(seq_no);

	send(sock, packet, sizeof(struct hw6_hdr), 0);
	sequence_number++;
}

int receive_ack(int sock, int timeout)
{
    int status = 1;
    char buf[1400];
	struct hw6_hdr *hdr = (struct hw6_hdr*)buf;

    struct timeval tv;
    msec_to_timeval(timeout,&tv);
    setsockopt(sock, SOL_SOCKET, SO_RCVTIMEO,(struct timeval *)&tv,sizeof(struct timeval));
    //setsockopt(sock, SOL_SOCKET, SO_RCVTIMEO,NULL,0);

    status = recv(sock, buf, MAX_PACKET, 0);
    if(status < 0) /* Timeout occured and ACK not rxed */
        return -2;

	int ack_number = ntohl(hdr->ack_number);
	if(ack_number >= 0){
	    return ack_number;
	}
	return -1;
}

void rel_send(int sock, void *buf, int len)
{
    int this_is_close_packet = 0;
    if(len == 0){
        this_is_close_packet = 1;
    }
    int status = -1;
    int send_time;
    int ack_time;
    if (user == UNKNOWN){
        fprintf(stderr, "<SHUBHAM> Identified sender\n");
        user = SENDER;
        fp = fopen("values","w+");
        if(fp == NULL){
            perror("Cant open file for storing means");
            exit(4);
        }
    }
 	// make the packet = header + buf
	char packet[1400];
	struct hw6_hdr *hdr = (struct hw6_hdr*)packet;
	hdr->sequence_number = htonl(sequence_number);
	hdr->ack_number	= htonl(-1);
	memcpy(hdr+1,buf,len); //hdr+1 is where the payload starts
	fprintf(stderr,"<SHUBHAM> Sent packet - %d\n",ntohl(hdr->sequence_number));
    send_time = current_msec(); /* RTT Start */
    int close_retry_attempts;
    close_retry_attempts = 0;
    while(status < 0){
        if(this_is_close_packet){
            close_retry_attempts++;
            if(close_retry_attempts == 20){
                fprintf(stderr,"<SHUBHAM> Close Packet -  %d was not acknowledged - %d times\n",ntohl(hdr->sequence_number),close_retry_attempts);
                break;
            }
        }
        send(sock, packet, sizeof(struct hw6_hdr)+len, 0);

        if(len >= 0){ /*IMP: need to wait for ACK for last packet/close packet, this is to inform other end to return 0 on rel_recv() */
            fprintf(stderr,"<SHUBHAM> Waiting for ACK %d ms <<\n",rtt );
            status = receive_ack(sock, rtt);
            if(status < sequence_number && status > 0){
                fprintf(stderr,"<SHUBHAM> Recevied Previous ACK - %d..ignoring...resending - %d\n",status, ntohl(hdr->sequence_number));
                status = -1;
                continue;
            }
            else if(status > sequence_number){
                fprintf(stderr,"<SHUBHAM> <FATAL> How is this possible");
                exit(2);
            }
            if(status > 0){
                ack_time = current_msec(); /* RTT end */
                /*
                {// Plain average 
                    //rtt = rtt / 2;
                    //
                    rtt = (rtt * rtt_count) + (ack_time - send_time);
                    rtt_count++;
                    rtt = rtt / rtt_count;
                    //rtt = rtt * 2;
                }*/
                {/*EWMA*/
                    rtt = (float)ALPHA * rtt + (float)(1 - ALPHA) * (ack_time - send_time);
                    rtt_count++;
                }
                if(fp){
                    fprintf(fp, "%d %d %d\n",rtt_count,(ack_time - send_time), rtt);
                }
                fprintf(stderr,"<SHUBHAM> GOT Ack - %d\n", status);
            }
            else{
                fprintf(stderr,"<SHUBHAM> MISSED Ack....Retransmitting - %d\n",ntohl(hdr->sequence_number));

            }
        }
        else {
            break;
        }

    }
	sequence_number++;
}

int rel_socket(int domain, int type, int protocol) {
	sequence_number = 1;
	return socket(domain, type, protocol);
}


int rel_recv(int sock, void * buffer, size_t length) {
    static int last_seq_no = 0;
	struct sockaddr_in fromaddr;
	unsigned int addrlen=sizeof(fromaddr);	
	char packet[MAX_PACKET];
	memset(&packet,0,sizeof(packet));
	struct hw6_hdr* hdr=(struct hw6_hdr*)packet;	

    if (user == UNKNOWN){
        fprintf(stderr, "<SHUBHAM> Identified receiver\n");
        user = RECEIVER;
    }

again:
	fprintf(stderr,"<SHUBHAM> Waiting for packet\n");
	// this is a shortcut to 'connect' a listening server socket to the incoming client.
	// after this, we can use send() instead of sendto(), which makes for easier bookkeeping
	//int recv_count = recvfrom(sock, packet, MAX_PACKET, 0, (struct sockaddr*)&fromaddr, &addrlen);		
    setsockopt(sock, SOL_SOCKET, SO_RCVTIMEO,NULL,0);
    int recv_count = recvfrom(sock, packet, MAX_PACKET, 0, (struct sockaddr*)&fromaddr, &addrlen);		
    if(connect(sock, (struct sockaddr*)&fromaddr, addrlen)) {
        perror("couldn't connect socket");
    }
	fprintf(stderr,"<SHUBHAM> Got packet\n");
	if(ntohl(hdr->sequence_number) <= last_seq_no){
        send_ack(sock,ntohl(hdr->sequence_number));
        fprintf(stderr,"<SHUBHAM> Detected Retramsitt - %d ..sending ack again and discarding\n", ntohl(hdr->sequence_number));
        goto again;
     }



	fprintf(stderr, "Got packet %d\n", ntohl(hdr->sequence_number));

	memcpy(buffer, packet+sizeof(struct hw6_hdr), recv_count-sizeof(struct hw6_hdr));

	fprintf(stderr,"<SHUBHAM> Sending ACK - %d\n", ntohl(hdr->sequence_number));
	char buf[100];
	

    send_ack(sock,ntohl(hdr->sequence_number));
    last_seq_no = ntohl(hdr->sequence_number);
	//sendto(sock, "ACK", 3, 0, (struct sockaddr*)&fromaddr, &addrlen);
	//send(sock, "ACK", 3, 0);
	//syncfs(sock);
	fprintf(stderr,"<SHUBHAM> ACK sent\n",ntohl(hdr->sequence_number));
	return recv_count - sizeof(struct hw6_hdr);
}

int rel_close(int sock) {
    if(user == SENDER){
        rel_send(sock, 0, 0); // send an empty packet to signify end of file
        if(fp)
            fclose(fp);
    }
    if(user == RECEIVER){
        //Do nothing
        //char buf[MAX_PACKET];
        //rel_recv(sock, buf, MAX_PACKET); // send an empty packet to signify end of file
    }
	close(sock);
}


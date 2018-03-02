#include <stdio.h>
#include <stdlib.h>
#include "project2.h"
 
/* ***************************************************************************
 ALTERNATING BIT AND GO-BACK-N NETWORK EMULATOR: VERSION 1.1  J.F.Kurose

   This code should be used for Project 2, unidirectional or bidirectional
   data transfer protocols from A to B and B to A.
   Network properties:
   - one way network delay averages five time units (longer if there
     are other messages in the channel for GBN), but can be larger
   - packets can be corrupted (either the header or the data portion)
     or lost, according to user-defined probabilities
   - packets may be delivered out of order.

   Compile as gcc -g project2.c student2.c -o p2
**********************************************************************/
//A side vars: (see A init for comments)
struct pkt buffer[50];//comments on buffer in readme
int seqnum;
int winbase;
int winsize;
int lastack;
int BUFFER;

//inwindow checks if a specific integer passed in is within the window for the sender
int inwindow(int s){
	if(s>=winbase && s<=winsize+winbase){
		return 1;
	}
	return 0;
}

//B side vars:
int acknum;

//common methods and constants:
const int DSIZE = 20;
const int TIME = 45;
//same checksum as ABP, assigns value to each spot so that even if bits are flipped or swapped can
//be caught
int check(struct pkt apkt){
	int a = 0;//sum of the packet
	a+= apkt.seqnum;//add seqnum
	a+= apkt.acknum * 2;//add 2* acknum
	for(int i =0; i < DSIZE; i++){//loop through the data
		a+=(i+2) * apkt.payload[i];//add data[0...20] * 3...22
	}
	return a;//return sum of packet
}
/********* STUDENTS WRITE THE NEXT SEVEN ROUTINES *********/
/* 
 * The routines you will write are detailed below. As noted above, 
 * such procedures in real-life would be part of the operating system, 
 * and would be called by other procedures in the operating system.  
 * All these routines are in layer 4.
 */

/* 
 * A_output(message), where message is a structure of type msg, containing 
 * data to be sent to the B-side. This routine will be called whenever the 
 * upper layer at the sending side (A) has a message to send. It is the job 
 * of your protocol to insure that the data in such a message is delivered 
 * in-order, and correctly, to the receiving side upper layer.
 */
void A_output(struct msg message) {
	struct pkt apkt;//create packet
	apkt.seqnum=seqnum;//assign data to packet
	for(int i =0; i <DSIZE; i++){//loop through and copy data to packet
		apkt.payload[i]=message.data[i];
	}
	apkt.acknum = 0;//acknum not used by sender
	apkt.checksum = check(apkt);//generate checksum
	buffer[seqnum] = apkt;//buffer the packet incase it's not in window or needs to be resent
	seqnum++;//increment seqnum
	if(inwindow(seqnum)){//if its in the window send it
		tolayer3(0,apkt);
	if(lastack==0 || seqnum==winbase){//if its the first packet or the first one after an ack start timer
		startTimer(0,TIME);
	}
	}
	else{//otherwise do nothing
		return;
	}
}

/*
 * Just like A_output, but residing on the B side.  USED only when the 
 * implementation is bi-directional.
 */
void B_output(struct msg message)  {

}

/* 
 * A_input(packet), where packet is a structure of type pkt. This routine 
 * will be called whenever a packet sent from the B-side (i.e., as a result 
 * of a tolayer3() being done by a B-side procedure) arrives at the A-side. 
 * packet is the (possibly corrupted) packet sent from the B-side.
 */
void A_input(struct pkt packet) {
	//if the ACK is correct and within the window and there is something in the buffer to send, send it
	if(buffer[lastack+winsize].acknum+1!=-1 && packet.checksum==check(packet) && inwindow(packet.acknum)){
		if(packet.acknum==lastack+1){//if this is base window packet(oldest unacked) start timer
			stopTimer(0);//stop timer for older packet if avaliable
			startTimer(0,TIME);//start the timer
		}
		lastack = packet.acknum;//set lastack to the most recent correct ack
		winbase = lastack + 1;//update window
		tolayer3(0,buffer[lastack+winsize+1]);//send packet
		return;
	}
	//removed but left in to show what the else actually checks for
	/*if(packet.acknum < lastack && packet.checksum==check(packet)){
		return;
	}
	if(packet.checksum!=check(packet)){
		return;
	}
	*/
	else if(packet.checksum==check(packet) && inwindow(packet.acknum)){
		if(packet.acknum==winbase){//if this is base window packet(oldest unacked) start timer
			stopTimer(0);//stop timer for older packet if avaliable
			startTimer(0,45);//start the timer
		}
		lastack = packet.acknum;
		winbase = lastack + 1;
	}
	else{//do nothing and wait for timeout if correct packet isn't recieved
		return;
	}

}

/*
 * A_timerinterrupt()  This routine will be called when A's timer expires 
 * (thus generating a timer interrupt). You'll probably want to use this 
 * routine to control the retransmission of packets. See starttimer() 
 * and stoptimer() in the writeup for how the timer is started and stopped.
 */
void A_timerinterrupt() {
	//this if is from an older version not necessary for the function, just an error checker
	if(getTimerStatus(0)){
		stopTimer(0);
	}
	startTimer(0,TIME);//restart the timer for the oldest packet
	for(int i = winbase; i < winbase+winsize; i++){//for all the packets in the window unacked resend
		if(buffer[i].acknum!=-1){//used to check if there is a packet buffered or not
			if(i == 0 || buffer[i].seqnum!=0){//^ same as above used to make sure there is a packet to send
				tolayer3(0,buffer[i]);//send packet
			}
		}
	}

}  

/* The following routine will be called once (only) before any other    */
/* entity A routines are called. You can use it to do any initialization */
void A_init() {
	seqnum = 0;//all values start at 0
	winbase = 0;
	winsize = 8;//window size of 8 as specified in project writeup
	lastack = 0;
	BUFFER = 50;//size of buffer
	for(int i =0; i < BUFFER; i++){//initalize all acknums in buffer to -1, is used to denote that the
		//packet has not been initlaized yet, otherwise it would be 0
		buffer[i].acknum = -1;
		}
}


/* 
 * Note that with simplex transfer from A-to-B, there is no routine  B_output() 
 */

/*
 * B_input(packet),where packet is a structure of type pkt. This routine 
 * will be called whenever a packet sent from the A-side (i.e., as a result 
 * of a tolayer3() being done by a A-side procedure) arrives at the B-side. 
 * packet is the (possibly corrupted) packet sent from the A-side.
 */
void B_input(struct pkt packet) {
	struct pkt bpkt;//create ACK packet
	if(packet.seqnum==acknum && packet.checksum == check(packet)){//if it is the expected packet
		struct msg bmsg;
		for(int i =0; i < DSIZE; i++){
			bmsg.data[i]=packet.payload[i];//copy data to send to 5
		}
		tolayer5(1,bmsg);
		bpkt.acknum= packet.seqnum;//generate new packet
		bpkt.seqnum= 0;
		bpkt.checksum = check(bpkt);
		acknum++;
	}
	if(packet.seqnum<acknum){//if the packet is below the most recent ACK do nothing out of order
		return;
	}
	else{//if the wrong packet was recieved, send the oldest ACK avaliable
		if(acknum>0){
		bpkt.acknum= acknum-1;
		bpkt.seqnum = 0;
		bpkt.checksum = check(bpkt);
		}
	}
	tolayer3(1,bpkt);//send ACK back
}

/*
 * B_timerinterrupt()  This routine will be called when B's timer expires 
 * (thus generating a timer interrupt). You'll probably want to use this 
 * routine to control the retransmission of packets. See starttimer() 
 * and stoptimer() in the writeup for how the timer is started and stopped.
 */
void  B_timerinterrupt() {
}

/* 
 * The following routine will be called once (only) before any other   
 * entity B routines are called. You can use it to do any initialization 
 */
void B_init() {
	acknum = 0;//acknum starts at 0

}


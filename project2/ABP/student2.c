#include <stdio.h>
#include <stdlib.h>
#include "project2.h"
#include <string.h>
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
//A side variables, only used be sender should not be accessed by receiver for point of the simulation
int seqnum;//seqnum between 0 and 1 for rdt3.0
int readyToSend;//readytosend where 0 is false can't send, and 1 is ready to send
int curpkt;//curpkt is the current message in the temp array of messages
int basetemp;//used to add new things to the array remembers upper bound
struct msg lastMessageA;//used to store the last message in case of retransmission
struct msg temp[100];//used to store temporary packets if the reciever recieves them too fast


//B side variable
int acknum;//acknum between 0 or 1 for rdt3.0


//common constants and functions:

const int MAXSEQ = 2;//used to keep ack and seq from 0 to 1
const int DSIZE = 20;//max size of data in a payload
const int TIME = 40;

//checksum algorithm uses placement of chars to determine if they were swapped, extremely simple, only works
//because size isn't a constraint, so each position in the packet simply gets a value between 0 and 22. Therefore there
//even if bits are swapped it will be detected.
int check(struct pkt pkt){
	int a = 0;
	a += pkt.seqnum;//add seqnum
	a+= pkt.acknum * 2;//add 2 * acknum
	for(int i =0; i < DSIZE; i++){
		a+= (pkt.payload[i] * (i)+2);//add 3...22 * payload[0...20]
	}
	return a;//return the checksum
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
	if(readyToSend==0){//if the sender cannot send as it hasn't recieved an ack yet store in temp
		temp[basetemp] = message;//store in temp
		basetemp++;//increment temp counter to know max
		if(basetemp==100){
			printf("Error has occurred, sender temporary queue can only hold 100, in an attempt to recover,"
					"the queue will be overwritten \n");
			basetemp=0;
		}
		return;
	}
	else{//if the sender can send
	struct pkt apkt;//creake a packet and fill in data
	apkt.acknum = 0;//acknum always zero as not used by receiver
	apkt.seqnum = seqnum;//set seqnum
	for(int i =0; i < DSIZE; i++){//set packet data to message data sent down from layer 5
		apkt.payload[i] = message.data[i];
	}
	apkt.checksum = check(apkt);//generate checksum
	tolayer3(0,apkt);//send packet from a to b
	startTimer(0,TIME);//start timer to detect loss
	readyToSend=0;//prevent other packets from being sent till ack or timeout
	for(int i =0; i < DSIZE; i++){//set last message data to this in case of timeout
		lastMessageA.data[i] = message.data[i];
	}
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
	if(packet.acknum==seqnum && packet.checksum==check(packet) && basetemp>0 && basetemp>curpkt){//if the ACK recieved is expected
		seqnum++;//increment seqnum but it only stays between 0 and 1
		seqnum = seqnum%MAXSEQ;
		stopTimer(0);//turn timer off
		readyToSend=1;//allow packets to be sent again
		A_output(temp[curpkt]);//send packet at the current place in temp
		curpkt++;//increment current packet counter
		curpkt = curpkt%100;//incase the program breaks, won't extend beyond array
		return;
	}
	else if(packet.acknum==seqnum && packet.checksum==check(packet) && basetemp==curpkt){
		stopTimer(0);//turn timer off
		readyToSend=1;//get ready to re-send
		seqnum++;//setup seq num
		seqnum = seqnum%2;
		return;//wait for packet from layer 5
	}
	else{//Does nothing, simply waits for timer to end to retransmit as found in  class slides
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
	readyToSend = 1;//retransmit lost packet
	A_output(lastMessageA);

}  

/* The following routine will be called once (only) before any other    */
/* entity A routines are called. You can use it to do any initialization */
void A_init() {
	seqnum = 0;//seqnum between 0 and 1 for rdt3.0
	readyToSend = 1;//readytosend where 0 is false can't send, and 1 is ready to send
	curpkt = 0;//curpkt is the current message in the temp array of messages
	basetemp = 0;//used to add new things to the array remembers upper bound
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
	struct msg bmsg;//create a message for the data
	if(packet.seqnum==acknum && packet.checksum==check(packet)){//if the correct packet arrived
	for(int i =0; i<DSIZE;i++){//transfer data to message for layer 5
		bmsg.data[i] = packet.payload[i];
	}
	tolayer5(1,bmsg);//send it up to the application
	struct pkt bpkt;//create an ACK packet
    bpkt.acknum = acknum;
    bpkt.seqnum = 0;//always 0 seqnum not used by receiver
    bpkt.checksum = check(bpkt);//generate checksum
	tolayer3(1,bpkt);//send packet
    acknum++;//increment acknum, always stays between 0 and 1
	acknum= acknum%MAXSEQ;
	return;
	}
	else{//if the wrong packet arrived send an ACK for the last number
		struct pkt bpkt;
		bpkt.acknum = (acknum +1)%MAXSEQ;
		bpkt.seqnum = 0;
		bpkt.checksum = check(bpkt);
		tolayer3(1,bpkt);
	}
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
	acknum = 0;//intialize acknum
}


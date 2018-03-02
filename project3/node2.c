#include <stdio.h>
#include "project3.h"

extern int TraceLevel;
extern float clocktime;

struct distance_table {
  int costs[MAX_NODES][MAX_NODES];
};
struct distance_table dt2;
struct NeighborCosts   *neighbor2;
void printdt2( int MyNodeNumber, struct NeighborCosts *neighbor, struct distance_table *dtptr );

/* students to write the following two routines, and maybe some others */

void rtinit2() {
	for(int i =0; i < MAX_NODES;i++){
		for(int j=0; j < MAX_NODES;j++){
			dt2.costs[i][j]=INFINITY;
		}
	}

	neighbor2 = getNeighborCosts(2);


	for(int i =0; i < MAX_NODES; i++){
		dt2.costs[i][i]=neighbor2->NodeCosts[i];
	}

	struct RoutePacket apkt;
	apkt.sourceid = 2;
	for(int i =0;i < MAX_NODES;i++){
		apkt.mincost[i]=INFINITY;
	}

	for(int i =0; i < MAX_NODES; i++){
		for(int j = 0; j < MAX_NODES; j++){
			if(dt2.costs[i][j]<apkt.mincost[j]){
				apkt.mincost[j] = dt2.costs[i][j];
			}
		}
	}
	for(int i =0; i< MAX_NODES; i++){
		if(neighbor2->NodeCosts[i]!=INFINITY && i!=apkt.sourceid){
			apkt.destid=i;
			toLayer2(apkt);
		}
	}
	struct distance_table *dtptr;
	dtptr = &dt2;
	//for(int i =0; i< MAX_NODES; i++){
		//for(int j =0; j< MAX_NODES; j++){
		//	printf("%d ",dtptr->costs[i][j]);
		//}
		//printf("\n");
	//}
	printf("Initialization of node 2, at time %f \n",clocktime);
	printdt2(2,neighbor2,dtptr);
}


void rtupdate2( struct RoutePacket *rcvdpkt ) {
	int source = rcvdpkt->sourceid;
		struct RoutePacket rtpkt;
		int isUpdate = 0;

		//intialize mins and obtain minimum cost to each node
		int mins[MAX_NODES];
		for(int i =0; i< MAX_NODES;i++){
			mins[i] = INFINITY;
			for(int j =0; j < MAX_NODES; j++){
				if(dt2.costs[i][j] < mins[i]){//if smaller then min set it
					mins[i] = dt2.costs[i][j];
				}
			}
		}

		if(dt2.costs[source][source] > rcvdpkt->mincost[2]){//if a shorter path is reported to node 0
			dt2.costs[source][source] = rcvdpkt->mincost[2];//change it
			isUpdate = 1;

		}

		for(int i =0; i < MAX_NODES;i++){//find new min paths by adding distance to source node
			if(dt2.costs[i][source] > rcvdpkt->mincost[i] + dt2.costs[source][source]){
				dt2.costs[i][source] = rcvdpkt->mincost[i] + dt2.costs[source][source];
				isUpdate = 1;

			}
		}


		//update mins using new data table
		for(int i =0; i < MAX_NODES; i++){
			for(int j =0; j < MAX_NODES; j++){
				if(dt2.costs[i][j] < mins[i]){
					isUpdate = 1;
					mins[i] = dt2.costs[i][j];

				}
			}
		}

		rtpkt.sourceid = 2;//configure new packet

		for(int i = 0; i < MAX_NODES;i++){//assign minimum cost values
				rtpkt.mincost[i] = mins[i];
			}

		if (TraceLevel > 1){
				printf("Node 2 is updating with data from %d \n",source);
			}
		if(isUpdate==1){//if updated send packets otherwise dont do anything
			for(int i =0; i< MAX_NODES; i++){
					if(neighbor2->NodeCosts[i]!=INFINITY && i!=rtpkt.sourceid){
						rtpkt.destid=i;
						toLayer2(rtpkt);//send out packet
					}
					if (TraceLevel > 1){
									printf("2 Updated, sending neighbor nodes packet with contents: ");
									for(int i =0; i < MAX_NODES; i++){
										printf("%d ",rtpkt.mincost[i]);
									}
									printf("\n time: %f \n",clocktime);
								}
		}
			struct distance_table *dtptr;
				dtptr = &dt2;
				printdt2(2,neighbor2,dtptr);//print table
		}
}



/////////////////////////////////////////////////////////////////////
//  printdt
//  This routine is being supplied to you.  It is the same code in
//  each node and is tailored based on the input arguments.
//  Required arguments:
//  MyNodeNumber:  This routine assumes that you know your node
//                 number and supply it when making this call.
//  struct NeighborCosts *neighbor:  A pointer to the structure 
//                 that's supplied via a call to getNeighborCosts().
//                 It tells this print routine the configuration
//                 of nodes surrounding the node we're working on.
//  struct distance_table *dtptr: This is the running record of the
//                 current costs as seen by this node.  It is 
//                 constantly updated as the node gets new
//                 messages from other nodes.
/////////////////////////////////////////////////////////////////////
void printdt2( int MyNodeNumber, struct NeighborCosts *neighbor, 
		struct distance_table *dtptr ) {
    int       i, j;
    int       TotalNodes = neighbor->NodesInNetwork;     // Total nodes in network
    int       NumberOfNeighbors = 0;                     // How many neighbors
    int       Neighbors[MAX_NODES];                      // Who are the neighbors

    // Determine our neighbors 
    for ( i = 0; i < TotalNodes; i++ )  {
        if (( neighbor->NodeCosts[i] != INFINITY ) && i != MyNodeNumber )  {
            Neighbors[NumberOfNeighbors] = i;
            NumberOfNeighbors++;
        }
    }
    // Print the header
    printf("                via     \n");
    printf("   D%d |", MyNodeNumber );
    for ( i = 0; i < NumberOfNeighbors; i++ )
        printf("     %d", Neighbors[i]);
    printf("\n");
    printf("  ----|-------------------------------\n");

    // For each node, print the cost by travelling thru each of our neighbors
    for ( i = 0; i < TotalNodes; i++ )   {
        if ( i != MyNodeNumber )  {
            printf("dest %d|", i );
            for ( j = 0; j < NumberOfNeighbors; j++ )  {
                    printf( "  %4d", dtptr->costs[i][Neighbors[j]] );
            }
            printf("\n");
        }
    }
    printf("\n");
}    // End of printdt2


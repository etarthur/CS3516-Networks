#include <stdio.h>
#include "project3.h"
#include <stdlib.h>

extern int TraceLevel;
extern float clocktime;
struct distance_table {
  int costs[MAX_NODES][MAX_NODES];
};
struct distance_table dt0;
struct NeighborCosts   *neighbor0;
void printdt0( int MyNodeNumber, struct NeighborCosts *neighbor, struct distance_table *dtptr);
/* students to write the following two routines, and maybe some others */

void rtinit0() {
	for(int i =0; i < MAX_NODES;i++){//loop through every index in dt0
		for(int j=0; j < MAX_NODES;j++){
			dt0.costs[i][j]=INFINITY;//assign them to default infinity
		}
	}

	neighbor0 = getNeighborCosts(0);//get distance to neighbors


	for(int i =0; i < MAX_NODES; i++){
		dt0.costs[i][i]=neighbor0->NodeCosts[i];//assign costs to neighbors in table
	}

	struct RoutePacket apkt;//create packet
	apkt.sourceid = 0;//from 0
	for(int i =0;i < MAX_NODES;i++){//default mincost to infinity
		apkt.mincost[i]=INFINITY;
	}

	for(int i =0; i < MAX_NODES; i++){//loop through the entire dt0 table
		for(int j = 0; j < MAX_NODES; j++){
			if(dt0.costs[i][j]<apkt.mincost[j]){//check if table entry is lower if so take it
				apkt.mincost[j] = dt0.costs[i][j];//assign table entry to mincost
			}
		}
	}
	for(int i =1; i< MAX_NODES; i++){//loop through neighboring nodes
		if(neighbor0->NodeCosts[i]!=INFINITY && i!=apkt.sourceid){//have to be connected directly
			apkt.destid=i;
			toLayer2(apkt);
		}
	}
	struct distance_table *dtptr;//generate pointer for table
	dtptr = &dt0;//assign it to dt0
	//for(int i =0; i< MAX_NODES; i++){
		//for(int j =0; j< MAX_NODES; j++){
		//	printf("%d ",dtptr->costs[i][j]);
		//}
		//printf("\n");
	//}
	printf("Initialization of node 0, at time %f \n",clocktime);
	printdt0(0,neighbor0,dtptr);//print table
}


void rtupdate0( struct RoutePacket *rcvdpkt ) {
	int source = rcvdpkt->sourceid;//generate source number from packet
	struct RoutePacket rtpkt;//create packet to send out
	int isUpdate = 0;//boolean for whether the mincost updates or not

	//intialize mins and obtain minimum cost to each node
	int mins[MAX_NODES];//generate mins
	for(int i =0; i< MAX_NODES;i++){
		mins[i] = INFINITY;//mins default to infinity
		for(int j =0; j < MAX_NODES; j++){
			if(dt0.costs[i][j] < mins[i]){//if smaller than min take it
				mins[i] = dt0.costs[i][j];//assign to min smaller value
			}
		}
	}

	if(dt0.costs[source][source] > rcvdpkt->mincost[0]){//if a shorter path is reported to node
		dt0.costs[source][source] = rcvdpkt->mincost[0];//change path to shorter one
		isUpdate = 1;//updated

	}

	for(int i =0; i < MAX_NODES;i++){//find new min paths by adding distance to source node
		if(dt0.costs[i][source] > rcvdpkt->mincost[i] + dt0.costs[source][source]){//if the path is is longer than the distance to the source
			//node plus its mincost
			dt0.costs[i][source] = rcvdpkt->mincost[i] + dt0.costs[source][source];//assign shorter path
			isUpdate = 1;//updated

		}
	}


	//update mins using new data table
	for(int i =0; i < MAX_NODES; i++){//loop through the data table
		for(int j =0; j < MAX_NODES; j++){
			if(dt0.costs[i][j] < mins[i]){//if value is smaller than min
				isUpdate = 1;//updated
				mins[i] = dt0.costs[i][j];//assign smaller value

			}
		}
	}

	rtpkt.sourceid = 0;//configure new packet

	for(int i = 0; i < MAX_NODES;i++){//assign minimum cost values
			rtpkt.mincost[i] = mins[i];
		}

	if (TraceLevel > 1){//if debugging print helpful data
		printf("Node 0 is processing data from %d \n",source);
	}

	if(isUpdate==1){//if updated send packets otherwise dont do anything
		for(int i =1; i< MAX_NODES; i++){//loop through neighboring nodes
				if(neighbor0->NodeCosts[i]!=9999 && i!=rtpkt.sourceid){//have to be connected
					rtpkt.destid=i;
					toLayer2(rtpkt);//send out packet
				}
	}
		if (TraceLevel > 1){//print out helpful stuff
				printf("0 Updated, sending neighbor nodes packet with contents: ");
				for(int i =0; i < MAX_NODES; i++){
					printf("%d ",rtpkt.mincost[i]);
				}
				printf("\n time: %f \n",clocktime);
			}
		struct distance_table *dtptr;//assign table
			dtptr = &dt0;
			printdt0(0,neighbor0,dtptr);//print table
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
void printdt0( int MyNodeNumber, struct NeighborCosts *neighbor, 
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
}    // End of printdt0


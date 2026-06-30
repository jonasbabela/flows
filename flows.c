#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <ctype.h>
#include <math.h>

typedef struct Flow_ Flow;
typedef struct Cluster_ Cluster; 

struct Flow_ {
    int id; //id of flow
    double totalBytes; //total bytes transmitted
    double duration; //time duration
    double packetCount; //number of packets
    double avgTime; //average interarrival time
    double avgLen; //average length of packet
    Cluster *currentlyIn; //pointer to cluster of flow
};

struct Cluster_ {
    int flowCount; //number of flows
    Flow **data; //array of pointers to flows
};

typedef struct {
    double distance; //euclidean distance
    Flow *flow1; //pointer to first flow
    Flow *flow2; //pointer to second flow
} Distance;

typedef struct {
    int len; //number of distances
    Distance *data; //array of distances
} DistArr;

typedef struct {
    int len; //number of clusters
    Cluster *data; //array of clusters
} ClsArr;


/*Creates array of flows
&param len number of flows*/
Flow *fArrayCtor (int len)  {

    Flow *arr = malloc(sizeof(Flow) * len); //Allocate memory for -len flows

    //Check if allocation was successful
    if (!arr)   {
        fprintf(stderr, "%s\n", "Error: Memory allocation failed.");
        }
    
    return arr;
}

/*Destroys array of flows
@param arr pointer to array of flows*/
void fArrayDtor (Flow *arr)   {

    if (arr)
        free(arr);
}

/*Increments array of pointers to flows
@param cls pointer to incremented cluster
@param newSize number of total flows*/
void fArrayInc (Cluster *cls, int newSize)  {

    //Reallocate memory for -newSize pointers
    Flow **arr = realloc(cls->data, newSize * sizeof(Flow *));

    //Check if reallocation was successful
    if (!arr)
        fprintf(stderr, "%s\n", "Error: Memory reallocation failed.");
    
    cls->data = arr; //Set cluster's array pointer to reallocated memory
}

/*Checks validity of input data
@param flow flow with loaded data*/
int flowCheck (Flow flow)   {

    if (flow.avgTime < 0 ||
        flow.totalBytes < 0 ||
        flow.packetCount <= 0 ||
        flow.duration < 0)
        return 1;
    return 0;
}

/*Load flows from file to array of flows
@param fileName name of input file
@param arr array of flows*/
int flowsLoad (char *fileName, Flow *arr)   {

    FILE *data = fopen(fileName, "r"); //Open file
    
    //Check if file was loaded correctly
    if (!data)  {
        fprintf(stderr, "%s\n", "Error: File open failed.");
        return 1;
    }
    
    //Save the number of flows in a file
    int count;
    fscanf(data, "count=%d", &count);
    
    //Save flows to structures
    for (int i = 0; i < count; i ++)    {

        Flow flowTbp;
        fscanf(data, 
        "%d  %*s  %*s  %lf  %lf  %lf  %lf",
        &flowTbp.id, &flowTbp.totalBytes, &flowTbp.duration, 
        &flowTbp.packetCount, &flowTbp.avgTime);

        //Check data validity
        if (flowCheck(flowTbp) == 1)    {
            fprintf(stderr, "%s\n", "Error: Invalid data format.");
            fclose(data);
            return 1;
        }

        //Calculate average length of single packet
        flowTbp.avgLen = flowTbp.totalBytes / flowTbp.packetCount;
        flowTbp.currentlyIn = NULL; //Initialize pointer to current cluster
        arr[i] = flowTbp; //Save loaded flow to array of flows
    }

    fclose(data); //Close file
    return 0;
}


/*Creates array of distances
@param len number of distances*/
DistArr dArrayCtor (int len)  {
    
    DistArr distances;
    
    //Allocate memory for -len distances
    Distance *arr = malloc(sizeof(Distance) * len); 

    //Check if allocation was successful
    if (!arr)   
        fprintf(stderr, "%s\n", "Error: Memory allocation failed.");

    //Initialize structure
    distances.len = len;
    distances.data = arr;

    return distances;
}

/*Destroys array of distances
@param dist pointer to array of distances*/
void dArrayDtor (DistArr *dist) {
    
    if (dist->data)
        free(dist->data);
    
    dist->len = 0;
}


/*Creates and initializes cluster with single flow
@param flow pointer to the flow to be assigned*/
Cluster clusterCtor (Flow *flow)   {

    Cluster new;
    new.flowCount = 1;   

    //Allocate memory for one flow
    new.data = malloc(sizeof(Flow *));   
    
    if (!new.data)  {
        fprintf(stderr, "%s\n", "Error: Memory allocation failed.");
        return new;
    }

    //Save the flow as the first item in allocated bytes
    new.data[0] = flow; 

    return new;
}

/*Destroys cluster
@param cls pointer to cluster*/
void clusterDtor (Cluster *cls)    {

    cls->flowCount = 0;

    if (cls->data)  {
        
        free(cls->data);
        cls->data = NULL;
    }
}


/*Creates array of clusters
@param len number of clusters*/
ClsArr clsArrCtor (int len) {
    
    ClsArr clusters;

    //Allocate memory
    clusters.data = malloc(len * sizeof(Cluster));

    //Check if allocation was successful
    if (!clusters.data)
        fprintf(stderr, "%s\n", "Error: Memory allocation failed.");
    
    clusters.len = len;

    return clusters;
}

/*Destroys array of clusters
@param clusters pointer to array of clusters*/
void clsArrDtor (ClsArr *clusters) {

    //Destroy all clusters in array
    for (int i = 0; i < clusters->len; i ++)  {
        clusterDtor(&clusters->data[i]);
    }

    free(clusters->data);
    clusters->data = NULL;
    
    clusters->len = 0;
}


/*Compares distances for qSort function
@param a first distance
@param b second distance*/
int distSortComp (const void *a, const void *b)   {

    const Distance *dist1 = a;
    const Distance *dist2 = b;

    if (dist1->distance > dist2->distance) return 1;
    if (dist1->distance < dist2->distance) return -1;
    return 0;
}

/*Sorts array of distances
@param distances array of distances
@param len number of distances*/
void distanceSort (Distance *distances, int len)    {

    qsort(distances, len, sizeof(Distance), &distSortComp);
}

/*Calculates euclidean distance between flows
@param wB weight B
@param wT weight T
@param wD weight D
@param wS weight S
@param flow1 first flow
@param flow2 second flow*/
double distCalc(double wB, double wT, double wD, double wS, 
    Flow flow1, Flow flow2) {

    //Calculates subtotals of formula
    double chckB = wB * pow(flow1.totalBytes - flow2.totalBytes, 2);
    double chckT = wT * pow(flow1.duration - flow2.duration, 2);
    double chckD = wD * pow(flow1.avgTime - flow2.avgTime, 2);
    double chckS = wS * pow(flow1.avgLen - flow2.avgLen, 2);

    //Calculates square root of sum of subtotals
    double sum = sqrt(chckB + chckD + chckS + chckT);

    return sum;
}


/*Calculates distances between all flows
@param flows array of flows
@param count number of flows
@param weights array of weights for calculation*/
DistArr flows2dists (Flow *flows, int count, double *weights) {

    int distanceCount = (count * (count - 1)) / 2;
    DistArr dArray = dArrayCtor(distanceCount); //Create empty array for distances

    if (!dArray.data)    {
        dArray.data = NULL;
        dArray.len = 0;
        return dArray;
    }

    dArray.len = distanceCount;

    int arrIdx = 0; //Set position in array to 0

    //Calculate distance for each pair of flows
    for (int i = 0; i < count; i ++)    {
        
        //j = i - Exclude duplicites
        //j = i + 1 - Exclude all self-flow relations
        for (int j = i + 1; j < count; j ++)    {
            
            dArray.data[arrIdx].distance = 
            distCalc(weights[0], weights[1], weights[2], 
            weights[3], flows[i], flows[j]); //Calculate distance using weights
            
            dArray.data[arrIdx].flow1 = &flows[i]; //Link first flow to distance
            dArray.data[arrIdx].flow2 = &flows[j]; //Link second flow to distance

            arrIdx ++; //Increment index in array
        }
    }

    distanceSort(dArray.data, distanceCount); //Sort distances
    return dArray;
}

/*Assigns one flow to one cluster
@param flows array of flows
@param len number of flows*/
ClsArr flows2clusters (Flow *flows, int len)    {

    //Create cluster
    ClsArr clusters = clsArrCtor(len);
    if (!clusters.data)
        return clusters;

    for (int i = 0; i < len; i ++)  {
        
        //Assign flow to cluster
        clusters.data[i] = clusterCtor(&flows[i]);

        if (!clusters.data[i].data)  {
            clusters.len = i;
            clsArrDtor(&clusters);
            return clusters;
        }

        //Update flow's pointer to cluster
        clusters.data[i].data[0]->currentlyIn = &clusters.data[i];
    }

    return clusters;

}



/*Merges second cluster to first cluster
@param cls1 first cluster
@param cls2 second cluster*/
Cluster* clusterMerge (Cluster *cls1, Cluster *cls2)   {

    int oldCount = cls1->flowCount; //Original number of flows in first cluster
    int newCount = cls1->flowCount + cls2->flowCount; //Sum of floats in both clusters

    fArrayInc(cls1, newCount); //Increment the 1st cluster's array of flows
    
    if (!cls1->data)
        return NULL;

    cls1->flowCount = newCount;

    //Copy all flows from the 2nd cluster to the 1st cluster
    for (int i = 0; i < cls2->flowCount; i ++)  {

        int idx = i + oldCount;  //Position in 1st cluster's array
        
        //Copy i-th flow from the 2nd cluster to the 1st cluster
        cls1->data[idx] = cls2->data[i];   
        cls1->data[idx]->currentlyIn = cls1;
    }

    return cls2; //Return pointer to destroyed cluster
}

/*Finds cluster in an array of clusters
@param cls cluster to find
@param clusters array of clusters*/
int clusterFind (Cluster *cls, ClsArr *clusters)    {

    //Return its index if found
    for (int i = 0; i < clusters->len; i ++)    {
        if (&clusters->data[i] == cls)
            return i;   
    }

    return -1;
}

/*Swaps last cluster with destroyed cluster and decreases size
@param clusters array of clusters
@param cls destroyed cluster*/
void clusterRemove (ClsArr *clusters, Cluster *cls) {

    int lastIdx = clusters->len - 1; //Index of last cluster in array
    int remIdx = clusterFind(cls, clusters); //Index of destroyed cluster

    //Swap clusters
    Cluster tmp = clusters->data[lastIdx];
    clusters->data[lastIdx] = clusters->data[remIdx];
    clusters->data[remIdx] = tmp;

    //Update flows' pointers to clusters to new address
    for (int i = 0; i < clusters->data[remIdx].flowCount; i ++) {

        clusters->data[remIdx].data[i]->currentlyIn = &clusters->data[remIdx];
    }

    clusters->len = clusters->len - 1; //Decrease size of array

    clusterDtor(&clusters->data[lastIdx]); //Destroy second cluster
}

/*Merges clusters until required amount
@param clusters array of clusters
@param distances array of distances
@param finalCount required number of clusters*/
int clusterCycle (ClsArr *clusters, DistArr *distances, int finalCount) {

    int idx = 0; //Index in array of distances

    //Repeat while too many clusters remain and there are still distances to process
    while (clusters->len > finalCount && idx < distances->len)  {

        Flow *flow1 = distances->data[idx].flow1; //Pointer to first flow from distance
        Flow *flow2 = distances->data[idx].flow2; //Pointer to second flow from distance
        
        //Merge their clusters if not already merged
        if (flow1->currentlyIn != flow2->currentlyIn)   {

            Cluster *toRemove = clusterMerge(flow1->currentlyIn, flow2->currentlyIn);
            
            if (!toRemove)
                return 1;

            clusterRemove(clusters, toRemove); //"Remove" second cluster from array
        }

        idx ++; //Increment position in distances' array
    }

    return 0;
}

/*Prints final clusters and flow IDs
@param clusters array of clusters*/
void clusterPrint (ClsArr *clusters)    {

    printf("Clusters:\n");
    
    for (int i = 0; i < clusters->len; i ++)    {
        
        printf("cluster %d:", i); //Print i-th cluster

        for (int j = 0; j < clusters->data[i].flowCount; j ++)  {

            //Print all flow IDs from i-th cluster
            printf(" %d", clusters->data[i].data[j]->id);
        }

        printf("\n");
    }
}


/*Compares flows for qSort function
@param a first flow
@param b second flow*/
int flowSortComp (const void *a, const void *b)   {

    const Flow *flow1 = *(Flow **)a;
    const Flow *flow2 = *(Flow **)b;

    if (flow1->id > flow2->id) return 1;
    if (flow1->id < flow2->id) return -1;
    return 0;
}

/*Sorts array of pointers to flows
@param flows array of pointers to flows
@param len number of pointers to flows*/
void flowSort (Flow **flows, int len)    {

    qsort(flows, len, sizeof(Flow *), &flowSortComp);
}


/*Compares clusters for qSort function
@param a first cluster
@param b second cluster*/
int clusterSortComp (const void *a, const void *b)   {

    const Cluster *cluster1 = a;
    const Cluster *cluster2 = b;

    if (cluster1->data[0]->id > cluster2->data[0]->id) return 1;
    if (cluster1->data[0]->id < cluster2->data[0]->id) return -1;
    return 0;
}

/*Sorts array of clusters
@param clusters array of clusters
@param len number of clusters*/
void clusterSort (Cluster *clusters, int len)    {

    qsort(clusters, len, sizeof(Cluster), &clusterSortComp);
}


/*Checks validity of file data
@param fileName name of input file*/
int fileCheck (char *fileName)  {
    
    FILE *data = fopen(fileName, "r");

    //Check if file opened correctly
    if (!data)  {
        fprintf(stderr, "%s\n", "Error: File open failed");
        return -1;
    }

    char buffer[100]; //Auxiliary variable to check first line
    
    //Check if file is not empty
    if (!fgets(buffer, sizeof(buffer), data))   {
        
        fprintf(stderr, "%s\n", "Error: File is empty");
        fclose(data);
        return -1;
    }

    //Check if first line contains 'count='
    if ((strncmp(buffer, "count=", 6)) != 0) {
        
        fprintf(stderr, "%s\n", "Error: Invalid file data");
        fclose(data);
        return -1;
    }

    fclose(data);
    return (atoi(buffer + 6)); //return number of flows in file
}

/*Checks validity of arguments
@param argc number of arguments
@param arguments array of pointers to arguments*/
int argCheck (int argc, char **arguments) {

    //Return if only file name is inserted
    if (argc == 2)
        return 0;

    //Check validity of required cluster count
    if (atoi(arguments[2]) <= 0)    {
        
        fprintf(stderr, "%s\n", "Error: Invalid arguments.");
        return 1;
    }

    //Check validity of weights
    for (int i = 3; i < argc; i ++)    {
        
        if (atoi(arguments[i]) < 0) {
        
            fprintf(stderr, "%s\n", "Error: Invalid arguments.");
            return 1;
        }
    }

    return 0;
}


/*Completes algorithm
@param argc number of arguments
@param arguments array of pointers to arguments
@param merge 0/1 - is merging algorithm necessary*/
int mergeComplete(int argc, char **arguments, int merge)  {

    //Check arguments
    if (argCheck(argc, arguments) == 1)
        return 1;
    
    //Check input file
    int count = fileCheck(arguments[1]);

    if (count == -1)
        return 1;
    
    //Check if there are more required clusters than flows 
    if (argc == 7 && atoi(arguments[2]) > count)    {
        fprintf(stderr, "%s\n", "Error: Too many required clusters.");
        return 1;
    }

    //Initialize array of flows
    Flow *flows = fArrayCtor(count);
    if (flows == NULL)  
        return 1;
    
    if (flowsLoad(arguments[1], flows) == 1)    {
        fArrayDtor(flows);
        return 1;
    }
    
    //Initialize array of clusters
    ClsArr clusters = flows2clusters(flows, count);
    if (!clusters.data) {
        fArrayDtor(flows);
        return 1;
    }

    //Do merging if necessary
    if (merge == 1) {
        
        //Insert weights in array
        double weights[4] = {atoi(arguments[3]), atoi(arguments[4]),
        atoi(arguments[5]), atoi(arguments[6])};

        //Calculate distances
        DistArr distances = flows2dists(flows, count, weights);
        if (!distances.data) {
            fArrayDtor(flows);
            clsArrDtor(&clusters);
            return 1;
        }    
        
        //Do merging
        if (clusterCycle(&clusters, &distances, atoi(arguments[2])) == 1)   {
            fArrayDtor(flows);
            clsArrDtor(&clusters);
            dArrayDtor(&distances);
            return 1;
        }    
        dArrayDtor(&distances); //Destroy array of distances
    }
    
    //Sort flows by ID in each cluster
    for (int i = 0; i < clusters.len; i ++) {
        flowSort(clusters.data[i].data, clusters.data[i].flowCount);
    }
    
    //Sort and print final clusters
    clusterSort(clusters.data, clusters.len);
    clusterPrint(&clusters);

    clsArrDtor(&clusters); //Destroy array of clusters
    fArrayDtor(flows); //Destroy array of flows


    return 0;
}

int main(int argc, char **argv) {

    int returnValue;
    
    //Required number of final clusters is inserted
    if (argc == 7)  {
        
        returnValue = mergeComplete(argc, argv, 1);
    }
    
    //Required number of final clusters is not inserted
    else if (argc == 6 || argc == 2) {
        
        returnValue = mergeComplete(argc, argv, 0);
    }

    //Wrong number of arguments is inserted
    else    {
        fprintf(stderr, "%s\n", "Syntax: ./flows N WB WT WD WS");
        returnValue = 1;
    }
    
    return returnValue;
}
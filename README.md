# Flow Clustering (C)

## Overview

This project implements hierarchical clustering (single linkage) on network flow data. Each flow is represented as a feature vector, and clusters are iteratively merged based on a weighted Euclidean distance until the desired number of clusters is reached.

## Algorithm

The clustering is performed using **single linkage hierarchical clustering**:

1. Each flow starts as its own cluster.
2. At each step, the two closest clusters are merged.
3. Cluster distance is defined as the minimum distance between any pair of flows from two clusters.
4. The process repeats until the target number of clusters is reached.

## Features

Each network flow is represented using the following features:

* **b** – total bytes transferred
* **t** – flow duration
* **d** – average inter-arrival time
* **s** – average packet size (computed)

Distance between flows is computed using a **weighted Euclidean distance**.

## Input format

The input file starts with:

```
count=X
```

Each subsequent line describes one flow:

```
FLOWID SRC_IP DST_IP TOTAL_BYTES FLOW_DURATION PACKET_COUNT AVG_INTERARRIVAL
```

## Output format

```
Clusters:
cluster 0: ID1 ID2 ...
cluster 1: ID3 ID4 ...
```

Rules:

* Flow IDs inside each cluster are sorted ascending
* Clusters are sorted by their smallest flow ID

## Build

```bash id="c2k9qp"
cc -std=c11 -Wall -Wextra -Werror -pedantic flows.c -o flows -lm
```

## Usage

```bash id="a91m2d"
./flows data.txt N WB WT WD WS
```

If `N` is not provided, each flow remains its own cluster.

## Key concepts

* Hierarchical clustering (single linkage)
* Weighted Euclidean distance
* Dynamic memory management in C
* Data parsing from structured input
* Algorithm design and optimization

## Notes

This project was implemented as a university assignment focused on data processing, clustering algorithms, and memory management in C.

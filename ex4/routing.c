#include <stdio.h>
#define MAX 20
#define INF 99999

void distanceVector(int n, int cost[MAX][MAX]);
void linkState(int n, int cost[MAX][MAX]);

void printGraphTable(int n, int distance[MAX][MAX]);

void printNodeTable(int n, int router,int distance [MAX][MAX],
                    int nextHop[MAX][MAX]);

void findAndPrintPath(int n, int source, int destination,
                      int distance[MAX][MAX],
                      int nextHop[MAX][MAX]);

/* Link State Helper Functions */

void runDijkstra(int n, int cost[MAX][MAX],
                 int source,
                 int distance[MAX],
                 int parent[MAX],
                 int showSteps);

void printLinkStateShortestPath(int n, int cost[MAX][MAX],
                                int source, int destination);

int main()
{
    int n, choice;
    int cost[MAX][MAX];
    int i, j;
    int u, v, new_cost;

    printf("============================================================\n");
    printf("             ROUTING ALGORITHMS SIMULATION\n");
    printf("============================================================\n");

    printf("\nEnter number of routers: ");
    scanf("%d", &n);

    if (n <= 0 || n > MAX)
    {
        printf("Invalid number of routers!\n");
        return 0;
    }

    printf("\nEnter Cost Matrix\n");
    printf("Use -1 for NO direct connection.\n");
    printf("Enter 0 for the same router.\n\n");

    for (i = 0; i < n; i++)
    {
        for (j = 0; j < n; j++)
        {
            scanf("%d", &cost[i][j]);

            if (cost[i][j] == -1)
                cost[i][j] = INF;

            if (i == j)
                cost[i][j] = 0;
        }
    }

    do
    {
        printf("\n============================================================\n");
        printf("                        MAIN MENU\n");
        printf("============================================================\n");
        printf("1. Distance Vector Routing Algorithm\n");
        printf("2. Link State Routing Algorithm\n");
        printf("3. Change Link Cost\n");
        printf("4. Exit\n");
        printf("============================================================\n");

        printf("Enter your choice: ");
        scanf("%d", &choice);

        switch (choice)
        {
            case 1:
                distanceVector(n, cost);
                break;

            case 2:
                linkState(n, cost);
                break;

            case 3:
                printf("\nEnter the two nodes between which you want to change the connection (u v): ");
                scanf("%d %d", &u, &v);
                if (u >= 0 && u < n && v >= 0 && v < n)
                {
                    printf("Enter the new cost (Use -1 for NO direct connection, 0 for same node): ");
                    scanf("%d", &new_cost);

                    if (new_cost == -1)
                        new_cost = INF;

                    cost[u][v] = new_cost;
                    cost[v][u] = new_cost; /* Assuming an undirected graph matching typical lab simulations */
                    printf("\nLink cost updated successfully between %d and %d!\n", u, v);
                }
                else
                {
                    printf("\nInvalid node indices!\n");
                }
                break;

            case 4:
                printf("\nExiting Program...\n");
                break;

            default:
                printf("\nInvalid Choice!\n");
        }

    } while (choice != 4);

    return 0;
}



void printGraphTable(int n, int distance[MAX][MAX])
{
    int i, j;
    printf("~~~~~~~~~~~~~~~~ FINAL ROUTING TABLE FOR GRAPH ~~~~~~~~~~~~~~\n\n");
    printf("%12s", "Source");

    for (j = 0; j < n; j++)
    {
        printf("%8d", j);
    }

    printf("\n");

    printf("------------");

    for (j = 0; j < n; j++)
    {
        printf("--------");
    }

    printf("\n");

    for (i = 0; i < n; i++)
    {
        printf("%10d |", i);

        for (j = 0; j < n; j++)
        {
            if (distance[i][j] >= INF)
                printf("%8s", "INF");
            else
                printf("%8d", distance[i][j]);
        }

        printf("\n");
    }
}



void printNodeTable(int n, int router,int distance[MAX][MAX],int nextHop[MAX][MAX])
{
    int j;

    printf("\n---------------- FINAL ROUTING TABLE FOR NODE %d ----------------\n\n",
           router);

    printf("+----------------+----------------+----------------+\n");
    printf("| Destination    | Cost           | Next Hop       |\n");
    printf("+----------------+----------------+----------------+\n");

    for (j = 0; j < n; j++)
    {
        printf("| %-14d |", j);

        /* Cost */

        if (distance[router][j] >= INF)
            printf(" %-14s |", "INF");
        else
            printf(" %-14d |", distance[router][j]);


        /* Next Hop */

        if (j == router)
            printf(" %-14s |\n", "-");

        else if (nextHop[router][j] == -1)
            printf(" %-14s |\n", "No Path");

        else
            printf(" %-14d |\n", nextHop[router][j]);
    }

    printf("+----------------+----------------+----------------+\n");
}

void findAndPrintPath(int n, int source, int destination,
                      int distance[MAX][MAX],
                      int nextHop[MAX][MAX])
{
    int current;
    int count = 0;
    printf("************ SHORTEST PATH **************\n");


    if (source < 0 || source >= n ||
        destination < 0 || destination >= n)
    {
        printf("\nInvalid Source or Destination Node!\n");
        return;
    }

    if (distance[source][destination] >= INF ||
        nextHop[source][destination] == -1)
    {
        printf("\nNo Path Exists from Node %d to Node %d\n",
               source, destination);
        return;
    }

    printf("\nSource Node      : %d\n", source);
    printf("Destination Node : %d\n", destination);

    printf("\nShortest Path    : ");

    current = source;

    printf("%d", current);

    while (current != destination)
    {
        current = nextHop[current][destination];

        if (current == -1 || count >= MAX)
        {
            printf("\nPath Error!\n");
            return;
        }

        printf(" -> %d", current);

        count++;
    }

    printf("\nTotal Cost       : %d\n",
           distance[source][destination]);
}

void distanceVector(int n, int cost[MAX][MAX])
{
    int distance[MAX][MAX];
    int nextHop[MAX][MAX];

    int i, j, k;
    int updated;

    int router;
    int source, destination;

    printf("\n============================================================\n");
    printf("           DISTANCE VECTOR ROUTING ALGORITHM\n");
    printf("============================================================\n");


    /* Initialize distance and next hop */

    for (i = 0; i < n; i++)
    {
        for (j = 0; j < n; j++)
        {
            distance[i][j] = cost[i][j];

            if (i == j)
            {
                distance[i][j] = 0;
                nextHop[i][j] = i;
            }
            else if (cost[i][j] < INF)
            {
                nextHop[i][j] = j;
            }
            else
            {
                nextHop[i][j] = -1;
            }
        }
    }


    /* Distance Vector Algorithm */

    do
    {
        updated = 0;

        for (i = 0; i < n; i++)
        {
            for (k = 0; k < n; k++)
            {
                if (i != k && cost[i][k] < INF)
                {
                    for (j = 0; j < n; j++)
                    {
                        if (distance[k][j] < INF)
                        {
                            int newDistance =
                                cost[i][k] + distance[k][j];

                            if (newDistance < distance[i][j])
                            {
                                distance[i][j] = newDistance;

                                nextHop[i][j] = k;

                                updated = 1;
                            }
                        }
                    }
                }
            }
        }

    } while (updated);


    /* Print final graph routing table */

    printGraphTable(n, distance);


    /* Ask for routing table of a node */

    printf("\nEnter the node whose routing table you want to display: ");
    scanf("%d", &router);

    if (router >= 0 && router < n)
    {
        printNodeTable(n, router,distance,nextHop);
    }
    else
    {
        printf("\nInvalid Node!\n");
    }


    /* Ask source and destination */

    printf("\nEnter source node to find shortest path: ");
    scanf("%d", &source);

    printf("Enter destination node: ");
    scanf("%d", &destination);


    /* Print shortest path */

    findAndPrintPath(n, source, destination,distance, nextHop);
}


void runDijkstra(int n, int cost[MAX][MAX],int source,
                 int distance[MAX],
                 int parent[MAX],
                 int showSteps)
{
    int visited[MAX];
    int i, j;
    int step = 1;

    /* Initialize */

    for (i = 0; i < n; i++)
    {
        distance[i] = cost[source][i];
        visited[i] = 0;

        if (i == source)
        {
            distance[i] = 0;
            parent[i] = -1;
        }
        else if (cost[source][i] < INF)
        {
            parent[i] = source;
        }
        else
        {
            parent[i] = -1;
        }
    }


    /* Print short Dijkstra steps */

    if (showSteps)
    {
        printf(" -------------------------   DIJKSTRA STEPS FOR NODE %d  ----------------------\n\n", source);

        printf("%-8s %-18s", "Step", "Selected Node");

        for (i = 0; i < n; i++)
        {
            printf("%6d", i);
        }

        printf("\n");

        printf("------------------------------------------------------------\n");
    }


    /* Dijkstra Algorithm */

    for (i = 0; i < n; i++)
    {
        int min = INF;
        int selected = -1;

        /* Find minimum distance node */

        for (j = 0; j < n; j++)
        {
            if (visited[j] == 0 &&
                distance[j] < min)
            {
                min = distance[j];
                selected = j;
            }
        }

        if (selected == -1)
            break;

        visited[selected] = 1;


        /* Update distances */

        for (j = 0; j < n; j++)
        {
            if (visited[j] == 0 &&
                cost[selected][j] < INF &&
                distance[selected] < INF)
            {
                int newDistance =
                    distance[selected] + cost[selected][j];

                if (newDistance < distance[j])
                {
                    distance[j] = newDistance;
                    parent[j] = selected;
                }
            }
        }


        /* Print simple step */

        if (showSteps)
        {
            printf("%-8d %-18d",
                   step, selected);

            for (j = 0; j < n; j++)
            {
                if (distance[j] >= INF)
                    printf("%6s", "INF");
                else
                    printf("%6d", distance[j]);
            }

            printf("\n");

            step++;
        }
    }
}


/* PRINT SHORTEST PATH FOR LINK STATE ROUTING
Uses PARENT array - FIXES PATH ERROR */

void printLinkStateShortestPath(int n, int cost[MAX][MAX],int source, int destination)
{
    int distance[MAX];
    int parent[MAX];

    int path[MAX];
    int pathCount = 0;

    int current;
    int i;

    /* Validate */

    if (source < 0 || source >= n ||
        destination < 0 || destination >= n)
    {
        printf("\nInvalid Source or Destination Node!\n");
        return;
    }


    /* Run Dijkstra */

    runDijkstra(n, cost, source,
                distance, parent, 0);

    printf(" ***************** SHORTEST PATH  *******************\n");



    /* Check path */

    if (distance[destination] >= INF)
    {
        printf("\nNo Path Exists from Node %d to Node %d\n",
               source, destination);
        return;
    }


    /* Trace path backwards */

    current = destination;

    while (current != -1)
    {
        path[pathCount] = current;
        pathCount++;

        current = parent[current];
    }

    printf("\nSource Node      : %d\n", source);
    printf("Destination Node : %d\n", destination);

    printf("\nShortest Path    : ");


    /* Print in correct order */

    for (i = pathCount - 1; i >= 0; i--)
    {
        printf("%d", path[i]);

        if (i != 0)
            printf(" -> ");
    }


    printf("\nTotal Cost       : %d\n",
           distance[destination]);
}

void linkState(int n, int cost[MAX][MAX])
{
    int allDistance[MAX][MAX];
    int allNextHop[MAX][MAX];

    int sourceRouter;
    int i;

    int router;
    int source, destination;

    printf("\n============================================================\n");
    printf("             LINK STATE ROUTING ALGORITHM\n");
    printf("============================================================\n");

    /*
       Run Dijkstra for all routers internally
       Steps are NOT printed here */

    for (sourceRouter = 0;
         sourceRouter < n;
         sourceRouter++)
    {
        int distance[MAX];
        int parent[MAX];

        runDijkstra(n, cost, sourceRouter,
                    distance, parent, 0);


        /* Store distance and find next hop */

        for (i = 0; i < n; i++)
        {
            allDistance[sourceRouter][i] = distance[i];

            if (i == sourceRouter)
            {
                allNextHop[sourceRouter][i] = -1;
            }
            else if (parent[i] == -1)
            {
                allNextHop[sourceRouter][i] = -1;
            }
            else
            {
                int current = i;

                /* Move backwards to find first hop */

                while (parent[current] != sourceRouter &&
                       parent[current] != -1)
                {
                    current = parent[current];
                }

                if (parent[current] == sourceRouter)
                    allNextHop[sourceRouter][i] = current;
                else
                    allNextHop[sourceRouter][i] = -1;
            }
        }
    }

    /*PRINT FINAL GRAPH ROUTING TABLE */
    printGraphTable(n, allDistance);
    /*ASK FOR PARTICULAR NODE */

    printf("\nEnter the node whose routing table you want to display: ");
    scanf("%d", &router);

    if (router < 0 || router >= n)
    {
        printf("\nInvalid Node!\n");
        return;
    }


    /*SHOW DIJKSTRA STEPS ONLY FOR SELECTED NODE */

    {
        int tempDistance[MAX];
        int tempParent[MAX];

        runDijkstra(n, cost, router,
                    tempDistance, tempParent, 1);
    }

    printNodeTable(n, router,allDistance, allNextHop);

    printf("\nEnter source node to find shortest path: ");
    scanf("%d", &source);

    printf("Enter destination node: ");
    scanf("%d", &destination);

    printLinkStateShortestPath(n, cost,
                               source, destination);
}


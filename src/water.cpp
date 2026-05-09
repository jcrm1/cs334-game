//given place, amount, flow
//calculate radial bfs for initial flow, location
//return initial water bottom mesh

#include <stdlib.h>
#include <stdio.h>
#include "Vertex.hpp"
#include "Water.hpp"
// #define TERRAIN_WIDTH 100
// #define MESHSIZE 10000

// float landMesh[MESHSIZE][3]; //assumed global??????
// int indices[(TERRAIN_WIDTH - 1) * (TERRAIN_WIDTH - 1) * 2][3]; // ditto
// int place = 5*3; //index, tentatively here
// int amount = 91; //not power of two (maybe float in future), tentatively here
// int flow = 3; //vector containing rate, tentatively here

//use triangle indices, not points, for ease and speed
//if in 

static int insertSort(int triangle, int** waterBottom, int amount, int ** validPoints,
    int * pointCount, int stopIndex, unsigned int (*indices)[3], Vertex (*landMesh)){
    //find lowest point in triangle
    //for each valid triangle in mesh
    //  if same triangle, break
    //  compare with all points in triangle
    //  check if point is less than any point
    //  check if point is same
    //    check second lowest, etc.
    //for each point in triangle
    //  for each valid point in validPoints
    //    compare

    // int lowestPoint = indices[triangle][0];
    // if (landMesh[indices[triangle][1]].position[1] < landMesh[lowestPoint].position[1]){
    //     lowestPoint = indices[triangle][1];
    // }
    // if (landMesh[indices[triangle][2]].position[1] < landMesh[lowestPoint].position[1]){
    //     lowestPoint = indices[triangle][2];
    // }
    int temp;
    int temp2;
    // int success = 0;
    // for (int j = 0; j < amount && j < stopIndex; j++){
    //     if ((*waterBottom)[j] == triangle) return 0;
    //     for (int i = 0; i < 2; i++){
    //         if (landMesh[lowestPoint].position[1] < (landMesh[indices[j][0]]).position[1] && 
    //             landMesh[lowestPoint].position[1] < (landMesh[indices[j][1]]).position[1] &&
    //             landMesh[lowestPoint].position[1] < (landMesh[indices[j][2]]).position[1]){
    //             temp = (*waterBottom)[j];
    //             (*waterBottom)[j] = triangle;
    //             printf("Inserting triangle %d at index %d\n", triangle, j);
    //             success++;
    //             while ((j++) < amount && (j) < stopIndex + 1){
    //                 temp2 = ((*waterBottom)[j]);
    //                 (*waterBottom)[j] = temp;
    //                 temp = temp2;
    //             }
    //             break;
    //         }
    //         if (landMesh[lowestPoint].position[1] != (landMesh[indices[j][0]]).position[1] || 
    //             landMesh[lowestPoint].position[1] != landMesh[indices[j][1]].position[1] ||
    //             landMesh[lowestPoint].position[1] != landMesh[indices[j][2]].position[1]) continue;
    //         if (indices[triangle][0] != lowestPoint && 
    //             (landMesh[indices[triangle][0]].position[1] < landMesh[indices[triangle][1]].position[1] || 
    //             landMesh[indices[triangle][0]].position[1] < landMesh[indices[triangle][2]].position[1])  ){
    //                 lowestPoint = indices[triangle][0];
    //         }
    //         else if (indices[triangle][1] != lowestPoint && 
    //             (landMesh[indices[triangle][1]].position[1] < landMesh[indices[triangle][0]].position[1] || 
    //             landMesh[indices[triangle][1]].position[1] < landMesh[indices[triangle][2]].position[1])  ){
    //                 lowestPoint = indices[triangle][1];
    //         }
    //         else{
    //                 lowestPoint = indices[triangle][2];
    //         }
    //         if ((landMesh[lowestPoint].position[1] < landMesh[indices[j][0]].position[1]) + 
    //             (landMesh[lowestPoint].position[1] < landMesh[indices[j][1]].position[1]) +
    //             (landMesh[lowestPoint].position[1] < landMesh[indices[j][2]].position[1]) == 2){ 
    //             temp = (*waterBottom)[j];
    //             (*waterBottom)[j] = triangle;
    //             success++;
    //             while ((j++) < amount && (j) < stopIndex + 1){
    //                 temp2 = ((*waterBottom)[j]);
    //                 (*waterBottom)[j] = temp;
    //                 temp = temp2;
    //             } 
    //             break;
    //         }
    //         lowestPoint = indices[triangle][0];
    //         if (landMesh[indices[triangle][1]].position[1] > landMesh[lowestPoint].position[1]){
    //             lowestPoint = indices[triangle][1];
    //         }
    //         if (landMesh[indices[triangle][2]].position[1] > landMesh[lowestPoint].position[1]){
    //             lowestPoint = indices[triangle][2];
    //         }
    //         if (landMesh[lowestPoint].position[1] != landMesh[indices[j][0]].position[1] || 
    //             landMesh[lowestPoint].position[1] != landMesh[indices[j][1]].position[1] ||
    //             landMesh[lowestPoint].position[1] != landMesh[indices[j][2]].position[1]) continue;
    //         if (landMesh[lowestPoint].position[1] < landMesh[indices[j][0]].position[1] || 
    //             landMesh[lowestPoint].position[1] < landMesh[indices[j][1]].position[1] ||
    //             landMesh[lowestPoint].position[1] < landMesh[indices[j][2]].position[1]){
    //             temp = (*waterBottom)[j];
    //             (*waterBottom)[j] = triangle;
    //             success++;
    //             while ((j++) < amount && (j) < stopIndex + 1){
    //                 temp2 = ((*waterBottom)[j]);
    //                 (*waterBottom)[j] = temp;
    //                 temp = temp2;
    //             }
    //             break;
    //         }
    //     }
    // }

    for (int j = 0; j < amount && j < stopIndex; j++){
        if ((*waterBottom)[j] == triangle){
            // printf("Triangle %d already exists in water bottom\n", triangle); 
            return 0;}
    }
    if (stopIndex >= amount) return 0;
    (*waterBottom)[stopIndex] = triangle;
    // printf("Adding triangle %d at index %d\n", triangle, stopIndex);
    
    for (int i = 0; i < 3; i++){
        for (int j = 0; j  <= *pointCount; j++){
            if (j >= amount){
                break;
            }
            // printf("j: %d, pointCount: %d\n", j, *pointCount);
            // printf("Comparing point %d\n", indices[triangle][i]);
            // 
            if (j < *pointCount && indices[triangle][i] == (*validPoints)[j]){
                // printf("Point %d is already valid, breaking\n", indices[triangle][i]);
                break;
            }
            if (j < *pointCount){
                // printf("with point: %d\n", (*validPoints)[j]);
                // printf("Comparing point %lf\n", (landMesh[indices[triangle][i]]).position[1]);
                // printf("with point: %lf\n", (landMesh[(*validPoints)[j]]).position[1]);
            }
            if (j < *pointCount && (landMesh[indices[triangle][i]]).position[1] < (landMesh[(*validPoints)[j]]).position[1]){
                // printf("Inserting point %d at index %d\n", indices[triangle][i], j);
                temp = (*validPoints)[j];
                // printf("Temp: %d\n", temp);
                (*validPoints)[j] = indices[triangle][i];
                // printf("Inserted valid point: %d\n", (*validPoints)[j]);
                (*pointCount)++;
                // printf("count incremented, current valid points: \n");
                // printf("amount: %d, stopIndex: %d\n", amount, stopIndex);
                while ((j++) < amount && (j) < (*pointCount) + 1){
                    // printf("Shifting point at index %d to index %d\n", j, j+1);
                    temp2 = (*validPoints)[j];
                    (*validPoints)[j] = temp;
                    temp = temp2;
                }
                // printf("Point inserted, breaking\n");
                break;
            }
            else if (j == *pointCount){
                // printf("Adding point %d at index %d\n", indices[triangle][i], j);
                (*validPoints)[j] = indices[triangle][i];
                // printf("Inserted odd valid point: %d\n", (*validPoints)[j]);
                // printf("Current valid points: ");
                (*pointCount)++;
                break;
            }
            else {
                // printf("j: %d\n", j);
            }
        }
    }
    return  1;
}

static void addTriangles(int ** mesh, int ** validPoints, int currentPoint, int * validCount, int * triCount, int amount, 
    int indexQuantity, int pointQuantity, unsigned int (*indices)[3], Vertex (*landMesh), int * includeTriangles){
    //for each triangle
    //if point is in it
    //  insert sort the triangle
    //  change counters appropriately
    // printf("tricount: %d", *triCount);
    int width = 1024;
    int indTriDim = 2*width-2;
    int currentX = currentPoint % width;
    int currentY = currentPoint / width;
    includeTriangles[0] = (currentY-1)*indTriDim+currentX*2+1;
    includeTriangles[1] = (currentY-1)*indTriDim+currentX*2;
    includeTriangles[2] = (currentY-1)*indTriDim+currentX*2-2;
    includeTriangles[3] = currentY*indTriDim+currentX*2+1;
    includeTriangles[4] = currentY*indTriDim+currentX*2;
    includeTriangles[5] = currentY*indTriDim+currentX*2-1;

    for (int i = 0; i < 6; i++){
        if (includeTriangles[i] < 0 || includeTriangles[i] >= indexQuantity){
            // printf("Triangle index %d is out of bounds, skipping\n", includeTriangles[i]);
            // printf("currentPoint: %d\n", currentPoint);
            continue;
        }
        for (int j = 0; j < 3; j++){
            if (indices[includeTriangles[i]][j] == currentPoint){
                // printf("Adding triangle %d\n", includeTriangles[i]);
                *triCount += insertSort(includeTriangles[i], mesh, amount, validPoints, validCount, *triCount, indices, landMesh);
                // printf("Current triCount: %d\n", *triCount);
            }
        }
    }
    // printf("Added triangles for point %d, current triCount: %d\n", currentPoint, *triCount);
}

// int addNeighbors(float origen[3], int ignore)


/*Creates bottom water mesh for calculations
Source: index of source point
Amount: how many triangles included originally
Flow: rate of water increase

Returns address of an array of triangles
*/
void createBottomMesh(int **waterBottom, int source, int amount, Vertex * landMesh, unsigned int (*indices)[3],
    int indexQuantity, int pointQuantity){
    // int (*waterBottom) = (int*) malloc(amount * sizeof(int));
    int (*validPoints) = (int*) malloc(amount * sizeof(int));
    int (*invalidPoints) = (int *) malloc(amount * sizeof(int));
    int invalidCount = 0;
    int validCount = 0;
    int triCount = 0;
    validPoints[0] = source;
    validCount++;

    int (*includeTriangles) = (int *) malloc(6 * sizeof(int));

    while (triCount < amount){
        //Get first point in validPoints (1)
        //If invalid, check next point (goto 2) (2)
        //Add surrounding triangles and indices to arrays (3)
        //increment validCount, triCount, invalidCount
        //add current point to invalidPoints
        if (triCount >200){
            // printf("triCount: %d\n", triCount);
            // printf("latest triangle: %d\n", (*waterBottom)[triCount-1]);
            // printf("current point: %d\n", validPoints[0]);
        }   

        int currentPoint = validPoints[0];
        // printf("Current point: %d\n", currentPoint);
        for (int i = 0; i < invalidCount; i++){
            if (currentPoint == invalidPoints[i]){
                currentPoint = validPoints[i+1];
                if (i+1 >= validCount){
                    // printf("No more valid points, stopping, at i: %d\n", i);
                    // printf("invalidCount: %d\n, triCount: %d\n", invalidCount, triCount);
                }
                // printf("Point is invalid, moving to next point: %d\n", currentPoint);
            }
        }
        // printf("Processing point %d\n", currentPoint);
        // if (0 == 0){
        //     printtriangles(validPoints, validCount);
        // }
        addTriangles(waterBottom, &validPoints, currentPoint, &validCount, &triCount, 
            amount, indexQuantity, pointQuantity, indices, landMesh, includeTriangles);
        //first find smallest point from triangle?
        invalidPoints[invalidCount] = currentPoint;
        invalidCount++;
    }
    // return waterBottom;
}

void printtriangles(int * waterBottom, int amount){
    printf("Water bottom triangles: \n[");
    for (int i = 0; i < amount; i++){
        printf("%d, ", waterBottom[i]);
    }
    printf("]\n");
}

//pre-calculate path until rest or end of map 
//return hashable, tiered, invisible mesh with spots for speed, acceleration, generated in steps

//given edges, current volume, turbulence (vector)
//limitations volumetric calculation
//return top mesh(es) 


//point-mesh generation (difficult)


// //pre-sort indices, maybe not

// static int insertSort(int indicesCopy[3], int ignore, float** waterBottom[3], 
//     int amount, int startIndex, int stopIndex){
//     int success = 0;
//         for (int i = 0; i < 3; i++){ 
//         float temp[3] = {0,0,0};
//         float temp2[3] = {0,0,0};
//         int k = 0;
//         if (currentIndex = ignore){
//             continue;
//         }
//         for (int j = startIndex; j < 1u << amount && j < stopIndex; j++){
//             if (landMesh[indicesCopy[0]][1] < (*waterBottom)[j][1]){ //compare height
//                 k = j;
//                 temp = (*waterBottom)[k];
//                 (*waterBottom)[k] = landMesh[indicesCopy[0]];
//                 success++;
//                 while ((k++) < 1u << amount && (k) < stopIndex + 1){
//                     temp2 = ((*waterBottom)[k]);
//                     (*waterBottom)[k] = temp;
//                 }
//             }

//         }
//     }
//     return success;
// }

// float* bottomMesh(float source, int amount, int flow){
//     //pre-alloc float[amount][3]
//     float* waterBottom = ((float*) calloc((1u << amount) * 3, sizeof(float[3])));
//     int startIndex = 0;
//     int index = 0;

//     //change algorithm to insertion sorting
//     for (int j = 0; j < MESHSIZE - 1; j++){ //replace 1000 with MESHSIZE
//         if (indices[j][0] = source){
//             insertSort(indices[j], 0, &waterBottom, amount, startIndex, j);
//         }
//         else if (indices[j][1] = source){
//             insertSort(indices[j], 1, &waterBottom, amount, startIndex, j);
//         }
//         else if (indices[j][2] = source){
//             insertSort(indices[j], 2, &waterBottom, amount, startIndex, j);
//         }
//     }
//     //search indices for source (pre-sorting gives a micro-improvement), 
//     //theoretically returns infinitely many triangles
//     //complexity: O(n), n is length of list
//     //pre-sorting can reduce it to O(lg(n))
//     int counter = 0;
//     while (amount > counter){
//         f
//     }
    
// }

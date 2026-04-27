//given place, amount, flow
//calculate radial bfs for initial flow, location
//return initial water bottom mesh

#include <stdlib.h>
#define TERRAIN_WIDTH 100
#define MESHSIZE 10000

// float landMesh[MESHSIZE][3]; //assumed global??????
// int indices[(TERRAIN_WIDTH - 1) * (TERRAIN_WIDTH - 1) * 2][3]; // ditto
// int place = 5*3; //index, tentatively here
// int amount = 91; //not power of two (maybe float in future), tentatively here
// int flow = 3; //vector containing rate, tentatively here

//use triangle indices, not points, for ease and speed
//if in 

static int insertSort(int triangle, int** waterBottom, int amount, int ** validPoints,
    int * pointCount, int stopIndex){
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

    int lowestPoint = indices[triangle][0];
    if (landMesh[indices[triangle][1]][1] < landMesh[lowestPoint][1]){
        lowestPoint = indices[triangle][1];
    }
    if (landMesh[indices[triangle][2]][1] < landMesh[lowestPoint][1]){
        lowestPoint = indices[triangle][2];
    }
    int temp;
    int temp2;
    int success = 0;
    for (int j = 0; j < amount && j < stopIndex; j++){
        if ((*waterBottom)[j] = triangle) return 0;
        for (int i = 0; i < 2; i++){
            if (landMesh[lowestPoint][1] < landMesh[indices[j][0]][1] && 
                landMesh[lowestPoint][1] < landMesh[indices[j][1]][1] &&
                landMesh[lowestPoint][1] < landMesh[indices[j][2]][1]){
                temp = (*waterBottom)[j];
                (*waterBottom)[j] = triangle;
                success++;
                while ((j++) < amount && (j) < stopIndex + 1){
                    temp2 = ((*waterBottom)[j]);
                    (*waterBottom)[j] = temp;
                    temp = temp2;
                }
                break;
            }
            if (landMesh[lowestPoint][1] != landMesh[indices[j][0]][1] || 
                landMesh[lowestPoint][1] != landMesh[indices[j][1]][1] ||
                landMesh[lowestPoint][1] != landMesh[indices[j][2]][1]) continue;
            if (indices[triangle][0] != lowestPoint && 
                (landMesh[indices[triangle][0]][1] < landMesh[indices[triangle][1]][1] || 
                landMesh[indices[triangle][0]][1] < landMesh[indices[triangle][2]][1])  ){
                    lowestPoint = indices[triangle][0];
            }
            else if (indices[triangle][1] != lowestPoint && 
                (landMesh[indices[triangle][1]][1] < landMesh[indices[triangle][0]][1] || 
                landMesh[indices[triangle][1]][1] < landMesh[indices[triangle][2]][1])  ){
                    lowestPoint = indices[triangle][1];
            }
            else{
                    lowestPoint = indices[triangle][2];
            }
            if ((landMesh[lowestPoint][1] < landMesh[indices[j][0]][1]) + 
                (landMesh[lowestPoint][1] < landMesh[indices[j][1]][1]) +
                (landMesh[lowestPoint][1] < landMesh[indices[j][2]][1]) == 2){ 
                temp = (*waterBottom)[j];
                (*waterBottom)[j] = triangle;
                success++;
                while ((j++) < amount && (j) < stopIndex + 1){
                    temp2 = ((*waterBottom)[j]);
                    (*waterBottom)[j] = temp;
                    temp = temp2;
                } 
                break;
            }
            lowestPoint = indices[triangle][0];
            if (landMesh[indices[triangle][1]][1] > landMesh[lowestPoint][1]){
                lowestPoint = indices[triangle][1];
            }
            if (landMesh[indices[triangle][2]][1] > landMesh[lowestPoint][1]){
                lowestPoint = indices[triangle][2];
            }
            if (landMesh[lowestPoint][1] != landMesh[indices[j][0]][1] || 
                landMesh[lowestPoint][1] != landMesh[indices[j][1]][1] ||
                landMesh[lowestPoint][1] != landMesh[indices[j][2]][1]) continue;
            if (landMesh[lowestPoint][1] < landMesh[indices[j][0]][1] || 
                landMesh[lowestPoint][1] < landMesh[indices[j][1]][1] ||
                landMesh[lowestPoint][1] < landMesh[indices[j][2]][1]){
                temp = (*waterBottom)[j];
                (*waterBottom)[j] = triangle;
                success++;
                while ((j++) < amount && (j) < stopIndex + 1){
                    temp2 = ((*waterBottom)[j]);
                    (*waterBottom)[j] = temp;
                    temp = temp2;
                }
                break;
            }
        }
    }
    for (int i = 0; i < 3; i++){
        for (int j = 0; j  < *pointCount; j++){
            if (j >= amount){
                break;
            }
            if (j < *pointCount || landMesh[indices[triangle][i]][1] < landMesh[(*validPoints)[j]][1]){
                temp = (*validPoints)[j];
                (*validPoints)[j] = indices[triangle][i];
                (*pointCount)++;
                while ((j++) < amount && (j) < stopIndex + 1){
                    temp2 = (*validPoints)[j];
                    (*validPoints)[j] = temp;
                    temp = temp2;
                }
                break;
            }
            else if (j == *pointCount){
                (*validPoints)[j] = indices[triangle][i];
                (*pointCount)++;
            }
        }
    }
    return success;
}

static void addTriangles(int ** mesh, int ** validPoints, int currentPoint, int * validCount, int * triCount, int amount){
    //for each triangle
    //if point is in it
    //  insert sort the triangle
    //  change counters appropriately
    for (int i = 0; i < (TERRAIN_WIDTH - 1) * (TERRAIN_WIDTH - 1) * 2; i++){
        for (int j = 0; j < 3; j++){
            if (indices[i][j] = currentPoint){
                *triCount += insertSort(i, mesh, amount, validPoints, validCount, *triCount);
            }
            //other cases here
        }
    }
}

// int addNeighbors(float origen[3], int ignore)


/*Creates bottom water mesh for calculations
Source: index of source point
Amount: how many triangles included originally
Flow: rate of water increase

Returns address of an array of triangles
*/
int ** createBottomMesh(int source, int amount, int flow){
    int (*waterBottom) = (int*) malloc(amount * sizeof(int));
    int (*validPoints) = (int*) malloc(amount * sizeof(int));
    int (*invalidPoints) = (int *) malloc(amount * sizeof(int));
    int invalidCount = 0;
    int validCount = 0;
    int triCount = 0;
    validPoints[0] = source;
    validCount++;

    while (triCount < amount){
        //Get first point in validPoints (1)
        //If invalid, check next point (goto 2) (2)
        //Add surrounding triangles and indices to arrays (3)
        //increment validCount, triCount, invalidCount
        //add current point to invalidPoints
        int currentPoint = validPoints[0];
        for (int i = 0; i < invalidCount; i++){
            if (currentPoint == invalidPoints[i]){
                currentPoint = validPoints[i+1];
            }
        }
        addTriangles(&waterBottom, &validPoints, currentPoint, &validCount, &triCount, amount);
        //first find smallest point from triangle
        invalidPoints[invalidCount] = currentPoint;
        invalidCount++;
    }
    return &waterBottom;
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

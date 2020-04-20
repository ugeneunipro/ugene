/**
* UGENE - Integrated Bioinformatics Tools.
* Copyright (C) 2008-2020 UniPro <ugene@unipro.ru>
* http://ugene.net
*
* This program is free software; you can redistribute it and/or
* modify it under the terms of the GNU General Public License
* as published by the Free Software Foundation; either version 2
* of the License, or (at your option) any later version.
*
* This program is distributed in the hope that it will be useful,
* but WITHOUT ANY WARRANTY; without even the implied warranty of
* MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE. See the
* GNU General Public License for more details.
*
* You should have received a copy of the GNU General Public License
* along with this program; if not, write to the Free Software
* Foundation, Inc., 51 Franklin Street, Fifth Floor, Boston,
* MA 02110-1301, USA.
*/

typedef int ScoreType;

// CUDA global constants
__constant__ int g_partSeqSize;
__constant__ int g_partsNumber;
__constant__ int g_overlapLength;
__constant__ int g_seqLibLength;
__constant__ int g_queryLength;
__constant__ int g_gapOpen;
__constant__ int g_gapExtension;
__constant__ int g_maxScore;
__constant__ int g_queryPartLength;
__constant__ char g_upSymbolDirectMatrix;
__constant__ char g_leftSymbolDirectMatrix;
__constant__ char g_diagSymbolDirectMatrix;
__constant__ char g_stopSymbolDirectMatrix;

//global function
__global__ void calculateMatrix(const char * seqLib, ScoreType* queryProfile,
                                ScoreType* g_HdataUp, ScoreType* g_HdataRec, ScoreType* g_HdataMax,
                                ScoreType* g_FdataUp,
                                ScoreType* g_directionsUp, ScoreType* g_directionsRec, ScoreType* g_directionsMax,
                                int queryStartPos, int * g_directionsMatrix, int * g_backtraceBegins)
{
    //registers
    int patternPos = threadIdx.x;
    int globalPatternPos =  queryStartPos + patternPos;

    int seqStartPos = blockIdx.x * (g_partSeqSize - g_overlapLength);
    int globalStartPos = blockIdx.x * (g_partSeqSize + 1);
    
    int seqPos = 0;
    int globalPos = 0;
    int diagNum = 0;

    ScoreType substScore = 0;
    ScoreType E = 0;
    ScoreType E_left = 0;
    ScoreType F = 0;
    ScoreType F_up = 0;
    ScoreType H = 0;
    ScoreType H_left = 0;
    ScoreType H_up = 0;
    ScoreType H_upleft = 0;
    ScoreType directionLeft = 0;
    ScoreType directionUp = 0;
    ScoreType directionUpLeft = 0;
    ScoreType direction = 0;

    //dynamic allocation shared memory
    extern __shared__ ScoreType shared_H[];
    ScoreType* shared_E = (ScoreType*)&shared_H[g_queryPartLength + 1];
    ScoreType* shared_direction = (ScoreType*)&shared_E[g_queryPartLength + 1];

    shared_H[patternPos] = 0;
    shared_E[patternPos] = 0;
    shared_direction[patternPos] = 0;

    __syncthreads();

    seqPos = seqStartPos - patternPos;
    globalPos = globalStartPos - patternPos;

    diagNum = g_partSeqSize + g_queryPartLength - 1;
    if (diagNum == 0) {
        return;
    }

    for (int iteration = 0; iteration < diagNum; iteration++) {
        //check boundaries
        bool isActual = seqPos < g_seqLibLength && seqPos >= seqStartPos && seqPos < seqStartPos + g_partSeqSize && globalPatternPos < g_queryLength;
        if (isActual) {
            substScore = queryProfile[seqLib[seqPos] * g_queryLength + globalPatternPos];

            H_left = shared_H[patternPos];
            E_left = shared_E[patternPos];
            directionLeft = shared_direction[patternPos];

            if (patternPos == 0) {

                H_left = g_HdataUp[globalPos];
                E_left = g_FdataUp[globalPos];
                directionLeft = g_directionsUp[globalPos];

                if (globalPos > 0) {
                    H_upleft = g_HdataUp[globalPos - 1];
                    directionUpLeft = g_directionsUp[globalPos - 1];

                } 
                if (queryStartPos == 0 || iteration == 0) {
                    directionUpLeft = seqPos;
                }   
            }
        }
        __syncthreads();

        if (isActual) {
            E = max(E_left - g_gapExtension, H_left - g_gapOpen);
            F = max(F_up - g_gapExtension, H_up - g_gapOpen);

            // Compute H
            H = max(0, E);
            H = max(H, F);
            H = max(H, H_upleft + substScore);

            //Collect best results
            if (g_HdataMax[globalPos] <= H_upleft + substScore) {
                g_HdataMax[globalPos] = H_upleft + substScore;
                g_directionsMax[globalPos] = directionUpLeft;

                if (NULL != g_directionsMatrix && NULL != g_backtraceBegins) {
                    g_backtraceBegins[globalPos * 2] = globalPatternPos;
                    g_backtraceBegins[globalPos * 2 + 1] = seqPos;
                }
            }

            //chose direction
            char directionForMatrix = g_stopSymbolDirectMatrix;

            if (H == 0) {
                direction = seqPos + 1;
            }
            else if (H == H_upleft + substScore) {
                direction = directionUpLeft;
                directionForMatrix = g_diagSymbolDirectMatrix;
            }
            else if (H == F) {
                direction = directionUp;
                directionForMatrix = g_leftSymbolDirectMatrix;
            }
            else {
                direction = directionLeft;
                directionForMatrix = g_upSymbolDirectMatrix;
            }                    
                
            shared_E[patternPos + 1] = E;
            shared_H[patternPos + 1] = H;
            shared_direction[patternPos + 1] = direction;

            H_upleft = H_left;
            H_up = H;
            F_up = F;

            directionUp = direction;
            directionUpLeft = directionLeft;

            if(0 != g_directionsMatrix) {
                g_directionsMatrix[g_seqLibLength * globalPatternPos + seqPos] = (int)directionForMatrix;
            }


            //if this last iteration then start prepare next
            if (patternPos == (g_queryPartLength - 1)) {
                g_HdataRec[globalPos] = H;
                g_FdataUp[globalPos] = E;
                g_directionsRec[globalPos]  = direction;
            }
        }
        __syncthreads();

        seqPos++;       // = seqStartPos + iteration - patternPos;
        globalPos++;    // = globalStartPos + iteration - patternPos;
    }
}

void calculateMatrix_wrap(int blockSize, int threadNum, const char * seqLib, ScoreType* queryProfile,                           
                          ScoreType* g_HdataUp, ScoreType* g_HdataRec, ScoreType* g_HdataMax,
                          ScoreType* g_FdataUp,
                          ScoreType* g_directionsUp, ScoreType* g_directionsRec, ScoreType* g_directionsMax,
                          int iteration, int * g_directionsMatrix, int * g_backtraceBegins)
{
    size_t sh_mem_size = sizeof(ScoreType) * (threadNum + 1) * 3;
    calculateMatrix<<<blockSize, threadNum, sh_mem_size>>>(seqLib,
        queryProfile, g_HdataUp, 
        g_HdataRec, g_HdataMax, g_FdataUp,
        g_directionsUp, g_directionsRec, g_directionsMax, iteration,
        g_directionsMatrix, g_backtraceBegins);
}

void setConstants(int partSeqSize,
                  int partsNumber,
                  int overlapLength,
                  int seqLibLength,
                  int queryLength,
                  int gapOpen,
                  int gapExtension,
                  int maxScore,
                  int queryPartLength,
                  char upSymbolDirectMatrix,
                  char leftSymbolDirectMatrix,
                  char diagSymbolDirectMatrix,
                  char stopSymbolDirectMatrix) {
    cudaMemcpyToSymbol(g_partSeqSize,               &partSeqSize,               sizeof(partSeqSize));
    cudaMemcpyToSymbol(g_partsNumber,               &partsNumber,               sizeof(partsNumber));
    cudaMemcpyToSymbol(g_overlapLength,             &overlapLength,             sizeof(overlapLength));
    cudaMemcpyToSymbol(g_seqLibLength,              &seqLibLength,              sizeof(seqLibLength));
    cudaMemcpyToSymbol(g_queryLength,               &queryLength,               sizeof(queryLength));
    cudaMemcpyToSymbol(g_gapOpen,                   &gapOpen,                   sizeof(gapOpen));
    cudaMemcpyToSymbol(g_gapExtension,              &gapExtension,              sizeof(gapExtension));
    cudaMemcpyToSymbol(g_maxScore,                  &maxScore,                  sizeof(maxScore));
    cudaMemcpyToSymbol(g_queryPartLength,           &queryPartLength,           sizeof(queryPartLength));
    cudaMemcpyToSymbol(g_upSymbolDirectMatrix,      &upSymbolDirectMatrix,      sizeof(upSymbolDirectMatrix));
    cudaMemcpyToSymbol(g_leftSymbolDirectMatrix,    &leftSymbolDirectMatrix,    sizeof(leftSymbolDirectMatrix));
    cudaMemcpyToSymbol(g_diagSymbolDirectMatrix,    &diagSymbolDirectMatrix,    sizeof(diagSymbolDirectMatrix));
    cudaMemcpyToSymbol(g_stopSymbolDirectMatrix,    &stopSymbolDirectMatrix,    sizeof(stopSymbolDirectMatrix));
}

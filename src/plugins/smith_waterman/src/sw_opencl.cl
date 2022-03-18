//#ifdef SW2_BUILD_WITH_CUDA

#define NULL 0

typedef int ScoreType;

// global function
__kernel void calculateMatrix(global const char* seqLib,
                              global ScoreType* queryProfile,
                              global ScoreType* g_HdataUp,
                              global ScoreType* g_HdataRec,
                              global ScoreType* g_HdataMax,
                              global ScoreType* g_FdataUp,
                              global ScoreType* g_directionsUp,
                              global ScoreType* g_directionsRec,
                              global ScoreType* g_directionsMax,
                              global int* g_directionsMatrix,
                              global int* g_backtraceBegins,
                              __const int queryStartPos,
                              __const int partSeqSize,
                              __const int partsNumber,
                              __const int overlapLength,
                              __const int seqLibLength,
                              __const int queryLength,
                              __const int gapOpen,
                              __const int gapExtension,
                              __const int queryPartLength,
                              __const char leftSymbolDirectMatrix,
                              __const char diagSymbolDirectMatrix,
                              __const char upSymbolDirectMatrix,
                              __const char stopSymbolDirectMatrix,
                              __local ScoreType* shared_H,
                              __local ScoreType* shared_E,
                              __local ScoreType* shared_direction) {
    // registers
    int patternPos = get_local_id(0);  // threadIdx.x;
    int globalPatternPos = queryStartPos + patternPos;

    // int seqStartPos = blockIdx.x * (partSeqSize - overlapLength);
    int seqStartPos = get_group_id(0) * (partSeqSize - overlapLength);

    //    int globalStartPos = blockIdx.x * (partSeqSize + 1);
    int globalStartPos = get_group_id(0) * (partSeqSize + 1);

    int seqPos = 0, globalPos = 0, diagNum = 0;
    ScoreType substScore = 0;
    ScoreType E = 0, E_left = 0, F = 0, F_up = 0, H = 0,
              H_left = 0, H_up = 0, H_upleft = 0,
              directionLeft = 0, directionUp = 0,
              directionUpLeft = 0, direction = 0,
              maxScore = 0;

    shared_H[patternPos] = 0;
    shared_E[patternPos] = 0;
    shared_direction[patternPos] = 0;

    barrier(CLK_LOCAL_MEM_FENCE);

    seqPos = seqStartPos - patternPos;
    globalPos = globalStartPos - patternPos;

    diagNum = (partSeqSize + queryPartLength - 1);
    for (int iteration = 0; iteration < diagNum; iteration++) {
        // check boundaries
        bool isActual = seqPos < seqLibLength && seqPos >= seqStartPos && seqPos < seqStartPos + partSeqSize && globalPatternPos < queryLength;
        if (isActual) {
            substScore = queryProfile[seqLib[seqPos] * queryLength + globalPatternPos];

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
        barrier(CLK_LOCAL_MEM_FENCE);

        if (isActual) {
            E = max(E_left - gapExtension, H_left - gapOpen);
            F = max(F_up - gapExtension, H_up - gapOpen);

            // Compute H
            H = max(0, E);
            H = max(H, F);
            H = max(H, H_upleft + substScore);

            // Collect best result
            if (g_HdataMax[globalPos] <= H_upleft + substScore) {
                g_HdataMax[globalPos] = H_upleft + substScore;
                g_directionsMax[globalPos] = directionUpLeft;

                if (NULL != g_directionsMatrix && NULL != g_backtraceBegins) {
                    g_backtraceBegins[globalPos * 2] = globalPatternPos;
                    g_backtraceBegins[globalPos * 2 + 1] = seqPos;
                }
            }

            // chose direction
            char directionForMatrix = stopSymbolDirectMatrix;

            if (H == 0) {
                direction = seqPos + 1;
            } else if (H == H_upleft + substScore) {
                direction = directionUpLeft;
                directionForMatrix = diagSymbolDirectMatrix;
            } else if (H == F) {
                direction = directionUp;
                directionForMatrix = leftSymbolDirectMatrix;
            } else {
                direction = directionLeft;
                directionForMatrix = upSymbolDirectMatrix;
            }

            shared_E[patternPos + 1] = E;
            shared_H[patternPos + 1] = H;
            shared_direction[patternPos + 1] = direction;

            H_upleft = H_left;
            H_up = H;
            F_up = F;

            directionUp = direction;
            directionUpLeft = directionLeft;

            if (NULL != g_directionsMatrix) {
                g_directionsMatrix[seqLibLength * globalPatternPos + seqPos] = (int)directionForMatrix;
            }

            // if this last iteration then start prepare next
            if (patternPos == (queryPartLength - 1)) {
                g_HdataRec[globalPos] = H;
                g_FdataUp[globalPos] = E;
                g_directionsRec[globalPos] = direction;
            }
        }
        barrier(CLK_LOCAL_MEM_FENCE);

        seqPos++;  // = seqStartPos + iteration - patternPos;
        globalPos++;  // = globalStartPos + iteration - patternPos;
    }
}
#pragma once

// Statistics for solver performance tracking
struct SolverStats
{
  int nodesExpanded;
  int nodesGenerated;
  int peakOpenSize;
  int peakClosedSize;
  long long elapsedMs;
  int solutionLength;
  int solutionCost;

  SolverStats()
      : nodesExpanded(0), nodesGenerated(0), peakOpenSize(0), peakClosedSize(0),
        elapsedMs(0), solutionLength(0), solutionCost(0) {}

  void reset()
  {
    nodesExpanded = 0;
    nodesGenerated = 0;
    peakOpenSize = 0;
    peakClosedSize = 0;
    elapsedMs = 0;
    solutionLength = 0;
    solutionCost = 0;
  }
};
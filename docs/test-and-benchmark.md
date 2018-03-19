
# Test and Benchmark

Basic algorithms that eventually solve a level are our base: BFS, DFS, astar, greedy.

On the way of developing a well-performing algorithm, automated benchmarking resulting in an easy to evalute result seems to be desireable. Possible interesting parameters are:
- t (time an algorithm ran)
- #nodes (number of nodes visited)
- solvable (could the level be solved)
- ?name (shortest path found)

Possibly of interest are the evaluation of performance across 1..n levels and 1..m heuristics with the following testnames, interesting parameters and required elements:

| Level | Heuristic | Name  | interesting parameters | required elements |
| :----: |:---------:| ------| ---------------------:| ---------:|
| 1      | m | benchmarkLevel | t | googletest | 
| n      | 1 | benchmarkHeuristic | t, #nodes, solvable | googletest, logging result | 
| 1      | m | benchmarkAll | #nodes **?** | googletest | googletest, logging result |  
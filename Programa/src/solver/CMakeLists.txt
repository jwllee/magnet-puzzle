add_library(Solver puzzle.c ../watcher/watcher.c ./utils.c)
add_library(SolverUtils utils.c puzzle.c)
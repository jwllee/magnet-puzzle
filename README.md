# Magnet puzzle
This problem requires the placement of magnets so that the positive and negative total charge constraints are fulfilled at all rows and columns. Since they are magnets, adjacent magnets need to have opposite charges. 

### How to run the program?
Using one of the puzzles in the directory ```Program/tests```, go to the directory ```Program``` and run 
```
  make
  ./solver [test.txt]
```
where ```[test.txt]``` is the file path to the selected puzzle.

This will open a graphical window which solves the puzzle automatically with visualization. (Note that I did not write the code for the graphical part.)

### Solution
The crux of the solution is to use backtracking to solve the CSP (Constraint Satisfaction Problem). Optimizations such as pruning and heuristics have also been incorporated to achieve significantly better performances.

### Notes
- There is a report in the directory ```Informe``` which contains some analysis of the solution.

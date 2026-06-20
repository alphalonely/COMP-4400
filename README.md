# COMP-4400
Name: Duc Nguyen
COMP 4400 - Assignment 5
- I run the program by these commands:
gcc -o generator generator.c
gcc -o display display.c
./generator words1.txt binpattern1.txt
swipl -s step2.pl
./display binpattern1.txt selectedwords.txt
(for 2nd puzzle, just replace words1 into words2, binpattern1 into binpattern2)

- Explanation:
+ generator.c reads words.txt and binpattern.txt -> generates step2.pl
+ display.c reads selectedwords.txt and binpattern.txt -> generates step2.csv
+ step2.pl is the prolog program that solves the crossword puzzle
+ selectedwords.txt is the output from step2.pl containing selected words with their positions
+ step2.csv is the final crossword grid in csv format

- selectedwords.txt:
+ format: word, row, column, direction
+ word is the selected word
+ row is the starting row position (0-index)
+ column is the starting column position (0-index)
+ direction has 'h' for horizontal and 'v' for vertical
example: duc,0,0,h

- Some predicates in prolog program (shortly explanation as it has been explained fully in assignment4's readme file):
+ select(X, List, Remain): select element X from List, return Remain
+ get_char(N, List, C): get character at position N in List
+ check_length(Word, Len): verify word has length Len
+ select_word(Word, List, Len, Remain): select word of specific length
+ check_intersection(HWord, HPos, VWord, VPos): verify words share same letter at intersection
+ crossword(...): main predicate that solves the puzzle by selecting words and checking intersections

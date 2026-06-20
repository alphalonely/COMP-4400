#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#define MAX 100

typedef struct {
    int row, col, len;
    char direction;
} Slot;
typedef struct {
    int hid, vid, hpos, vpos;
} Intersection;

int grid[MAX][MAX];
int num_rows, num_cols;
Slot hwords[MAX];
Slot vwords[MAX];
int hcount, vcount;
Intersection inter[MAX*MAX];
int intercount;

void read_input (char *filename) {
    FILE *file = fopen(filename, "r");
    num_cols=num_rows=0;
    char line[MAX+2];
    while (fgets(line, sizeof(line), file)) {
        int col = 0;
        for (int i=0; line[i] && line[i] != '\n'; i++) {
            if (line[i] == '0'|| line[i] == '1') {
                grid[num_rows][col] = line[i] - '0';
                col++;
            }
        }
        if (col > 0) {
            if (col > num_cols) num_cols = col;
            num_rows++;
        }
    }
    fclose(file);
}

void horizontal_words() {
    for (int row = 0; row < num_rows; row++) {
        int start_col = -1;
        for (int col = 0; col <= num_cols; col++) {
            if (col < num_cols && grid[row][col] == 1) {
                if (start_col == -1) start_col = col;
            } else {
                if (start_col != -1 && col - start_col >= 2) {
                    hwords[hcount].row = row;
                    hwords[hcount].col = start_col;
                    hwords[hcount].len = col - start_col;
                    hwords[hcount].direction = 'h';
                    hcount++;
                }
                start_col = -1;
            }
        }
    }
}

void vertical_words() {
    for (int col = 0; col < num_cols; col++) {
        int start_row = -1;
        for (int row = 0; row <= num_rows; row++) {
            if (row < num_rows && grid[row][col] == 1) {
                if (start_row == -1) start_row = row;
            } else {
                if (start_row != -1 && row - start_row >= 2) {
                    vwords[vcount].row = start_row;
                    vwords[vcount].col = col;
                    vwords[vcount].len = row - start_row;
                    vwords[vcount].direction = 'v';
                    vcount++;
                }
                start_row = -1;
            }
        }
    }
}

void intersections() {
    for (int hidx = 0; hidx < hcount; hidx++) {
        for (int vidx = 0; vidx < vcount; vidx++) {
            int hrow = hwords[hidx].row;
            int hcol_start = hwords[hidx].col;
            int hcol_end = hcol_start + hwords[hidx].len - 1;

            int vcol = vwords[vidx].col;
            int vrow_start = vwords[vidx].row;
            int vrow_end = vrow_start + vwords[vidx].len - 1;

            if (vcol >= hcol_start && vcol <= hcol_end && hrow >= vrow_start && hrow <= vrow_end) {
                inter[intercount].hid = hidx;
                inter[intercount].hpos = vcol - hcol_start;
                inter[intercount].vid = vidx;
                inter[intercount].vpos = hrow - vrow_start;
                intercount++;
            }
        }
    }
}

void generate_prolog (char *words_filename) {
    FILE *prolog_file = fopen("step2.pl", "w");
    //write the same to step1.pl
    fprintf(prolog_file, "select(X, [X|Remain], Remain).\n");
    fprintf(prolog_file, "select(X, [Y|List], [Y|Remain]) :- select(X, List, Remain).\n\n");
    fprintf(prolog_file, "get_char(0, [H|_], H).\n");
    fprintf(prolog_file, "get_char(N, [_|T], C) :- N > 0, M is N-1, get_char(M, T, C).\n\n");
    fprintf(prolog_file, "check_length(Word, Len) :- atom_chars(Word, Chars), length(Chars, Len).\n\n");
    fprintf(prolog_file, "select_word(Word, List, Len, Remain) :- select(Word, List, Remain), check_length(Word, Len).\n\n");
    fprintf(prolog_file, "check_intersection(HWord, HPos, VWord, VPos) :-\n");
    fprintf(prolog_file, "    atom_chars(HWord, HChars), atom_chars(VWord, VChars),\n");
    fprintf(prolog_file, "    get_char(HPos, HChars, Chars), get_char(VPos, VChars, Chars).\n\n");
    //read words from input files + create crossword predicate
    fprintf(prolog_file, "read_words(L) :- open('%s', read, S), read_lines(S, L), close(S).\n", words_filename);
    fprintf(prolog_file, "read_lines(S, []) :- at_end_of_stream(S), !.\n");
    fprintf(prolog_file, "read_lines(S, [W|R]) :- \\+ at_end_of_stream(S), read_line_to_string(S, Line), atom_string(W, Line), read_lines(S, R).\n\n");
    fprintf(prolog_file, "crossword(");
    for (int i = 0; i < hcount; i++) {
        fprintf(prolog_file, "H%d", i+1);
        if (i < hcount-1 || vcount > 0) fprintf(prolog_file, ", ");
    }
    for (int i = 0; i < vcount; i++) {
        fprintf(prolog_file, "V%d", i+1);
        if (i < vcount-1) fprintf(prolog_file, ", ");
    }
    fprintf(prolog_file, ") :- read_words(List),\n");
    //select horizontal words
    for (int i = 0; i < hcount; i++) {
        if (i == 0) {
            fprintf(prolog_file, "    select_word(H1, List, %d, Remain1),\n", hwords[i].len);
        } else {
            fprintf(prolog_file, "    select_word(H%d, Remain%d, %d, Remain%d),\n", i+1, i, hwords[i].len, i+1);
        }
    }
    //select vertical words
    for (int i = 0; i < vcount; i++) {
        if (i < vcount-1) {
            fprintf(prolog_file, "    select_word(V%d, Remain%d, %d, Remain%d)%s\n", i+1, hcount+i, vwords[i].len, hcount+i+1, intercount > 0 ? "," : ".");
        } else {
            fprintf(prolog_file, "    select_word(V%d, Remain%d, %d, _)%s\n", i+1, hcount+i, vwords[i].len, intercount > 0 ? "," : ".");
        }
    }
    //check intersections
    for (int i = 0; i < intercount; i++) {
        fprintf(prolog_file, "    check_intersection(H%d, %d, V%d, %d)%s\n", inter[i].hid+1, inter[i].hpos, inter[i].vid+1, inter[i].vpos, i < intercount-1 ? "," : ".");
    }
    //write solution into selectedwords.txt
    fprintf(prolog_file, "\nwrite_solution :- crossword(");
    for (int i = 0; i < hcount; i++) {
        fprintf(prolog_file, "H%d", i+1);
        if (i < hcount-1 || vcount > 0) fprintf(prolog_file, ", ");
    }
    for (int i = 0; i < vcount; i++) {
        fprintf(prolog_file, "V%d", i+1);
        if (i < vcount-1) fprintf(prolog_file, ", ");
    }
    fprintf(prolog_file, "), open('selectedwords.txt', write, S),\n");
    //write horizontal words
    for (int i = 0; i < hcount; i++) {
        fprintf(prolog_file, "    write(S, H%d), write(S, ',%d,%d,h'), nl(S),\n", i+1, hwords[i].row, hwords[i].col);
    }
    //write vertical words
    for (int i = 0; i < vcount; i++) {
        fprintf(prolog_file, "    write(S, V%d), write(S, ',%d,%d,v'), nl(S)%s\n", i+1, vwords[i].row, vwords[i].col, i < vcount-1 ? "," : ".");
    }
    fprintf(prolog_file, "    close(S).\n\n");
    fprintf(prolog_file, ":- write_solution, halt.\n");
    fclose(prolog_file);
}

int main(int argc, char *argv[]) {
    if (argc != 3) return 1;
    read_input(argv[2]);
    horizontal_words();
    vertical_words();
    intersections();
    generate_prolog(argv[1]);
    return 0;
}
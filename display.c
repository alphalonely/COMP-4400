#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <ctype.h>
#define MAX 100

char grid[MAX][MAX];
int pattern[MAX][MAX];
int num_rows, num_cols;

void read_input (char *filename) {
    FILE *file = fopen(filename, "r");
    num_cols=num_rows=0;
    char line[MAX+2];
    while (fgets(line, sizeof(line), file)) {
        int col = 0;
        for (int i=0; line[i] && line[i] != '\n'; i++) {
            if (line[i] == '0'|| line[i] == '1') {
                pattern[num_rows][col] = line[i] - '0';
                col++;
            }
        }
        if (col > 0) {
            if (col > num_cols) num_cols = col;
            num_rows++;
        }
    }
    fclose(file);
    
    for (int row = 0; row < num_rows; row++) {
        for (int col = 0; col < num_cols; col++) {
            grid[row][col] = ' ';
        }
    }
}

void read_and_place_words(char *filename) {
    FILE *file = fopen(filename, "r");
    char line[256];
    char word[50];
    int row, col;
    char direction;
    while (fgets(line, sizeof(line), file)) {
        if (sscanf(line, "%[^,],%d,%d,%c", word, &row, &col, &direction) == 4) {
            int len = strlen(word);
            for (int i = 0; i < len; i++) {
                if (direction == 'h') {
                    grid[row][col + i] = toupper(word[i]);
                } else {
                    grid[row + i][col] = toupper(word[i]);
                }
            }
        }
    }
    fclose(file);
}

void write_csv(char *filename) {
    FILE *file = fopen(filename, "w");
    for (int row = 0; row < num_rows; row++) {
        for (int col = 0; col < num_cols; col++) {
            if (pattern[row][col] == 0) {
                fprintf(file, " ");
            } else {
                fprintf(file, "%c", grid[row][col]);
            }
            if (col < num_cols - 1) {
                fprintf(file, ",");
            }
        }
        fprintf(file, "\n");
    }
    fclose(file);
}

int main(int argc, char *argv[]) {
    char *pattern_file = argc >= 2 ? argv[1] : "binpattern.txt";
    char *words_file = argc >= 3 ? argv[2] : "selectedwords.txt";
    char *output_file = "step2.csv";
    read_input(pattern_file);
    read_and_place_words(words_file);
    write_csv(output_file);
    return 0;
}
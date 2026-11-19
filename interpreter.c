#include <stdio.h>
#include <stdlib.h>
#include <ctype.h>
#include "interpreter.h"

int * memory_cells;
long icell;
long file_size;
char * file_data;

int main(int argc, char const *argv[])
{
    if (argc < 2) {
        fprintf(stderr, "Usage: %s <filename>\n", argv[0]);
        return 1;
    }

    if (upload_file_data(argv) != 0)
        return 1;
    
    memory_cells = calloc(MEMORY_SIZE, sizeof(int));
    if (memory_cells == NULL) {
        perror("unable to allcoate memory cells");
        return 1;
    }

    icell = 0;

    for (long i = 0; i < file_size; i++)
    {
        if ((file_data[i] != '+') &&
            (file_data[i] != '-') &&
            (file_data[i] != '>') &&
            (file_data[i] != '<') &&
            (file_data[i] != '.') &&
            (file_data[i] != ',') &&
            (file_data[i] != '[') &&
            (file_data[i] != ']')) continue;

        if (file_data[i] == '+') memory_cells[icell]++;
        else if (file_data[i] == '-') memory_cells[icell]--;
        else if (file_data[i] == '>') {if (icell < MEMORY_SIZE - 1) icell++;}
        else if (file_data[i] == '<') {if (icell > 0) icell--;}
        else if (file_data[i] == '.') putchar(memory_cells[icell]);
        else if (file_data[i] == ',') {
            int c = getchar();
            memory_cells[icell] = (c == EOF) ? 0 : c;
        }
        else if (file_data[i] == '[') {
            if (memory_cells[icell] != 0) continue;
            // else

            // the matching closing brace, would be the one that is the subsequent to have net zero opening/closing braces.

            i++; // move over the current '['
            int net_braces = 0;
            for (int j = i; j < file_size; j++){
                if ((file_data[j] == ']') && (net_braces == 0)) {
                    i = j;
                    break;
                } else if (file_data[j] == ']') net_braces--;
                else if (file_data[j] == '[') net_braces++;
            }
        }
        else if (file_data[i] == ']') {
            if (memory_cells[icell] == 0) continue;
            // else

            // the matching opening brace, would be the one that is the Prior to have net zero opening/closing braces.

            int net_braces = 0;
            for (int j = i-1 /* -1 for lag behind the current ] */; j >= 0; j--){
                if ((file_data[j] == '[') && (net_braces == 0)) {
                    i = j;
                    break;
                } else if (file_data[j] == ']') net_braces--;
                else if (file_data[j] == '[') net_braces++;
            }
        }
    }

    free(memory_cells);
    free(file_data);
    return 0;
}

int upload_file_data(char const *argv[]) {
    FILE *file = fopen(argv[1], "rb"); // Open in binary mode for exact byte count
    if (file == NULL) {
        perror("Failed to open file");
        return 1;
    }

    // 1. Find the size of the file
    fseek(file, 0, SEEK_END);
    file_size = ftell(file);
    rewind(file); // Go back to the beginning of the file

    // 2. Allocate memory (+1 for the null terminator '\0')
    file_data = malloc(file_size + 1);
    if (file_data == NULL) {
        perror("Memory allocation failed");
        fclose(file);
        return 1;
    }

    // 3. Read the entire file into the array
    size_t bytes_read = fread(file_data, 1, file_size, file);
    file_data[bytes_read] = '\0'; // Null-terminate so it can be used as a string

    // 4. Clean up
    fclose(file);
    return 0;
}
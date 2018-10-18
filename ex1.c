#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#define STARTING_VALUE 100
#define False 0
#define True 1

typedef struct frame {
    unsigned char refered;
    unsigned int span;
    int number;
} frame;

typedef struct page_class {
    frame **frames;
} page_class;

page_class *initialise(int no_of_frames) {

    page_class *system = (page_class *) malloc(sizeof(page_class));
    system->frames = (frame **) malloc(sizeof(frame) * no_of_frames);

    for (int i = 0; i < no_of_frames; ++i) {
        system->frames[i] = NULL;
    }
    return system;
}

int page_fill(page_class *system, int number, int no_of_frames);

int page_fill(page_class *system, int number, int no_of_frames) {
    int lowest = 0;
    for (int i = 0; i < no_of_frames; ++i) {
        if (system->frames[i] == NULL) {
            system->frames[i] = (frame *) malloc(sizeof(frame));
            system->frames[i]->number = number;
            system->frames[i]->span = 1u << (8 * sizeof(char) - 1);
            system->frames[i]->refered = 1;
            return 0;
        } else {
            if (system->frames[lowest]->span > system->frames[i]->span) {
                system->frames[lowest] = system->frames[i];
            }
        }
    }

    system->frames[lowest]->number = number;
    system->frames[lowest]->span = 1u << (8 * sizeof(char) - 1);
    system->frames[lowest]->refered = 1;
    return 0;
}

int get_page(page_class *system, int number, int no_of_frames) {
    int found = False;
    for (int i = 0; i < no_of_frames; ++i) {
        frame *current_frame = system->frames[i];

        if (current_frame != NULL) {
            if (current_frame->number == number) {
                found = True;
                current_frame->refered = 1;
                current_frame->span = (current_frame->span >> 1u) | (1u << (8 * sizeof(char) - 1));
            } else {
                current_frame->refered = 0;
                current_frame->span = current_frame->span >> 1u;
            }
        }
    }

    if (!found) {
        page_fill(system, number, no_of_frames);
    }

    return !found;
}

int main(int argc, char *argv[]) {

    if (argc != 2) {
        printf("No. of frames?\n");
        return 1;
    }

    int no_of_frames = atoi(argv[1]);
    page_class *system = initialise(no_of_frames);
    int size = STARTING_VALUE;
    int *a = (int *) malloc(size * sizeof(int));
    int n = 0;
    FILE *fp;
    int c;
    fp = fopen("inputfile.txt", "r");
    
    while (fscanf(fp, "%d", &c) == 1) {
        a[n] = c;
        n++;
        if (n >= size) {
            size += STARTING_VALUE;
            a = (int *) realloc(a, size * sizeof(int));
        }
    }

    double miss = 0;
    for (int i = 0; i < n; ++i) {
        miss += get_page(system, a[i], no_of_frames);
    }

    printf("Hit / Miss ratio: %.5f\n", ((n - miss) / miss));
}

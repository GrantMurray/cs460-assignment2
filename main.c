#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <pthread.h>
#include <errno.h>

typedef struct PCB {
  int pid, pr;
  int num_cpu_burst, num_io_burst;
  int* cpu_burst;
  int* io_burst;
  int cpu_index, io_index;
  struct PCB* prev;
  struct PCB* next;
} PCB;

struct FILE_READ_ARGS {
  char* file_name;
  PCB** ready_q;
};

#define MAX_LINE_LEN 256
#define MAX_FILE_LEN 256

// ** FILE READ THREAD ** // 

void* file_read_thread(void* argp) {
  struct FILE_READ_ARGS* args = (struct FILE_READ_ARGS*) argp;
  
  FILE* file_pointer = fopen(args->file_name, "r");
  if (file_pointer == NULL) {
    fprintf(stderr, "Error: unable to open file %s\n", args->file_name);
    exit(1);
  }

  size_t line_len = (size_t) MAX_LINE_LEN;
  char* line_buffer = malloc(MAX_LINE_LEN * sizeof(char));
  while (getline(&line_buffer, &line_len, file_pointer) != -1) {
    printf("%s", line_buffer);
  }
  
  free(line_buffer);
  fclose(file_pointer);

  return NULL;
}

// ** MAIN THREAD ** //

// Checks if value is inside array
int check_membership(char* value, char** array, int length) {
  for (int i = 0; i < length; i++) {
    if (strcmp(value, array[i]) == 0) return 1;
  }
  return 0;
}

// Will validate all input for program, and return valid file pointer
// to provided file, read only, must be freed with fclose()
int validate_input(int argc, char** argv) {
  char* valid_algv[3] = {"FIFO", "SJF", "PR"};
  int valid_algc = 3;

  if (argc != 5) {
    fprintf(stderr, "Error: invalid argument count\n");
    return 1;
  }

  if (strcmp(argv[1], "-alg") != 0) {
    fprintf(stderr, "Error: missing '-alg' flag\n");
    return 1;
  }
  
  if (check_membership(argv[2], valid_algv, valid_algc) == 0) {
    fprintf(stderr, "Error: invalid alg\n");
    return 1;
  }
  
  if (strcmp(argv[3], "-input")) {
    fprintf(stderr, "Error: missing '-input' flag\n");
    return 1;
  }

  if (access(argv[4], R_OK) != 0) {
    fprintf(stderr, "Error: unable to open input file\n");
    return 1;
  }

  return 0;
}

// Takes input file and reads all lines from file, returns 2d array
// where each string is a file line, must be freed with free()
char** read_file_lines(FILE* input_fp) {
  char** file_lines = calloc(MAX_FILE_LEN, sizeof(char*));
  size_t line_len = MAX_LINE_LEN;
  for (int i = 0; i < MAX_FILE_LEN; i++) {
    if (getline(&file_lines[i], &line_len, input_fp) == -1) break;
  }

  return file_lines;
}

int main(int argc, char** argv) {
  // Init input
  if (validate_input(argc, argv) != 0) {
    fprintf(stderr, "Usage: ./micro-schedule -alg [FIFO|SJF|PR] -input [FILE]\n");
    exit(1);
  }

  char* algorithm = argv[2];
  char* file_name = argv[4];

  PCB* ready_q = malloc(sizeof(PCB));
  PCB* io_q = malloc(sizeof(PCB));

  struct FILE_READ_ARGS* fr_thread_args = malloc(sizeof(struct FILE_READ_ARGS));
  fr_thread_args->file_name = file_name;
  fr_thread_args->ready_q = NULL;

  pthread_t fr_thread_id;
  if (pthread_create(
    &fr_thread_id, NULL, &file_read_thread, (void*) fr_thread_args
  ) == -1) {
    fprintf(stderr, "Error: unable to start thread 'file_read'\n");
    free(fr_thread_args);
    exit(1);
  }

  pthread_join(fr_thread_id, NULL);
  free(fr_thread_args);
  
  free(ready_q);
  free(io_q);

  return 0;
}

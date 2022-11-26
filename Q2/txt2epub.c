#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/wait.h>
#include <unistd.h>

int file_exists(const char *filename) {
  FILE *fp = fopen(filename, "r");
  int exists = 0;
  if (fp != NULL) {
    exists = 1;
    fclose(fp);
  }
  return exists;
}

char *str_list_cat(char **list, int n) {
  int total_len = 0;
  for (int i = 0; i < n; i++)
    total_len += strlen(list[i]);
  char *ret = (char *)malloc(total_len * sizeof(char));
  *ret = 0;
  for (int i = 0; i < n; i++) {
    strcat(ret, list[i]);
    strcat(ret, " ");
  }
  return ret;
}

char *change_to_epub(char *filename) {
  char *cp = (char *)malloc((strlen(filename) + 1) * sizeof(char));
  strcpy(cp, filename);
  char *dot = cp + strlen(cp) - 3;
  strcpy(dot, "epub");
  return cp;
}

int pandoc(char *filename) {
  char *cp = change_to_epub(filename);
  char *command_array[4] = {"pandoc", filename, "-o", cp};

  char *ptr = str_list_cat(command_array, 4);
  int status = system(ptr);
  if (status == -1) {
    perror("Zip Error");
    return -1;
  }

  free(ptr);
  return 0;
}

int zip(char **list, int n) {
  int acc = 0;
  char *command_array[n + 2];
  command_array[0] = "zip";
  command_array[1] = "ebooks.zip";
  for (int i = 0; i < n; i++) {
    char *cp = change_to_epub(list[i + 1]);
    if (file_exists(cp)) {
      command_array[i + 2] = cp;
      acc++;
    } else {
      char *sp = "";
      command_array[i + 2] = sp;
    }
  }

  char *ptr = str_list_cat(command_array, n + 2);
  int status = 0;
  if (acc > 0)
    status = system(ptr);
  if (status == -1) {
    perror("Zip Error");
    return -1;
  }

  free(ptr);
  return 0;
}

int main(int argc, char *argv[]) {
  pid_t pid = getpid();
  int forked = 0;
  // A cada processo é atribuido um numero (process_number), que o identifica
  int process_number = 1;
  for (; process_number < argc - 1; process_number++) {
    forked = 1;
    pid = fork();
    if (pid == -1) {
      perror("Fork Error");
      return EXIT_FAILURE;
    }
    if (pid != 0)
      break;
  }

  if (!file_exists(argv[process_number])) {
    printf("%s doesn't exit\n", argv[process_number]);
  } else {
    printf("[pid%d] coverting %s ...\n", getpid(), argv[process_number]);
    int st_pandoc = pandoc(argv[process_number]);
    if (st_pandoc == -1)
      return EXIT_FAILURE;
  }

  if (forked && pid != 0 && waitpid(pid, NULL, 0) == -1) {
    perror("wait");
    return EXIT_FAILURE;
  }

  if (process_number == 1) {
    printf("...\n");
    int st_zip = zip(argv, argc - 1);
    if (st_zip == -1)
      return EXIT_FAILURE;
  }
  return EXIT_SUCCESS;
}



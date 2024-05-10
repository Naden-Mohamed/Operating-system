#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#define MAX_COMMAND_LENGTH 50
#define MAX_LINE_LENGTH 100

typedef struct {
  char dos_command[MAX_COMMAND_LENGTH];
  char linux_command[MAX_COMMAND_LENGTH];
} CommandMapping;

/**
 * Execute the given command and print the output to stdout
 *
 * @param command The command to execute
 * @return
 */
void execute_command(const char *command) {
  char buffer[MAX_LINE_LENGTH];
  FILE *pipe = popen(command, "r");
  if (!pipe) {
    fprintf(stderr, "Error executing command\n");
    return;
  }

  while (fgets(buffer, sizeof(buffer), pipe) != NULL) {
    printf("%s", buffer);
  }

  pclose(pipe);
}

/**
 * Read the command mappings from the given file
 *
 * @param filename The file to read the mappings from
 * @param mappings The array to store the mappings
 * @param numMappings The number of mappings read
 * @return 1 if successful, 0 otherwise
 */
int read_command_mappings(const char *filename, CommandMapping **mappings,
                          int *numMappings) {
  FILE *file = fopen(filename, "r");
  if (!file) {
    fprintf(stderr, "Error: File '%s' not found\n", filename);
    return 0;
  }

  int count = 0;
  char line[MAX_LINE_LENGTH];
  while (fgets(line, sizeof(line), file) != NULL) {
    count++;
  }

  fseek(file, 0, SEEK_SET);

  CommandMapping *map = malloc(count * sizeof(CommandMapping));
  if (!map) {
    fprintf(stderr, "Error: Memory allocation failed\n");
    fclose(file);
    return 0;
  }

  int index = 0;
  while (fgets(line, sizeof(line), file) != NULL && index < count) {
    char dos_cmd[MAX_COMMAND_LENGTH], linux_cmd[MAX_COMMAND_LENGTH];
    if (sscanf(line, "%49[^=]=%49s", dos_cmd, linux_cmd) == 2) {
      strncpy(map[index].dos_command, dos_cmd,
              sizeof(map[index].dos_command) - 1);
      strncpy(map[index].linux_command, linux_cmd,
              sizeof(map[index].linux_command) - 1);
      map[index].dos_command[sizeof(map[index].dos_command) - 1] = '\0';
      map[index].linux_command[sizeof(map[index].linux_command) - 1] = '\0';
      index++;
    }
  }

  fclose(file);

  *mappings = map;
  *numMappings = count;
  return 1;
}

/**
 * Handle user input and execute the corresponding command
 *
 * @param mappings The command mappings
 * @param numMappings The number of mappings
 * @return
 */
void handle_user_input(CommandMapping *mappings, int numMappings) {
  char userinput[MAX_LINE_LENGTH];
  while (1) {
    printf("\n****** SHELL SIMULATOR ******\n");
    printf("Enter DOS command (or 'exit' to quit): ");
    if (!fgets(userinput, sizeof(userinput), stdin)) {
      fprintf(stderr, "Error reading user input\n");
      break;
    }
    userinput[strcspn(userinput, "\n")] = '\0';

    if (strcmp(userinput, "exit") == 0) {
      break;
    }

    char *command = strtok(userinput, " ");
    char *args = strtok(NULL, "");

    if (command == NULL) {
      printf("Invalid command\n");
      continue;
    }

    int found = 0;
    for (int i = 0; i < numMappings; i++) {
      if (strcmp(command, mappings[i].dos_command) == 0) {

        char linux_command[MAX_LINE_LENGTH];
        snprintf(linux_command, sizeof(linux_command), "%s %s",
                 mappings[i].linux_command, args ? args : "");
        execute_command(linux_command);
        found = 1;
        break;
      }
    }

    if (!found) {
      printf("Invalid command, check the man page\n");
    }
  }
}

/**
 * Free the memory allocated for the command mappings
 *
 * @param mappings The command mappings
 * @return
 */
void free_mappings(CommandMapping *mappings) { free(mappings); }

int main() {
  CommandMapping *mappings;
  int numMappings;

  if (!read_command_mappings("mapping.txt", &mappings, &numMappings)) {
    return 1;
  }

  handle_user_input(mappings, numMappings);

  free_mappings(mappings);

  return 0;
}


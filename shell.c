#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/wait.h>
#include <unistd.h>

#include "utils.h"

int my_exec(char* progname, char** args) {
  return execvp(progname, args);
}

ssize_t prompt_and_get_input(const char* prompt,
                            char **line,
                            size_t *len) {
  fputs(prompt, stderr);
  return getline(line, len, stdin);
}

int exec_with_redir(cmd_struct* command, int in_fd, int out_fd) {
  if (in_fd != -1) {
    dup2(in_fd, STDIN_FILENO);
    close(in_fd);
  }
  if (out_fd != -1) {
    dup2(out_fd, STDOUT_FILENO);
    close(out_fd);
  }
  return execvp(command->progname, command->args);
}

pid_t run_with_redir(cmd_struct* command, int in_fd, int out_fd) {
  pid_t child_pid = fork();

  if (child_pid) {  /* We are the parent. */
    switch(child_pid) {
      case -1:
        fprintf(stderr, "Oh dear.\n");
        return -1;
      default:
        return child_pid;
    }
  } else {  // We are the child. */
    fputs("about to exec!\n", stderr);
    exec_with_redir(command, in_fd, out_fd);
    perror("OH DEAR");
    return 0;
  }
}

int main(int argc, char **argv) {
  pid_t child_pid;
  cmd_struct* command1, *command2;
  int stat_loc = 0;
  char *line = NULL, *cmd = NULL;
  size_t len = 0;

  while(prompt_and_get_input("heeee> ", &line, &len) > 0) {
    fprintf(stderr, "%s\n", line);
    pipeline_struct* pipeline = parse_pipeline(line);
    print_pipeline(pipeline);
    return 1;
    //print_command(command1);
  //  char* cmd_str2= strsep(&line, "|");
  //  fprintf(stderr, "%s\n", cmd_str2);
  //  command2 = parse_command(cmd_str2);
  //  print_command(command2);

  //  int ends[2];
  //  pipe(ends);
  //  fprintf(stderr, "in: %d\nout: %d\n", ends[0], ends[1]);

  //  run_with_redir(command1, -1, ends[1]);
  //  run_with_redir(command2, ends[0], -1);
  //  close(ends[0]);
  //  close(ends[1]);
    run_with_redir(command1, -1, -1);
    while (wait(NULL) > 0);
  }
  fputs("\n", stderr);
  return 0;

}

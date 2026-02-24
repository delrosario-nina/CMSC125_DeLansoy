#include "dccsh.h"

// for background jobs
static pid_t bg_jobs[MAX_BG_JOBS];
static int bg_job_count = 0;

void execute_command(Command *cmd) {
    if (cmd == NULL)
        return;

    if (is_builtin(cmd)) {
        run_builtin(cmd);
        return;
    }

    if (!is_builtin(cmd)) {
        run_external(cmd);
    }
}

// checks if user typed built in func
bool is_builtin(Command *cmd) {
    if (cmd == NULL || cmd->command == NULL)
        return false;

    return strcmp(cmd->command, "exit") == 0 ||
           strcmp(cmd->command, "cd") == 0   ||
           strcmp(cmd->command, "pwd") == 0;
}

void run_builtin(Command *cmd) {
    // if exit, shell exits lol 
    if (strcmp(cmd->command, "exit") == 0) {
        exit(0);
    }

    // if pwd, gets the path of working directory using getcwd
    else if (strcmp(cmd->command, "pwd") == 0) {
        char cwd[MAX_INPUT];
        if (getcwd(cwd, sizeof(cwd)) != NULL) {
            printf("%s\n", cwd);
        } else {
            perror("pwd");
        }
    }

    else if (strcmp(cmd->command, "cd") == 0) {
        char *path = cmd->args[1];
        if (path == NULL) {
            path = getenv("HOME");
            if (path == NULL) {
                fprintf(stderr, "dccsh: HOME not set\n"); 
                return;
            }
        }
        
        if (chdir(path) != 0) {
            perror("cd");
        }
    }
}

void run_external(Command *cmd) {
    pid_t pid = fork();

    if (pid < 0) {
        perror("fork");
        return;
    }

    if (pid == 0) {
        // child process
        if (apply_io_redirection(cmd) != 0) {
            exit(1);
        }
        execvp(cmd->args[0], cmd->args);
        if (errno == ENOENT || errno == EACCES) {
            fprintf(stderr, "%s: command not found\n", cmd->args[0]);
        } else {
            perror(cmd->args[0]);
        }
        _exit(127);
    } else {
        // parent process
        if (cmd->background) {
            // add to background job tracking
            add_bg_job(pid);
        } else {
            pid_t w = waitpid(pid, NULL, 0);
            if (w == -1) {
                perror("waitpid");
            }
        }
    }
}

int apply_io_redirection(Command *cmd) {
    int fd;
    if (cmd->input_file) {
        fd = open(cmd->input_file, O_RDONLY);
        if (fd < 0) {
            perror("open");
            return -1;
        }
        if (dup2(fd, STDIN_FILENO) == -1) {
            perror("dup2");
            close(fd);
            return -1;
        }
        if (close(fd) == -1) {
            perror("close");
            return -1;
        }
    }
    if (cmd->output_file) {
        int flags = O_WRONLY | O_CREAT | (cmd->append ? O_APPEND : O_TRUNC);
        fd = open(cmd->output_file, flags, 0644);
        if (fd < 0) {
            perror("open");
            return -1;
        }
        if (dup2(fd, STDOUT_FILENO) == -1) {
            perror("dup2");
            close(fd);
            return -1;
        }
        if (close(fd) == -1) {
            perror("close");
            return -1;
        }
    }
    return 0;
}

// add background job to tracking list
void add_bg_job(pid_t pid) {
    if (bg_job_count < MAX_BG_JOBS) {
        bg_jobs[bg_job_count++] = pid;
        printf("Started background job [%d] %d\n", bg_job_count, pid);
    } else {
        fprintf(stderr, "dccsh: too many background jobs\n");
    }
}

// clean up finished background jobs using WNOHANG
void cleanup_bg_jobs(void) {
    int status;
    
    for (int i = 0; i < bg_job_count; i++) {
        pid_t result = waitpid(bg_jobs[i], &status, WNOHANG);
        
        if (result > 0) {
            if (WIFEXITED(status)) {
                printf("[%d]+ Done    %d (exit %d)\n", 
                       i + 1, bg_jobs[i], WEXITSTATUS(status));
            } else if (WIFSIGNALED(status)) {
                printf("[%d]+ Terminated    %d (signal %d)\n", 
                       i + 1, bg_jobs[i], WTERMSIG(status));
            } else {
                printf("[%d]+ Done    %d\n", i + 1, bg_jobs[i]);
            }
            
            for (int j = i; j < bg_job_count - 1; j++) {
                bg_jobs[j] = bg_jobs[j + 1];
            }
            bg_job_count--;
            i--; 
        } else if (result == 0) {
            /* still running */
            continue;
        } else {
            /* result == -1 */
            if (errno == ECHILD) {
                /* no such child; remove stale PID */
                for (int j = i; j < bg_job_count - 1; j++) {
                    bg_jobs[j] = bg_jobs[j + 1];
                }
                bg_job_count--;
                i--;
                continue;
            }
            perror("waitpid");
        }
    }
}

// gets bg count
int get_bg_job_count(void) {
    return bg_job_count;
}
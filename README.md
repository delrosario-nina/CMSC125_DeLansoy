**Members**
Del Rosario, Nina Claudia
Lansoy, Sam

# dccsh — Dubai Chewy Cookie Shell

dccsh is a simple UNIX-like command-line shell written in C. It covers core operating system concepts like process creation, command execution, I/O redirection, built-in commands, and background job handling.

---

## Compilation and Usage

**Build:**
```
make
```

**Run:**
```
./dccsh
```

Once running, the shell displays a prompt and waits for input:
```
dccsh>
```

Type any command and press Enter. Use `exit` or Ctrl+D to quit.

---

## Implemented Features

**Interactive Shell Loop**

The shell runs a continuous read-execute loop (`main.c`). It displays `dccsh>`, reads a line of input, parses it, executes it, and then checks for any finished background jobs before looping again. EOF (Ctrl+D) exits cleanly.

**Command Parsing**

Raw input is tokenized and stored in a `Command` struct. The parser handles:
- Command name and argument list
- Input redirection (`<`)
- Output redirection (`>` for overwrite, `>>` for append)
- Background execution (`&`)
- Syntax errors like missing filenames after redirection operators
- Empty or whitespace-only input (silently ignored)

**Built-in Commands**

Built-ins run directly in the shell process without forking:

- `cd [path]` — changes the working directory; defaults to `$HOME` if no path is given
- `pwd` — prints the current working directory
- `exit` — terminates the shell

**External Command Execution**

Non-built-in commands are run by forking a child process and calling `execvp()`. The parent waits on foreground jobs with `waitpid()`. For unrecognized commands, a descriptive error is printed before exiting the child with code 127.
```
ls -l
cat file.txt
grep main dccsh.c
echo hello world
```

**I/O Redirection**

File descriptors are redirected using `open()` and `dup2()` inside the child process before `execvp()` is called:
```
ls > files.txt          # overwrite
echo hello >> log.txt   # append
cat < input.txt         # stdin from file
```

**Background Jobs**

Commands suffixed with `&` run in the background. The shell returns to the prompt immediately and tracks background PIDs in an internal array (up to `MAX_BG_JOBS`). After each command, finished background jobs are reaped with `WNOHANG` to prevent zombies, and a completion message is printed.
```
sleep 10 &
```

**Error Handling**

System call failures (`fork`, `execvp`, `open`, `dup2`, `waitpid`, `chdir`) are caught and reported via `perror()` or `fprintf(stderr, ...)`. The shell keeps running after any error rather than crashing.

---

## Known Limitations and Bugs

- **No signal handling** — Ctrl+C will terminate the shell itself rather than just the foreground process
- **Background job limit** — the number of concurrent background jobs is capped at `MAX_BG_JOBS`; excess jobs are rejected
- **No quoting or escaping** — arguments with spaces or special characters cannot be wrapped in quotes

---

## Design Decisions and Architecture Overview

The project is split across a few focused files:

- `main.c` — the shell loop: prompt, read, parse, execute, cleanup
- `executor.c` (this file) — command dispatch, built-ins, external execution, I/O redirection, and background job management
- `parser.c` — input tokenization and `Command` struct construction
- `dccsh.h` — shared types, constants, and function declarations

**Command struct ownership** — `parse_input()` takes ownership of the input buffer. On success it embeds the buffer in the `Command` struct so `free_command()` can release everything in one call. On failure, the buffer is freed inside the parser, so `main.c` doesn't need to track it separately.

**Built-in detection** — `is_builtin()` is a simple string comparison against the three supported built-ins. Because built-ins need to affect the shell's own state (e.g. `chdir` must run in the parent), they skip the fork entirely.

**Background job cleanup** — rather than using `SIGCHLD`, cleanup is done synchronously at the top of each loop iteration using `WNOHANG`. This keeps the design simple and avoids signal handler complexity, at the cost of only reaping jobs between commands.

**I/O redirection in the child** — redirection is applied after `fork()` but before `execvp()`, so it only affects the child's file descriptors and never touches the shell's own stdin/stdout.
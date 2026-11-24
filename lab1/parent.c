#include <unistd.h>
#include <fcntl.h>
#include <sys/wait.h>
#include <stdlib.h>
#include <string>
#include <vector>

int main() {

    std::string filename;
    char c;
    while (read(STDIN_FILENO, &c, 1) > 0) {
        if (c == '\n' || c == '\r') break;
        filename += c;
    }

    if (filename.empty()) {
        const char* err = "Error: empty filename\n";
        write(STDERR_FILENO, err, 21);
        exit(EXIT_FAILURE);
    }

    int file_fd = open(filename.c_str(), O_RDONLY);
    if (file_fd == -1) {
        const char* err = "Error: cannot open file\n";
        write(STDERR_FILENO, err, 24);
        exit(EXIT_FAILURE);
    }

    int pipefd[2];
    if (pipe(pipefd) == -1) {
        const char* err = "Error: pipe failed\n";
        write(STDERR_FILENO, err, 19);
        exit(EXIT_FAILURE);
    }

    pid_t pid = fork();
    if (pid == -1) {
        const char* err = "Error: fork failed\n";
        write(STDERR_FILENO, err, 19);
        exit(EXIT_FAILURE);
    }

    if (pid == 0) {
        close(pipefd[0]);

        dup2(file_fd, STDIN_FILENO);
        dup2(pipefd[1], STDOUT_FILENO);

        close(file_fd);
        close(pipefd[1]);

        char* const args[] = {(char*)"./child", nullptr};
        execv("./child", args);

        const char* err = "Error: execv failed\n";
        write(STDERR_FILENO, err, 20);
        exit(EXIT_FAILURE);
    }

    close(pipefd[1]);
    close(file_fd);

    char buffer[4096];
    ssize_t bytes_read;
    while ((bytes_read = read(pipefd[0], buffer, sizeof(buffer))) > 0) {
        write(STDOUT_FILENO, buffer, bytes_read);
    }
    close(pipefd[0]);

    int status;
    waitpid(pid, &status, 0);

    if (WIFEXITED(status) && WEXITSTATUS(status) == 1) {
        const char* err = "Parent terminating due to child division by zero\n";
        write(STDERR_FILENO, err, 48);
        exit(EXIT_FAILURE);
    }

    return EXIT_SUCCESS;
}

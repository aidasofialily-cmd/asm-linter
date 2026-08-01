#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <signal.h>
#include <unistd.h>
#include <execinfo.h>
#include "../include/asmlint.h"

static void crash_handler(int sig) {
    static volatile sig_atomic_t already_crashing = 0;
    if (already_crashing) {
        _exit(1);
    }
    already_crashing = 1;

    const char *sig_name = "UNKNOWN";
    switch (sig) {
        case SIGSEGV: sig_name = "SIGSEGV"; break;
        case SIGFPE:  sig_name = "SIGFPE";  break;
        case SIGILL:  sig_name = "SIGILL";  break;
        case SIGBUS:  sig_name = "SIGBUS";  break;
        case SIGABRT: sig_name = "SIGABRT"; break;
    }

    const char *msg1 = "\n*** asmlint crashed ***\nReceived signal: ";
    ssize_t ret;
    ret = write(STDERR_FILENO, msg1, strlen(msg1));
    (void)ret;
    ret = write(STDERR_FILENO, sig_name, strlen(sig_name));
    (void)ret;
    const char *msg2 = "\nStack trace:\n";
    ret = write(STDERR_FILENO, msg2, strlen(msg2));
    (void)ret;

    void *array[32];
    int size = backtrace(array, 32);
    backtrace_symbols_fd(array, size, STDERR_FILENO);

    signal(sig, SIG_DFL);
    raise(sig);
}

static void report_issue(const asm_issue_t *issue, void *user_data) {
    (void)user_data;
    const char *sev = (issue->severity == ASM_SEVERITY_ERROR) ? "ERROR" :
                      (issue->severity == ASM_SEVERITY_WARNING) ? "WARN" : "INFO";
    printf("[%s] %s:%u [%s] %s\n", sev, issue->file_path, issue->line_number, issue->rule_id, issue->message);
}

int main(int argc, char **argv) {
    struct sigaction sa;
    memset(&sa, 0, sizeof(sa));
    sa.sa_handler = crash_handler;
    sigemptyset(&sa.sa_mask);
    sa.sa_flags = SA_RESETHAND;

    sigaction(SIGSEGV, &sa, NULL);
    sigaction(SIGFPE, &sa, NULL);
    sigaction(SIGILL, &sa, NULL);
    sigaction(SIGBUS, &sa, NULL);
    sigaction(SIGABRT, &sa, NULL);

    if (argc < 2) {
        printf("Usage: %s <file_or_directory_path>\n", argv[0]);
        return 1;
    }

    for (int i = 1; i < argc; i++) {
        if (strcmp(argv[i], "--crash") == 0) {
            /* Deliberate NULL pointer dereference to trigger SIGSEGV */
            volatile int *ptr = NULL;
            (void)*ptr;
            *ptr = 42;
        }
    }

    asmlint_config_t config = {
        .thread_count = 0,
        .require_tab_indent = 1,
        .max_line_length = 100,
        .callback = report_issue,
        .user_data = NULL
    };

    asmlint_init(&config);
    
    for (int i = 1; i < argc; i++) {
        asmlint_process_file(argv[i]);
    }

    asmlint_destroy();
    return 0;
}

#include <stdio.h>
#include <stdlib.h>
#include "../include/asmlint.h"

static void report_issue(const asm_issue_t *issue, void *user_data) {
    (void)user_data;
    const char *sev = (issue->severity == ASM_SEVERITY_ERROR) ? "ERROR" :
                      (issue->severity == ASM_SEVERITY_WARNING) ? "WARN" : "INFO";
    printf("[%s] %s:%u [%s] %s\n", sev, issue->file_path, issue->line_number, issue->rule_id, issue->message);
}

int main(int argc, char **argv) {
    if (argc < 2) {
        printf("Usage: %s <file_or_directory_path>\n", argv[0]);
        return 1;
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

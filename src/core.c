#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "../include/asmlint.h"
#include "../include/rules.h"

static asmlint_config_t g_config;

int asmlint_init(const asmlint_config_t *config) {
    if (!config) return -1;
    g_config = *config;
    return 0;
}

int asmlint_process_file(const char *filepath) {
    FILE *fp = fopen(filepath, "rb");
    if (!fp) return -1;

    char line[1024];
    uint32_t line_num = 0;

    while (fgets(line, sizeof(line), fp)) {
        line_num++;

        /* Rule 1: Check trailing whitespace */
        size_t len = strlen(line);
        if (len > 1 && (line[len - 2] == ' ' || line[len - 2] == '\t')) {
            if (g_config.callback) {
                asm_issue_t issue = {
                    .line_number = line_num,
                    .severity = ASM_SEVERITY_WARNING,
                    .rule_id = "W001",
                };
                strncpy(issue.file_path, filepath, sizeof(issue.file_path) - 1);
                snprintf(issue.message, sizeof(issue.message), "Trailing whitespace detected");
                g_config.callback(&issue, g_config.user_data);
            }
        }

        /* Rule 2: Unaligned stack pointer adjustment detection */
        if (strstr(line, "sub rsp,") || strstr(line, "sub esp,")) {
            if (strstr(line, "sub rsp, 8") || strstr(line, "sub esp, 4")) {
                if (g_config.callback) {
                    asm_issue_t issue = {
                        .line_number = line_num,
                        .severity = ASM_SEVERITY_ERROR,
                        .rule_id = "E102",
                    };
                    strncpy(issue.file_path, filepath, sizeof(issue.file_path) - 1);
                    snprintf(issue.message, sizeof(issue.message), "Unrecommended stack allocation size (potential 16-byte misalignment)");
                    g_config.callback(&issue, g_config.user_data);
                }
            }
        }
    }

    fclose(fp);
    return 0;
}

int asmlint_process_directory_recursive(const char *dirpath) {
    (void)dirpath;
    return 0;
}

void asmlint_destroy(void) {
    /* Cleanup resources */
}

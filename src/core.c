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

static int ends_with_case_insensitive(const char *str, const char *suffix) {
    size_t str_len = strlen(str);
    size_t suffix_len = strlen(suffix);
    if (str_len < suffix_len) {
        return 0;
    }
    const char *p = str + str_len - suffix_len;
    while (*p && *suffix) {
        char c1 = *p;
        char c2 = *suffix;
        if (c1 >= 'A' && c1 <= 'Z') c1 = c1 - 'A' + 'a';
        if (c2 >= 'A' && c2 <= 'Z') c2 = c2 - 'A' + 'a';
        if (c1 != c2) {
            return 0;
        }
        p++;
        suffix++;
    }
    return 1;
}

int asmlint_process_file(const char *filepath) {
    if (filepath) {
        if (ends_with_case_insensitive(filepath, ".html") || ends_with_case_insensitive(filepath, ".css")) {
            printf("Your .html and .css files are not supported.\n");
            return -1;
        }
        if (!ends_with_case_insensitive(filepath, ".asm")) {
            printf("Please upload a .asm file.\n");
            return -1;
        }
    }

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

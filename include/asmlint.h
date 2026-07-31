#ifndef ASMLINT_H
#define ASMLINT_H

#include <stddef.h>
#include <stdint.h>

#ifdef __cplusplus
extern "C" {
#endif

typedef enum {
    ASM_SEVERITY_INFO = 0,
    ASM_SEVERITY_WARNING,
    ASM_SEVERITY_ERROR
} asm_severity_t;

typedef struct {
    char file_path[512];
    uint32_t line_number;
    asm_severity_t severity;
    const char *rule_id;
    char message[256];
} asm_issue_t;

typedef void (*asm_issue_cb)(const asm_issue_t *issue, void *user_data);

typedef struct {
    uint32_t thread_count;      /* Set 0 for auto-detection */
    uint8_t max_line_length;     /* Standard: 80 or 100 */
    uint8_t require_tab_indent;  /* Enforcement flag */
    asm_issue_cb callback;
    void *user_data;
} asmlint_config_t;

/* Primary Engine API Functions */
int asmlint_init(const asmlint_config_t *config);
int asmlint_process_file(const char *filepath);
int asmlint_process_directory_recursive(const char *dirpath);
void asmlint_destroy(void);

#ifdef __cplusplus
}
#endif

#endif /* ASMLINT_H */

#include "shell.h"
#include "fs.h"
#include "console.h"
#include "string.h"

// Helper: Print file entry for ls command
static void print_file_entry(const char *name, file_type_t type, uint32_t size) {
    if (type == TYPE_DIR) {
        console_puts("  [DIR]  ");
    } else {
        console_puts("  [FILE] ");
    }
    console_puts(name);
    console_puts("  (");
    
    // Print size
    char size_buf[16];
    int idx = 0;
    if (size == 0) {
        size_buf[idx++] = '0';
    } else {
        char temp[16];
        int temp_idx = 0;
        while (size > 0) {
            temp[temp_idx++] = '0' + (size % 10);
            size /= 10;
        }
        for (int i = temp_idx - 1; i >= 0; i--) {
            size_buf[idx++] = temp[i];
        }
    }
    size_buf[idx] = '\0';
    
    console_puts(size_buf);
    console_puts(" bytes)\n");
}

// Helper: Parse first word and rest of string
static void parse_args(const char *input, char *first, char *rest) {
    int i = 0;
    
    // Skip leading spaces
    while (input[i] == ' ') i++;
    
    // Copy first word
    int j = 0;
    while (input[i] && input[i] != ' ' && j < 63) {
        first[j++] = input[i++];
    }
    first[j] = '\0';
    
    // Skip spaces between words
    while (input[i] == ' ') i++;
    
    // Copy rest
    j = 0;
    while (input[i] && j < 255) {
        rest[j++] = input[i++];
    }
    rest[j] = '\0';
}

// ls - List directory contents
void shell_ls(const char *args) {
    int cwd = fs_get_cwd();
    int count = fs_list(cwd, print_file_entry);
    
    if (count == 0) {
        console_puts("  (empty directory)\n");
    }
}

// cat - Display file contents
void shell_cat(const char *args) {
    if (args[0] == '\0') {
        console_puts("Usage: cat <filename>\n");
        return;
    }
    
    int idx = fs_find(args);
    if (idx < 0) {
        console_puts("File not found: ");
        console_puts(args);
        console_putc('\n');
        return;
    }
    
    if (fs_get_type(idx) != TYPE_FILE) {
        console_puts("Not a file: ");
        console_puts(args);
        console_putc('\n');
        return;
    }
    
    const char *data = fs_get_data(idx);
    uint32_t size = fs_get_size(idx);
    
    for (uint32_t i = 0; i < size; i++) {
        console_putc(data[i]);
    }
}

// touch - Create empty file
void shell_touch(const char *args) {
    if (args[0] == '\0') {
        console_puts("Usage: touch <filename>\n");
        return;
    }
    
    if (fs_find(args) >= 0) {
        console_puts("File already exists: ");
        console_puts(args);
        console_putc('\n');
        return;
    }
    
    if (fs_create(args, TYPE_FILE) < 0) {
        console_puts("Failed to create file: ");
        console_puts(args);
        console_putc('\n');
    }
}

// mkdir - Create directory
void shell_mkdir(const char *args) {
    if (args[0] == '\0') {
        console_puts("Usage: mkdir <dirname>\n");
        return;
    }
    
    if (fs_find(args) >= 0) {
        console_puts("Directory already exists: ");
        console_puts(args);
        console_putc('\n');
        return;
    }
    
    if (fs_create(args, TYPE_DIR) < 0) {
        console_puts("Failed to create directory: ");
        console_puts(args);
        console_putc('\n');
    }
}

// cd - Change directory
void shell_cd(const char *args) {
    if (args[0] == '\0') {
        // Go to root
        fs_set_cwd(0);
        return;
    }
    
    int idx = fs_find(args);
    if (idx < 0) {
        console_puts("Directory not found: ");
        console_puts(args);
        console_putc('\n');
        return;
    }
    
    if (fs_get_type(idx) != TYPE_DIR) {
        console_puts("Not a directory: ");
        console_puts(args);
        console_putc('\n');
        return;
    }
    
    fs_set_cwd(idx);
}

// pwd - Print working directory
void shell_pwd(void) {
    int cwd = fs_get_cwd();
    if (cwd == 0) {
        console_puts("/\n");
    } else {
        console_puts("/");
        console_puts(fs_get_name(cwd));
        console_putc('\n');
    }
}

// rm - Remove file or empty directory
void shell_rm(const char *args) {
    if (args[0] == '\0') {
        console_puts("Usage: rm <filename>\n");
        return;
    }
    
    if (fs_delete(args) < 0) {
        console_puts("Failed to remove: ");
        console_puts(args);
        console_puts(" (not found or directory not empty)\n");
    }
}

// write - Write content to file
// Usage: write <filename> <content>
void shell_write(const char *args) {
    char filename[64];
    char content[256];
    
    parse_args(args, filename, content);
    
    if (filename[0] == '\0') {
        console_puts("Usage: write <filename> <content>\n");
        return;
    }
    
    int idx = fs_find(filename);
    if (idx < 0) {
        console_puts("File not found. Creating new file: ");
        console_puts(filename);
        console_putc('\n');
        idx = fs_create(filename, TYPE_FILE);
        if (idx < 0) {
            console_puts("Failed to create file\n");
            return;
        }
    }
    
    // Add newline if content doesn't end with one
    int len = strlen(content);
    if (len > 0 && content[len - 1] != '\n') {
        content[len] = '\n';
        content[len + 1] = '\0';
        len++;
    }
    
    int written = fs_write(filename, content, len);
    if (written < 0) {
        console_puts("Failed to write to file\n");
    }
}

// echo - Write or append content to file using > or >>
// Usage: echo <content> > <filename>  (overwrite)
//        echo <content> >> <filename> (append)
void shell_echo(const char *args) {
    char content[256];
    char filename[64];
    int append = 0;
    
    // Find > or >>
    int i = 0;
    int redirect_pos = -1;
    
    while (args[i]) {
        if (args[i] == '>') {
            redirect_pos = i;
            if (args[i + 1] == '>') {
                append = 1;
                i++;
            }
            break;
        }
        i++;
    }
    
    if (redirect_pos < 0) {
        // No redirect, just print to console
        console_puts(args);
        console_putc('\n');
        return;
    }
    
    // Extract content (before >)
    int j = 0;
    for (int k = 0; k < redirect_pos && j < 255; k++) {
        if (args[k] != ' ' || j > 0) {  // Skip leading spaces
            content[j++] = args[k];
        }
    }
    // Trim trailing spaces
    while (j > 0 && content[j - 1] == ' ') j--;
    content[j] = '\n';  // Add newline
    content[j + 1] = '\0';
    
    // Extract filename (after > or >>)
    i = redirect_pos + (append ? 2 : 1);
    while (args[i] == ' ') i++;  // Skip spaces
    
    j = 0;
    while (args[i] && args[i] != ' ' && j < 63) {
        filename[j++] = args[i++];
    }
    filename[j] = '\0';
    
    if (filename[0] == '\0') {
        console_puts("Usage: echo <text> > <file> or echo <text> >> <file>\n");
        return;
    }
    
    // Find or create file
    int idx = fs_find(filename);
    if (idx < 0) {
        idx = fs_create(filename, TYPE_FILE);
        if (idx < 0) {
            console_puts("Failed to create file: ");
            console_puts(filename);
            console_putc('\n');
            return;
        }
        append = 0;  // New file, always write
    }
    
    int result;
    if (append) {
        result = fs_append(filename, content, strlen(content));
    } else {
        result = fs_write(filename, content, strlen(content));
    }
    
    if (result < 0) {
        console_puts("Failed to write to file\n");
    }
}

// sh - Execute shell script
void shell_sh(const char *args) {
    if (args[0] == '\0') {
        console_puts("Usage: sh <script.sh>\n");
        return;
    }
    
    int idx = fs_find(args);
    if (idx < 0) {
        console_puts("Script not found: ");
        console_puts(args);
        console_putc('\n');
        return;
    }
    
    if (fs_get_type(idx) != TYPE_FILE) {
        console_puts("Not a file: ");
        console_puts(args);
        console_putc('\n');
        return;
    }
    
    const char *script = fs_get_data(idx);
    uint32_t size = fs_get_size(idx);
    
    // Execute line by line
    char line[128];
    int line_idx = 0;
    
    for (uint32_t i = 0; i < size; i++) {
        if (script[i] == '\n') {
            line[line_idx] = '\0';
            
            // Execute line (parse command and args)
            if (line[0] != '\0' && line[0] != '#') {  // Skip empty and comments
                char cmd[64];
                char cmd_args[128];
                parse_args(line, cmd, cmd_args);
                
                // Execute command
                if (strcmp(cmd, "echo") == 0) {
                    shell_echo(cmd_args);
                } else if (strcmp(cmd, "ls") == 0) {
                    shell_ls(cmd_args);
                } else if (strcmp(cmd, "cat") == 0) {
                    shell_cat(cmd_args);
                } else if (strcmp(cmd, "touch") == 0) {
                    shell_touch(cmd_args);
                } else if (strcmp(cmd, "mkdir") == 0) {
                    shell_mkdir(cmd_args);
                } else if (strcmp(cmd, "pwd") == 0) {
                    shell_pwd();
                } else if (strcmp(cmd, "write") == 0) {
                    shell_write(cmd_args);
                } else {
                    console_puts("Unknown command in script: ");
                    console_puts(cmd);
                    console_putc('\n');
                }
            }
            
            line_idx = 0;
        } else if (line_idx < 127) {
            line[line_idx++] = script[i];
        }
    }
    
    // Execute last line if no trailing newline
    if (line_idx > 0) {
        line[line_idx] = '\0';
        if (line[0] != '\0' && line[0] != '#') {
            char cmd[64];
            char cmd_args[128];
            parse_args(line, cmd, cmd_args);
            
            if (strcmp(cmd, "echo") == 0) {
                console_puts(cmd_args);
                console_putc('\n');
            }
        }
    }
}

#include "fs.h"
#include "string.h"

static inode_t inodes[MAX_FILES];
static int current_dir = 0;  // Current working directory index

// Helper: Find a free inode
static int alloc_inode(void) {
    for (int i = 0; i < MAX_FILES; i++) {
        if (!inodes[i].used) {
            inodes[i].used = 1;
            inodes[i].size = 0;
            inodes[i].parent_idx = -1;
            return i;
        }
    }
    return -1; // No free inodes
}

// Helper: Parse path and find file/directory
int fs_find(const char *path) {
    // Handle absolute path (starts with /)
    int search_dir = (path[0] == '/') ? 0 : current_dir;
    
    // Handle root directory
    if (strcmp(path, "/") == 0) {
        return 0;
    }
    
    // Skip leading slash
    if (path[0] == '/') {
        path++;
    }
    
    // Handle empty path or "."
    if (path[0] == '\0' || strcmp(path, ".") == 0) {
        return search_dir;
    }
    
    // Handle parent directory ".."
    if (strcmp(path, "..") == 0) {
        if (search_dir == 0) return 0; // Already at root
        return inodes[search_dir].parent_idx;
    }
    
    // Search for file/directory in search_dir
    for (int i = 0; i < MAX_FILES; i++) {
        if (inodes[i].used && 
            inodes[i].parent_idx == search_dir &&
            strcmp(inodes[i].name, path) == 0) {
            return i;
        }
    }
    
    return -1; // Not found
}

// Initialize filesystem
void fs_init(void) {
    // Clear all inodes
    for (int i = 0; i < MAX_FILES; i++) {
        inodes[i].used = 0;
    }
    
    // Create root directory
    int root_idx = alloc_inode();
    strcpy(inodes[root_idx].name, "/");
    inodes[root_idx].type = TYPE_DIR;
    inodes[root_idx].parent_idx = 0; // Root is its own parent
    
    current_dir = 0;
    
    // Create some initial files
    fs_create("welcome.txt", TYPE_FILE);
    fs_write("welcome.txt", "Welcome to Tiny RISC-V Kernel!\nTry 'ls', 'cat', 'touch', 'mkdir', 'cd', and 'sh' commands.\n", 95);
    
    fs_create("hello.sh", TYPE_FILE);
    fs_write("hello.sh", "echo Hello from shell script!\necho This is a simple script\n", 60);
    
    fs_create("test.txt", TYPE_FILE);
    fs_write("test.txt", "This is a test file.\n", 21);
}

// Create a new file or directory
int fs_create(const char *path, file_type_t type) {
    // Check if already exists
    if (fs_find(path) >= 0) {
        return -1; // Already exists
    }
    
    // Allocate new inode
    int idx = alloc_inode();
    if (idx < 0) {
        return -1; // No space
    }
    
    // Set up inode
    strncpy(inodes[idx].name, path, MAX_FILENAME - 1);
    inodes[idx].name[MAX_FILENAME - 1] = '\0';
    inodes[idx].type = type;
    inodes[idx].size = 0;
    inodes[idx].parent_idx = current_dir;
    
    return idx;
}

// Write data to a file
int fs_write(const char *path, const char *data, uint32_t size) {
    int idx = fs_find(path);
    if (idx < 0) {
        return -1; // File not found
    }
    
    if (inodes[idx].type != TYPE_FILE) {
        return -1; // Not a file
    }
    
    // Limit size to max file size
    if (size > MAX_FILE_SIZE) {
        size = MAX_FILE_SIZE;
    }
    
    // Copy data
    for (uint32_t i = 0; i < size; i++) {
        inodes[idx].data[i] = data[i];
    }
    inodes[idx].size = size;
    
    return size;
}

// Append data to a file
int fs_append(const char *path, const char *data, uint32_t size) {
    int idx = fs_find(path);
    if (idx < 0) {
        return -1; // File not found
    }
    
    if (inodes[idx].type != TYPE_FILE) {
        return -1; // Not a file
    }
    
    uint32_t current_size = inodes[idx].size;
    uint32_t available = MAX_FILE_SIZE - current_size;
    
    if (size > available) {
        size = available;
    }
    
    // Append data
    for (uint32_t i = 0; i < size; i++) {
        inodes[idx].data[current_size + i] = data[i];
    }
    inodes[idx].size = current_size + size;
    
    return size;
}

// Read data from a file
int fs_read(const char *path, char *buf, uint32_t size) {
    int idx = fs_find(path);
    if (idx < 0) {
        return -1; // File not found
    }
    
    if (inodes[idx].type != TYPE_FILE) {
        return -1; // Not a file
    }
    
    // Limit size to actual file size
    if (size > inodes[idx].size) {
        size = inodes[idx].size;
    }
    
    // Copy data
    for (uint32_t i = 0; i < size; i++) {
        buf[i] = inodes[idx].data[i];
    }
    
    return size;
}

// List directory contents
int fs_list(int dir_idx, void (*callback)(const char *name, file_type_t type, uint32_t size)) {
    if (dir_idx < 0 || dir_idx >= MAX_FILES || !inodes[dir_idx].used) {
        return -1;
    }
    
    if (inodes[dir_idx].type != TYPE_DIR) {
        return -1; // Not a directory
    }
    
    int count = 0;
    for (int i = 0; i < MAX_FILES; i++) {
        if (inodes[i].used && inodes[i].parent_idx == dir_idx) {
            callback(inodes[i].name, inodes[i].type, inodes[i].size);
            count++;
        }
    }
    
    return count;
}

// Get current working directory
int fs_get_cwd(void) {
    return current_dir;
}

// Set current working directory
void fs_set_cwd(int idx) {
    if (idx >= 0 && idx < MAX_FILES && inodes[idx].used && inodes[idx].type == TYPE_DIR) {
        current_dir = idx;
    }
}

// Get file/directory name
const char* fs_get_name(int idx) {
    if (idx >= 0 && idx < MAX_FILES && inodes[idx].used) {
        return inodes[idx].name;
    }
    return NULL;
}

// Get file/directory type
file_type_t fs_get_type(int idx) {
    if (idx >= 0 && idx < MAX_FILES && inodes[idx].used) {
        return inodes[idx].type;
    }
    return TYPE_FILE; // Default
}

// Get file size
uint32_t fs_get_size(int idx) {
    if (idx >= 0 && idx < MAX_FILES && inodes[idx].used) {
        return inodes[idx].size;
    }
    return 0;
}

// Get file data
const char* fs_get_data(int idx) {
    if (idx >= 0 && idx < MAX_FILES && inodes[idx].used) {
        return inodes[idx].data;
    }
    return NULL;
}

// Delete a file or empty directory
int fs_delete(const char *path) {
    int idx = fs_find(path);
    if (idx < 0 || idx == 0) {
        return -1; // Not found or trying to delete root
    }
    
    // If directory, check if empty
    if (inodes[idx].type == TYPE_DIR) {
        for (int i = 0; i < MAX_FILES; i++) {
            if (inodes[i].used && inodes[i].parent_idx == idx) {
                return -1; // Directory not empty
            }
        }
    }
    
    inodes[idx].used = 0;
    return 0;
}

#ifndef FS_H
#define FS_H

#include "types.h"

#define MAX_FILES 32
#define MAX_FILENAME 32
#define MAX_FILE_SIZE 1024
#define MAX_PATH 128

typedef enum {
    TYPE_FILE,
    TYPE_DIR
} file_type_t;

typedef struct {
    char name[MAX_FILENAME];
    file_type_t type;
    uint32_t size;
    char data[MAX_FILE_SIZE];
    int parent_idx;  // Index of parent directory (-1 for root)
    int used;        // 1 if this inode is in use
} inode_t;

// Filesystem API
void fs_init(void);
int fs_create(const char *path, file_type_t type);
int fs_write(const char *path, const char *data, uint32_t size);
int fs_append(const char *path, const char *data, uint32_t size);
int fs_read(const char *path, char *buf, uint32_t size);
int fs_list(int dir_idx, void (*callback)(const char *name, file_type_t type, uint32_t size));
int fs_find(const char *path);
int fs_delete(const char *path);
int fs_get_cwd(void);
void fs_set_cwd(int idx);
const char* fs_get_name(int idx);
file_type_t fs_get_type(int idx);
uint32_t fs_get_size(int idx);
const char* fs_get_data(int idx);

#endif

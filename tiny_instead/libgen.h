#ifndef __LIB_GEN_PORTABLE_H_
#define __LIB_GEN_PORTABLE_H_

char *dirname(char *path);
char *basename(char *path);
char *realpath(const char *path, char *resolved_path);

#endif
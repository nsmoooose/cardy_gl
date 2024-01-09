#ifndef __DATABASE_H__
#define __DATABASE_H__

#include <stdbool.h>

bool db_open_database(const char *filename);
bool db_create_database(const char *filename);

#endif /* __DATABASE_H__ */

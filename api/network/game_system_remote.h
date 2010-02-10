#ifndef __GAME_SYSTEM_REMOTE_H__
#define __GAME_SYSTEM_REMOTE_H__

#include "../game_system.h"

game_system *game_system_connect(const char* server, int port, const char *username, const char *password);

#endif /* __GAME_SYSTEM_REMOTE_H__ */

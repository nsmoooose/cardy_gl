#ifndef __GAME_SYSTEM_REMOTE_H__
#define __GAME_SYSTEM_REMOTE_H__

#include "../game_system.h"

typedef struct game_system_provider_St {
	int (*open)(struct game_system_provider_St *provider, const char *path);
	int (*read)(struct game_system_provider_St *provider, char *buffer, int size);
	int (*write)(struct game_system_provider_St *provider, const char *buffer, int size);
	void (*close)(struct game_system_provider_St *provider);
	void *internal;
} game_system_provider;

game_system *game_system_connect(game_system_provider *provider, const char *path, const char *username, const char *password);

#endif /* __GAME_SYSTEM_REMOTE_H__ */

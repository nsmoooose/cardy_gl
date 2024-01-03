#include <check.h>
#include <stdlib.h>
#include "../api/network/game_system_remote.h"

char dummy_calls[1000];

static int dummy_open(game_system_provider *provider, const char *path) {
	strncat(dummy_calls, "open(\"", 1000);
	strncat(dummy_calls, path, 1000);
	strncat(dummy_calls, "\");", 1000);
	return 0;
}

static int dummy_write(game_system_provider *provider, const char *buffer,
                       int size) {
	strncat(dummy_calls, "write(\"", 1000);
	strncat(dummy_calls, buffer, 1000);
	strncat(dummy_calls, "\");", 1000);
	return 0;
}

static int dummy_read(game_system_provider *provider, char *buffer, int size) {
	strncat(dummy_calls, "read();", 1000);
	strncpy(buffer, "OK", size);
	return 2;
}

static void dummy_close(game_system_provider *provider) {
	strncat(dummy_calls, "close();", 1000);
}

game_system_provider *dummy_provider() {
	game_system_provider *provider = calloc(1, sizeof(game_system_provider));
	provider->open = dummy_open;
	provider->write = dummy_write;
	provider->read = dummy_read;
	provider->close = dummy_close;
	return provider;
}

START_TEST(test_game_system_remote_connect) {
	game_system_provider *provider = dummy_provider();
	game_system *system = game_system_connect(provider, "apa", "henrikn", "hn");
	ck_assert_msg(
		strcmp(dummy_calls,
	           "open(\"apa\");write(\"henrikn\n\");write(\"hn\n\");read();") ==
			0,
		"Was %s", dummy_calls);
	ck_assert(system != 0);
}
END_TEST

void add_network_tests(Suite *suite) {
	TCase *c = tcase_create("Network");
	tcase_add_test(c, test_game_system_remote_connect);
	suite_add_tcase(suite, c);
}

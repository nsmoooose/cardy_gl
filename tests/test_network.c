#include <check.h>
#include <stdlib.h>
#include "api/network/game_system_remote.h"

#define DUMMY_SIZE 1000
char dummy_calls[DUMMY_SIZE];

static void dummy_cat(const char *s) {
	strncat(dummy_calls, s, DUMMY_SIZE - strlen(dummy_calls));
}

static int dummy_open(game_system_provider *provider, const char *path) {
	dummy_cat("open(\"");
	dummy_cat(path);
	dummy_cat("\");");
	return 0;
}

static int dummy_write(game_system_provider *provider, const char *buffer,
                       int size) {
	dummy_cat("write(\"");
	dummy_cat(buffer);
	dummy_cat("\");");
	return 0;
}

static int dummy_read(game_system_provider *provider, char *buffer, int size) {
	dummy_cat("read();");
	strncpy(buffer, "OK", size);
	return 2;
}

static void dummy_close(game_system_provider *provider) {
	dummy_cat("close();");
}

game_system_provider *dummy_provider(void) {
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

#include <stdlib.h>
#include "rule.h"
#include "solitaire_maltesercross.h"

typedef struct {
	char state;

	pile* deck;

	/* The four piles where we draw cards from. */
	pile* src1;
	pile* src2;
	pile* src3;
	pile* src4;

	/* The four diagonal piles where you build
	 * from king to ace. */
	pile* king1;
	pile* king2;
	pile* king3;
	pile* king4;

	/* Center pile build from ace to king. */
	pile* center;

	/* Build from any card up. */
	pile* build1;
	pile* build2;
	pile* build3;
	pile* build4;

	/* All cards are moved to this pile. */
	pile* done;

	pile* pile1;
	pile* pile2;
	pile* pile3;
	pile* pile4;
	pile* pile5;

	ruleset *ruleset;
} internal;

static void my_deal(solitaire* sol, visual_pile* pile) {
	internal* i = sol->data;

	if(i->state == 0) {
		card_move_count(i->src1, i->deck, 13);
		card_move_count(i->src2, i->deck, 13);
		card_move_count(i->src3, i->deck, 13);
		card_move_count(i->src4, i->deck, 13);

		card_reveal(i->src1->cards[12]);
		card_reveal(i->src2->cards[12]);
		card_reveal(i->src3->cards[12]);
		card_reveal(i->src4->cards[12]);
		i->state++;
	}

	if(i->state == 1) {
		/* Dealing 5 cards to the bottom 5 columns. This state is valid
		   until the deck is empty.
		*/

		if(card_count(i->deck) == 0) {
			i->state = 2;
			card_move_all(i->deck, i->pile1);
			card_move_all(i->deck, i->pile2);
			card_move_all(i->deck, i->pile3);
			card_move_all(i->deck, i->pile4);
			card_move_all(i->deck, i->pile5);
			card_hide_all(i->deck);
		}
		else {
			card_append(card_take_last(i->deck), i->pile1);
			if(card_count(i->deck) > 0)
				card_append(card_take_last(i->deck), i->pile2);
			if(card_count(i->deck) > 0)
				card_append(card_take_last(i->deck), i->pile3);
			if(card_count(i->deck) > 0)
				card_append(card_take_last(i->deck), i->pile4);
			if(card_count(i->deck) > 0)
				card_append(card_take_last(i->deck), i->pile5);

			card_reveal_all(i->pile1);
			card_reveal_all(i->pile2);
			card_reveal_all(i->pile3);
			card_reveal_all(i->pile4);
			card_reveal_all(i->pile5);
		}
	}
	else if(i->state == 2) {
		if(card_count(i->deck) == 0) {
			i->state = 3;
			card_move_all(i->deck, i->pile1);
			card_move_all(i->deck, i->pile2);
			card_move_all(i->deck, i->pile3);
			card_move_all(i->deck, i->pile4);
			card_move_all(i->deck, i->pile5);
			card_hide_all(i->deck);
		}
		else {
			card_append(card_take_last(i->deck), i->pile1);
			if(card_count(i->deck) > 0)
				card_append(card_take_last(i->deck), i->pile2);
			if(card_count(i->deck) > 0)
				card_append(card_take_last(i->deck), i->pile3);
			if(card_count(i->deck) > 0)
				card_append(card_take_last(i->deck), i->pile4);

			card_reveal_all(i->pile1);
			card_reveal_all(i->pile2);
			card_reveal_all(i->pile3);
			card_reveal_all(i->pile4);
		}
	}
	else if(i->state == 3) {
		if(card_count(i->deck) == 0) {
			i->state = 4;
			card_move_all(i->deck, i->pile1);
			card_move_all(i->deck, i->pile2);
			card_move_all(i->deck, i->pile3);
			card_move_all(i->deck, i->pile4);
			card_hide_all(i->deck);
		}
		else {
			card_append(card_take_last(i->deck), i->pile1);
			if(card_count(i->deck) > 0)
				card_append(card_take_last(i->deck), i->pile2);
			if(card_count(i->deck) > 0)
				card_append(card_take_last(i->deck), i->pile3);

			card_reveal_all(i->pile1);
			card_reveal_all(i->pile2);
			card_reveal_all(i->pile3);
		}
	}
	else if(i->state == 4) {
		if(card_count(i->deck) == 0) {
			i->state = 5;
			card_move_all(i->deck, i->pile1);
			card_move_all(i->deck, i->pile2);
			card_move_all(i->deck, i->pile3);
			card_hide_all(i->deck);
		}
		else {
			card_append(card_take_last(i->deck), i->pile1);
			if(card_count(i->deck) > 0)
				card_append(card_take_last(i->deck), i->pile2);

			card_reveal_all(i->pile1);
			card_reveal_all(i->pile2);
		}
	}
	else if(i->state == 5) {
		if(card_count(i->deck) == 0) {
			i->state = 6;
		}
		else {
			card_append(card_take_last(i->deck), i->pile1);
			card_reveal_all(i->pile1);
		}
	}

	visual_sync(sol->visual);
}

static bool my_append_to_pile(solitaire *sol, visual_pile *dest, card_proxy *card) {
	internal* i = sol->data;
	return ruleset_move_card(i->ruleset, sol->visual, dest, card);
}

solitaire* solitaire_maltesercross(mem_context *context, visual_settings *settings) {
	visual_pile *deck, *done, *king1, *king2, *king3, *king4;
	visual_pile *src1, *src2, *src3, *src4;
	visual_pile *center, *build1, *build2, *build3, *build4;
	visual_pile *pile1, *pile2, *pile3, *pile4, *pile5;
	condition *build_pile_cond, *king_pile_cond;
	rule *rule1, *rule2, *rule3, *rule4, *rule5, *rule6;

	/* The one solitaire instance we have.*/
	solitaire* s = mem_alloc(context, sizeof(solitaire));

	/* This is the internal data representation of this
	 * solitaire. This is a local struct hidden from other
	 * members. */
	internal* i = mem_alloc(context, sizeof(internal));
	s->data = i;
	s->visual = visual_create(context, settings);

	i->deck = pile_create(context, 104);
	deck = visual_pile_create(context, i->deck);
	deck->origin[0] = 0 - (settings->card_width + settings->card_spacing * 2 + settings->card_height * 2);
	deck->origin[1] = settings->card_width;
	deck->rotation = -30.0f;
	deck->pile_action = my_deal;
	visual_add_pile(context, s->visual, deck);

	i->done = pile_create(context, 104);
	done = visual_pile_create(context, i->done);
	done->origin[0] = settings->card_width + settings->card_spacing * 2 + settings->card_height * 2;
	done->origin[1] = 0 - settings->card_width;
	done->rotation = -30.0f;
	visual_add_pile(context, s->visual, done);

	i->center = pile_create(context, 13);
	center = visual_pile_create(context, i->center);
	visual_add_pile(context, s->visual, center);

	i->king1 = pile_create(context, 13);
	king1 = visual_pile_create(context, i->king1);
	king1->origin[0] = settings->card_width / 2 + settings->card_spacing * 2 + settings->card_height / 2;
	king1->origin[1] = settings->card_width / 2 + settings->card_spacing * 2 + settings->card_height / 2;
	king1->rotation = -45.0f;
	visual_add_pile(context, s->visual, king1);

	i->king2 = pile_create(context, 13);
	king2 = visual_pile_create(context, i->king2);
	king2->origin[0] = 0 - (settings->card_width / 2 + settings->card_spacing * 2 + settings->card_height / 2);
	king2->origin[1] = settings->card_width / 2 + settings->card_spacing * 2 + settings->card_height / 2;
	king2->rotation = 45.0f;
	visual_add_pile(context, s->visual, king2);

	i->king3 = pile_create(context, 13);
	king3 = visual_pile_create(context, i->king3);
	king3->origin[0] = settings->card_width / 2 + settings->card_spacing * 2 + settings->card_height / 2;
	king3->origin[1] = 0 - (settings->card_width / 2 + settings->card_spacing * 2 + settings->card_height / 2);
	king3->rotation = 45.0f;
	visual_add_pile(context, s->visual, king3);

	i->king4 = pile_create(context, 13);
	king4 = visual_pile_create(context, i->king4);
	king4->origin[0] = 0 - (settings->card_width / 2 + settings->card_spacing * 2 + settings->card_height / 2);
	king4->origin[1] = 0 - (settings->card_width / 2 + settings->card_spacing * 2 + settings->card_height / 2);
	king4->rotation = -45.0f;
	visual_add_pile(context, s->visual, king4);

	i->src1 = pile_create(context, 13);
	src1 = visual_pile_create(context, i->src1);
	src1->origin[1] = settings->card_height + settings->card_spacing;
	visual_add_pile(context, s->visual, src1);

	i->src2 = pile_create(context, 13);
	src2 = visual_pile_create(context, i->src2);
	src2->origin[1] = 0 - (settings->card_height + settings->card_spacing);
	visual_add_pile(context, s->visual, src2);

	i->src3 = pile_create(context, 13);
	src3 = visual_pile_create(context, i->src3);
	src3->origin[0] = 0 - (settings->card_width / 2 + settings->card_spacing + settings->card_height / 2);
	src3->rotation = 90.0f;
	visual_add_pile(context, s->visual, src3);

	i->src4 = pile_create(context, 13);
	src4 = visual_pile_create(context, i->src4);
	src4->origin[0] = settings->card_width / 2 + settings->card_spacing + settings->card_height / 2;
	src4->rotation = 90.0f;
	visual_add_pile(context, s->visual, src4);

	i->build1 = pile_create(context, 13);
	build1 = visual_pile_create(context, i->build1);
	build1->origin[0] = 0 - (settings->card_width + settings->card_spacing * 2 + settings->card_height);
	visual_add_pile(context, s->visual, build1);

	i->build2 = pile_create(context, 13);
	build2 = visual_pile_create(context, i->build2);
	build2->origin[0] = settings->card_width + settings->card_spacing * 2 + settings->card_height;
	visual_add_pile(context, s->visual, build2);

	i->build3 = pile_create(context, 13);
	build3 = visual_pile_create(context, i->build3);
	build3->origin[1] = settings->card_height / 2 + settings->card_spacing * 2 + settings->card_height + settings->card_width / 2;
	build3->rotation = 90.0f;
	visual_add_pile(context, s->visual, build3);

	i->build4 = pile_create(context, 13);
	build4 = visual_pile_create(context, i->build4);
	build4->origin[1] = 0 - (settings->card_height / 2 + settings->card_spacing * 2 + settings->card_height + settings->card_width / 2);
	build4->rotation = 90.0f;
	visual_add_pile(context, s->visual, build4);

	i->pile1 = pile_create(context, 52);
	pile1 = visual_pile_create(context, i->pile1);
	pile1->origin[0] = 0 - (settings->card_spacing * 2 + settings->card_width * 2);
	pile1->origin[1] = 0 - (settings->card_height / 2 + settings->card_spacing * 3 + settings->card_height + settings->card_width + settings->card_height / 2);
	visual_add_pile(context, s->visual, pile1);

	i->pile2 = pile_create(context, 52);
	pile2 = visual_pile_create(context, i->pile2);
	pile2->origin[0] = 0 - (settings->card_spacing + settings->card_width);
	pile2->origin[1] = pile1->origin[1];
	visual_add_pile(context, s->visual, pile2);

	i->pile3 = pile_create(context, 52);
	pile3 = visual_pile_create(context, i->pile3);
	pile3->origin[0] = 0;
	pile3->origin[1] = pile1->origin[1];
	visual_add_pile(context, s->visual, pile3);

	i->pile4 = pile_create(context, 52);
	pile4 = visual_pile_create(context, i->pile4);
	pile4->origin[0] = settings->card_spacing + settings->card_width;
	pile4->origin[1] = pile1->origin[1];
	visual_add_pile(context, s->visual, pile4);

	i->pile5 = pile_create(context, 52);
	pile5 = visual_pile_create(context, i->pile5);
	pile5->origin[0] = settings->card_spacing * 2 + settings->card_width * 2;
	pile5->origin[1] = pile1->origin[1];
	visual_add_pile(context, s->visual, pile5);

	create_deck(context, i->deck, 1);
	create_deck(context, i->deck, 1);

	card_shuffle(i->deck);

	visual_sync(s->visual);

	i->ruleset = create_ruleset(context);

	/* Shared condition of a build pile destination */
	build_pile_cond = condition_or(
		context,
		condition_or(
			context,
			condition_or(
				context,
				condition_destination(context, i->build1),
				condition_destination(context, i->build2)),
			condition_destination(context, i->build3)),
		condition_destination(context, i->build4));

	king_pile_cond = condition_or(
		context,
		condition_or(
			context,
			condition_or(
				context,
				condition_destination(context, i->king1),
				condition_destination(context, i->king2)),
			condition_destination(context, i->king3)),
		condition_destination(context, i->king4));

	/* Allow moving any card to an empty pile outside of the four source cards. */
	rule1 = create_rule(context);
	rule_add_condition(context, rule1, condition_destination_empty(context));
	rule_add_condition(context, rule1, condition_top_card(context));
	rule_add_condition(context, rule1, build_pile_cond);
	rule_add_action(context, rule1, action_reveal_source_top_card(context));
	ruleset_add_rule(context, i->ruleset, rule1);

	/* Allow building of cards following suit on build piles. */
	rule2 = create_rule(context);
	rule_add_condition(context, rule2, build_pile_cond);
	rule_add_condition(context, rule2, condition_top_card(context));
	rule_add_condition(
		context, rule2, condition_top_card_compare(
			context, 0, e_follow_suit|e_dest_1lower_value));
	rule_add_action(context, rule2, action_reveal_source_top_card(context));
	ruleset_add_rule(context, i->ruleset, rule2);

	/* Allow a king to be moved to the king pile. */
	rule3 = create_rule(context);
	rule_add_condition(context, rule3, king_pile_cond);
	rule_add_condition(context, rule3, condition_top_card(context));
	rule_add_condition(context, rule3, condition_destination_empty(context));
	rule_add_condition(
		context, rule3,
		condition_top_card_equal(context, e_suit_none, 13, e_equal_value));
	rule_add_action(context, rule3, action_reveal_source_top_card(context));
	ruleset_add_rule(context, i->ruleset, rule3);

	/* Allow building of cards in descending order on king piles and
	   following suit. */
	rule4 = create_rule(context);
	rule_add_condition(context, rule4, king_pile_cond);
	rule_add_condition(context, rule4, condition_top_card(context));
	rule_add_condition(
		context, rule4, condition_top_card_compare(
			context, 0, e_follow_suit|e_dest_1higher_value));
	rule_add_action(context, rule4, action_reveal_source_top_card(context));
	ruleset_add_rule(context, i->ruleset, rule4);

	/* Allow an ace to be placed in the center pile if empty. */
	rule5 = create_rule(context);
	rule_add_condition(context, rule5, condition_destination_empty(context));
	rule_add_condition(context, rule5, condition_destination(context, i->center));
	rule_add_condition(context, rule5, condition_top_card(context));
	rule_add_condition(
		context, rule5,
		condition_top_card_equal(context, e_suit_none, 1, e_equal_value));
	rule_add_action(context, rule5, action_reveal_source_top_card(context));
	ruleset_add_rule(context, i->ruleset, rule5);

	/* Allow to build on the ace pile up to king in the same suit. */
	rule6 = create_rule(context);
	rule_add_condition(context, rule6, condition_destination(context, i->center));
	rule_add_condition(context, rule6, condition_top_card(context));
	rule_add_condition(
		context, rule6, condition_top_card_compare(
			context, 0, e_follow_suit|e_dest_1lower_value));
	rule_add_action(context, rule6, action_reveal_source_top_card(context));

	/* Add our implementation for the common functionality
	 * shared by all solitaires. */
	s->append_to_pile = my_append_to_pile;
	return s;
}

====
TODO
====

Build system improvements
=========================

Ninja
-----

How does this work together with clang and analysis there?

Proper dependencies of files when building
------------------------------------------

UI
==

Instructions for each solitaire
-------------------------------

Help the user on how each game works.

Reflection effect on button widget
----------------------------------

This looks nice and polished

Keyboard handling
-----------------

Focus handling
--------------

Options window
--------------

* Default deck configuration
* background

Background: Growing grass
-------------------------

Background: Fixed SVG
---------------------

Generic message box
-------------------

For messages and errors. Also used for yes|no|cancel.

Slidebar control
----------------

Checkbox control
----------------

TextBox control
---------------

Animation of cards between positions
------------------------------------

Own mouse pointer
-----------------

Could be implemented using a rendering object applied last to the tree of nodes.
We also need to attach a mouse movement handler.
This can be used to hide the cursor: glutSetCursor(GLUT_CURSOR_NONE);

Angle on camera controlled by slidebar
--------------------------------------

Random rotation of each card
----------------------------

Undo of last action
-------------------

State transitions
-----------------

TODO split this up in several smaller tasks!

* Base style that defines a widget without any state set.
* Style for a state. A state can be something like::

    -- hover
    -- focus
    -- selected
    -- closing

* Each set property in state style can have three transition expressions set::

    -- One for changing to the state.
    -- Continous state (an animated button for example)
    -- One for leaving the state.

* Each property has a option telling what operation it is in::

    -- entering state
    -- looping
    -- leaving state

* Events when state reaches different operation::

    -- Window is removed when fading out effect has been completed. User clicks
       close. We then changes state to "closing". When that state has finished
       its entering state (fade out). We then close the window.
    -- Events called after the main loop rendering.


Some code::

    typedef enum {
            e_entering_state,
            e_continuous_state,
            e_leaving_state
    } widget_operation;

    /* Enqueu a state change. */
    void widget_add_state(render_object *widget, const char *state);

    /* Set the leaving state operation for a widget. */
    void widget_remove_state(render_object *widget, const char *state);

    /* Returns true if the widget or any subwidget has the state active in any transition. */
    bool widget_state_active(render_object *widget, const char *state);

    typedef struct {
        widget_operation op;
        float length;
        expression *exp;
    } transition;

    transition *t = widget_transition_create(e_entering_state, 3.0f, expression_parse(""));
    widget_style_add_transition(style, "hover", "backcolor_red", t);

Rotation for cards
------------------

+-2 degrees or something like that. Should be remembered
for each card between renderings. Sugest an array in the visual pile struct with some
predefined rotations when the pile is created.

Hint: Indication when only a few cards left in a pile
-----------------------------------------------------

Hint: Show which pile that is dealing cards
-------------------------------------------

Use an svg image for this.

Server application for multiuser plays
======================================

Login / logout
chatting

Multiplayer card games
======================

Poker
-----

Solitaires
==========

Pyramid
-------

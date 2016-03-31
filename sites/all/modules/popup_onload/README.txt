
Popup On Load README

CONTENTS OF THIS FILE
----------------------

  * Introduction
  * Dependencies
  * Hooks


INTRODUCTION
------------
The module allows to display popup after a certain delay on a page load.
This fills a gap in popup modules family (Popups, Popup), which are designed
to show popups only after a click on an HTML element, or to implement
a complex behavior like modal dialog forms.

If you want to just display a popup after a page load, the Popup On Load
module possibly fits your needs. It does some very simple things:

1. Creates PopupOnLoad entity type, so that popups-on-load are separated
   from another content in database and administrative interface.

2. Provides some basic configuration options, which can be extended by other
   modules via hooks and standard Drupal form alter functions.

3. Provides an action for popup on load displaying. The action can be used as
   a result of rule evaluation.


DEPENDENCIES
------------
The module depends on following modules:

1. Entity API (http://drupal.org/project/entity)

2. Libraries API (http://drupal.org/project/libraries)

...and uses Colorbox (http://www.jacklmoore.com/colorbox) library.


HOOKS
-----
The module provides and uses three hooks:

- hook_popup_onload_check_display_conditions()
- hook_popup_onload_sort_methods()
- hook_popup_onload_js_settings_alter()

CONTENTS OF THIS FILE
---------------------

 * Introduction
 * Information
 * Dependencies
 * Installation
 * Support

INTRODUCTION
------------

Current Maintainer: Daniel Imhoff <dwieeb@gmail.com>

This module will allow webform creators to specify a start and end date for
webforms. It has Date Popup integration for easy date and time selection.

INFORMATION
-----------

It adds an optional start date and end date field to the webform form settings.
If the current date and time falls outside the specified window, the webform is
disabled. 

DEPENDENCIES
------------

- Webform (http://drupal.org/project/webform)
- Date API (provided by http://drupal.org/project/date)
- (optional) Date Popup (provided by http://drupal.org/project/date)

INSTALLATION
------------

1. Make sure the Date API is enabled by installing Date
   (http://drupal.org/project/date) and enabling it.

2. Also, you may optionally enable the Date Popup module.

3. Copy the entire webform_scheduler directory into sites/all/modules directory.

4. Login as an administrator. Enable the module in the "Administer" -> "Modules"

5. Setup your webforms by editing a webform node and going to the Webform tab,
   and then clicking Form settings, scrolling down to Scheduler.

SUPPORT
-------

Please use the issue queue for filing bugs with this module at
http://drupal.org/project/issues/1700606

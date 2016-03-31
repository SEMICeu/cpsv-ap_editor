DRUPAL SPLASHIFY MODULE
------------------------
Maintainers:
  Chris Roane (http://drupal.org/user/1283000)
Requires - Drupal 7, Library and jStorage (jQuery plugin)
License - GPL (see LICENSE)


1.0 OVERVIEW
-------------
Splashify is a full featured splash page module that is designed to be search
engine friendly. It is originally based on the Drupal 6 Splash module. It
allows you to specify a page to be displayed anywhere on your site, using one
of a few different delivery options (redirect, popup window or ColorBox). The
module also provides options specific to mobile devices.

The main focus of this module is the following:
- Be search engine friendly by redirecting via JavaScript (when applicable).
- Allow specific options for mobile devices.
- Allow different ways in delivering the splash page.
- Use ColorBox for displaying the splash page in a lightbox.
- You can have the system pick from a random list of splash pages, have a
list of splash pages show up in a specified order, display the specified
text/html in the site template or display the text/html full screen.

All of the features of this module have been confirmed to work in FF, Chrome,
Safari and IE7 through IE9.


2.0 INSTALLATION
-----------------
1. Download and enable the "Libraries" Drupal module. Version 2.0 or higher is
required.
Link: http://drupal.org/project/libraries

2. Download the jStorage library: https://github.com/andris9/jStorage/zipball/master
You will want to put the files in one of the following directories:
/sites/all/libraries/jstorage/
/sites/[site_name]/libraries/jstorage/

Splashify will be looking for "jstorage.min.js". So it should look something
like /sites/all/libraries/jstorage/jstorage.min.js

Below is a direct link to download the zip file.
Link: https://github.com/andris9/jStorage/zipball/master

3. If you want to enable options for mobile devices, you will need to add
the Mobile Detect library files: http://mobiledetect.net/

You can put the files into any of the following directories:
/sites/all/libraries/Mobile_Detect/
/sites/default/libraries/Mobile_Detect/
/sites/[YOUR_DOMAIN.COM]/libraries/Mobile_Detect/

4. Download and enable the latest version of the Splashify module.
Link: http://drupal.org/project/splashify

5. Verify there are no splashify errors on the Status report page
(admin/reports/status).

6. Configure the module. If the system can't find the jStorage library, it will
display an error.
Link: /config/system/splashify


2.1 CONFIGURATION
------------------
Go to "Configuration" -> "System" -> "Splashify" to find all the configuration
options.

If you are using the mobile options, you need to make sure to configure all
of the mobile settings on each tab for the mobile splash to work properly.

2.2 MOBILE SPLASH
------------------
In order to use the mobile splash options, the system must be able to find the
Mobile_Detect.php file from the Mobile Detect library:
http://mobiledetect.net/

Below are a few options in where you can put the file:
/sites/all/libraries/Mobile_Detect/Mobile_Detect.php
/sites/default/libraries/Mobile_Detect/Mobile_Detect.php
/sites/[YOUR_DOMAIN.COM]/libraries/Mobile_Detect/Mobile_Detect.php

One this file is there, you should see the Mobile settings appear on the When
tab of the Splashify config area: /admin/config/system/splashify

3.0 PROBLEMS OR FEATURE REQUESTS
---------------------------------
First make sure an issue doesn't already exist. If it doesn't, create a new
issue: http://drupal.org/project/issues/splashify


4.0 TODO
--------
- Need to do more in depth mobile browser testing.


LAST UPDATED
-------------
09/09/2012


SPONSORS
--------
This module has been sponsored by The Brick Factory (thebrickfactory.com).

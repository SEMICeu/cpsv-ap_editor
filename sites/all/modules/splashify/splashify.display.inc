<?php

/**
 * @file
 * Handles displaying the splash page.
 *
 * Either creates the JS for handling the splash page (redirect, lightbox or
 * popup) or displays the What: Content full screen or in the site design.
 */

/**
 * Implements hook_init().
 *
 * Generates the JS for the redirect, lightbox or popup window.
 */
function splashify_init() {
  global $user;
  global $base_url;

  // Load the jStorage library.
  $library = libraries_load('jstorage', 'minified');

  // If this is the cron script or drush, do not run the splashify code.
  $drush_check = (function_exists('drush_verify_cli') && call_user_func('drush_verify_cli')) ? TRUE : FALSE;
  $cron_check = (strpos($_SERVER['PHP_SELF'], 'cron.php') !== FALSE) ? TRUE : FALSE;
  if ($drush_check || $cron_check) {
    return;
  }

  /*
   * Step #1: Retrieve the admin settings.
   */

  $config = _splashify_get_config();

  $only_anonymous = $config['only_anonymous'];
  $when_frequency = $config['when_frequency'];
  $where_page = $config['where_page'];
  $where_listpages = $config['where_listpages'];
  $where_opposite = $config['where_opposite'];
  $what_mode = $config['what_mode'];
  $what_content_value = $config['what_content_value'];
  $how_mode = $config['how_mode'];
  $how_size = $config['how_size'];

  /*
   * Step #2: Should we display the splash page?
   */
  if ($only_anonymous && $user->uid > 0) {
    return;
  }

  if ($when_frequency == 'never') {
    return;
  }
  
  // Set variable whether to show page, default is true
  $is_role = TRUE;
  // Check whether use role setting is checked
  if($config['when_roles']){
    // If use role setting is checked set variable not to show.
    $is_role = FALSE;
    // Check if any roles match and then sat show to true.
    foreach($user->roles as $v) {
      if(in_array($v, $config['when_roles_options'])) {
        $is_role = TRUE;
      }
    }
  }
  
  // Check the outcome of above and return if no roles match
  if (!$is_role) {
    return;
  }

  // Default to not showing the splash page.
  $splash_display = FALSE;

  $splash_get_var = '';
  if (isset($_GET['splash'])) {
    $splash_get_var = $_GET['splash'];
  }

  $splash_correct_page = FALSE;
  switch ($where_page) {
    case 'all':
      // Display on all pages.
      $splash_correct_page = TRUE;
      break;

    case 'home':
      $home_page = drupal_get_normal_path(variable_get('site_frontpage', 'node'));
      if ($where_opposite) {
        if ($_GET['q'] != $home_page) {
          // Display on every page except the home page.
          $splash_correct_page = TRUE;
        }
      }
      else {
        if ($_GET['q'] == $home_page) {
          // Display on the home page.
          $splash_correct_page = TRUE;
        }
      }
      break;

    case 'list':
      $list_paths = drupal_strtolower($where_listpages);
      $path = drupal_strtolower(drupal_get_path_alias($_GET['q']));
      $splash_correct_page = drupal_match_path($path, $list_paths);
      if ($path != $_GET['q']){
	      $splash_correct_page = $splash_correct_page || drupal_match_path($_GET['q'], $list_paths);
      }

      $splash_correct_page = ($splash_correct_page xor $where_opposite);
      break;
  }

  /*
   * Goes through the main server side checks to see if we should show the
   * splash page.
   */

  if (empty($what_content_value)) {
    // The What: Content field is not specified.
    $splash_display = FALSE;
  }
  elseif ($splash_get_var == 'off') {
    // Special case for preventing the splash page from showing up.
    $splash_display = FALSE;
  }
  elseif ($splash_get_var == 'on') {
    // Special case for forcing the splash page to show up.
    $splash_display = TRUE;

    // Modify this value to force the splash to show up in this case.
    $when_frequency = 'always';
  }
  elseif ($splash_correct_page) {
    // We need to show the splash page on this page. Do one last check.
    $parsed_url = parse_url($base_url);
    // We passed all of the tests...display the splash page!
    $splash_display = TRUE;
  }

  // If we shouldn't display the splash, end the code here.
  if (!$splash_display) {
    return;
  }

  /*
   * Step #3: Display the Splash Page
   *
   * At this point, we know that we should display the splash page.
   */

  // Controls when the jStorage variable should expire next. In other words,
  // when should the splash page show up again?
  $js_expiretime = '';

  // Since the time is based on server time, we need to pass this value to
  // the JS.
  $js_nowtime = time();

  // This variable is a way to always show the splash in the JS.
  $js_splash_always = FALSE;

  switch ($when_frequency) {
    case 'once':
      // Set to expire in one year.
      $js_expiretime = time() + (365 * 24 * 60 * 60);
      break;

    case 'daily':
      // Set to expire in 24 hours.
      $js_expiretime = time() + (24 * 60 * 60);
      break;

    case 'weekly':
      // Set to expire in 7 days.
      $js_expiretime = time() + (7 * 24 * 60 * 60);
      break;

    case 'always':
      // This should make the splash always show up on the next page load.
      $js_expiretime = time();
      $js_splash_always = TRUE;
      break;
  }

  // Deal with the mode settings.
  $js_mode_settings = array();
  $js_mode_settings['mode'] = $what_mode;
  $js_mode_settings['system_splash'] = '';
  $js_mode_settings['urls'] = '';

  if ($what_mode == 'template' || $what_mode == 'fullscreen') {
    // We need to redirect to the System generated splash page. Define the
    // action url. This variable tells the JS that we only are dealing with one
    // url.
    $js_mode_settings['system_splash'] = url('splashify-splash');
  }
  elseif ($what_mode == 'sequence' || $what_mode == 'random') {
    // Split up the textarea field by lines.
    $what_paths = preg_split('/[\n\r]+/', $what_content_value);

    // This holds all of the url values in the entered order.
    $js_mode_settings['urls'] = array();
    $js_mode_settings['total_urls'] = count($what_paths);
    foreach ($what_paths as $path) {
      $js_mode_settings['urls'][] = url(trim($path));
    }
  }

  // Define the last remaining JS variables we need to send.
  $size_action = $how_size ? explode('x', $how_size) : FALSE;
  $js_mode = '';
  switch ($how_mode) {
    case 'redirect':
      // Redirect to a different url.
      $js_mode = 'redirect';
      break;

    case 'window':
      // Open up a popup window.
      $js_mode = 'window';
      $js_mode_settings['size'] = $size_action ? 'width=' . $size_action[0] . ',height=' . $size_action[1] : '';
      break;

    case 'lightbox':
      if (module_exists('colorbox')) {
        // Display a ColorBox.
        $js_mode = 'colorbox';

        // Get the size of the lightbox.
        if (count($size_action) > 1) {
          $colorbox_width = $size_action[0];
          $colorbox_height = $size_action[1];
        }
        else {
          // Default size settings for the colorbox.
          $colorbox_width = 800;
          $colorbox_height = 600;
        }

        $js_mode_settings['size_width'] = $colorbox_width;
        $js_mode_settings['size_height'] = $colorbox_height;
      }
      else {
        return;
      }

      break;

    default:
      // Do nothing! This is a fail safe.
      return;
  }


  /*
   * Finally: Include the JS that puts it all together!
   *
   * At this point the page passed all of the server side checks. We now
   * implement JS code that checks if the splash page should show up, based
   * on when it last showed up. It then executes the JS action code, based
   * on the specified settings.
   */

  // Make our splash settings variables available to our JavaScript.
  $js_settings = array(
    'js_splash_always' => $js_splash_always ? '1' : '0',
    'js_expire_after' => $js_expiretime - $js_nowtime,
    'js_mode' => $js_mode,
    'js_mode_settings' => $js_mode_settings,
    'js_disable_referrer_check' => variable_get('disable_referrer_check', 0),
  );

  drupal_add_js(array('splashify' => $js_settings), array(
    'type' => 'setting',
    'cache' => TRUE,
    'weight' => -100,
    'every_page' => TRUE,
  ));

  // Include the main JS file that does the heavy lifting.
  $js_splashify_int = drupal_get_path('module', 'splashify') . '/js/splashify_init_min.js';
  drupal_add_js($js_splashify_int, array(
    'type' => 'file',
    'scope' => 'header',
    'group' => JS_THEME,
    'cache' => TRUE,
    'weight' => -99,
    'every_page' => TRUE,
  ));
}

/**
 * Display the What: Content either in the site template or full screen.
 *
 * @return string
 *   If the $what_mode equals 'template', returns the string entered into
 *   $what_content_value. If $what_mode equals 'fullscreen', it displays
 *   the text right there and exits the request.
 */
function splashify_display_splashtext() {
  $config = _splashify_get_config();

  $what_mode = $config['what_mode'];
  $what_content_value = $config['what_content_value'];
  $what_content_title = $config['what_content_title'];

  if ($what_mode == 'template') {
    // Display what they entered in the field in the site template.
    if (!empty($what_content_title)) {
      drupal_set_title($what_content_title);
    }

    return $what_content_value;
  }
  elseif ($what_mode == 'fullscreen') {
    // Display what they entered in the field as a full page.
    echo $what_content_value;
    drupal_exit();
  }
}

/**
 * Retrieves the admin settings.
 *
 * Will either retrieve the desktop or mobile settings, depending whether this
 * is a mobile request or not.
 *
 * @return array
 *   Returns an associate array with the following keys: when_frequency,
 *   when_mobile_test, where_page, where_listpages, what_mode, what_content,
 *   what_content_value, how_mode, and how_size.
 */
function _splashify_get_config() {
  // Control whether user roles are being used or not
  $when_roles = variable_get('splashify_when_roles', '');
  
  // If roles are enabled get the options
  if($when_roles){
    $when_roles_options = variable_get('splashify_when_roles_options', '');
      foreach($when_roles_options as $k => $v) {
        if ($v === 0) {
          unset($when_roles_options[$k]);
        }
      }
    }
  else{
    $when_roles_options = 0;
  }
  
  // This controls whether mobile is enabled or not.
  $is_mobile_enabled = variable_get('splashify_when_mobile', 0);

  if ($is_mobile_enabled) {
    // Verify the Mobile Detect library is installed.
    $mobile_detect = libraries_load('Mobile_Detect');
    if (!$mobile_detect['installed'] || !file_exists(DRUPAL_ROOT . '/' . $mobile_detect['library path'] . '/Mobile_Detect.php')) {
      $is_mobile_enabled = 0;
    }
  }

  // This controls whether or not we should use the mobile settings for this
  // request.
  $do_mobile = FALSE;

  // If this admin setting is true, force the system to use the mobile
  // settings.
  $force_mobile = variable_get('splashify_when_mobile_test', FALSE);

  if (!empty($is_mobile_enabled)) {

    // Is this a mobile request?
    $detect = new Mobile_Detect();
    if ($detect->isMobile() || $force_mobile) {
      // This is a mobile request!
      $do_mobile = TRUE;

      // Get the mobile admin settings.
      $when_frequency = variable_get('splashify_when_mobile_frequency', 'never');
      $where_page = variable_get('splashify_where_mobile_page', 'home');
      $where_listpages = variable_get('splashify_where_mobile_listpages', '');
      $where_opposite = variable_get('splashify_where_mobile_opposite', FALSE);
      $what_mode = variable_get('splashify_what_mobile_mode', 'random');
      $what_content = variable_get('splashify_what_mobile_content', '');
      $what_content_title = variable_get('splashify_what_mobile_pagetitle', '');
      $how_mode = variable_get('splashify_how_mobile_mode', 'redirect');

      // This is no longer an option for the mobile settings.
      $how_size = '';
    }
  }

  if (!$do_mobile) {
    // Get the desktop admin settings.
    $when_frequency = variable_get('splashify_when_desktop_frequency', 'never');
    $where_page = variable_get('splashify_where_desktop_page', 'home');
    $where_listpages = variable_get('splashify_where_desktop_listpages', '');
    $where_opposite = variable_get('splashify_where_desktop_opposite', FALSE);
    $what_mode = variable_get('splashify_what_desktop_mode', 'random');
    $what_content = variable_get('splashify_what_desktop_content', '');
    $what_content_title = variable_get('splashify_what_desktop_pagetitle', '');
    $how_mode = variable_get('splashify_how_desktop_mode', 'redirect');
    $how_size = variable_get('splashify_how_desktop_size', '');
  }

  $only_anonymous = variable_get('splashify_when_anonymous', FALSE);

  // This is a textarea field, so make sure this value is safe.
  $where_listpages = check_plain($where_listpages);

  if (is_array($what_content)) {
    // We are dealing with a text_format field.
    $what_content_value = check_markup($what_content['value'], $what_content['format']);
  }
  elseif (isset($what_content)) {
    // We are dealing with a textarea field.
    $what_content_value = check_plain($what_content);
  }
  else {
    $what_content_value = '';
  }

  return array(
    'only_anonymous' => $only_anonymous,
    'when_frequency' => $when_frequency,
    'when_mobile_test' => $force_mobile,
    'when_roles' => $when_roles,
    'when_roles_options' => $when_roles_options,
    'where_page' => $where_page,
    'where_listpages' => html_entity_decode($where_listpages),
    'where_opposite' => $where_opposite,
    'what_mode' => $what_mode,
    'what_content' => $what_content,
    'what_content_value' => $what_content_value,
    'what_content_title' => $what_content_title,
    'how_mode' => $how_mode,
    'how_size' => $how_size,
  );
}

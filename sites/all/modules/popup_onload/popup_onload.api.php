<?php

/**
 * @file
 * Hooks provided by the Popup On Load module.
 */

/**
 * Provide sort methods for popups.
 *
 * Must return an array of 'callback' => 'human readable name' pairs.
 * Each callback must return a single popup, which will be displayed.
 *
 * @see popup_onload_sort_methods()
 */
function hook_popup_onload_sort_methods() {
  return array(
    'mymodule_callback' => t('Sort method'),
  );
}

/**
 * Check popup display conditions.
 *
 * Should return TRUE or FALSE. If at least one FALSE value is returned
 * from any module, popup is not displayed.
 *
 * @see popup_onload_check_display_conditions()
 */
function hook_popup_onload_check_display_conditions($popup_onload) {
  return !popup_onload_check_time_cookie();
}

/**
 * Alter colorbox JS settings right before the popup is displayed.
 */
function hook_popup_onload_js_settings_alter(&$popup_settings, &$popup_onload) {
  // Disable closing popup colorbox on 'esc' key press.
  $popup_settings['escKey'] = FALSE;
}

<?php
/**
 * @file
 * popup_onload_display.rules_defaults.inc
 */

/**
 * Implements hook_default_rules_configuration().
 */
function popup_onload_display_default_rules_configuration() {
  $items = array();
  $items['rules_display_popup_onload'] = entity_import('rules_config', '{ "rules_display_popup_onload" : {
      "LABEL" : "Display a popup on page load",
      "PLUGIN" : "reaction rule",
      "REQUIRES" : [ "popup_onload" ],
      "ON" : [ "popup_onload_event_choose_popup" ],
      "IF" : [ { "popup_onload_condition_check_cookie" : { "popup_onload" : [ "popup_onload" ] } } ],
      "DO" : [ { "popup_onload_action_display_popup" : { "popup_onload" : [ "popup_onload" ] } } ]
    }
  }');
  return $items;
}

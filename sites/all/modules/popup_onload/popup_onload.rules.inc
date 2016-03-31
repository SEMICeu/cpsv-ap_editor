<?php

/**
 * @file
 * Rules-related hooks and functions.
 */

/**
 * Implements hook_rules_event_info().
 */
function popup_onload_rules_event_info() {
  return array(
    'popup_onload_event_choose_popup' => array(
      'label' => t('There is a popup to display'),
      'group' => t('Popup On Load'),
      'variables' => array(
        'popup_onload' => array(
          'label' => t('popup-on-load entity'),
          'type' => 'popup_onload',
          'access callback' => 'popup_onload_integration_access',
        ),
      ),
    ),
  );
}

/**
 * Implements hook_rules_condition_info().
 */
function popup_onload_rules_condition_info() {
  return array(
    'popup_onload_condition_check_cookie' => array(
      'label' => t('Popup-on-load cookie is not set'),
      'parameter' => array(
        'popup_onload' => array(
          'type' => 'popup_onload',
          'label' => t('Popup-on-load entity'),
        ),
      ),
      'group' => t('Popup On Load'),
    ),
  );
}

/**
 * Implements hook_rules_action_info().
 */
function popup_onload_rules_action_info() {
  return array(
    'popup_onload_action_display_popup' => array(
      'label' => t('Display a popup'),
      'parameter' => array(
        'popup_onload' => array(
          'type' => 'popup_onload',
          'label' => t('Popup On Load'),
        ),
      ),
      'group' => t('Popup On Load'),
    ),
  );
}

/**
 * Condition to check if popup cookie is set.
 */
function popup_onload_condition_check_cookie($popup_onload, $context = array()) {
  return !popup_onload_check_time_cookie($popup_onload, $context = array());
}

/**
 * Action to display a popup.
 */
function popup_onload_action_display_popup($popup_onload, $context = array()) {
  $popup_settings = popup_onload_prepare_popup($popup_onload);
  popup_onload_add_popup($popup_settings);
  popup_onload_save_time_cookie($popup_onload);
}

/**
 * Popup ad integration access callback.
 */
function popup_onload_integration_access($type, $name) {
  if ($type == 'event' || $type == 'condition') {
    return entity_access('view', 'popup_onload');
  }
}

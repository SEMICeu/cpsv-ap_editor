<?php

/**
 * @file
 * Rules integration for the "Simple modal overlay" module.
 */

/**
 * Implements hook_rules_action_info().
 */
function simple_modal_overlay_rules_action_info() {
  return array(
    'simple_modal_overlay_message' => array(
      'label' => t('Show content in a simple overlay'),
      'group' => t('System'),
      'parameter' => array(
        'title' => array(
          'type' => 'text',
          'label' => t('Title of the overlay message'),
        ),
          
        'content' => array(
          'type' => 'text',
          'label' => t('Content of the overlay message'),
        ),
      ),
      'base' => 'simple_modal_overlay_rules_action',
    ),
  );
}

/**
 * Rules action: displays a message in a simple modal overlay.
 */
function simple_modal_overlay_rules_action($title, $content) {
  simple_modal_overlay_show($title, $content);
}
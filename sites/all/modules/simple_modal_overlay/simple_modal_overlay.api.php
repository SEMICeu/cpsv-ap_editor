<?php
/**
* @file
* This file contains no working PHP code; it exists to provide additional
* documentation for doxygen as well as to document the Simple modal overlay
* API in the standard Drupal manner.
*/

/**
 * A sample of how to invoke a simple modal overlay.
 */
function simple_modal_overlay_example() {
  simple_modal_overlay_show(
    t("The title of my message"),
    array(
      '#type'   => 'fieldset',
      '#title'  => t('My fieldset'),
      '#collapsible' => TRUE,
      '#collapsed' => FALSE,

      'content' => array(
        '#type' => 'markup',
        '#markup' => "The content of my message.",
      )
    ));
}
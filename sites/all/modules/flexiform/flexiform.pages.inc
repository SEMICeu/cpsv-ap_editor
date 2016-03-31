<?php
/**
 * @file
 * Page and page title callbacks for flexiform.
 */

/**
 * Page callback for flexiform submission pages.
 */
function flexiform_page_wrapper($flexiform, $display, $base_entity_id = NULL) {
  if (!is_object($flexiform)) {
    $flexiform = flexiform_load($flexiform);
  }

  if (!$flexiform) {
    return drupal_not_found();
  }

  $context = array(
    'base_entity_id' => $base_entity_id,
    'args' => array_slice(func_get_args(), 3),
  );
  return $flexiform->getDisplay($display)->build($context);
}

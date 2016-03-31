<?php

/**
 * @file
 *
 * Contains additional entity information for the flexiform webform submission entity.
 */

/**
 * Implements hook_entity_property_info_alter().
 */
function flexiform_webform_entity_property_info_alter(&$property_info) {
  $properties =& $property_info['flexiform_webform_submission']['properties'];

  $properties['created']['type'] = 'date';
  $properties['changed']['type'] = 'date';
  $properties['webform']['required'] = TRUE;
  $properties['webform']['type'] = 'flexiform_webform';
}

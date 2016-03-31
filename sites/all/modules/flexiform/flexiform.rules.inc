<?php
/**
 * @file
 * Rules integration for the flexiform module.
 */

/**
 * Implements hook_rules_event_info().
 */
function flexiform_rules_event_info() {
  $defaults = array(
    'group' => t('Flexiform'),
    'module' => 'flexiform',
  );

  $events = array();
  // Add an event for all flexiforms.
  $event = $defaults + array(
    'label' => t('On submitting any flexiform'),
    'variables' => array(
      'base_entity' => array(
        'type' => 'entity',
        'label' => t('Base Entity'),
      ),
    ),
  );
  $events['flexiform_any_submit'] = $event;

  // Add an event for each group.
  foreach (flexiform_get_groups() as $group => $info) {
    $event = $defaults + array(
      'label' => t('On submitting any flexiform in the @group group', array('@group' => $info['label'])),
      'variables' => array(
        'base_entity' => array(
          'type' => 'entity',
          'label' => t('Base Entity'),
        ),
      ),
    );
    $events['flexiform_group_submit_' . $group] = $event;
  }

  // Add an event for each flexiform.
  foreach(entity_load('flexiform') as $flexiform) {
    $event = $defaults + array(
      'label' => t('On submitting @flexiform', array('@flexiform' => $flexiform->label)),
      'variables' => array(),
    );

    foreach ($flexiform->entities as $entity_namespace => $settings) {
      $variable = array(
        'type' => $settings['entity_type'],
        'label' => $settings['label'],
      );
      $event['variables'][$entity_namespace] = $variable;
    }

    $events['flexiform_submit_'. $flexiform->form] = $event;
  }

  return $events;
}

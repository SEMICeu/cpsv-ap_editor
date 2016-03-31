<?php
/**
 * @file
 * All the flexiform hooks.
 */

/**
 * Implements hook_flexiform_group_info().
 */
function flexiform_flexiform_group_info() {
  return array(
    'general' => array(
      'label' => t('General'),
    ),
  );
}

/**
 * Implements hook_flexiform_builder_info().
 */
function flexiform_flexiform_builder_info() {
  $builders = array();

  $builders['FlexiformBuilderFlexiform'] = array(
    'class' => 'FlexiformBuilderFlexiform',
    'label' => t('Flexiform Form Builder'),
    'description' => t('The flexiform custom form builder. Use the configuration pages to add fields and entities to the form.'),
  );

  $fieldable_entities = array();
  foreach (entity_get_info() as $entity_type => $info) {
    if ($info['fieldable']) {
      $fieldable_entities[] = $entity_type;
    }
  }
  $builders['FlexiformBuilderEntityForm'] = array(
    'class' => 'FlexiformBuilderEntityForm',
    'label' => t('Entity Field Form'),
    'description' => t('Render the standard entity field form.'),
    'entity_types' => $fieldable_entities,
  );

  return $builders;
}

/**
 * Implements hook_flexiform_entity_getter_info().
 */
function flexiform_flexiform_entity_getter_info() {
  $getters = array();

  // Base entity getter.
  $getters['base_entity'] = array(
    'label' => 'Base Entity',
    'description' => 'The Base Entity for this Flexiform',
    'entity_types' => array_keys(entity_get_info()),
    'class' => 'FlexiformFormEntityBase',
  );

  // Load entity getter.
  $getters['load_entity'] = array(
    'label' => 'Load Entity from ID',
    'description' => 'Load the entity based on id.',
    'entity_types' => array_keys(entity_get_info()),
    'class' => 'FlexiformFormEntityLoad',
  );

  // New entity getter.
  $getters['new_entity'] = array(
    'label' => 'Create a New Entity',
    'description' => 'Create a new entity.',
    'entity_types' => array_keys(entity_get_info()),
    'class' => 'FlexiformFormEntityNew',
  );

  // User Getters
  $getters['user_current_user'] = array(
    'label' => 'Current User',
    'description' => 'Load the current user into the Form',
    'entity_types' => array('user'),
    'class' => 'FlexiformFormEntityCurrentUser',
  );

  // Profile2 Getters
  if (module_exists('profile2')) {
    $getters['profile2_profile_from_user'] = array(
      'label' => 'Profile2 from User',
      'description' => 'Load a Profile 2 Basede on a User',
      'params' => array(
        'user' => array(
          'entity_type' => 'user',
        ),
      ),
      'entity_types' => array('profile2'),
      'file' => 'profile2.flexiform.inc',
    );
  }

  // Party Getters.
  if (module_exists('party')) {
    // Getters for data sets.
    foreach (party_get_data_set_info() as $data_set_name => $info) {
      $getters[$data_set_name . '_from_party'] = array(
        'label' => t('@label data set', array('@label' => $info['label'])),
        'description' => t('Load a data set from a party.'),
        'data_set_name' => $data_set_name,
        'params' => array(
          'party' => array(
            'entity_type' => 'party',
          ),
        ),
        'entity_types' => array($info['entity type']),
        'class' => 'FlexiformFormEntityPartyDataSet',
      );
    }

    // Party from user Getters
    $getters['party_from_user'] = array(
      'label' => 'Party from User',
      'description' => 'Load a Party Based on a User',
      'params' => array(
        'user' => array(
          'entity_type' => 'user',
        ),
      ),
      'entity_types' => array('party'),
      'class' => 'FlexiformFormEntityPartyFromUser',
    );
  }

  // Build entity reference getters.
  if (module_exists('entityreference')) {
    $fields = db_select('field_config', 'fc')
      ->condition('type', 'entityreference')
      ->fields('fc', array('field_name'))
      ->execute()
      ->fetchCol();

    foreach ($fields as $field_name) {
      // Get all the info we need.
      $field = field_info_field($field_name);
      $target_entity_type = $field['settings']['target_type'];
      $target_entity_info = entity_get_info($target_entity_type);

      // Build a getter for each entity_type this field is present on.
      foreach ($field['bundles'] as $base_entity_type => $bundles) {
        $base_entity_info = entity_get_info($base_entity_type);

        // Prepare replacements for the label.
        $label_replacements = array(
          '@target' => $target_entity_info['label'],
          '@base' => $base_entity_info['label'],
          '@field' => $field_name,
        );

        // Build getter info.
        $getters["entityreference__{$field_name}__{$base_entity_type}"] = array(
          'label' => t('@target from @base via @field', $label_replacements),
          'description' => t('Load an entity via an entityreference field'),
          'field_name' => $field_name,
          'params' => array(
            'base' => array(
              'entity_type' => $base_entity_type,
            ),
          ),
          'entity_types' => array($target_entity_type),
          'class' => 'FlexiformFormEntityEntityReference',
        );
      }
    }
  }

  // Build field collection getters.
  if (module_exists('field_collection')) {
    $fields = db_select('field_config', 'fc')
      ->condition('type', 'field_collection')
      ->fields('fc', array('field_name'))
      ->execute()
      ->fetchCol();

    foreach ($fields as $field_name) {
      $field = field_info_field($field_name);

      // Build a getter for each entity_type this field is present on.
      foreach ($field['bundles'] as $base_entity_type => $bundles) {
        $base_entity_info = entity_get_info($base_entity_type);

        // Prepare replacements for the label.
        $label_replacements = array(
          '@base' => $base_entity_info['label'],
          '@field' => $field_name,
        );

        $getters["field_collection__{$field_name}__{$base_entity_type}"] = array(
          'label' => t('Field Collection Item: @field from @base', $label_replacements),
          'description' => t('Load the field collection item'),
          'field_name' => $field_name,
          'params' => array(
            'base' => array(
              'entity_type' => $base_entity_type,
            ),
          ),
          'entity_types' => array('field_collection_item'),
          'class' => 'FlexiformFormEntityFieldCollection',
        );
      }
    }
  }

  // Build reply module getters.
  if (module_exists('reply')) {
    $fields = db_select('field_config', 'fc')
      ->condition('type', 'reply')
      ->fields('fc', array('field_name'))
      ->execute()
      ->fetchCol();

    foreach ($fields as $field_name) {
      $field = field_info_field($field_name);

      foreach ($field['bundles'] as $base_entity_type => $bundles) {
        $base_entity_info = entity_get_info($base_entity_type);

        // Prepare replacements for the label.
        $label_replacements = array(
          '@base' => $base_entity_info['label'],
          '@field' => $field_name,
        );

        $getters["reply_new__{$field_name}__{$base_entity_type}"] = array(
          'label' => t('New Reply: @field from @base', $label_replacements),
          'description' => t('Get a new reply on the entity.'),
          'field_name' => $field_name,
          'params' => array(
            'base' => array(
              'entity_type' => $base_entity_type,
            ),
          ),
          'entity_types' => array('reply'),
          'class' => 'FlexifromFormEntityNewReply',
        );
      }
    }
  }
  return $getters;
}

/**
 * Implements hook_flexiform_display_info().
 */
function flexiform_flexiform_display_info() {
  $displays = array();

  $displays['FlexiformDisplayCreatePage'] = array(
    'label' => t('Add page'),
    'description' => t('A page to add a new submission of the form. Will create a new base entity.'),
  );

  $displays['FlexiformDisplayEditPage'] = array(
    'label' => t('Edit page'),
    'description' => t('A page to an edit form. Will load an existing base entity.'),
  );

  if (module_exists('ctools')) {
    $displays['FlexiformDisplayAddPane'] = array(
      'label' => t('Add Pane (Ctools Content)'),
      'description' => t('Expose the form in a Ctools Content Plugin.'),
    );

    $displays['FlexiformDisplayEditPane'] = array(
      'label' => t('Edit Pane (Ctools Content)'),
      'description' => t('Expose the form in a Ctools Content Plugin.'),
    );
  }

  if (module_exists('user')) {
    $displays['FlexiformDisplayUserCategory'] = array(
      'label' => t('User category'),
      'description' => t('Expose this form as a tab under user edit.'),
    );
  }

  return $displays;
}

/**
 * Implements hook_flexiform_element_info().
 */
function flexiform_flexiform_element_info() {
  $elements = array();

  // Get the element for all nested flexiforms.
  $forms = db_select('flexiform', 'f')
    ->fields('f', array('label', 'form', 'base_entity', 'base_entity_bundle'))
    ->execute()
    ->fetchAllAssoc('form');

  // Add custom form elements to every entity type.
  foreach (entity_get_info() as $entity_type => $info) {
    foreach ($info['bundles'] as $bundle => $info) {
      $elements[$entity_type][$bundle]['custom:html'] = array(
        'label' => t('Custom HTML'),
        'class' => 'FlexiformElementCustomHtml',
        'type' => 'custom_html',
        'group' => 'Custom',
      );
    }
  }

  foreach ($forms as $form) {
    $elements[$form->base_entity][$form->base_entity_bundle]['flexiform:' . $form->form] = array(
      'label' => $form->label,
      'class' => 'FlexiformElementFlexiform',
      'type' => 'form',
      'group' => 'Flexiform',
      'form' => $form->form,
    );
  }

  // Get all the field elements.
  $fields_info = field_info_instances();
  foreach ($fields_info as $entity_type => $entity_fields) {
    foreach ($entity_fields as $bundle => $bundle_fields) {
      foreach ($bundle_fields as $field_name => $instance) {
        $field = field_info_field($instance['field_name']);
        $field_class = 'FlexiformElementField_' . $field['type'];
        $elements[$entity_type][$bundle]['field:' . $field_name] = array(
          'label' => $instance['label'],
          'class' => class_exists($field_class) ? $field_class : 'FlexiformElementField',
          'type' => 'field',
          'group' => 'Field',
          'field_name' => $field_name,
        );
      }
    }
  }

  // Get node elements
  foreach (node_type_get_types() as $bundle => $info) {
    if ($info->has_title) {
      $elements['node'][$bundle]['node:title'] = array(
        'label' => $info->title_label,
        'class' => 'FlexiformElementNodeTitle',
        'type' => 'title',
        'group' => 'Node',
      );
    }

    $elements['node'][$bundle]['node:author'] = array(
      'label' => t('Authored by'),
      'class' => 'FlexiformElementNodeAuthor',
      'type' => 'author',
      'group' => 'Node',
    );

    $elements['node'][$bundle]['node:status'] = array(
      'label' => t('Published'),
      'class' => 'FlexiformElementNodeStatus',
      'type' => 'status',
      'group' => 'Node',
    );

    $elements['node'][$bundle]['node:promote'] = array(
      'label' => t('Promoted to front page'),
      'class' => 'FlexiformElementNodePromote',
      'type' => 'promote',
      'group' => 'Node',
    );

    $elements['node'][$bundle]['node:sticky'] = array(
      'label' => t('Sticky at top of lists'),
      'class' => 'FlexiformElementNodeSticky',
      'type' => 'sticky',
      'group' => 'Node',
    );
  }
  
  $elements['user']['user']['user:name'] = array(
    'label' => t('Name'),
    'class' => 'FlexiformElementUserName',
    'type' => 'name',
    'group' => 'User',
  );
  $elements['user']['user']['user:pass'] = array(
    'label' => t('Password'),
    'class' => 'FlexiformElementUserPass',
    'type' => 'pass',
    'group' => 'User',
  );
  $elements['user']['user']['user:mail'] = array(
    'label' => t('E-mail'),
    'class' => 'FlexiformElementUserMail',
    'type' => 'mail',
    'group' => 'User',
  );
  $elements['user']['user']['user:roles'] = array(
    'label' => t('Roles'),
    'class' => 'FlexiformElementUserRoles',
    'type' => 'roles',
    'group' => 'User',
  );

  return $elements;
}

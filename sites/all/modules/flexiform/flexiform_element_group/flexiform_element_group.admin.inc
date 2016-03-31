<?php
/**
 * @file
 * Contains alterations to the flexiform admin pages for element groups.
 */

/**
 * Helper function to get the form parameters to use while
 * building the fields and display overview form.
 */
function flexiform_element_group_manage_form_fields_form_params($form) {

  $params = new stdClass();
  $params->flexiform = $form['#flexiform'];
  $params->entity_type = 'flexiform';
  $params->bundle = $params->flexiform->form;
  $params->admin_path = 'admin/structure/flexiforms/manage/' . $params->flexiform->form . '/form-fields';
  $params->display_overview = FALSE;

  $formatters = field_group_formatter_info();
  $params->region_callback = 'field_group_field_overview_row_region';
  $params->mode = 'form';

  $params->groups = field_group_info_groups('flexiform', $params->flexiform->form, $params->mode, TRUE);

  // Gather parenting data.
  $params->parents = array();
  foreach ($params->groups as $name => $group) {
    foreach ($group->children as $child) {
      $params->parents[$child] = $name;
    }
  }

  return $params;
}

/**
 * Function to alter the form fields overview screen.
 */
function flexiform_element_group_manage_form_fields_form_alter(&$form, &$form_state) {
  form_load_include($form_state, 'inc', 'field_group', 'field_group.field_ui');
  form_load_include($form_state, 'inc', 'field_ui', 'field_ui.admin');

  $params = flexiform_element_group_manage_form_fields_form_params($form);

  // This key is used to store the current updated field.
  $form_state += array(
    'formatter_settings_edit' => NULL,
  );

  // Add AJAX wrapper.
  $form['fields']['#prefix'] = '<div id="field-display-overview-wrapper">';
  $form['fields']['#suffix'] = '</div>';
  $form['#groups'] = array_keys($params->groups);

  $table = &$form['fields'];

  // Add a region for 'add_new' rows, but only when fields are
  // available and thus regions.
  if (isset($table['#regions'])) {
    $table['#regions'] += array(
      'add_new' => array('title' => '&nbsp;'),
    );
  }

  // Extend available parenting options.
  foreach ($params->groups as $name => $group) {
    $table['#parent_options'][$name] = $group->label;
  }
  $table['#parent_options']['_add_new_group'] = t('Add new group');

  // Update existing rows accordingly to the parents.
  foreach (element_children($table) as $name) {
    $table[$name]['parent_wrapper']['parent']['#options'] = $table['#parent_options'];
    // Inherit the value of the parent when default value is empty.
    if (empty($table[$name]['parent_wrapper']['parent']['#default_value'])) {
      $table[$name]['parent_wrapper']['parent']['#default_value'] = isset($params->parents[$name]) ? $params->parents[$name] : '';
    }
  }

  $formatter_options = field_group_field_formatter_options('form');

  $refresh_rows = isset($form_state['values']['refresh_rows']) ? $form_state['values']['refresh_rows'] : (isset($form_state['input']['refresh_rows']) ? $form_state['input']['refresh_rows'] : NULL);

  // Create the group rows and check actions.
  foreach (array_keys($params->groups) as $name) {

    // Play around with form_state so we only need to hold things
    // between requests, until the save button was hit.
    if (isset($form_state['flexiform_element_group'][$name])) {
      $group = & $form_state['flexiform_element_group'][$name];
    }
    else {
      $group = & $params->groups[$name];
    }

    // Check the currently selected formatter, and merge persisted values for
    // formatter settings for the group.
    // This needs to be done first, so all fields are updated before creating form elements.
    if (isset($refresh_rows) && $refresh_rows == $name) {
      $settings = isset($form_state['values']['fields'][$name]) ? $form_state['values']['fields'][$name] : (isset($form_state['input']['fields'][$name]) ? $form_state['input']['fields'][$name] : NULL);
      if (array_key_exists('settings_edit', $settings)) {
        //$group->format_type = $form_state['flexiform_element_group'][$name]->format_type;
        $group = $form_state['flexiform_element_group'][$name];
      }
      field_group_formatter_row_update($group, $settings);
    }

    // Save the group when the configuration is submitted.
    if (!empty($form_state['values'][$name . '_formatter_settings_update'])) {
      field_group_formatter_settings_update($group, $form_state['values']['fields'][$name]);
    }
    // After all updates are finished, let the form_state know.
    $form_state['flexiform_element_group'][$name] = $group;

    $settings = field_group_format_settings_form($group);

    $id = strtr($name, '_', '-');
    $js_rows_data[$id] = array('type' => 'group', 'name' => $name);
    // A group cannot be selected as its own parent.
    $parent_options = $table['#parent_options'];
    unset($parent_options[$name]);
    $table[$name] = array(
      '#attributes' => array('class' => array('draggable', 'field-group'), 'id' => $id),
      '#row_type' => 'group',
      '#region_callback' => $params->region_callback,
      '#js_settings' => array('rowHandler' => 'group'),
      'label' => array(
        '#markup' => check_plain(t($group->label)),
        '#prefix' => '<span class="group-label">',
        '#suffix' => '</span>',
      ),
      'weight' => array(
        '#type' => 'textfield',
        '#default_value' => $group->weight,
        '#size' => 3,
        '#attributes' => array('class' => array('field-weight')),
      ),
      'parent_wrapper' => array(
        'parent' => array(
          '#type' => 'select',
          '#options' =>  $parent_options,
          '#empty_value' => '',
          '#default_value' => isset($params->parents[$name]) ? $params->parents[$name] : '',
          '#attributes' => array('class' => array('field-parent')),
          '#parents' => array('fields', $name, 'parent'),
        ),
        'hidden_name' => array(
          '#type' => 'hidden',
          '#default_value' => $name,
          '#attributes' => array('class' => array('field-name')),
        ),
      ),
      'type' => array(
        '#markup' => t('Group'),
      ),
      'entity' => array(
        '#markup' => '&nbsp',
      ),
    );

    $table[$name] += array(
      'group_name' => array(
        '#markup' => check_plain($name),
      ),
      'format' => array(
        'type' => array(
          '#type' => 'select',
          '#options' => $formatter_options,
          '#default_value' => $group->format_type,
          '#attributes' => array('class' => array('field-group-type')),
        ),
      ),
    );

    $base_button = array(
      '#submit' => array('field_ui_display_overview_multistep_submit'),
      '#ajax' => array(
        'callback' => 'field_ui_display_overview_multistep_js',
        'wrapper' => 'field-display-overview-wrapper',
        'effect' => 'fade',
      ),
      '#field_name' => $name,
    );

    if ($form_state['formatter_settings_edit'] == $name) {
      $table[$name]['format']['#cell_attributes'] = array('colspan' => 1);
      $table[$name]['format']['format_settings'] = array(
        '#type' => 'container',
        '#attributes' => array('class' => array('field-formatter-settings-edit-form')),
        '#parents' => array('fields', $name, 'format_settings'),
        '#weight' => -5,
        'label' => array(
          '#markup' => t('Field group format:') . ' <span class="formatter-name">' . $group->format_type . '</span>',
        ),
        // Create a settings form where hooks can pick in.
        'settings' => $settings,
        'actions' => array(
          '#type' => 'actions',
          'save_settings' => $base_button + array(
            '#type' => 'submit',
            '#name' => $name . '_formatter_settings_update',
            '#value' => t('Update'),
            '#op' => 'update',
          ),
          'cancel_settings' => $base_button + array(
            '#type' => 'submit',
            '#name' => $name . '_formatter_settings_cancel',
            '#value' => t('Cancel'),
            '#op' => 'cancel',
            // Do not check errors for the 'Cancel' button.
            '#limit_validation_errors' => array(),
          ),
        ),
      );
      $table[$name]['#attributes']['class'][] = 'field-formatter-settings-editing';
      $table[$name]['format']['type']['#attributes']['class'] = array('element-invisible');
    }
    else {
      // After saving, the settings are updated here aswell. First we create
      // the element for the table cell.
      $table[$name]['settings_summary'] = array('#markup' => '');
      if (!empty($group->format_settings)) {
        $table[$name]['settings_summary'] = field_group_format_settings_summary($name, $group);
      }
      // Add the configure button.
      $table[$name]['settings_edit'] = $base_button + array(
        '#type' => 'image_button',
        '#name' => $name . '_group_settings_edit',
        '#src' => 'misc/configure.png',
        '#attributes' => array('class' => array('field-formatter-settings-edit'), 'alt' => t('Edit')),
        '#op' => 'edit',
        // Do not check errors for the 'Edit' button.
        '#limit_validation_errors' => array(),
        '#prefix' => '<div class="field-formatter-settings-edit-wrapper">',
        '#suffix' => '</div>',
      );
    }

    $table[$name] += array(
      'delete' => array(
        '#markup' => l(t('remove'), $params->admin_path . '/groups/' . $name . '/delete/form'),
      ),
    );
  }

  // Additional row: add new group.
  $parent_options = $table['#parent_options'];
  unset($parent_options['_add_new_group']);
  $table['_add_new_group'] = field_group_add_row('_add_new_group', $parent_options, $params);

  $table['_add_new_group'] += array(
    'format' => array(
      'type' => array(
        '#type' => 'select',
        '#options' => $formatter_options,
        '#default_value' => 'fieldset',
      ),
    ),
    'settings_summary' => array(),
    'settings_edit' => array(),
  );

  // See field_ui.admin.inc for more details on refresh rows.
  $form['refresh_rows'] = array('#type' => 'hidden');
  $form['refresh'] = array(
    '#type' => 'submit',
    '#value' => t('Refresh'),
    '#op' => 'refresh_table',
    '#submit' => array('field_ui_display_overview_multistep_submit'),
    '#ajax' => array(
      'callback' => 'field_ui_display_overview_multistep_js',
      'wrapper' => 'field-display-overview-wrapper',
      'effect' => 'fade',
      // The button stays hidden, so we hide the AJAX spinner too. Ad-hoc
      // spinners will be added manually by the client-side script.
      'progress' => 'none',
    ),
  );

  $form['#attached']['css'][] = drupal_get_path('module', 'field_group') . '/field_group.field_ui.css';
  $form['#attached']['js'][] = drupal_get_path('module', 'field_group') . '/field_group.field_ui.js';

  $form['#validate'][] = 'flexiform_element_group_manage_form_fields_form_validate';
  $form['#submit'][] = 'flexiform_element_group_manage_form_fields_form_submit';

  // Create the settings for fieldgroup as vertical tabs (merged with DS).
  field_group_field_ui_create_vertical_tabs($form, $form_state, $params);
}

/**
 * Validation callback for flexiform_element_groups.
 */
function flexiform_element_group_manage_form_fields_form_validate($form, &$form_state) {
  $form_values = $form_state['values']['fields'];

  // This is a bit of a dirty hack but for the sake of working with field
  // group module we pretend the form name is the bundle of the flexiform
  // entity.
  $entity_type = 'flexiform';
  $bundle = $form['#flexiform']->form;
  $mode = 'form';

  $group = $form_values['_add_new_group'];

  // Validate if any information was provided in the 'add new group' row.
  if (array_filter(array($group['label'], $group['group_name']))) {
    // Missing label.
    if (!$group['label']) {
      form_set_error('fields][_add_new_group][label', t('Add new group: you need to provide a label.'));
    }

    // Missing group name.
    if (!$group['group_name']) {
      form_set_error('fields][_add_new_group][group_name', t('Add new group: you need to provide a group name.'));
    }
    // Group name validation.
    else {
      $group_name = $group['group_name'];

      // Add the 'group_' prefix.
      if (drupal_substr($group_name, 0, 6) != 'group_') {
        $group_name = 'group_' . $group_name;
        form_set_value($form['fields']['_add_new_group']['group_name'], $group_name, $form_state);
      }

      // Invalid group name.
      if (!preg_match('!^group_[a-z0-9_]+$!', $group_name)) {
        form_set_error('fields][_add_new_group][group_name', t('Add new group: the group name %group_name is invalid. The name must include only lowercase unaccentuated letters, numbers, and underscores.', array('%group_name' => $group_name)));
      }
      if (drupal_strlen($group_name) > 32) {
        form_set_error('fields][_add_new_group][group_name', t("Add new group: the group name %group_name is too long. The name is limited to 32 characters, including the 'group_' prefix.", array('%group_name' => $group_name)));
      }

      // Group name already exists.
      if (field_group_exists($group_name, $entity_type, $bundle, $mode)) {
        form_set_error('fields][_add_new_group][group_name', t('Add new group: the group name %group_name already exists.', array('%group_name' => $group_name)));
      }
    }
  }
}

/**
 * Submit callback for element groups.
 */
function flexiform_element_group_manage_form_fields_form_submit($form, &$form_state) {
  $form_values = $form_state['values']['fields'];
  $entity_type = 'flexiform';
  $bundle = $form['#flexiform']->form;
  $mode = 'form';

  // Collect children.
  $children = array_fill_keys($form['#groups'], array());
  foreach ($form_values as $name => $value) {
    if (!empty($value['parent'])) {
      // Substitute newly added fields, in case they were dragged
      // directly in a group.
      if ($name == '_add_field' && isset($form_state['fields_added']['_add_field'])) {
        $name = $form_state['fields_added']['_add_field'];
      }
      elseif ($name == '_add_form' && isset($form_state['fields_added']['_add_form'])) {
        $name = $form_state['fields_added']['_add_form'];
      }
      $children[$value['parent']][$name] = $name;
    }
  }

  // Prepare storage with ctools.
  ctools_include('export');

  // Create new group.
  if (!empty($form_values['_add_new_group']['group_name'])) {
    $values = $form_values['_add_new_group'];

    $field_group_types = field_group_formatter_info();
    $formatter = $field_group_types['form'][$values['format']['type']];

    $new_group = (object) array(
      'identifier' => $values['group_name'] . '|' . $entity_type . '|' . $bundle . '|' . $mode,
      'group_name' => $values['group_name'],
      'entity_type' => $entity_type,
      'bundle' => $bundle,
      'mode' => $mode,
      'children' => isset($children['_add_new_group']) ? array_keys($children['_add_new_group']) : array(),
      'parent_name' => $values['parent'],
      'weight' => $values['weight'],
      'label' => $values['label'],
      'format_type' => $values['format']['type'],
      'disabled' => FALSE,
    );
    $new_group->format_settings = array('formatter' => isset($formatter['default_formatter']) ? $formatter['default_formatter'] : '');
    if (isset($formatter['instance_settings'])) {
      $new_group->format_settings['instance_settings'] = $formatter['instance_settings'];
    }

    // Save and enable it in ctools
    ctools_export_crud_save('field_group', $new_group);
    ctools_export_crud_enable('field_group', $new_group->identifier);

    // Store new group information for any additional submit handlers.
    $form_state['groups_added']['_add_new_group'] = $new_group->group_name;
    drupal_set_message(t('New group %label successfully created.', array('%label' => $new_group->label)));

    // Replace the newly created group in the $children array, in case it was
    // dragged directly in an existing field.
    foreach (array_keys($children) as $parent) {
      if (isset($children[$parent]['_add_new_group'])) {
        unset($children[$parent]['_add_new_group']);
        $children[$parent][$new_group->group_name] = $new_group->group_name;
      }
    }
  }

  // Update existing groups.
  $groups = field_group_info_groups($entity_type, $bundle, $mode, TRUE);
  foreach ($form['#groups'] as $group_name) {
    $group = $groups[$group_name];
    $group->label = $form_state['flexiform_element_group'][$group_name]->label;
    $group->children = array_keys($children[$group_name]);
    $group->parent_name = $form_values[$group_name]['parent'];
    $group->weight = $form_values[$group_name]['weight'];
    $group->format_type = isset($form_values[$group_name]['format']['type']) ? $form_values[$group_name]['format']['type'] : 'visible';
    if (isset($form_state['flexiform_element_group'][$group_name]->format_settings)) {
      $group->format_settings = $form_state['flexiform_element_group'][$group_name]->format_settings;
    }
    ctools_export_crud_save('field_group', $group);
  }

  cache_clear_all('field_groups', 'cache_field');
}

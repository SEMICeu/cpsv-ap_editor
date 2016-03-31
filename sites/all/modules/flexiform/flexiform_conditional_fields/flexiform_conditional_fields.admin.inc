<?php
/**
 * @file
 * Admin UI for flexiform_conditional_fields.
 */

/**
 * Add the conditional fields form.
 */
function flexiform_conditional_fields_flexiform_element_configure_form($form, &$form_state, $complete_form) {
  $element = $complete_form['#flexiform_element'];
  $flexiform = $complete_form['#flexiform'];
  $settings = $element->getSettings();
  $dependent = $element_namespace = $element->getElementNamespace();

  $dependees = !empty($settings['conditional_fields']) ? $settings['conditional_fields'] : array();
  $form['table'] = array(
    '#theme' => 'table',
    '#header' => array(
      t('Dependees'),
      array('data' => t('Description'), 'colspan' => 2),
      array('data' => t('Operations'), 'colspan' => 2),
    ),
    '#rows' => array(),
    '#attributes' => array(
      'class' => array('conditional-fields-overview'),
    ),
    'dependencies' => array(),
  );
  $form['table']['#attached']['css'][] = drupal_get_path('module', 'conditional_fields') . '/conditional_fields.css';

  if (!empty($dependees)) {
    $destination = drupal_get_destination();
    $grouping_count = array();

    // Enable row grouping by operator.
    $first_row = $show_AND = $show_OR = $show_XOR = TRUE;
    if (!isset($grouping_count[$dependent])) {
      $grouping_count[$dependent] = array(
        'AND' => 0,
        'OR' => 0,
        'XOR' => 0,
      );
      foreach ($dependees as $options) {
        $grouping_count[$dependent][$options['grouping']]++;
      }
    }

    foreach ($dependees as $dependency_key => $options) {
      list($dependee) = explode('|', $dependency_key);
      if (empty($flexiform->elements[$dependee])) {
        unset($flexiform->elements[$element_namespace]['conditional_fields'][$dependency_key]);
        unset($dependees[$dependency_key]);
        continue;
      }
      $form['table'][$dependency_key] = array();
      $dependee_count = count($dependees);
      $dependee_element = FlexiformElement::getElement($flexiform, $dependee);

      $row = array(
        'dependee' => array(
          '#markup' => check_plain($dependee_element->label() . ' (' . $dependee . ')'),
        ),
      );

      // To avoid clutter, collect information about groupings so we can show each
      // operator once per dependent.
      if ($dependee_count > 1) {
        if (${'show_' . $options['grouping']}) {
          $row['group'] = array(
            '#markup' => $options['grouping'],
            '#rowspan' => $grouping_count[$dependent][$options['grouping']],
          );
          ${'show_' . $options['grouping']} = FALSE;
        }
        $row['description']['#colspan'] = 1;
      }
      else {
        $row['description']['#colspan'] = 2;
      }

      $row['description']['#markup'] = conditional_fields_dependency_description($dependee_element->label(), $element->label(), $options);
      $row['edit'] = array(
        '#type' => 'link',
        '#title' => t('edit'),
        '#href' => 'admin/structure/flexiforms/manage/'.$flexiform->form.'/form-fields/'.$element_namespace.'/conditional-fields/'.$dependency_key.'/edit',
        '#options' => array('query' => $destination, 'attributes' => array('title' => t('Edit dependency settings.'))),
        '#query' => drupal_get_destination(),
      );
      $row['delete'] = array(
        '#type' => 'link',
        '#title' => t('delete'),
        '#href' => 'admin/structure/flexiforms/manage/'.$flexiform->form.'/form-fields/'.$element_namespace.'/conditional-fields/'.$dependency_key.'/delete',
        '#options' => array('query' => $destination, 'attributes' => array('title' => t('Delete dependency.'))),
      );

      if (!empty($row['group'])) {
        $form['table'][$dependency_key] = array(
          'dependee' => &$row['dependee'],
          'group' => &$row['group'],
          'description' => &$row['description'],
          'edit' => &$row['edit'],
          'delete' => &$row['delete'],
        );
        $form['table']['#rows'][$dependency_key] = array(
          'dependee' => array('data' => &$row['dependee']),
          'group' => array('data' => &$row['group']),
          'description' => array('data' => &$row['description'], 'colspan' => $row['description']['#colspan']),
          'edit' => array('data' => &$row['edit']),
          'delete' => array('data' => &$row['delete']),
        );
      }
      else {
        $form['table'][$dependency_key] = array(
          'dependee' => &$row['dependee'],
          'description' => &$row['description'],
          'edit' => &$row['edit'],
          'delete' => &$row['delete'],
        );
        $form['table']['#rows'][$dependency_key] = array(
          'dependee' => array('data' => &$row['dependee']),
          'description' => array('data' => &$row['description'], 'colspan' => $row['description']['#colspan']),
          'edit' => array('data' => &$row['edit']),
          'delete' => array('data' => &$row['delete']),
        );
      }

      unset($row);
    }
  }

  // Build a list of fields.
  $fields = array();
  foreach ($flexiform->elements as $namespace => $settings) {
    $fields[$namespace] = FlexiformElement::getElement($flexiform, $namespace)->label();
  }

  // Build list of states.
  $states = array_map('drupal_strtolower', conditional_fields_states());

  // Build list of conditions.
  foreach (conditional_fields_conditions() as $condition => $label) {
    $conditions[$condition] = $condition == 'value' ? t('has value...') : t('is !label', array('!label' => drupal_strtolower($label)));
  }

  // Add new dependency row.
  $form_row = array(
    'dependee' => array(
      '#type' => 'select',
      '#title' => t('Dependee'),
      '#title_display' => 'invisible',
      '#description' => t('Dependee'),
      '#options' => $fields,
      '#prefix' => '<div class="add-new-placeholder">&nbsp;</div>',
    ),
    'state' => array(
      '#type' => 'select',
      '#title' => t('State'),
      '#title_display' => 'invisible',
      '#options' => $states,
      '#default_value' => 'visible',
      '#prefix' => t('This element is') . '&nbsp;<span class="description-select">',
      '#suffix' => '</span>&nbsp;' . t('when the dependee'),
    ),
    'condition' => array(
      '#type' => 'select',
      '#title' => t('Condition'),
      '#title_display' => 'invisible',
      '#options' => $conditions,
      '#default_value' => 'value',
      '#prefix' => '&nbsp;<span class="description-select">',
      '#suffix' => '</span>',
    ),
    'actions' => array(
      'submit' => array(
        '#type' => 'submit',
        '#value' => t('Add dependency'),
        '#submit' => array('flexiform_conditional_fields_flexiform_element_configure_form_submit'),
      ),
    ),
  );
  $form['table']['add_new_dependency'] = array(
    'dependee' => &$form_row['dependee'],
    'state' => &$form_row['state'],
    'condition' => &$form_row['condition'],
    'actions' => &$form_row['actions'],
  );
  $form['table']['#rows']['add_new_dependency'] = array(
    'dependee' => array('data' => &$form_row['dependee']),
    'state' => array('data' => &$form_row['state']),
    'condition' => array('data' => &$form_row['condition']),
    'actions' => array('data' => &$form_row['actions']),
  );

  return $form;
}

/**
 * Submit the conditional fields form.
 */
function flexiform_conditional_fields_flexiform_element_configure_form_submit($form, &$form_state) {
  $flexiform = $form['#flexiform'];
  $element_namespace = $form['#flexiform_element']->getElementNamespace();

  $values = $form_state['input']['conditional_fields']['table']['add_new_dependency'];
  $options = array(
    'state' => $values['state'],
    'condition' => $values['condition'],
  );
  $options += conditional_fields_dependency_default_options();

  $dependency_key = $values['dependee'];
  $i = 1;
  while (!empty($flexiform->elements[$element_namespace]['conditional_fields'][$dependency_key])) {
    $dependency_key = $values['dependee'] . '|' . $i;
    $i++;
  }

  $flexiform->elements[$element_namespace]['conditional_fields'][$dependency_key] = $options;
  $flexiform->save();
}

/**
 * Page Callback for handling conditional fields operations.
 */
function flexiform_conditional_fields_operation($flexiform, $dependent, $dependency_key, $op = FALSE) {
  $form = ($op == 'edit') ? 'flexiform_conditional_fields_configure_form' : (($op == 'delete') ? 'flexiform_conditional_fields_delete_form' : FALSE);

  if (!$form) {
    return drupal_not_found();
  }

  return drupal_get_form($form, $flexiform, $dependent, $dependency_key);
}

/**
 * Form to edit conditional fields.
 */
function flexiform_conditional_fields_configure_form($form, &$form_state, $flexiform, $element_namespace, $dependency_key) {
  list($dependee) = explode('|', $dependency_key);
  $form['#flexiform'] = $flexiform;
  $form['#flexiform_element'] = FlexiformElement::getElement($flexiform, $element_namespace);
  $form['#flexiform_dependee'] = FlexiformElement::getElement($flexiform, $dependee);
  $form['#dependency_key'] = $dependency_key;

  $settings = $form['#flexiform_element']->getSettings();
  $options = $settings['conditional_fields'][$dependency_key];

  $form['#attached']['css'][] = drupal_get_path('module', 'conditional_fields') . '/conditional_fields.css';

  if ($form['#flexiform_dependee'] instanceof FlexiformElementFieldAPIInterface) {
    // Retrieve needed information from the dependee instance.
    // Since we only have the instance id here (id column of the
    // field_config_instance table), not the entity id (id column of field_config)
    // we can't call field_info_field_by_id. This is needed because we don't
    // want dependencies to be shared between bundles.
    // So we first load instance information to obtain the entity id, then we load
    // the entity using field_info_field().
    $dependee_instance = $form['#flexiform_dependee']->getInstance();
    $dependee_field = $form['#flexiform_dependee']->getField();

    // Build a dummy field widget to use as form field in single value selection
    // option.
    $dummy_form = array('#parents' => array());
    $dependee_instance['default_value'] = $options['value'];
    $dependee_instance['default_value_function'] = '';
    $dependee_instance['required'] = FALSE;

    $dummy_field = field_default_form($dependee_instance['entity_type'], NULL, $dependee_field, $dependee_instance, LANGUAGE_NONE, array(), $dummy_form, $form_state);
  }
  else {
    $dummy_field = array(
      '#markup' => t('Value comparisons are only supported for field API fields.'),
    );
  }

  $checkboxes = FALSE;
  if ($dependee_instance) {
    $checkboxes = ($dependee_instance['widget']['type'] == 'options_buttons' && $dependee_field['cardinality'] != 1) || $dependee_instance['widget']['type'] == 'options_onoff' ? TRUE : FALSE;
  }

  $condition_options = conditional_fields_conditions($checkboxes);
  // Currently only support value if this is an actual field API field.
  if (!$dependee_instance) {
    unset($condition_options['value']);
  }

  $form['condition'] = array(
    '#type' => 'select',
    '#title' => t('Condition'),
    '#description' => t('The condition that should be met by the dependee %field to trigger the dependency.', array('%field' => $dependee_instance['label'])),
    '#options' => $condition_options,
    '#default_value' => $options['condition'],
    '#required' => TRUE,
  );

  $form['values_set'] = array(
    '#type' => 'select',
    '#title' => t('Values input mode'),
    '#description' => t('The input mode of the values that trigger the dependency.'),
    '#options' => array(
      CONDITIONAL_FIELDS_DEPENDENCY_VALUES_WIDGET => t('Insert value from widget...'),
      CONDITIONAL_FIELDS_DEPENDENCY_VALUES_REGEX => t('Regular expression...'),
      t('Set of values') => array(
        CONDITIONAL_FIELDS_DEPENDENCY_VALUES_AND => t('All these values (AND)...'),
        CONDITIONAL_FIELDS_DEPENDENCY_VALUES_OR  => t('Any of these values (OR)...'),
        CONDITIONAL_FIELDS_DEPENDENCY_VALUES_XOR => t('Only one of these values (XOR)...'),
        CONDITIONAL_FIELDS_DEPENDENCY_VALUES_NOT => t('None of these values (NOT)...'),
        // TODO: PHP evaluation
      ),
    ),
    '#default_value' => $options['values_set'],
    '#required' => TRUE,
    '#states' => array(
      'visible' => array(
        ':input[name="condition"]' => array('value' => 'value'),
      ),
    ),
  );

  $form['value'] = array(
    '#type' => 'fieldset',
    '#title' => t('Insert value from widget'),
    '#description' => t('The dependency is triggered when the field has exactly the same value(s) inserted in the widget below.'),
    '#states' => array(
      'visible' => array(
        ':input[name="values_set"]' => array(
          'value' => (string) CONDITIONAL_FIELDS_DEPENDENCY_VALUES_WIDGET,
        ),
        ':input[name="condition"]' => array('value' => 'value'),
      ),
    ),
    '#tree' => TRUE,
    'field' => $dummy_field,
  );

  $form['values'] = array(
    '#type' => 'textarea',
    '#title' => t('Set of values'),
    '#description' => t('The values of the dependee %field that trigger the dependency.', array('%field' => $dependee_instance['label'])) . '<br>' . t('Enter one value per line. Note: if the dependee has allowed values, these are actually the keys, not the labels, of those values.'),
    '#default_value' => implode("\n", $options['values']),
    '#states' => array(
      'visible' => array(
        ':input[name="values_set"]' => array(
          array('value' => (string) CONDITIONAL_FIELDS_DEPENDENCY_VALUES_AND),
          array('value' => (string) CONDITIONAL_FIELDS_DEPENDENCY_VALUES_OR),
          array('value' => (string) CONDITIONAL_FIELDS_DEPENDENCY_VALUES_XOR),
          array('value' => (string) CONDITIONAL_FIELDS_DEPENDENCY_VALUES_NOT),
        ),
        ':input[name="condition"]' => array('value' => 'value'),
      ),
      'required' => array(
        ':input[name="condition"]' => array('value' => 'value'),
      ),
    ),
  );

  $form['regex'] = array(
    '#type' => 'textfield',
    '#title' => t('Regular expression'),
    '#description' => t('The dependency is triggered when all the values of the dependee %field match the regular expression. The expression should be valid both in PHP and in Javascript. Do not include delimiters.', array('%field' => $dependee_instance['label'])) . '<br>' . t('Note: If the dependee has allowed values, these are actually the keys, not the labels, of those values.'),
    '#maxlength' => 2048,
    '#size' => 120,
    '#default_value' => isset($options['value']['RegExp']) ? $options['value']['RegExp'] : '',
    '#states' => array(
      'visible' => array(
        ':input[name="values_set"]' => array('value' => (string) CONDITIONAL_FIELDS_DEPENDENCY_VALUES_REGEX),
        ':input[name="condition"]' => array('value' => 'value'),
      ),
      'required' => array(
        ':input[name="values_set"]' => array('value' => (string) CONDITIONAL_FIELDS_DEPENDENCY_VALUES_REGEX),
        ':input[name="condition"]' => array('value' => 'value'),
      ),
    ),
  );

  $form['grouping'] = array(
    '#type' => 'radios',
    '#title' => t('Interaction with other dependencies'),
    '#description' => t('When this dependent has more than one dependee, how should this condition be evaluated against the others?') . '<br />' . t('Note that sets will be grouped this way: (ANDs) AND (ORs) AND (XORs).'),
    '#options' => array('AND' => 'AND', 'OR' => 'OR', 'XOR' => 'XOR'),
    '#default_value' => $options['grouping'],
    '#required' => TRUE,
  );

  $entity = entity_get_info($form['#flexiform_dependee']->getEntityType());

  $form['entity_edit'] = array(
    '#type' => 'fieldset',
    '#title' => t('Edit context settings'),
    '#description' => t('These settings apply when the @entity is being added or edited in a form.', array('@entity' => drupal_strtolower($entity['label']))),
    '#collapsible' => FALSE,
  );

  $form['entity_edit']['state'] = array(
    '#type' => 'select',
    '#title' => t('Form state'),
    '#description' => t('The Javascript form state that is applied to the dependent field when the condition is met. Note: this has no effect on server-side logic and validation.'),
    '#options' => conditional_fields_states(),
    '#default_value' => $options['state'],
    '#required' => TRUE,
    '#ajax' => array(
      'callback' => 'conditional_fields_ajax_admin_state_callback',
      'wrapper' => 'effects-wrapper',
    ),
  );

  $effects = $effects_options = array();
  $selected_state = isset($form_state['values']['state']) ? $form_state['values']['state'] : $options['state'];
  foreach (conditional_fields_effects() as $effect_name => $effect) {
    if (in_array($selected_state, $effect['states'])) {
      $effects[$effect_name] = $effect['label'];
      if (isset($effect['options'])) {
        $effects_options[$effect_name] = $effect['options'];
      }
    }
  }

  $form['entity_edit']['effects_wrapper'] = array(
    '#type' => 'container',
    '#attributes' => array(
      'id' => 'effects-wrapper',
    ),
  );

  $effect = isset($form_state['values']['effect']) ? $form_state['values']['effect'] : $options['effect'];

  if (count($effects) == 1) {
    $effects_keys = array_keys($effects);
    $form['entity_edit']['effects_wrapper']['effect'] = array(
      '#type' => 'hidden',
      '#value' => array_shift($effects_keys),
      '#default_value' => array_shift($effects_keys),
    );
  }
  elseif (count($effects) > 1) {
    $form['entity_edit']['effects_wrapper']['effect'] = array(
      '#type' => 'select',
      '#title' => t('Effect'),
      '#description' => t('The effect that is applied to the dependent when its state is changed.'),
      '#options' => $effects,
      '#default_value' => $effect,
      '#states' => array(
        'visible' => array(
          ':input[name="state"]' => array(
            array('value' => 'visible'),
            array('value' => '!visible'),
          ),
        ),
      ),
    );
  }

  $form['entity_edit']['effects_wrapper']['effect_options'] = array('#tree' => TRUE);

  foreach ($effects_options as $effect_name => $effect_options) {
    foreach ($effect_options as $effect_option_name => $effect_option) {
      $effect_option += array(
        '#title' => t('@effect effect option: @effect_option', array('@effect' => $effects[$effect_name], '@effect_option' => $effect_option_name)),
        '#states' => array(
          'visible' => array(
            ':input[name="effect"]' => array(
              array('value' => $effect_name),
            ),
          ),
        ),
      );

      if (isset($form_state['values']['effect_options'][$effect_name][$effect_option_name])) {
        $effect_option['#default_value'] = $form_state['values']['effect_options'][$effect_name][$effect_option_name];
      }
      elseif ($options['effect'] == $effect_name) {
        $effect_option['#default_value'] = $options['effect_options'][$effect_option_name];
      }

      $form['entity_edit']['effects_wrapper']['effect_options'][$effect_name][$effect_option_name] = $effect_option;
    }
  }

  $form['entity_edit']['element_edit_per_role'] = array(
    '#type' => 'checkbox',
    '#title' => t('Activate per user role settings in edit context'),
    '#description' => t('If the user has more than one role, the first matching role will be used.'),
    '#default_value' => $options['element_edit_per_role'],
  );

  $behaviors = conditional_fields_behaviors();

  $form['entity_edit']['element_edit'] = array(
    '#type' => 'checkboxes',
    '#title' => t('Edit context settings for all roles'),
    '#title_display' => 'invisible',
    '#options' => $behaviors['edit'],
    '#default_value' => $options['element_edit'],
    '#states' => array(
      'visible' => array(
        ':input[name="element_edit_per_role"]' => array('checked' => FALSE),
      ),
    ),
  );

  $roles = user_roles();
  $element_edit_roles = array('element_edit_roles' => array('#tree' => TRUE));
  foreach ($roles as $rid => $role) {
    $element_edit_roles['element_edit_roles'][$rid] = array(
      '#type' => 'checkboxes',
      '#title' => t('Edit context settings for %role', array('%role' => $role)),
      '#options' => $behaviors['edit'],
      '#default_value' => isset($options['element_edit_roles'][$rid]) ? $options['element_edit_roles'][$rid] : $options['element_edit'],
      '#states' => array(
        'visible' => array(
          ':input[name="element_edit_per_role"]' => array('checked' => TRUE),
        ),
      ),
    );
  }

  array_push($form['entity_edit'], $element_edit_roles);

  $form['entity_edit']['dependency_advanced'] = array(
    '#type' => 'fieldset',
    '#title' => t('Advanced edit context settings', array('@entity' => drupal_strtolower($entity['label']))),
    '#collapsible' => TRUE,
    '#collapsed' => TRUE,
  );

  $selector_description = t('Only use if you know what you are doing, otherwise leave the field empty to let the dependency use an automatically generated selector.');
  $selector_description .= '<br />' . t('You can use the following placeholders:');
  $selector_description .= "<ul>\n";
  $selector_description .= '<li>' . t('%lang: current language of the field.') . "</li>\n";
  $selector_description .= '<li>' . t('%key: part identifier for fields composed of multiple form elements, like checkboxes.') . "</li>\n";
  $selector_description .= '</ul>';

  $form['entity_edit']['dependency_advanced']['selector'] = array(
    '#type' => 'textfield',
    '#title' => t('Custom jQuery selector for dependee'),
    '#description' => $selector_description,
    '#default_value' => $options['selector'],
  );

  $form['actions'] = array(
    '#type' => 'actions',
    'save' => array(
      '#type' => 'submit',
      '#value' => t('Save settings'),
    ),
  );

  // Redirect to bundle dependencies form if destination is set.
  $destination = drupal_get_destination();
  if ($destination['destination'] != 'admin/structure/dependencies') {
    $form_state['redirect'] = $destination['destination'];
  }

  return $form;
}

/**
 * Form submit to edit conditional fields.
 */
function flexiform_conditional_fields_configure_form_submit($form, &$form_state) {
  $dependent = $form['#flexiform_element']->getElementNamespace();
  $dependee = $form['#flexiform_dependee']->getElementNamespace();
  $dependency_key = $form['#dependency_key'];

  if ($form['#flexiform_dependee'] instanceof FlexiformElementFieldAPIInterface) {
    $dependee_field = $form['#flexiform_dependee']->getField();
    $widget_type = $form_state['field'][$dependee_field['field_name']][LANGUAGE_NONE]['instance']['widget']['type'];
  }

  // TODO: validate values against allowed values.
  // TODO: unify 'value' and 'values' fields.

  $options = array(
    'state'                 => $form_state['values']['state'],
    'condition'             => $form_state['values']['condition'],
    'grouping'              => $form_state['values']['grouping'],
    'effect'                => isset($form_state['values']['effect']) ? $form_state['values']['effect'] : FALSE,
    'effect_options'        => isset($form_state['values']['effect'], $form_state['values']['effect_options'][$form_state['values']['effect']]) ? $form_state['values']['effect_options'][$form_state['values']['effect']] : array(),
    'element_edit'          => $form_state['values']['element_edit'],
    'element_edit_per_role' => $form_state['values']['element_edit_per_role'],
    'element_edit_roles'    => $form_state['values']['element_edit_roles'],
    'selector'              => $form_state['values']['selector'],
  );

  // Store values only if the condition requires them.
  if ($dependee_field && $form_state['values']['condition'] == 'value') {

    $options += array(
      'values_set' => $form_state['values']['values_set'],
      // Raw user input is needed to build actual value conditions in the
      // #states array, so we store it in 'value_form'.
      'value_form' => $form_state['input']['value']['field'][$dependee_field['field_name']][LANGUAGE_NONE],
    );

    if ($options['values_set'] == CONDITIONAL_FIELDS_DEPENDENCY_VALUES_WIDGET) {
      $value = array();
      field_default_extract_form_values(NULL, NULL, array('field_name' => $dependee_field['field_name']), NULL, LANGUAGE_NONE, $value, $form['value']['field'], $form_state);
      $options['value'] = $value;
    }
    elseif ($options['values_set'] == CONDITIONAL_FIELDS_DEPENDENCY_VALUES_REGEX) {
      // The regular expression is inserted in an associative array, so it will
      // be converted to a Javascript object.
      // In states_enhancements.js we will then use the "RegExp" key to
      // distinguish the object and evaluate it as a regular expression.
      $options['value'] = array('RegExp' => $form_state['values']['regex']);
    }
    else {
      $values = explode("\n", $form_state['values']['values']);
      $values = array_map('trim', $values);
      $values = array_filter($values, 'strlen');
      $options['values'] = $values;
    }
  }

  $options += conditional_fields_dependency_default_options();

  $form['#flexiform']->elements[$dependent]['conditional_fields'][$dependency_key] = $options;
  $form['#flexiform']->save();
  drupal_set_message(t('Saved dependency configuration.'));
}

/**
 * Form to delete conditional fields.
 */
function flexiform_conditional_fields_delete_form($form, &$form_state, $flexiform, $element_namespace, $dependency_key) {
  $destination = drupal_get_destination();
  list($dependee) = explode('|', $dependency_key);

  $form['#flexiform'] = $flexiform;
  $form['#flexiform_element'] = FlexiformElement::getElement($flexiform, $element_namespace);
  $form['#flexiform_dependee'] = FlexiformElement::getElement($flexiform, $dependee);
  $form['#dependency_key'] = $dependency_key;
  return confirm_form(
    $form,
    t('Are you sure you want to delete this dependency?'),
    $destination['destination'],
    t('This action cannot be undone.'),
    t('Delete dependency'),
    t('Cancel')
  );
}

/**
 * Form submit to delete conditional fields.
 */
function flexiform_conditional_fields_delete_form_submit($form, &$form_state) {
  $dependent = $form['#flexiform_element']->getElementNamespace();
  $dependency_key = $form['#dependency_key'];
  unset($form['#flexiform']->elements[$dependent]['conditional_fields'][$dependency_key]);
  $form['#flexiform']->save();
}

<?php

/**
 * @file
 * Model type editing UI.
 */

/**
 * UI controller.
 */
class FlexiformUIController extends EntityDefaultUIController {

  /**
   * Overrides operationCount();
   */
  protected function operationCount() {
    $count = 6;
    $count += !empty($this->entityInfo['i18n controller class']) ? 1 : 0;
    return $count;
  }

  /**
   * {@inheritdoc}
   */
  public function overviewForm($form, &$form_state, $conditions = array()) {
    $collapsed = TRUE;

    $tags = array();
    if (!empty($_GET['tags'])) {
      $tags = $conditions['tags'] = drupal_explode_tags($_GET['tags']);
      $collapsed = FALSE;
    }

    $form['filter'] = array(
      '#type' => 'fieldset',
      '#title' => t('Filter'),
      '#collapsible' => TRUE,
      '#collapsed' => $collapsed,
    );
    $form['filter']['#id'] = 'flexiform-filter-form';
    $form['filter']['tags'] = array(
      '#title' => t('Filter by tag'),
      '#type' => 'textfield',
      '#default_value' => drupal_implode_tags($tags),
      '#autocomplete_path' => 'admin/structure/flexiforms/autocomplete_tags',
    );
    $form['filter']['submit'] = array(
      '#type' => 'submit',
      '#value' => t('Filter'),
      '#name' => '', // prevent from showing up in $_GET.
    );

    // Overridden to allow the passing of conditions through.
    $form['table'] = $this->overviewTable($conditions);
    $form['pager'] = array('#theme' => 'pager');

    $form['#method'] = 'get';
    $form['#submit'][] = 'flexiform_form_submit_rebuild';
    return $form;
  }

  /**
   * Overrides overviewTable();
   */
  public function overviewTable($conditions = array()) {
    $entities = entity_load($this->entityType, FALSE, $conditions);
    ksort($entities);

    $rows = array();
    foreach ($entities as $entity) {
      $rows[] = $this->overviewTableRow($conditions, entity_id($this->entityType, $entity), $entity);
    }

    $render = array(
      '#theme' => 'table',
      '#header' => $this->overviewTableHeaders($conditions, $rows),
      '#rows' => $rows,
      '#empty' => t('None.'),
    );
    return $render;
  }

  /**
   * Overrides overviewTableHeaders();
   */
  protected function overviewTableHeaders($conditions, $rows, $additional_header = array()) {
    $additional_header = array(
      t('Base Entity'),
      t('Bundle'),
    );
    return parent::overviewTableHeaders($conditions, $rows, $additional_header);
  }

  /**
   * Overrides overviewTableRow();
   */
  protected function overviewTableRow($conditions, $id, $entity, $additional_cols = array()) {
    $row = array();
    // Take the first column as usual.
    $row[] = array(
      'data' => array(
        '#theme' => 'flexiform_ui_overview_item',
        '#label' => entity_label($this->entityType, $entity),
        '#tags' => drupal_implode_tags($entity->tags),
        '#name' => !empty($this->entityInfo['exportable']) ? entity_id($this->entityType, $entity) : FALSE,
        '#url' => array(
          'path' => $this->path . '/manage/' . $id,
          'options' => array(),
        ),
        '#entity_type' => $this->entityType,
      ),
    );

    // Add the base entity.
    $base_info = entity_get_info($entity->base_entity);
    $row[] = $base_info['label'];
    $row[] = $base_info['bundles'][$entity->base_entity_bundle]['label'];

    // Add in any passed additional cols.
    foreach ($additional_cols as $col) {
      $row[] = $col;
    }

    $row[] = array('data' => array(
      '#theme' => 'entity_status',
      '#status' => $entity->{$this->statusKey},
    ));

    $i18n = !empty($this->entityInfo['i18n controller class']);

    // Add operations depending on the status.
    if (entity_has_status($this->entityType, $entity, ENTITY_FIXED)) {
      $row[] = array(
        'data' => l(t('clone'), $this->path . '/manage/' . $id . '/clone'),
        'colspan' => $this->operationCount(),
      );
    }
    else {
      $row[] = l(t('edit'), $this->path . '/manage/' . $id);
      $row[] = l(t('manage form fields'), $this->path . '/manage/' . $id . '/form-fields');
      $row[] = l(t('manage form entities'), $this->path . '/manage/' . $id . '/form-entities');

      if ($i18n) {
        $row[] = l(t('translate'), $this->path . '/manage/' . $id . '/translate');
      }
      $row[] = l(t('clone'), $this->path . '/manage/' . $id . '/clone');

      if (empty($this->entityInfo['exportable']) || !entity_has_status($this->entityType, $entity, ENTITY_IN_CODE)) {
        $row[] = l(t('delete'), $this->path . '/manage/' . $id . '/delete', array('query' => drupal_get_destination()));
      }
      elseif (entity_has_status($this->entityType, $entity, ENTITY_OVERRIDDEN)) {
        $row[] = l(t('revert'), $this->path . '/manage/' . $id . '/revert', array('query' => drupal_get_destination()));
      }
      else {
        $row[] = '';
      }
    }
    $row[] = l(t('export'), $this->path . '/manage/' . $id . '/export');

    return $row;
  }

  /**
   * {@inheritdoc}
   */
  public function hook_forms() {
    $forms = parent::hook_forms();
    $forms['flexiform_overview_form']['callback'] = 'flexiform_overview_form';

    return $forms;
  }

  /**
   * Overrides hook_menu() defaults.
   */
  public function hook_menu() {
    $wildcard = '%flexiform';
    $items = parent::hook_menu();
    $items[$this->path]['description'] = 'Manage flexiforms, including adding
		and removing fields and the display of fields.';
    // Change the way they clone things.
    $items["{$this->path}/manage/%entity_object/clone"]['page callback'] = 'flexiform_ui_get_clone_form';

    // Tags autocomplete pages.
    $items["{$this->path}/autocomplete_tags"] = array(
      'title' => 'Flexiform tags autocomplete',
      'page callback' => 'flexiform_autocomplete_tags',
      'page arguments' => array(4),
      'type' => MENU_CALLBACK,
      'file' => 'flexiform.admin.inc',
      'file path' => drupal_get_path('module', 'flexiform'),
      'access arguments' => array('administer flexiforms'),
    );

    $items["{$this->path}/manage/{$wildcard}/form-fields"] = array(
      'title' => 'Manage form fields',
      'page callback' => 'drupal_get_form',
      'page arguments' => array('flexiform_manage_form_fields_form', 4),
      'type' => MENU_LOCAL_TASK,
      'weight' => 2,
      'file' => 'flexiform.admin.inc',
      'file path' => drupal_get_path('module', 'flexiform'),
      'access arguments' => array('administer flexiforms'),
    );

    $items["{$this->path}/manage/{$wildcard}/form-fields/%flexiform_formfield"] = array(
      'title' => 'Edit Field',
      'page callback' => 'drupal_get_form',
      'page arguments' => array('flexiform_field_configure_form', 4, 6),
      'file' => 'flexiform.admin.inc',
      'file path' => drupal_get_path('module', 'flexiform'),
      'access arguments' => array('administer flexiforms'),
    );

    $items["{$this->path}/manage/{$wildcard}/form-fields/%flexiform_formfield/edit"] = array(
      'title' => 'Configure',
      'page callback' => 'drupal_get_form',
      'page arguments' => array('flexiform_field_configure_form', 4, 6),
      'file' => 'flexiform.admin.inc',
      'file path' => drupal_get_path('module', 'flexiform'),
      'type' => MENU_DEFAULT_LOCAL_TASK,
      'access arguments' => array('administer flexiforms'),
    );

    $items["{$this->path}/manage/{$wildcard}/form-fields/%flexiform_formfield/widget-type"] = array(
      'title' => 'Widget type',
      'page callback' => 'drupal_get_form',
      'page arguments' => array('flexiform_field_widget_type_form', 4, 6),
      'access callback' => 'flexiform_field_widget_type_form_access',
      'access arguments' => array(4, 6),
      'file' => 'flexiform.admin.inc',
      'file path' => drupal_get_path('module', 'flexiform'),
      'type' => MENU_LOCAL_TASK,
    );

    $items["{$this->path}/manage/{$wildcard}/form-fields/%flexiform_formfield/remove"] = array(
      'title' => 'Remove',
      'page callback' => 'drupal_get_form',
      'page arguments' => array('flexiform_field_remove_form', 4, 6),
      'file' => 'flexiform.admin.inc',
      'file path' => drupal_get_path('module', 'flexiform'),
      'type' => MENU_LOCAL_TASK,
      'access arguments' => array('administer flexiforms'),
    );

    $items["{$this->path}/manage/{$wildcard}/form-entities"] = array(
      'title' => 'Manage form entities',
      'page callback' => 'drupal_get_form',
      'page arguments' => array('flexiform_manage_form_entities_form', 4),
      'type' => MENU_LOCAL_TASK,
      'weight' => 3,
      'file' => 'flexiform.admin.inc',
      'file path' => drupal_get_path('module', 'flexiform'),
      'access arguments' => array('administer flexiforms'),
    );

    $items["{$this->path}/manage/{$wildcard}/form-entities/%"] = array(
      'title' => 'Configure',
      'page callback' => 'drupal_get_form',
      'page arguments' => array('flexiform_entity_configure_form', 4, 6),
      'type' => MENU_CALLBACK,
      'weight' => 3,
      'file' => 'flexiform.admin.inc',
      'file path' => drupal_get_path('module', 'flexiform'),
      'access arguments' => array('administer flexiforms'),
    );

    $items["{$this->path}/manage/{$wildcard}/form-entities/%/configure"] = array(
      'title' => 'Configure',
      'type' => MENU_DEFAULT_LOCAL_TASK,
      'weight' => 3,
    );

    $items["{$this->path}/manage/{$wildcard}/form-entities/%/remove"] = array(
      'title' => 'Remove',
      'page callback' => 'drupal_get_form',
      'page arguments' => array('flexiform_entity_remove_form', 4, 6),
      'type' => MENU_LOCAL_TASK,
      'weight' => 4,
      'file' => 'flexiform.admin.inc',
      'file path' => drupal_get_path('module', 'flexiform'),
      'access arguments' => array('administer flexiforms'),
    );

    if (module_exists('rules_admin')) {
      $items["{$this->path}/manage/{$wildcard}/rules/add-action"] = array(
        'title' => 'Add Action',
        'page callback' => 'drupal_get_form',
        'page arguments' => array('rules_admin_add_reaction_rule', "{$this->path}/manage/{$wildcard}/rules/add-action"),
        'access callback' => TRUE,
        'file' => 'rules_admin.inc',
        'file path' => drupal_get_path('module', 'rules_admin'),
      );
    }

    // Generate paths for all other entity types with field UI.
    // @see field_ui_menu
    $entity_info = entity_get_info();
    foreach ($entity_info as $entity_type => $info) {
      // Only do this if the bundle is set.
      if (empty($info['bundles'])) {
        continue;
      }

      foreach ($info['bundles'] as $bundle_name => $bundle_info) {
        // Only act if there is an admin page set.
        if (empty($bundle_info['admin'])) {
          continue;
        }
        // Get the path for this bundle.
        $path = $bundle_info['admin']['path'];
        // The bundle arg could be either a position in the path or an actual
        // bundle.
        if (isset($bundle_info['admin']['bundle argument'])) {
          $bundle_arg = $bundle_info['admin']['bundle argument'];
        }
        else {
          $bundle_arg = $bundle_name;
        }
        // Extract access information, providing defaults.
        $access = array_intersect_key($bundle_info['admin'], drupal_map_assoc(array('access callback', 'access arguments')));
        $access += array(
          'access callback' => 'user_access',
          'access arguments' => array('administer site configuration'),
        );

        $items["$path/forms"] = array(
          'title' => t('Manage Forms'),
          'type' => MENU_LOCAL_TASK,
          'page callback' => 'drupal_get_form',
          'page arguments' => array('flexiform_overview_form', 'flexiform', $entity_type, $bundle_arg),
          'description' => 'Manage ' . $info['label'] . ' Forms',
          'weight' => 3,
          'file' => 'includes/entity.ui.inc',
        )+$access;

        $items["$path/forms/add"] = array(
          'title' => t('Add Form'),
          'type' => MENU_LOCAL_ACTION,
          'page callback' => 'flexiform_add_for_entity',
          'page arguments' => array($entity_type, $bundle_arg),
          'description' => t('Add Form'),
          'file' => 'flexiform.admin.inc',
          'file path' => drupal_get_path('module', 'flexiform'),
        )+$access;
      }
    }

    return $items;
  }
}

/**
 * Form builder function for the flexiform overview form.
 */
function flexiform_overview_form($form, &$form_state, $entity_type = 'flexiform', $base_entity = '', $base_entity_bundle = '') {
  // Build a conditions array.
  $conditions = array();
  if (!empty($base_entity)) {
    $conditions['base_entity'] = $base_entity;
  }
  if (!empty($base_entity) && !empty($base_entity_bundle)) {
    $base_entity_bundle = field_extract_bundle($base_entity, $base_entity_bundle);
    $conditions['base_entity_bundle'] = $base_entity_bundle;
  }

  return entity_ui_controller($entity_type)->overviewForm($form, $form_state, $conditions);
}

/**
 * Form wrapper to add a flexiform for a given entity type and bundle.
 */
function flexiform_add_for_entity($entity_type, $bundle) {
  $bundle = field_extract_bundle($entity_type, $bundle);

  $values = array(
    'base_entity' => $entity_type,
    'base_entity_bundle' => $bundle,
  );
  $flexiform = entity_create('flexiform', $values);

  $info = entity_get_info($entity_type);
  if (!empty($info['bundles'][$bundle]['admin']['real path'])) {
    $form_state['redirect'] = $info['bundles'][$bundle]['admin']['real path'] . '/forms';
  }

  // Fix the entity and bundle.
  $form_state['fix_entity_bundle'] = TRUE;

  return entity_ui_get_form('flexiform', $flexiform, 'add', $form_state);
}

/**
 * Generates the model type editing form.
 */
function flexiform_form($form, &$form_state, $flexiform, $op = 'edit') {
  $form['#flexiform'] = $flexiform;

  if ($op == 'clone') {
    $form['#cloned_from'] = $flexiform->cloned_from;

    $flexiform->label .= ' (cloned)';
    $flexiform->form = '';
  }

  $form['label'] = array(
    '#title' => t('Label'),
    '#type' => 'textfield',
    '#default_value' => $flexiform->label,
    '#description' => t('The human-readable name of this flexiform.'),
    '#required' => TRUE,
    '#size' => 30,
  );

  // Machine-readable type name.
  $form['form'] = array(
    '#type' => 'machine_name',
    '#default_value' => isset($flexiform->form) ? $flexiform->form : '',
    '#maxlength' => 32,
    '#machine_name' => array(
      'exists' => 'flexiform_get_flexiforms',
      'source' => array('label'),
    ),
    '#description' => t('A unique machine-readable name for this flexiform. It must only contain lowercase letters, numbers, and underscores.'),
    '#disabled' => empty($flexiform->is_new),
  );

  $entity_info = entity_get_info();
  $entity_type_options = array();
  foreach ($entity_info as $type => $info) {
    $entity_type_options[$type] = $info['label'];
  }
  $form['base_entity'] = array(
    '#type' => 'select',
    '#title' => t('Base Entity'),
    '#description' => t('The base entity type for this form. Other entities can be added based on relationships to this entity.'),
    '#options' => $entity_type_options,
    '#empty_option' => t('- Select an Entity Type -'),
    '#default_value' => !empty($flexiform->base_entity) ? $flexiform->base_entity : FALSE,
    '#required' => TRUE,
    '#ajax' => array(
      'event' => 'change',
      'method' => 'replace',
      'wrapper' => 'flexiform-base-entity-bundle-selector',
      'callback' => 'flexiform_form_base_entity_bundle_selector',
    ),
    '#disabled' => empty($flexiform->is_new) || !empty($form_state['fix_entity_bundle']) || $op == 'clone',
  );

  if (isset($form_state['values']['base_entity'])) {
    $entity_type = $form_state['values']['base_entity'];
  }
  else if (!empty($flexiform->base_entity)) {
    $entity_type = $flexiform->base_entity;
  }
  else {
    $entity_type = NULL;
  }

  $bundle_options = array();
  if ($entity_type) {
    $entity_info = entity_get_info($entity_type);
    foreach ($entity_info['bundles'] as $bundle => $info) {
      $bundle_options[$bundle] = $info['label'];
    }
  }

  $default_bundle = FALSE;
  if (!empty($flexiform->base_entity) && !empty($flexiform->base_entity_bundle)
    && ($flexiform->base_entity == $entity_type)) {
    $default_bundle = $flexiform->base_entity_bundle;
  }
  $form['base_entity_bundle'] = array(
    '#type' => 'select',
    '#title' => t('Base Entity Bundle'),
    '#description' => t('The base entity bundle for this form.'),
    '#options' => $bundle_options,
    '#empty_option' => t('- Select a Bundle -'),
    '#default_value' => $default_bundle,
    '#required' => TRUE,
    '#prefix' => '<div id="flexiform-base-entity-bundle-selector">',
    '#suffix' => '</div>',
    '#ajax' => array(
      'event' => 'change',
      'method' => 'replace',
      'wrapper' => 'flexiform-builder-selector',
      'callback' => 'flexiform_form_builder_selector',
    ),
    '#disabled' => empty($flexiform->is_new) || !empty($form_state['fix_entity_bundle']) || $op == 'clone',
  );

  if (isset($form_state['values']['base_entity_bundle'])) {
    $bundle = $form_state['values']['base_entity_bundle'];
  }
  else if (!empty($flexiform->base_entity_bundle)) {
    $bundle = $flexiform->base_entity_bundle;
  }
  else {
    $bundle = NULL;
  }

  $builder_options = array();
  $builder_description = '';
  if ($bundle && $entity_type) {
    $builder_info = flexiform_entity_type_get_builders($entity_type);
    foreach ($builder_info as $builder => $info) {
      $builder_options[$builder] = $info['label'];
      $builder_description .= t('%label: @desc<br />', array('%label' => $info['label'], '@desc' => $info['description']));
    }
  }

  $default_builder = FALSE;
  $default_builder_desc = '';
  if (!empty($flexiform->base_entity) && !empty($flexiform->builder)
    && ($flexiform->base_entity == $entity_type)) {
    $default_builder = $flexiform->builder;
  }
  $form['builder'] = array(
    '#type' => 'select',
    '#title' => t('Builder'),
    '#description' => t('The builder to use to render this form.<br />!builder_desc', array('!builder_desc' => $builder_description)),
    '#options' => $builder_options,
    '#empty_option' => t('- Select a Builder -'),
    '#default_value' => $default_builder,
    '#required' => TRUE,
    '#prefix' => '<div id="flexiform-builder-selector">',
    '#suffix' => '</div>',
    '#disabled' => $op == 'clone',
  );

  $form['form_group'] = array(
    '#title' => t('Group'),
    '#description' => t('Select a group for this flexiform. Some groups do special things with forms.'),
  );

  $groups = flexiform_get_groups();
  if (!empty($flexiform->is_new) && count($groups) > 1) {
    $form['form_group']['#type'] = 'select';
    $form['form_group']['#default_value'] = isset($flexiform->form_group) ? $flexiform->form_group: FALSE;
    $form['form_group']['#required'] = TRUE;
    foreach ($groups as $group => $info) {
      $form['form_group']['#options'][$group] = $info['label'];
    }
  }
  else {
    $group = !empty($flexiform->form_group) ? $flexiform->form_group: key($groups);

    $form['form_group_display'] = $form['form_group'];
    $form['form_group_display']['#type'] = 'item';
    $form['form_group_display']['#markup'] = $groups[$group]['label'];

    $form['form_group']['#type'] = 'value';
    $form['form_group']['#value'] = $group;
  }

  // Add tags element
  $form['tags'] = array(
    '#type' => 'textfield',
    '#title' => t('Tags'),
    '#default_value' => isset($flexiform->tags) ? drupal_implode_tags($flexiform->tags) : '',
    '#autocomplete_path' => 'admin/structure/flexiforms/autocomplete_tags',
    '#description' => t('Tags associated with this flexible form. Separate multiple tags with commas.'),
  );

  // Displays Section of the form.
  $form['displays'] = array(
    '#type' => 'fieldset',
    '#title' => t('Displays'),
    '#description' => t('Configure places this form appears on your site.'),
    '#tree' => TRUE,
  );

  foreach (flexiform_display_info() as $display => $info) {
    $controller = $flexiform->getDisplay($display);
    if (get_class($controller) == 'FlexiformDisplayNull') {
      $controller = $flexiform->createDisplay($display);
    }

    $displayForm = array(
      '#type' => 'fieldset',
      '#collapsible' => TRUE,
      '#title' => $info['label'],
      '#collapsed' => !$controller->isEnabled(),
      '#description' => $info['description'],
    );
    $form['displays'][$display] = $controller->configForm($displayForm, $form_state);
  }

  $form['actions'] = array('#type' => 'actions');
  $form['actions']['submit'] = array(
    '#type' => 'submit',
    '#value' => t('Save Flexiform'),
    '#weight' => 40,
  );

  // Submit Rule Actions
  // @todo: Remove dependency on rules admin by implement our own form for
  //        adding submit rules.
  if (module_exists('rules_admin') && !empty($flexiform->form)) {
    $form['rules']['#type'] = 'fieldset';
    $form['rules']['#title'] = t('Submit Actions');
    $form['rules']['#collapsible'] = TRUE;
    $form['rules']['#collapsed'] = TRUE;
    $form['rules']['title']['#markup'] = '<h3>' . t('Submit Actions') . '</h3>';
    $conditions = array(
      'plugin' => 'reaction rule',
      'active' => TRUE,
      'event' => 'flexiform_submit_' . $flexiform->form,
    );
    $options = array(
      'show plugin' => FALSE,
      'base path' => current_path(),
    );

    $form['rules']['active'] = rules_ui()->overviewTable($conditions, $options);
    $form['rules']['active']['#caption'] = t('Active Actions');
    $form['rules']['active']['#empty'] = t('There are no active rules. <a href="!url">Add new Action</a>.', array('!url' => url('admin/config/workflow/rules/reaction/add')));

    $conditions['active'] = FALSE;
    $form['rules']['inactive'] = rules_ui()->overviewTable($conditions, $options);
    $form['rules']['inactive']['#caption'] = t('Inactive Actions');
    $form['rules']['inactive']['#empty'] = t('There are no inactive rules.');
  }



  return $form;
}

/**
 * AJAX Callback to build the base entity bundle select list.
 */
function flexiform_form_base_entity_bundle_selector($form, $form_state) {
  return $form['base_entity_bundle'];
}

/**
 * AJAX Callback to build the builder select list.
 */
function flexiform_form_builder_selector($form, $form_state) {
  return $form['builder'];
}

/**
 * Form API submit callback for the type form.
 */
function flexiform_form_submit(&$form, &$form_state) {
  $flexiform = entity_ui_form_submit_build_entity($form, $form_state);

  // Sort out tags.
  $flexiform->tags = drupal_explode_tags($form_state['values']['tags']);

  $base_entity_info = entity_get_info($flexiform->base_entity);
  if (empty($flexiform->entities)) {
    $flexiform->entities = array(
      'base_entity' => array(
        'label' => t('Base @entity_type', array('@entity_type' => $base_entity_info['label'])),
        'entity_type' => $flexiform->base_entity,
        'bundle' => $flexiform->base_entity_bundle,
        'create' => TRUE,
        'getter' => 'base_entity',
        'weight' => -10,
      ),
    );
  }

  if (empty($flexiform->elements)) {
    $flexiform->elements = array();
  }

  $flexiform->save();
  if (empty($form_state['redirect'])) {
    $form_state['redirect'] = 'admin/structure/flexiforms';
  }

  $form['#flexiform'] = $flexiform;
}

/**
 * Form API submit callback for the delete button.
 */
function flexiform_form_submit_delete(&$form, &$form_state) {
  $form_state['redirect'] = 'admin/structure/flexiforms/manage/' . $form_state['flexiform']->type . '/delete';
}

/**
 * flexiform_manage_form_fields_form
 *
 * This form configures the fields on an flexiform. You can choose them
 * rearrange them and override verious instance settings through this form.
 */
function flexiform_manage_form_fields_form($form, &$form_state, $flexiform) {
  // Get list of entities
  $entities = $flexiform->entities;

  // Get the fields in the flexiform deff
  $elements = array();
  foreach ($flexiform->elements as $element_namespace => $settings) {
    $element = FlexiformElement::getElement($flexiform, $element_namespace);
    $elements[$element->getElementNamespace()] = $element;
  };

  $form += array(
    '#flexiform' => $flexiform,
    '#elements' => array_keys($elements),
  );

  $form_state += array(
    '#flexiform' => $flexiform,
    '#elements' => $elements,
  );

  $table = array(
    '#type' => 'flexiform_field_table',
    '#tree' => TRUE,
    '#header' => array(
      t('Label'),
      t('Weight'),
      t('Parent'),
      t('Type'),
      t('Entity'),
      t('Name'),
      t('Widget'),
      t('Operations'),
    ),
    '#parent_options' => array(),
    '#regions' => array(
      'main' => array('message' => t('No fields are present yet')),
      'add_new' => array('title' => '&nbsp;'),
    ),
    '#attributes' => array(
      'class' => array('field-ui-overview'),
      'id' => 'field-overview',
    ),
  );

  foreach($elements as $element_namespace => $element) {
    $element_path = 'admin/structure/flexiforms/manage/' . $flexiform->form . '/form-fields/' . $element_namespace;

    if ($element instanceof FlexiformElementField) {
      $widget = array(
        '#type' => 'link',
        '#title' => t($element->getWidgetLabel()),
        '#href' => $element_path . '/widget-type',
        '#options' => array('attributes' => array('title' => t('Change widget type.'))),
      );
    }
    else {
      $widget = array(
        '#type' => 'markup',
        '#markup' => '',
      );
    }

    $table[$element_namespace] = array(
      '#attributes' => array('class' => array('draggable', 'tabledrag-leaf')),
      '#row_type' => 'field',
      '#flexiform_element' => $element,
      '#region_callback' => 'flexiform_field_form_row_region',
      'label' => array(
        '#markup' => $element->label(),
      ),
      'weight' => array(
        '#type' => 'textfield',
        '#title' => t('Weight for @title', array('@title' => $element->label())),
        '#title_display' => 'invisible',
        '#default_value' => $element->getWeight(),
        '#size' => 3,
        '#attributes' => array('class' => array('field-weight')),
      ),
      'parent_wrapper' => array(
        'parent' => array(
          '#type' => 'select',
          '#title' => t('Parent for @title', array('@title' =>  $element->label())),
          '#title_display' => 'invisible',
          '#options' => $table['#parent_options'],
          '#empty_value' => '',
          '#attributes' => array('class' => array('field-parent')),
          '#parents' => array('fields', $element_namespace, 'parent'),
        ),
        'hidden_name' => array(
          '#type' => 'hidden',
          '#default_value' => $element_namespace,
          '#attributes' => array('class' => array('field-name')),
        ),
      ),
      'type' => array(
        '#markup' => $element->type(),
      ),
      'entity' => array(
        '#markup' => $element->getEntityNamespace(),
      ),
      'field' => array(
        '#markup' => $element->name(),
      ),
      'widget_type' => $widget,
      'operations' => array(
        '#theme' => 'item_list',
        '#items' => array(
          'configure' => l(t('configure'), $element_path, array('attributes' => array('title' => t('Configure settings for this form')))),
          'remove' => l(t('remove'), $element_path . '/remove', array('attributes' => array('title' => t('Remove this element.')))),
        ),
      ),
    );
  }

  // Additional row: add new field.
  $max_weight = flexiform_field_max_weight($flexiform);
  $entity_options = array();
  foreach($entities as $entity_namespace => $info) {
    $entity_options[$entity_namespace] = $entity_namespace;
  }

  if (isset($form_state['values']['fields']['_add_element']['entity'])) {
    $element_entity_namespace = $form_state['values']['fields']['_add_element']['entity'];
  }
  else {
    $element_entity_namespace = NULL;
  }
  $element_options = flexiform_field_element_options($flexiform, $element_entity_namespace);

  if ($entity_options) {
    $name = '_add_element';
    $table[$name] = array(
      '#attributes' => array('class' => array('draggable', 'tabledrag-leaf', 'add-new')),
      '#row_type' => 'add_field',
      '#region_callback' => 'flexiform_field_form_row_region',
      'label' => array(
        '#type' => 'textfield',
        '#title' => 'Lable',
        '#title_display' => 'invisible',
        '#size' => 15,
        '#description' => t('Label'),
        '#prefix' => '<div class="label-input"><div class="add-new-placeholder">' . t('Add element') .'</div>',
        '#suffix' => '</div>',
      ),
      'weight' => array(
        '#type' => 'textfield',
        '#default_value' => $max_weight + 1,
        '#size' => 3,
        '#title_display' => 'invisible',
        '#title' => t('Weight for element'),
        '#attributes' => array('class' => array('field-weight')),
        '#prefix' => '<div class="add-new-placeholder">&nbsp;</div>',
      ),
      'parent_wrapper' => array(
        'parent' => array(
          '#type' => 'select',
          '#title' => t('Parent for element'),
          '#title_display' => 'invisible',
          '#options' => $table['#parent_options'],
          '#empty_value' => '',
          '#attributes' => array('class' => array('field-parent')),
          '#prefix' => '<div class="add-new-placeholder">&nbsp;</div>',
          '#parents' => array('fields', $name, 'parent'),
        ),
        'hidden_name' => array(
          '#type' => 'hidden',
          '#default_value' => $name,
          '#attributes' => array('class' => array('field-name')),
        ),
      ),
      'type' => array(
        '#markup' => 'Field',
      ),
      'entity' => array(
        '#type' => 'select',
        '#title' => t('The Enitity'),
        '#title_display' => 'invisible',
        '#options' => $entity_options,
        '#empty_option' => t('- Select an Entity -'),
        '#description' => t('Entity from which to take the field'),
        '#attributes' => array('class' => array('entity-select')),
        '#prefix' => '<div class="add-new-placeholder">&nbsp;</div>',
        '#ajax' => array(
          'event' => 'change',
          'method' => 'replace',
          'wrapper' => 'flexiform-element-selector',
          'callback' => 'flexiform_manage_form_fields_elements',
        ),
      ),
      'field' => array(
        '#type' => 'select',
        '#title' => t('The Field'),
        '#title_display' => 'invisible',
        '#options' => $element_options,
        '#empty_option' => t('- Select an Element -'),
        '#description' => t('Pick an Element'),
        '#attributes' => array('class' => array('entity-select')),
        '#prefix' => '<div id="flexiform-element-selector"><div class="add-new-placeholder">&nbsp;</div>',
        '#suffix' => '</div>',
      ),
      'widget' => array(),
      'operations' => array(),
    );
  }

  $form['fields'] = $table;

  $form['actions'] = array('#type' => 'actions');
  $form['actions']['submit'] = array('#type' => 'submit', '#value' => t('Save'));

  $form['#attached']['css'][] = drupal_get_path('module', 'flexiform') . '/flexiform_ui.css';
  $form['#attached']['js'][] = drupal_get_path('module', 'flexiform') . '/flexiform_ui.js';

  // Add tabledrag behavior.
  $form['#attached']['drupal_add_tabledrag'][] = array('field-overview', 'order', 'sibling', 'field-weight');
  $form['#attached']['drupal_add_tabledrag'][] = array('field-overview', 'match', 'parent', 'field-parent', 'field-parent', 'field-name');

  return $form;
}

/**
 * Validate handler for the flexiform_manage_form_fields_form
 */
function flexiform_manage_form_fields_form_validate($form, &$form_state) {
  $element = $form_state['values']['fields']['_add_element'];

  if (array_filter(array($element['label'], $element['entity'], $element['field']))) {
    // Missing Label
    if (!$element['label']) {
      form_set_error('fields][_add_element][label', t('Add element: you must provide a label'));
    }

    if (!$element['entity']) {
      form_set_error('fields][_add_element][entity', t('Add element: you must choose an entity'));
    }

    if (!$element['field']) {
      form_set_error('fields][_add_element][field', t('Add element: you must choose an element'));
    }
  }
}

/**
 * Submit handler for the flexiform_manage_form_fields_form
 */
function flexiform_manage_form_fields_form_submit($form, &$form_state) {
  $form_values = $form_state['values']['fields'];
  $flexiform = $form_state['#flexiform'];
  $elements = $form_state['#elements'];

  $element_path = '';
  foreach($form_values as $element_namespace => $values) {
    if (in_array($element_namespace, $form['#elements'])) {
      $element = $elements[$element_namespace];
      $element->setWeight($values['weight']);
      $flexiform->updateElement($element);
    }
  }

  // Add an element to the form.
  $settings = array();
  if (!empty($form_values['_add_element']['field'])) {
    $values = $form_values['_add_element'];

    $settings['element_name'] = $values['field'];
    $settings['entity_namespace'] = $values['entity'];

    $element = FlexiformElement::createElement($flexiform, $settings);
    $element->setWeight($values['weight']);
    $element->setLabel($values['label']);
    $flexiform->addElement($element);

    // Send the User to the Instance Settings page
  }
  $flexiform->save();

  drupal_set_message(t('Your settings have been saved'));
}

/**
 * Ajax callback to update the elements available
 */
function flexiform_manage_form_fields_elements($form, $form_state) {
  return $form['fields']['_add_element']['field'];
}

/**
 * Configure flexiform field instance settings
 */
function flexiform_field_configure_form($form, &$form_state, $flexiform, $element_namespace) {
  $element = FlexiformElement::getElement($flexiform, $element_namespace);

  $form['#flexiform'] = $flexiform;
  $form['#flexiform_element'] = $element;

  return $element->configureForm($form, $form_state, $flexiform);
}

/**
 * Validate Callback to configure a flexiform field.
 */
function flexiform_field_configure_form_validate($form, &$form_state) {
  $flexiform = $form['#flexiform'];
  $form['#flexiform_element']->configureFormValidate($form, $form_state, $flexiform);
}

/**
 * Submit Callback to configure a flexiform field.
 */
function flexiform_field_configure_form_submit($form, &$form_state) {
  $flexiform = $form['#flexiform'];
  $form['#flexiform_element']->configureFormSubmit($form, $form_state, $flexiform);
}

/**
 * Remove a flexiform field
 */
function flexiform_field_remove_form($form, &$form_state, $flexiform, $element_namespace) {
  $element = FlexiformElement::getElement($flexiform, $element_namespace);

  $form['#flexiform'] = $flexiform;
  $form['#flexiform_element'] = $element;

  return $element->removeForm($form, $form_state, $flexiform);
}

/**
 * Validate Callback to remove a flexiform field.
 */
function flexiform_field_remove_form_validate($form, &$form_state) {
  $flexiform = $form['#flexiform'];
  $form['#flexiform_element']->removeFormValidate($form, $form_state, $flexiform);
}

/**
 * Submit Callback to remove a flexiform field.
 */
function flexiform_field_remove_form_submit($form, &$form_state) {
  $flexiform = $form['#flexiform'];
  $form['#flexiform_element']->removeFormSubmit($form, $form_state, $flexiform);
}

/**
 * Choose a widget type
 */
function flexiform_field_widget_type_form($form, &$form_state, $flexiform, $element_namespace) {
  $element = FlexiformElement::getElement($flexiform, $element_namespace);
  if ($element instanceof FlexiformElementFieldAPIInterface) {
    form_load_include($form_state, 'inc', 'field_ui', 'field_ui.admin');
    $form = field_ui_widget_type_form($form, $form_state, $element->getInstance());

    $form['#flexiform_element'] = $element;
    $form['#flexiform'] = $flexiform;

    $form['#submit'] = array('flexiform_field_widget_type_form_submit');
  }

  return $form;
}

/**
 * Submit callback for the widget type form.
 */
function flexiform_field_widget_type_form_submit($form, &$form_state) {
  $form_values = $form_state['values'];
  $flexiform = $form['#flexiform'];
  $element = $form['#flexiform_element'];

  // Set the right module information.
  $widget_type = field_info_widget_types($form_values['widget_type']);
  $widget_module = $widget_type['module'];

  $instance = &$flexiform->elements[$element->getElementNamespace()]['instance'];
  $instance['widget']['type'] = $form_values['widget_type'];
  $instance['widget']['module'] = $widget_module;
  $flexiform->save();

  // Set a message.
  drupal_set_message(t('The configuration has been saved.'));

  $form_state['redirect'] = 'admin/structure/flexiforms/manage/' . $flexiform->form . '/form-fields';
}

/**
 * flexiform_manage_form_entities_form
 *
 * This form configures the entities involved in a flexiform.
 */
function flexiform_manage_form_entities_form($form, &$form_state, $flexiform) {
  // Get the list of entities
  $entities = $flexiform->entities;

  $form += array(
    '#flexiform' => $flexiform,
    '#entities' => array_keys($entities),
  );

  $form_state += array(
    '#flexiform' => $flexiform,
    '#entities' => $entities,
  );

  $table = array(
    '#type' => 'flexiform_entity_table',
    '#tree' => TRUE,
    '#header' => array(
      t('Label'),
      t('Weight'),
      t('Namespace'),
      t('Entity Type'),
      t('Bundle'),
      t('Getter'),
      array('data' => t('Operations'), 'colspan' => 2),
    ),
    '#parent_options' => array(),
    '#regions' => array(
      'main' => array('message' => t('No entities are present')),
      'add_new' => array('title' => '&nbsp;'),
    ),
    '#attributes' => array(
      'class' => array('field-ui-overview'),
      'id' => 'field-overview',
    ),
  );

  $getters = flexiform_entity_getter_info();
  foreach($entities as $entity_namespace => $settings) {
    $entity_path = 'admin/structure/flexiforms/manage/' . $flexiform->form . '/form-entities/' . $entity_namespace;
    $table[$entity_namespace] = array(
      '#attributes' => array('class' => array('draggable', 'tabledrag-leaf')),
      '#row_type' => 'entity',
      '#region_callback' => 'flexiform_entity_form_row_region',
      'label' => array(
        '#markup' => check_plain($settings['label']),
      ),
      'weight' => array(
        '#type' => 'textfield',
        '#title' => t('Weight for @title', array('@title' => $settings['label'])),
        '#title_display' => 'invisible',
        '#default_value' => $settings['weight'],
        '#size' => 3,
        '#attributes' => array('class' => array('field-weight')),
      ),
      'namespace' => array(
        '#markup' => $entity_namespace,
      ),
      'entity_type' => array(
        '#markup' => $settings['entity_type'],
      ),
      'bundle' => array(
        '#markup' => $settings['bundle'],
      ),
      'getter' => array(
        '#markup' => $settings['getter'],
      ),
      'configure' => array(
        '#type' => 'link',
        '#title' => t('configure'),
        '#href' => $entity_path,
        '#options' => array('attributes' => array('title' => t('Configure entity getter settings'))),
      ),
      'remove' => array(
        '#type' => 'link',
        '#title' => t('remove'),
        '#href' => $entity_path . '/remove',
        '#options' => array('attributes' => array('title' => t('Remove this entity.'))),
      ),
    );
  }

  // Additional row: add new field.
  $max_weight = flexiform_entity_max_weight($flexiform);

  // Entity type options
  $entity_type_options = array();
  foreach(entity_get_info() as $entity_type => $info) {
    $entity_type_options[$entity_type] = $info['label'];
  }

  // Get entity type if its been set
  if (isset($form_state['values']['entities']['_add_entity']['entity_type'])) {
    $entity_type = $form_state['values']['entities']['_add_entity']['entity_type'];
  }
  else {
    $entity_type = NULL;
  }

  // Get bundle options
  $bundle_options = array();
  if (!empty($entity_type)) {
    $entity_info = entity_get_info($entity_type);
    foreach ($entity_info['bundles'] as $bundle => $info) {
      $bundle_options[$bundle] = $info['label'];
    }
  }

  // Get getter options
  $getter_options = array();
  if (!empty($entity_type)) {
    foreach (flexiform_entity_type_get_getters($entity_type) as $getter => $info) {
      if (!empty($info['params'])) {
        // Put the available entities into
        $entities_by_type = array();
        foreach ($flexiform->entities as $entity_namespace => $settings) {
          $entities_by_type[$settings['entity_type']][] = $entity_namespace;
        }

        $combinations = array();
        flexiform_get_getter_parameter_combinations($info['params'], $entities_by_type, $combinations);

        foreach ($combinations as $combination) {
          $key_bits = array($getter);
          $label_bits = array();
          $label = t('@label from ', array('@label' => $info['label']));
          foreach ($combination as $param => $namespace) {
            $key_bits[] = $param . '=' . $namespace;
            $label_bits[] = t('!label (@param)', array('!label' => $flexiform->entities[$namespace]['label'], '@param' => $param));
          }
          $key = implode(':', $key_bits);
          $label .= implode(', ', $label_bits);

          $getter_options[$key] = $label;
        }
      }
      else {
        $getter_options[$getter] = $info['label'];
      }
    }
  }

  if ($entity_type_options) {
    $name = '_add_entity';
    $table[$name] = array(
      '#attributes' => array('class' => array('draggable', 'tabledrag-leaf', 'add-new')),
      '#row_type' => 'add_entity',
      '#region_callback' => 'flexiform_entity_form_row_region',
      'label' => array(
        '#type' => 'textfield',
        '#title' => 'Label',
        '#title_display' => 'invisible',
        '#size' => 15,
        '#description' => t('Label'),
        '#prefix' => '<div class="label-input"><div class="add-new-placeholder">' . t('Add entity') .'</div>',
        '#suffix' => '</div>',
      ),
      'weight' => array(
        '#type' => 'textfield',
        '#default_value' => $max_weight + 1,
        '#size' => 3,
        '#title_display' => 'invisible',
        '#title' => t('Weight for new entity'),
        '#attributes' => array('class' => array('field-weight')),
        '#prefix' => '<div class="add-new-placeholder">&nbsp;</div>',
      ),
      'namespace' => array(
        '#type' => 'textfield',
        '#title' => 'Namespace',
        '#title_display' => 'invisible',
        '#size' => 15,
        '#description' => t('namespace'),
      ),
      'entity_type' => array(
        '#type' => 'select',
        '#title' => t('Enitity Type'),
        '#title_display' => 'invisible',
        '#options' => $entity_type_options,
        '#empty_option' => t('- Select an Entity Type -'),
        '#description' => t('Type of entity'),
        '#attributes' => array('class' => array('bundle-select')),
        '#prefix' => '<div class="add-new-placeholder">&nbsp;</div>',
        '#ajax' => array(
          'event' => 'change',
          'method' => 'replace',
          'wrapper' => 'flexiform-bundle-selector',
          'callback' => 'flexiform_manage_form_entities_bundles',
        ),
      ),
      'bundle' => array(
        '#type' => 'select',
        '#title' => t('Bundle'),
        '#title_display' => 'invisible',
        '#options' => $bundle_options,
        '#empty_option' => t('- Select a Bundle -'),
        '#description' => t('Bundle'),
        '#attributes' => array('class' => array('bundle-select')),
        '#prefix' => '<div id="flexiform-bundle-selector"><div class="add-new-placeholder">&nbsp;</div>',
        '#suffix' => '</div>',
        '#ajax' => array(
          'event' => 'change',
          'method' => 'replace',
          'wrapper' => 'flexiform-getter-selector',
          'callback' => 'flexiform_manage_form_entities_getters',
        ),
      ),
      'getter' => array(
        '#type' => 'select',
        '#title' => t('Getter'),
        '#title_display' => 'invisible',
        '#options' => $getter_options,
        '#empty_option' => t('- Select a Getter -'),
        '#description' => t('Pick a getter'),
        '#attributes' => array('class' => array('getter-select')),
        '#prefix' => '<div id="flexiform-getter-selector"><div class="add-new-placeholder">&nbsp;</div>',
        '#suffix' => '</div>',
      ),
    );
  }

  $form['entities'] = $table;

  $form['actions'] = array('#type' => 'actions');
  $form['actions']['submit'] = array('#type' => 'submit', '#value' => t('Save'));

  $form['#attached']['css'][] = drupal_get_path('module', 'flexiform') . '/flexiform_ui.css';
  $form['#attached']['js'][] = drupal_get_path('module', 'flexiform') . '/flexiform_ui.js';

  // Add tabledrag behavior.
  $form['#attached']['drupal_add_tabledrag'][] = array('field-overview', 'order', 'sibling', 'field-weight');
  $form['#attached']['drupal_add_tabledrag'][] = array('field-overview', 'match', 'parent', 'field-parent', 'field-parent', 'field-name');

  return $form;
}

/**
 * Get all combinations possible for the parameters of an entity getter.
 */
function flexiform_get_getter_parameter_combinations($parameters, $collated_items, &$combinations, $combination_so_far = array()) {
  $key = key($parameters);
  $parameter = current($parameters);
  array_shift($parameters);

  foreach ($collated_items[$parameter['entity_type']] as $item) {
    $combination_so_far[$key] = $item;

    // If this is the last parameter then save the $combination to the list.
    if (empty($parameters)) {
      // Check for no duplicates.
      if (count(array_unique($combination_so_far)) == count($combination_so_far)) {
        $combinations[] = $combination_so_far;
      }
    }
    else {
      flexiform_get_getter_parameter_combinations($parameters, $collated_items, $combinations, $combination_so_far);
    }
  }
}

/**
 * Ajax callback to update the bundles available
 */
function flexiform_manage_form_entities_bundles($form, $form_state) {
  return $form['entities']['_add_entity']['bundle'];
}

/**
 * Ajax callback to update the getters available
 */
function flexiform_manage_form_entities_getters($form, $form_state) {
  return $form['entities']['_add_entity']['getter'];
}

/**
 * Validate the manage entities form
 */
function flexiform_manage_form_entities_form_validate($form, &$form_state) {
  $entity = $form_state['values']['entities']['_add_entity'];

  if (array_filter(array($entity['label'], $entity['namespace'], $entity['entity_type'], $entity['bundle'], $entity['getter']))) {
    // Missing Label
    if (!$entity['label']) {
      form_set_error('entities][_add_entity][label', t('Add entity: you must provide a label'));
    }

    if (!$entity['namespace']) {
      // @todo: Check this is unique and machine nameable
      form_set_error('entities][_add_entity][namespace', t('Add entity: you must choose a unique namespace'));
    }

    if (!$entity['entity_type']) {
      form_set_error('entities][_add_entity][entity_type', t('Add entity: you must choose an entity type'));
    }

    if (!$entity['bundle']) {
      form_set_error('entities][_add_entity][bundle', t('Add entity: you must choose a bundle'));
    }

    if (!$entity['getter']) {
      form_set_error('entities][_add_entity][getter', t('Add entity: you must choose an entity getter callback'));
    }
  }
}

/**
 * Validate the manage entities form
 */
function flexiform_manage_form_entities_form_submit($form, &$form_state) {
  $form_values = $form_state['values']['entities'];
  $flexiform = $form_state['#flexiform'];
  $entities = $form_state['#entities'];

  $entity_path = '';

  foreach($form_values as $entity_namespace => $values) {
    if ($entity_namespace == '_add_entity') {
      continue;
    }
    $flexiform->entities[$entity_namespace]['weight'] = $values['weight'];
  }

  // Add a field
  $entity = array();
  if (!empty($form_values['_add_entity']['namespace'])) {
    $values = $form_values['_add_entity'];

    $entity = array(
      'namespace' => $values['namespace'],
      'label' => $values['label'],
      'entity_type' => $values['entity_type'],
      'bundle' => $values['bundle'],
      'getter' => $values['getter'],
      'create' => FALSE,
      'weight' => $values['weight'],
    );

    $input = explode(':', $values['getter']);
    if (count($input) > 1) {
      $entity['getter'] = array_shift($input);
      foreach ($input as $param_info) {
        $bits = explode('=', $param_info);
        $entity['parameters'][$bits[0]] = $bits[1];
      }
    }

    $flexiform->entities[$entity['namespace']] = $entity;

    // Send the User to the Instance Settings page
  }

  $flexiform->save();

  drupal_set_message(t('Your settings have been saved'));
}

/**
 * Get the max weight of fields in the flexiform
 */
function flexiform_field_max_weight($flexiform) {
  $max_weight = 0;
  foreach($flexiform->elements as $element_namespace => $settings) {
    $weight = !empty($settings['weight']) ? $settings['weight'] : 0;
    if ($weight > $max_weight) {
      $max_weight = $weight;
    }
  }

  return $max_weight;
}

/**
 * Get the max weight of entities in the flexiform
 */
function flexiform_entity_max_weight($flexiform) {
  $max_weight = 0;
  foreach($flexiform->entities as $entity_namespace => $settings) {
    $weight = $settings['weight'];
    if ($weight > $max_weight) {
      $max_weight = $weight;
    }
  }

  return $max_weight;
}

/**
 * Get all available elements for a given entity
 */
function flexiform_field_element_options($flexiform, $entity_namespace = NULL) {
  if (!isset($entity_namespace) || !isset($flexiform->entities[$entity_namespace])) {
    return array();
  }

  $entity_info = $flexiform->entities[$entity_namespace];
  $elements = flexiform_get_element_info($entity_info['entity_type'], $entity_info['bundle']);

  $options = array();
  foreach ($elements as $name => $element) {
    $options[$element['group']][$name] = $element['label'];
  }

  return $options;
}

/**
 * Pre-render callback for flexiform_field_table elements.
 */
function flexiform_field_table_pre_render($elements) {
  $js_settings = array();

  // For each region, build the tree structure from the weight and parenting
  // data contained in the flat form structure, to determine row order and
  // indentation.
  $regions = $elements['#regions'];
  $tree = array('' => array('name' => '', 'children' => array()));
  $trees = array_fill_keys(array_keys($regions), $tree);

  $parents = array();
  $list = drupal_map_assoc(element_children($elements));

  // Iterate on rows until we can build a known tree path for all of them.
  while ($list) {
    foreach ($list as $name) {
      $row = &$elements[$name];
      $parent = $row['parent_wrapper']['parent']['#value'];
      // Proceed if parent is known.
      if (empty($parent) || isset($parents[$parent])) {
        // Grab parent, and remove the row from the next iteration.
        $parents[$name] = $parent ? array_merge($parents[$parent], array($parent)) : array();
        unset($list[$name]);

        // Determine the region for the row.
        $function = $row['#region_callback'];
        $region_name = $function($row);

        // Add the element in the tree.
        $target = &$trees[$region_name][''];
        foreach ($parents[$name] as $key) {
          $target = &$target['children'][$key];
        }
        $target['children'][$name] = array('name' => $name, 'weight' => $row['weight']['#value']);

        // Add tabledrag indentation to the first row cell.
        if ($depth = count($parents[$name])) {
          $cell = current(element_children($row));
          $row[$cell]['#prefix'] = theme('indentation', array('size' => $depth)) . (isset($row[$cell]['#prefix']) ? $row[$cell]['#prefix'] : '');
        }

        // Add row id and associate JS settings.
        $id = drupal_html_class($name);
        $row['#attributes']['id'] = $id;
        if (isset($row['#js_settings'])) {
          $row['#js_settings'] += array(
            'rowHandler' => $row['#row_type'],
            'name' => $name,
            'region' => $region_name,
          );
          $js_settings[$id] = $row['#js_settings'];
        }
      }
    }
  }
  // Determine rendering order from the tree structure.
  foreach ($regions as $region_name => $region) {
    $elements['#regions'][$region_name]['rows_order'] = array_reduce($trees[$region_name], '_flexiform_field_form_reduce_order');
  }

  $elements['#attached']['js'][] = array(
    'type' => 'setting',
    'data' => array('fieldUIRowsData' => $js_settings),
  );
  return $elements;
}

/**
 * Returns HTML for Flexiform Field tables.
 *
 * @param $variables
 *   An associative array containing:
 *   - elements: An associative array containing a Form API structure to be
 *     rendered as a table.
 *
 * @ingroup themeable
 */
function theme_flexiform_field_table($variables) {
  $elements = $variables['elements'];
  $table = array();
  $js_settings = array();

  // Add table headers and attributes.
  foreach (array('header', 'attributes') as $key) {
    if (isset($elements["#$key"])) {
      $table[$key] = $elements["#$key"];
    }
  }

  // Determine the colspan to use for region rows, by checking the number of
  // columns in the headers.
  $colums_count = 0;
  foreach ($table['header'] as $header) {
    $colums_count += (is_array($header) && isset($header['colspan']) ? $header['colspan'] : 1);
  }

  // Render rows, region by region.
  foreach ($elements['#regions'] as $region_name => $region) {
    $region_name_class = drupal_html_class($region_name);
    // Add region rows.
    if (isset($region['title'])) {
      $table['rows'][] = array(
        'class' => array('region-title', 'region-' . $region_name_class . '-title'),
        'no_striping' => TRUE,
        'data' => array(
          array('data' => $region['title'], 'colspan' => $colums_count),
        ),
      );
    }
    if (isset($region['message'])) {
      $class = (empty($region['rows_order']) ? 'region-empty' : 'region-populated');
      $table['rows'][] = array(
        'class' => array('region-message', 'region-' . $region_name_class . '-message', $class),
        'no_striping' => TRUE,
        'data' => array(
          array('data' => $region['message'], 'colspan' => $colums_count),
        ),
      );
    }

    // Add form rows, in the order determined at pre-render time.
    foreach ($region['rows_order'] as $name) {
      $element = $elements[$name];

      $row = array('data' => array());
      if (isset($element['#attributes'])) {
        $row += $element['#attributes'];
      }

      // Render children as table cells.
      foreach (element_children($element) as $cell_key) {
        $child = &$element[$cell_key];
        // Do not render a cell for children of #type 'value'.
        if (!(isset($child['#type']) && $child['#type'] == 'value')) {
          $cell = array('data' => drupal_render($child));
          if (isset($child['#cell_attributes'])) {
            $cell += $child['#cell_attributes'];
          }
          $row['data'][] = $cell;
        }
      }
      $table['rows'][] = $row;
    }
  }
  return theme('table', $table);
}

/**
 * Returns the region to which a row in the 'Manage fields' screen belongs.
 *
 * This function is used as a #row_callback in field_ui_field_overview_form(),
 * and is called during field_ui_table_pre_render().
 */
function flexiform_field_form_row_region($row) {
  switch ($row['#row_type']) {
    case 'field':
    case 'extra_field':
      return 'main';
    case 'add_field':
      // If no input in 'label', assume the row has not been dragged out of the
      // 'add new' section.
      return (!empty($row['label']['#value']) ? 'main' : 'add_new');
  }
}

/**
 * Helper function: determines the rendering order of a tree array.
 *
 * This is intended as a callback for array_reduce().
 */
function _flexiform_field_form_reduce_order($array, $a) {
  $array = !isset($array) ? array() : $array;
  if ($a['name']) {
    $array[] = $a['name'];
  }
  if (!empty($a['children'])) {
    uasort($a['children'], 'drupal_sort_weight');
    $array = array_merge($array, array_reduce($a['children'], '_flexiform_field_form_reduce_order'));
  }
  return $array;
}

/**
 * Pre-render callback for flexiform_field_table elements.
 */
function flexiform_entity_table_pre_render($elements) {
  $js_settings = array();

  // For each region, build the tree structure from the weight and parenting
  // data contained in the flat form structure, to determine row order and
  // indentation.
  $regions = $elements['#regions'];
  $tree = array('' => array('name' => '', 'children' => array()));
  $trees = array_fill_keys(array_keys($regions), $tree);

  $parents = array();
  $list = drupal_map_assoc(element_children($elements));

  // Iterate on rows until we can build a known tree path for all of them.
  while ($list) {
    foreach ($list as $name) {
      $row = &$elements[$name];
      $parent = NULL;
      // Proceed if parent is known.
      if (empty($parent) || isset($parents[$parent])) {
        // Grab parent, and remove the row from the next iteration.
        $parents[$name] = $parent ? array_merge($parents[$parent], array($parent)) : array();
        unset($list[$name]);

        // Determine the region for the row.
        $function = $row['#region_callback'];
        $region_name = $function($row);

        // Add the element in the tree.
        $target = &$trees[$region_name][''];
        foreach ($parents[$name] as $key) {
          $target = &$target['children'][$key];
        }
        $target['children'][$name] = array('name' => $name, 'weight' => $row['weight']['#value']);

        // Add tabledrag indentation to the first row cell.
        if ($depth = count($parents[$name])) {
          $cell = current(element_children($row));
          $row[$cell]['#prefix'] = theme('indentation', array('size' => $depth)) . (isset($row[$cell]['#prefix']) ? $row[$cell]['#prefix'] : '');
        }

        // Add row id and associate JS settings.
        $id = drupal_html_class($name);
        $row['#attributes']['id'] = $id;
        if (isset($row['#js_settings'])) {
          $row['#js_settings'] += array(
            'rowHandler' => $row['#row_type'],
            'name' => $name,
            'region' => $region_name,
          );
          $js_settings[$id] = $row['#js_settings'];
        }
      }
    }
  }
  // Determine rendering order from the tree structure.
  foreach ($regions as $region_name => $region) {
    $elements['#regions'][$region_name]['rows_order'] = array_reduce($trees[$region_name], '_flexiform_field_form_reduce_order');
  }

  $elements['#attached']['js'][] = array(
    'type' => 'setting',
    'data' => array('fieldUIRowsData' => $js_settings),
  );
  return $elements;
}

/**
 * Returns HTML for Flexiform Field tables.
 *
 * @param $variables
 *   An associative array containing:
 *   - elements: An associative array containing a Form API structure to be
 *     rendered as a table.
 *
 * @ingroup themeable
 */
function theme_flexiform_entity_table($variables) {
  $elements = $variables['elements'];
  $table = array();
  $js_settings = array();

  // Add table headers and attributes.
  foreach (array('header', 'attributes') as $key) {
    if (isset($elements["#$key"])) {
      $table[$key] = $elements["#$key"];
    }
  }

  // Determine the colspan to use for region rows, by checking the number of
  // columns in the headers.
  $colums_count = 0;
  foreach ($table['header'] as $header) {
    $colums_count += (is_array($header) && isset($header['colspan']) ? $header['colspan'] : 1);
  }

  // Render rows, region by region.
  foreach ($elements['#regions'] as $region_name => $region) {
    $region_name_class = drupal_html_class($region_name);
    // Add region rows.
    if (isset($region['title'])) {
      $table['rows'][] = array(
        'class' => array('region-title', 'region-' . $region_name_class . '-title'),
        'no_striping' => TRUE,
        'data' => array(
          array('data' => $region['title'], 'colspan' => $colums_count),
        ),
      );
    }
    if (isset($region['message'])) {
      $class = (empty($region['rows_order']) ? 'region-empty' : 'region-populated');
      $table['rows'][] = array(
        'class' => array('region-message', 'region-' . $region_name_class . '-message', $class),
        'no_striping' => TRUE,
        'data' => array(
          array('data' => $region['message'], 'colspan' => $colums_count),
        ),
      );
    }

    // Add form rows, in the order determined at pre-render time.
    foreach ($region['rows_order'] as $name) {
      $element = $elements[$name];

      $row = array('data' => array());
      if (isset($element['#attributes'])) {
        $row += $element['#attributes'];
      }

      // Render children as table cells.
      foreach (element_children($element) as $cell_key) {
        $child = &$element[$cell_key];
        // Do not render a cell for children of #type 'value'.
        if (!(isset($child['#type']) && $child['#type'] == 'value')) {
          $cell = array('data' => drupal_render($child));
          if (isset($child['#cell_attributes'])) {
            $cell += $child['#cell_attributes'];
          }
          $row['data'][] = $cell;
        }
      }
      $table['rows'][] = $row;
    }
  }
  return theme('table', $table);
}

/**
 * Returns the region to which a row in the 'Manage fields' screen belongs.
 *
 * This function is used as a #row_callback in field_ui_field_overview_form(),
 * and is called during field_ui_table_pre_render().
 */
function flexiform_entity_form_row_region($row) {
  switch ($row['#row_type']) {
    case 'entity':
      return 'main';
    case 'add_entity':
      // If no input in 'label', assume the row has not been dragged out of the
      // 'add new' section.
      return (!empty($row['label']['#value']) ? 'main' : 'add_new');
  }
}

/**
 * Helper function: determines the rendering order of a tree array.
 *
 * This is intended as a callback for array_reduce().
 */
function _flexiform_entity_form_reduce_order($array, $a) {
  $array = !isset($array) ? array() : $array;
  if ($a['name']) {
    $array[] = $a['name'];
  }
  if (!empty($a['children'])) {
    uasort($a['children'], 'drupal_sort_weight');
    $array = array_merge($array, array_reduce($a['children'], '_flexiform_field_form_reduce_order'));
  }
  return $array;
}

/**
 * Auto-complete callback for flexiform tags.
 */
function flexiform_autocomplete_tags($tags_typed = '') {
  // The user enters a comma-separated list of tags. We only autocomplete the
  // last tag.
  $tags_typed = drupal_explode_tags($tags_typed);
  $tag_last = drupal_strtolower(array_pop($tags_typed));

  $tag_matches = array();
  if ($tag_last != '') {
    $query = db_select('flexiform_tags', 'ft');
    // Do not select already entered terms.
    if (!empty($tags_typed)) {
      $query->condition('ft.tag', $tags_typed, 'NOT IN');
    }
    // Select rows that match by tag name.
    $tags_return = $query
      ->distinct()
      ->fields('ft', array('tag'))
      ->condition('ft.tag', '%' . db_like($tag_last) . '%', 'LIKE')
      ->groupBy('ft.tag')
      ->range(0, 10)
      ->execute()
      ->fetchCol('ft.tag');

    $prefix = count($tags_typed) ? drupal_implode_tags($tags_typed) . ', ' : '';

    foreach ($tags_return as $name) {
      $n = $name;
      // Tag names containing commas or quotes must be wrapped in quotes.
      if (strpos($name, ',') !== FALSE || strpos($name, '"') !== FALSE) {
        $n = '"' . str_replace('"', '""', $name) . '"';
      }
      $tag_matches[$prefix . $n] = check_plain($name);
    }
  }
  drupal_json_output($tag_matches);
}

/**
 * Render an item on the flexiform overview page.
 */
function theme_flexiform_ui_overview_item($variables) {
  $output = $variables['url'] ? l($variables['label'], $variables['url']['path'], $variables['url']['options']) : check_plain($variables['label']);
  if ($variables['name']) {
    $output .= ' <small> (' . t('Machine name') . ': ' . check_plain($variables['name']) . ')</small>';
  }
  if ($variables['tags']) {
    $output .= '<br /><small>' . t('Tags') . ': ' . check_plain($variables['tags']) . '</small>';
  }
  return $output;
}

/**
 * Trigger a rebuild of the form.
 */
function flexiform_form_submit_rebuild($form, &$form_state) {
  $form_state['rebuild'] = TRUE;
}

/**
 * Form to Configure a Flexiform Entity.
 */
function flexiform_entity_configure_form($form, &$form_state, $flexiform, $entity_namespace) {
  $form['#flexiform'] = $flexiform;
  $handler = $flexiform->getBuilder()->getEntityManager()
    ->getEntityHandler($entity_namespace);
  $form['#form_entity'] = $handler;
  $form += $handler->configForm($form, $form_state);

  return $form;
}

/**
 * Validation Handler.
 */
function flexiform_entity_configure_form_validate($form, &$form_state) {
  $form['#form_entity']->configFormValidate($form, $form_state);
}

/**
 * Submit handler.
 */
function flexiform_entity_configure_form_submit($form, &$form_state) {
  $form['#form_entity']->configFormSubmit($form, $form_state);

  drupal_set_message(t('The configuration has been updated.'));
  $form_state['redirect'] = "admin/structure/flexiforms/manage/{$form['#flexiform']->form}/form-entities";
}

/**
 * Form callback for removing entities.
 */
function flexiform_entity_remove_form($form, &$form_state, $flexiform, $entity_namespace) {
  $form['#flexiform'] = $flexiform;
  $form['#entity_namespace'] = $entity_namespace;
  $manager = $flexiform->getBuilder()->getEntityManager();
  $dependants = $manager->checkForDependants($entity_namespace);
  $settings = $manager->getEntitySettings($entity_namespace);
  if ($entity_namespace == 'base_entity') {
    $form['message']['#markup'] = t('Cannot remove the base entity of a form.');
  }
  else if (!empty($dependants)) {
    $dependantList = array(
      '#theme' => 'item_list',
      '#items' => $dependants,
    );
    $args = array(
      '%this' => $settings['label'],
      '!dependants' => drupal_render($dependantList),
    );
    $form['message']['#markup'] = t('Cannot remove %this as the following entities depend on it. Remove these entities first. !dependants', $args);
  }
  else {
    $form['message']['#markup'] = t('Are you sure you want to remove %this from the form?', array('%this' => $settings['label']));

    $form['actions'] = array(
      '#type' => 'actions',
    );

    $form['actions']['submit'] = array(
      '#type' => 'submit',
      '#value' => t('Remove'),
    );
  }

  return $form;
}

/**
 * Form submit for removing entities.
 */
function flexiform_entity_remove_form_submit($form, &$form_state) {
  $flexiform = $form['#flexiform'];
  $entity_namespace = $form['#entity_namespace'];
  $flexiform->getBuilder()->getEntityManager()->removeEntitySettings($entity_namespace);

  $form_state['redirect'] = "admin/structure/flexiforms/manage/{$flexiform->form}/form-entities";
}

/**
 * Clone form page callback.
 */
function flexiform_ui_get_clone_form($entity_type, $entity, $op = 'clone', $form_state = array()) {
  $form_id = 'flexiform_form';

  // Do not use drupal_get_form(), but invoke drupal_build_form() ourself so
  // we can prepulate the form state.
  $form_state['wrapper_callback'] = 'entity_ui_main_form_defaults';
  $form_state['entity_type'] = 'flexiform';
  form_load_include($form_state, 'inc', 'entity', 'includes/entity.ui');

  $entity = clone $entity;
  $entity->id = FALSE;
  $entity->cloned_from = $entity->form;
  $entity->form = FALSE;
  $entity->is_new = TRUE;
  $entity->status = ENTITY_CUSTOM;

  // We don't pass the entity type as first parameter, as the implementing
  // module knows the type anyway. However, in order to allow for efficient
  // hook_forms() implementiations we append the entity type as last argument,
  // which the module implementing the form constructor may safely ignore.
  // @see entity_forms()
  $form_state['build_info']['args'] = array($entity, $op, $entity_type);
  return drupal_build_form($form_id, $form_state);
}

<?php
/**
 * @file
 * Contains FlexiformElementField class.
 */

/**
 * Class for Field API elements.
 */
class FlexiformElementField extends FlexiformElement implements FlexiformElementFieldAPIInterface {

  /**
   * The field name of the field.
   */
  protected $field_name;

  /**
   * The instance settings for the field
   */
  protected $instance;

  /**
   * The field settings for the field.
   */
  protected $field;

  /**
   * Overrides FlexiformElement::__construct().
   */
  public function __construct($flexiform, $settings, $element_namespace = '') {
    parent::__construct($flexiform, $settings, $element_namespace);
    $this->field_name = $this->element_info['field_name'];

    // Build the fake instance for this form (incase the widget has changed -
    // we would normally have to be careful of default_value but that doesn't
    // figure here, so we should be fine.
    $this->instance = !empty($settings['instance']) ? $settings['instance'] : array();
    $this->field = !empty($settings['field']) ? $settings['field'] : array();

    // Get the initial weight.
    $instance = $this->getInstance();
    $this->weight = isset($settings['weight']) ? $settings['weight'] : $instance['widget']['weight'];
  }

  /**
   * Overrides FlexiformElement::label().
   *
   * @return string
   *   The field label.
   */
  public function label() {
    $instance = $this->getInstance();
    return check_plain($instance['label']);
  }

  /**
   * Overrides FlexiformElement::setLabel().
   */
  public function setLabel($label) {
    $this->instance['label'] = $label;
  }

  /**
   * Return the type.
   */
  public function type() {
    return 'Field';
  }

  /**
   * Get the widget type from the instance settings.
   *
   * @return string
   *   Machine name of the field widget in use.
   */
  public function getWidgetType() {
    $instance = $this->getInstance();
    return $instance['widget']['type'];
  }

  /**
   * Get the widget label from the instance settings.
   *
   * @return string
   *   Human readable name of the field widget.
   */
  public function getWidgetLabel() {
    $widget_types = field_info_widget_types();
    return $widget_types[$this->getWidgetType()]['label'];
  }

  /**
   * Get the instance array.
   *
   * At this point we merge the instance settings for the form into the normal
   * field instance settings.
   */
  public function getInstance() {
    if (empty($this->full_instance)) {
      $field_instance = field_info_instance($this->entity_type, $this->field_name, $this->bundle);
      $this->full_instance = array_replace_recursive($field_instance, $this->instance);

      // Default value might be a numerically indexed array, so we'll overwrite
      if (isset($this->instance['default_value'])) {
        $this->full_instance['default_value'] = $this->instance['default_value'];
      }

      // Here we check if a default value function is used.
      if (isset($this->instance['use_default_value_function'])) {
        if (empty($this->instance['use_default_value_function'])) {
          unset($this->full_instance['default_value_function']);
        }
        else {
          unset($this->full_instance['default_value']);
        }
      }
    }

    return $this->full_instance;
  }

  /**
   * {@inheritdoc}
   */
  public function setWeight($weight) {
    parent::setWeight($weight);

    $this->instance['widget']['weight'] = $weight;
  }

  /**
   * Get the Difference of two settings arrays.
   *
   * @see http://uk1.php.net/manual/en/function.array-diff-assoc.php#111675
   *
   * @todo: Build in special casing for default values.
   */
  public function diffArrays($array, $original_array) {
    $difference = array();
    foreach ($array as $key => $value) {
      if (is_array($value)) {
        if (!isset($original_array[$key]) || !is_array($original_array[$key])) {
          $difference[$key] = $value;
        }
        else {
          $new_diff = $this->diffArrays($value, $original_array[$key]);
          if (!empty($new_diff)) {
            $difference[$key] = $new_diff;
          }
        }
      }
      elseif (!array_key_exists($key,$original_array) || $original_array[$key] !== $value) {
        $difference[$key] = $value;
      }
    }
    return $difference;
  }

  /**
   * Get the field array.
   *
   * At this point we merge the field settings for the form into the normal
   * field settings.
   */
  public function getField() {
    if (empty($this->full_field)) {
      $field = field_info_field($this->field_name);
      $this->full_field = array_replace_recursive($field, $this->field);
    }

    return $this->full_field;
  }

  /**
   * Overrides FlexiformElement::configureForm().
   */
  public function configureForm($form, &$form_state, $flexiform) {
    form_load_include($form_state, 'inc', 'field_ui', 'field_ui.admin');
    $instance = $this->getInstance();
    $form = field_ui_field_edit_form($form, $form_state, $instance);

    // Invoke hook_form_alter() and hook_form_field_ui_field_edit_form_alter()
    /*$hooks = array('form');
    $hooks[] = 'form_field_ui_field_edit_form';
    $fake_form_id = 'field_ui_field_edit_form';
    drupal_alter($hooks, $form, $form_state, $fake_form_id);*/

    // Allow changes to the default values regardless of anything else thats going on.
    $form['instance']['use_default_value_function'] = array(
      '#type' => 'checkbox',
      '#title' => t('Use a Default Value Function'),
      '#default_value' => !(field_behaviors_widget('default value', $instance) == FIELD_BEHAVIOR_DEFAULT && empty($instance['default_value_function'])),
    );
    $form['instance']['default_value_function'] = array(
      '#type' => 'textfield',
      '#title' => t('Default Value Function'),
      '#description' => t('If you wish to use a custom default value function, enter the name of it here. Reccommended for advanced users only.'),
      '#default_value' => isset($instance['default_value_function']) ? $instance['default_value_function'] : NULL,
      '#states' => array('invisible' => array(':input[name="instance[use_default_value_function]"]' => array('checked' => FALSE))),
    );
    $form['instance']['default_value_widget'] = field_ui_default_value_widget($this->getField(), $instance, $form, $form_state);
    $form['instance']['default_value_widget']['#states'] = array(
      'visible' => array(':input[name="instance[use_default_value_function]"]' => array('checked' => FALSE)),
    );
    $form['instance']['use_default_value_tokens'] = array(
      '#type' => 'checkbox',
      '#title' => t('Use Tokens in Default Value'),
      '#default_value' => !empty($instance['use_default_value_tokens']),
      '#states' => array(
        'visible' => array(':input[name="instance[use_default_value_function]"]' => array('checked' => FALSE)),
      ),
    );
    $form['instance']['contexts'] = array(
      '#title' => t('Substitutions'),
      '#type' => 'fieldset',
      '#collapsible' => TRUE,
      '#collapsed' => TRUE,
      '#status' => array(
        'visible' => array(':input[name="instance[use_default_value_tokens]"]' => array('checked' => TRUE)),
      ),
    );
    $form['instance']['contexts']['contexts'] = $this->getCtoolsSubstitutionsList();

    // Rename the boxes and discourage use of field settings.
    $form['instance']['#title'] = t('Form Settings');
    $form['instance']['#description'] = t('These settings will override default Field API settings for this field instance. Overrides will only apply to this flexiform.');
    $form['field']['#title'] = t('Field Settings');
    $form['field']['#description'] = t('These settings will override default Field API settings for this field. Overrides will only apply to this flexiform. <strong>Handle with care: Altering these settings can cause unexpected results.</strong>');
    $form['field']['#collapsible'] = TRUE;
    $form['field']['#collapsed'] = TRUE;

    // Delete the 'Save Settings' button added by FieldUI.
    unset($form['actions']);

    $form = parent::configureForm($form, $form_state, $flexiform);

    // Unset the label field added by  FlexiformElementBase.
    unset($form['label']);

    return $form;
  }

  /**
   * Overrides FlexiformElement::configureFormValidate().
   */
  public function configureFormValidate($form, &$form_state, $flexiform) {
    if ($form_state['values']['instance']['use_default_value_function']) {
      unset($form['instance']['default_value_widget']);
    }

    field_ui_field_edit_form_validate($form, $form_state);
  }

  /**
   * Overrides FlexiformElement::configureFormSubmit().
   */
  public function configureFormSubmit($form, &$form_state, $flexiform) {
    $instance = $form_state['values']['instance'];
    $field = $form_state['values']['field'];

    // Update any field settings that have changed.
    $element_field = $this->getField();
    $field = array_merge($element_field, $field);
    $original_field = field_info_field($instance['field_name']);
    $this->field = $this->diffArrays($field, $original_field);

    // Work out element instance settings. This gets a little complicated as
    // we want to make sure the default value settings work.
    $element_instance = $this->getInstance();
    $instance = array_merge($element_instance, $instance);
    $original_instance = field_read_instance($instance['entity_type'], $instance['field_name'], $instance['bundle']);

    if (!$instance['use_default_value_function']) {
      $element = $form['instance']['default_value_widget'];

      // Extract field values.
      $items = array();
      field_default_extract_form_values(NULL, NULL, $element_field, $element_instance, LANGUAGE_NONE, $items, $element, $form_state);
      field_default_submit(NULL, NULL, $element_field, $element_instance, LANGUAGE_NONE, $items, $element, $form_state);

      $instance['default_value'] = $items ? $items : NULL;
    }
    else {
      unset($instance['default_value']);
    }

    // Cast Required to a boolean so it doesn't get erroneously stored.
    $instance['required'] = (bool) $instance['required'];
    $this->instance = $this->diffArrays($instance, $original_instance);

    parent::configureFormSubmit($form, $form_state, $flexiform);
  }

  /**
   * Return the form element for this FlexiformElement.
   */
  public function form($form, &$form_state, $entity, $language = LANGUAGE_NONE) {
    $items = field_get_items($this->entity_type, $entity, $this->field_name, $language);

    if (!field_info_field($this->field_name)
      || !field_info_instance($this->entity_type, $this->field_name, $this->bundle)) {
      debug("Error in " . __CLASS__ . "::" . __METHOD__ .": Field <em>{$this->field_name}</em> does not exist or does not have an instance on {$this->entity_type}:{$this->bundle}.", "error");
      return $form;
    }

    $instance = $this->getInstance();

    // Deal with default value tokens.
    if (!empty($instance['use_default_value_tokens'])) {
      $defaults = &$instance['default_value'];
      foreach ($defaults as $delta => &$item) {
        foreach ($item as &$value) {
          if (is_string($value)) {
            $value = $this->replaceCtoolsSubstitutions($value, $form['#flexiform_entities']);
          }
        }
      }
    }

    $form_fields = field_default_form($this->entity_type, $entity, $this->getField(), $instance, $language, $items, $form, $form_state);
    foreach ($form_fields as $form_key => $form_field) {
      // Make sure the parents stuff is always correct.
      $form_field['#parents'] = $form['#parents'];
      array_push($form_field['#parents'], $this->field_name);

      if (!empty($instance['remove_none'])) {
        unset($form_field[$language]['#options']['']);
        unset($form_field[$language]['#options']['_none']);
      }

      if (isset($instance['attributes']) && is_array($instance['attributes'])) {
        if (isset($form_field['#attributes'])) {
          $form_field['#attributes'] = array_replace_recursive($form_field['#attributes'], $instance['attributes']);
        }
        else {
          $form_field['#attributes'] = $instance['attributes'];
        }
      }

      // Honour the hide title settings.
      if (!empty($this->settings['display_options']['hide_label'])) {
        $form_field[LANGUAGE_NONE]['#title_display'] = 'invisible';
      }

      $form[$this->element_namespace] = $form_field;
    }

    $form = parent::form($form, $form_state, $entity);
    return $form;
  }

  /**
   * Validate this element.
   */
  public function formValidate($form, &$form_state, $entity, $language = LANGUAGE_NONE) {
    // Prepare arrays.
    $items = isset($entity->{$this->field_name}[$language]) ? $entity->{$this->field_name}[$language] : array();
    $errors = array();
    $field = $this->getField();

    // Extract form values.
    $form_elements = $this->extractFormElements($form);
    field_default_extract_form_values($this->getEntityType(), $entity, $field, $this->getInstance(), $language, $items, $form_elements, $form_state);

    // Set items onto the entity.
    if ($items !== array() || isset($entity->{$this->field_name}[$language])) {
      $entity->{$this->field_name}[$language] = $items;
    }

    // Validate values
    field_default_validate($this->getEntityType(), $entity, $field, $this->getInstance(), $language, $items, $errors);
    $specific_func = $field['module'] . '_field_validate';
    if (function_exists($specific_func)) {
      $specific_func($this->getEntityType(), $entity, $field, $this->getInstance(), $language, $items, $errors);
    }

    // Let other modules validate the entity.
    // Avoid module_invoke_all() to let $errors be taken by reference.
    // @todo Work out a way of not calling this loads and loads of times.
    foreach (module_implements('field_attach_validate') as $module) {
      $function = $module . '_field_attach_validate';
      $function($this->getEntityType(), $entity, $errors);
    }

    // Pass field-level validation errors back to widgets for accurate error
    // flagging.
    foreach ($errors as $field_name => $field_errors) {
      foreach ($field_errors as $langcode => $errors) {
        $field_state = field_form_get_state($form_elements['#parents'], $field_name, $language, $form_state);
        $field_state['errors'] = $errors;
        field_form_set_state($form_elements['#parents'], $field_name, $language, $form_state, $field_state);
      }
    }

    field_default_form_errors($this->getEntityType(), $entity, $field, $this->getInstance(), $language, $items, $form_elements, $form_state);
  }

  /**
   * Submit callback for this form.
   */
  function formSubmit($form, &$form_state, $entity, $language = LANGUAGE_NONE) {
    // Prepare arrays.
    $items = isset($entity->{$this->field_name}[$language]) ? $entity->{$this->field_name}[$language] : array();
    $field = $this->getField();

    // Extract form values.
    $form_elements = $this->extractFormElements($form);
    field_default_extract_form_values($this->getEntityType(), $entity, $field, $this->getInstance(), $language, $items, $form_elements, $form_state);
    field_default_submit($this->getEntityType(), $entity, $field, $this->getInstance(), $language, $items, $form_elements, $form_state);

    // Set items onto the entity.
    if ($items !== array() || isset($entity->{$this->field_name}[$language])) {
      $entity->{$this->field_name}[$language] = $items;
    }
  }

  /**
   * Overrides FlexiformElement::formIsEmpty().
   */
  function formIsEmpty($form, &$form_state, $entity, $language = LANGUAGE_NONE) {
    $items = $this->formExtractValues($form, $form_state, $entity, $language);
    $items = _field_filter_items($this->getField(), $items);

    if (empty($items)) {
      return TRUE;
    }
  }

  /**
   * Overrides FlexiformElement::formExtractValues().
   */
  function formExtractValues($form, &$form_state, $entity, $language = LANGUAGE_NONE) {
    // Extract form values.
    $form_elements = $this->extractFormElements($form);
    $items = array();
    field_default_extract_form_values($this->getEntityType(), $entity, $this->getField(), $this->getInstance(), $language, $items, $form_elements, $form_state);

    return $items;
  }

  /**
   * Extract the form element from $form and give it the correct key.
   */
  function extractFormElements($form) {
    $form_element = $form[$this->getElementNamespace()];
    return array(
      '#parents' => array_merge($form['#parents'], array($this->getEntityNamespace())),
      $this->field_name => $form_element,
    );
  }

  /**
   * Overrides FlexifromElement::toSettingsArray();
   */
  public function toSettingsArray() {
    $settings = parent::toSettingsArray();
    $settings['type'] = 'field';
    $settings['field_name'] = $this->field_name;
    $settings['instance'] = $this->instance;
    $settings['field'] = $this->field;

    return $settings;
  }
}

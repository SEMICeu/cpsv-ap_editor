<?php
/**
 * @file
 * Controller class for flexiform elements.
 */

/**
 * Interface for all types of flexiform elements.
 */
interface FlexiformElementInterface {

}

/**
 * Base class for all FlexiformElements
 */
class FlexiformElement implements FlexiformElementInterface {

  /**
   * The flexiform entity this element is one.
   */
  protected $flexiform;

  /**
   * The namespace of this element.
   */
  protected $element_namespace;

  /**
   * The entity namespace of the entity this element is acting on.
   */
  protected $entity_namespace;

  /**
   * The entity type this field is on.
   */
  protected $entity_type;

  /**
   * The bundle this is on.
   */
  protected $bundle;

  /**
   * The weight of this element.
   */
  protected $weight;

  /**
   * The settings for this element.
   */
  protected $settings;

  /**
   * Construct the class.
   *
   * @param string $namespace
   *   The unique namespace of this form element.
   * @param array $settings
   *   An array of settings for the element. It must include at least the
   *   following keys:
   *     - 'entity_namespace' - The namespace of the entity this element is
   *     acting on. This is the key of the entity in the $flexiform->entities
   *     array.
   *     - 'element_name' - The name of the element we're using.
   */
  public function __construct($flexiform, $settings, $element_namespace = '') {
    $this->flexiform = $flexiform;
    $this->entity_namespace = $settings['entity_namespace'];
    $this->entity_type = $flexiform->entities[$settings['entity_namespace']]['entity_type'];
    $this->bundle = $flexiform->entities[$settings['entity_namespace']]['bundle'];
    $this->element_name = $settings['element_name'];
    $this->element_info = flexiform_get_element_info($this->entity_type, $this->bundle, $this->element_name);
    $this->settings = $settings;
    $this->weight = !empty($settings['weight']) ? $settings['weight'] : 0;

    $this->settings += array(
      'display_options' => array(),
    );

    $this->element_namespace = !empty($element_namespace) ? $element_namespace : $this->makeElementNamespace();
  }

  /**
   * Get the label for this form element.
   */
  public function label() {
    if (!empty($this->settings['label'])) {
      return $this->settings['label'];
    }

    return $this->element_info['label'];
  }

  /**
   * Set the label for this form element.
   */
  public function setLabel($label) {
    $this->settings['label'] = $label;
  }

  /**
   * Get the name for this form element.
   */
  public function name() {
    return $this->element_name;
  }

  /**
   * Get the type of this form element.
   */
  public function type() {
    return $this->element_info['type'];
  }

  /**
   * Get the element namespace for this form element.
   *
   * @return string
   *   The namespace of the element in the form.
   */
  public function getElementNamespace() {
    return $this->element_namespace;
  }

  /**
   * Get the entity namespace for this form element.
   *
   * @return string
   *   The namespace of the entity.
   */
  public function getEntityNamespace() {
    return $this->entity_namespace;
  }

  /**
   * Get the entity type for this element.
   */
  public function getEntityType() {
    return $this->entity_type;
  }

  /**
   * Get the weight of this form element.
   *
   * @return int
   *   The weight (ordering number) of the element.
   */
  public function getWeight() {
    return $this->weight;
  }

  /**
   * Set the weight of this form element.
   *
   * @param int $weight
   */
  public function setWeight($weight) {
    $this->weight = $weight;
  }

  /**
   * Get the settings.
   *
   * @return array
   */
  public function getSettings() {
    return $this->settings;
  }

  /**
   * Build the configure form for the element.
   */
  public function configureForm($form, &$form_state, $flexiform) {
    $form['label'] = array(
      '#type' => 'textfield',
      '#title' => t('Label'),
      '#default_value' => !empty($this->settings['label']) ? $this->settings['label'] : '',
      '#weight' => -99,
    );

    $form['display_options'] = array(
      '#tree' => TRUE,
      '#type' => 'fieldset',
      '#title' => t('Display Options'),
      '#weight' => 50,
      '#collapsible' => TRUE,
      '#collapsed' => TRUE,
    );

    $form['display_options']['css_classes'] = array(
      '#type' => 'textfield',
      '#title' => t('Additional CSS Classes'),
      '#default_value' => !empty($this->settings['display_options']['css']) ? $this->settings['display_options']['css'] : '',
    );

    $form['display_options']['hide_label'] = array(
      '#type' => 'checkbox',
      '#title' => t('Hide the Element Label'),
      '#default_value' => !empty($this->settings['display_options']['hide_label']),
    );

    $form['display_options']['hide_element'] = array(
      '#type' => 'checkbox',
      '#title' => t('Hide the Element'),
      '#description' => t('Completely hide the element from the form. If hidden an element will pass through its default values.'),
      '#default_value' => !empty($this->settings['display_options']['hide_element']),
    );

    $form['submit'] = array(
      '#type' => 'submit',
      '#value' => t('Save'),
      '#weight' => 100,
    );
    return $form;
  }

  /**
   * Validate the configure form for the element.
   */
  public function configureFormValidate($form, &$form_state, $flexiform) {}

  /**
   * Submit the configure form for the element.
   */
  public function configureFormSubmit($form, &$form_state, $flexiform) {
    $this->settings['label'] = $form_state['values']['label'];
    $this->settings['display_options']['css'] = $form_state['values']['display_options']['css_classes'];
    $this->settings['display_options']['hide_label'] = $form_state['values']['display_options']['hide_label'];
    $this->settings['display_options']['hide_element'] = $form_state['values']['display_options']['hide_element'];

    $flexiform->updateElement($this);
    $flexiform->save();

    $form_state['redirect'] = 'admin/structure/flexiforms/manage/' . $flexiform->form . '/form-fields';
  }

  /**
   * Build the remove form for the element.
   */
  public function removeForm($form, &$form_state, $flexiform) {
    $form['confirm'] = array(
      '#type' => 'hidden',
      '#value' => 1,
    );

    $question = t('Are you sure you want to remove @element?', array('@element' => $this->label()));
    drupal_set_title($question, PASS_THROUGH);

    $form['#attributes']['class'][] = 'confirmation';
    $form['description'] = array('#markup' => t('This action cannot be undone.'));

    $form['actions'] = array('#type' => 'actions');
    $form['actions']['submit'] = array(
      '#type' => 'submit',
      '#value' => t('Confirm'),
    );
    $form['actions']['cancel'] = array(
      '#type' => 'link',
      '#title' => t('Cancel'),
      '#href' => 'admin/structure/flexiforms/manage/' . $flexiform->form . '/form-fields',
    );

    $form['#theme'] = 'confirm_form';

    return $form;
  }

  /**
   * Validate the remove form for the element.
   */
  public function removeFormValidate($form, &$form_state, $flexiform) {}

  /**
   * Submit the remove form for the element.
   */
  public function removeFormSubmit($form, &$form_state, $flexiform) {
    $flexiform->removeElement($this);
    $flexiform->save();

    $form_state['redirect'] = 'admin/structure/flexiforms/manage/' . $flexiform->form . '/form-fields';
  }

  /**
   * Get the form element.
   */
  public function form($form, &$form_state, $entity) {
    $form[$this->element_namespace]['#weight'] = $this->getWeight();
    $form[$this->element_namespace]['#flexiform'] = $this->flexiform;
    $form[$this->element_namespace]['#flexiform_element'] = $this;

    if (!empty($form[$this->element_namespace]['#title'])
      && !empty($this->settings['display_options']['hide_label'])) {
      $form[$this->element_namespace]['#title_display'] = 'invisible';
    }

    if (!empty($form[$this->element_namespace]['#type'])
      && !empty($this->settings['display_options']['hide_element'])) {
      $form[$this->element_namespace]['#access'] = FALSE;
    }

    if (!empty($this->settings['display_options']['css'])) {
      foreach (explode(' ', $this->settings['display_options']['css']) as $class) {
        $form[$this->element_namespace]['#attributes']['class'][] = $class;
      }
    }

    return $form;
  }

  /**
   * Validate the form element.
   */
  public function formValidate($form, &$form_state, $entity) { }

  /**
   * Submit the form element.
   */
  public function formSubmit($form, &$form_state, $entity) { }

  /**
   * Extract the submitted values for this form element.
   */
  public function formExtractValues($form, &$form_state, $entity) {
    return array();
  }

  /**
   * Work out if the submitted value constitutes empty.
   */
  public function formIsEmpty($form, &$form_state, $entity) {
    $values = $this->formExtractValues($form, $form_state, $entity);
    return empty($values);
  }

  /**
   * Convert this object into a settings array.
   */
  public function toSettingsArray() {
    $settings = array(
      'entity_namespace' => $this->entity_namespace,
      'element_name' => $this->element_name,
      'weight' => $this->getWeight(),
      'label' => $this->label(),
    ) + $this->settings;

    return $settings;
  }

  /**
   * Make namespace for the element.
   */
  public function makeElementNamespace() {
    return $this->entity_namespace . ':' . $this->element_name;
  }

  /**
   * Get an array of ctools context for the flexiform.
   *
   * @param array $entities
   *   An array of entities keyed by namespace. If NULL, placeholder contexts
   *   will be used.
   *
   * @return ctools_context[]
   *   An array of ctools contexts.
   */
  public function getCtoolsContexts($entities = array()) {
    global $user;

    ctools_include('context');
    $contexts = array(
      'current-user' => ctools_context_create('entity:user', $user),
    );
    $contexts['current-user']->keyword = 'current-user';
    $contexts['current-user']->identifier = t('Logged-in user');
    foreach ($this->flexiform->entities as $namespace => $info) {
      $type = 'entity:' . $info['entity_type'];
      if (isset($entities[$namespace])) {
        $contexts[$namespace] = ctools_context_create($type, $entities[$namespace]);
      }
      else {
        $contexts[$namespace] = ctools_context_create_empty($type);
      }
      $contexts[$namespace]->keyword = $namespace;
      $contexts[$namespace]->identifier = $info['label'];
    }

    return $contexts;
  }

  /**
   * Replace ctools substitutions with their values.
   *
   * @param string $string
   *   The string we want to replace in.
   * @param array $entities
   *   An array of entities keyed by namespace. If NULL, placeholder contexts
   *   will be used.
   * @param array $keywords
   *   Optionally provide additional keywords to replace.
   *
   * @return string
   *   $string with it's substitutions replaced.
   */
  public function replaceCtoolsSubstitutions($string, $entities, $keywords = array()) {
    $contexts = $this->getCtoolsContexts($entities);
    return ctools_context_keyword_substitute($string, $keywords, $contexts);
  }

  /**
   * Build a list of possible ctools substitutions.
   *
   * @param array $keywords
   *   Optionally provide additional keywords to show.
   *
   * @return array
   *   A render array of substitutions.
   */
  public function getCtoolsSubstitutionsList($keywords = array()) {
    $content = array(
      '#theme' => 'table',
      '#header' => array(t('Keyword'), t('Value')),
      '#rows' => array(),
    );

    foreach ($this->getCtoolsContexts() as $context) {
      foreach (ctools_context_get_converters('%' . check_plain($context->keyword) . ':', $context) as $keyword => $title) {
        $content['#rows'][] = array(
          check_plain($keyword),
          t('@identifier: @title', array('@title' => $title, '@identifier' => $context->identifier)),
        );
      }
    }

    if (count($content['#rows'])) {
      return $content;
    }
    else {
      return array();
    }
  }

  /**
   * Get an element object.
   */
  public static function getElement($flexiform, $element_namespace) {
    $cached_elements = &drupal_static(__CLASS__, array());
    if (empty($cached_elements[$flexiform->form][$element_namespace])) {
      $settings = $flexiform->elements[$element_namespace];
      $entity_namespace = $settings['entity_namespace'];
      $element_name = $settings['element_name'];
      // If we don't have a cached version then create one.
      $entity_type = $flexiform->entities[$entity_namespace]['entity_type'];
      $bundle = $flexiform->entities[$entity_namespace]['bundle'];
      $element_info = flexiform_get_element_info($entity_type, $bundle, $element_name);

      $class_name = $element_info['class'];
      if (!class_exists($class_name)
        || !in_array('FlexiformElementInterface', class_implements($class_name))) {
        throw new Exception('Invalid Flexiform Element class ' . $class_name . ' for element ' . $element_name);
      }

      $cached_elements[$flexiform->form][$element_namespace] = new $class_name($flexiform, $settings, $element_namespace);
    }

    return $cached_elements[$flexiform->form][$element_namespace];
  }

  /**
   * Create an element object.
   */
  public static function createElement($flexiform, $settings) {
    $entity_namespace = $settings['entity_namespace'];
    $element_name = $settings['element_name'];
    // If we don't have a cached version then create one.
    $entity_type = $flexiform->entities[$entity_namespace]['entity_type'];
    $bundle = $flexiform->entities[$entity_namespace]['bundle'];
    $element_info = flexiform_get_element_info($entity_type, $bundle, $element_name);

    $class_name = $element_info['class'];
    if (!class_exists($class_name)
      || !in_array('FlexiformElementInterface', class_implements($class_name))) {
      throw new Exception('Invalid Flexiform Element class ' . $class_name . ' for element ' . $element_name);
    }

    return new $class_name($flexiform, $settings);
  }
}

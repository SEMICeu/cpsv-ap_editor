<?php
/**
 * @file
 * Specify base display classes for flexiforms.
 */

/**
 * Interface for all display classes.
 */
interface FlexiformDisplayInterface {

  /**
   * Get the base entity for the flexible form.
   *
   * @param Flexiform $flexiform
   *   A flexiform object.
   * @param array $configuration
   *   The configration as stored in the flexiform.
   * @param array $context
   *   (optional) An array of settings and values to help the plugin return the
   *   entity.
   *
   * @return stdClass|Entity
   *   The base entity for the form.
   */
  public function getBaseEntity($context = array());

  /**
   * Build the form ready for rendering.
   */
  public function build($context = array());

  /**
   * Get the title for a built form.
   */
  public function title($context = array());

  /**
   * Get the config form.
   */
  public function configForm($form, &$form_state);

  /**
   * Retrieve the flexiform for this display.
   *
   * @return Flexiform
   */
  public function getFlexiform();

  /**
   * Get the path, if applicable, for the display.
   *
   * @param $base_entity_id
   *   The identifier for the base entity.
   * @param ...
   *  Any additional arguments.
   *
   * @return string|NULL
   *   The path to the flexiform or NULL if there is none.
   */
  public function getPath($base_entity_id = NULL);

}

/**
 * Base class for all flexiform displays.
 */
class FlexiformDisplayBase implements FlexiformDisplayInterface {

  /**
   * The Flexiform the display is for.
   *
   * @var Flexiform
   */
  protected $flexiform;

  /**
   * {@inheritdoc}
   */
  public function getFlexiform() {
    return $this->flexiform;
  }

  public function __construct(Flexiform $flexiform, array $configuration = array()) {
    $this->flexiform = $flexiform;
    $this->configuration = $configuration;
  }

  /**
   * Get the base entity for a flexiform.
   *
   * By default we just create a new entity.
   */
  public function getBaseEntity($context = array()) {
    $info = entity_get_info($this->flexiform->base_entity);
    $values = array();

    if (!empty($info['entity keys']['bundle'])) {
      $values[$info['entity keys']['bundle']] = $this->flexiform->base_entity_bundle;
    }

    $base_entity = entity_create($this->flexiform->base_entity, $values);

    // Make sure the entity label is always set to play nicely with ctools.
    if (!empty($info['entity keys']['label'])
      && !isset($base_entity->{$info['entity keys']['label']})) {
      $base_entity->{$info['entity keys']['label']} = '';
    }

    return $base_entity;
  }

  /**
   * Is this display enabled.
   */
  public function isEnabled() {
    return !empty($this->configuration['enabled']);
  }

  /**
   * Build the form ready for rendering.
   */
  public function build($context = array()) {
    module_load_include('inc', 'flexiform', 'includes/flexiform.flexiform');
    $base_entity = $this->getBaseEntity($context);
    module_invoke_all('flexiform_prepare_base_entity', $base_entity, $this->flexiform, $this);

    // Allow other modules to change the form wrapper.
    $wrapper = 'flexiform_wrapper';
    drupal_alter('flexiform_wrapper', $wrapper, $this, $context);

    $args = isset($context['args']) ? $context['args'] : array();
    array_unshift($args, $base_entity);
    array_unshift($args, $this->flexiform);
    return call_user_func_array($wrapper, $args);
  }

  /**
   * Get the title.
   */
  public function title($context = array()) {
    return $this->flexiform->label;
  }

  /**
   * Build the config form.
   */
  public function configForm($form, &$form_state) {
    $form['enabled'] = array(
      '#type' => 'checkbox',
      '#title' => 'Enabled',
      '#default_value' => !empty($this->configuration['enabled']),
    );

    $form['title'] = array(
      '#type' => 'textfield',
      '#title' => t('Title'),
      '#default_value' => !empty($this->configuration['title']) ? $this->configuration['title'] : '',
    );

    return $form;
  }

  /**
   * {@inheritdoc}
   */
  public function getPath($base_entity_id = NULL) {
    return NULL;
  }

}

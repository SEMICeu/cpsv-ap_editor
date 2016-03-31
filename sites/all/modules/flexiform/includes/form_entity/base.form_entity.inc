<?php
/**
 * @file
 *  Contains class for a basic entity getter.
 */

/**
 * Form Entity Class for Managing the Base Entity.
 */
class FlexiformFormEntityBase implements FlexiformFormEntityInterface {

  /**
   * The Flexiform Entity Manager
   * @var FlexiformFormEntityManagerInterface
   */
  public $manager;

  /**
   * The namespace of this entity.
   * @var string
   */
  public $entity_namespace;

  /**
   * The type of this entity.
   * @var string
   */
  public $entity_type;

  /**
   * Details of the getter.
   * @var array
   */
  public $getter = array();

  /**
   * The settings for this entity on the flexiform.
   * @var array
   */
  public $settings = array();

  /**
   * Construct a Flexiform Form Entity class.
   *
   * @param FlexiformFormEntityManagerInterface $manager
   * @param string $namespace
   * @param array $getter
   * @param array $settings (Optional)
   */
  public function __construct(FlexiformFormEntityManagerInterface $manager, $namespace, $getter, $settings = array()) {
    $this->manager = $manager;
    $this->entity_namespace = $namespace;
    $this->getter = $getter;
    $this->settings = $this->manager->getEntitySettings($namespace) ?: $settings;
    $this->entity_type = $this->settings['entity_type'];
  }

  /**
   * Get a Parameter From the Entity Manager.
   *
   * @return
   *  An entity object.
   */
  public function getParam($param_name) {
    return $this->manager->getEntity($this->settings['parameters'][$param_name]);
  }

  /**
   * Get a Parameter's entity settings from the Entity Manager.
   */
  public function getParamSettings($param_name) {
    return $this->manager->getEntitySettings($this->settings['parameters'][$param_name]);
  }

  /**
   * Get the entity type of a parameter.
   */
  public function getParamType($param_name) {
    return $this->manager->getEntityType($this->settings['parameters'][$param_name]);
  }

  /**
   * Check bundle.
   *
   * @param $entity
   *   The entity to check.
   *
   * @return boolean
   *   True if the entity is the right bundle for this form entity. False
   *   otherwise.
   */
  public function checkBundle($entity) {
    list(,,$bundle) = entity_extract_ids($this->entity_type, $entity);
    if ($bundle == $this->settings['bundle']) {
      return TRUE;
    }

    return FALSE;
  }


  /**
   * {@inheritdoc}
   */
  public function getEntity() {
    return $this->manager->getBaseEntity();
  }

  /**
   * {@inheritdoc}
   */
  public function saveEntity($entity) {
    entity_save($this->entity_type, $entity);
  }

  /**
   * {@inheritdoc}
   */
  public function configForm($form, &$form_state) {
    if (empty($this->getter['configurable'])) {
      $form['message']['#markup'] = t('This entity has no configuration options available.');
    }

    return $form;
  }

  /**
   * {@inheritdoc}
   */
  public function configFormValidate($form, &$form_state) {}

  /**
   * {@inheritdoc}
   */
  public function configFormSubmit($form, &$form_state) {
    $this->settings = $form_state['values']['settings'] + $this->settings;
    $this->manager->updateEntitySettings($this->entity_namespace, $this->settings);
  }
}

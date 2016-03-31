<?php
/**
 * @file
 *  Contains the default entity manager for flexiforms.
 */

/**
 * Interface for form entities.
 */
interface FlexiformFormEntityManagerInterface {

  /**
   * Set the base entity.
   *
   * @param $entity
   *   The base entity object.
   * @param $base_entity_namespace
   *   The namespace of the base entity, usually 'base entity'.
   */
  public function setBaseEntity($entity, $base_entity_namespace);

  /**
   * Get the base entity.
   *
   * @return Entity|stdClass
   *   The base entity as set by setBaseEntity().
   *
   * @see FlexiformFormEntityManagerInterface::setBaseEntity()
   */
  public function getBaseEntity();

  /**
   * Prepare the entities needed for the form, either by creating or loading.
   */
  public function prepareEntities();

  /**
   * Prepare an individual entity.
   *
   * @param $namespace
   *   The namespace of the entity to be prepared.
   */
  public function prepareEntity($namespace);

  /**
   * Get the entities.
   *
   * @return array
   *   An array of all the entities ready to be used in this form.
   */
  public function getEntities();

  /**
   * Get an entity from the set.
   *
   * This method will prepare the entity if it has not already been prepared.
   *
   * @param string $namespace
   *   The namespace of the required entity.
   *
   * @return Entity|stdClass
   *   The prepared entity object.
   */
  public function getEntity($namespace);
}

/**
 * Class that manages entities in a flexiform.
 */
class FlexiformFormEntityManagerDefault implements FlexiformFormEntityManagerInterface {

  /**
   * The form builder.
   * @var FlexiformBuilderInterface
   */
  protected $builder;

  /**
   * The entity settings keyed by entity namespace.
   * @var array
   */
  protected $entitySettings = array();

  /**
   * The entities after they have been prepared.
   * @var array
   */
  protected $entities = array();

  /**
   * Whether the entities have been prepared or not.
   */
  protected $prepared = array();

  /**
   * The base entity key.
   * @var string
   */
  protected $base_entity_key = '';

  /**
   * An array of booleans keyed by namespace of entities to skip on save.
   *
   * @var bool[]
   */
  protected $skip_save = array();

  /**
   * Build and instance of this entity manager.
   *
   * @param array $settings
   *   An array of entity settings keyed by entity namespace.
   */
  public function __construct(array $settings, FlexiformBuilderInterface $builder) {
    $this->entitySettings = $settings;
    $this->builder = $builder;
  }

  /**
   * Set the base entity.
   */
  public function setBaseEntity($entity, $namespace = 'base_entity') {
    $this->entities[$namespace] = $entity;
    $this->prepared[$namespace] = TRUE;
    $this->base_entity_key = $namespace;
    return $this;
  }

  /**
   * Get the base entity.
   */
  public function getBaseEntity() {
    return $this->entities[$this->base_entity_key];
  }

  /**
   * Prepare the entities needed for the form, either by creating or loading.
   */
  public function prepareEntities() {
    foreach($this->entitySettings as $namespace => $info) {
      $this->prepareEntity($namespace);
    }
  }

  /**
   * Prepare an individual entity.
   *
   * @param $namespace
   *   The namespace of the entity to be prepared.
   *
   * @todo: Put some infinite recursion checking in place.
   */
  public function prepareEntity($namespace) {
    // Don't prepare again if and entity is already prepared.
    if (!empty($this->prepared[$namespace])) {
      return;
    }

    // If this is the base entity and its not prepared we must throw an error
    // as without a base entity set nothing is going to work.
    if ($namespace === $this->base_entity_key) {
      throw new Exception(t('Cannot Prepare Base Entity, this must be set using setBaseEntity.'));
    }

    $info = $this->getEntitySettings($namespace);

    // First make sure that any parameters have been prepared.
    if (!empty($info['parameters'])) {
      foreach ($info['parameters'] as $param_namespace) {
        if (empty($this->prepared[$param_namespace])) {
          $this->prepareEntity($param_namespace);
        }
      }
    }

    // Prepare this entity.
    if ($handler = $this->getEntityHandler($namespace)) {
      $this->entities[$namespace] = $handler->getEntity();
      $this->prepared[$namespace] = TRUE;
    }
  }

  /**
   * Get the entity handler class.
   *
   * @param $namespace
   *   The namespace of the entity to get.
   *
   * @return FlexiformFormEntityInterface
   *   The form entity handler object.
   */
  public function getEntityHandler($namespace) {
    if ($getter = $this->getEntityGetterInfo($namespace)) {
      $class = $getter['class'];

      if (empty($class) || !class_exists($class)
        || !in_array('FlexiformFormEntityInterface', class_implements($class))) {
        throw new Exception(t('Form Entity Class %class not found or not suitable using %getter', array('%getter' => $getter['label'], '%class' => $class)));
      }
      return new $class($this, $namespace, $getter);
    }
    return FALSE;
  }

  /**
   * Get the entities.
   *
   * @return array
   *   An array of all the entities ready to be used in this form.
   */
  public function getEntities() {
    // If not all entities are prepared.
    if (count(array_filter($this->prepared)) != count($this->entitySettings)) {
      $this->prepareEntities();
    }

    return $this->entities;
  }

  /**
   * Get the entities without checking if they are prepared.
   *
   * @return array
   *   An array of all the entities that have been prepared at the time this
   *   method is called.
   */
  public function getEntitiesRaw() {
    return $this->entities;
  }

  /**
   * Get an entity from the set.
   *
   * This method will prepare the entity if it has not already been prepared.
   *
   * @param string $namespace
   *   The namespace of the required entity.
   *
   * @return Entity|stdClass
   *   The prepared entity object.
   */
  public function getEntity($namespace) {
    if (empty($this->prepared[$namespace])) {
      $this->prepareEntity($namespace);
    }

    if (isset($this->entities[$namespace])) {
      return $this->entities[$namespace];
    }

    return FALSE;
  }

  /**
   * Get an entities settings from the settings array.
   *
   * @param string $namespace
   *   The namespace of the required entity settings.
   *
   * @return array
   *   Settings for the entity with namespace $namespace.
   */
  public function getEntitySettings($namespace) {
    if (isset($this->entitySettings[$namespace])) {
      return $this->entitySettings[$namespace];
    }

    return FALSE;
  }

  /**
   * Get the entity type from the settings.
   */
  public function getEntityType($namespace) {
    if (isset($this->entitySettings[$namespace])) {
      return $this->entitySettings[$namespace]['entity_type'];
    }

    return FALSE;
  }

  /**
   * Get an entities bundle from the settings.
   */
  public function getEntityBundle($namespace) {
    if (isset($this->entitySettings[$namespace])) {
      return $this->entitySettings[$namespace]['bundle'];
    }

    return FALSE;
  }

  /**
   * Get an entities getter info.
   */
  public function getEntityGetterInfo($namespace) {
    if (isset($this->entitySettings[$namespace])) {
      if (isset($this->entitySettings[$namespace]['getter'])) {
        $getter = $this->entitySettings[$namespace]['getter'];
        return flexiform_entity_getter_info($getter);
      }
    }

    return FALSE;
  }

  /**
   * Mark an entity to be skipped or not on save.
   *
   * @param string $namespace
   *   The namespace of the entity to skip.
   * @param bool $skip
   *   Whether to skip this entity on save.
   */
  public function skipOnSave($namespace, $skip) {
    $this->skip_save[$namespace] = $skip;
  }

  /**
   * Save Entities.
   */
  public function saveEntities() {
    foreach ($this->entitySettings as $namespace => $info) {
      // Allow forms to skip saving particular entities.
      if (!empty($this->skip_save[$namespace])) {
        continue;
      }

      if ($handler = $this->getEntityHandler($namespace)) {
        $handler->saveEntity($this->getEntity($namespace));
      }
    }

    // Reset the skip entities.
    $this->skip_save = array();
  }

  /**
   * Save Entity Settings.
   */
  public function saveEntitySettings() {
    $flexiform = $this->getBuilder()->getFlexiform();
    $flexiform->entities = $this->entitySettings;
    $flexiform->save();
  }

  /**
   * Remove and entity from this form.
   */
  public function removeEntitySettings($namespace) {
    unset($this->entitySettings[$namespace]);
    $this->saveEntitySettings();
  }

  /**
   * Update entity settings.
   */
  public function updateEntitySettings($namespace, $settings) {
    $this->entitySettings[$namespace] = $settings;
    $this->saveEntitySettings();
  }

  /**
   * Find out if any entities depend on this entity.
   */
  public function checkForDependants($namespace) {
    $dependants = array();
    foreach ($this->entitySettings as $curNamespace => $settings) {
      if (!empty($settings['parameters'])) {
        foreach ($settings['parameters'] as $param => $paramNamespace) {
          if ($namespace === $paramNamespace) {
            $dependants[$curNamespace] = $settings['label'];
          }
        }
      }
    }

    return $dependants;
  }

  /**
   * Get the builder.
   */
  public function getBuilder() {
    return $this->builder;
  }
}

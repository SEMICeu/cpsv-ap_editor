<?php
/**
 * @file flexiform.entity.inc
 *
 * Entity class and controller for the flexiform entity
 */

/**
 * The class used for flexiform entities
 */
class Flexiform extends Entity {

  public $form;
  public $label;
  public $elements = array();
  public $entities = array();
  public $displays = array();
  public $settings = array();

  public function __construct($values = array()) {
    parent::__construct($values, 'flexiform');
  }

  public function getBuilder($base_entity = NULL) {
    $builder = flexiform_builder_info($this->builder);
    $class_name = $builder['class'];

    if (!class_exists($class_name)) {
      throw new Exception('Builder class does not exist!');
    }

    return new $class_name($this, $base_entity);
  }

  /**
   * Get hold of a particular display.
   *
   * @param string $display
   *   The display we want to get.
   *
   * @return FlexiformDisplayInterface
   *   The flexiform display class if it exists, FlexiformDisplayNull if not.
   * 
   * @throws Exception
   */
  public function getDisplay($display) {
    if (empty($this->displays[$display])) {
      return new FlexiformDisplayNull($this);
    }

    $display_info = flexiform_display_info($display);
    if (empty($display_info)) {
      drupal_set_message(t('Display plugin @plugin not found.', array('@plugin' => $display)), 'error');
      return new FlexiformDisplayNull($this, $this->displays[$display]);
    }

    $class_name = $display_info['class'];
    if (!class_exists($class_name)) {
      throw new Exception('Display class does not exist.');
    }

    return new $class_name($this, $this->displays[$display]);
  }

  public function createDisplay($display) {
    $display_info = flexiform_display_info($display);
    if (empty($display_info)) {
      drupal_set_message(t('Display plugin @plugin not found.', array('@plugin' => $display)), 'error');
      return new FlexiformDisplayNull($this, $this->displays[$display]);
    }

    $class_name = $display_info['class'];
    if (!class_exists($class_name)) {
      throw new Exception('Display class does not exist.');
    }

    return new $class_name($this);
  }

  public function addElement($element) {
    $this->elements[$element->getElementNamespace()] = $element->toSettingsArray();
  }

  public function removeElement($element) {
    unset($this->elements[$element->getElementNamespace()]);
  }

  public function updateElement($element) {
    $this->elements[$element->getElementNamespace()] = $element->toSettingsArray();
  }
}

/**
 * The Controller for FlexiformSubmission entities
 */
class FlexiformController extends EntityAPIControllerExportable {
  public function __construct($entityType) {
    parent::__construct($entityType);
  }

   /**
   * Create a flexiform - we first set up the values that are specific
   * to our flexiform schema but then also go through the EntityAPIController
   * function.
   *
   * @param $type
   *   The machine-readable type of the flexiform_submission.
   *
   * @return
   *   A flexiform object with all default fields initialized.
   */
  public function create(array $values = array()) {
    // Add values that are specific to our FlexiformSubmission
    $values += array(
      'id' => '',
      'is_new' => TRUE,
    );
    $flexiform = parent::create($values);

    return $flexiform;
  }

  /**
   * Overrides EntityAPIController::save().
   */
  public function save($flexiform, DatabaseTransaction $transaction = NULL) {
    $return = parent::save($flexiform);
    $this->storeTags($flexiform);
  }

  /**
   * Overrides EntityAPIController::invoke().
   */
  public function invoke($hook, $entity) {
    parent::invoke($hook, $entity);

    if ($hook == 'insert' || $hook == 'update') {
      // Get hold of the display info.
      $display_info = flexiform_display_info();

      // Check for any displays implementing hook_menu.
      foreach ($entity->displays as $id => $display) {
        if (method_exists($display_info[$id]['class'], 'hook_menu')) {
          if ($hook == 'update') {
            // If the config has changed continue.
            if (isset($entity->original->displays[$id]) && $display == $entity->original->displays[$id]) {
              continue;
            }
          }

          // Mark the menu to be rebuilt.
          variable_set('menu_rebuild_needed', TRUE);
          break;
        }
      }
    }
  }

  /**
   * Overrides EntityAPIController::attachLoad().
   */
  public function attachLoad(&$queried_entities, $revision_id = FALSE) {
    $ids = array_keys($queried_entities);
    $tags = array();
    $result = db_select('flexiform_tags')
      ->fields('flexiform_tags', array('id', 'tag'))
      ->condition('id', $ids, 'IN')
      ->execute();
    foreach ($result as $row) {
      $tags[$row->id][] = $row->tag;
    }

    foreach ($queried_entities as $entity_id => &$entity) {
      $entity->tags = !empty($tags[$entity_id]) ? $tags[$entity_id] : array();
    }
  }

  /**
   * Store the tags associated with a given flexiform.
   */
  protected function storeTags($flexiform) {
    db_delete('flexiform_tags')
      ->condition('id', $flexiform->id)
      ->execute();
    if (!empty($flexiform->tags)) {
      foreach ($flexiform->tags as $tag) {
        db_insert('flexiform_tags')
          ->fields(array('id', 'tag'), array($flexiform->id, $tag))
          ->execute();
      }
    }
  }

  /**
   * {@inheritdoc}
   */
  protected function buildQuery($ids, $conditions = array(), $revision_id = FALSE) {
    $query = parent::buildQuery($ids, $conditions, $revision_id);
    $query_conditions = &$query->conditions();
    foreach ($query_conditions as &$condition) {
      // One entry in $query_conditions is a string with key '#conjunction'.
      // @see QueryConditionInterface::conditions().
      if (is_array($condition)) {
        // Support using 'tags' => array('tag1', 'tag2') as condition.
        if ($condition['field'] == 'base.tags') {
          $query->join('flexiform_tags', 'ft', 'base.id = ft.id');
          $condition['field'] = 'ft.tag';
        }
      }
    }
    return $query;
  }
}

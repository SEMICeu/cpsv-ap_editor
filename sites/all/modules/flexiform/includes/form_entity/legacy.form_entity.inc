<?php
/**
 * @file
 *  Contains Legacy Form Entity Class to Utilise Old Getters and Setters.
 */

/**
 * Legacy form entity class.
 */
class FlexiformFormEntityLegacy extends FlexiformFormEntityBase {

  /**
   * {@inheritdoc}
   */
  public function getEntity() {
    $getter = $this->getter;
    $base_entity = $this->manager->getBaseEntity();
    $entities = $this->manager->getEntitiesRaw();
    if (isset($getter['file']) && file_exists(drupal_get_path('module', $getter['module']) . '/' . $getter['file'])) {
      include_once drupal_get_path('module', $getter['module']) . '/' . $getter['file'];
    }

    $callback = $getter['getter callback'];
    if (function_exists($callback)) {
      // Todo: Sort out params
      return $callback($this->manager->getBuilder()->getFlexiform(), $base_entity, $entities, $this->entity_namespace, $getter);
    }
  }

  /**
   * {@inheritdoc}
   */
  public function saveEntity($entity) {
    $getter = $this->getter;
    $base_entity = $this->manager->getBaseEntity();
    $entities = $this->manager->getEntitiesRaw();
    if (isset($getter['file']) && file_exists(drupal_get_path('module', $getter['module']) . '/' . $getter['file'])) {
      include_once drupal_get_path('module', $getter['module']) . '/' . $getter['file'];
    }

    $callback = $getter['setter callback'];
    if (function_exists($callback)) {
      $callback($this->manager->getBuilder()->getFlexiform(), $base_entity, $entity, $this->entity_type, $entities, $this->entity_namespace, $getter);
    }
  }
}

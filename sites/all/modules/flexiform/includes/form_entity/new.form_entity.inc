<?php
/**
 * @file
 *  Contains class for a basic entity getter.
 */

/**
 * Form Entity that creates a new entity.
 */
class FlexiformFormEntityNew extends FlexiformFormEntityBase {

  /**
   * {@inheritdoc}
   */
  public function getEntity() {
    $values = array();

    // Work out if there is a bundle key.
    $entity_info = entity_get_info($this->entity_type);
    if (!empty($entity_info['entity keys']['bundle'])) {
      $bundle_key = $entity_info['entity keys']['bundle'];
      $values[$bundle_key] = $this->settings['bundle'];
    }

    // Make a new entity.
    $entity = entity_create($this->entity_type, $values);
    return $this->checkBundle($entity) ? $entity : FALSE;
  }
}

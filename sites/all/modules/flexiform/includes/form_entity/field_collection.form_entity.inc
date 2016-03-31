<?php
/**
 * @file
 *  Contains class for a basic entity getter.
 */

/**
 * Form entity to get a field collection item.
 */
class FlexiformFormEntityFieldCollection extends FlexiformFormEntityBase {

  /**
   * {@inheritdoc}
   */
  public function getEntity() {
    $settings = $this->settings;
    $base_type = $this->getParamType('base');

    // Get the base entity.
    $base_entity = $this->getParam('base');

    // Check we have enough information to load the entity.
    if (!$base_entity) {
      return FALSE;
    }

    if (empty($base_entity->{$this->getter['field_name']}[LANGUAGE_NONE][0]['value'])) {
      $target_entity = entity_create('field_collection_item', array('field_name' => $settings['bundle']));
      $target_entity->setHostEntity($base_type, $base_entity);
    }
    else {
      // Load the entity;
      $target_id = $base_entity->{$this->getter['field_name']}[LANGUAGE_NONE][0]['value'];
      $target_entity = entity_load_single('field_collection_item', $target_id);
    }

    if (!$target_entity) {
      return FALSE;
    }

    // Check the entity is the right bundle.
    list(,,$bundle) = entity_extract_ids('field_collection_item', $target_entity);
    if ($bundle != $settings['bundle']) {
      return FALSE;
    }

    return $target_entity;
  }
}

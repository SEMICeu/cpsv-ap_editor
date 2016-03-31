<?php
/**
 * @file
 *  Contains class for a basic entity getter.
 */

/**
 * Form entity to get a new reply.
 */
class FlexifromFormEntityNewReply extends FlexiformFormEntityBase {

  /**
   * {@inheritdoc}
   */
  public function getEntity() {
    $base_type = $this->getParamType('base');

    // Get the base entity.
    $base_entity = $this->getParam('base');

    // Check we have enough information to load the entity.
    if (!$base_entity) {
      return FALSE;
    }

    $base_settings = $this->getParamSettings('base');
    $base_bundle = $base_settings['bundle'];
    $instance = field_info_instance($base_type, $this->getter['field_name'], $base_bundle);
    $instance_id = $instance['id'];

    $base_id = entity_id($base_type, $base_entity);

    $values = array(
      'entity_id' => $base_id,
      'instance_id' => $instance_id,
      'entity_type' => $base_type,
      'bundle' => $this->settings['bundle'],
    );
    $new_reply = entity_create('reply', $values);

    if (!$new_reply) {
      return FALSE;
    }

    return $new_reply;
  }

  /**
   * {@inheritdoc}
   */
  public function saveEntity($entity) {
    // If the entity is still false do not save it!
    if ($entity === FALSE) {
      return;
    }

    $base_id = entity_id($this->getParamType('base'), $this->getParam('base'));
    $entity->entity_id = $base_id;

    parent::saveEntity($entity);
  }
}

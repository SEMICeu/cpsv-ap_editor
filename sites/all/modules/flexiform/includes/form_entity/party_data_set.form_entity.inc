<?php
/**
 * @file
 *  Contains class for a basic entity getter.
 */

/**
 * Form entity for attached entities.
 */
class FlexiformFormEntityPartyDataSet extends FlexiformFormEntityBase {

  /**
   * {@inheritdoc}
   */
  public function getEntity() {
    $settings = $this->settings;

    // Get the party entity.
    if ($party = $this->getParam('party')) {
      $data_set_name = $this->getter['data_set_name'];
      return $party->getDataSetController($data_set_name)->getEntity(0, TRUE);
    }

    return FALSE;
  }

  /**
   * {@inheritdoc}
   */
  public function saveEntity($entity) {
    $settings = $this->settings;

    // Get the party entity. We like to party. We like to, we like to party.
    $party = $this->getParam('party');
    $party->getDataSetController($this->getter['data_set_name'])
      ->attachEntity($entity)
      ->save(TRUE);
  }
}

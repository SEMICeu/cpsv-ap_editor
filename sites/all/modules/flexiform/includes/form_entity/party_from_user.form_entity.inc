<?php
/**
 * @file
 *  Contains class for a basic entity getter.
 */

/**
 * Form entity to get the party from a user.
 */
class FlexiformFormEntityPartyFromUser extends FlexiformFormEntityBase {

  /**
   * {@inheritdoc}
   */
  public function getEntity() {
    $settings = $this->settings;

    // Get the party entity.
    $user = $this->getParam('user');
    if ($user) {
      return party_user_get_party($user);
    }

    return FALSE;
  }
}

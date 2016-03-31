<?php
/**
 * @file
 *  Contains class for a basic entity getter.
 */

/**
 * Simple form entity for the current user.
 *
 * Note that if the user is logged out the function returns false.
 */
class FlexiformFormEntityCurrentUser extends FlexiformFormEntityBase {

  /**
   * {@inheritdoc}
   */
  public function getEntity() {
    global $user;

    if ($user->uid == 0) {
      return FALSE;
    }

    return user_load($user->uid);
  }
}

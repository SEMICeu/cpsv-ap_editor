<?php
/**
 * @file
 * Contains interface and base class for flexiform entities.
 */

/**
 * Interface for all flexiform entities.
 */
interface FlexiformFormEntityInterface {

  /**
   * Get the entity for the form.
   *
   * @return
   *   A loaded or created entity object ready for use in the form.
   */
  public function getEntity();

  /**
   * Save the entity upon submission of the form.
   *
   * @param $entity
   *   The entity object being saved.
   */
  public function saveEntity($entity);

  /**
   * Get the Configuration Form.
   */
  public function configForm($form, &$form_state);

  /**
   * Validate the configuration form.
   */
  public function configFormValidate($form, &$form_state);

  /**
   * Submit the Configuration Form.
   */
  public function configFormSubmit($form, &$form_state);
}

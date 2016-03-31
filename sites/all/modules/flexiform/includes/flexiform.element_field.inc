<?php
/**
 * @file
 * Contains interface for Field API elements.
 */

/**
 * Interface for field API elements.
 */
interface FlexiformElementFieldAPIInterface extends FlexiformElementInterface {

  /**
   * Get the instance array.
   *
   * Get the instance array including any overridden settings.
   */
  public function getInstance();

  /**
   * Get the field array.
   *
   * Get the field array including any overridden settings.
   */
  public function getField();

}

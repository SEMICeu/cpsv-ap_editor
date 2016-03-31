<?php
/**
 * @file flexiform.entity.inc
 *
 * Entity class and controller for the flexiform entity
 */

/**
 * The class used for flexiform entities
 */
class FlexiformWebform extends Entity {

  public $id;
  public $webform;
  public $label;
  public $data = array();

  public function __construct($values = array()) {
    parent::__construct($values, 'flexiform_webform');
  }
}

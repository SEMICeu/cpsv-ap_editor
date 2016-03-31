<?php
/**
 * @file
 * Specify builder classes for flexiforms.
 */

/**
 * Interface for all Flexiform Builders.
 */
interface FlexiformBuilderInterface {

  /**
   * Build the form for this flexiform.
   */
  public function form($form, &$form_state);

  /**
   * Validate the input for the form.
   */
  public function formValidate($form, &$form_state);

  /**
   * Submit the form.
   */
  public function formSubmit($form, &$form_state);
}

/**
 * Base class for flexiform builders.
 */
class FlexiformBuilder implements FlexiformBuilderInterface {

  /**
   * The flexiform object.
   */
  protected $flexiform;

  /**
   * The base entity.
   */
  protected $base_entity;

  /**
   * The base entity type.
   */
  protected $base_entity_type;

  /**
   * Construct the Flexiform Builder.
   */
  public function __construct($flexiform, $base_entity) {
    $this->flexiform = $flexiform;
    $this->base_entity = $base_entity;
    $this->base_entity_type = $flexiform->base_entity;
  }

  /**
   * Build the form for this flexiform.
   */
  public function form($form, &$form_state) {
    $form['#flexiform'] = $this->flexiform;
    $form['#flexiform_base_entity'] = $this->base_entity;
    $form['#flexiform_builder'] = $this;

    return $form;
  }

  /**
   * Validate the input for the form.
   */
  public function formValidate($form, &$form_state) {}

  /**
   * Submit the form.
   */
  public function formSubmit($form, &$form_state) {}

  /**
   * Invoke hooks to alter the build of this form.
   */
  public function invoke(&$form, &$form_state, $hook = '') {
    if (!empty($hook)){
      $hooks = array(
        'flexiform_build_' . $hook,
        'flexiform_build_' . $this->flexiform->builder . '_' . $hook,
      );
    }
    else {
      $hooks = array(
        'flexiform_build',
        'flexiform_build_' . $this->flexiform->builder,
      );
    }

    drupal_alter($hooks, $form, $form_state, $this->flexiform);
  }

  /**
   * Get the Flexiform.
   */
  public function getFlexiform() {
    return $this->flexiform;
  }
}

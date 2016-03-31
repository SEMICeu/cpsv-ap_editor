<?php
/**
 * @file
 * Define Display plugin for the edit panels pane.
 */

/**
 * Class for edit panel panes.
 */
class FlexiformDisplayEditPane extends FlexiformDisplayBase {

  /**
   * {@inheritdoc}
   */
  public function getBaseEntity($context = array()) {
    $base_entity = $context['base_entity']->data;

    //@todo: Check the entity is the right bundle.
    return $base_entity;
  }
}

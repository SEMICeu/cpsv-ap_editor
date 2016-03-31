<?php
/**
 * @file
 * Define Display plugin for if the plugin doesnt exist
 */

/**
 * Class for null displays.
 */
class FlexiformDisplayNull extends FlexiformDisplayBase {

  /**
   * {@inheritdoc}
   */
  public function isEnabled() {
    return FALSE;
  }

  /**
   * Build the form ready for rendering.
   */
  public function build($context = array()) {
    return array();
  }
}

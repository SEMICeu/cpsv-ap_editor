<?php
/**
 * @file
 * Contains the FlexiformRepeatableElement class.
 */

/**
 * Class for Elements that can be added to the form more than once with
 * different configurations.
 */
class FlexiformRepeatableElement extends FlexiformElement {

  /**
   * Make a namespace for the element.
   *
   * If that namespace is already taken then make a new one.
   */
  public function makeElementNamespace() {
    $namespace = parent::makeElementNamespace();
    $full_namespace = $namespace;

    $delta = 0;
    while(!empty($this->flexiform->elements[$full_namespace])) {
      $full_namespace = $namespace . ':' . $delta;
      $delta++;
    }

    return $full_namespace;
  }
}

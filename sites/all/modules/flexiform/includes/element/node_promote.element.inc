<?php
/**
 * @file
 * Contains class for the Node promote element.
 */

/**
 * Class to add the node promote field to a form.
 */
class FlexiformElementNodePromote extends FlexiformElement {

  /**
   * Return the form element for this FlexiformElement.
   */
  public function form($form, &$form_state, $entity, $language = LANGUAGE_NONE) {
    $parents = $form['#parents'];
    $parents[] = 'promote';

    $form[$this->element_namespace] = array(
      '#type' => 'checkbox',
      '#parents' => $parents,
      '#title' => $this->label(),
      '#default_value' => !empty($entity->promote) ? $entity->promote : FALSE,
    );

    $form = parent::form($form, $form_state, $entity);

    return $form;
  }

  /**
   * Validate the form element.
   */
  public function formValidate($form, &$form_state, $entity, $language = LANGUAGE_NONE) {
  }

  /**
   * Submit the form element.
   */
  public function formSubmit($form, &$form_state, $entity, $language = LANGUAGE_NONE) {
    $entity->promote = $this->formExtractValues($form, $form_state, $entity);
  }

  /**
   * Extract the submitted values for this form element.
   */
  public function formExtractValues($form, &$form_state, $entity) {
    $parents = $form['#parents'];
    $parents[] = $this->getEntityNamespace();
    $parents[] = 'promote';

    return drupal_array_get_nested_value($form_state['values'], $parents);
  }
}

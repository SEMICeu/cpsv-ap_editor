<?php
/**
 * @file
 * Contains class for the User roles element.
 */

/**
 * Class to add the node title field to a form.
 */
class FlexiformElementUserRoles extends FlexiformElement {

  /**
   * Return the form element for this FlexiformElement.
   */
  public function form($form, &$form_state, $entity, $language = LANGUAGE_NONE) {
    $parents = $form['#parents'];
    $parents[] = 'roles';

    // Work out the default value.
    $default = '';
    if (!empty($this->settings['default_value']['default_value'])) {
      $default = $this->settings['default_value']['default_value'];
    }
    if (!empty($this->settings['default_value']['use_tokens'])) {
      $default = $this->replaceCtoolsSubstitutions($default, $form['#flexiform_entities']);
    }

    $roles = array_map('check_plain', user_roles(TRUE));
    $form[$this->element_namespace] = array(
      '#type' => 'checkboxes',
      '#parents' => $parents,
      '#title' => $this->label(),
      '#required' => FALSE,
      '#default_value' => !empty($entity->roles) ? array_keys($entity->roles) : array(),
      '#options' => $roles,
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
    $roles = $this->formExtractValues($form, $form_state, $entity);
    $entity->roles = $roles;
  }

  /**
   * Extract the submitted values for this form element.
   */
  public function formExtractValues($form, &$form_state, $entity) {
    $parents = $form['#parents'];
    $parents[] = $this->getEntityNamespace();
    $parents[] = 'roles';

    $roles = drupal_array_get_nested_value($form_state['values'], $parents);
    return $roles;
  }

  /**
   * {@inheritdoc}
   */
  public function configureForm($form, &$form_state, $flexiform) {
    $form = parent::configureForm($form, $form_state, $flexiform);

    return $form;
  }

  /**
   * {@inheritdoc}
   */
  public function configureFormSubmit($form, &$form_state, $flexiform) {
    parent::configureFormSubmit($form, $form_state, $flexiform);
  }
}

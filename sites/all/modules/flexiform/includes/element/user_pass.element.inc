<?php
/**
 * @file
 * Contains class for the User pass element.
 */

/**
 * Class to add the node title field to a form.
 */
class FlexiformElementUserPass extends FlexiformElement {

  /**
   * Return the form element for this FlexiformElement.
   */
  public function form($form, &$form_state, $entity, $language = LANGUAGE_NONE) {
    $parents = $form['#parents'];
    $parents[] = 'pass';

    $form[$this->element_namespace] = array(
      '#type' => 'password_confirm',
      '#parents' => $parents,
      '#title' => $this->label(),
      '#required' => $this->settings['required'],
      '#maxlength' => 255,
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
    $pass = $this->formExtractValues($form, $form_state, $entity);
    $entity->pass = $pass;
  }

  /**
   * Extract the submitted values for this form element.
   */
  public function formExtractValues($form, &$form_state, $entity) {
    $parents = $form['#parents'];
    $parents[] = $this->getEntityNamespace();
    $parents[] = 'pass';

    $pass = drupal_array_get_nested_value($form_state['values'], $parents);
    return $pass;
  }

  /**
   * {@inheritdoc}
   */
  public function configureForm($form, &$form_state, $flexiform) {
    $form = parent::configureForm($form, $form_state, $flexiform);

    $form['required'] = array(
      '#type' => 'checkbox',
      '#title' => t('This field is required'),
      '#default_value' => !empty($this->settings['required']),
    );

    return $form;
  }

  /**
   * {@inheritdoc}
   */
  public function configureFormSubmit($form, &$form_state, $flexiform) {
    $this->settings['required'] = $form_state['values']['required'];

    parent::configureFormSubmit($form, $form_state, $flexiform);
  }

  /**
   * {@inheritdoc}
   */
  public function toSettingsArray() {
    $settings = parent::toSettingsArray();

    if (isset($this->settings['required'])) {
      $settings['required'] = $this->settings['required'];
    }

    return $settings;
  }
}

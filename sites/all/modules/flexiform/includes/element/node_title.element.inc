<?php
/**
 * @file
 * Contains class for the Node title element.
 */

/**
 * Class to add the node title field to a form.
 */
class FlexiformElementNodeTitle extends FlexiformElement {

  /**
   * Return the form element for this FlexiformElement.
   */
  public function form($form, &$form_state, $entity, $language = LANGUAGE_NONE) {
    $parents = $form['#parents'];
    $parents[] = 'title';

    // Work out the default value.
    $default = '';
    if (!empty($this->settings['default_value']['default_value'])) {
      $default = $this->settings['default_value']['default_value'];
    }
    if (!empty($this->settings['default_value']['use_tokens'])) {
      $default = $this->replaceCtoolsSubstitutions($default, $form['#flexiform_entities']);
    }

    $form[$this->element_namespace] = array(
      '#type' => 'textfield',
      '#parents' => $parents,
      '#title' => $this->label(),
      '#required' => TRUE,
      '#default_value' => !empty($entity->title) ? $entity->title : $default,
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
    $title = $this->formExtractValues($form, $form_state, $entity);
    $entity->title = $title;
  }

  /**
   * Extract the submitted values for this form element.
   */
  public function formExtractValues($form, &$form_state, $entity) {
    $parents = $form['#parents'];
    $parents[] = $this->getEntityNamespace();
    $parents[] = 'title';

    $title = drupal_array_get_nested_value($form_state['values'], $parents);
    return $title;
  }

  /**
   * {@inheritdoc}
   */
  public function configureForm($form, &$form_state, $flexiform) {
    $form = parent::configureForm($form, $form_state, $flexiform);

    $form['default_value'] = array(
      '#tree' => TRUE,
      '#type' => 'fieldset',
      '#title' => t('Default Value'),
      '#weight' => -5,
    );

    $form['default_value']['default_value'] = array(
      '#type' => 'textfield',
      '#title' => $this->label() ? $this->label() : t('Default'),
      '#default_value' => !empty($this->settings['default_value']['default_value']) ? $this->settings['default_value']['default_value'] : '',
      '#maxlength' => 255,
    );

    $form['default_value']['use_tokens'] = array(
      '#type' => 'checkbox',
      '#title' => t('Use Tokens in Default Value'),
      '#default_value' => !empty($this->settings['default_value']['use_tokens']),
    );

    $form['default_value']['contexts'] = array(
      '#title' => t('Substitutions'),
      '#type' => 'fieldset',
      '#collapsible' => TRUE,
      '#collapsed' => TRUE,
    );

    $form['default_value']['contexts']['contexts'] = $this->getCtoolsSubstitutionsList();

    return $form;
  }

  /**
   * {@inheritdoc}
   */
  public function configureFormSubmit($form, &$form_state, $flexiform) {
    $this->settings['default_value']['default_value'] = $form_state['values']['default_value']['default_value'];
    $this->settings['default_value']['use_tokens'] = $form_state['values']['default_value']['use_tokens'];

    parent::configureFormSubmit($form, $form_state, $flexiform);
  }
}

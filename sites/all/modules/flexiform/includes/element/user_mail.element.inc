<?php
/**
 * @file
 * Contains class for the User mail element.
 */

/**
 * Class to add the User mail field to a form.
 */
class FlexiformElementUserMail extends FlexiformElement {

  /**
   * Return the form element for this FlexiformElement.
   */
  public function form($form, &$form_state, $entity, $language = LANGUAGE_NONE) {
    $parents = $form['#parents'];
    $parents[] = 'mail';

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
      '#required' => FALSE,
      '#default_value' => !empty($entity->mail) ? $entity->mail : $default,
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
    $mail = $this->formExtractValues($form, $form_state, $entity);
    $entity->mail = $mail;
  }

  /**
   * Extract the submitted values for this form element.
   */
  public function formExtractValues($form, &$form_state, $entity) {
    $parents = $form['#parents'];
    $parents[] = $this->getEntityNamespace();
    $parents[] = 'mail';

    $mail = drupal_array_get_nested_value($form_state['values'], $parents);
    return $mail;
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

  /**
   * {@inheritdoc}
   */
  public function toSettingsArray() {
    $settings = parent::toSettingsArray();

    if (isset($this->settings['default_value'])) {
      $settings['default_value'] = $this->settings['default_value'];
    }

    return $settings;
  }
}

<?php

/**
 * @file
 * Contains class for the Custom HTML form element.
 */
class FlexiformElementCustomHtml extends FlexiformRepeatableElement {

  /**
   * Returns the custom html that is defined in the configure form.
   */
  public function form($form, &$form_state, $entity) {
    $parents = $form['#parents'];
    $parents[] = 'custom_html';

    $value = $form['#flexiform']->elements[$this->getElementNamespace()]['custom_html']['value'];
    $format = $form['#flexiform']->elements[$this->getElementNamespace()]['custom_html']['format'];

    // Perform some keyword substitution.
    $value = $this->replaceCtoolsSubstitutions($value, $form['#flexiform_entities']);

    $form[$this->element_namespace] = array(
      '#type' => 'item',
      '#markup' => check_markup($value, $format),
      '#parents' => $parents,
    );

    $form = parent::form($form, $form_state, $entity);

    return $form;
  }

  /**
   * Builds the configuration form for the form element.
   */
  public function configureForm($form, &$form_state, $flexiform) {
    $form['custom_html'] = array(
      '#type' => 'text_format',
      '#title' => $this->label(),
      '#format' => !empty($this->settings['custom_html']['format']) ? $this->settings['custom_html']['format'] : NULL,
      '#default_value' => !empty($this->settings['custom_html']['value']) ? $this->settings['custom_html']['value'] : '',
    );

    $form['contexts'] = array(
      '#title' => t('Substitutions'),
      '#type' => 'fieldset',
      '#collapsible' => TRUE,
      '#collapsed' => TRUE,
    );

    $form['contexts']['contexts'] = $this->getCtoolsSubstitutionsList();

    $form = parent::configureForm($form, $form_state, $flexiform);

    return $form;
  }

  /**
   * Validate the form element.
   */
  public function configureFormValidate($form, &$form_state, $flexiform) {
    parent::configureFormValidate($form, $form_state, $flexiform);
  }

  /**
   * Submit the form element.
   */
  public function configureFormSubmit($form, &$form_state, $flexiform) {
    $this->settings['custom_html'] = $form_state['values']['custom_html'];

    parent::configureFormSubmit($form, $form_state, $flexiform);
  }
}

<?php
/**
 * @file
 * Contains FlexiformElementFlexiform class.
 */

/**
 * Class to embed one flexiform inside another.
 */
class FlexiformElementFlexiform extends FlexiformElement {

  /**
   * The form name of the form.
   */
  protected $form;

  /**
   * Overrides FlexiformElement::__construct().
   */
  public function __construct($flexiform, $settings, $element_namespace = '') {
    parent::__construct($flexiform, $settings, $element_namespace);
    $this->form = $this->element_info['form'];
    $this->label = $settings['label'];
    $this->weight = $settings['weight'];
  }

  /**
   * Overrires FlexiformElement::label().
   */
  public function label() {
    return $this->settings['label'];
  }

  /**
   * Return the type.
   */
  public function type() {
    return 'Flexiform';
  }

  /**
   * Get the flexiform object of the embedded form.
   */
  public function getFlexiform() {
    return flexiform_load($this->form);
  }

  /**
   * Build the configure form for the element.
   */
  public function configureForm($form, &$form_state, $flexiform) {
    $form = parent::configureForm($form, $form_state, $flexiform);

    $link = l(t('Edit @flexiform', array('@flexiform' => $this->label())), 'admin/structure/flexiform/manage/' . $this->form);
    $form['description'] = array(
      '#weight' => -1,
      '#markup' => t('There are no settings for @flexiform as it is a nested form. You can edit the flexiform by clicking on the link below, however these changes will effect the form everywhere it is used.<br />!link', array('@flexiform' => $this->label(), '!link' => $link)),
    );

    return $form;
  }

  /**
   * Overrides FlexiformElement::form().
   */
  public function form($form, &$form_state, $entity) {
    $form = parent::form($form, $form_state, $entity);

    $sub_form = array(
      '#parents' => $form['#parents'] + array($this->element_namespace),
    );

    $builder = $this->getFlexiform()->getBuilder($entity);
    $form[$this->element_namespace]['#flexiform_builder'] = $builder;
    $form[$this->element_namespace]['#flexiform_element'] = $this;
    $form[$this->element_namespace]['#type'] = 'fieldset';
    $form[$this->element_namespace]['#title'] = $this->label();
    $form[$this->element_namespace] += $builder->form($sub_form, $form_state, $entity);

    return $form;
  }

  /**
   * Overrides FlexiformElement::formValidate().
   */
  public function formValidate($form, &$form_state, $entity, $language = LANGUAGE_NONE) {
    $sub_form = $form[$this->element_namespace];

    $sub_form['#flexiform_builder']->formValidate($sub_form, $form_state);
  }

  /**
   * Overrides FlexiformElement::formSubmit().
   */
  public function formSubmit($form, &$form_state, $entity, $language = LANGUAGE_NONE) {
    $sub_form = $form[$this->element_namespace];

    $sub_form['#flexiform_builder']->formSubmit($sub_form, $form_state);
  }

  /**
   * Overrides FlexifromElement::toSettingsArray();
   */
  public function toSettingsArray() {
    $settings = parent::toSettingsArray();
    $settings['type'] = 'flexiform';
    $settings['form'] = $this->form;
    $settings['label'] = $this->label();

    return $settings;
  }
}

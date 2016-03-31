<?php
/**
 * @file
 * Contains FlexiformElementField_taxonomy_term_reference class.
 */

/**
 * Class for taxonomy term reference Field API elements.
 */
class FlexiformElementField_taxonomy_term_reference extends FlexiformElementField {

  /**
   * {@inheritdoc}
   */
  public function configureForm($form, &$form_state, $flexiform) {
    $form = parent::configureForm($form, $form_state, $flexiform);

    // Expose the parent term field setting.
    $field = $this->getField();
    $form['field']['settings']['allowed_values'][0]['parent'] = array(
      '#type' => 'select',
      '#title' => t('Parent term'),
      '#options' => taxonomy_options_list(field_info_field($this->field_name), NULL, NULL, NULL),
      '#default_value' => isset($field['settings']['allowed_values'][0]['parent']) ? $field['settings']['allowed_values'][0]['parent'] : 0,
      '#empty_value' => 0,
    );

    return $form;
  }

}

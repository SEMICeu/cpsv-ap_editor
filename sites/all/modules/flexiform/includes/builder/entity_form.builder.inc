<?php
/**
 * @file
 * Contains the FlexiformBuilderEntityForm Class
 */

/**
 * Flexiform Builder Class for stock entity forms.
 */
class FlexiformBuilderEntityForm extends FlexiformBuilder {

  /**
   * Attach all fields for this entity to the form.
   */
  public function form($form, &$form_state) {
    $form = parent::form($form, $form_state);
    $form['#tree'] = TRUE;
    field_attach_form($this->base_entity_type, $this->base_entity, $form, $form_state);

    $this->invoke($form, $form_state);

    return $form;
  }

  /**
   * Validate the Fields
   */
  public function formValidate($form, &$form_state) {
    field_attach_form_validate($this->base_entity_type, $this->base_entity, $form, $form_state);
    $this->invoke($form, $form_state, 'validate');
  }

  /**
   * Submit the form.
   */
  public function formSubmit($form, &$form_state) {
    field_attach_submit($this->base_entity_type, $this->base_entity, $form, $form_state);
    $this->invoke($form, $form_state, 'submit');

    entity_save($this->base_entity_type, $this->base_entity);

    if (module_exists('rules')) {
      $base_entity_wrapper = entity_metadata_wrapper($flexiform->base_entity, $form['#flexiform_base_entity']);
      $args = array(
        'base_entity' => $base_entity_wrapper,
      );

      // Fire the most specific rules first and track back to the less specific.
      rules_invoke_event_by_args('flexiform_submit_'.$this->flexiform->form, array('base_entity' => $this->base_entity));
      rules_invoke_event_by_args('flexiform_group_submit_'.$this->flexiform->form_group, $args);
      rules_invoke_event_by_args('flexiform_any_submit', $args);
    }
  }
}

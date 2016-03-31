<?php
/**
 * @file
 * Contains FlexiformElementField_field_collection class.
 */

/**
 * Class for field collection reference Field API elements.
 */
class FlexiformElementField_field_collection extends FlexiformElementField {

  /**
   * {@inheritdoc}
   */
  public function form($form, &$form_state, $entity, $language = LANGUAGE_NONE) {
    $form = parent::form($form, $form_state, $entity, $language);

    // Make us compatible with the field collection table module.
    if (module_exists('field_collection_table')) {
      $instance = $this->getInstance();
      if ($instance['widget']['type'] == 'field_collection_table') {
        $form[$this->element_namespace][LANGUAGE_NONE]['#theme'] = 'field_collection_table_multiple_value_fields';
        $form[$this->element_namespace][LANGUAGE_NONE]['#pre_render'][] = 'field_collection_table_pre_render_multiple_fields';
      }
    }

    return $form;
  }

}

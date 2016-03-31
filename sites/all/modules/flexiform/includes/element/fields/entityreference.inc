<?php
/**
 * @file
 * Contains FlexiformElementField_entityreference class.
 */

/**
 * Class for entityreference Field API elements.
 */
class FlexiformElementField_entityreference extends FlexiformElementField {

  /**
   * {@inheritdoc}
   */
  public function formSubmit($form, &$form_state, $entity, $language = LANGUAGE_NONE) {
    parent::formSubmit($form, $form_state, $entity, $language);

    // Make us compatible with the inline_entity_form module.
    // @todo: Consider just invoking this by default.
    if (module_exists('inline_entity_form')) {
      inline_entity_form_field_attach_submit($this->getEntityType(), $entity, $this->extractFormElements($form), $form_state);
    }

    return $form;
  }

}

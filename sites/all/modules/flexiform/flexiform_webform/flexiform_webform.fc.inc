<?php
/**
 * @file
 * Contains the Field Comlete handler for the FlexiformWebform Module.
 */

class FlexiformFCHandler extends fcComplete {

  protected $ignored = array();

  /**
   * Get which form to use to calculate completeness.
   */
  public function getForm() {
    $webform = entity_load_single('flexiform_webform', $this->entity->webform);
    $form = '__none';
    if (!empty($webform->data['fc']['form'])) {
      $form = $webform->data['fc']['form'];
    }

    return $form;
  }

  /**
   * Calculate whether an element has been ignored.
   *
   * @return TRUE|FALSE
   *   True if the element is ignored.
   */
  public function elementIgnored($flexiform, $element_namespace) {
    if (isset($this->ignored[$element_namespace])) {
      return $this->ignored[$element_namespace];
    }

    $element = FlexiformElement::getElement($flexiform, $element_namespace);
    // Currently only support field api elements.
    if (!($element instanceof FlexiformElementField)) {
      $this->ignored[$element_namespace] = TRUE;
      return TRUE;
    }

    $field = $element->getField();
    $instance = $element->getInstance();
    $settings = !empty($instance['settings']['fc']) ? $instance['settings']['fc'] : array();

    // Work out if this field needs to be included.
    $this->ignored[$element_namespace] = TRUE;
    if (!empty($settings['fc_include']) && $settings['fc_include'] == 'always') {
      $this->ignored[$element_namespace] = FALSE;
    }
    else if (!empty($settings['fc_include']) && $settings['fc_include'] == 'entity_exists'
      && !empty($this->entities[$settings['fc_include__entity_exists']])) {
      $this->ignored[$element_namespace] = FALSE;
    }
    else if (!empty($settings['fc_include']) && $settings['fc_include'] == 'element_complete') {
      $other_element = $settings['fc_include__element_complete'];
      $complete = $this->getElementCompleteness($flexiform, $other_element);
      if ($complete === NULL) {
        $option = $settings['fc_include__element_complete_if_ignored'];
        if ($option == 'include') {
          $this->ignored[$element_namespace] = FALSE;
        }
        else {
          $this->ignored[$element_namespace] = TRUE;
        }
      }
      else {
        $this->ignored[$element_namespace] = !$complete;
      }
    }

    return $this->ignored[$element_namespace];
  }

  /**
   * Get Element completeness.
   */
  public function getElementCompleteness($flexiform, $element_namespace) {
    if ($this->elementIgnored($flexiform, $element_namespace)) {
      return NULL;
    }

    if (isset($this->completeness[$element_namespace])) {
      return $this->completeness[$element_namespace];
    }

    $this->completeness[$element_namespace] = $this->calculateElementCompleteness($flexiform, $element_namespace);
    return $this->completeness[$element_namespace];
  }

  /**
   * Calculate if a given element is complete.
   *
   * @return TRUE|FALSE|NULL
   *   True/False if the fields is complete/not complete. Null if the field is ignored.
   */
  public function calculateElementCompleteness($flexiform, $element_namespace) {
    // If we already know that this is ignored then return NULL;
    if ($this->elementIgnored($flexiform, $element_namespace)) {
      return NULL;
    }

    $element = FlexiformElement::getElement($flexiform, $element_namespace);
    $element_settings = $flexiform->elements[$element_namespace];
    $field = $element->getField();
    $instance = $element->getInstance();
    $settings = !empty($instance['settings']['fc']) ? $instance['settings']['fc'] : array();

    // Get Necessary information.
    $entity = $this->entities[$element_settings['entity_namespace']];
    $entity_type = $flexiform->entities[$element_settings['entity_namespace']]['entity_type'];
    $field_name = $field['field_name'];

    // If the entity is empty and the field isn't allowed to be empty then
    // obviously this form isn't complete.
    if (empty($entity)) {
      if (!empty($settings['fc_allow_empty'])) {
        return TRUE;
      }
      return FALSE;
    }

    // Go through the field items if any are "not empty" then
    // we count that as complete (a bit simplistic but okay).
    $field_items = field_get_items($entity_type, $entity, $field_name);
    if (empty($field_items)) {
      if (!empty($settings['fc_allow_empty'])) {
        // Complex fields can be set so that they optionally
        // have content, and if they don't they must be
        // counted as complete
        return TRUE;
      }
      return FALSE;
    }

    // Choose the right plugin for the field type.
    $plugin = fc_get_plugin($field['type']);

    if ($function = ctools_plugin_get_function($plugin, 'completeness check')) {
      // Process the field to determine whether it's complete, normally we
      // just compare the number of complete field_items with the cardinality
      // but other fields might be more complex (like matrix fields).
      $cardinality = ctools_plugin_get_function($plugin, 'cardinality check');
      return $cardinality($function, $field_items, $instance, $field);
    }

    return FALSE;
  }

  /**
   * {@inheritdoc}
   */
  public function completeness() {
    $form = $this->getForm();

    if ($form == '__none') {
      return parent::completeness();
    }

    $flexiform = entity_load_single('flexiform', $form);
    if (!$flexiform) {
      throw new Exception(t('Completeness Calculation Failed: The flexiform %form does not exist.'));
    }

    // Get the Entities.
    $this->entities = $flexiform->getBuilder($this->entity)->getEntityManager()->getEntities();

    $this->completeness = array();
    foreach ($flexiform->elements as $element_namespace => $element_settings) {
      $elementComplete = $this->getElementCompleteness($flexiform, $element_namespace);
    }

    $count_fields = count($this->completeness);
    $complete_fields = count(array_filter($this->completeness));

    $this->complete = $count_fields == $complete_fields;
    $this->percentage = $count_fields ? (int) ($complete_fields * 100 / $count_fields) : 100;
  }
}

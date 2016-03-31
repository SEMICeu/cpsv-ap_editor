<?php
/**
 * @file
 *  Contains class for a basic entity getter.
 */

/**
 * Form Entity Class for Managing the Entity Reference fields
 */
class FlexiformFormEntityEntityReference extends FlexiformFormEntityBase {

  /**
   * {@inheritdoc}
   */
  public function getEntity() {
    $settings = $this->settings;

    // Get the base entity.
    $base_entity = $this->getParam('base');

    // Get the delta.
    $delta = 0;
    if (!empty($settings['delta'])) {
      $delta = $settings['delta'];
    }

    // Check we have enough information to load the entity.
    if (!$base_entity || empty($base_entity->{$this->getter['field_name']}[LANGUAGE_NONE][$delta]['target_id'])) {
      return FALSE;
    }

    // Load the entity;
    $target_id = $base_entity->{$this->getter['field_name']}[LANGUAGE_NONE][$delta]['target_id'];
    $target_entity = entity_load_single($this->entity_type, $target_id);

    if (!$target_entity) {
      return FALSE;
    }

    return $this->checkBundle($target_entity) ? $target_entity : FALSE;
  }

  /**
   * {@inheritdoc}
   */
  public function saveEntity($entity) {
    // If the entity is still false do not save it!
    if ($entity === FALSE) {
      return;
    }

    // Save the entity.
    entity_save($this->entity_type, $entity);
    list($id,,) = entity_extract_ids($this->entity_type, $entity);

    // Get Settings for this Getter/Setter.
    $settings = $this->settings;

    // Get the delta.
    $delta = 0;
    if (!empty($settings['delta'])) {
      $delta = $settings['delta'];
    }

    // Get the base entity and save the reference back.
    $base_entity = $this->getParam('base');
    if (empty($base_entity->{$this->getter['field_name']}[LANGUAGE_NONE][$delta]['target_id'])
      || $base_entity->{$this->getter['field_name']}[LANGUAGE_NONE][$delta]['target_id'] != $id) {
      $base_entity->{$this->getter['field_name']}[LANGUAGE_NONE][$delta]['target_id'] = $id;
      $base_entity_type = $this->getParamType('base');
      entity_save($base_entity_type, $base_entity);
    }
  }

  /**
   * {@inheritdoc}
   */
  public function configForm($form, &$form_state) {
    $field = field_info_field($this->getter['field_name']);

    // Allow the selection of which one to use.
    if ($field['cardinality'] != 1) {
      $form['settings'] = array(
        '#type' => 'container',
        '#tree' => TRUE,
      );

      $options = array();
      $i = 0;
      while ($i < 10) {
        if ($field['cardinality'] > 0 && $i >= $field['cardinality']) {
          break;
        }

        $options[$i] = $i + 1;
        ++$i;
      }
      $form['settings']['delta'] = array(
        '#title' => t('Delta'),
        '#description' => t('Select which value of this field you want to use.'),
        '#type' => 'select',
        '#options' => $options,
        '#default_value' => !empty($this->settings['delta']) ? $this->settings['delta'] : 0,
      );
    }
    else {
      return parent::configForm($form, $form_state);
    }

    $form['actions'] = array(
      '#type' => 'actions',
    );
    $form['actions']['submit'] = array(
      '#type' => 'submit',
      '#value' => t('Save Settings'),
    );

    return $form;
  }
}

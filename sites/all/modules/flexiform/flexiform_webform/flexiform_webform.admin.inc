<?php
/**
 * @file
 * Model type editing UI.
 */

/**
 * UI Class
 */
class FlexiformWebformUIController extends EntityDefaultUIController {

  /**
   * {@inheritdoc}
   */
  public function hook_menu() {
    $items = parent::hook_menu();

    // Change the way they clone things.
    $items['admin/structure/flexiform_webforms/manage/%entity_object/clone']['page callback'] = 'flexiform_webform_ui_get_clone_form';

    return $items;
  }

  /**
   * {@inheritdoc}
   */
  public function operationCount() {
    $count = parent::operationCount() + 1;

    if (module_exists('panelizer')) {
      $count++;
    }

    return $count;
  }

  /**
   * {@inheritdoc}
   */
  public function overviewTableRow($conditions, $id, $entity, $additional_cols = array()) {
    $row = parent::overviewTableRow($conditions, $id, $entity, $additional_cols);
    // We want to put 2 operations in the table before clone, export and revert.
    $lastOperations = array_splice($row, -3, 3);

    // Add the manage forms element.
    $row[] = l(t('manage forms'), $this->path . '/manage/' . $id . '/forms');

    if (module_exists('panelizer')) {
      $row[] = l(t('panelizer'), 'admin/config/content/panelizer/flexiform_webform_submission/' . $id . '.default/content');
    }

    $row = array_merge($row, $lastOperations);
    return $row;
  }
}

/**
 * Generates the model type editing form.
 */
function flexiform_webform_form($form, &$form_state, $webform, $op = 'edit') {
  if ($op == 'clone') {
    $webform->label .= ' (cloned)';
    $webform->webform = '';
  }

  $form['#op'] = $op;

  $form['label'] = array(
    '#title' => t('Label'),
    '#type' => 'textfield',
    '#default_value' => $webform->label,
    '#description' => t('The human-readable name of this webform.'),
    '#required' => TRUE,
    '#size' => 30,
  );

  // Machine-readable type name.
  $form['webform'] = array(
    '#type' => 'machine_name',
    '#default_value' => isset($webform->webform) ? $webform->webform : '',
    '#maxlength' => 32,
    '#machine_name' => array(
      'exists' => 'flexiform_webform_load',
      'source' => array('label'),
    ),
    '#description' => t('A unique machine-readable name for this webform. It must only contain lowercase letters, numbers, and underscores.'),
    '#disabled' => empty($webform->is_new),
  );

  // Extended Cloning Options
  $form['cloned_from'] = array(
    '#type' => 'value',
    '#value' => !empty($webform->cloned_from) ? $webform->cloned_from : NULL,
  );

  $form['clone_fields'] = array(
    '#type' => 'checkbox',
    '#title' => t('Copy Field Definitions'),
    '#description' => t('This will copy the field instance settings from the original webform.'),
    '#default_value' => FALSE,
    '#access' => ($op == 'clone'),
  );

  $flexiforms = array();
  if ($op == 'clone') {
    $query = db_select('flexiform', 'f');
    $query->fields('f', array('form', 'label'));
    $query->condition('base_entity', 'flexiform_webform_submission');
    $query->condition('base_entity_bundle', $webform->cloned_from);
    $flexiforms = $query->execute()->fetchAllKeyed();
  }

  $form['clone_flexiforms'] = array(
    '#type' => 'checkboxes',
    '#title' => t('Clone Flexible Forms'),
    '#description' => t('Tick which flexible forms to clone with this flexiform webform. The machine name of this flexiform webform will replace or be appended to the machine names and paths associated with these flexible forms.'),
    '#default_value' => array(),
    '#options' => $flexiforms,
    '#access' => ($op =='clone'),
    '#states' => array(
      'visible' => array(
        ':input[name="clone_fields"]' => array('checked' => TRUE),
      ),
    ),
  );

  $info = entity_get_info('flexiform_webform_submission');
  if (module_exists('fc') && $info['fc handler class'] == 'FlexiformFCHandler') {
    $form['fc'] = array(
      '#type' => 'fieldset',
      '#tree' => TRUE,
      '#title' => t('Field Complete Settings'),
      '#description' => t('These options allow you to choose which flexible form (if any) to use to calculate the completeness of the submission. A submission will be considered complete based on the fields and settings in that flexiform.'),
    );

    $usable_forms = array(
      '__none' => t('Use Field Complete Default Handler'),
    );
    if ($webform->webform) {
      $usable_forms += db_select('flexiform', 'f')
        ->fields('f', array('form', 'label'))
        ->condition('base_entity', 'flexiform_webform_submission')
        ->condition('base_entity_bundle', $webform->webform)
        ->condition('builder', 'FlexiformBuilderFlexiform')
        ->execute()
        ->fetchAllKeyed();
    }

    $form['fc']['form'] = array(
      '#type' => 'select',
      '#options' => $usable_forms,
      '#default_value' => !empty($webform->data['fc']['form']) ? $webform->data['fc']['form'] : '__none',
      '#title' => t('Flexible Form'),
      '#description' => t('Which form to use to calculate the completeness of a flexiform webform submission.'),
    );
  }

  $form['actions'] = array('#type' => 'actions');
  $form['actions']['submit'] = array(
    '#type' => 'submit',
    '#value' => t('Save'),
  );

  return $form;
}

/**
 * Form API submit callback for the type form.
 */
function flexiform_webform_form_submit(&$form, &$form_state) {
  $webform = entity_ui_form_submit_build_entity($form, $form_state);
  if (module_exists('fc')) {
    $webform->data['fc'] = $form_state['values']['fc'];
  }
  $webform->save();

  $form_state['redirect'] = 'admin/structure/flexiform_webforms';

  // Copy fields.
  if (!empty($form_state['values']['clone_fields'])) {
    $original = $form_state['values']['cloned_from'];
    foreach (field_info_instances('flexiform_webform_submission', $original) as $instance) {
      $instance['bundle'] = $webform->webform;
      if (!field_info_instance('flexiform_webform_submission', $instance['field_name'], $webform->webform)) {
        field_create_instance($instance);
      }
    }
  }
  else {
    return;
  }

  // Copy Flexiforms
  $forms = array_filter($form_state['values']['clone_flexiforms']);
  $cloned = array();
  $original = $form_state['values']['cloned_from'];
  foreach (entity_load('flexiform', $forms) as $oldform) {
    $newForm = clone $oldform;

    // Clear id
    unset($newForm->id);

    // Work out new machine name.
    $new_form = str_replace($original, $webform->webform, $oldform->form, $count);
    $newForm->form = (!empty($count)) ? $new_form : $newForm->form . '_' . $webform->webform;

    // Stop from getting too long
    if (strlen($newForm->form) > 252) {
      $newForm->form = substr($newForm->form, 0, 252);
    }

    // Reduce duplicate.
    $query = db_select('flexiform')->condition('form', $newForm->form)->countQuery();
    if ($query->execute()->fetchField() > 0) {
      $i = 1;
      $new_form = $newForm->form . '_' . $i;

      $query = db_select('flexiform')->condition('form', $new_form)->countQuery();
      while ($query->execute()->fetchField() > 0) {
        $new_form = $newForm->form . "_{$i}";
        $query = db_select('flexiform')->condition('form', $new_form)->countQuery();
        $i++;
      }

      $newForm->form = $new_form;
    }

    // Change base_entity_bundle
    $newForm->base_entity_bundle = $webform->webform;
    $newForm->entities['base_entity']['bundle'] = $webform->webform;

    // Work out menu paths:
    $displays = array(
      'flexiform_create_entity_page',
      'flexiform_edit_entity_page'
    );

    foreach ($displays as $display) {
      if ($newForm->displays[$display]['enabled']) {
        $path = &$newForm->displays[$display]['path'];
        $new_path = str_replace($original, $webform->webform, $path, $count);
        $path = (!empty($count)) ? $new_path : $path . '/' . $webform->webform;
      }
    }

    $newForm->save();

    if (module_exists('flexiform_element_group')) {
      flexiform_element_group_clone_groups($oldform->form, $newForm->form);
    }

    $cloned[] = l($newForm->label, "admin/structure/flexiforms/manage/{$newForm->form}", array('attributes' => array('target' => '_blank')));
  }

  if (!empty($cloned)) {
    $message = t('The following flexiforms have been cloned:', array('%field' => $instance['label']));
    $list = array(
      '#theme' => 'item_list',
      '#items' => $cloned,
    );
    $message .= drupal_render($list);
    drupal_set_message($message);
  }
}

/**
 * Form API submit callback for the delete button.
 */
function flexiform_webform_form_submit_delete(&$form, &$form_state) {
  $form_state['redirect'] = 'admin/structure/flexiform_webforms/manage/' . $form_state['webform']->webform . '/delete';
}

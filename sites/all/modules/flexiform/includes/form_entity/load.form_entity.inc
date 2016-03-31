<?php
/**
 * @file
 *  Contains class for a basic entity getter.
 */

/**
 * Form Entity that loads an entity from an id stored in config.
 */
class FlexiformFormEntityLoad extends FlexiformFormEntityBase {

  /**
   * {@inheritdoc}
   */
  public function getEntity() {
    $id = $this->settings['entity_id'];

    // Don't do anything if the $id is empty.
    if (empty($id)) {
      return FALSE;
    }

    // Handle path arguments.
    if (stripos($id, '%') === 0 && is_numeric(ltrim($id, '%'))) {
      // Try to use the standard drupal menu handling. This allows the use
      // of paths with load wildcards in them.
      $position = ltrim($id, '%');
      $menu_object = menu_get_object($this->entity_type, $position);
      if ($menu_object && $this->checkBundle($menu_object)) {
        return $menu_object;
      }
      else if ($menu_object) {
        return FALSE;
      }

      // If the menu system hasn't yielded a result use the arg function.
      $id = arg($position);
    }

    $entity = entity_load_single($this->entity_type, $id);
    return $this->checkBundle($entity) ? $entity : FALSE;
  }

  /**
   * {@inheritdoc}
   */
  public function configForm($form, &$form_state) {
    $form['settings'] = array(
      '#type' => 'container',
      '#tree' => TRUE,
    );

    $form['settings']['entity_id'] = array(
      '#type' => 'textfield',
      '#title' => t('Entity ID'),
      '#description' => t('You may also specify a position in the path to take the id from, e.g. "%2" will use the third section of the page path.'),
      '#default_value' => !empty($this->settings['entity_id']) ? $this->settings['entity_id'] : '',
      '#required' => TRUE,
    );

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

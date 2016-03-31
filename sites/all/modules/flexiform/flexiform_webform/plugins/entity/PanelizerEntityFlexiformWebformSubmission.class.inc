<?php
/**
 * @file
 * Class for the Panelizer commerce_order entity plugin.
 */

// Make sure the default file is loaded.
module_load_include('php', 'panelizer', 'plugins/entity/PanelizerEntityDefault.class');

/**
 * Panelizer Entity commerce_order plugin class.
 *
 * Handles commerce_order specific functionality for Panelizer.
 */
class PanelizerEntityFlexiformWebformSubmission extends PanelizerEntityDefault {
  /**
   * True if the entity supports revisions.
   */
  public $supports_revisions = FALSE; // Can be sniffed
  public $entity_admin_root = 'admin/structure/flexiform_webform/manage/%flexiform_webform'; // Can be sniffed
  public $entity_admin_bundle = 4; // Can be sniffed.
  public $views_table = 'flexiform_webform_submission';
  public $uses_page_manage = FALSE;

  /**
   * Determine if the entity allows revisions.
   */
  public function entity_allows_revisions($entity) {
    $retval[0] = $this->supports_revisions;
    $retval[1] = user_access('administer flexiform_webform_submissions');

    return $retval;
  }

  /**
   * Implements a delegated hook_form_alter.
   *
   * We want to add Panelizer settings for the bundle to the flexiform form.
   */
  public function hook_form_alter(&$form, &$form_state, $form_id) {
    if ($form_id == 'flexiform_webform_form') {
      if (isset($form['form'])) {
        $bundle = $form['form']['#default_value'];
        $this->add_bundle_setting_form($form, $form_state, $bundle, array('form'));
      }
    }
  }

  /**
   * Implements entity_access();
   */
  public function entity_access($op, $entity) {
    return entity_access($op, 'flexiform_webform_submission', $entity);
  }

  /**
   * Implements entity_save();
   */
  public function entity_save($entity) {
    return entity_save('flexiform_webform_submission', $entity);
  }
}


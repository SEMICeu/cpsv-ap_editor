<?php
/**
 * @file
 * Admin functions for Flexiform Multistep.
 */

/**
 * Implements hook_form_FORM_ID_alter() for flexiform_manage_form_fields_form().
 */
function _flexiform_multistep_form_flexiform_manage_form_fields_form_alter(&$form, &$form_state, $form_id) {
  $settings = isset($form['#flexiform']->settings['flexiform_multistep']) ? $form['#flexiform']->settings['flexiform_multistep'] : array();

  array_unshift($form['#submit'], 'flexiform_multistep_manage_form_fields_submit');

  $form['additional_settings']['flexiform_multistep'] = array(
    '#type' => 'fieldset',
    '#title' => t('Mulit-page'),
    '#description' => t('Configure this form for server side multi-pages.'),
    '#collapsible' => TRUE,
    '#collapsed' => FALSE,
    '#parents' => array('flexiform_multistep'),
  );

  $form['additional_settings']['flexiform_multistep']['enabled'] = array(
    '#type' => 'checkbox',
    '#title' => t('Enable server side multi-page for this form.'),
    '#default_value' => !empty($settings['enabled']),
  );

  ctools_include('wizard');
  $ctools_wizard_defaults = array('id' => NULL, 'forms' => array());
  ctools_wizard_defaults($ctools_wizard_defaults);

  $form['additional_settings']['flexiform_multistep']['form_info']['show trail'] = array(
    '#type' => 'checkbox',
    '#title' => t('Show progress indicator'),
    '#default_value' => isset($settings['form_info']['show trail']) ? $settings['form_info']['show trail'] : $ctools_wizard_defaults['show trail'],
    '#states' => array(
      'visible' => array(
        ':input[name="flexiform_multistep[enabled]"]' => array('checked' => TRUE),
      ),
    ),
  );

  $form['additional_settings']['flexiform_multistep']['form_info']['next text'] = array(
    '#type' => 'textfield',
    '#title' => t('Next button text'),
    '#default_value' => isset($settings['form_info']['next text']) ? $settings['form_info']['next text'] : NULL,
    '#description' => t('Leave blank to use %text', array('%text' => $ctools_wizard_defaults['next text'])),
    '#states' => array(
      'visible' => array(
        ':input[name="flexiform_multistep[enabled]"]' => array('checked' => TRUE),
      ),
    ),
  );

  $form['additional_settings']['flexiform_multistep']['form_info']['finish text'] = array(
    '#type' => 'textfield',
    '#title' => t('Finish button text'),
    '#default_value' => isset($settings['form_info']['finish text']) ? $settings['form_info']['finish text'] : NULL,
    '#description' => t('Leave blank to use %text', array('%text' => $ctools_wizard_defaults['finish text'])),
    '#states' => array(
      'visible' => array(
        ':input[name="flexiform_multistep[enabled]"]' => array('checked' => TRUE),
      ),
    ),
  );

  $form['additional_settings']['flexiform_multistep']['form_info']['show back'] = array(
    '#type' => 'checkbox',
    '#title' => t('Show back button'),
    '#default_value' => isset($settings['form_info']['show back']) ? $settings['form_info']['show back'] : $ctools_wizard_defaults['show back'],
    '#states' => array(
      'visible' => array(
        ':input[name="flexiform_multistep[enabled]"]' => array('checked' => TRUE),
      ),
    ),
  );

  $form['additional_settings']['flexiform_multistep']['form_info']['back text'] = array(
    '#type' => 'textfield',
    '#title' => t('Back button text'),
    '#default_value' => isset($settings['form_info']['back text']) ? $settings['form_info']['back text'] : NULL,
    '#description' => t('Leave blank to use %text', array('%text' => $ctools_wizard_defaults['back text'])),
    '#states' => array(
      'visible' => array(
        ':input[name="flexiform_multistep[enabled]"]' => array('checked' => TRUE),
        ':input[name="flexiform_multistep[form_info][show back]"]' => array('checked' => TRUE),
      ),
    ),
  );

  $form['additional_settings']['flexiform_multistep']['form_info']['show cancel'] = array(
    '#type' => 'checkbox',
    '#title' => t('Show cancel button'),
    '#default_value' => isset($settings['form_info']['show cancel']) ? $settings['form_info']['show cancel'] : $ctools_wizard_defaults['show cancel'],
    '#states' => array(
      'visible' => array(
        ':input[name="flexiform_multistep[enabled]"]' => array('checked' => TRUE),
      ),
    ),
  );

  $form['additional_settings']['flexiform_multistep']['form_info']['cancel text'] = array(
    '#type' => 'textfield',
    '#title' => t('Cancel button text'),
    '#default_value' => isset($settings['form_info']['cancel text']) ? $settings['form_info']['cancel text'] : NULL,
    '#description' => t('Leave blank to use %text', array('%text' => $ctools_wizard_defaults['cancel text'])),
    '#states' => array(
      'visible' => array(
        ':input[name="flexiform_multistep[enabled]"]' => array('checked' => TRUE),
        ':input[name="flexiform_multistep[form_info][show cancel]"]' => array('checked' => TRUE),
      ),
    ),
  );

  $form['additional_settings']['flexiform_multistep']['form_info']['show return'] = array(
    '#type' => 'checkbox',
    '#title' => t('Show return button'),
    '#description' => t('Saves and keeps you on the current page.'),
    '#default_value' => isset($settings['form_info']['show return']) ? $settings['form_info']['show return'] : $ctools_wizard_defaults['show return'],
    '#states' => array(
      'visible' => array(
        ':input[name="flexiform_multistep[enabled]"]' => array('checked' => TRUE),
      ),
    ),
  );

  $form['additional_settings']['flexiform_multistep']['form_info']['return text'] = array(
    '#type' => 'textfield',
    '#title' => t('Return button text'),
    '#default_value' => isset($settings['form_info']['return text']) ? $settings['form_info']['return text'] : NULL,
    '#description' => t('Leave blank to use %text', array('%text' => $ctools_wizard_defaults['return text'])),
    '#states' => array(
      'visible' => array(
        ':input[name="flexiform_multistep[enabled]"]' => array('checked' => TRUE),
        ':input[name="flexiform_multistep[form_info][show return]"]' => array('checked' => TRUE),
      ),
    ),
  );

  $form['additional_settings']['flexiform_multistep']['save_entities'] = array(
    '#type' => 'checkbox',
    '#title' => t('Save entities on the submission of each page'),
    '#description' => t('If checked, all entities used on the page will be saved when the page is submitted rather than all entities being saved at the end. This can be overridden per page.'),
    '#default_value' => !empty($settings['save_entities']),
    '#states' => array(
      'visible' => array(
        ':input[name="flexiform_multistep[enabled]"]' => array('checked' => TRUE),
      ),
    ),
  );


  $tokens = array(
    '#theme' => 'item_list',
    '#items' => array(),
  );
  foreach ($form['#flexiform']->entities as $namespace => $info) {
    $tokens['#items'][] = format_string('%namespace: @label', array(
      '%namespace' => '%' . $namespace,
      '@label' => $info['label'],
    ));
  }
  $form['additional_settings']['flexiform_multistep']['redirect'] = array(
    '#type' => 'textfield',
    '#title' => t('Redirect on completion'),
    '#description' => t('Provide an internal path to redirect to on completion of the form. The following tokens are available: !tokens', array(
      '!tokens' => drupal_render($tokens),
    )),
    '#default_value' => isset($settings['redirect']) ? $settings['redirect'] : NULL,
  );
}

/**
 * Submission handler for flexiform_manage_form_fields_form().
 *
 * See _flexiform_multistep_form_flexiform_manage_form_fields_form_alter().
 */
function flexiform_multistep_manage_form_fields_submit(&$form, &$form_state) {
  $settings = $form_state['values']['flexiform_multistep'];

  // Clear out empties.
  $settings['form_info'] = array_filter($settings['form_info']);

  $form_state['#flexiform']->settings['flexiform_multistep'] = $settings;
}

/**
 * Add additional settings for multi-page forms.
 */
function _flexiform_multistep_field_group_format_settings($group) {
  $flexiform = flexiform_load($group->bundle);
  $settings = isset($flexiform->settings['flexiform_multistep']) ? $flexiform->settings['flexiform_multistep'] : array();

  $form = array(
    'instance_settings' => array(
      '#tree' => TRUE,
      '#weight' => 2,
    ),
  );

  $form['instance_settings']['save_entities'] = array(
    '#type' => 'select',
    '#title' => t('Save entities on page submission'),
    '#empty_option' => t('Use form default'),
    '#options' => array(
      'y' => t('Yes'),
      'n' => t('No'),
      'c' => t('Choose which entities to save'),
    ),
    '#description' => t('Choose whether to save entities on page submission. The current default is %default.', array(
      '%default' => !empty($settings['save_entities']) ? t('yes') : t('no'),
    )),
    '#default_value' => isset($group->format_settings['instance_settings']['save_entities']) ? $group->format_settings['instance_settings']['save_entities'] : NULL,
  );

  $form['instance_settings']['save_entities_custom'] = array(
    '#type' => 'checkboxes',
    '#title' => t('Entities to save on page submission'),
    '#options' => array(),
    '#description' => t('Select which entities to save when this page is submitted. The last page will save all entities if none are selected.'),
    '#default_value' => isset($group->format_settings['instance_settings']['save_entities_custom']) ? $group->format_settings['instance_settings']['save_entities_custom'] : array(),
    '#states' => array(
      'visible' => array(
        ':input[name="fields[' . $group->group_name . '][format_settings][settings][instance_settings][save_entities]"]' => array('value' => 'c'),
      ),
    ),
  );
  foreach ($flexiform->entities as $namespace => $entity) {
    $form['instance_settings']['save_entities_custom']['#options'][$namespace] = $entity['label'];
  }


  $form['instance_settings']['page_title'] = array(
    '#type' => 'textfield',
    '#title' => t('Page title'),
    '#description' => t('Optionally provide a page title.'),
    '#default_value' => isset($group->format_settings['instance_settings']['page_title']) ? $group->format_settings['instance_settings']['page_title'] : NULL,
  );

  return $form;
}

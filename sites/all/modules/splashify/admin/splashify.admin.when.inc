<?php

/**
 * @file
 * The admin "When" tab.
 *
 * Admin functionality that determines when the splash page should show up
 * on the website.
 */

/**
 * "When" settings tab.
 */
function splashify_admin_when_form($form, &$form_state) {
  $roles = user_roles(TRUE);
  $form_roles = array();
  
  foreach($roles as $v){
    $form_roles[$v] = $v;
  }
  
  $form = array();

  $form['description'] = array(
    '#markup' => '<p>' . t('When should the splash page show up? This is also where the main mobile option is defined.') . '</p>',
  );

  $form['splashify_when_anonymous'] = array(
    '#type' => 'checkbox',
    '#title' => t('Only show to Anonymous users'),
    '#default_value' => variable_get('splashify_when_anonymous', FALSE),
  );

  // Add control to disable referrer check
  // @see splashify_init.js
  $form['disable_referrer_check'] = array(
    '#type' => 'checkbox',
    '#title' => t('Disable referrer check'),
    '#default_value' => variable_get('disable_referrer_check', 0),
    '#description' => t('Show splash page even when page was loaded from an internal page.<br /><strong>WARNING:</strong> Be very careful with this setting when you use the Always option and have the splash page displayed on every page! This would prevent you from accessing the admin.'),
  );

  $form['desktop'] = array(
    '#type' => 'fieldset',
    '#title' => t('Desktop Settings'),
  );

  $form['desktop']['splashify_when_desktop_frequency'] = array(
    '#type' => 'select',
    '#title' => t('How often should visitors see the splash page?'),
    '#default_value' => variable_get('splashify_when_desktop_frequency', 'never'),
    '#options' => array(
      'never' => t('Never (off)'),
      'always' => t('Always'),
      'once' => t('Once'),
      'daily' => t('Daily'),
      'weekly' => t('Weekly'),
    ),
  );
  
  $form['roles'] = array(
    '#type' => 'fieldset',
    '#title' => t('Display for roles'),
    '#states' => array(
      'visible' => array(
        ':input[name="splashify_when_anonymous"]' => array('checked' => FALSE),
      )      
    ),
  );

  $form['roles']['splashify_when_roles'] = array (
    '#type' => 'checkbox',
    '#title' => 'Restrict to selected roles',
    '#default_value' => variable_get('splashify_when_roles', FALSE),
  );
  
  $form['roles']['splashify_when_roles_options'] = array(
    '#type' => 'checkboxes',
    '#title' => t('Which roles should this apply to'),    
    '#default_value' => variable_get('splashify_when_roles_options', array('authenticated user')),
    '#options' => $form_roles,
    '#description' => t('Select the roles for which it will apply'),
  );

  $form['mobile'] = array(
    '#type' => 'fieldset',
    '#title' => t('Mobile Settings'),
  );

  $mobile_detect = libraries_load('Mobile_Detect');
  if ($mobile_detect['installed']) {
    $form['mobile']['splashify_when_mobile'] = array(
      '#type' => 'checkbox',
      '#title' => t('Enable Unique Mobile Splash'),
      '#default_value' => variable_get('splashify_when_mobile', 0),
      '#disabled' => FALSE,
      '#ajax' => array(
        'callback' => 'splashify_ajax_when_mobile_callback',
        'wrapper' => 'when-mobile-settings',
        'method' => 'replace',
        'effect' => 'slide',
      ),
    );

    // Set a variable that is either defined by the ajax checkbox, or a
    // previously saved value.
    if (isset($form_state['values']['splashify_when_mobile'])) {
      $when_mobile_set = $form_state['values']['splashify_when_mobile'];
    }
    else {
      $when_mobile_set = variable_get('splashify_when_mobile', 0);
    }

    $form['mobile']['options']['begin'] = array('#markup' => '<div id="when-mobile-settings">');

    // If the mobile splash is enabled, display the mobile options.
    if ($when_mobile_set == 1) {
      $form['mobile']['options']['splashify_when_mobile_frequency'] = array(
        '#type' => 'select',
        '#title' => t('How often should mobile visitors see the mobile splash page?'),
        '#default_value' => variable_get('splashify_when_mobile_frequency', 'never'),
        '#options' => array(
          'never' => t('Never (off)'),
          'always' => t('Always'),
          'once' => t('Once'),
          'daily' => t('Daily'),
          'weekly' => t('Weekly'),
        ),
      );

      $form['mobile']['options']['splashify_when_mobile_test'] = array(
        '#type' => 'checkbox',
        '#title' => t('Test Mobile'),
        '#default_value' => variable_get('splashify_when_mobile_test', 0),
        '#description' => t('Turn this option on to force the mobile settings to take affect so you can test from your desktop browser.'),
      );
    }

    $form['mobile']['options']['end'] = array('#markup' => '</div>');
  }
  else {
    $form['mobile']['description'] = array('#markup' => t('In order to enable the mobile splash, you need to have the Mobile Detect library installed. <a href="@download" target="_blank">Download the library</a> and put the files into /sites/all/libraries/Mobile_Detect/ .', array('@download' => 'http://mobiledetect.net/')));
  }

  $form['#submit'][] = 'splashify_admin_when_form_submit';
  return system_settings_form($form);
}

/**
 * Implements form submit handler.
 */
function splashify_admin_when_form_submit($form, &$form_state) {
  if (isset($form_state['values']['splashify_when_mobile'])) {
    if ($form_state['values']['splashify_when_mobile'] != 1) {
      // Unset these values, if the mobile option is disabled.
      variable_set('splashify_when_mobile_frequency', 'never');
      variable_set('splashify_when_mobile_test', 0);
    }
  }

  if (isset($form_state['values']['splashify_when_anonymous'])) {
    if ($form_state['values']['splashify_when_anonymous'] == 1) {
      $form_state['values']['splashify_when_roles'] = FALSE;
    }
  }

  if ($form_state['values']['splashify_when_roles'] !== 1) {
    variable_set('splashify_when_roles_options', 0);
  }
}

/**
 * Ajax callback for the "enable mobile" checkbox.
 */
function splashify_ajax_when_mobile_callback($form, &$form_state) {
  return $form['mobile']['options'];
}

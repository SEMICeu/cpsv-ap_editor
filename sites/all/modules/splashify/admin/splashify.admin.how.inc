<?php

/**
 * @file
 * The admin "How" tab.
 *
 * Admin functionality that determines how the splash page shows up to the
 * user.
 */

/**
 * "How" settings tab.
 */
function splashify_admin_how_form($form, &$form_state) {
  $form = array();

  $form['description'] = array(
    '#markup' => '<p>' . t('How should the splash page come up?') . '</p>',
  );

  // Specify the display mode options for the splash page.
  $splashify_how_mode_options = array(
    'redirect' => t('Redirect'),
    'window' => t('Open in new window'),
  );

  if (module_exists('colorbox')) {
    $splashify_how_mode_options['lightbox'] = t('Open in a Lightbox (colorbox)');
  }
  else {
    $colorbox_warning = t('In order to access the lightbox option, you need to have the <a href="@colorbox" target="_blank">Colorbox</a> module installed.', array(
      '@colorbox' => 'http://drupal.org/project/colorbox',
    ));

    drupal_set_message($colorbox_warning, 'warning');
  }

  $form['desktop'] = array(
    '#type' => 'fieldset',
    '#title' => t('Desktop Settings'),
  );

  // Determines how the splash page shows up.
  $form['desktop']['splashify_how_desktop_mode'] = array(
    '#type' => 'select',
    '#title' => t('Splash Display Mode'),
    '#options' => $splashify_how_mode_options,
    '#default_value' => variable_get('splashify_how_desktop_mode', 'redirect'),
    '#description' => t('Determines how the splash page should show up. If you want to use the lightbox option, you need to have the Colorbox module installed.'),
    '#ajax' => array(
      'callback' => 'splashify_ajax_how_desktop_mode_callback',
      'wrapper' => 'how-mode-desktop-size',
      'method' => 'replace',
      'effect' => 'slide',
    ),
  );

  // Set a variable that is either defined by the selection from the ajax
  // dropdown menu, or a previously saved value.
  if (isset($form_state['values']['splashify_how_desktop_mode'])) {
    $how_desktop_mode_set = $form_state['values']['splashify_how_desktop_mode'];
  }
  else {
    $how_desktop_mode_set = variable_get('splashify_how_desktop_mode', '');
  }

  $form['desktop']['mode_value']['begin'] = array('#markup' => '<div id="how-mode-desktop-size">');

  // If they specified the redirect option, we want to hide the window size
  // text field.
  if ($how_desktop_mode_set != 'redirect') {
    $form['desktop']['mode_value']['splashify_how_desktop_size'] = array(
      '#type' => 'textfield',
      '#title' => t('Window/Box size'),
      '#default_value' => variable_get('splashify_how_desktop_size', ''),
      '#description' => t('Size (<code>WIDTHxHEIGHT</code>, e.g. 400x300) of the Window or Lightbox.'),
    );
  }

  $form['desktop']['mode_value']['end'] = array('#markup' => '</div>');

  $form['mobile'] = array(
    '#type' => 'fieldset',
    '#title' => t('Mobile Settings'),
  );

  // If the mobile splash is enabled, display the mobile options.
  if (variable_get('splashify_when_mobile', 0) == 1) {
    // Determines how the splash page shows up.
    $form['mobile']['splashify_how_mobile_mode'] = array(
      '#type' => 'select',
      '#title' => t('Splash Display Mode'),
      '#options' => $splashify_how_mode_options,
      '#default_value' => variable_get('splashify_how_mobile_mode', 'redirect'),
      '#description' => t('How should we load the splash page? Note: Redirect is currently the only option.'),

      // For the time being, the only option we are allowing for mobile
      // devices is "redirect". We display this so that it is obvious the
      // method that is used.
      '#disabled' => TRUE,
    );
  }
  else {
    $form['mobile']['splashify_how_mobile_mode'] = array(
      '#markup' => '<p>' . t('In order to specify mobile options, you need to enable the "When: Enable Unique Mobile Splash" option.') . '</p>',
    );
  }

  return system_settings_form($form);
}

/**
 * Ajax callback for the desktop mode dropdown.
 */
function splashify_ajax_how_desktop_mode_callback($form, &$form_state) {
  return $form['desktop']['mode_value'];
}

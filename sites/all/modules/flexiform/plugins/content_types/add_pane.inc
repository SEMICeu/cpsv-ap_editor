<?php

/**
 * @file
 * Plugin to handle attached entity content types
 */

/**
 * Plugins are described by creating a $plugin array which will be used
 * by the system that includes this file.
 */
$plugin = array(
  'title' => t('Flexiform Add Pane'),
  'defaults' => array(),
  'content type' => 'flexiform_add_pane_content_type_content_type',
);

/**
 * Get one sub type of this flexiform edit pane.
 */
function flexiform_add_pane_content_type_content_type($subtype) {
  $types = flexiform_add_pane_content_type_content_types();
  if (isset($types[$subtype])) {
    return $types[$subtype];
  }
}

/**
 * Return all flexiform edit pane sub types
 */
function flexiform_add_pane_content_type_content_types() {
  $types = &drupal_static(__FUNCTION__, array());
  if (!empty($types)) {
    return $types;
  }

  // This will hold all the individual field content types.
  $forms = entity_load('flexiform');

  foreach ($forms as $form) {
    if (!($display = $form->getDisplay('FlexiformDisplayAddPane')) || !$display->isEnabled()) {
      continue;
    }

    $types[$form->form] = array(
      'category' => t('Flexiforms'),
      'title' => t('Flexiform: @form_label (@form)', array(
        '@form_label' => $form->label,
        '@form' => $form->form,
      )),
      'description' => t('Flexiform form edit pane.'),
      'edit form' => 'flexiform_add_pane_content_type_options',
    );
  }

  return $types;
}

/**
 * Options form for the pane.
 */
function flexiform_add_pane_content_type_options($form, &$form_state) {
  return $form;
}

/**
 * Options form submit.
 */
function flexiform_add_pane_content_type_options_submit($form, &$form_state) { }

/**
 * Returns the administrative title for a type.
 */
function flexiform_add_pane_content_type_admin_title($subtype, $conf, $context) {
  $flexiform = entity_load_single('flexiform', $subtype);
  return t('Flexiform: @form', array('@form' => $flexiform->label));
}

/**
 * Render the custom content type.
 */
function flexiform_add_pane_content_type_render($subtype, $conf, $panel_args, $context) {
  $flexiform = entity_load_single('flexiform', $subtype);
  $content = $flexiform->getDisplay('FlexiformDisplayAddPane')->build();
  $title = $flexiform->getDisplay('FlexiformDisplayAddPane')->title();

  // Build the content type block.
  $block = new stdClass();
  $block->module  = 'flexiform';
  $block->title = $title;
  $block->content = $content;
  $block->delta   = $subtype;

  return $block;
}

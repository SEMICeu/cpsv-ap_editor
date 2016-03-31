<?php

/**
 * @file
 * Contains handler for tags filter on flexiform views.
 */

class flexiform_handler_filter_tags extends views_handler_filter_many_to_one {

  function has_extra_options() { return TRUE; }

  function get_value_options() { /* don't overwrite the value options */ }

  function option_definition() {
    $options = parent::option_definition();

    $options['type'] = array('default' => 'textfield');

    return $options;
  }

  function extra_options_form(&$form, &$form_state) {
    $form['type'] = array(
      '#type' => 'radios',
      '#title' => t('Selection type'),
      '#options' => array('select' => t('Dropdown'), 'textfield' => t('Autocomplete')),
      '#default_value' => $this->options['type'],
    );
  }

  function value_form(&$form, &$form_state) {
    if ($this->options['type'] == 'textfield') {
      $default = '';
      if ($this->value) {
        $default = drupal_implode_tags($this->value);
      }

      $form['value'] = array(
        '#title' => t('Select tags'),
        '#type' => 'textfield',
        '#default_value' => $default,
        '#autocomplete_path' => 'admin/structure/flexiforms/autocomplete_tags',
      );
    }
    else {
      $options = array();
      $query = db_select('flexiform_tags', 'ft');
      $query->distinct()->addField('ft', 'tag', 'tag');
      $result = $query->execute()->fetchCol();
      $options = drupal_map_assoc($result);

      $default_value = (array) $this->value;

      $form['value'] = array(
        '#type' => 'select',
        '#title' => t('Select tags'),
        '#multiple' => TRUE,
        '#options' => $options,
        '#size' => min(4, count($options)),
        '#default_value' => $default_value,
      );
    }
  }

  function value_validate($form, &$form_state) {
    // We only validate if they've chosen the text field style.
    if ($this->options['type'] != 'textfield') {
      return;
    }

    $values = drupal_explode_tags($form_state['values']['options']['value']);
    $form_state['values']['options']['value'] = $values;
  }

  function accept_exposed_input($input) {
    if (empty($this->options['exposed'])) {
      return TRUE;
    }

    // If view is an attachment and is inheriting exposed filters, then assume
    // exposed input has already been validated
    if (!empty($this->view->is_attachment) && $this->view->display_handler->uses_exposed()) {
      $this->validated_exposed_input = (array) $this->view->exposed_raw_input[$this->options['expose']['identifier']];
    }

    // If it's non-required and there's no value don't bother filtering.
    if (!$this->options['expose']['required'] && empty($this->validated_exposed_input)) {
      return FALSE;
    }

    $rc = parent::accept_exposed_input($input);
    if ($rc) {
      // If we have previously validated input, override.
      if (!$this->is_a_group() && isset($this->validated_exposed_input)) {
        $this->value = $this->validated_exposed_input;
      }
    }

    return $rc;
  }

  function exposed_validate(&$form, &$form_state) {
    if (empty($this->options['exposed'])) {
      return;
    }

    $identifier = $this->options['expose']['identifier'];
    if (empty($this->options['expose']['identifier'])) {
      return;
    }

    if ($this->options['type'] != 'textfield') {
      if ($form_state['values'][$identifier] != 'All')  {
        $this->validated_exposed_input = (array) $form_state['values'][$identifier];
      }
      return;
    }

    $values = drupal_explode_tags($form_state['values'][$identifier]);
    $this->validated_exposed_input = $values;
  }

  function value_submit($form, &$form_state) {
    // prevent array_filter from messing up our arrays in parent submit.
  }
}

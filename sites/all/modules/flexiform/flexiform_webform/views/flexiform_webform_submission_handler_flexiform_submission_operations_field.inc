<?php

/**
 * This field handler aggregates operations that can be done on a flexiform_webform_submission
 * under a single field providing a more flexible way to present them in a view
 */
class flexiform_webform_submission_handler_flexiform_webform_submission_operations_field extends views_handler_field {
  function construct() {
    parent::construct();

    $this->additional_fields['flexiform_webform_submission_id'] = 'flexiform_webform_submission_id';
  }

  function query() {
    $this->ensure_my_table();
    $this->add_additional_fields();
  }

  function render($values) {

    $links = menu_contextual_links('flexiform_webform_submission', 'admin/content/flexiform_webform_submissions/flexiform_webform_submission', array($this->get_value($values, 'flexiform_webform_submission_id')));
    
    if (!empty($links)) {
      return theme('links', array('links' => $links, 'attributes' => array('class' => array('links', 'inline', 'operations'))));
    }
  }
}

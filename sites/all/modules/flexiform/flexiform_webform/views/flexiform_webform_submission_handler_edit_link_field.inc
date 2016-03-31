<?php

/**
 * @file
 * Contains a Views field handler to take care of displaying edit links
 * as fields
 */


class flexiform_webform_submission_handler_edit_link_field extends flexiform_webform_submission_handler_link_field {
  function construct() {
    parent::construct();
    $this->additional_fields['type'] = 'type';
  }


  function render($values) {
    $type = $values->{$this->aliases['type']};
    
    //Creating a dummy flexiform_webform_submission to check access against
    $dummy_flexiform_webform_submission = (object) array('type' => $type);
    if (!flexiform_webform_submission_access('edit', $dummy_flexiform_webform_submission)) {
      return;
    }
    
    $text = !empty($this->options['text']) ? $this->options['text'] : t('edit');
    $flexiform_webform_submission_id = $values->{$this->aliases['flexiform_webform_submission_id']};
    
    return l($text, 'admin/content/flexiform_webform_submissions/flexiform_webform_submission/' . $flexiform_webform_submission_id . '/edit');
  }
}

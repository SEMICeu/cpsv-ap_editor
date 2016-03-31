<?php

/**
 * @file
 * Contains a Views field handler to take care of displaying deletes links
 * as fields
 */

class flexiform_handler_field_tags extends views_handler_field {

  function construct() {
    parent::construct();

    $this->additional_fields['id'] = 'id';
  }

  function query() {
    $this->ensure_my_table();
    $this->add_additional_fields();
  }

  function render($values) {
    $id = $values->{$this->aliases['id']};
    $flexiform = entity_load_single('flexiform', $id);

    return drupal_implode_tags($flexiform->tags);
  }
}

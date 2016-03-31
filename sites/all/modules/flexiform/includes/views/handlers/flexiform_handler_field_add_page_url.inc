<?php

/**
 * @file
 * Contains a Views field handler to take care of displaying the path to the
 * add page display.
 */


class flexiform_handler_field_add_page_url extends views_handler_field {

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

    if (empty($flexiform->displays['flexiform_create_entity_page']['enabled'])) {
      return '';
    }

    $path = $flexiform->displays['flexiform_create_entity_page']['path'];
    return $path;
  }
}

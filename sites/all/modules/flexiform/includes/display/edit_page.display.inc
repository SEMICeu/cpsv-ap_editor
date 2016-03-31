<?php
/**
 * @file
 * Define Display plugin for the add page.
 */

/**
 * Class for add page displays.
 */
class FlexiformDisplayEditPage extends FlexiformDisplayPageBase {

  /**
   * The position of the base entity id.
   *
   * @var int
   */
  private $base_id_position;

  /**
   * Retrieve the position of the base id in the menu path.
   *
   * @return int|FALSE
   */
  protected function getBaseIdPosition() {
    // Assume the first wildcard is the base id position.
    if (!isset($this->base_id_position)) {
      $path_bits = explode('/', $this->configuration['path']);
      $this->base_id_position = array_search('%', $path_bits);
    }

    return $this->base_id_position;
  }

  /**
   * {@inheritdoc}
   */
  public function hook_menu() {
    // If there is no base id in this path we cannot use it for an edit page.
    if (!$this->getBaseIdPosition()) {
      return array();
    }

    return parent::hook_menu();
  }

  /**
   * {@inheritdoc}
   */
  protected function getMenuArguments() {
    $args = parent::getMenuArguments();
    $args[2] = $this->getBaseIdPosition();
    return $args;
  }

  /**
   * {@inheritdoc}
   */
  public function getBaseEntity($context = array()) {
    $base_entity = entity_load_single($this->getFlexiform()->base_entity, $context['base_entity_id']);

    //@todo: Check the entity is the right bundle.
    return $base_entity;
  }

  /**
   * Get the configuration form.
   */
  public function configForm($form, &$form_state) {
    $form = parent::configForm($form, $form_state);

    $form['path']['#description'] = t('The path to this form. Must include a single \'%\' where the id of the base entity will go.');

    return $form;
  }
}

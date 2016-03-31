<?php
/**
 * @file
 * Define Display plugin for the add page.
 */

/**
 * Class for add page displays.
 */
class FlexiformDisplayUserCategory extends FlexiformDisplayBase {

  /**
   * Define the menu links
   */
  public function hook_menu_alter(&$items) {
    $path = 'user/%user_category/edit/' . $this->configuration['category'];
    $items[$path]['page callback'] = 'flexiform_page_wrapper';
    $items[$path]['page arguments'] = array($this->getFlexiform()->form, get_class($this), 1);
    $items[$path]['module'] = 'flexiform';
    $items[$path]['file'] = 'flexiform.pages.inc';
    $items[$path]['weight'] = isset($this->configuration['weight']) ? $this->configuration['weight'] : 0;
  }

  /**
   * Define the category for for this form.
   */
  public function hook_user_categories() {
    return array(array(
      'name' => check_plain($this->configuration['category']),
      'title' => $this->configuration['title'],
      'weight' => 0,
    ));
  }

  /**
   * {@inheritdoc}
   */
  public function getBaseEntity($context = array()) {
    //@todo: Check the entity is the right bundle.
    return $context['base_entity_id'];
  }

  /**
   * Get the configuration form.
   */
  public function configForm($form, &$form_state) {
    $form = parent::configForm($form, $form_state);

    $form['category'] = array(
      '#title' => t('Category'),
      '#type' => 'textfield',
      '#default_value' => isset($this->configuration['category']) ? $this->configuration['category'] : NULL,
      '#description' => t('The path component for this category.'),
    );

    $form['weight'] = array(
      '#title' => t('Weight'),
      '#type' => 'weight',
      '#description' => t('Heavier items will sink and lighter items will be positioned near the top.'),
      '#default_value' => isset($this->configuration['weight']) ? $this->configuration['weight'] : 0,
      '#delta' => 30,
    );

    return $form;
  }
}

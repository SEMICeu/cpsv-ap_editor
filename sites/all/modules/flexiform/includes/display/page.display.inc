<?php
/**
 * @file
 * Base definition for pages.
 */

/**
 * Base class for page displays.
 */
abstract class FlexiformDisplayPageBase extends FlexiformDisplayBase {

  /**
   * Define the menu links
   */
  public function hook_menu() {
    $items = array();

    $items[$this->configuration['path']] = array(
      'title callback' => 'flexiform_page_title',
      'title arguments' => $this->getMenuArguments(),
      'page callback' => 'flexiform_page_wrapper',
      'page arguments' => $this->getMenuArguments(),
      'access callback' => TRUE,
      'file' => 'flexiform.pages.inc',
      'weight' => isset($this->configuration['weight']) ? $this->configuration['weight'] : 0,
    );

    // Add the menu name if we have one.
    if (isset($this->configuration['menu_name'])) {
      $items[$this->configuration['path']]['menu_name'] = $this->configuration['menu_name'];
    }

    // Add the type if we have one.
    if (isset($this->configuration['type'])) {
      $items[$this->configuration['path']]['type'] = $this->configuration['type'];

      // @TODO: Deal properly with default local tasks.
    }

    // Add the access settings.
    if (!empty($this->configuration['access']['permission'])) {
      $items[$this->configuration['path']]['access callback'] = 'user_access';
      $items[$this->configuration['path']]['access arguments'] = array($this->configuration['access']['permission']);
    }

    return $items;
  }

  /**
   * Get the page/title arguments for hook_menu.
   *
   * @see FlexiformDisplayPageBase::hook_menu()
   */
  protected function getMenuArguments() {
    return array(
      $this->getFlexiform()->form,
      get_class($this),
      NULL,
    );
  }

  /**
   * {@inheritdoc}
   */
  public function getPath($base_entity_id = NULL) {
    // Get hold of all the info.
    $items = $this->hook_menu();
    $path = explode('/', reset(array_keys($items)));
    $item = reset($items);
    $args = func_get_args();

    // Deal with placeholders.
    foreach ($item['page arguments'] as $pos => $arg) {
      if (is_int($arg)) {
        $path[$arg] = array_shift($args);
      }
    }

    // Append the rest of the arguments.
    $path = array_merge($path, $args);

    // Re-combine and return.
    return implode('/', $path);
  }

  /**
   * Get the configuration form.
   */
  public function configForm($form, &$form_state) {
    $form = parent::configForm($form, $form_state);

    $form['path'] = array(
      '#title' => t('Path'),
      '#type' => 'textfield',
      '#default_value' => isset($this->configuration['path']) ? $this->configuration['path'] : NULL,
    );

    $form['type'] = array(
      '#title' => t('Menu type'),
      '#type' => 'select',
      '#description' => t('Select the type of menu item to use.'),
      '#options' => array(
        MENU_CALLBACK => t('No menu item'),
        MENU_NORMAL_ITEM => t('Normal menu item'),
        MENU_LOCAL_ACTION => t('Local action'),
        MENU_LOCAL_TASK => t('Tab'),
        MENU_DEFAULT_LOCAL_TASK => t('Default tab'),
      ),
      '#default_value' => isset($this->configuration['type']) ? $this->configuration['type'] : MENU_CALLBACK,
    );

    $form['menu_name'] = array(
      '#title' => t('Menu'),
      '#type' => 'select',
      '#description' => t('Select the menu the item should be put in.'),
      '#options' => menu_get_menus(),
      '#default_value' => isset($this->configuration['menu_name']) ? $this->configuration['menu_name'] : NULL,
      '#states' => array(
        'visible' => array(':input[name="displays[flexiform_create_entity_page][type]"]' => array('!value' => MENU_CALLBACK)),
      ),
    );

    $form['weight'] = array(
      '#title' => t('Weight'),
      '#type' => 'weight',
      '#description' => t('Heavier items will sink and lighter items will be positioned near the top.'),
      '#default_value' => isset($this->configuration['weight']) ? $this->configuration['weight'] : 0,
      '#delta' => 30,
    );

    $form['access'] = array(
      '#type' => 'fieldset',
      '#title' => t('Access'),
      '#tree' => TRUE,
      '#description' => t('Alter settings for who can access this page.'),
      '#collapsible' => TRUE,
      '#collapsed' => TRUE,
    );

    $form['access']['permission'] = array(
      '#type' => 'select',
      '#title' => t('Required Permission'),
      '#description' => t('Only users with the following permission will be able to access this form.'),
      '#options' => $this->getPermissionOptions(),
      '#empty_option' => t(' - None - '),
      '#default_value' => isset($this->configuration['access']['permission']) ? $this->configuration['access']['permission'] : '',
    );

    return $form;
  }

  /**
   * Get Permission Options.
   */
  protected function getPermissionOptions() {
    $options = &drupal_static('FlexiformDisplayPagePermissionOptions', array());

    if (!empty($options)) {
      return $options;
    }

    $module_info = system_get_info('module');
    $modules = array();
    foreach (module_implements('permission') as $module) {
      $modules[$module] = $module_info[$module]['name'];
      $permissions[$module] = module_invoke($module, 'permission');
    }

    foreach($permissions as $module => $perms) {
      foreach ($perms as $perm => $info) {
        $options[$modules[$module]][$perm] = strip_tags($info['title']);
      }
    }

    return $options;
  }

  /**
   * Get the title.
   */
  public function title($context = array()) {
    return $this->configuration['title'];
  }
}

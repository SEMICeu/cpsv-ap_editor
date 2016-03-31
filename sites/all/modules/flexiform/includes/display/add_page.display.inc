<?php
/**
 * @file
 * Define Display plugin for the add page.
 */

/**
 * Class for add page displays.
 */
class FlexiformDisplayCreatePage extends FlexiformDisplayPageBase {

  /**
   * Get the configuration form.
   */
  public function configForm($form, &$form_state) {
    $form = parent::configForm($form, $form_state);

    $form['path']['#description'] = t('The path to a new submission of this form.');

    return $form;
  }
}

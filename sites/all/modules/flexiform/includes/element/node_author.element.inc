<?php
/**
 * @file
 * Contains class for the Node author element.
 */

/**
 * Class to add the node author field to a form.
 */
class FlexiformElementNodeAuthor extends FlexiformElement {

  /**
   * Return the form element for this FlexiformElement.
   */
  public function form($form, &$form_state, $entity, $language = LANGUAGE_NONE) {
    $parents = $form['#parents'];
    $parents[] = 'author';
    global $user;

    // If the node has the UID set then we load that user as the default,
    // otherwise use the global user.
    if (isset($entity->uid)) {
      $account = user_load($entity->uid);
    }
    else if (!empty($this->settings['default_value']['use_current'])) {
      $account = $user;
    }

    // Work out the default value.
    $default = '';
    if (!empty($this->settings['default_value']['default_value'])) {
      $default = $this->settings['default_value']['default_value'];
    }
    if (!empty($this->settings['default_value']['use_tokens'])) {
      $default = $this->replaceCtoolsSubstitutions($default, $form['#flexiform_entities']);
    }

    $form[$this->element_namespace] = array(
      '#type' => 'textfield',
      '#parents' => $parents,
      '#title' => $this->label(),
      '#autocomplete_path' => 'user/autocomplete',
      '#default_value' => !empty($account->name) ? $account->name : $default,
      '#maxlength' => 60,
      '#description' => t('Leave blank for %anonymous.', array('%anonymous' => variable_get('anonymous', t('Anonymous')))),
    );

    $form = parent::form($form, $form_state, $entity);

    return $form;
  }

  /**
   * Validate the form element.
   */
  public function formValidate($form, &$form_state, $entity, $language = LANGUAGE_NONE) {
    $author = $this->formExtractValues($form, $form_state, $entity);
    $entity->name = $author;

    // Validate the "authored by" field.
    if (!empty($entity->name) && !($account = user_load_by_name($entity->name))) {
      // The use of empty() is mandatory in the context of usernames
      // as the empty string denotes the anonymous user. In case we
      // are dealing with an anonymous user we set the user ID to 0.
      form_set_error('name', t('The username %name does not exist.', array('%name' => $entity->name)));
    }
  }

  /**
   * Submit the form element.
   */
  public function formSubmit($form, &$form_state, $entity, $language = LANGUAGE_NONE) {
    $author = $this->formExtractValues($form, $form_state, $entity);
    $entity->name = $author;

    // A user might assign the node author by entering a user name in the node
    // form, which we then need to translate to a user ID.
    if (isset($entity->name)) {
      if ($account = user_load_by_name($entity->name)) {
        $entity->uid = $account->uid;
      }
      else {
        $entity->uid = 0;
      }
    }
  }

  /**
   * Extract the submitted values for this form element.
   */
  public function formExtractValues($form, &$form_state, $entity) {
    $parents = $form['#parents'];
    $parents[] = $this->getEntityNamespace();
    $parents[] = 'author';

    $author = drupal_array_get_nested_value($form_state['values'], $parents);
    return $author;
  }

  /**
   * {@inheritdoc}
   */
  public function configureForm($form, &$form_state, $flexiform) {
    $form = parent::configureForm($form, $form_state, $flexiform);

    $form['default_value'] = array(
      '#tree' => TRUE,
      '#type' => 'fieldset',
      '#title' => t('Default Value'),
      '#weight' => -5,
    );

    $form['default_value']['use_current'] = array(
      '#type' => 'checkbox',
      '#title' => t('Use the logged in user'),
      '#default_value' => !empty($this->settings['default_value']['use_current']),
    );

    $form['default_value']['default_value'] = array(
      '#type' => 'textfield',
      '#title' => $this->label() ? $this->label() : t('Default'),
      '#default_value' => !empty($this->settings['default_value']['default_value']) ? $this->settings['default_value']['default_value'] : '',
      '#maxlength' => 60,
    );

    $form['default_value']['use_tokens'] = array(
      '#type' => 'checkbox',
      '#title' => t('Use Tokens in Default Value'),
      '#default_value' => !empty($this->settings['default_value']['use_tokens']),
    );

    $form['default_value']['contexts'] = array(
      '#title' => t('Substitutions'),
      '#type' => 'fieldset',
      '#collapsible' => TRUE,
      '#collapsed' => TRUE,
    );

    $form['default_value']['contexts']['contexts'] = $this->getCtoolsSubstitutionsList();

    return $form;
  }

  /**
   * {@inheritdoc}
   */
  public function configureFormSubmit($form, &$form_state, $flexiform) {
    $this->settings['default_value']['default_value'] = $form_state['values']['default_value']['default_value'];
    $this->settings['default_value']['use_tokens'] = $form_state['values']['default_value']['use_tokens'];
    $this->settings['default_value']['use_current'] = $form_state['values']['default_value']['use_current'];

    parent::configureFormSubmit($form, $form_state, $flexiform);
  }
}

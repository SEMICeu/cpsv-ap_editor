<?php
/**
 * @file
 * API documentation for Flexiform.
 */

/**
 * Inform flexiform of a group of forms.
 *
 * Flexiforms are assigned to a group to allow additional logic to be performed
 * by other modules. For example, a module may define a group which it then
 * adds additional fields to that are relevant for that form group.
 *
 * @return
 *   An array whose keys are the value for the group and whose values are an
 *   an array with the following:
 *   - label: The human-readable name of the group.
 *   - locked: Set to TRUE to prevent forms being created in this group through
 *     the UI.
 */
function hook_flexiform_group_info() {
  return array(
    'application' => array(
      'label' => t('Application'),
      // We want site builders to use the UI for this group.
      'locked' => FALSE,
    ),
  );
}

/**
 * Alter a flexiform as it gets built.
 *
 * @param array $form
 *   The form array that has been built by the flexiform builder.
 * @param array $form_state
 *   The form_state of the form.
 * @param Flexiform $flexiform
 *   The flexiform object.
 *
 * @see FlexiformBuilder::invoke()
 * @see FlexiformBuilderFlexiform::form()
 */
function hook_flexiform_build_alter(&$form, &$form_state, $flexiform) {

}

/**
 * Alter a flexiform as it gets built by a particular builder.
 *
 * @param array $form
 *   The form array that has been built by the flexiform builder.
 * @param array $form_state
 *   The form_state of the form.
 * @param Flexiform $flexiform
 *   The flexiform object.
 *
 * @see FlexiformBuilder::invoke()
 * @see FlexiformBuilderFlexiform::form()
 * @see flexiform_get_builder_info()
 */
function hook_flexiform_build_FLEXIFORM_BUILDER_alter(&$form, &$form_state, $flexiform) {

}

/**
 * Act on the validation of a flexiform.
 *
 * @param array $form
 *   The form array that has been built by the flexiform builder.
 * @param array $form_state
 *   The form_state of the form.
 * @param Flexiform $flexiform
 *   The flexiform object.
 *
 * @see FlexiformBuilder::invoke()
 * @see FlexiformBuilderFlexiform::formValidate()
 */
function hook_flexiform_build_validate_alter(&$form, &$form_state, $flexiform) {

}

/**
 * Act on the validation of a flexiform built by a particular builder.
 *
 * @param array $form
 *   The form array that has been built by the flexiform builder.
 * @param array $form_state
 *   The form_state of the form.
 * @param Flexiform $flexiform
 *   The flexiform object.
 *
 * @see FlexiformBuilder::invoke()
 * @see FlexiformBuilderFlexiform::formValidate()
 */
function hook_flexiform_build_FLEXIFORM_BUILDER_validate_alter(&$form, &$form_state, $flexiform) {

}

/**
 * Act on the submission of a flexiform.
 *
 * @param array $form
 *   The form array that has been built by the flexiform builder.
 * @param array $form_state
 *   The form_state of the form.
 * @param Flexiform $flexiform
 *   The flexiform object.
 *
 * @see FlexiformBuilder::invoke()
 * @see FlexiformBuilderFlexiform::formSubmit()
 */
function hook_flexiform_build_submit_alter(&$form, &$form_state, $flexiform) {

}

/**
 * Act on the submission of a flexiform built by a particular builder.
 *
 * @param array $form
 *   The form array that has been built by the flexiform builder.
 * @param array $form_state
 *   The form_state of the form.
 * @param Flexiform $flexiform
 *   The flexiform object.
 *
 * @see FlexiformBuilder::invoke()
 * @see FlexiformBuilderFlexiform::formSubmit()
 */
function hook_flexiform_build_FLEXIFORM_BUILDER_submit_alter(&$form, &$form_state, $flexiform) {

}

/**
 * Register builder classes with the Flexiform system.
 *
 * Builders are used to turn the configuration stored in the flexiform entity
 * into a usable FAPI form.
 *
 * @return
 *   An array whose keys are unique builder machine-names and whose values are
 *   arrays of important information about the builders that must contain the
 *   following keys:
 *   - class: The name of the class used to build the form.
 *   - label: The human-readable name of the builder.
 *   - description: A description that will be used as help text on the
 *     flexiform config form.
 *   - entity_types: An array of entity types that this builder can build forms
 *     for. Defaults to all entity types if left blank.
 */
function hook_flexiform_builder_info() {
  $builders = array();

  $builders['FlexiformBuilderFlexiform'] = array(
    'class' => 'FlexiformBuilderFlexiform',
    'label' => t('Flexiform Form Builder'),
    'description' => t('The flexiform custom form builder. Use the configuration pages to add fields and entities to the form.'),
  );

  $fieldable_entities = array();
  foreach (entity_get_info() as $entity_type => $info) {
    if ($info['fieldable']) {
      $fieldable_entities[] = $entity_type;
    }
  }
  $builders['FlexiformBuilderEntityForm'] = array(
    'class' => 'FlexiformBuilderEntityForm',
    'label' => t('Entity Field Form'),
    'description' => t('Render the standard entity field form.'),
    'entity_types' => $fieldable_entities,
  );

  return $builders;
}

/**
 * Alter flexiform builder info.
 *
 * @param $builder_info
 *   The builder info array, keyed by builder machine-name.
 *
 * @see hook_flexiform_builder_info()
 */
function hook_flexiform_builder_info_alter(&$builder_info) {

}

/**
 * Define elements for use in flexiforms.
 *
 * Elements should be defined in a multi-dimensional array keyed by entity
 * type, bundle and name. The whole definition for an element will be passed to
 * the element constructor so it is possible to store other settings in this
 * array as necessary.
 *
 * @return
 *   A multidimensional array of elements keyed by entity_type, bundle and an
 *   element_name that must be unique for that entity-bundle. Each element
 *   definition should be an array with the following keys:
 *   - label: The human readable label for the element, this can usually be
 *     overridden once the element is in a form.
 *   - class: The Element class to use for the rendering of the element.
 *   - group: (optional) What group should the element appear in when being
 *     selected to be added to the form. Defaults to 'Other'.
 */
function hook_flexiform_element_info() {
  $elements = array();

  // Get the element for all nested flexiforms.
  $forms = db_select('flexiform', 'f')
    ->fields('f', array('label', 'form', 'base_entity', 'base_entity_bundle'))
    ->execute()
    ->fetchAllAssoc('form');

  foreach ($forms as $form) {
    $elements[$form->base_entity][$form->base_entity_bundle]['flexiform:' . $form->form] = array(
      'label' => $form->label,
      'class' => 'FlexiformElementFlexiform',
      'type' => 'form',
      'group' => 'Flexiform',
      'form' => $form->form,
    );
  }

  // Get all the field elements.
  $fields_info = field_info_instances();
  foreach ($fields_info as $entity_type => $entity_fields) {
    foreach ($entity_fields as $bundle => $bundle_fields) {
      foreach ($bundle_fields as $field_name => $instance) {
        $elements[$entity_type][$bundle]['field:' . $field_name] = array(
          'label' => $instance['label'],
          'class' => 'FlexiformElementField',
          'type' => 'field',
          'group' => 'Field',
          'field_name' => $field_name,
        );
      }
    }
  }

  return $elements;
}

/**
 * Alter flexiform element info.
 *
 * @param $element_info
 *   The element info array, keyed by element name.
 *
 * @see hook_flexiform_element_info()
 */
function hook_flexiform_element_info_alter(&$element_info) {

}

/**
 * Register entity getters/setters with the flexiform system.
 *
 * Entity getters are used by flexiform to load entities into the flexiform.
 *
 * @return
 *   An array of getter definitions keyed by a unique getter name. Each
 *   definition should have the following keys:
 *   - label (required): A human readable name for the getter
 *   - description: A Description of the getter
 *   - entity_types (required): An array of entity types this getter can return
 *   - file: Which file is the getter in
 *   - params: An array of parameters for the getter keyed by variable name
 *     with the following keys:
 *     - entity_type: What type of entity to expect
 *   - getter callback: the name of the function to call for the getter
 *    (defaults to flexiform_entity_getter_GETTER_NAME)
 *   - setter callback: the name of the function to call to save the entity
 */
function hook_flexiform_entity_getter_info() {
  $getters = array();

  // Base entity getter.
  $getters['base_entity'] = array(
    'label' => 'Base Entity',
    'description' => 'The Base Entity for this Flexiform',
    'entity_types' => array_keys(entity_get_info()),
    'file' => 'includes/flexiform.flexiform.inc',
  );

  // User Getters
  $getters['user_current_user'] = array(
    'label' => 'Current User',
    'description' => 'Load the current user into the Form',
    'entity_types' => array('user'),
    'file' => 'user.flexiform.inc',
  );

  // Profile2 Getters
  if (module_exists('profile2')) {
    $getters['profile2_profile_from_user'] = array(
      'label' => 'Profile2 from User',
      'description' => 'Load a Profile 2 Basede on a User',
      'params' => array(
        'user' => array(
          'entity_type' => 'user',
        ),
      ),
      'entity_types' => array('profile2'),
      'file' => 'profile2.flexiform.inc',
    );
  }

  return $getters;
}

/**
 * Alter flexiform entity getter info.
 *
 * @param $entity_getter_info
 *   The entity getter info array, keyed by entity getter name.
 *
 * @see hook_flexiform_entity_getter_info()
 */
function hook_flexiform_entity_getter_info_alter(&$entity_getter_info) {

}

/**
 * Prepare the base entity of a flexiform.
 *
 * @param $base_entity
 *   The base entity of the form, provided by the FlexiformDisplay.
 * @param Flexiform $flexiform
 *   The flexiform that is about to be built.
 * @param FlexiformDisplayBase $display
 *   The flexiform display handler that has provided the base entity.
 *
 * @see FlexiformDisplayBase::build()
 */
function hook_flexiform_prepare_base_entity($base_entity, Flexiform $flexiform, FlexiformDisplayBase $display) {
  global $user;

  // Default an entity reference field to the current user id.
  // This allows the form to use the entityreference getter to load this
  // user into the form.
  if ($flexiform->form == 'node_create_form') {
    $base_entity->field_author[LANGUAGE_NONE][0] = array(
      'target_id' => $user->uid,
    );
  }
}

/**
 * Alter the form wrapper for a Flexiform.
 *
 * @param callable $wrapper
 *   The form wrapper callback.
 * @param FlexiformDisplayInterface $display
 *   The display being built.
 * @param array $context
 *   The context in which the form is being built.
 */
function hook_flexiform_wrapper_alter($wrapper, FlexiformDisplayInterface $display, $context) {
  // Swap the wrapper to our custom one for a particular flexiform.
  if ($display->getFlexiform()->form == 'my_form') {
    $wrapper = 'my_wrapper';
  }
}

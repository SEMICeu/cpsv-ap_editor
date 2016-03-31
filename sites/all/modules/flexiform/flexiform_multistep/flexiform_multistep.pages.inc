<?php
/**
 * @file
 * Page callbacks for Flexiform Multistep.
 */

/**
 * Page callback for CTools Multistep Forms.
 *
 * @param $flexiform
 * @param string $display
 * @param mixed $base_entity_id
 * @param string $step
 *
 * @return array
 *   The page array.
 */
function flexiform_multistep_wrapper($flexiform, $base_entity, $current_step = NULL) {
  // Start setting up our ctools wizard multistep form info.
  ctools_include('wizard');

  $menu_item = menu_get_item();
  $settings = isset($flexiform->settings['flexiform_multistep']) ? $flexiform->settings['flexiform_multistep'] : array();

  $form_info = array(
    'id' => 'flexiform_multistep_' . $flexiform->form,
    'path' => $menu_item['href'] . '/%step',
    'next callback' => 'flexiform_multistep_step_next',
    'finish callback' => 'flexiform_multistep_step_finish',
  );
  if (isset($settings['form_info'])) {
    $form_info += $settings['form_info'];
  }

  // Add in our pages from the existing field groups.
  $groups = field_group_info_groups('flexiform', $flexiform->form, 'form');
  foreach ($groups as $group) {
    if ($group->format_type == 'flexiform_step') {
      // Skip groups with no children.
      if (!count($group->children)) {
        continue;
      }

      $step = substr($group->group_name, 6);
      $form_info['forms'][$step] = array(
        'form id' => 'flexiform_multistep_step_form',
        'title' => $group->label,
        'weight' => $group->weight,
        'group' => $group,
      );
    }
  }
  uasort($form_info['forms'], 'drupal_sort_weight');

  // Build our form state with our arguments.
  $form_state = array(
    'build_info' => array(
      'args' => array(
        $flexiform,
        $base_entity,
      ),
    ),
  );
  form_load_include($form_state, 'inc', 'flexiform_multistep', 'flexiform_multistep.pages');

  return ctools_wizard_multistep_form($form_info, $current_step, $form_state);
}

/**
 * Form constructor for a specific step of a multistep flexiform.
 */
function flexiform_multistep_step_form($form, &$form_state, $flexiform, $base_entity) {
  // Get hold of the full form.
  $form = drupal_retrieve_form('flexiform__' . $flexiform->form, $form_state);

  // Clear out the standard submission buttons.
  unset($form['actions']);

  // Get our current group.
  $form_state['step_group'] = $form_state['form_info']['forms'][$form_state['step']]['group'];

  // Set up the more advanced trail.
  if (!empty($form['ctools_trail'])) {
    unset($form['ctools_trail']['#markup']);
    $form['ctools_trail']['#theme'] = 'item_list';
    $form['ctools_trail']['#type'] = 'ol';
    $form['ctools_trail']['#attributes']['class'][] = 'flexiform-multistep-progress';
    $form['ctools_trail']['#attributes']['class'][] = 'clearfix';
    $form['ctools_trail']['#attributes']['class'][] = 'inline';

    $past_current = FALSE;
    foreach ($form_state['form_info']['forms'] as $step => $info) {
      $item = array(
        'class' => array($info['group']->group_name),
      );

      if (!$past_current) {
        $params = ctools_wizard_get_path($form_state['form_info'], $step);
        $params[] = array();
        $item['class'][] = 'available';
        $item['data'] = l($info['group']->label, $params[0], $params[1]);
      }
      else {
        $item['data'] = '<span>' . $info['group']->label . '</span>';
      }

      if ($step == $form_state['step']) {
        $item['class'][] = 'active';
        $past_current = TRUE;
      }

      $form['ctools_trail']['#items'][$step] = $item;
    }
  }

  // Clear out the multi step field groups.
  foreach ($form['#groups'] as $name => $group) {
    if ($group->format_type == 'flexiform_step') {
      unset($form['#groups'][$name]);
      unset($form['#fieldgroups'][$name]);
    }
  }

  // Set the page title if requested.
  if (!empty($form_state['step_group']->format_settings['instance_settings']['page_title'])) {
    drupal_set_title($form_state['step_group']->format_settings['instance_settings']['page_title']);
  }

  // Hide any elements that aren't in this step.
  $old_form = $form;
  foreach (element_children($form) as $key) {
    // Preserve special items.
    if (in_array($key, array('ctools_trail', 'buttons'))) {
      continue;
    }

    if (!in_array($key, $form_state['step_group']->children)) {
      unset($form[$key]);
    }
    else if(module_exists('flexiform_conditional_fields')) {
      $el_settings = $form[$key]['#flexiform_element']->getSettings();
      if (empty($el_settings['conditional_fields'])) {
        continue;
      }

      foreach ($el_settings['conditional_fields'] as $dependency_key => $options) {
        list($dependee) = explode('|', $dependency_key);
        if (in_array($dependee, $form_state['step_group']->children)) {
          // If the dependee is also on this page then we don't need to do
          // anything.
          continue;
        }
        $condition_met = FALSE;
        $dependee_element = $old_form[$dependee];
        if (!empty($dependee_element[$dependee_element['#language']])) {
          $dependee_element = $dependee_element[$dependee_element['#language']];
        }

        $values = array();
        if (element_children($dependee_element)) {
        }
        else {
          $values = $dependee_element['#default_value'];
          if (!is_array($values)) {
            $values = array(
              $dependee_element['#delta'] => $values,
            );
          }
          foreach ($values as $delta => $value) {
            if (!is_array($value)) {
              $values[$delta] = array(
                $dependee_element['#value_key'] => $value,
              );
            }
          }
        }

        switch ($options['condition']) {
          case '!empty':
            $condition_met = !empty($values);
            break;
          case 'empty':
            $condition_met = empty($values);
            break;
          case 'focused':
            break;
          case '!focused':
            $condition_met = TRUE;
            break;
          case 'checked':
            $condition_met = ($dependee_element['#type'] == 'checkbox' && $dependee_element['#default_value'] == 1);
            break;
          case '!checked':
            $condition_met = ($dependee_element['#type'] == 'checkbox' && !$dependee_element['#default_value'] == 1);
            break;
          case 'value':
            if ($values == $options['value']) {
              $condition_met = TRUE;
            }
            break;
        }

        if ($condition_met) {
          switch ($options['state']) {
            case 'visible':
              break;
            case '!visible':
              $form[$key]['#access'] = FALSE;
              break;
            case '!disabled':
              $form[$key]['#disabled'] = FALSE;
              break;
            case 'disabled':
              $form[$key]['#disabled'] = TRUE;
              break;
            case '!required':
              $form[$key]['#required'] = TRUE;
              break;
            case 'required':
              $form[$key]['#required'] = FALSE;
              break;
          }
        }
        else {
          switch ($options['state']) {
            case 'visible':
              $form[$key]['#access'] = FALSE;
              break;
            case '!visible':
              break;
            case '!disabled':
              $form[$key]['#disabled'] = TRUE;
              break;
            case 'disabled':
              $form[$key]['#disabled'] = FALSE;
              break;
            case '!required':
              $form[$key]['#required'] = TRUE;
              break;
            case 'required':
              $form[$key]['#required'] = FALSE;
              break;
          }
        }
      }
    }
  }

  return $form;
}

/**
 * Form submission handler for flexiform_multistep_step_form().
 */
function flexiform_multistep_step_next(&$form_state) {
  $form = $form_state['complete form'];

  /** @var Flexiform $flexiform */
  $flexiform = $form['#flexiform'];

  /** @var FlexiformBuilderInterface $builder */
  $builder = $form['#flexiform_builder'];
  $form_settings = isset($flexiform->settings['flexiform_multistep']) ? $flexiform->settings['flexiform_multistep'] : array();
  $step_settings = isset($form_state['step_group']->format_settings['instance_settings']) ? $form_state['step_group']->format_settings['instance_settings'] : array();

  // Inherit settings from the form if we're supposed to.
  if (!isset($step_settings['save_entities'])) {
    $step_settings['save_entities'] = !empty($form_settings['save_entities']) ? 'y' : 'n';
  }

  // Tweak our flexiform state so we can skip saving particular items.
  $flexiform_state = &$builder->getFlexiformState($form, $form_state);
  $entity_manager = $builder->getEntityManager($flexiform_state);
  $entities = array_keys($flexiform->entities);
  $save_entities = array();

  // Save the custom step of entities.
  if ($step_settings['save_entities'] == 'c') {
    $save_entities = array_filter($step_settings['save_entities_custom']);
  }
  // If this is the last step, switch to 'y' mode.
  elseif ($step_settings['save_entities'] == 'n' && !empty($form_state['last_step'])) {
    $step_settings['save_entities'] = 'y';
  }

  // If we're not on the last step, only save entities we've exposed.
  if ($step_settings['save_entities'] == 'y' && empty($form_state['last_step'])) {
    foreach (element_children($form) as $element) {
      if (!empty($form[$element]['#flexiform_element'])) {
        $namespace = $form[$element]['#flexiform_element']->getEntityNamespace();
        $save_entities[$namespace] = $namespace;
      }
    }
  }

  // If not saving everything, remove items we don't want.
  foreach (array_diff($entities, $save_entities) as $namespace) {
    $entity_manager->skipOnSave($namespace, TRUE);
  }

  $builder->formSubmit($form, $form_state);
}

/**
 * Form submission handler for flexiform_multistep_step_form().
 */
function flexiform_multistep_step_finish(&$form_state) {
  // Pass onto the normal step submission.
  $form_state['last_step'] = TRUE;
  flexiform_multistep_step_next($form_state);

  // Deal with the completion redirect.
  $form_settings = isset($form_state['complete form']['#flexiform']->settings['flexiform_multistep']) ? $form_state['complete form']['#flexiform']->settings['flexiform_multistep'] : array();

  if (!empty($form_settings['redirect'])) {
    $builder = $form_state['complete form']['#flexiform_builder'];
    $flexiform_state = &$builder->getFlexiformState($form_state['complete form'], $form_state);
    $entity_manager = $builder->getEntityManager($flexiform_state);
    $replacements = array();
    foreach ($entity_manager->getEntities() AS $namespace => $entity) {
      $wrapper = entity_metadata_wrapper($entity_manager->getEntityType($namespace), $entity);
      $replacements['%' . $namespace] = $wrapper->getIdentifier();
    }
    $form_state['redirect'] = strtr($form_settings['redirect'], $replacements);
  }
}

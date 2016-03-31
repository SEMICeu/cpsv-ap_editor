<?php
/**
 * @file
 * taxo_faceted_navigation.admin.inc
 * Admin section of the taxo faceted navigation module
 *
 * Set various options for administering this module
 */

/**
 * Provides admin page.
 *
 * @param array $form
 *   admin form
 * @param array $form_state
 *   admin form
 *
 * @return array
 *   returns admin form
 */
function taxo_faceted_navigation_admin_settings($form, &$form_state) {
  $form['taxo_faceted_navigation_first_argument'] = array(
    '#type' => 'textfield',
    '#title' => t('First argument'),
    '#size' => 60,
    '#maxlength' => 250,
    '#default_value' => variable_get('taxo_faceted_navigation_first_argument', 'items_list'),
    '#required' => TRUE,
    '#description' => t('First argument in the url, it is necessary for the working of this module. For example, if this is an ecommerce site the argument could be products, so URL would look something like http://yoursite.com/products/hardware/monitors. Can not be null. Be careful not to use name produced by token used to generate clean url.s'),
  );
  $form['taxo_faceted_navigation_display_link_if_empty'] = array(
    '#type' => 'checkbox',
    '#title' => t('Do not display link if empty'),
    '#default_value' => variable_get('taxo_faceted_navigation_display_link_if_empty', FALSE),
    '#description' => t('If there are no nodes under particular link, do not display it.'),
  );
  $form['taxo_faceted_navigation_display_link_if_intersection_empty'] = array(
    '#type' => 'checkbox',
    '#title' => t('Do not display link if the intersection of terms is empty'),
    '#default_value' => variable_get('taxo_faceted_navigation_display_link_if_intersection_empty', FALSE),
    '#description' => t('Do not display link if there are no nodes under particular link, but also taking into account current filter selection. I.e if there are no nodes for current filter combination. Works well only for flat hierarchies, i.e single level hierarhy'),
  );
  $form['taxo_faceted_navigation_redirect_to_home'] = array(
    '#type' => 'checkbox',
    '#title' => t('Redirect to Home page if no filters applied'),
    '#default_value' => variable_get('taxo_faceted_navigation_redirect_to_home', FALSE),
    '#description' => t('If no arguments passed as filters, i.e if all filters deselected, redirect to home page if this checkbox checked. If unchecked all nodes will appear, this is similar behaviour as on sitename.com/node page.'),
  );
  
  $form['taxo_faceted_navigation_redirect_to_page'] = array(
    '#type' => 'textfield',
    '#title' => t('Redirect to a page if no filters applied'),
    '#default_value' => variable_get('taxo_faceted_navigation_redirect_to_gae', FALSE),
    '#description' => t('If no arguments passed as filters, i.e if all filters deselected, redirect to a specified page. NOTE: Make sure you check above check box "Redirect to Home page..." end specify the page url in this field, if you want to redirect to home just leave this field empty.'),
  );


  //********************************* Display text about configuring taxonomy paterns ******************************
      $form['set_taxo_patterns'] = array(
          '#type' => 'fieldset',
          '#title' => t('Set taxonomy patterns'),
          '#collapsible' => TRUE,
          '#collapsed' => FALSE,
      );

      $form['set_taxo_patterns']['set_taxo_patterns_text'] = array(

        '#markup' => ' Go to:  <br>
        Administration > Configuration > Search and metadata > URL aliases > <a href="/admin/config/search/path/patterns">Patterns</a>
        <br>
        In the TAXONOMY TERM PATHS section configure term patterns, change it to
        single argument. <br>For example change default pattern:
        [term:vocabulary]/[term:name] to [term:name].<br>
        You can use [term:vocabulary]-[term:name] or similar, the key is to NOT have
        any / character in the pattern, as this module expects a single argument for
              each filter.<br><br>
              If you already have taxonomy term aliases generated with a different pattern than  update them:<br>
        * <a href="/admin/config/search/path/delete_bulk"> Delete aliases for taxonomy terms.</a><br>
        * <a href="/admin/config/search/path/update_bulk">Bulk update taxonomy term paths</a><br>.
        ',

      );


  // **************************** select content type **********************************************

  $form['content_types'] = array(
    '#type' => 'fieldset',
    '#title' => t('Select content types'),
    '#collapsible' => TRUE,
    '#collapsed' => FALSE,
  );

  $form['content_types']['taxo_faceted_navigation_content_type_options'] = array(
    '#type' => 'checkboxes',
    '#options' => taxo_faceted_get_node_types(),
    '#default_value' => variable_get('taxo_faceted_navigation_content_type_options', array()),
    '#title' => t('What content types should be displayed on taxonomy facets listing page,
    i.e the page that you specified in the "First argument" field above?'),
  );

  //********************************* Display text about adding taxo facets block  ******************************
  $form['set_taxo_block'] = array(
      '#type' => 'fieldset',
      '#title' => t('Add taxonomy faceted navigation blocks'),
      '#collapsible' => TRUE,
      '#collapsed' => FALSE,
  );

  $form['set_taxo_block']['set_taxo_block_text'] = array(

      '#markup' => ' Go to:  <br>
        Administration > Structure > <a href="/admin/structure/block">Blocks</a>
        <br>
        Click on <a href="/admin/structure/block/add-taxofacet-block">"Add taxofacet block"</a>
          <br>Select desired taxonomy.
         <br>In the visibility setngs set desired options, for example if your "Firs argument" is "items_list", than
         in the  "Show block on specific pages"
    <br>
        Click on "Only the listed pages" radio
        <br>
        and set it to: items_list*
         <br>.
        ',

  );

  //********************************* Display text about theming  ******************************
  $form['set_taxo_theme'] = array(
      '#type' => 'fieldset',
      '#title' => t('Theme templates'),
      '#collapsible' => TRUE,
      '#collapsed' => FALSE,
  );

  $form['set_taxo_theme']['set_taxo_theme_text'] = array(

      '#markup' =>  'There a 3 templates that you can use to theme filter blocks. Simlpy copy files from the templates folder inside this module in your themes templte folder.
        ',

  );


  return system_settings_form($form);
}

/**
 * Validate the configuration form.
 */
function taxo_faceted_navigation_admin_settings_validate($form, &$form_state) {
  // Get firs argument as entered by user.
  $first_arg = $form_state['values']['taxo_faceted_navigation_first_argument'];
  // Query menu router table and see if this argument has been already used.
  $duplicate_menu_item = taxo_faceted_navigation_firstarg_exsists_in_menu_router($first_arg);
  if ($duplicate_menu_item) {
    form_set_error('taxo_faceted_navigation_first_argument', t('The keyword') . ' ' . $first_arg .  ' ' . t('can not be used as first argument. It has already been used in the first elementh of fallowing menu path(s)') . ':<br />' . $duplicate_menu_item . 
       '</br>' . t('Please either rename this item or go to clean urls page end rename exsisting item, or examine router page to see which item is cousing the problem.'));
  }
  else {
    global $base_url;
    $link = l(t('this page to clear the cache'), check_plain($base_url) . '/admin/config/development/performance');
    drupal_set_message(t('Value saved, please clear all caches now or you may get unexpected results. Go to: !link', array('!link' => $link)), 'warning');
  }
}

/**
 * Check the menu_router, url_alias table and see if the keyword exsits.
 */
function taxo_faceted_navigation_firstarg_exsists_in_menu_router($first_arg) {
  $duplicate_path = NULL;

  // Check the menu_router table.
  $query = db_select('menu_router', 'mr');
  $or = db_or()->condition('mr.path', $first_arg . '/%', 'LIKE')->condition('mr.path', $first_arg);
  $query
    ->condition($or)
    ->fields('mr', array('path','page_callback'))
    ->range(0, 50);
  $result = $query->execute();

  foreach ($result as $row) {
    // if path produced by this module ignore

    if($row->page_callback != 'taxo_faceted_navigation_print_landing_page'){
          $duplicate_path .= $row->path . '<br />';
    }
  }

  // Check url_alias table, alias field.
  $query = db_select('url_alias', 'ua');
  $or = db_or()->condition('ua.alias', $first_arg . '/%', 'LIKE')->condition('ua.alias', $first_arg);
  $query
    ->condition($or)
    ->fields('ua', array('alias'))
    ->range(0, 50);
  $result = $query->execute();

  foreach ($result as $row) {
    $duplicate_path .= $row->alias . '<br />';
  }

  // Check url_alias table, source field.
  $query = db_select('url_alias', 'ua');
  $or = db_or()->condition('ua.source', $first_arg . '/%', 'LIKE')->condition('ua.source', $first_arg);
  $query
    ->condition($or)
    ->fields('ua', array('source'))
    ->range(0, 50);
  $result = $query->execute();

  foreach ($result as $row) {
    $duplicate_path .= $row->source . '<br />';
  }
  return $duplicate_path;
}

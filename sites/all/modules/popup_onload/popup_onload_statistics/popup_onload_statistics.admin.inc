<?php

/**
 * @file
 * Popup stats administrative callbacks.
 */

/**
 * Callback for the admin report table.
 */
function popup_onload_statistics_admin_form($form, &$form_state) {
  $form = array();

  $get_query = drupal_get_query_parameters();

  if (isset($get_query['date_from']['day']) && isset($get_query['date_from']['month']) && isset($get_query['date_from']['year'])) {
    $date_from = $get_query['date_from'];
  }
  else {
    $date_from = NULL;
  }

  if (isset($get_query['date_to']['day']) && isset($get_query['date_to']['month']) && isset($get_query['date_to']['year'])) {
    $date_to = $get_query['date_to'];
  }
  else {
    $date_to = NULL;
  }

  $form['date_from'] = array(
    '#type' => 'date',
    '#title' => t('Start date'),
    '#default_value' => $date_from,
  );
  $form['date_to'] = array(
    '#type' => 'date',
    '#title' => t('End date'),
    '#default_value' => $date_to,
  );
  $form['submit'] = array(
    '#type' => 'submit',
    '#value' => t('Filter statistics'),
  );

  if ($date_from) {
    $date_from = strtotime($date_from['year'] . '-' . $date_from['month'] . '-' . $date_from['day']);
  }
  if ($date_to) {
    $date_to = strtotime($date_to['year'] . '-' . $date_to['month'] . '-' . $date_to['day']);
  }

  $stats = popup_onload_statistics_get_stats($date_from, $date_to);
  $stats_table = theme('popup_onload_statistics_table', array('stats' => $stats));

  $form['stats_table'] = array(
    '#markup' => $stats_table,
  );

  $form['#method'] = 'get';

  return $form;
}

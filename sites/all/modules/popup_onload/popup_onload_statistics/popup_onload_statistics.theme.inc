<?php

/**
 * @file
 * Popup stats theme file.
 */

/**
 * Theme popup statistics table.
 */
function theme_popup_onload_statistics_table($vars) {
  $output = '';

  $stats_table = array(
    'header' => array(
      t('Popup'),
      t('Impressions'),
      t('Clicks'),
      t('CTR (Click-Through Rate)'),
    ),
    'rows' => array(),
  );

  foreach ($vars['stats'] as $row) {
    if (!isset($row['view'])) {
      $row['view'] = 0;
    }
    if (!isset($row['click'])) {
      $row['click'] = 0;
    }
    $rate = $row['view'] != 0 ?
      $row['click'] / $row['view'] :
      0;
    $stats_table['rows'][] = array(
      l($row['name'], 'admin/structure/popup_onload/manage/' . $row['popup_id'], array('attributes' => array('target' => '_blank'))),
      isset($row['view']) ? $row['view'] : 0,
      isset($row['click']) ? $row['click'] : 0,
      sprintf('%.2f%%', $rate * 100),
    );
  }

  $output .= theme('table', $stats_table);

  return $output;
}

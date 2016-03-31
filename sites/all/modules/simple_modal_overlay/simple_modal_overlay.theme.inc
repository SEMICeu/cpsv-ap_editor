<?php
/**
 * @file
 *  Theme hooks for the "Simple Modal Overlay" module.
 *
 *  © 2014-2015 RedBottle Design, LLC and Inveniem. All rights reserved.
 *
 * @author Guy Paddock (guy.paddock@redbottledesign.com)
 */

/**
 * Default theme implementation for simple modal overlays.
 *
 * @param array $variables
 *   An associative array containing:
 *     - title: The title text that should appear at the top of the overlay.
 *     - content: The content of the overlay. This can be either HTML markup or
 *       a render array.
 *     - dismissible: Whether or not the user can dismiss the overlay by
 *       clicking a close link in the overlay. The default is TRUE.
 *
 * @return string
 *   The HTML representation of the overlay.
 */
function theme_simple_modal_overlay(array $variables) {
  $title        = $variables['title'];
  $content      = $variables['content'];
  $dismissible  = $variables['dismissible'];

  if (is_array($content)) {
    $content = drupal_render($content);
  }

  $output = '<div class="message-inner">';

  if (!empty($title)) {
    $output .= '<div class="title clearfix">' . $title . '</div>';
  }

  $output .= '<div class="content">' . $content . '</div>';

  if ($dismissible) {
    $close_text = t('Close');

    $output .= '<a class="simple-overlay-close" href="#">';
    $output .= '  <span class="element-invisible">' . $close_text . '</span>';
    $output .= '</a>';
  }

  $output .= '</div>';

  return $output;
}
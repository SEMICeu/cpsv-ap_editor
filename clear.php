<?php
// Define static var.
define('DRUPAL_ROOT', getcwd());
// Include bootstrap.
include_once('./includes/bootstrap.inc');
// Initialize stuff.
drupal_bootstrap(DRUPAL_BOOTSTRAP_FULL);
// Clear cache.
drupal_flush_all_caches();
?>

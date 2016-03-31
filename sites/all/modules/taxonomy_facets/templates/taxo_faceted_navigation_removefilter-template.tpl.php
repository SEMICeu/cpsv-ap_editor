<?php
/**
 * @file
 * Template "remove filter" in the filters block
 *
 * Theme your own "remove filter" sighn here.
 */

?>
 <h3><?php print t('Filter applied') . ':' ?><br /> <strong><?php print t($taxo_menu_item['term name']); ?></strong>
<a <?php print $taxo_menu_item['active'] ?> title ="<?php print t('Remove filter') . ': ' . t($taxo_menu_item['term name']); ?>" href="<?php print $taxo_menu_item['url alias'] ?>">
[X]</a>
</h3>

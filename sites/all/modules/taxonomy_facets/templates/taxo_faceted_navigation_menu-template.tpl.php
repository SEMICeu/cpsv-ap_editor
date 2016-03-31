<?php
/**
 * @file
 * Template for menu items in the filters block
 *
 * This themes one menu item at the time.
 */

 ?>
<li class="<?php print $taxo_menu_item['menu item class'] ?>">
<a <?php print $taxo_menu_item['active'] ?> title ="<?php  print $taxo_menu_item['term name']; ?>" href="<?php print $taxo_menu_item['url alias'] ?>">
<?php print $taxo_menu_item['term name'];?></a>
</li>

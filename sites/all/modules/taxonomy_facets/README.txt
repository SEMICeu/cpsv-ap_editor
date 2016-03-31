TAXONOMY FACETS
--------------------


TAXO FACETED NAVIGATION module - Faceted search with clean url's.

CONTENT:

* INTRO
* INSTALLATION
* CONFIGURATION
* HOW TO USE


IMPORTANT: this module will not work with default URL alias patterns for 
Taxonomies, see "Configure URL alias patterns" section below

==========================================================================
                                         INTRO
==========================================================================

Progresive content filtering, with clean url's, using taxonomies as facets.

Ideal for eCommerce carts, real estate / properyies sites, classified ads, or other sites with multiple categories, where users need to filter content progressively by applying filters from one or more categories.

Category Landing Pages

Taxonomy terms are used as Facets to help users filter content progressively. Similar to the way it is done on most of today’s eCommerce sites, like Amazon. This module is all about SEO, so as the user applies different filters the clean URL's are preserved with each filter change. For example, if the user applies these three filters: Computer Monitors, Samsung and LCD, then the URL will look something like:

http://sitename.com/products/computer-monitors/samsung/lcd

When the user then changes the filters to Computer Monitors, HP and LCD, the URL will change to:

http://sitename.com/products/computer-monitors/HP/lcd

Filters can be applied in various permutations, so producing a clean URL for each unique filter combination will allow search engines to index a huge amount of landing pages.

Node pages

When a user arrives on the “node body page”, like the product page, the full URL path will be preserved.

http://sitename.com/products/computer-monitors/HP/lcd/Monitor-123-AB

If a user finds the same product (node) with different filter permutations this will be reflected in the URL:

http://sitename.com/products/multimedia/displays/HP/Monitor-123-AB

Futhermore, if a user arrives on the product page via a direct URL, say directly from a Google search, the filters from the URL will be applied automatically and menu items expanded and highlighted accordingly.

For example, if a user arrives on the site via this URL:

http://sitename.com/products/computer-monitors/HP/lcd/Monitor-123-AB

The menu tree / facets will look something like

Computer hardware
--Monitors (highlighted)
--Peripherals
--Hard discs
--Workstations

Brand
--Sony
--HP (highlighted)
--Dell
--Apple

TYPE
--LCD (highlighted)
--Plasma

But if they arrive on the same product page via a different URL, say:

http://sitename.com/products/multimedia/displays/HP/Monitor-123-AB

The menu tree / facets will now look something like

Stores
--Multimedia (highlighted)
--Books
--House appliance

Product type
--displays (highlighted)
--gaming
--handheld

Brand
--Sony
--HP (highlighted)
--Dell
--Apple

This module produces "menus" on the fly, i.e. no need for rebuilding menus or indexing as the menu items are not Drupal menu items but just items in the block. This is useful for sites where taxonomies may change frequently.

Filters / facets blocks are cached for better performance.


==========================================================================
                                INSTALLATION
==========================================================================

1) Place this module directory in your "modules" folder (this will usually be
   "sites/all/modules/"). Don't install your module in Drupal core's "modules"
   folder, since that will cause problems and is bad practice in general. If
   "sites/all/modules" doesn't exist yet, just create it.

2) Enable the Taxonomy Faceted navigation module in Drupal at:
   administration -> site configuration -> modules (admin/build/modules)
   The Drupal core taxonomy module is required.
   
 3) IMPORTANT: this module will not work with default URL alias patterns for 
 Taxonomies, see "Configure URL alias patterns" section below


==========================================================================
                            CONFIGURATION
==========================================================================

Set Permissions
------------------
Go to Permissions page: admin/people/permissions
Find permision: "View taxonomy facets" and select for appropriate 
roles, i.e select ANONYMOUS USER role.

* Expose taxonomy as Faceted Filters / Navigation blocks
------------------------
Go to Blocks admin page:
Administration » Structure » Blocks
and create Taxo Faceterd block by clicking on "Add taxofaceted block" link.
Select desired taxonomy in the Taxonomy drop down field.

You can oweride the default block title (which is Taxonomy name) by
filing the title filed of the block.

Save block.	

The block will appear in the "Disabled" section on the blocks page.
and the blocks name will be prefixed by "Taxo Faceted Filter:", say
"Taxo Faceted Filter: Monitors".

Select region you want your block to apper in, i.e "Sidebar first".

Save blocks.

* Set first argument
--------------------
Go to admin section: 
Configuration >> SEARCH AND METADATA >> Taxo Faceted Navigation 
( admin/config/search/tax_faceted_nav )
Setting up of the first argument in the url is necessary for the working of 
this module. 
For example, if this is an ecommerce site the argument could be store, so URL 
would look something like http://yoursite.com/store/hardware/monitors. 
Can not be null. 

* !!! IMPORTANT: Configure URL alias patterns (if Pathauto module is installed)
----------------------------------------
Go to: 
Administration > Configuration > Search and metadata > URL aliases >Patterns
In the TAXONOMY TERM PATHS section configure term patterns, change it to 
single argument, for example change default pattern: 
[term:vocabulary]/[term:name] to [term:name]. 
You can use [term:vocabulary]-[term:name] or similar, the key is to NOT have 
any / character in the pattern, as this module expects a single argument for 
each filter.

If you already have taxonomy term aliases generated with a different pattern update them:
* Delete aliases for taxonomy terms.
* Bulk update taxonomy term paths.

* Configure other options
--------------------------
Go to admin section: 
Configuration >> SEARCH AND METADATA >> Taxo Faceted Navigation 
( admin/config/search/tax_faceted_nav )
Configure as desired


==========================================================================
                              HOW TO USE
==========================================================================

If you do not have any taxonomies in your system create some. Add them them as 
taxonmy facets block, by going to blocks page 
Administration » Structure » Blocks and click on +Add taxofacet block

For example create a vocabulary Catalog, 
create some terms in this vocabulary, for example:

Catalog
   Monitors
     LCD
     TFT
   Laptops
   Workstations
 
 Create another 2 vocabularies
 
 Brand
   Sony
   HP
   Acer
   
 Price range
  less than $100
  $100 - $200
  $200 - $500
  $500 - $1000
  $1000 +
  
Set URL aliases. Either install Pathauto module and generate URL aliases or set
 aliases manually. 
This module will not work without URL aliases
   
Add some nodes in to your CMS and tag them with appropriate taxonomies: Lets 
say you have Product content type. Add 3 fields to the product content type. 
Field type is Term Reference. 
Add fields Catalog, Brand and Price range, of type Term Reference, and select 
appropriate Vocabularies respectively.

Create some Products and associate with appropriate terms. For Example add 
Product node Sony
LCD monitor that costs $400 and associate it with:
 Product-LCD
 Brand-Sony
 Price range - ($200 - $500)
 

EasyRdf module

This module provides Libraries API compatibility for EasyRdf, a PHP library
for accessing and working with RDF resources.

This module provides no end-user functionality beyond linking EasyRdf via the
Libraries API, and as such should only be installed if requested by another
module.

EasyRdf is by Nicholas J Humfrey, and is available for download at
http://www.aelius.com/njh/easyrdf/.

INSTALLATION
------------

1) Download this module as normal, and http://drupal.org/project/libraries.
2) Download the latest version of EasyRDF from http://www.aelius.com/njh/easyrdf/,
   and place it in your sites/all/libraries folder, so that the path to the
   EasyRdf class loader is:

     sites/all/modules/easyrdf/lib/EasyRdf.php

   You may need to rename the folder extracted from the download by removing
   the version number, or similar.
3) If the "Help" module is enabled, you can check EasyRdf is working by visiting
   its help page.

DEVELOPER USAGE
---------------

The EasyRdf library can be accessed simply by calling the easyrdf() function in
your code. This function returns TRUE if the library is available, so the
following is effective:

<?php

if (easyrdf()) {
  // Do something with EasyRdf... e.g.
  $graph = new EasyRdf_Graph("http://www.bbc.co.uk/music/artists/70248960-cb53-4ea4-943a-edb18f7d336f.rdf");
}

?>
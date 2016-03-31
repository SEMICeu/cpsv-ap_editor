Introduction
------------

The Flexiform module provides a UI and configuration storage for the creation
of complex forms including fields and elements from multiple entities.

Requirements
------------

Drupal 7.x
Dependencies:
 - Entity API
 - Views

Installation
------------

Install and enable the Flexiform module as usual.

Getting Started
---------------

To start building your forms visit admin/structure/flexiforms and click
'Add Flexible Form'.

Creating a Flexiform
--------------------

1. Visit admin/structure/flexiforms/add and enter the Title for your form.
2. Select a base entity type and base entity bundle using the two drop downs
   provided. The base entity is the main entity the form deals with, other
   entities can be added to the form later.
3. Select a builder to use to build the form. Flexiform ships with 2 builders
   as standard:
   - Flexiform Form Builder: This is the main component of flexiform. It
     allows you to add more entities and custom elements to your form.
   - Entity Field Form: This builder will render a form containing all of the
     Field API fields on the base entity and bundle. Adding entities and
     elements will have no effect when using this builder!
4. Enter the Path to the form. There are two path inputs:
   - Path to Form: Path to the form in 'create' mode, this will create a new
     base entity and build the form from there.
   - Path to Edit Form: Path to the form in 'edit' mode, this will load the
     base entity based on an argument in the path (denoted by a %).
5. Click 'Save Flexiform'. You can now add Entities and Elements to your form
   by clicking 'Manage Form Entities' and 'Manage Form Fields' respectively.

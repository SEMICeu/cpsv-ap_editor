(function ($) {
  Drupal.behaviors.export = {
    attach: function (context, settings) {
      // Code to be run on page load, and
      // on ajax load added here
     $("#edit-type").on('change', function() {
       var organization_id =  "edit-title"; 
       var organization_field = $("#"+ organization_id);
       var organization_label = $('label[for="'+organization_id+'"]');
       var selection = $(this).find("option:selected").text();
       if(selection == "Public Service") {
          organization_field.show();
          organization_label.show()
       } else {
          organization_field.hide();
          organization_label.hide();
       }
    });
  }
}}(jQuery));

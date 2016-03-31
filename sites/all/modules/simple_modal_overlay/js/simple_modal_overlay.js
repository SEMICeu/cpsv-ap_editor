(function ($) {
  Drupal.behaviors.simple_modal_overlay = {
    attach:function (context, settings) {
      // If there are any simple overlays on the page, trigger special handling.
      if ($('.simple-overlay').length > 0) {
        // Enable the close link.
        $('.simple-overlay-close').click(function(e) {
          e.preventDefault();
          
          // When the overlay is closed, remove it and the background behind it.
          $('.simple-overlay .message-inner').fadeOut(300, function() {
        	  $(this).remove();

              // Fade out the background last
              $('.simple-overlay').fadeOut(300, function() {
            	  $(this).remove();
        	  });
    	  });
        });
      }
    }
  }
})(jQuery);

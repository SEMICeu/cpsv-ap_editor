(function ($) {
Drupal.behaviors.lang_dropdown = {
  attach: function (context, settings) {
    var settings = settings || Drupal.settings;

    if (settings.lang_dropdown) {
      var flags, msddSettings;
      for (key in settings.lang_dropdown) {
        msddSettings = settings.lang_dropdown[key].jsWidget;
        flags = msddSettings.languageicons;
        if (flags) {
          $.each(flags, function(index, value) {
            if (msddSettings.widget == "msdropdown") {
              $('select#lang-dropdown-select-' + key + ' option[value="' + index + '"]', context).attr('data-image', value);
            }
            else if (msddSettings.widget == "ddslick" && Boolean(msddSettings.showSelectedHTML)) {
              $('select#lang-dropdown-select-' + key + ' option[value="' + index + '"]', context).attr('data-imagesrc', value);
            }
          });
        }

        if (msddSettings.widget == "msdropdown") {
          try {
            $('select#lang-dropdown-select-' + key, context).msDropDown({
              visibleRows: msddSettings.visibleRows,
              roundedCorner: Boolean(msddSettings.roundedCorner),
              animStyle: msddSettings.animStyle,
              event: msddSettings.event,
              mainCSS: msddSettings.mainCSS
            });
          }
          catch (e) {
            if (console) { console.log(e); }
          }
        }
        else if (msddSettings.widget == "chosen") {
          $('select#lang-dropdown-select-' + key, context).chosen({
            disable_search: msddSettings.disable_search,
            no_results_text: msddSettings.no_results_text
          });
        }
        else if (msddSettings.widget == "ddslick") {
          $('select#lang-dropdown-select-' + key, context).ddslick({
            width: (msddSettings.width == 0) ? null : msddSettings.width,
            height: (msddSettings.height == 0) ? null : msddSettings.height,
            showSelectedHTML: Boolean(msddSettings.showSelectedHTML),
            imagePosition: msddSettings.imagePosition,
            onSelected: function(data) {
              // ddselect also fires this function on initialization, so we have
              // to make sure this select has already been initialized.
              if (!data.selectedItem.parent().data('ddslick-has-been-initialized')) {
                data.selectedItem.parent().data('ddslick-has-been-initialized', true);
                return;
              }

              var lang = data.selectedData.value;
              var href = $('#lang-dropdown-select-'+key, context).parents('form').find('input[name="' + lang + '"]').val();
              window.location.href = href;
            }
          });
        }
      }
    }

    $('select.lang-dropdown-select-element', context).change(function() {
      var lang = this.options[this.selectedIndex].value;
      var href = $(this).parents('form').find('input[name="' + lang + '"]').val();
      window.location.href = href;
    });

    $('form.lang-dropdown-form', context).after('<div style="clear:both;"></div>');
  }
};
})(jQuery);

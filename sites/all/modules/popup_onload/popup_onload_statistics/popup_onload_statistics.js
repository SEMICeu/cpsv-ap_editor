/**
 * @file
 * Attach clicks/views statistics JS to colorbox.
 */

(function ($) {

Drupal.behaviors.attachPopupOnLoadStatistics = {

  attach: function (context, settings) {
    if (!$.isFunction($.colorbox)) {
        return;
      }

    function PopupOnLoadStatisticsLogAction(atype) {
      var popup_onload_settings = settings.popup_onload;

      $.ajax({
        async: false,
        dataType: 'json',
        data: {
          atype: atype,
          popup_id: popup_onload_settings.popup_id
        },
        type: 'POST',
        url: '/popup_onload_stats_log'
      });
    }

    $(document).bind('cbox_complete', function() {
      PopupOnLoadStatisticsLogAction('view');

      $('#colorbox.popup_onload a, #colorbox.popup_onload area').click(function (event) {
        PopupOnLoadStatisticsLogAction('click');
        return true;
      });
      $('#colorbox.popup_onload form').submit(function (event) {
        PopupOnLoadStatisticsLogAction('click');
        return true;
      });
    });
  }
}

})(jQuery);

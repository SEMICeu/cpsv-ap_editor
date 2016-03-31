
jQuery.noConflict();

  jQuery(document).ready( function() {

    // dynamically change the value of default namespace URI option in vocabulary form as the vocabulary id changes
    jQuery('#edit-title').bind('keyup', function() {
        if(jQuery("#edit-title").val() != '') {
          jQuery("#neologism-default-ns").html(jQuery("#edit-title").val());
        }else {
          jQuery("#neologism-default-ns").html('vocabulary-id');
        }
      }
    );

    // dynamically change the value of default namespace URI option in vocabulary form as the vocabulary id changes
    jQuery('#edit-vocabulary-title-und-0-value').bind('keyup', function() {
        if(jQuery("#edit-vocabulary-title-und-0-value").val() != '') {
          jQuery("#neologism-default-ns").html(jQuery("#edit-vocabulary-title-und-0-value").val());
        }else {
          jQuery("#neologism-default-ns").html('vocabulary-id');
        }
      }
    );


    // when custom option is selected in namespace URI, create textbox if not present else set visibility to show in vocabulary form
    jQuery('#edit-vocabulary-namespace-uri-und-1').change(function() {
    // this is called when radio button is pressed, i.e., document.getElementById('edit-vocabulary-namespace-uri-und-1').value ==1
          if( ! document.getElementById('form-item form-type-radio form-item-vocabulary-namespace-uri-und custom-textbox')){
            jQuery('#edit-vocabulary-namespace-uri-und').append('<p><input style = "width:300px" type="text" class = "text-full form-text" id="form-item form-type-radio form-item-vocabulary-namespace-uri-und custom-textbox" name="form-item form-type-radio form-item-vocabulary-namespace-uri-und custom-textbox" value="http://'+ window.location.href.split( '/' )[2] + '/" style="display:block" /></p>');
          }else{
            document.getElementById('form-item form-type-radio form-item-vocabulary-namespace-uri-und custom-textbox').style.display="";
          }
        }
    );


  // when default option is selected in namespace URI, hide textbox in vocabulary form
  jQuery('#edit-vocabulary-namespace-uri-und-0').change(function() {
    // this is called when radio button is pressed.
          if( document.getElementById('form-item form-type-radio form-item-vocabulary-namespace-uri-und custom-textbox')){
            document.getElementById('form-item form-type-radio form-item-vocabulary-namespace-uri-und custom-textbox').style.display="none";
            }
        }
    );


// create vocabulary field for class and property forms - NOW DONE USING FIELD API
//    jQuery('#class-node-form div:eq(0)').before('<br /><div id ="related-to-vocabulary"><span>Related Vocabulary Id&nbsp;&nbsp;&nbsp;</span> <input style = "width:300px" type="text" class = "text-full form-text" id="form-item-class-vocabulary-related-textbox" name="form-item-class-vocabulary-related-textbox" style="display:block" /></div><br />');
//    jQuery('#property-node-form div:eq(0)').before('<br /><div id ="related-to-vocabulary"><span>Related Vocabulary Id&nbsp;&nbsp;&nbsp;</span><input style = "width:300px" type="text" class = "text-full form-text" id="form-item-property-vocabulary-related-textbox" name="form-item-property-vocabulary-related-textbox" style="display:block" /><br /></div>');



// for class form : dynamically change the value of the prefix of the class title as per vocabulary id
    jQuery('#edit-class-related-vocabulary-und-0-nid').bind('keyup', function() {
        if(jQuery('#form-item-class-vocabulary-related-textbox').val() != ''){ 
          jQuery('#class-related-vocabulary-id-value').html(jQuery('#edit-class-related-vocabulary-und-0-nid').val().split(' ')[0] ) ;
        }else {
          jQuery('#class-related-vocabulary-id-value').html("vocabulary-id") ;
        }
      }
    );
    jQuery('#edit-title').bind('keyup', function() {
        if(jQuery('#form-item-class-vocabulary-related-textbox').val() != ''){ 
          jQuery('#class-related-vocabulary-id-value').html(jQuery('#edit-class-related-vocabulary-und-0-nid').val().split(' ')[0] ) ;
        }else {
          jQuery('#class-related-vocabulary-id-value').html("vocabulary-id");
        }
      }
    );


// for property form : dynamically change the value of the prefix of the property title as per vocabulary id
    jQuery('#edit-property-related-vocabulary-und-0-nid').bind('keyup', function() {
        if(jQuery('#form-item-property-vocabulary-related-textbox').val() != ''){ 
          jQuery('#property-related-vocabulary-id-value').html(jQuery('#edit-class-related-vocabulary-und-0-nid').val().split(' ')[0] ) ;
        }else {
          jQuery('#property-related-vocabulary-id-value').html("vocabulary-id") ;
        }
      }
    );
    jQuery('#edit-title').bind('keyup', function() {
        if(jQuery('#form-item-property-vocabulary-related-textbox').val() != ''){ 
          jQuery('#property-related-vocabulary-id-value').html(jQuery('#edit-class-related-vocabulary-und-0-nid').val().split(' ')[0] ) ;
        }else {
          jQuery('#property-related-vocabulary-id-value').html("vocabulary-id") ;
        }
      }
    );

  } );
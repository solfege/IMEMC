var secs;
var timerID = null;
var timerRunning = false;
var delay = 1000;


var gv_selected_image = 0;

      function callAlertTest() { alert("Basic Javascript Test. Ok."); }
      function toggleTinyMCE(is_story)
      {
          var summary_id = 'summary_mce';
          var textarea_id = 'story_mce';
          if (tinyMCE.get(textarea_id)) {
	      if (is_story) tinyMCE.execCommand('mceRemoveControl', false,summary_id);
              tinyMCE.execCommand('mceRemoveControl', false,textarea_id);
	  } else {
	      if (is_story) tinyMCE.execCommand('mceAddControl', false,summary_id);
              tinyMCE.execCommand('mceAddControl', false,textarea_id);
	  }
      }

      function InitializeTimer(x)
      {
          // Set the length of the timer, in seconds
          secs = x;
          StopTheClock();
          StartTheTimer();
      }

      function StopTheClock()
      {
          if(timerRunning)
              clearTimeout(timerID);
          timerRunning = false;
      }

      function StartTheTimer()
      {
          if (secs==0)
          {
              StopTheClock();
              alert("Your Lock on this item has expired.  You should save your work now or risk having somebody else over-write it!");
          }
          else
          {
              self.status = secs;
              secs = secs - 1;
              timerRunning = true;
              showtime();
              timerID = self.setTimeout("StartTheTimer()", delay);
          }
      }

      /* -------------------------------------------------
         showtime()
         Puts the amount of time that has passed since
         loading the page into the field named timerField in
         the form named timeForm
         -------------------------------------------------  */

      function showtime()
      {
         var hours = Math.floor( secs / 3600 );
         var elapsedSecs = secs - (hours*3600);

         var minutes =  Math.floor( elapsedSecs / 60 );
         elapsedSecs = elapsedSecs - (minutes*60);

         var seconds = elapsedSecs;

         var timeValue = "" + hours;
         timeValue  += ((minutes < 10) ? ":0" : ":") + minutes;
         timeValue  += ((seconds < 10) ? ":0" : ":") + seconds;

         // Update display
         document.timerform.timeleft.value = timeValue;
      }

// -- End of JavaScript code -------------- -->


function checkForEventType(event_type_id)
{
    if(document.publishform.type_id.value== event_type_id)
    {
        document.publishform.event_time_min.disabled=false;
        document.publishform.event_time_min.style.display="";
        document.publishform.event_time_hr.disabled=false;
        document.publishform.event_time_hr.style.display="";
        document.publishform.event_time_day.disabled=false;
        document.publishform.event_time_day.style.display="";
        document.publishform.event_time_month.disabled=false;
        document.publishform.event_time_month.style.display="";
        document.publishform.event_time_year.disabled=false;
        document.publishform.event_time_year.style.display="";
    }
    else
    {
        document.publishform.event_time_min.disabled=true;
        document.publishform.event_time_min.style.display="none";
        document.publishform.event_time_hr.disabled=true;
        document.publishform.event_time_hr.style.display="none";
        document.publishform.event_time_day.disabled=true;
        document.publishform.event_time_day.style.display="none";
        document.publishform.event_time_month.disabled=true;
        document.publishform.event_time_month.style.display="none";
        document.publishform.event_time_year.disabled=true;
        document.publishform.event_time_year.style.display="none";
    }
}

function hideEmbeddedVideoForm()
{
    if(document.publishform.number_embed_video.value>=1)
    {
        document.publishform.youtube_id_1.disabled=false;
        document.publishform.youtube_id_1.style.display="";
        document.publishform.embed_vid_desc_1.disabled=false;
        document.publishform.embed_vid_desc_1.style.display="";
        document.getElementById('embv_1').style.display="";
    }
    if(document.publishform.number_embed_video.value>=2)
    {
        document.publishform.youtube_id_2.disabled=false;
        document.publishform.youtube_id_2.style.display="";
        document.publishform.embed_vid_desc_2.disabled=false;
        document.publishform.embed_vid_desc_2.style.display="";
        document.getElementById('embv_2').style.display="";
    }

    if(document.publishform.number_embed_video.value>=3)
    {
        document.publishform.youtube_id_3.disabled=false;
        document.publishform.youtube_id_3.style.display="";
        document.publishform.embed_vid_desc_3.disabled=false;
        document.publishform.embed_vid_desc_3.style.display="";
        document.getElementById('embv_3').style.display="";
    }

    if(document.publishform.number_embed_video.value>=4)
    {
        document.publishform.youtube_id_4.disabled=false;
        document.publishform.youtube_id_4.style.display="";
        document.publishform.embed_vid_desc_4.disabled=false;
        document.publishform.embed_vid_desc_4.style.display="";
        document.getElementById('embv_4').style.display="";
    }

    if(document.publishform.number_embed_video.value>=5)
    {
        document.publishform.youtube_id_5.disabled=false;
        document.publishform.youtube_id_5.style.display="";
        document.publishform.embed_vid_desc_5.disabled=false;
        document.publishform.embed_vid_desc_5.style.display="";
        document.getElementById('embv_5').style.display="";
    }
}

function hideEmbeddedAudioForm()
{
    if(document.publishform.number_embed_audio.value>=1)
    {
        document.publishform.audio_id_1.disabled=false;
        document.publishform.audio_id_1.style.display="";
        document.publishform.embed_audio_desc_1.disabled=false;
        document.publishform.embed_audio_desc_1.style.display="";
        document.getElementById('emba_1').style.display="";
    }
    if(document.publishform.number_embed_audio.value>=2)
    {
        document.publishform.audio_id_2.disabled=false;
        document.publishform.audio_id_2.style.display="";
        document.publishform.embed_audio_desc_2.disabled=false;
        document.publishform.embed_audio_desc_2.style.display="";
        document.getElementById('emba_2').style.display="";
    }

    if(document.publishform.number_embed_audio.value>=3)
    {
        document.publishform.audio_id_3.disabled=false;
        document.publishform.audio_id_3.style.display="";
        document.publishform.embed_audio_desc_3.disabled=false;
        document.publishform.embed_audio_desc_3.style.display="";
        document.getElementById('emba_3').style.display="";
    }

    if(document.publishform.number_embed_audio.value>=4)
    {
        document.publishform.audio_id_4.disabled=false;
        document.publishform.audio_id_4.style.display="";
        document.publishform.embed_audio_desc_4.disabled=false;
        document.publishform.embed_audio_desc_4.style.display="";
        document.getElementById('emba_4').style.display="";
    }

    if(document.publishform.number_embed_audio.value>=5)
    {
        document.publishform.audio_id_5.disabled=false;
        document.publishform.audio_id_5.style.display="";
        document.publishform.embed_audio_desc_5.disabled=false;
        document.publishform.embed_audio_desc_5.style.display="";
        document.getElementById('emba_5').style.display="";
    }
}

function CheckFile(no_of_files, is_comment, legal_section_included)
{
    var max_files = no_of_files;
    var empty_count = 0;
    var warning_msg = "";
    var files_msg = "";
    var tmp_file;
    var tmp_filetype;
    var form_fileCount=0;
    total_elements = document.publishform.elements.length;
    // Goes through all the elements checking those of type file
    for ( iFile = 0; iFile < total_elements ; iFile++ ) { 
	    if ( document.publishform.elements[iFile].type == "file")
	    {
                form_fileCount++;
		tmp_val = document.publishform.elements[iFile].value;
		if ( document.publishform.elements[iFile].value == "")
		{
                    tmp_file = document.publishform.elements[iFile].name;
                    warning_msg += "Filename for file " + form_fileCount +": is EMPTY!\n\n";
                    empty_count++;
		} else {
                    // Check the file types
                    tmp_file = document.publishform.elements[iFile].name;
                    if ( tmp_val.lastIndexOf('.') != -1 )
		    {
                        tmp_filetype = tmp_val.substring(tmp_val.lastIndexOf('.'));
                        // warning_msg += "Filetype for " + tmp_val +" is " + tmp_filetype + "\n";
                        // CheckFileType(tmp_filetype);
                        // warning_msg += "Filetype [" + tmp_filetype +"] for " + tmp_val + " " + CheckFormats(tmp_filetype) + "\n";
                        format_rsp_msg = CheckFormats(tmp_filetype);
			if ( format_rsp_msg != "" )
			{
                            warning_msg += "Failure for file " + form_fileCount + ": [" + tmp_val +"]\n";
                            warning_msg += "Filetype is [" + tmp_filetype +"] " + " " + format_rsp_msg + "\n\n";
                        }
                    } else {
                        warning_msg += "No filetype set for file " + form_fileCount + ": " + tmp_val + "\n\n";
                    }
		}
	    }
    }
    if (empty_count > 0 ) warning_msg += "You selected " + max_files + " files for uploading, but " + empty_count + " are not filled in.\nCheck the selected number otherwise this will cause an error on upload\n";

    if (warning_msg !="") warning_msg += "\n";
    if (document.publishform.author_name.value =="") warning_msg += "Author Name field is empty\n";

    if(is_comment == 1) 
    {
        if (document.publishform.comment_title.value =="") warning_msg += "Comment Title field is empty\n";
        if (document.publishform.comment_content.value =="") warning_msg += "Comment Contents field is empty\n";
    } else {
        if (document.publishform.story_title.value =="") warning_msg += "Story Title field is empty\n";
        if (document.publishform.story_summary.value =="") warning_msg += "Story Summary field is empty\n";
        if (document.publishform.story_content.value =="") warning_msg += "Story Contents field is empty\n";
        if (CheckIsDigit(document.publishform.type_id.value) == 0 )
           warning_msg += "Story Type is not selected\n" ;

        if (CheckIsDigit(document.publishform.topic_id.value) == 0 )
           warning_msg += "Story Topic is not selected\n" ;

        if (CheckIsDigit(document.publishform.region_id.value) == 0 )
           warning_msg += "Story Region is not selected\n" ;

        if (CheckIsDigit(document.publishform.language_id.value) == 0 )
           warning_msg += "Language is not selected\n" ;
    }
    if(legal_section_included == true)
    {
        if (document.publishform.accept_terms.checked != true )
	{
            warning_msg += "Terms & Conditions Checkbox is not checked.\n";
        }
    }
    if (warning_msg != "") alert("---------- VALIDATON FAILURE DETECTED FOR THE FOLLOWING FIELDS ---------- \n\n" +warning_msg);
    else alert("All mandatory fields filled in. \nFile sizes cannot be validated until upload.");
}

function validateTerms(txt)
{
    if(document.publishform.accept_terms.checked==true) return true;
    else
    {
        alert(txt);
        return false;
    }
}

function validateVideoId(vidIndex)
{
    var is_image = 0;
    var is_video = 0;
    var is_audio = 0;
    var is_misc = 0;
    //var tmp_type = target_filetype.substring(1);
    var type_msg = "";

    if (document.publishform.videoType.value != 1 ) return;
    if (vidIndex == 1) type_msg = document.publishform.youtube_id_1.value;
    else if (vidIndex == 2) type_msg = document.publishform.youtube_id_2.value;
    else if (vidIndex == 3) type_msg = document.publishform.youtube_id_3.value;
    else if (vidIndex == 4) type_msg = document.publishform.youtube_id_4.value;
    else if (vidIndex == 5) type_msg = document.publishform.youtube_id_5.value; 

    if (type_msg.indexOf('v=') != -1) alert('Invalid video Id. Remove the v= part of the Id.');
    else if (type_msg.indexOf('=') != -1) alert('Invalid video Id. Remove the = character');
    else if (type_msg.indexOf('&') != -1) alert('Invalid video Id. Remove all data after the & including the & character.');
    else if (type_msg.indexOf('http') != -1) alert('Invalid video Id. Just enter the Id, not the full URL');
    else if (type_msg.indexOf('www.') != -1) alert('Invalid video Id. Do not enter the full URL, just the video Id. See help text for more details');
}

function CheckImagesMain(max_files, graphics_path)
{
            var local_max = 0;
            var form_filename;
            var form_filetype;
            var img_h, img_w; 
            var max_h = 800;
            var max_w = 2000;

	    gv_selected_image++;

	    if (gv_selected_image > 0 ) {
	        // Goes through all the elements checking those of type file
	        var total_elements = document.publishform.elements.length;
	        for ( iFile = 0; iFile < total_elements ; iFile++ ) { 
		    if ( document.publishform.elements[iFile].type == "file")
		    {
                        // form_fileCount++;
		        form_filename = document.publishform.elements[iFile].value;
		        if ( document.publishform.elements[iFile].value != "")
			{
                            // If the filename is not empty then consider it.
                            if ( form_filename.lastIndexOf('.') != -1 )
			    {
                                form_filetype = form_filename.substring(form_filename.lastIndexOf('.'));
                                if (CheckFileType(form_filetype) == 1) {
                                    local_max++;
	                            if (gv_selected_image == local_max ) {
                                        var dispmsg = "\nfile:///" + form_filename;
                                        document.images["check_dim_image"].src = "file:///" + form_filename;
                                    }
                                }
                            }
			}
	            }
	        }
	        if (local_max == 0) alert("None of the files selected are images files");
            }
	    if (gv_selected_image > local_max) gv_selected_image = 0;
	    if (gv_selected_image == 0) 
	        document.images['check_dim_image'].src = graphics_path+"feedlogo.gif";

            img_h = document.images["check_dim_image"].height;
            img_w = document.images["check_dim_image"].width;
            var btn_msg; 
            btn_msg = "Validate Image Dimensions";
	    if (gv_selected_image != 0) 
            {
                btn_msg += "\nDimensions are height: " + img_h + " width: " + img_w;
                btn_msg += " Img No: = " + gv_selected_image;
            }
            // There is a delay in the loading. Need to add a delay to wait for the height and width
            // to become the correct values.
            document.publishform.check_images.value = btn_msg;
            if (img_h > max_h || img_w > max_w ) alert("Image dimensions " + img_w + "x" + img_h + " exceed permitted values");

}

<?php
/************************************************************************/
/* Oscailt                                                              */
/* Indepenent Media Centre Content Management System                    */
/* ==================================================================== */
/* Copyright (c)2003-2005 by Independent Media Centre Ireland           */
/* http://www.indymedia.ie                                              */
/* Development List: oscailt@lists.indymedia.org                        */
/* See contributions.txt for the list of contributors                   */
/*                                                                      */
/* This program is free software. You can redistribute it and/or modify */
/* it under the terms of the GNU General Public License as published by */
/* the Free Software Foundation. http://www.gnu.org/copyleft/gpl.html   */
/*                                                                      */
/* This program is distributed in the hope that it will be useful,      */
/* but WITHOUT ANY WARRANTY; without even the implied warranty of       */
/* MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the        */
/* GNU General Public License for more details.                         */
/************************************************************************/

require_once("oscailt_init.inc");
require_once("objects/comment.inc");
require_once("objects/editorial.inc");


// This function has 3 modes: comment, story or both for displaying hidden items.
// Page size controls how many items to display. It probably should be less for stories.
// There is a grey period (secs) for which it is possible to display hidden comments.
// A stiple background image can be switched on or off for the title background.
// Recommended page sizes: comments 20, story 10 and both 30
function displayHiddenListing($page_size=20, $display_mode = "comment", $grey_period=0, $use_stipple=false)
{
   $start_row = 0;
   // $page_size = 20;
   if ($page_size < 0 ) $page_size = 15;

   // $grey_period  = 10 * 3600;
   
   // Display list of hidden comments
   $editorialObject = new Editorial();
   $editorialObject->resetArrays();
   $editorial_query = $editorialObject->selectHiddenItems($start_row, $page_size, "hide", $display_mode);
   if($editorial_query === false)
   {
       echo "<P class=error>Failed to read editorial_actions table</P>";
   } else {

       if (isset($_REQUEST['comment_id']) && $_REQUEST['comment_id'] != "") {
           displayHiddenComment($grey_period, $editorialObject);
       }
       if ($display_mode == "comment") {
           writeHeading("<H3>List of hidden comments</H3>");

       } elseif ($display_mode == "story") {
           writeHeading("<H3>List of hidden stories</H3>");

       } elseif ($display_mode == "both") {
           writeHeading("<H3>List of hidden stories and comments </H3>");
       }

       displayHiddenItemsList($grey_period, $editorialObject, $use_stipple);
   }

}

function getHideTimeout($grey_period)
{
   return (time() - $grey_period);
}

function convertToHex($number)
{
     if ($number > 256) $number = 255;
     $number = round($number);
     $top_digit = floor($number/16);
     $bot_digit = $number-($top_digit*16);

     if ($top_digit > 9 ) $top_digit = CHR(65+($top_digit-10));
     if ($bot_digit > 9 ) $bot_digit = CHR(65+($bot_digit-10));

     return $top_digit.$bot_digit.$top_digit.$bot_digit.$top_digit.$bot_digit;
}

function displayHiddenComment($grey_period, $editorialObject)
{
   $current_time = time();

   // For color 255 is white and 0 is black. So give an offset away from white and you can set the max
   // blackness by setting color_mindark to anything above zero.
   $color_offset = 5;
   $color_mindark= 50;
   $hide_timeout = getHideTimeout($grey_period);

   $valid_comment = false;
   $decay_time    = 0;
   if (isset($_REQUEST['comment_id']) && $_REQUEST['comment_id'] != "") {
       $t_comment_id = $_REQUEST['comment_id'];
       $valid_comment = false;
       for ($jrow=0; $jrow < $editorialObject->query_count; $jrow++)
       {
           if ($editorialObject->secondary_id[$jrow] == $t_comment_id) {
               if( $editorialObject->action_time[$jrow] > $hide_timeout) {

                   $decay_time = $current_time - $editorialObject->action_time[$jrow];

                   $decay_time = 256 - $color_offset - ((256-$color_offset) * $decay_time / $grey_period);
		   if ($decay_time < $color_mindark) $decay_time = $color_mindark;

                   $valid_comment = true;
               }
               break;
           }
       }
   }
   if (isset($_REQUEST['display_comment']) && $_REQUEST['display_comment'] == 'true') {
       // Validate comment.
       // if (isset($_REQUEST['comment_id']) && $_REQUEST['comment_id'] != "") {
       if ($valid_comment == true ) {
           require_once("objects/comment.inc");
           $comment = new Comment();
           
           $comment->comment_id = $t_comment_id;
           $comment->load() ;
           //$decay_time = 155;
           // echo "DecayTime= ". $decay_time . " <BR>";
           $t_color=convertToHex($decay_time);

           // echo " hex ". convertToHex($decay_time);
	   // $bki_url = "url('graphics/stipple2.jpg')"; 
       
           echo '<div style="background-color: #'.$t_color.'">';
           echo '<p style="text-align: center">';
           echo "Contents of hidden comment id: ".$comment->comment_id ;
           echo '</p>';
           // echo '<div class="comment" style="background-image: '.$bki_url.';">';
           echo '<div class="comment">';
           echo '<div class="comment-title">';
           echo $comment->comment_title;
           echo '</div>';
           echo "<span class='article-details'>";
           echo "<span class='article-detail'>By ".$comment->author_name."</span>";
           echo "</span>";
           echo "<P class='comment'>".$comment->comment_content. "</P>";
           echo '</div>';
           echo '</div>';
           echo '<br>';
       } else {
           echo 'Comment id select is beyond the view period <BR>';
       }

   }
}
function writeHeading($headingText)
{
   echo "<P class='admin'>". $headingText ."</P>";

}
// This function can display a list of either hidden comments or stories. You can turn on
// stipple to make it harder to read the titles.

function displayHiddenItemsList($grey_period, $editorialObject, $use_stipple=false)
{

   global $system_config, $editor_session ;
   $show_reltime = 1;
   $current_time = time();
   $hide_timeout = getHideTimeout($grey_period);


   ?><TABLE class='admin' border=1><TR>
     <TH class='admin'>Time Hidden</TH><TH class='admin'>Type</TH><TH class='admin'> Id </TH><TH class='admin'>Title</TH><TH class='admin'>Reason</TH></TR>
   <?


   $url_base = '<a href="http://'.$system_config->site_url."/";

   
   for ($jrow=0; $jrow < $editorialObject->query_count; $jrow++)
   {
         // $irow = ($editorialObject->query_count -1) - $jrow;
         $irow = $jrow;

         if ($show_reltime == 1) {
             $unix_timestamp = $current_time - $editorialObject->action_time[$irow];
             $rel_timemsg =  getTimeAgoString($unix_timestamp);
             ?><TR class='admin'><TD class='admin'><?=$rel_timemsg?></TD><?

         } else {
	     $action_timestamp = strftime($system_config->default_strftime_format, $editorialObject->action_time[$irow] + $system_config->timezone_offset);
             ?><TR class='admin'><TD class='admin'><?=$action_timestamp?></TD><?
         }

	 echo "<TD class='admin'>".$editorialObject->content_type[$irow]."</TD>";

         if( $editorialObject->action_time[$irow] > $hide_timeout) {
             if( ($editorialObject->action[$irow] == "hide" || $editorialObject->action[$irow] == "unhide")  && ($editorialObject->content_type[$irow] == "comment" || $editorialObject->content_type[$irow] == "story" || $editorialObject->content_type[$irow] == "feature") ) {
    
                 if ($editorialObject->content_type[$irow] == "comment" ) {
    		    // Make sure to continue using single quotes
		    if($editor_session->isSessionOpen() && $editor_session->editor->allowedReadAccessTo("viewhiddencontent")) {
                        $log_url = $url_base . 'article/' . $editorialObject->content_id[$irow] . '#comment'. $editorialObject->secondary_id[$irow].'">'.$editorialObject->content_id[$irow].",".$editorialObject->secondary_id[$irow].'</a>';
		    } else {
                        $log_url = "";
		    }
                    $link2   = '<a href="?display_comment=true&comment_id='. $editorialObject->secondary_id[$irow].'"> '.$editorialObject->secondary_id[$irow].'</a>';
    	            echo "<TD class='admin'>$log_url $link2</TD>";
                 } elseif ($editorialObject->content_type[$irow] == "story" || $editorialObject->content_type[$irow] == "feature" ) {
    		    // Make sure to continue using single quotes
                    // $log_url = $url_base . 'article/' . $editorialObject->content_id[$irow] . '">' .$editorialObject->content_id[$irow].'</a>';
    	            // echo "<TD class='admin'>$log_url</TD>";
    	            echo "<TD class='admin'>".$editorialObject->content_id[$irow]."</TD>";
                 } 
             } 
         } else { 
            if ($editorialObject->content_type[$irow] == "comment" ) {
                $log_url = $editorialObject->content_id[$irow] . ', ' .$editorialObject->secondary_id[$irow];
	    } else {
                $log_url = $editorialObject->content_id[$irow] ;
            }
	    echo "<TD class='admin'>$log_url</TD>";
         }

         $editorialObject->editor_reason[$irow] = str_replace(">", "&gt;", $editorialObject->editor_reason[$irow]);
         $editorialObject->editor_reason[$irow] = str_replace("<", "&lt;", $editorialObject->editor_reason[$irow]);
         if ($use_stipple == true) {
             $bki_url = 'background-image: url("graphics/stipple.jpg")'; 
             echo "<TD class='admin' style=' ".$bki_url.";'>".$editorialObject->item_title[$irow]."</TD>";
	 } else {
             echo "<TD class='admin'>".$editorialObject->item_title[$irow]."</TD>";
	 }
         echo "<TD class='admin'>".$editorialObject->editor_reason[$irow]."</TD>";
         echo "</TR>";
   }

   ?></TABLE> <?
}

function writeHideList()
{
   global $system_config ;
   $show_reltime = 0;

   $logfile = 'hides_list.log';
   $cols = 6;   

   $log = $system_config->log_store.$logfile;
   if(file_exists($log))
   {
      $logsize = filesize($log);
   }
   else $logsize = 0;

   $chunksize = 5120;
   $chunksize = 3072;
   $startoffset = $chunksize;
   
   if($startoffset > $logsize) $startoffset = $logsize;
   if($chunksize > $logsize) $chunksize = $logsize;

  
   if (!file_exists($log))
   {
      echo "<P class=error>Editorial action file not found.</P>";
      return;
   }
   $fp = fopen($log,"r") ;
   if(is_resource($fp)){
         fseek($fp,-$startoffset,SEEK_END);//last 10k
         $content = fread($fp, $chunksize);
         fclose($fp);
   }
   else
   {
      echo "<P class=error>Error reading entries</P>";
      return;
   }

   ?><TABLE class='admin' border=1><TR>
     <TH class='admin'>Time Hidden</TH><TH class='admin'>Type</TH><TH class='admin'> Id </TH><TH class='admin'>Title</TH><TH class='admin'>Reason</TH></TR>
   <?


   $log_entries = array();
   $entries = array();

   // $current_time = time();

   if(strlen($content) > 0)
   {
         // This will parse them based on a cr-lf separator
         $entries = explode("\n", $content);
         //$messages = explode("\n", $content);
         //foreach($messages as $message)
         //{
	     // debug echo("Entry=" .$message."<BR>");
             //list($t, $m) = explode("\n", $message);
             //if($t != null and $m != null) {
                 //$entries[] = array($t, $m);
             //}
         //}
   }
   if(count($entries) == 0)
   {
       ?><TR class='admin'><TD class='admin' colspan=5><i>No entries found</i></TD></TR><?
       return;
   }
   $entries = array_reverse($entries);


   foreach($entries as $entry)
   {
       // Format is: hide: date("Y-M-d:H:i") story: story_id : story_title reason: 
       //            123456789012345678901234567890123456789012345678901234567890

       $date_part = trim(substr($entry, 5,17));
       if ( strlen($date_part) < 10 ) continue;

       ?><TR><TD class='admin'><?=$date_part?></TD><?
       $rest_of_line = substr($entry, 22,-1);
       $parts = explode(": ", $rest_of_line);
       $token_cnt = 0;

       // 1st part = story or comment
       // 2nd part = item id
       // 3rd part = title
       // 4th part = reason:
       // 5th part = reason text
       $reason_text = strstr($rest_of_line, "reason:");
       $reason_text = substr($reason_text,8);
       foreach($parts as $part)
       {
	    $token_cnt++;

            if($token_cnt  == 4 ) {
		break;
            }
            if($token_cnt  == 3 ) {
		$part = substr($part,0,-6);
            }
            echo "<TD class='admin'>$part</TD>";
       }
       echo "<TD class='admin'>$reason_text</TD>";
       echo "</TR>";
   }
   ?></TABLE> <?
}

   // This is the old version which is retained for compatibity and can be 
   // uncommented out.
   // writeHideList();

   // page size, type, timeout, stipple
   // displayHiddenListing(20, "comment", 2*3600, false);
   // displayHiddenListing(10, "story", 0, false);

   displayHiddenListing(30, "both", 2*3600, false);

?> 

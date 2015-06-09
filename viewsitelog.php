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

//this is due an over-haul - works for the moment.
require_once("oscailt_init.inc");
require_once("objects/story.inc");
$OSCAILT_SCRIPT = "viewsitelog.php";

addToPageTitle("View Site Logs");

function writeCommonHeader($write_top_hdr, $write_subhead, $cols, $header, $page_control, $write_fileinfo=false,$target_file="")
{
   global $system_config, $OSCAILT_SCRIPT;

   // Need to determine what mode of display we are.
   if(isset($_REQUEST['log_type']))
   {
      if($_REQUEST['log_type'] == 'action') $mode_msg = 'Editorial Log';
      elseif($_REQUEST['log_type'] == 'security') $mode_msg = 'Security Log';
      elseif($_REQUEST['log_type'] == 'spam') $mode_msg = 'Spam Log';
      elseif($_REQUEST['log_type'] == 'spam_contact') $mode_msg = 'Contact Form Spam Log';
      elseif($_REQUEST['log_type'] == 'reported') $mode_msg = 'Reported Posts';
      elseif($_REQUEST['log_type'] == 'spammail') $mode_msg = 'Spam Mail';
      elseif($_REQUEST['log_type'] == 'rssfeeds') $mode_msg = 'RSS Cache Directory';
      else $mode_msg = 'Site Log';
   }
   else
   {
      $mode_msg = 'Site Log';
   }

   if(isset($_REQUEST['emailaddr']) && $_REQUEST['emailaddr'] == 'hide')
   {
       $emailaddr_opt = "&emailaddr=hide";
   } else {
       $emailaddr_opt = "";
   }

   if( $write_top_hdr == true )
   {
      $timeMsg = strftime($system_config->default_strftime_format, time() + $system_config->timezone_offset);

      ?>
         <TABLE class='admin'>
            <TR class='admin'><TD class='admin' colspan='<?=$cols?>'><a href="<?=$OSCAILT_SCRIPT?>?log_type=sitelog">Site Error Log</a> | <a href="<?=$OSCAILT_SCRIPT?>?log_type=action_db">Editorial Action Db</a> | <a href="<?=$OSCAILT_SCRIPT?>?log_type=action">Editorial Action Log</a> | <a href="<?=$OSCAILT_SCRIPT?>?log_type=security">Security Log</a> | <a href="<?=$OSCAILT_SCRIPT?>?log_type=spam">Spam Log</a> |
            <a href="<?=$OSCAILT_SCRIPT?>?log_type=reported<?=$emailaddr_opt?>">Reported Posts</a> |
            <a href="<?=$OSCAILT_SCRIPT?>?log_type=spammail<?=$emailaddr_opt?>">Spam Mail</a>|
            <a href="<?=$OSCAILT_SCRIPT?>?log_type=rssfeeds">RSS Errs</a> </TD></TR>
      <?

      if(isset($_REQUEST['log_type']) && $_REQUEST['log_type'] == 'reported')
      {
          if(isset($_REQUEST['emailaddr']) && $_REQUEST['emailaddr'] == 'hide')
	  {
            ?>
            <TR class='admin'><TD class='admin' colspan='<?=$cols?>'><a href="<?=$OSCAILT_SCRIPT?>?log_type=reported">Show From: Field</a></TD></TR>
            <?
	  } else {
            ?>
            <TR class='admin'><TD class='admin' colspan='<?=$cols?>'><a href="<?=$OSCAILT_SCRIPT?>?log_type=reported&emailaddr=hide">Hide From: Field</a></TD></TR>
            <?
	  }
      }

      ?>
          <TR class='admin'><TH COLSPAN=<?=$cols?> class='admin'>Latest Entries for <?=$mode_msg?> &nbsp; &nbsp;<?=$page_control?><BR> <?=$timeMsg?> </TH></TR>
      <?

      if ($write_fileinfo == true) writeFileInfo($target_file, $cols, $header );

      if ($write_subhead == true)
      {
         ?>
         <TR class='admin'><TH class='admin'>Time</TH><?=$header?>
         <?
      }
   } else {
      ?>
      <TR class='admin'><TD class='admin' colspan='<?=$cols?>'><a href="<?=$OSCAILT_SCRIPT?>?log_type=sitelog">Site Error Log</a> | <a href="<?=$OSCAILT_SCRIPT?>?log_type=action_db">Editorial Action Db</a> |<a href="<?=$OSCAILT_SCRIPT?>?log_type=action">Editorial Action Log</a> | <a href="<?=$OSCAILT_SCRIPT?>?log_type=security">Security Log</a> | <a href="<?=$OSCAILT_SCRIPT?>?log_type=spam">Spam Log</a> |
            <a href="<?=$OSCAILT_SCRIPT?>?log_type=reported<?=$emailaddr_opt?>">Reported Posts</a> |
            <a href="<?=$OSCAILT_SCRIPT?>?log_type=spammail<?=$emailaddr_opt?>">Spam Mail</a> </TD></TR>
      <TR class='admin'><TH COLSPAN=<?=$cols?> class='admin'>Latest Entries for <?=$mode_msg?> &nbsp;<?=$page_control?></TH></TR>
      <?

      writeFilterForm($cols, $header);

      if ($write_fileinfo == true) writeFileInfo($target_file, $cols, $header );
      ?>
      </TABLE>
      <P><small>if somebody wanted to put a button here to auto-rotate the log file and compress the existing one, that would be cool</small></P>
      <?
   }
}
function writeFileInfo($target_file, $cols, $header )
{
   global $system_config;

   if ($target_file == "") return;

   clearstatcache();
   $number_bytes = filesize($system_config->log_store.$target_file);
 
   // Report it in Kb
   $number_bytes = round(100 * $number_bytes /1024 ) / 100 ;
   // Note: stat returns an array and item 10 is the modified time.
   $file_modt = filemtime($system_config->log_store.$target_file);
   $time_msg = strftime("%H:%M:%S, %a %d %b", $file_modt);

   $def_page_factor = 1;
   if(isset($_REQUEST['page_factor']) && $_REQUEST['page_factor'] > 0)
   {
      $def_page_factor = $_REQUEST['page_factor'];
      //$page_size = $def_page_size * $_REQUEST['page_factor'];
      // $page_str = "Page size increased by factor of " . $_REQUEST['page_factor']." to ". $def_page_size. "<BR>";
   }

   ?>
       <TR class='admin'><TD COLSPAN=<?=$cols?> class='admin'>Filesize <?=$number_bytes?> Kb. Last update: <?=$time_msg?></TD></TR>
       <TR class='admin'><TD COLSPAN=<?=$cols?> class='admin'>
       <form name=view_page_size' action='' method=POST> 
       Page Size <select name='page_factor' onchange=submit()><option value='All' selected>Page Size</option>
   <?
   foreach (array(1,2,4,6,8,10,12,14,16) as $each_val ) {
       ?> <option value='<?=$each_val?>' <?
       if ($each_val == $def_page_factor ) 
       {
           ?> selected <?
       }
       ?> >x <?=$each_val?></option> <?
   }
   ?>
       </select>
       <input type=hidden name=page_factor_change value="1" >
       </form>
       </TD></TR>
   <?
}
function generateSelects($select_name, $selectionArray, $target_selected)
{
   $ActionFormData ="<select name='".$select_name."' onchange=submit()>";

   foreach ($selectionArray as $select_id => $select_name)
   {
       $t_selected = "";
       if ($target_selected != null) {
           if ($select_id == $target_selected) $t_selected = " selected";
       }
       $ActionFormData .= "<option value='".$select_id."'".$t_selected.">".$select_name."</option>";
   }
   $ActionFormData .="</select>";

   return $ActionFormData;
}

function writeFilterForm($cols, $header)
{
   // Need to determine what mode of display we are.
   if(isset($_REQUEST['log_type']))
   {
      if($_REQUEST['log_type'] != 'action_db') return;
   }
   else
   {
      return;
   }
   
   // $ActionFormData ="<select name='filter_type' onchange=submit()><option value='All' selected>Filter Action</option><option value='hide'>hide</option><option value='unhide'>unhide</option><option value='hide_unhide'>hide and unhide</option><option value='edit'>edit</option><option value='delete'>delete</option><option value='lock'>lock</option><option value='unlock'>unlock</option><option value='clip'>clip</option>><option value='swap'>swap</option><option value='translate'>translate</option><option value='ban'>ban</option><option value='unban'>unban</option><option value='create'>create</option><option value=''>Show All</option></select> ";
   $selectionArray = array("All" => "Filter Action", "hide" => "hide", "unhide" => "unhide", "hide_unhide" => "hide and unhide", "edit" => "edit", "delete" => "delete", "lock" => "lock", "unlock" => "unlock", "clip" => "clip", "swap" => "swap", "translate" => "translate", "ban" => "ban", "unban" => "unban", "create" => "create", "" => "Show All");

   if (isset($_REQUEST['filter_type'])) 
       $ActionFormData = generateSelects("filter_type", $selectionArray, $_REQUEST['filter_type']);
   else 
       $ActionFormData = generateSelects("filter_type", $selectionArray, null);

   // $TypeFormData ="Filter <select name='filter_item' onchange=submit()><option value='All' selected>Filter Item</option><option value='story'>story</option><option value='comment'>comment</option><option value='feature'>feature</option><option value='IP'>IP</option><option value=''>Show All</option></select> ";
   $TypeFormArray = array("All" => "Filter Item", "story" => "story", "comment" => "comment", "feature" => "feature", "IP" => "IP", "" => "Show All");
   
   if (isset($_REQUEST['filter_item'])) 
       $TypeFormData = generateSelects("filter_item", $TypeFormArray, $_REQUEST['filter_item']);
   else 
       $TypeFormData = generateSelects("filter_item", $TypeFormArray, null);

   $subcols = $cols -5;
   ?>
     <TR class='admin'>
     <TD class='admin' colspan='<?=$subcols?>'> 
       <FORM name='editor_id_filter' action='' method=POST>
            <input type=submit name=find_editor_btn value="Filter Editor">
            <input type=text name=editor_id value="" size=10>
            <input type=hidden name=page value="1" >
       </FORM>
     </TD>
     <TD class='admin' colspan='2'>
       <FORM name='action_type_filter' action='' method=POST> 
            <?=$ActionFormData?> 
            <input type=hidden name=page value="1" >
       </FORM>
     </TD>
     <TD class='admin' colspan='2'>
       <FORM name='item_type_filter' action='' method=POST> <?=$TypeFormData?> 
            <input type=hidden name=page value="1" >
       </FORM>
     </TD>
     <TD class='admin' colspan='<?=$subcols?>'> 
       <FORM name='action_id_filter' action='' method=POST>
            <input type=submit name=find_btn value="Filter Item Id">
            <input type=text name=filter_id value="" size=10>
       </FORM>
     </TD>
     </TR>
   <?

   // Get the HTML that generates the GoTo Article button and the associated Javascript.
   $GotoArticleHTML = getGoToHTML(false);
   ?>
     <TR class='admin'>
     <TD class='admin' colspan='<?=$cols?>'>
	 <?=$GotoArticleHTML?>
     </TD>
     </TR>
   <?
}
function getStatusText($url_text, &$storiesList, &$commentsList)
{

   $status_txt = "<font color='orange'>Status: unknown</font> ";

   $story_pos = strpos($url_text,"story_id");
   if ($story_pos !== false) {
       $story_id = substr($url_text,$story_pos);
       $end_pos  = strpos($story_id,"&");
       if ($end_pos !== false) $story_id = substr($story_id,9, ($end_pos-9));
       else 
       {
           $end_pos  = strpos($story_id,"#");
           if ($end_pos !== false) $story_id = substr($story_id,9, ($end_pos-9));
           else
           $story_id = substr($story_id,9);
       }

       $comment_pos = strpos($url_text,"comment");
       if ($comment_pos === false) {
           for ($jindex = 0; $jindex < count($storiesList); $jindex++) {
               // echo("Matcing array story id " . $storiesList[$jindex]->story_id ." with [". $story_id."]<BR>");
               if ($storiesList[$jindex]->story_id == $story_id ) {
                   if ($storiesList[$jindex]->hidden == true ) 
                       $status_txt = "<font color='red'>Status: hidden</font> ";
                   else
                       $status_txt = "<font color='green'>Status: visible</font> ";
    
                   break;
               }
           }
       } else {
           $comment_id = substr($url_text,$comment_pos);
           $end_pos  = strpos($comment_id,"&");
           if ($end_pos !== false) $comment_id = substr($comment_id,11, ($end_pos-11));
	   else 
           {
               $end_pos  = strpos($comment_id,"comment_id");
               if ($end_pos !== false) $comment_id = substr($comment_id,11);
               else $comment_id = substr($comment_id,7);
           }

           for ($jindex = 0; $jindex < count($commentsList); $jindex++) {
               // echo("Matcing array comment id " . $commentsList[$jindex]->comment_id ." with [". $comment_id."]<BR>");
               if ($commentsList[$jindex]->comment_id == $comment_id ) {
                   if ($commentsList[$jindex]->hidden == true ) 
                       $status_txt = "<font color='red'>Status: hidden</font> ";
                   else
                       $status_txt = "<font color='green'>Status: visible</font> ";
    
                   break;
               }
           }
       }
   }
   return $status_txt;

}
function writeReportedPosts($spam_mail_mode, $logfile = 'reported_post.txt')
{
   global $system_config, $OSCAILT_SCRIPT;

   // $logfile = 'reported_post.txt';
   $header = "<TH class='admin'>Text</TH></TR>";
   $cols = 2;

   $log = $system_config->log_store.$logfile;
   if(file_exists($log))
   {
      $logsize = filesize($log);
   }
   else $logsize = 0;

   // Some of the spam can be full of garbage, so to be able to display a decent number of these,
   // then the chunksize needs to be a good bit bigger than the former 10k which was fine when
   // in reported posts mode.
   $allow_email = false ;
   if ($spam_mail_mode == true ) $allow_email = true ;
   if ($system_config->notification_to_email_address == "") $allow_email = false;

   if ($spam_mail_mode == true ) $chunksize = 1024*60 ;
   else $chunksize = 1024*14 ;

   if(isset($_REQUEST['page']) && $_REQUEST['page'] > 0)
   {
      $startoffset = $chunksize * $_REQUEST['page'];
   }
   else
   {
      $startoffset = $chunksize;
   }
   if($startoffset > $logsize) $startoffset = $logsize;
   if($chunksize > $logsize) $chunksize = $logsize;

   $pages = ceil($logsize / $chunksize);
   $page_num = ceil($startoffset / $chunksize);

   if(isset($_REQUEST['emailaddr']) && $_REQUEST['emailaddr'] == 'hide') $emailaddr_opt = "&emailaddr=hide";
   else $emailaddr_opt = "";

   if($page_num > 1)
   {
      $newer_link = "<A class='editor-option' HREF='$OSCAILT_SCRIPT?log_type=".$_REQUEST['log_type'].'&page='.($page_num-1) . $emailaddr_opt . "'>&lt;&lt; newer logs</a> ";
   }
   else
   {
      $newer_link = "";
   }
   if($page_num < $pages)
   {
      $older_link = " <A class='editor-option' HREF='$OSCAILT_SCRIPT?log_type=".$_REQUEST['log_type'].'&page='.($page_num+1) . $emailaddr_opt . "'>older logs &gt;&gt;</a> ";
   }
   else
   {
      $older_link = "";
   }

   // Was using small html tag but I got rid of it.
   $code = $newer_link."(Page " . $page_num . " of " . $pages .")". $older_link;

   $maxline_buf = 4096;
   $maxline_cnt = 80;
   $line_count = 0;
   $line_len   = 0;
   $longest_line=0;
   $show_reltime=1;
   $show_subjtime=0;

   $alternate_mode = true;

   $file_contents = array();
   $bytes_readsofar = 0;
   $fp = fopen($log,"r") ;
   if(is_resource($fp)){
         fseek($fp,-$startoffset,SEEK_END);//last 10k
	 // Reading to a particular line in a file is messy so we still seek blocks but relatively
	 // big ones and read past it slightly to find the end of the email but it is good enough.
	 // So it is a bit mad, but we just the line lengths to figure out how much of the block is read.
	 // And if you go over the block size, then start searching for the --End_Post-

	 while (!feof ($fp) ) {
             $tmp_line = fgets($fp, $maxline_buf);
             $line_len = strlen($tmp_line) + 1;
             $bytes_readsofar += $line_len ;
	     if ($line_len > $longest_line) $longest_line = $line_len;

             $file_contents[] = $tmp_line;
             if ($bytes_readsofar >= $chunksize)
	     {
                 if( strpos($tmp_line, "--End_Post") !== false)
		 {
                     break;
                 }
	     }
             $line_count++;
         }
         fclose($fp);
   }
   else
   {
      echo "<P class=error>failed to open $log</P>";
   }
   $log_entries = array();
   writeCommonHeader(true, false, $cols, $header, $code );

   if(isset($_REQUEST['emailaddr']) && $_REQUEST['emailaddr'] == 'hide') $display_emailaddr = false;
   else $display_emailaddr = true;

   $chop_subjectline = true;

   if ($spam_mail_mode == false ) {
       $storiesList = getStoriesStatus();
       $commentsList = getCommentsStatus();
   }

   if(count($file_contents) == 0)
   {
       ?><TR class='admin'><TD class='admin' colspan=2><i>No entries found</i></TD></TR><?
   }
   elseif(isset($_REQUEST['sendmail']) && $_REQUEST['sendmail'] > 0) {
       if(isset($_REQUEST['msg_confirm']) && $_REQUEST['msg_confirm'] == 'true') {
          forwardPreviewedEmail();
       } else {
          writeEmailPreview($file_contents, $_REQUEST['sendmail'], $longest_line);
       return;
       }
       writeCommonHeader(false, false, $cols, $header, $code, true, $logfile);
       return;
   } else {

      $class_types = array("admin","admin_fade");
      $admin_cls=$class_types[0];
      $cls_ptr = 0;

      $writing_msgbody = false;
      // Basically don't start displaying emails until your first Start_Post is found otherwise you
      // will probably be hitting a fragment of it.
      $start_everfound = false;
      $fragment_linecnt= 0;
      $current_time = time();
      echo '<FORM name="reported_post_form" enctype="multipart/form-data" action="" method="POST">';
      if ($allow_email == true) {
          // Add hidden variables to allow sending of email.
          ?><INPUT type='hidden' name='sendmail' value='-1'><?
      }
   
      $email_count = 1;
      foreach($file_contents as $entry)
      {
         if( strpos($entry, "--Start_Post") === false)
         {
            if( $start_everfound == false)
	    {
               $fragment_linecnt++;
               continue;
            }
            if( strpos($entry, "--End_Post") === false)
            {
               if( strpos($entry, "Subject:") !== false)
               {
	          $entry = rtrim($entry,"\r\n");
		  // Will remove the text: Reported Post for which mode is false
                  if ($spam_mail_mode == false ) {
                      if ($chop_subjectline == true) $txt_msg = substr($entry, 8+15);
		      else $txt_msg = substr($entry, 8);
                  } else {
		      $txt_msg = substr($entry, 8);
                  }

                  if ($show_subjtime == 1) $txt_msg .= " &nbsp; <b>Sent:</b> " . $hdrtxt_msg;

                  if ($show_subjtime == 1)  {
                     ?><TR class='admin'><TH class='admin'><b>Subject: </b></TH><TH class='admin'><?=$txt_msg?></TH><?
                  } else {
                     ?><TR class='admin'><TD class='<?=$admin_cls?>'><b>Subject: </b></TD><TD class='admin'><?=$txt_msg?></TD><?
                  }
                  echo "</TR>";
               }
	       elseif( strpos($entry, "Email:") !== false)
               {
                  if ($display_emailaddr == false) continue;

	          $entry = rtrim($entry,"\r\n");
	          $txt_msg = substr($entry, 6);
                  ?><TR class='admin'><TD class='<?=$admin_cls?>'>From: </TD><TD class='admin'><?=$txt_msg?></TD><?
                  echo "</TR>\n";
               }
	       elseif( strpos($entry, "Reasons:") !== false)
               {
	          $entry = rtrim($entry,"\r\n");
                  // Sometimes the HTTP referrer is present and the spaces are written as %20. We need to convert these
                  $entry = str_replace("%20"," ",$entry);
	          $txt_msg = substr($entry, 8);
                  ?><TR class='admin'><TD class='<?=$admin_cls?>'>URI Data: </TD><TD class='admin' style='text-wrap: normal'><?=$txt_msg?></TD><?
                  echo "</TR>\n";
               }
	       elseif( strpos($entry, "MsgText:") !== false)
               {
	          $txt_msg = substr($entry, 9);
		  if ($writing_msgbody == false)
		  {
                      $btn_str = "<INPUT type='button' name='msgbtn' value='Show Msg' onClick='JavaScript:toggleHide(" . $email_count . ")'>";
      
                      ?><TR class='admin'><TD class='<?=$admin_cls?>'><?=$btn_str?><?
		      if ($allow_email == true)
		      {
                          ?> &nbsp;<BR> <INPUT type='submit' name='emailbtn' value='Send Email' onClick='JavaScript:submitMail(<?=$email_count?>)'><?
                      }
                      ?></TD><TD class="admin"><?
                      echo("\n");
                      ?><TEXTAREA name="msgbody" rows="9" cols="<?=($longest_line+2)?>"><?
		  }
                  echo($txt_msg);
                  $writing_msgbody = true;
                  $email_count++;
               }
	       elseif( strpos($entry, "Ref_URL:") !== false)
               {
	          $entry = rtrim($entry,"\r\n");
		  $status_txt = getStatusText($entry, $storiesList, $commentsList);
	          $txt_msg = "<a href=\"" . substr($entry, 8) . "\">" .  substr($entry, 8) . "</a>";
                  ?><TR class='admin'><TD class='<?=$admin_cls?>'>URL: </TD><TD class='admin'><?=$status_txt?> &nbsp; <?=$txt_msg?></TD><?
                  echo "</TR>";
               }
	       else
               {
                  if ($writing_msgbody == true ) echo($entry);
               }
            } else {
               // If we get here then we found an end message tag
               if ($writing_msgbody == true ) 
               {
                  $writing_msgbody = false;
                  echo "</TEXTAREA>";
                  echo "</TD>";
                  echo "</TR>";
               }

	       if ($alternate_mode == true) {
                  $cls_ptr++;
                  if ($cls_ptr > 1 ) $cls_ptr = 0;
                  $admin_cls = $class_types[$cls_ptr];
               }
            }
         } else {
            if ($start_everfound == false && $fragment_linecnt > 0)
	    {
               ?><TR class='admin'><TD colspan=2 class='admin'>Fragment of <?=$fragment_linecnt?> lines from previous email skipped over.</TD><?
	    }
            $start_everfound = true;
            // Write out the time the message was sent
	    $hdrtxt_msg = substr($entry, strpos($entry, "Time_Sent:")+10 );
            if ($show_reltime == 1)
	    {
	        $t_diff = $current_time - substr($entry, strpos($entry, "Start_Post-")+11,10 );
	        $hdrtxt_msg .= " ( " .  getTimeAgoString($t_diff) . " )";
	    }
            if ($show_subjtime == 0) {
                ?><TR class='admin'><TH class='admin'>#<?=$email_count?> Sent: </TH><TH class='admin' align='left'>&nbsp;<?=$hdrtxt_msg?></TH><?
            }
         }
      }
      if ($writing_msgbody == true ) echo "</TR>";
      echo "</FORM>";
   }
   echo "\n";

   $email_count--;
   if ($email_count < 0 ) $email_count = 0;


   writeViewLogJScript($email_count, $allow_email);

   writeCommonHeader(false, false, $cols, $header, $code, true, $logfile);

}

function writeViewLogJScript($email_count, $allow_email)
{
   // Now for each email written hide the message
   ?>
      <SCRIPT type="text/javascript" language="javascript">
   <?

   for($i_email=0; $i_email < $email_count;$i_email++)
   {
      ?>
      document.reported_post_form.msgbody[<?=$i_email?>].disabled=true;
      document.reported_post_form.msgbody[<?=$i_email?>].style.display="none";
      <?
      if ($allow_email == true) {
         ?>
         document.reported_post_form.emailbtn[<?=$i_email?>].disabled=true;
         document.reported_post_form.emailbtn[<?=$i_email?>].style.display="none";
         <?
      }
   }
   ?>
      </SCRIPT>
   <?

   ?>
    <SCRIPT type="text/javascript" language="javascript">
    function toggleHide( field_id)
    {
	var real_id = field_id - 1;
        if (document.reported_post_form.msgbody[real_id].disabled == true)
	{
            document.reported_post_form.msgbody[real_id].disabled=false;
            document.reported_post_form.msgbody[real_id].style.display="";
            document.reported_post_form.msgbtn[real_id].value="Hide";
        } else {
            document.reported_post_form.msgbody[real_id].disabled=true;
            document.reported_post_form.msgbody[real_id].style.display="none";
            document.reported_post_form.msgbtn[real_id].value="Show";
        }
	<?
        if ($allow_email == true) {
           ?>
           if (document.reported_post_form.emailbtn[real_id].disabled == true)
	   {
               document.reported_post_form.emailbtn[real_id].disabled=false;
               document.reported_post_form.emailbtn[real_id].style.display="";
           } else {
               document.reported_post_form.emailbtn[real_id].disabled=true;
               document.reported_post_form.emailbtn[real_id].style.display="none";
           }
           <?
        }
	?>
    }
    <?
    if ($allow_email == true) {
    ?>
    function submitMail( field_id)
    {
        document.reported_post_form.sendmail.value=field_id;
	return (true);
    }
    <?
    }
    ?>
    </SCRIPT>
   <?

}

function writeEmailPreview($file_contents, $email_index)
{
    global $system_config;
    $writing_msgbody = false;
    // Basically don't start displaying emails until your first Start_Post is found otherwise you
    // will probably be hitting a fragment of it.
    $start_everfound = false;
    $fragment_linecnt= 0;

    $from_email_address = "";
    $subject_line = "";

    $current_time = time();
    echo '<FORM name="reported_post_send_form" enctype="multipart/form-data" action="" method="POST">';

    ?><INPUT type='hidden' name='sendmail' value='<?=$email_index?>'><?
   
    $email_count = 1;
    foreach($file_contents as $entry)
    {
         if( strpos($entry, "--Start_Post") === false)
         {
            if( $start_everfound == false)
	    {
               $fragment_linecnt++;
               continue;
            }
            if( strpos($entry, "--End_Post") === false)
            {
               if( strpos($entry, "Subject:") !== false)
               {
                  if ($email_count == $email_index) {
	             $entry = rtrim($entry,"\r\n");
                     $entry = substr($entry, 8);
                     $subject_line = $entry;
                     ?><TR class='admin'><TD class='admin'><b>Subject: </b></TD><TD class='admin'><?=$entry?></TD><?
                     echo "</TR>";
                  }
               }
	       elseif( strpos($entry, "Email:") !== false)
               {
                  if ($email_count == $email_index) {
	             $entry = rtrim($entry,"\r\n");
                     $from_email_address = substr($entry, 6);
                     $entry = str_replace("<","&lt;",$entry);
                     $entry = str_replace(">","&gt;",$entry);
	             $txt_msg = substr($entry, 6);
                     ?><TR class='admin'><TD class='admin'><b>From: </b></TD><TD class='admin'><?=$txt_msg?></TD><?
                     echo "</TR>\n";
                  }
               }
	       elseif( strpos($entry, "Reasons:") !== false)
               {
                  if ($email_count == $email_index) {
	             $entry = rtrim($entry,"\r\n");
                     // Convert encoded spaces which are written as %20. 
                     $entry = str_replace("%20"," ",$entry);
	             $txt_msg = substr($entry, 8);
                     ?><TR class='admin'><TD class='admin_fade'>URI Data: </TD><TD class='admin_fade' style='text-wrap: normal'><?=$txt_msg?></TD><?
                     echo "</TR>\n";
                  }
               }
	       elseif( strpos($entry, "MsgText:") !== false)
               {
	          $txt_msg = substr($entry, 9);
		  if (($writing_msgbody == false) && ($email_count == $email_index)) 
		  {
                      ?><TR class='admin'><TD class='admin'><b>Msg Text: </b></TD><TD class="admin"><?
                      echo("\n");
                      ?><TEXTAREA name="msgbody" rows="12" cols="<?=($longest_line+2)?>"><?
		  }
		  if ($email_count == $email_index) {
                      echo($txt_msg);
                      $writing_msgbody = true;
                  }
               }
	       else
               {
		  if ($email_count == $email_index) {
                      if ($writing_msgbody == true ) echo($entry);
                  }
               }
            } else {
               // If we get here then we found an end message tag
               if ($writing_msgbody == true && ($email_count == $email_index))
               {
                  $writing_msgbody = false;
                  echo "</TEXTAREA>";
                  echo "</TD>";
                  echo "</TR>";
               }
               // This executes when the EndPost is found
               $email_count++;
            }
         } else {
            $start_everfound = true;
            if ($email_count == $email_index) {
               // Write out the time the message was sent
	       $hdrtxt_msg = substr($entry, strpos($entry, "Time_Sent:")+10 );
               if ($show_reltime == 1)
	       {
	          $t_diff = $current_time - substr($entry, strpos($entry, "Start_Post-")+11,10 );
	          $hdrtxt_msg .= " ( " .  getTimeAgoString($t_diff) . " )";
	       }
               ?><TR class='admin'><TH class='admin'>#<?=$email_count?> Sent: </TH><TH class='admin' align='left'>&nbsp;<?=$hdrtxt_msg?></TH><?
            }
         }
    }
    if ($writing_msgbody == true ) echo "</TR>";

    ?><TR class='admin'><TD class='admin' colspan=2 align=center> <BR>
    <INPUT type='hidden' name='msg_subject' value='<?=$subject_line?>'> 
    <INPUT type='hidden' name='msg_fromfield' value='<?=$from_email_address?>'>
    <INPUT type='hidden' name='msg_confirm' value='true'> 
    Verify that this is the correct email that you want to forward.<BR>
    <INPUT type='submit' name='msgbtn' value='Forward This Email As It is Not Spam'> <BR>
    It will be sent to the email address <b><?=$system_config->notification_to_email_address?> </b><BR> <BR> <BR>
    </TD>
    </TR>
    <?

    echo "</FORM>";
    echo "\n";
}
function forwardPreviewedEmail()
{
   global $system_config;

   $emailto = $system_config->notification_to_email_address;
   if ($emailto == "") {
       ?><b>No notification-to-email-address set in the system. Therefore email cannot be sent. </b><?
       return;
   }
   if($emailfrom == "") $emailfrom = $system_config->notification_from_email_address;
   $emailreply = $emailto;

   $tmp_to = str_replace("<","&lt;",$emailto);
   $tmp_to = str_replace(">","&gt;",$tmp_to);

   ?><P>The following email will be forwarded to the newswire mailing list at: <?
   echo("<b>" .$tmp_to."</b>");
   ?><TR class='admin'>
      <TD class='admin' colspan=2>
        <b>From:</b> <?=$_REQUEST['msg_fromfield']?> <BR><BR>
        <b>Subject:</b> <?=$_REQUEST['msg_subject']?> <BR><BR>
        <b>MsgText:</b> <?=$_REQUEST['msgbody']?> <BR>
	<BR><BR>
        </TD></TR>
        </P>
   <?

   $emailfrom = $_REQUEST['msg_fromfield'];
   $subject = $_REQUEST['msg_subject'];
   $message = $_REQUEST['msgbody'];

   $emailto = $system_config->contact_email_address;
   if($emailfrom == "") $emailfrom = $system_config->notification_from_email_address;
   $emailreply = $emailto;

   $success =   mail($emailto, $subject, $message, "From: ".$emailfrom."\r\n"."Reply-To: ".$emailfrom.",".$emailreply."\r\n"."X-Mailer: $system_config->software_name/$system_config->software_version PHP/" . phpversion());

   if ($success) {
       ?>Mail was sent successfully <?
   } else {
       ?>Mail was sent unsuccessfully <?
   }

}

function writeLogUserMessage($msg)
{
   echo "<div class='user-message'>".$msg."</div>";
}

function displayBadRssFiles()
{
   global $system_config, $OSCAILT_SCRIPT;

   $rss_dir = $system_config->rss_cache.$logfile;

   if(!file_exists($rss_dir))
   {
      writeLogUserMessage("RSS Cache directory does not exist");
      return;
   }

    
   $logfile = 'rssfeeds';
   $header = "<TH class='admin'>Filename</TH></TR>";
   $cols=2;
   $code="";
   writeCommonHeader(true, false, $cols, $header, $code, false, $logfile);
   
   if (isset($_REQUEST['viewfile']) && $_REQUEST['viewfile'] != "") $viewfile_mode = true;
   else $viewfile_mode = false;

   if ($viewfile_mode == true) {
       $rss_filename = $_REQUEST['viewfile'];
       $rss_create = "Created on: " . date("F d Y H:i:s", filectime($rss_dir.$rss_filename));

       ?> <TR class='admin'><TH class='admin' width=10%>Line</TH><TH class='admin'> Filename: <?=$rss_filename?> <BR><?=$rss_create?> </TH></TR><?
       if(!file_exists(($rss_dir.$rss_filename))) {
            ?><TR class='admin'>
    	          <TD class='admin' align=center> &nbsp;</TD>
    	          <TD class='admin'><?=$rss_filename?> does not exist</TD>
              </TR>
    	    <?
       }

       $rss_filename = $rss_dir . $rss_filename;
       $file_contents = htmlspecialchars(file_get_contents($rss_filename));
       if (strlen($file_contents) > 0 ) {
	   // This will parse them based on a double cr-lf separator
           $file_lines = explode("\n", $file_contents);
           $line_no = 0;
           foreach($file_lines as $each_line)
           {
               $line_no++;
               ?><TR class='admin'>
    	             <TD class='admin' align=center> <?=$line_no?></TD>
    	             <TD class='admin'><?=$each_line?></TD>
                     </TR>
    	       <?
           }
       }
       else
       {
           echo "<P class=error>Contents of file $rss_filename is empty.</P>";
       }

   } else {
       $dh=opendir($rss_dir);
    
       if ($dh === FALSE or $dh == null) {
           reportError("Couldn't open query cache directory: ".$rss_dir);
           return 0;
       }
    
       ?> <TR class='admin'><TH class='admin'>#</TH><TH class='admin'><a class='editor-option' href="<?=$OSCAILT_SCRIPT?>?log_type=rssfeeds&sort=true">Filename</a> </TH></TR><?
       $bad_file_count = 0;
       $delete_bad_files = false;
       if (isset($_REQUEST['bad_rss_clear'])) $delete_bad_files = true;

       $rss_filelist = array();
       while($file=readdir($dh))
       {
             if(!is_dir($rss_dir.$file))
             {
                // So it is a file. Now just look for ones with type .rss 
                if( stristr($file,".rss") !=false) 
                {
                    if ($delete_bad_files == true) {
    	                echo("Deleting " .$rss_dir."/".$file. "<BR>");
    	                unlink($rss_dir."/".$file);
                    } else {
    	                $rss_filelist[] = $file;
                    }
                }
             }
       }
       closedir($dh);
       if (isset($_REQUEST['sort']) && $_REQUEST['sort'] == 'true') sort($rss_filelist);

       foreach($rss_filelist as $file)
       {
    	   $view_url = "<A class='admin' HREF='$OSCAILT_SCRIPT?log_type=rssfeeds&viewfile=".$file."'>View</a> &nbsp; &nbsp;";
           $bad_file_count++;
           ?><TR class='admin'>
    	         <TD class='admin' align=center><?=$bad_file_count?></TD>
    	         <TD class='admin'> <?=$view_url?> <?=$file?></TD>
             </TR>
    	   <?
       }
    
       if($bad_file_count == 0)
       {
           ?><TR class='admin'><TD class='admin' colspan=2><i>No faulty RSS files found </i></TD></TR><?
       } else {
           ?>
             <TR class='admin'>
             <TD class='admin' colspan='2' align=center>
	         <FORM name="rss_logs" action='' method=POST>
	          <input type=submit name="bad_rss_clear" value="Delete all bad RSS files">
	          <input type=hidden name="log_type" value="rssfeeds">
                 </FORM>
             </TD></TR>
           <?
       }
   }

   writeCommonHeader(false, false, $cols, $header, $code, true, "");


}
function writeLogBox()
{
   global $system_config, $OSCAILT_SCRIPT;
   $show_reltime = 0;

   if(isset($_REQUEST['log_type']) && $_REQUEST['log_type'] == 'action')
   {
      if(isset($_REQUEST['reltime']) && $_REQUEST['reltime'] == 'true') $show_reltime = 1;

      $logfile = 'actionlog.txt';
      $cols = 6;   //$nm.":\t".$item_action.":\t".$item_type.":\t".$item_id.":\t".$reason;
      $header = "<TH class='admin'>Editor</TH><TH class='admin'>Action</TH><TH class='admin'>Item Type</TH><TH class='admin'>Item ID</TH><TH class='admin'>Reason</TH></TR>";

   }
   elseif(isset($_REQUEST['log_type']) && $_REQUEST['log_type'] == 'security')
   {
      $logfile = 'securitylog.txt';
      $header = "<TH class='admin'>IP Address</TH><TH class='admin'>User</TH><TH class='admin'>Event</TH></TR>";
      $cols = 4;
   }
   elseif(isset($_REQUEST['log_type']) && $_REQUEST['log_type'] == 'spam')
   {
      $logfile = 'spamlog.txt';
      $header = "<TH class='admin'>IP Address</TH><TH class='admin'>URI</TH></TR>";
      $cols = 3;
   }
   elseif(isset($_REQUEST['log_type']) && $_REQUEST['log_type'] == 'spam_contact')
   {
      $logfile = 'spamlog_contact.txt';
      $header = "<TH class='admin'>IP Address</TH><TH class='admin'>URI</TH></TR>";
      $cols = 3;
   }
   elseif(isset($_REQUEST['log_type']) && $_REQUEST['log_type'] == 'sitelog')
   {
      $logfile = 'sitelog.txt';
      $header = "<TH class='admin'>Event</TH></TR>";
      $cols = 2;
   }
   else
   {
      $_REQUEST['log_type'] = "sitelog";
      $logfile = 'sitelog.txt';
      $header = "<TH class='admin'>Event</TH></TR>";
      $cols = 2;
   }

   $log = $system_config->log_store.$logfile;
   if(file_exists($log))
   {
      $logsize = filesize($log);
   }
   else $logsize = 0;

   // If this parameter has been passed boost the chunksize by that factor. Silent feature for the moment.
   if(isset($_REQUEST['page_factor']) && $_REQUEST['page_factor'] > 0)
   {
      $chunksize = 5120 * $_REQUEST['page_factor'];
      if(isset($_REQUEST['page_factor_change']) && $_REQUEST['page_factor_change'] == '1') {
          $t_user_msg = "Page size increased by factor of " . $_REQUEST['page_factor']." to ". round($chunksize/1024). "k <BR>";
          writeLogUserMessage($t_user_msg);
      }
      $page_factor_opt = "&page_factor=".$_REQUEST['page_factor'];
   } else {
      $page_factor_opt = "&page_factor=1";
      $chunksize = 5120;
   }

   if(isset($_REQUEST['page']) && $_REQUEST['page'] > 0)
   {
      $startoffset = $chunksize * $_REQUEST['page'];
      // But subtract about 2 lines of data to allow for overlap.
      $startoffset = $startoffset - 160;
   }
   else
   {
      $startoffset = $chunksize;
   }
   if($startoffset > $logsize) $startoffset = $logsize;
   if($chunksize > $logsize) $chunksize = $logsize;

   $pages = ceil($logsize / $chunksize);
   $page_num = ceil($startoffset / $chunksize);
   if($page_num > 1)
   {
      $newer_link = "<A class='editor-option' HREF='$OSCAILT_SCRIPT?log_type=".$_REQUEST['log_type'].'&page='.($page_num-1).$page_factor_opt."'>&lt;&lt; newer logs</a> ";
   }
   else
   {
      $newer_link = "";
   }
   if($page_num < $pages)
   {
      $older_link = " <A class='editor-option' HREF='$OSCAILT_SCRIPT?log_type=".$_REQUEST['log_type'].'&page='.($page_num+1).$page_factor_opt."'>older logs &gt;&gt;</a> ";
   }
   else
   {
      $older_link = "";
   }

   $code = "<small>" . $newer_link."(Page " . $page_num . " of " . $pages .")". $older_link ."</small>";



   $fp = fopen($log,"r") ;
   if(is_resource($fp)){
         fseek($fp,-$startoffset,SEEK_END);//last 10k
         $content = fread($fp, $chunksize);
         fclose($fp);
   }
   else
   {
      echo "<P class=error>failed to open $log</P>";
   }

   writeCommonHeader(true, true, $cols, $header, $code, false, $logfile);

   $log_entries = array();
   $entries = array();

   $current_time = time();

   if(strlen($content) > 0)
   {
         // This will parse them based on a double cr-lf separator
         $messages = explode("\r\n\r\n", $content);
         foreach($messages as $message)
         {
	     // debug echo("Entry=" .$message."<BR>");
	     if (strpos($message,"\r\n") === false) continue;
             list($t, $m) = explode("\r\n", $message);
             if($t != null and $m != null)
             {
                 $entries[] = array($t, $m);
             }
         }
   }
   if(count($entries) == 0)
   {
       ?><TR class='admin'><TD class='admin' colspan=<?=$cols?>><i>No entries found</i></TD></TR><?
   }
   $entries = array_reverse($entries);
   $http_mode = isset($_SERVER['HTTPS']) ? "https" : "http";
   $url_base = '<a href="'.$http_mode.'://'.$system_config->site_url.'/';


   foreach($entries as $entry)
   {
      if($logfile == 'sitelog.txt')
      {
         ?><TR class='admin'><TD class='admin'><?=$entry[0]?></TD><TD class='admin'><?=htmlspecialchars($entry[1], ENT_QUOTES);?></TD></TR><?
      }
      else
      {
         if ($show_reltime == 1) {
             $unix_timestamp = $current_time - strtotime($entry[0]);
             $rel_timemsg =  getTimeAgoString($unix_timestamp);
             ?><TR class='admin'><TD class='admin'><?=$rel_timemsg?></TD><?

         } else {
             ?><TR class='admin'><TD class='admin'><?=$entry[0]?></TD><?
         }
         $parts = explode(":\t", $entry[1]);
	 $token_cnt = 0;
	 $generate_url= false;
	 $itemtype = "";
         foreach($parts as $part)
         {
	    $token_cnt++;
            if($logfile == 'spamlog.txt' && $token_cnt == 2) {
                ?><TD class='admin'><?=str_replace("%20"," ",$part)?></TD><?
		continue;
            }
            if($logfile == 'actionlog.txt' && $token_cnt == 2) {
                if($part == "hide" || $part == "unhide") $generate_url= true;
            }
	    // It is not if else because we want to draw this
            if($generate_url == true && $token_cnt == 3) $itemtype = $part;
            
	    if($generate_url == true && $token_cnt == 4) {
                $object_ids = explode(", ", $part);
		if (count($object_ids) > 1 ) {
		    // Make sure to continue using single quotes
                    $log_url = $url_base . 'article/' . $object_ids[0] . '#comment'. $object_ids[1] .'">' .$part .'</a>';
	            echo "<TD class='admin'>$log_url</TD>";
                } elseif ($itemtype == "story" || $itemtype == "feature") {
		    // Make sure to continue using single quotes
                    $log_url = $url_base . 'article/' . $part . '">' .$part .'</a>';
	            echo "<TD class='admin'>$log_url</TD>";
                } 
		else echo "<TD class='admin'>$part</TD>";

		$generate_url = false;
            }
	    else {
                $part = str_replace(">", "&gt;", $part);
                $part = str_replace("<", "&lt;", $part);
                echo "<TD class='admin'>$part</TD>";
            }
         }
         echo "</TR>";
      }
   }

   writeCommonHeader(false, true, $cols, $header, $code, true, $logfile);
}
function writeEditorialDbLogEntries()
{
   global $system_config, $OSCAILT_SCRIPT;
   $show_reltime = 0;

   if(isset($_REQUEST['reltime']) && $_REQUEST['reltime'] == 'true') $show_reltime = 1;

   $def_page_size = 20;
   $cols = 6;   //$nm.":\t".$item_action.":\t".$item_type.":\t".$item_id.":\t".$reason;
   // $ActionFormData ="<FORM name='action_type_filter' action='' method=POST><select name='filter_type' onchange=submit()><option value='hide'>hide</option><option value='unhide'>unhide</option><option value='edit'>edit</option><option value='delete'>delete</option><option value='lock'>lock</option><option value='unlock'>unlock</option><option value='clip'>clip</option><option value='All' selected>Action</option></select></FORM>";
   $ActionFormData ="Action";

   $ItemIdFormData ="Item Id";
   $header = "<TH class='admin'>Editor</TH><Th class='admin'>".$ActionFormData."</Th><TH class='admin'>Item Type</TH><TH class='admin'>".$ItemIdFormData ."</TH><TH class='admin'>Reason</TH></TR>";

   // If this parameter has been passed boost the chunksize by that factor. Silent feature for the moment.
   if(isset($_REQUEST['page_factor']) && $_REQUEST['page_factor'] > 0)
   {
      $page_size = $def_page_size * $_REQUEST['page_factor'];
      echo("Page size increased by factor of " . $_REQUEST['page_factor']." to ". $def_page_size. "<BR>");
   } else {
      $page_size = $def_page_size;
   }

   if(isset($_REQUEST['page']) && $_REQUEST['page'] > 0)
   {
      $start_page = $_REQUEST['page'];
      $start_row = $page_size * ($start_page - 1);
      // echo("Page IS " .$start_page ."<BR>");
   }
   else
   {
      $start_row = 1;
      $start_page = 1;
   }

   if(isset($_REQUEST['editor_id']) && $_REQUEST['editor_id'] != '')
       $editor_id = $_REQUEST['editor_id'];
   else
       $editor_id = "";

   if(isset($_REQUEST['filter_type']) && $_REQUEST['filter_type'] != '') {
       if($_REQUEST['filter_type'] == 'All') $filter_type = "";
       else $filter_type = $_REQUEST['filter_type'];
   } else {
       $filter_type = "";
   }
   
   if(isset($_REQUEST['filter_item']) && $_REQUEST['filter_item'] != '') {
       if($_REQUEST['filter_item'] == 'All') $filter_item = "";
       else $filter_item = $_REQUEST['filter_item'];
   } else {
       $filter_item = "";
   }


   if(isset($_REQUEST['filter_id']) && $_REQUEST['filter_id'] != '')
       $filter_id = $_REQUEST['filter_id'];
   else
       $filter_id = "";

   $editorialObject = new Editorial();
   $editorial_query = $editorialObject->selectEditorialRows($start_row, $page_size, $editor_id, $filter_type,$filter_item, $filter_id);
   if($editorial_query === false)
   {
      echo "<P class=error>Failed to read editorial_actions table</P>";
   }

   $link_options = "";
   if($editor_id != "" ) $link_options = "&editor_id=" . $editor_id;
   elseif( $filter_type != "") $link_options = "&filter_type=" . $filter_type;
   elseif( $filter_item != "") $link_options = "&filter_item=" . $filter_item;

   $pages = ceil($editorialObject->getNumberEditorialRows($editor_id,$filter_type,$filter_item) / $page_size);

   // echo("start_page ".$start_page . " pages ".$pages."<BR>");
   if($start_page > $pages) $start_page = $pages;

   $_REQUEST['log_type'] = "action_db";
   if($start_page > 1)
   {
      $newer_link = "<A class='editor-option' HREF='$OSCAILT_SCRIPT?log_type=".$_REQUEST['log_type'].'&page='.($start_page-1).$link_options."'>&lt;&lt; newer logs</a> ";
   }
   else
   {
      $newer_link = "";
   }
   if($start_page < $pages)
   {
      $older_link = "<A class='editor-option' HREF='$OSCAILT_SCRIPT?log_type=".$_REQUEST['log_type'].'&page='.($start_page+1).$link_options."'>older logs &gt;&gt;</a> ";
   }
   else
   {
      $older_link = "";
   }

   $code = "<small>" . $newer_link."(Page " . $start_page . " of " . $pages .") ". $older_link ."</small>";

   writeCommonHeader(true, true, $cols, $header, $code, false, "");

   $current_time = time();

   if($editorialObject->query_count == 0)
   {
       ?><TR class='admin'><TD class='admin' colspan=<?=$cols?>><i>No entries found</i></TD></TR><?

       writeCommonHeader(false, true, $cols, $header, $code, true, "");
       return;
   }

   $http_mode = isset($_SERVER['HTTPS']) ? "https" : "http";
   $url_base = '<a href="'.$http_mode.'://'.$system_config->site_url.'/';

   for ($jrow=0; $jrow < $editorialObject->query_count; $jrow++)
   {
         // $irow = ($editorialObject->query_count -1) - $jrow;
         $irow = $jrow;

         if ($show_reltime == 1) {
             $unix_timestamp = $current_time - strtotime($editorialObject->action_time[$irow]);
             $rel_timemsg =  getTimeAgoString($unix_timestamp);
             ?><TR class='admin'><TD class='admin'><?=$rel_timemsg?></TD><?

         } else {
	     $action_timestamp = strftime($system_config->default_strftime_format, $editorialObject->action_time[$irow] + $system_config->timezone_offset);
             ?><TR class='admin'><TD class='admin'><?=$action_timestamp?></TD><?
         }

	 echo "<TD class='admin'>".$editorialObject->editor_name[$irow]."</TD>";
	 echo "<TD class='admin'>".$editorialObject->action[$irow]."</TD>";
	 echo "<TD class='admin'>".$editorialObject->content_type[$irow]."</TD>";

         if($editorialObject->action[$irow] == "hide" || $editorialObject->action[$irow] == "unhide" || ($editorialObject->action[$irow] == "edit" && ($editorialObject->content_type[$irow] == "comment" || $editorialObject->content_type[$irow] == "story" || $editorialObject->content_type[$irow] == "feature") )) {

             if ($editorialObject->content_type[$irow] == "comment" ) {
		    // Make sure to continue using single quotes
                    $log_url = $url_base . 'article/' . $editorialObject->content_id[$irow] . '#comment'. $editorialObject->secondary_id[$irow].'">'.$editorialObject->content_id[$irow].",".$editorialObject->secondary_id[$irow].'</a>';
	            echo "<TD class='admin'>$log_url</TD>";
             } elseif ($editorialObject->content_type[$irow] == "story" || $editorialObject->content_type[$irow] == "feature" ) {
		    // Make sure to continue using single quotes
                    $log_url = $url_base . 'article/' . $editorialObject->content_id[$irow] . '">' .$editorialObject->content_id[$irow].'</a>';
	            echo "<TD class='admin'>$log_url</TD>";
             } elseif ($editorialObject->content_type[$irow] == "attachment" ) {
		    // Make sure to continue using single quotes
	            echo "<TD class='admin'>".$editorialObject->content_id[$irow]."</TD>";
             } 
         } 
	 else if ($editorialObject->content_type[$irow] == "comment" ) 
	     echo "<TD class='admin'>".$editorialObject->content_id[$irow]." (".$editorialObject->secondary_id[$irow].")</TD>";
	 else echo "<TD class='admin'>".$editorialObject->content_id[$irow]."</TD>";

         $editorialObject->editor_reason[$irow] = str_replace(">", "&gt;", $editorialObject->editor_reason[$irow]);
         $editorialObject->editor_reason[$irow] = str_replace("<", "&lt;", $editorialObject->editor_reason[$irow]);
         echo "<TD class='admin'>".$editorialObject->editor_reason[$irow]."</TD>";
         echo "</TR>";
   }

   writeCommonHeader(false, true, $cols, $header, $code, true, "");
}
// Function to get the status of the latest stories
function getStoriesStatus()
{
    global $dbconn,$prefix;

    $stmt = "select s.story_id, UNIX_TIMESTAMP(s.time_posted), s.hidden from ".$prefix."_stories as s order by s.time_posted DESC limit 0, 50";

    //execute statement
    $result = sql_query($stmt, $dbconn, 2);
    checkForError($result);
    $stories = array();
    if(sql_num_rows( $result ) > 0)
    {
        for ($i=0; $i<sql_num_rows( $result ); $i++)
        {
            $story = new Story();
            list($story->story_id, $story->time_posted, $story->hidden) = sql_fetch_row($result, $dbconn);
            array_push($stories,$story);
        }
    }
    return $stories;
}
// Function to get the status of the latest comments
function getCommentsStatus()
{
    global $dbconn,$prefix;

    $stmt = "select c.comment_id, c.story_id, UNIX_TIMESTAMP(c.time_posted), c.hidden from ".$prefix."_comments as c order by c.time_posted DESC limit 0, 100";

    //execute statement
    $result = sql_query($stmt, $dbconn, 2);
    checkForError($result);
    $comments = array();
    if(sql_num_rows( $result ) > 0)
    {
        for ($i=0; $i<sql_num_rows( $result ); $i++)
        {
            $comment = new Comment();

            list($comment->comment_id, $comment->story_id, $comment->time_posted, $comment->hidden) = sql_fetch_row($result, $dbconn);
             array_push($comments,$comment);
        }
    }
    return $comments;

}

ob_start();

if($editor_session->isSessionOpen())
{
   if(isset($_REQUEST['log_type']) && $_REQUEST['log_type'] == 'action_db')
       $admin_table_width = "90%";


   writeAdminHeader("editorstatus.php", "User Status");

   if(isset($_REQUEST['log_type']) && $_REQUEST['log_type'] == 'reported')
   {
       writeReportedPosts(false);
   } 
   elseif(isset($_REQUEST['log_type']) && $_REQUEST['log_type'] == 'spammail')
   {
       writeReportedPosts(true, "spamlog_contact.txt");
   } 
   elseif(isset($_REQUEST['log_type']) && $_REQUEST['log_type'] == 'rssfeeds')
   {
       displayBadRssFiles(true, "spamlog_contact.txt");
   } 
   else 
   {
       if(isset($_REQUEST['log_type']) && $_REQUEST['log_type'] == 'action_db')
           writeEditorialDbLogEntries();
       else
           writeLogBox();
   }
}
else
{
   $editor_session->writeNoSessionError();
}

class Editorial
{
    var $no_rows;
    var $query_count;
    var $action_id;
    var $action_time;
    var $editor_name;
    var $action;
    var $content_type;
    var $editor_reason;
    var $content_id;
    var $secondary_id;

    function getNumberEditorialRows($filter_editor ="", $filter_type="", $filter_item="")
    {
       global $prefix, $dbconn;
      
       $grand_total = 0;

       $count_query = "SELECT count(*) from ".$prefix."_editorial_actions ";

       // If filters active, paging is relative to them, so total is different
       if ($filter_editor != "")
       {
           $count_query .= " WHERE editor_name='".$filter_editor."' ";
       }
       else if ($filter_type != "")
       {
           if ($filter_type == "hide_unhide") $count_query .= " WHERE action='hide' OR action='unhide' ";
	   else if ($filter_type == "unban") $count_query .= " WHERE action='unban' OR action='name unban' ";
	   else if ($filter_type == "ban") $count_query .= " WHERE action='ban' OR action='name ban' ";
	   else $count_query .= " WHERE action='".$filter_type."' ";
       }
       else if ($filter_item != "")
       {
           $count_query .= " WHERE content_type='".$filter_item."' ";
       }

       // echo("query ".$count_query. "<BR>");
       $result = sql_query($count_query , $dbconn, 1);
       checkForError($result);
       if(sql_num_rows( $result ) > 0)
       {
           list($grand_total) = sql_fetch_row($result, $dbconn);
           $this->no_rows = $grand_total;
           return $grand_total;
       }
       else
       {
           reportError("No rows present in editorial actions table");
       }
       return 0;
    }
    function resetArrays()
    {
       $this->query_count = 0;
       $this->action_id = array();
       $this->action_time = array();
       $this->editor_name = array();
       $this->action = array();
       $this->content_type = array();
       $this->editor_reason = array();
       $this->content_id = array();
       $this->secondary_id = array();
    }

    function selectEditorialRows($start_limit, $page_size, $filter_editor ="", $filter_type="", $filter_item="",$filter_id="")
    {
       global $prefix, $dbconn;
       $editorial_query = "SELECT action_id, UNIX_TIMESTAMP(action_time), editor_name, action, content_type, editor_reason, content_id, secondary_id from ".$prefix."_editorial_actions";

       if ($filter_editor != "")
       {
           $editorial_query .= " WHERE editor_name='".$filter_editor."' ";
       }
       else if ($filter_type != "")
       {
           if ($filter_type == "hide_unhide") $editorial_query .= " WHERE action='hide' OR action='unhide' ";
	   else if ($filter_type == "unban") $editorial_query .= " WHERE action='unban' OR action='name unban' ";
	   else if ($filter_type == "ban") $editorial_query .= " WHERE action='ban' OR action='name ban' ";
	   else $editorial_query .= " WHERE action='".$filter_type."' ";
       }
       else if ($filter_item != "")
       {
           $editorial_query .= " WHERE content_type='".$filter_item."' ";
       }
       else if ($filter_id != "")
       {
           $editorial_query .= " WHERE content_id='".$filter_id."' ";
       }
       if ($start_limit == 1) $start_limit = 0;
       $editorial_query .= " ORDER BY action_time DESC LIMIT ".$start_limit.", ".$page_size;
       // echo("Query: ".$editorial_query."<BR>");

       $result = sql_query($editorial_query, $dbconn, 1);
       checkForError($result);
       if(sql_num_rows( $result ) > 0)
       {
          $this->resetArrays();
          $this->query_count = sql_num_rows( $result ); 
          for ($i=0; $i<sql_num_rows( $result ); $i++)
          {
             list($this->action_id[$i], $this->action_time[$i],$this->editor_name[$i],$this->action[$i], $this->content_type[$i], $this->editor_reason[$i], $this->content_id[$i], $this->secondary_id[$i]) = sql_fetch_row($result, $dbconn);
             if($this->action[$i] == "ban" || $this->action[$i] == "unban") 
	         $this->content_id[$i] = convertNumberToIP($this->content_id[$i]);
          }
       }
       return true;
   }
}


require_once("adminfooter.inc");
?>
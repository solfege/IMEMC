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
require_once("objects/videos.inc");
require_once("objects/story.inc");
require_once("objects/comment.inc");
require_once('objects/adminutilities.inc');
require_once("objects/indyobjects/indyobject.inc");
require_once("objects/indyobjects/indyitemset.inc");

$OSCAILT_SCRIPT = "editspam.php";

$textLabels = array("title" => "Oscailt Spam Settings Configuration",
	            "main_title" => "Words Entry Screen for Publish Control",
	            "spamwords" => " Enter a list of one or more words on one or more lines which if detected
        in the story content signifies it is an attempt to publish spam.<br>
        <br>
        To mark a word as mandatory place a '+' symbol in front of it. e.g +viagra <br>

        <br>Take care to enter strings unlikely to appear in valid posts.
        A bad choice may cause problems for valid posts. 
        All spam generally links to a website URL (otherwise what's the point!)
        hence these make good choices for spam words.",
	            "spamwords_story" => " Enter a list of one or more words on one or more lines which if detected
        in the story content signifies it is an attempt to publish possibly a bogus story.<br>
        <br>
        To mark a word as mandatory place a '+' symbol in front of it. e.g +minister <br>

        <br>Take care to enter strings unlikely to appear in valid posts.
        A bad choice may cause problems for valid posts. 
        All spam generally links to a website URL (otherwise what's the point!)
        hence these make good choices for spam words.",
	            "spamwords_comment" => "Enter a list of one or more words on one or more lines which if detected
        in the comment content signifies it is an attempt to publish possibly a bogus or malicious comment.<br>
        <br>
        To mark a word as mandatory place a '+' symbol in front of it. e.g +viagra <br>

        <br>Take care to enter strings unlikely to appear in valid posts.
        A bad choice may cause problems for valid posts. 
        All spam generally links to a website URL (otherwise what's the point!)
        hence these make good choices for spam words.",
	            "save_spamwords" => "Save Spamwords",
	            "test_spam_config" => "Test Spam Configuration",
	            "test_spam_description" => "Run a test against the spam word configuration. Enter a sample story Id from the newswire that was previously hidden.",
	            "test_results" => "Test Results",
		    "story_id" => "Story Id",
	            "test_configuration" => "Test configuration",
	            "spam_setting_config" => "Spam Settings Configuration",
	            "checkspam_msg_1" => "Turn on spam words checking for publish stories",
	            "checkspam_msg_2" => "check_spamwords",
	            "checkspam_msg_3" => "Spam words are entered in a separate screen and are checked for in the story content during publish and rejected if they are present. These are entered in the 'Edit Spam Story Wordlist screen'", 
		    "discard_msg_1" => "Tick to discard stories detected as spam otherwise auto-hide",
		    "discard_msg_2" => "discard_spam",
		    "discard_msg_3" => "When enabled stories detected as spam will be discarded and will not get published. Turning this off means the stories are published but remain hidden to allow editor review.",
		    "check_spamlink_msg_1" => "Set threshold for spam links",
		    "check_spamlink_msg_2" => "check_spamlinks",
		    "check_spamlink_msg_3" => "The maximum number of links in a story before it is classified as spam. zero disables it. Start with at least 10.",
		    "bogus_header" => "Bogus or Malicious Stories and Comments",
		    "bogus_header_msg" => "With these settings switched on only system warnings are generated. The stories or comments are still allowed to be published.",

		    "check_scanstory_msg_1" => "Turn on scanning of stories for detection of published bogus stories",
		    "check_scanstory_msg_2" => "check_scanstory",
		    "check_scanstory_msg_3" => "A list of mandatory and optional words are entered which are scanned to try and detect bogus or malicious stories and generate a Oscailt message warning. These are entered in the 'Edit Scan Story Wordlist screen'",
		    "check_scancomment_msg_1" => "Turn on scanning of comments for detection of published bogus comments",
		    "check_scancomment_msg_2" => "check_scancomment",
		    "check_scancomment_msg_3" => "A list of mandatory and optional words are entered which are scanned to try and detect bogus or malicious comments and generate a Oscailt message warning. These are entered in the 'Edit Scan Comment Wordlist screen'", 
	            "save_configuration" => "Save Configuration",
	            "edit_scan_story_wordlist_linktext" => "Edit Scan Story Wordlist",
	            "edit_scan_comment_wordlist_linktext" => "Edit Scan Comment Wordlist",
	            "edit_spam_story_wordlist_linktext" => "Edit Spam Story Wordlist",
	            "spamwords_saved_msg" => "Spamwords file saved successfully!",
	            "configuration_saved_msg" => "Configuration Saved!");

$textObj = new indyItemSet();
$system_config->user_error_reporting=8;
if($textObj->load($system_config->xml_store, "editspam") === false)
{
    $textObj->setUserMessage( array( USER_ERROR, "Failed to get text strings for the Edit Spam. -Using defaults",""));
    $textObj->writeUserMessageBox();
} else {
    foreach (array_keys($textLabels) as $str_key ) {
	if (trim($textObj->getString($str_key)) != "" )
            $textLabels[$str_key] = $textObj->getString($str_key);
	else
        {
            if ($system_config->user_error_reporting == 8) $textLabels[$str_key] .= " using default ";
	} 
    }
}

addToPageTitle($textLabels["title"]);
//addToPageTitle("Oscailt Spam Settings Configuration");

// This function displays some basic information about the PHP installation itself on the server.
// This function displays the entry screen for editing the spamwords text file
function writeSpamwordsEntryScreen()
{
    global $system_config, $textLabels;;
    $spamwords = array();
    $spam_filename = "spamwords.txt";
    $main_title = "Spam";
    if (isset($_REQUEST['spammode']) && $_REQUEST['spammode'] != "") {
        if ($_REQUEST['spammode'] == "spamwords") {
	    $spam_filename = "spamwords.txt";
            $test_parameter_name = "run_story_test";
	} else if ($_REQUEST['spammode'] == "story") {
	    $spam_filename = "story_scanner.txt";
            $test_parameter_name = "run_bogus_story_test";
            $main_title = "Bogus or Malicious ";
	} else if ($_REQUEST['spammode'] == "comment") {
	    $spam_filename = "comment_scanner.txt";
            $test_parameter_name = "run_comment_test";
	} else return ;
    } else {
	echo "Incorrect mode ";
    }
    ?>
    <table align=center class=admin width="65%">
    <tr class=admin>
    <th class=admin colspan=2><font size=+1><?=$main_title.$textLabels['main_title']?> </font> <BR> for <?=$spam_filename?></th>
    </tr>
    <tr class=admin>
      <td class=admin colspan=2> 
    <?
    if ($_REQUEST['spammode'] == "spamwords") {
       echo $textLabels['spamwords'];
       /*
       ?>
        Enter a list of one or more words on one or more lines which if detected
        in the story content signifies it is an attempt to publish spam.<br>
        <br>
        To mark a word as mandatory place a '+' symbol in front of it. e.g +viagra <br>

        <br>Take care to enter strings unlikely to appear in valid posts.
        A bad choice may cause problems for valid posts. 
        All spam generally links to a website URL (otherwise what's the point!)
        hence these make good choices for spam words.
       <?
       */
    } else if ($_REQUEST['spammode'] == "story") {
       echo $textLabels['spamwords_story'];
       /*
       ?>
        Enter a list of one or more words on one or more lines which if detected
        in the story content signifies it is an attempt to publish possibly a bogus story.<br>
        <br>
        To mark a word as mandatory place a '+' symbol in front of it. e.g +minister <br>

        <br>Take care to enter strings unlikely to appear in valid posts.
        A bad choice may cause problems for valid posts. 
        All spam generally links to a website URL (otherwise what's the point!)
        hence these make good choices for spam words.
       <?
       */
    } else if ($_REQUEST['spammode'] == "comment") {
       echo $textLabels['spamwords_comment'];
       /*
       ?>
        Enter a list of one or more words on one or more lines which if detected
        in the comment content signifies it is an attempt to publish possibly a bogus or malicious comment.<br>
        <br>
        To mark a word as mandatory place a '+' symbol in front of it. e.g +viagra <br>

        <br>Take care to enter strings unlikely to appear in valid posts.
        A bad choice may cause problems for valid posts. 
        All spam generally links to a website URL (otherwise what's the point!)
        hence these make good choices for spam words.
       <?
       */
    }
    ?>
      </td>
    </tr>
    <form name="spam_config" action="editspam.php" method=post>
    <?

    // if (isset($_REQUEST['spamfile']) && $_REQUEST['spamfile'] != "") $spam_filename = $_REQUEST['spamfile'];

    $spam_filepath = $system_config->private_cache.$spam_filename;

    if (file_exists($spam_filepath)) {
        $spamwordlist = implode("",file($spam_filepath));
    }
 
    ?>
       <tr><td class=admin align=center> 
           <textarea rows=8 cols=80 name='spamtext_block'><?=$spamwordlist?> </textarea>
           </td>
       </tr>
       <tr>
       <td colspan=2 align=center><input type=submit name=save_spamwords value="<?=$textLabels['save_spamwords']?>">
	 <input type=hidden name=spammode value="<?=$_REQUEST['spammode']?>">
	 <input type=hidden name=spamfile value="<?=$spam_filename?>">
         <br>
         <br>
         </td>
       </tr>
    </form>
    <?
    // Run a test.
    $test_results = "";
    if (isset($_REQUEST['item_id']) && $_REQUEST['item_id'] != "") {
        $t_story_id = cleanseNumericalQueryField($_REQUEST['item_id']);
    }

    if (isset($_REQUEST['run_story_test']) && $_REQUEST['run_story_test'] == "true") {
        if ($t_story_id > 0 ) {
            $story = new story();
            $story->story_id = $t_story_id;
            if ($story->load() == true ) $test_results = storySpamCheck($story, false );
            else $test_results = "Story id ".$t_story_id. " not found. Test cannot be carried out.";
        }
    }
    if (isset($_REQUEST['run_bogus_story_test']) && $_REQUEST['run_bogus_story_test'] == "true") {
        if ($t_story_id > 0 ) {
            $story = new story();
            $story->story_id = $t_story_id;
            if ($story->load() == true ) $test_results = storyBogusCheck($story, false );
            else $test_results = "Story id ".$t_story_id. " not found. Test cannot be carried out.";
        }
    }
    if (isset($_REQUEST['run_comment_test']) && $_REQUEST['run_comment_test'] == "true") {
        if ($t_story_id > 0 ) {
	    // id is a comment in this case
            $comment = new comment();
            $comment->comment_id = $t_story_id;
            if ($comment->load() == true ) $test_results = commentBogusCheck($comment, false );
            else $test_results = "Comment id ".$t_story_id. " not found. Test cannot be carried out.";
        }
    }
    // Run a test against the spam word configuration. Enter a sample story Id from the newswire that was previously hidden.
    ?>
    <tr class=admin>
    <th class=admin colspan=2><?=$textLabels['test_spam_config']?>
    </th>
    <tr class=admin>
    <td class=admin colspan=2><br><?=$textLabels['test_spam_description']?>
    </td>
    <?
    if ($test_results != "") {
	// Test Results
        ?>
        <tr class=admin> <th class=admin colspan=2><?=$textLabels['test_results']?></th></tr>
        <tr class=admin>
        <td class=admin colspan=2 > <?=$test_results?> </td>
        </tr>
        <?
    }
    ?>
    <tr class=admin>
      <form name="test_config" action="editspam.php" method=post>
      <td class=admin colspan=2 align=center> 
          <?=$textLabels['story_id']?>
          <input type=input name=item_id value="" size=15 >
          <input type=submit name=testbtn value="<?=$textLabels['test_configuration']?>">
	  <input type=hidden name=<?=$test_parameter_name?> value="true">
	  <input type=hidden name=spammode value="<?=$_REQUEST['spammode']?>">
      </td>
      </form>
    </tr>
    </table>
    <?

}
// This function displays the information your browser is returning.
function tidyUpValue($t_value)
{
    if ($t_value == null) return "NULL";
    if ($t_value == false) return "FALSE";
    return $t_value;

}

function readOutConfig()
{
}
function writeConfigForm()
{
   global $system_config, $textLabels;
   //$system_config->check_spamwords = 3 ;
   $t_check_spamwords = $system_config->check_spamwords & 1 ;
   $t_check_scanstory = $system_config->check_spamwords & 2 ;
   $t_check_scancomment = $system_config->check_spamwords & 4 ;

   $t_discard_spam = $system_config->check_spamwords & 8 ;

   // Spam Settings Configuration
   ?>
   <table align=center class=admin>
   <tr class=admin>
      <th class=admin colspan=4><font size=+1><?=$textLabels['spam_setting_config']?></font></th>
   </tr>
   <form name="main_config" action="editspam.php" method=post>
   <?

   writeConfigBooleanItem($textLabels['checkspam_msg_1'], $textLabels['checkspam_msg_2'], $t_check_spamwords,$textLabels['checkspam_msg_3']);
   writeConfigBooleanItem($textLabels['discard_msg_1'], $textLabels['discard_msg_2'], $t_discard_spam, $textLabels['discard_msg_3']);
   //writeConfigBooleanItem("Turn on spam words checking for publish stories","check_spamwords",$t_check_spamwords,"Spam words are entered in a separate screen and are checked for in the story content during publish and rejected if they are present. These are entered in the 'Edit Spam Story Wordlist screen'");

      // writeConfigBooleanItem("Tick to discard stories detected as spam otherwise auto-hide","discard_spam",$t_discard_spam,"When enabled stories detected as spam will be discarded and will not get published. Turning this off means the stories are published but remain hidden to allow editor review.");

      writeConfigBooleanItem($textLabels['check_spamlink_msg_1'], $textLabels['check_spamlink_msg_2'], $system_config->check_spamlinks, $textLabels['check_spamlink_msg_3']);
      // writeConfigNumericItem("Set threshold for spam links","check_spamlinks",$system_config->check_spamlinks,"The maximum number of links in a story before it is classified as spam. zero disables it. Start with at least 10.","", 0,60,1, true);
      // Separator
      writeConfigHeader($textLabels['bogus_header'], $textLabels['bogus_header_msg'] );
      // writeConfigHeader("Bogus or Malicious Stories and Comments","With these settings switched on only system warnings are generated. The stories or comments are still allowed to be published.");

      writeConfigBooleanItem($textLabels['check_scanstory_msg_1'], $textLabels['check_scanstory_msg_2'], $t_check_scanstory, $textLabels['check_scanstory_msg_3']);
      // writeConfigBooleanItem("Turn on scanning of stories for detection of published bogus stories","check_scanstory",$t_check_scanstory,"A list of mandatory and optional words are entered which are scanned to try and detect bogus or malicious stories and generate a Oscailt message warning. These are entered in the 'Edit Scan Story Wordlist screen'");

      writeConfigBooleanItem($textLabels['check_scancomment_msg_1'], $textLabels['check_scancomment_msg_2'], $t_check_scancomment, $textLabels['check_scancomment_msg_3']);
      // writeConfigBooleanItem("Turn on scanning of comments for detection of published bogus comments","check_scancomment",$t_check_scancomment,"A list of mandatory and optional words are entered which are scanned to try and detect bogus or malicious comments and generate a Oscailt message warning. These are entered in the 'Edit Scan Comment Wordlist screen'");

   ?>
   <tr>
      <td colspan=2 align=center><input type=submit name=save value="Save configuration"></td>
      </form>
   </tr>
   </table>
   <?
}

function writeConfigItem($displayName,$name,$value,$details)
{
   ?>
   <tr class=admin valign=top>
      <td class=admin>&nbsp;<B><?=$displayName?></B>&nbsp;<BR><small><?=$details?></small></td>
      <td class=admin><input size=40 name="<?=$name?>" value="<?=$value?>"></td>
   </tr>
   <?
}


function writeConfigTextAreaItem($displayName,$name,$value,$details)
{
   ?>
   <tr class=admin valign=top>
      <td class=admin>&nbsp;<B><?=$displayName?></B>&nbsp;<BR><small><?=$details?></small></td>
      <td class=admin><textarea rows=5 cols=33 name="<?=$name?>"><?=$value?></textarea></td>
   </tr>
   <?
}


function writeConfigBooleanItem($displayName,$name,$value,$details)
{
   ?>
   <tr class=admin valign=top>
      <td class=admin>&nbsp;<B><?=$displayName?></B>&nbsp;<BR><small><?=$details?></small></td>
      <td class=admin><input type=checkbox name="<?=$name?>" <? if($value==true) echo("checked"); ?>></td>
   </tr>
   <?
}


function writeConfigNumericItem($displayName,$name,$value,$details,$units,$min,$max,$increment,$multiplier_off=false)
{
   if ($multiplier_off == true) {
       $multiplier = 1;
   } else {
       if($units=="Kilobytes") $multiplier = 1024;
       else if($units=="Megabytes") $multiplier = 1024*1024;
       else if($units=="Minutes") $multiplier = 60;
       else if($units=="Hours") $multiplier = 60*60;
       else if($units=="Days") $multiplier = 60*60*24;
       else if($units=="Weeks") $multiplier = 60*60*24*7;
       else $multiplier = 1;
   }
   $display_value=$value/$multiplier;
   ?>
   <tr class=admin valign=top>
      <td class=admin>&nbsp;<B><?=$displayName?></B>&nbsp;<BR><small><?=$details?></small></td>
      <td class=admin>
      <select name="<?=$name?>">
      <?
      for($i=$min;$i<=$max;$i=$i+$increment)
      {
         ?><option <? if($display_value==$i) echo("selected ");?>value=<?=$i*$multiplier?>>
         <? if($units=="Hours" && $i>0 && $min<0) echo("+");?> <?=$i?><?
      }
      ?>
      </select>
      <?=$units?></td>
   </tr>
   <?
}


function writeInfoItem($header,$details)
{
   ?>
   <tr class=admin>
      <td class=admin>&nbsp;<?=$header?></td><td class=admin> <?=$details?></td>
   </tr>
   <?
}

function writeWarningItem($warn_msg)
{
   ?>
     <tr class=admin>
        <td class=admin colspan=2><div class='error'>&nbsp;<?=$warn_msg?></div></td>
     </tr>
   <?
}
function writeConfigHeader($header,$details)
{
   ?>
   <tr class=admin>
      <th class=admin colspan=2>&nbsp;<?=$header?>&nbsp;</th>
   </tr>
   <tr class=admin>
      <td class=admin colspan=2><small><?=$details?></small></td>
   </tr>
   <?
}

ob_start();
$admin_table_width = "78%";

if($editor_session->isSessionOpen())
{
   global $system_config, $textLabels;

   if ( $system_config->check_spamwords & 1 OR $system_config->check_spamwords & 2 OR $system_config->check_spamwords & 4 ) {
       $link_array = array();

       // $link_array["editspam.php?spammode=story"] = "Edit Scan Story Wordlist";
       // $link_array["editspam.php?spammode=comment"] = "Edit Scan Comment Wordlist";
       $link_array["editspam.php?spammode=story"] = $textLabels['edit_scan_story_wordlist_linktext'];
       $link_array["editspam.php?spammode=comment"] = $textLabels['edit_scan_comment_wordlist_linktext'];

       writeAdminHeader("editspam.php?spammode=spamwords",$textLabels['edit_spam_story_wordlist_linktext'], $link_array);
       // writeAdminHeader("editspam.php?spammode=spamwords","Edit Spam Story Wordlist", $link_array);
   } else {
       writeAdminHeader("","");
   }

   if($editor_session->editor->allowedReadAccessTo("editspam"))
   {
      $display_config = true;
      if ( isset($_REQUEST['spammode']) && $_REQUEST['spammode'] != "")
      {
          if(isset($_REQUEST['save_spamwords']) && $_REQUEST['save_spamwords'] !=null)
          {
              if($editor_session->editor->allowedWriteAccessTo("editspam")) {
                  if(isset($_REQUEST['spamtext_block']) && $_REQUEST['spamtext_block'] !=null) {
                      if (isset($_REQUEST['spamfile']) && $_REQUEST['spamfile'] != "") 
                          $spam_filepath = $system_config->private_cache.$_REQUEST['spamfile'];
		      else
                          $spam_filepath = $system_config->private_cache."spamwords.txt";

		      $fh = fopen($spam_filepath,"w");
		      if ($fh != null) {
                          fwrite($fh,$_REQUEST['spamtext_block']);
                          fclose($fh);
                          echo "<P class='error'>".$textLabels['spamwords_saved_msg']."</P>";
                      }
	          }
	      }
	  }
          writeSpamwordsEntryScreen();
          $display_config = false;
      }
      else if(isset($_REQUEST['save']) && $_REQUEST['save'] !=null)
      {
         if($editor_session->editor->allowedWriteAccessTo("editspam"))
         {
            $system_config->check_spamwords = 0 ;
            if (isset($_REQUEST['check_spamwords']) && ($_REQUEST['check_spamwords'] == 'true' OR $_REQUEST['check_spamwords'] == 'on')) 
                $system_config->check_spamwords = $system_config->check_spamwords | 1 ;

            if (isset($_REQUEST['check_scanstory']) && ($_REQUEST['check_scanstory'] == 'true' OR $_REQUEST['check_scanstory'] == 'on')) 
                $system_config->check_spamwords = $system_config->check_spamwords | 2 ;

            if (isset($_REQUEST['check_scancomment']) && ($_REQUEST['check_scancomment'] == 'true' OR $_REQUEST['check_scancomment'] == 'on')) 
                $system_config->check_spamwords = $system_config->check_spamwords | 4 ;

            if (isset($_REQUEST['discard_spam']) && ($_REQUEST['discard_spam'] == 'true' OR $_REQUEST['discard_spam'] == 'on')) 
                $system_config->check_spamwords = $system_config->check_spamwords | 8 ;

            $system_config->updateConfigItem("check_spamwords",$system_config->check_spamwords);

	    // Now do same for spamlinks.
            if (isset($_REQUEST['check_spamlinks']) && $_REQUEST['check_spamlinks'] != "") {
                // $system_config->check_spamwords = $_REQUEST['check_spamlinks'];
		//$system_config->updateConfigItem("check_spamlinks",$system_config->check_spamlinks);
		$system_config->updateConfigItem("check_spamlinks", $_REQUEST['check_spamlinks']);
	    }
            // echo "<P class='error'>Configuration Saved!</P>"; 
            echo "<P class='error'>".$textLabels['configuration_saved_msg']."</P>";
            $system_config->load();
         }
         else $editor_session->writeNoWritePermissionError();
      }
      if ($display_config == true)
         writeConfigForm();
   }
   else $editor_session->writeNoReadPermissionError();
}
else $editor_session->writeNoSessionError();

include_once("adminfooter.inc");
?>

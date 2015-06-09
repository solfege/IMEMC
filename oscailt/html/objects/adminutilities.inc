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


function getBanLengthSelect($ban_author=false)
{
   $str = "<select name='ban_period'>";
   $opts = array(1 => "1 hour", 2 => "2 hours", 6 => "6 hours", 12 => "12 hours", 24 => "24 hours", 48 => "2 days", 72 => "3 days", 120 => "5 days", 168 => "1 week", 336 => "2 weeks");
   // For author bans only allow up to 2 weeks
   if ($ban_author == false) {
       $opts[720]  = "1 month";
       $opts[1440] = "2 months";
       $opts[2160] = "3 months";
       $opts[2880] = "4 months";
       $opts[4320] = "6 months";
       $opts[5760] = "8 months";
       $opts[8640] = "1 year";
       $opts[0] = "ever and ever"; 
   }

   foreach(array_keys($opts) as $k)
   {
      $str .= "<option value='$k'>".$opts[$k]."</option>\n";
   }
   $str .= "</select>";
   return $str;
}

function writeError($error)
{
   ?><BR><BR><font class=error><B><?=$error?></B></font><BR><BR><?
}

function writeConfirmBanHostBox( $ban_reason )
{
   global $system_config, $hostBanList, $monitorList, $editor_session, $OSCAILT_SCRIPT;
   $target_host = trim($_REQUEST['target_host']);
   if(strlen($target_host) == 0)
   {
         writeError("You must enter a valid hostname to block!");
         return;
   }
   $target_browser = trim($_REQUEST['target_browser']);
   if(strlen($target_browser) == 0)
   {
         writeError("You must enter a valid browser to block!");
         return;
   }
   if (isset($_REQUEST['target_ban_type']))
   	$target_ban_type = $_REQUEST['target_ban_type'];
   else
   	$target_ban_type = "";

   if($_REQUEST['hostsubpage'] == 'unban')
   {
      $action = 'unban';
      $extra_str = "";
      $intro = "Are you sure that you wish to remove the ban of ";
      $intro .= "( " .$target_host ." ) with browser (".$target_browser.") publishing on this site?";
   }
   else
   {
      $action = 'ban';
      $intro = "Are you sure that you wish to ban ";
      $intro .= "( " .$target_host ." ) with browser (".$target_browser.") publishing on this site?";

      $extra_str = "<P>Select the length of the ban: ". getBanLengthSelect()."</P>";
      $extra_str .= "<P align='center'>Enter Reason for ban: <br /><TEXTAREA rows=5 cols=30 name='ban_reason'>" . $ban_reason . "</TEXTAREA></P>";
   }
   ?>
   <table align=center>
   <form name="banned_data" action="<?=$OSCAILT_SCRIPT?>" method="post">
   <input type="hidden" name="hostsubpage" value="<?=$action?>">

   <input type="hidden" name="target_host" value="<?=$target_host?>">
   <input type="hidden" name="target_browser" value="<?=$target_browser?>">
   <input type="hidden" name="target_ban_type" value="<?=$target_ban_type?>">

   <tr>
      <td colspan=2 align=center><img src="graphics/caution.gif" align=middle><BR><BR><B><?=htmlspecialchars($intro, ENT_QUOTES)?></B><BR><BR>
      <?=$extra_str?></td>
   </tr>
   <tr>
      <td align=right width=50%><input type=submit name=cancel value="&lt;&lt; Cancel"></td>
      <td><input type=submit name=confirm value="Confirm <?=$action?> &gt;&gt;"></td>
   </tr>
   </form>
   </table>
   <?
}

function writeConfirmBanBox( $ban_reason, $ban_author=false)
{

   if ($ban_author == true) {
     global $system_config, $monitorList, $editor_session, $OSCAILT_SCRIPT;
     $target_author = trim($_REQUEST['target_author']);
     if($target_author == "")
     {
        writeError("You must enter a valid author name to block!");
        //writeMonitorPage();
        return;
     }
   }
   else
   {
      global $system_config, $ipBanList, $monitorList, $editor_session, $OSCAILT_SCRIPT;
      $target_ip = cleanseIP($_REQUEST['target_ip']);
      if($target_ip === false)
      {
         writeError("You must enter a valid IP address to block!");
         //writeMonitorPage();
         return;
      }
   }

   if($_REQUEST['subpage'] == 'unban')
   {
      $action = 'unban';
      $extra_str = "";
      $intro = "Are you sure that you wish to remove the ban on ";

      if ($ban_author == true)
          $intro .= "( " .$target_author ." ) publishing on this site?";
      else
          $intro .= $target_ip ."(".gethostbyaddr($target_ip).") publishing on this site?";
   }
   else
   {
      $action = 'ban';
      $intro = "Are you sure that you wish to ban ";
      if ($ban_author == true)
          $intro .= $target_author ." from publishing on this site?";
      else
          $intro .= $target_ip . "(".gethostbyaddr($target_ip).") from publishing on this site?";

      $extra_str = "<P>Select the length of the ban: ". getBanLengthSelect($ban_author)."</P>";
      $extra_str .= "<P align='center'>Enter Reason for ban: <br /><TEXTAREA rows=5 cols=30 name='ban_reason'>" . $ban_reason . "</TEXTAREA></P>";
   }
   ?>
   <table align=center>
   <form name="banned_data" action="<?=$OSCAILT_SCRIPT?>" method="post">
   <input type="hidden" name="subpage" value="<?=$action?>">
   <?

   if ($ban_author == true) {
       ?><input type="hidden" name="target_author" value="<?=$target_author?>"><?
   } else {
       ?><input type="hidden" name="target_ip" value="<?=$target_ip?>"><?
   }

   ?>
   <tr>
      <td colspan=2 align=center><img src="graphics/caution.gif" align=middle><BR><BR><B><?=htmlspecialchars($intro, ENT_QUOTES)?></B><BR><BR>
      <?=$extra_str?></td>
   </tr>
   <tr>
      <td align=right><input type=submit name=cancel value="&lt;&lt; Cancel"></td>
      <td><input type=submit name=confirm value="Confirm <?=$action?> &gt;&gt;"></td>
   </tr>
   </form>
   </table>
   <?
}
// Optional and mandatory word counts for content and title
function getRuleSet()
{
    $detectRules = array("storyspam" => array(
                           "Method 1" => array("content" => array("mandatory" => 2, "optional" => 5), "result"  => 3),
                           "Method 2" => array("content" => array("mandatory" => 3, "optional" => 4), "result"  => 3),
                           "Method 3" => array("content" => array("mandatory" => 2, "optional" => 5), "result"  => 3),
                           "Method 4" => array("content" => array("mandatory" => 2, "optional" => 7), "result"  => 3)),
                         "storybogus" => array(
                           "Method 1" => array("content" => array("mandatory" => 0, "optional" => 2),
                                               "title"   => array("mandatory" => 1, "optional" => 0),
                                               "result"  => 3),
                           "Method 2" => array("content" => array("mandatory" => 0, "optional" => 5),
                                               "title"   => array("mandatory" => 1, "optional" => 0),
                                               "result"  => 3),
                           "Method 3" => array("content" => array("mandatory" => 0, "optional" => 1),
                                               "title"   => array("mandatory" => 2, "optional" => 0),
                                               "result"  => 3)),
                         "commentbogus" => array(
                           "Method 1" => array("content" => array("optional" => 2), "result" => 1),
                           "Method 2" => array("content" => array("mandatory" => 1, "optional" => 0), "result" => 3),
                           "Method 3" => array("content" => array("mandatory" => 3, "optional" => 1), "result" => 3) 
                          ));
    return $detectRules;
}
function displayRules($ruleType)
{
    $detectRules = getRuleSet();
    $subRules = $detectRules[$ruleType];
    $rule_text = "";
    foreach (array_keys($subRules) as $rule_key ) {
	$rule_text .= $rule_key.": ";
        $rule_method = $subRules[$rule_key];

        if (isset($rule_method["content"])) {
            $c_result = 0;
	    $rule_text .= " Content check: ";
            foreach ($rule_method["content"] as $count_type => $threshold ) {
                if ($count_type == "mandatory") {
	            $rule_text .= $count_type." words > ".$threshold." and ";
                }
		if ($count_type == "optional") {
	            $rule_text .= $count_type." words > ".$threshold ;
                }
            }
        }

        if (isset($rule_method["title"])) {
	    $rule_text .= " or Title check: ";
            foreach ($rule_method["title"] as $count_type => $threshold ) {
                if ($count_type == "mandatory") {
	            $rule_text .= $count_type." words > ".$threshold." and ";
		}
		if ($count_type == "optional") {
	            $rule_text .= $count_type." words > ".$threshold ;
		}
            }
        }
	$rule_text .= "<BR>";
    }
    $rule_text .= "<BR>";
    return $rule_text;
}
function implementRules($ruleType, $mandword_count, $spamword_count, $t_mandword_count=null, $t_spamword_count=null)
{
    $detectRules = getRuleSet();
    $subRules = $detectRules[$ruleType];
    // echo "Passed: ".$ruleType. " ".$mandword_count. " ".$spamword_count;

    foreach (array_keys($subRules) as $rule_key ) {
        $rule_method = $subRules[$rule_key];

        if (isset($rule_method["result"])) $result_criteria = $rule_method["result"];
        if (isset($rule_method["content"])) {
            $c_result = 0;
            foreach ($rule_method["content"] as $count_type => $threshold ) {
                if ($count_type == "mandatory") {
                    if ($mandword_count > $threshold) $c_result = $c_result | 1;
                }
		if ($count_type == "optional") {
                    if ($spamword_count > $threshold) $c_result = $c_result | 2;
                }
                if ($c_result == $result_criteria) break;
            }
        }

        if (isset($rule_method["title"])) {
            $t_result = 0;
            foreach ($rule_method["title"] as $count_type => $threshold ) {
                if ($count_type == "mandatory") {
                    if ($t_mandword_count > $threshold) $t_result = $t_result | 1;
		}
		if ($count_type == "optional") {
                    if ($t_spamword_count > $threshold) $t_result = $t_result | 2;
		}
                if ($c_result == $result_criteria) break;
            }
        }

        if (isset($rule_method["title"])) {
            if ($t_result == $result_criteria OR $c_result == $result_criteria) return $rule_key;
	} else {
            if ($c_result == $result_criteria) return $rule_key;
	}
    }
    return "";

}

function storySpamCheck($storyObj, $publish_mode=true, $content_type="story")
{
    list ($spamword_count, $foundwords, $mandword_count, $foundmandatory ) = spamCheck($storyObj->story_content);
    /*
    $is_it_spam = 0;
    if ($spamword_count > 5 && $mandword_count > 2 ) $is_it_spam = 1;
    else if ($spamword_count > 4 && $mandword_count > 3 ) $is_it_spam = 2;
    else if ($spamword_count > 5 && $mandword_count > 2 ) $is_it_spam = 3;
    else if ($spamword_count > 7 && $mandword_count > 0 ) $is_it_spam = 4;
    */

    $method_used = implementRules("storyspam", $mandword_count, $spamword_count);

    if ($publish_mode == true) {
        if (strlen($method_used) > 0) {
   	    logMessage("Publish (".$content_type." by ".$storyObj->author_name.") failure. Reason: Contained ".$mandword_count." spam words. Detected with ".$method_used, "sitelog.txt");
   	    logSpammerMessage("------------- spamword capture ----------\nFrom: ".$_SERVER['REMOTE_ADDR'], "spamword_capture.txt", false);
   	    logSpammerMessage("Spam words found: ".$foundwords, "spamword_capture.txt", false);
            logSpammerStoryString($storyObj, true, "spamword_capture.txt" );
            return false;
        } else {
            return true;
        } 
    } else {
        $test_results = "";
	$test_results .= displayRules("storyspam");
        if (strlen($method_used) > 0) {
            $test_results .= "<b>Story Id (".$storyObj->story_id.") detected as spam.</b> Reason: Contained spam words. Method used = ".$method_used;
            $test_results .= "<BR>";
            $test_results .= "<b>Mandatory spam words found:</b> ".$foundmandatory . " (".$mandword_count.")";
            $test_results .= "<BR>";
            $test_results .= "<b>Optional spam words found:</b> ".$foundwords. " (".$spamword_count.")";
            $test_results .= "<BR>";
            $test_results .= getFormattedStoryString($storyObj);
        } else {
	    $test_results = "<b>Story id: ".$storyObj->story_id." was NOT detected as spam </b><BR>";
	    $test_results .= "<b>No of mandatory spam words found: ".$mandword_count." </b><BR>";
	    $test_results .= "<b>No of optional spam words found: ".$spamword_count." </b><BR>";
            $test_results .= getFormattedStoryString($storyObj);
       }
       return $test_results;
    }
}
function storyBogusCheck($storyObj, $publish_mode=true, $content_type="story", $spamfile="story_scanner.txt")
{
    // Check title and the story body
    list ($t_spamword_count, $t_foundwords, $t_mandword_count, $t_foundmandatory ) = spamCheck($storyObj->story_title,true, $spamfile);
    list ($spamword_count, $foundwords, $mandword_count, $foundmandatory ) = spamCheck($storyObj->story_content,false, $spamfile);

    // echo "spamwor_count ".$spamword_count ." spamwords ".$foundwords;
    // echo " Mandatory ".$mandword_count ." mandwords ".$foundmandatory. " <BR>";
    $method_used = implementRules("storybogus", $mandword_count, $spamword_count, $t_mandword_count, $t_spamword_count);

    /*
    $is_it_spam = 0;
    if (($spamword_count > 2 && $mandword_count > 0) or ($t_mandword_count > 1 && $t_spamword_count > 0)) $is_it_spam = 1;
    else if (($spamword_count > 5 && $mandword_count > 0) or ($t_mandword_count > 1 && $t_spamword_count > 0)) $is_it_spam = 2;
    else if (($spamword_count > 1) && ($t_mandword_count > 2 && $t_spamword_count > 0)) $is_it_spam = 3;
    */

    if ($publish_mode == true) {
        if (strlen($method_used) > 0) {
   	    logMessage("Publish of possible bogus (".$content_type." by ".$storyObj->author_name.") detected. Contained combination of spammer words determined by ".$method_used, "sitelog.txt");
   	    logSpammerMessage("\n------------- spammer capture by ".$method_used." ---------- \nPosted from: ".$_SERVER['REMOTE_ADDR'], "spammer_capture.txt", false);
   	    logSpammerMessage("\nSpam words found in title: ".$t_foundwords, "spammer_capture.txt",false);
   	    logSpammerMessage("\nMandatory words found in title: ".$t_foundmandatory, "spammer_capture.txt",false);
   	    logSpammerMessage("\nSpam words found in content: ".$foundwords, "spammer_capture.txt",false);
   	    logSpammerMessage("\nMandatory words found in content: ".$foundmandatory, "spammer_capture.txt",false);
            logSpammerStoryString($storyObj, true, "spammer_capture.txt");
            $editorStatusList = new PublishState();
            $editorStatusList->load();
            $editorStatusList->add("system", "post", time(),"Publish of bogus story attempted by author ".$storyObj->author_name.". Check newswire for title: ".$storyObj->story_title);
            $editorStatusList->save();
	}
	return false;
    } else {
	$test_results = "";
	$test_results .= displayRules("storybogus");
        if (strlen($method_used) > 0) {
            $test_results .= "<b>Story Id (".$storyObj->story_id.") detected as possibly bogus or malicious.</b> Reason: Contained spam words. Method parameter = ".$method_used;
            $test_results .= "<BR>Spam words found in title: ".$t_foundwords. "</b> <BR>";
            $test_results .= "Mandatory words found in title: ".$t_foundmandatory . "</b> <BR>";
            $test_results .= "Spam words found in content: ".$foundwords . "</b> <BR>";
            $test_results .= "Mandatory words found in content: ".$foundmandatory ."</b> <BR>";
            $test_results .= "<BR>";
            $test_results .= "<b>Mandatory spam words found:</b> ".$foundmandatory. " (".$mandword_count.")";
            $test_results .= "<BR>";
            $test_results .= "<b>Optional spam words found:</b> ".$foundwords. " (".$spamword_count.")";
            $test_results .= "<BR>";
	} else {
	    $test_results = "<b>Story id: ".$storyObj->story_id." was NOT detected as bogus or malicious </b><BR>";
	    $test_results .= "<b>No of mandatory spam words found in title: ".$t_mandword_count." </b><BR>";
	    $test_results .= "<b>No of optional spam words found in title: ".$t_spamword_count." </b><BR>";
	    $test_results .= "<b>No of mandatory spam words found in content: ".$mandword_count." </b><BR>";
	    $test_results .= "<b>No of optional spam words found in content: ".$spamword_count." </b><BR>";
       }
       $test_results .= getFormattedStoryString($storyObj);
       return $test_results;
    }
}
function commentBogusCheck($commentObj, $publish_mode=true, $content_type="comment", $spamfile="comment_scanner.txt")
{
    list ($spamword_count, $foundwords, $mandword_count, $foundmandatory ) = spamCheck($commentObj->comment_content,false, $spamfile);

    $method_used = implementRules("commentbogus", $mandword_count, $spamword_count);

    if ($publish_mode == true) {
        if (strlen($method_used) > 0 ) {
            $editorStatusList = new PublishState();
            $editorStatusList->load();
            $editorStatusList->add("system", "post", time(),"Publish of possible bogus or malicious comment by author ".$commentObj->author_name);
            $editorStatusList->save();
   	    logSpammerMessage("\n------------- scanner capture ----------\nFrom: ".$_SERVER['REMOTE_ADDR'], "scanner_capture.txt", false);
   	    logSpammerMessage("\nSpam words found: ".$foundwords, "scanner_capture.txt", false);
   	    logSpammerMessage("\nTitle: ".$commentObj->comment_title, "scanner_capture.txt", false);
   	    logSpammerMessage("\nby: ".$commentObj->author_name, "scanner_capture.txt", false);
   	    logSpammerMessage("\nContent: ".$commentObj->comment_content, "scanner_capture.txt",false);
        }
        return false;
    } else {
	$test_results = "";
	$test_results .= displayRules("commentbogus");
        if (strlen($method_used) > 0 ) {
            $test_results .= "<b>Comment detected as possibly bogus or malicious.</b> Reason: Contained 2 or more spam words. Method parameter = ". $method_used;
            $test_results .= "<BR>Spam words found in content: ".$foundwords . "(".$mandword_count.")</b> <BR>";
            $test_results .= "Mandatory words found in content: ".$foundmandatory . "(".$spamword_count.")</b> <BR>";
            $test_results .= "<BR>";
            $test_results .= "<b>Mandatory spam words found:</b> ".$foundmandatory;
            $test_results .= "<BR>";
            $test_results .= "<b>Optional spam words found:</b> ".$foundwords;
            $test_results .= "<BR>";
	} else {
	    $test_results = "<b>Comment id: ".$commentObj->comment_id." was NOT detected as bogus or malicious </b><BR>";
	    $test_results .= "<b>No of mandatory spam words found in content: ".$mandword_count." </b><BR>";
	    $test_results .= "<b>No of optional spam words found in content: ".$spamword_count." </b><BR>";
       }
       $test_results .= getFormattedCommentString($commentObj, false);
       return $test_results;
    }
}
// Checking links. Note returning false means reject it.
function checkStoryLinks($storyObj, $link_threshold, $publish_mode=true)
{
    $link_total =  preg_match_all("!((itms|http|https|ftp):/{2}[\w\-]{2,}\.[\w\-\.]{2,}[/\w\-\.\,\;\+\:\~\?\&\=\#\%]*[/\w\-\?\&\=\#]+)!e", $storyObj->story_content, $t_matches);

    // echo "Spam links check. Limit = ".$link_threshold . " found ". $link_total . "<BR>";
    if ($publish_mode == true) {
        if ($link_total > $link_threshold) {
   	    logMessage("Publish (".$content_type." by ".$storyObj->author_name.") failure. Reason: Contained more than ".$link_threshold. " links.", "sitelog.txt");
   	    logSpammerMessage("------------- spamword capture ----------\nFrom: ".$_SERVER['REMOTE_ADDR'], "spamword_capture.txt", false);
   	    logSpammerMessage("No of links found: ".$link_total, "spamword_capture.txt", false);
            logSpammerStoryString($storyObj, true, "spamword_capture.txt");
            return false;
        } else {
            return true;
        } 
    } else {
        $test_results = "";
        if ($link_total > $link_threshold) {
            $test_results .= "<b>Story detected as spam.</b> Reason: Contained more than ".$link_threshold." links.";
            $test_results .= "<BR>";
            $test_results .= "<b>No of links found:</b> ".$link_total;
            $test_results .= "<BR>";
            $test_results .= getFormattedStoryString($storyObj);
        } else {
	    $test_results = "<b>Story id: ".$storyObj->story_id." was NOT detected as spam </b><BR>";
            $test_results .= "<b>No of links found:</b> ".$link_total;
            $test_results .= getFormattedStoryString($storyObj);
       }
       return $test_results;
    }
}
// Function to cover the main parts of a story
function logSpammerStoryString($storyObj, $show_summary=true, $log_filename)
{
    // Always log as spam
    logSpammerMessage("\nTitle: ".$storyObj->story_title, $log_filename, false);
    logSpammerMessage("\nBy: ".$storyObj->author_name, $log_filename, false);
    if ($show_summary == true) 
        logSpammerMessage("\nSummary: ".$storyObj->story_summary, $log_filename, false);

    logSpammerMessage("\nContent: ".$storyObj->story_content, $log_filename, false);
}

// Format a story into a string. 
function getFormattedStoryString($storyObj, $show_summary=true)
{
    $format_string = "";
    $format_string .= "<BR><b>Title:</b> ".$storyObj->story_title;
    $format_string .= "<BR><b>by:</b> ".$storyObj->author_name;

    if ($show_summary == true) {
        $format_string .= "<BR><b>Summary</b>:" .$storyObj->story_summary;
    }
    $format_string .= "<BR>";
    $format_string .= "<BR><b>Content: </b>".$storyObj->story_content;

    return $format_string;
}
// Format a comment into a string. 
function getFormattedCommentString($commentObj, $publish_mode=true)
{
    $format_string = "";
    if ($publish_mode == true) {
        // To be finished
        $format_string = "";
        $format_string .= "Title: ".$commentObj->comment_title;
        $format_string .= "by: ".$commentObj->author_name;
        $format_string .= $commentObj->comment_summary;
        $format_string .= "Content: ".$commentObj->comment_content;
    } else {
        $format_string .= "<BR><b>Title:</b> ".$commentObj->comment_title;
	$format_string .= "<BR><b>by:</b> ".$commentObj->author_name;
        $format_string .= "<BR>";
        $format_string .= "<BR><b>Content: </b>".$commentObj->comment_content;
    }

    return $format_string;
}
?>

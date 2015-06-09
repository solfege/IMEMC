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

$OSCAILT_SCRIPT = "bulkdelete.php";
require_once("oscailt_init.inc");
require_once("objects/statistics.inc");
require_once("objects/story.inc");
require_once("objects/attachment.inc");

addToPageTitle('Bulk Deletion');

function writeDeleteForm()
{
   // Figure out when the site started and present a year for each year.
   $StartdateArray = getStartDate();
   $first_year = $StartdateArray['year'];
   $first_month = $StartdateArray['month'];
   $this_year = strftime("%Y");

   ?>
   <table align=center class=admin>
   <TR class=admin>
      <th class=admin colspan=5 align=center>Bulk Delete</th>
   </tr>
   <tr class=admin>
      <td class=admin colspan=5>The form below allows you to delete all attachments, comments, and stories 
      (excluding features) that have remained hidden for a specified period of time.<BR><BR>
      The purpose of this is to remove hidden items from the server which, after review by the
      editorial group, have not been reinstated.<BR><BR>
      If there are many hidden stories, comments or attachments, try to delete them in time blocks to avoid
      excessive memory consumption as Oscailt will attempt to load each into memory and may abort if the 
      memory limit is exceeded.<BR><BR>
      <small>Note: The first story on this site was published in <?=$first_month?> <?=$first_year?> </small>
      </td>
   </tr>
   <?
      $fourweekstats = new Statistics();
      $fourweekstats->setTimePostedUpperLimit("12am 4 weeks ago");
      $fourweekstats->hidden=1;
      $threeweekstats = new Statistics();
      $threeweekstats->setTimePostedUpperLimit("12am 3 weeks ago");
      $threeweekstats->hidden=1;
      $twoweekstats = new Statistics();
      $twoweekstats->setTimePostedUpperLimit("12am 2 weeks ago");
      $twoweekstats->hidden=1;
      $oneweekstats = new Statistics();
      $oneweekstats->setTimePostedUpperLimit("12am 1 week ago");
      $oneweekstats->hidden=1;
   ?>
   <tr class=admin>
      <th class=admin align=center colspan=5>Hidden Items</th>
   </tr>
   <tr class=admin>
      <th class=admin align=center rowspan=2>Type</th>
      <th class=admin align=center colspan=4>Amount Older Than ...</th>
   </tr>
   <tr class=admin>
      <th class=admin align=center>1 Week</th>
      <th class=admin align=center>2 Weeks</th>
      <th class=admin align=center>3 Weeks</th>
      <th class=admin align=center>4 Weeks</th>
   </tr>
   <tr class=admin>
      <td class=admin>All <small>(excl. features)</small></td>
      <td class=admin align=center><?=$oneweekstats->getNonFeatureItemCount()?></td>
      <td class=admin align=center><?=$twoweekstats->getNonFeatureItemCount()?></td>
      <td class=admin align=center><?=$threeweekstats->getNonFeatureItemCount()?></td>
      <td class=admin align=center><?=$fourweekstats->getNonFeatureItemCount()?></td>
   </tr>
   <tr class=admin>
      <td class=admin><img src="graphics/subgrouping.gif"> Stories <small>(excl. features)</small></td>
      <td class=admin align=center><?=$oneweekstats->getNonFeatureStoryCount()?></td>
      <td class=admin align=center><?=$twoweekstats->getNonFeatureStoryCount()?></td>
      <td class=admin align=center><?=$threeweekstats->getNonFeatureStoryCount()?></td>
      <td class=admin align=center><?=$fourweekstats->getNonFeatureStoryCount()?></td>
   </tr>
   <tr class=admin>
      <td class=admin><img src="graphics/subgrouping.gif"> Comments</td>
      <td class=admin align=center><?=$oneweekstats->getCommentCount()?></td>
      <td class=admin align=center><?=$twoweekstats->getCommentCount()?></td>
      <td class=admin align=center><?=$threeweekstats->getCommentCount()?></td>
      <td class=admin align=center><?=$fourweekstats->getCommentCount()?></td>
   </tr>
   <tr class=admin>
      <td class=admin><img src="graphics/subgrouping.gif"> Attachments</td>
      <td class=admin align=center><?=$oneweekstats->getAttachmentCount()?></td>
      <td class=admin align=center><?=$twoweekstats->getAttachmentCount()?></td>
      <td class=admin align=center><?=$threeweekstats->getAttachmentCount()?></td>
      <td class=admin align=center><?=$fourweekstats->getAttachmentCount()?></td>
   </tr>
   <tr class=admin>
      <td class=admin>&nbsp;&nbsp;<img src="graphics/subgrouping.gif"> Image</td>
      <td class=admin align=center><?=$oneweekstats->getImageAttachmentCount()?></td>
      <td class=admin align=center><?=$twoweekstats->getImageAttachmentCount()?></td>
      <td class=admin align=center><?=$threeweekstats->getImageAttachmentCount()?></td>
      <td class=admin align=center><?=$fourweekstats->getImageAttachmentCount()?></td>
   </tr>
   <tr class=admin>
      <td class=admin>&nbsp;&nbsp;<img src="graphics/subgrouping.gif"> Video</td>
      <td class=admin align=center><?=$oneweekstats->getVideoAttachmentCount()?></td>
      <td class=admin align=center><?=$twoweekstats->getVideoAttachmentCount()?></td>
      <td class=admin align=center><?=$threeweekstats->getVideoAttachmentCount()?></td>
      <td class=admin align=center><?=$fourweekstats->getVideoAttachmentCount()?></td>
   </tr>
   <tr class=admin>
      <td class=admin>&nbsp;&nbsp;<img src="graphics/subgrouping.gif"> Audio</td>
      <td class=admin align=center><?=$oneweekstats->getAudioAttachmentCount()?></td>
      <td class=admin align=center><?=$twoweekstats->getAudioAttachmentCount()?></td>
      <td class=admin align=center><?=$threeweekstats->getAudioAttachmentCount()?></td>
      <td class=admin align=center><?=$fourweekstats->getAudioAttachmentCount()?></td>
   </tr>
   <tr class=admin>
      <td class=admin>&nbsp;&nbsp;<img src="graphics/subgrouping.gif"> Miscellaneous</td>
      <td class=admin align=center><?=$oneweekstats->getMiscellaneousAttachmentCount()?></td>
      <td class=admin align=center><?=$twoweekstats->getMiscellaneousAttachmentCount()?></td>
      <td class=admin align=center><?=$threeweekstats->getMiscellaneousAttachmentCount()?></td>
      <td class=admin align=center><?=$fourweekstats->getMiscellaneousAttachmentCount()?></td>
   </tr>
   <tr class=admin>
      <td class=admin colspan=5><small>&nbsp;<B>NOTE</B>: Attachment figures do not include those which are hidden as a result of their parent story or comment being hidden&nbsp;</small></td>
   </tr>

   <?
      $twoYearStats = new Statistics();
      $twoYearStats->setTimePostedUpperLimit("12am 2 years ago");
      $twoYearStats->hidden=1;
      $oneYearStats = new Statistics();
      $oneYearStats->setTimePostedUpperLimit("12am 1 year ago");
      $oneYearStats->hidden=1;
      $sixMonthStats = new Statistics();
      $sixMonthStats->setTimePostedUpperLimit("12am 6 months ago");
      $sixMonthStats->hidden=1;
      $threeMonthStats = new Statistics();
      $threeMonthStats->setTimePostedUpperLimit("12am 3 months ago");
      $threeMonthStats->hidden=1;
   ?>
   <tr class=admin>
      <th class=admin align=center colspan=5>Hidden Items</th>
   </tr>
   <tr class=admin>
      <th class=admin align=center rowspan=2>Type</th>
      <th class=admin align=center colspan=4>Amount Older Than ...</th>
   </tr>
   <tr class=admin>
      <th class=admin align=center>3 Months</th>
      <th class=admin align=center>6 Months</th>
      <th class=admin align=center>1 Year</th>
      <th class=admin align=center>2 Years</th>
   </tr>
   <tr class=admin>
      <td class=admin>All <small>(excl. features)</small></td>
      <td class=admin align=center><?=$threeMonthStats->getNonFeatureItemCount()?></td>
      <td class=admin align=center><?=$sixMonthStats->getNonFeatureItemCount()?></td>
      <td class=admin align=center><?=$oneYearStats->getNonFeatureItemCount()?></td>
      <td class=admin align=center><?=$twoYearStats->getNonFeatureItemCount()?></td>
   </tr>
   <tr class=admin>
      <td class=admin><img src="graphics/subgrouping.gif"> Stories <small>(excl. features)</small></td>
      <td class=admin align=center><?=$threeMonthStats->getNonFeatureStoryCount()?></td>
      <td class=admin align=center><?=$sixMonthStats->getNonFeatureStoryCount()?></td>
      <td class=admin align=center><?=$oneYearStats->getNonFeatureStoryCount()?></td>
      <td class=admin align=center><?=$twoYearStats->getNonFeatureStoryCount()?></td>
   </tr>
   <tr class=admin>
      <td class=admin><img src="graphics/subgrouping.gif"> Comments</td>
      <td class=admin align=center><?=$threeMonthStats->getCommentCount()?></td>
      <td class=admin align=center><?=$sixMonthStats->getCommentCount()?></td>
      <td class=admin align=center><?=$oneYearStats->getCommentCount()?></td>
      <td class=admin align=center><?=$twoYearStats->getCommentCount()?></td>
   </tr>
   <tr class=admin>
      <td class=admin><img src="graphics/subgrouping.gif"> Attachments</td>
      <td class=admin align=center><?=$threeMonthStats->getAttachmentCount()?></td>
      <td class=admin align=center><?=$sixMonthStats->getAttachmentCount()?></td>
      <td class=admin align=center><?=$oneYearStats->getAttachmentCount()?></td>
      <td class=admin align=center><?=$twoYearStats->getAttachmentCount()?></td>
   </tr>
   <tr class=admin>
      <td class=admin>&nbsp;&nbsp;<img src="graphics/subgrouping.gif"> Image</td>
      <td class=admin align=center><?=$threeMonthStats->getImageAttachmentCount()?></td>
      <td class=admin align=center><?=$sixMonthStats->getImageAttachmentCount()?></td>
      <td class=admin align=center><?=$oneYearStats->getImageAttachmentCount()?></td>
      <td class=admin align=center><?=$twoYearStats->getImageAttachmentCount()?></td>
   </tr>
   <tr class=admin>
      <td class=admin>&nbsp;&nbsp;<img src="graphics/subgrouping.gif"> Video</td>
      <td class=admin align=center><?=$threeMonthStats->getVideoAttachmentCount()?></td>
      <td class=admin align=center><?=$sixMonthStats->getVideoAttachmentCount()?></td>
      <td class=admin align=center><?=$oneYearStats->getVideoAttachmentCount()?></td>
      <td class=admin align=center><?=$twoYearStats->getVideoAttachmentCount()?></td>
   </tr>
   <tr class=admin>
      <td class=admin>&nbsp;&nbsp;<img src="graphics/subgrouping.gif"> Audio</td>
      <td class=admin align=center><?=$threeMonthStats->getAudioAttachmentCount()?></td>
      <td class=admin align=center><?=$sixMonthStats->getAudioAttachmentCount()?></td>
      <td class=admin align=center><?=$oneYearStats->getAudioAttachmentCount()?></td>
      <td class=admin align=center><?=$twoYearStats->getAudioAttachmentCount()?></td>
   </tr>
   <tr class=admin>
      <td class=admin>&nbsp;&nbsp;<img src="graphics/subgrouping.gif"> Miscellaneous</td>
      <td class=admin align=center><?=$threeMonthStats->getMiscellaneousAttachmentCount()?></td>
      <td class=admin align=center><?=$sixMonthStats->getMiscellaneousAttachmentCount()?></td>
      <td class=admin align=center><?=$oneYearStats->getMiscellaneousAttachmentCount()?></td>
      <td class=admin align=center><?=$twoYearStats->getMiscellaneousAttachmentCount()?></td>
   </tr>
   <tr class=admin>
      <td class=admin colspan=5><small>&nbsp;<B>NOTE</B>: Attachment figures do not include those which are hidden as a result of their parent story or comment being hidden&nbsp;</small></td>
   </tr>
   <?
   // Figure out when the site started and present a year for each year.
   $StartdateArray = getStartDate();
   $first_year = $StartdateArray['year'];
   $this_year = strftime("%Y");

   ?>
   <form name="bulk_delete_form" action="bulkdelete.php" method=post>
   <tr class=admin>
      <td class=admin align=center colspan=5>
      <BR>
      <select name=delete>
      <?
         if ($first_year < $this_year) {
             for ($choice_year = $first_year;$choice_year < ($this_year-1);$choice_year++) {
                 $t_yrs_ago = $this_year - $choice_year;
		 ?>
                 <option value="12am <?=$t_yrs_ago?> year ago">Delete all hidden items over <?=$t_yrs_ago?> years old (<?=$choice_year?>)</option>
		 <?
	     }
	 }
      ?>
         <option value="12am 1 year ago">Delete all hidden items over 1 year old</option>
         <option value="12am 9 months ago">Delete all hidden items over 9 months old</option>
         <option value="12am 6 months ago">Delete all hidden items over 6 months old</option>
         <option value="12am 3 month ago">Delete all hidden items over 3 months old</option>
         <option value="12am 2 month ago">Delete all hidden items over 2 months old</option>
         <option selected value="12am 4 weeks ago">Delete all hidden items over 4 weeks old</option>
         <option value="12am 3 weeks ago">Delete all hidden items over 3 weeks old</option>
         <option value="12am 2 weeks ago">Delete all hidden items over 2 weeks old</option>
         <option value="12am 1 week ago">Delete all hidden items over 1 week old</option>
      </select>
      <BR><BR>
      <input type=submit value="Perform Bulk Delete &gt;&gt;">
      <BR><BR>
      </td>
   </tr>
   </form>
   </table>
   <?
}


function writeError($error)
{
   ?><BR><BR><font class=error><B><?=$error?></B></font><BR><BR><?
}

function writeConfirmDeleteBox()
{
   global $system_config;
   $time_posted_upper_limit = strtotime($_REQUEST['delete'])+$system_config->timezone_offset;
   ?>
   <table align=center>
   <form action="bulkdelete.php" method=post>
   <input type=hidden name=delete value="<?=$_REQUEST['delete']?>"><?
   ?>
   <tr>
      <td colspan=2 align=center><img src="graphics/caution.gif" align=middle><BR><BR><B>Are you sure you delete all hidden items published before<BR><?=date("l, M j Y, g:ia",$time_posted_upper_limit)?>?</B><BR><BR></td>
   </tr>
   <tr>
      <td align=right><input type=submit name=cancel value="&lt;&lt; Cancel"></td>
      <td><input type=submit name=confirm value="Delete &gt;&gt;"></td>
   </tr>
   </form>
   </table>
   <?
}


function deleteAllHiddenStories($time_posted_upper_limit)
{
   global $prefix, $system_config, $dbconn;
   $result = sql_query("SELECT story_id FROM ".$prefix."_stories WHERE type_id!=1 and hidden='1' and UNIX_TIMESTAMP(time_posted) < ".($time_posted_upper_limit-$system_config->timezone_offset), $dbconn, 2);
   checkForError($result);

   $stories = array();
   if(sql_num_rows( $result ) > 0)
   {
      for ($i=0; $i<sql_num_rows( $result ); $i++)
      {
         $story = new Story();
         list($story->story_id) = sql_fetch_row($result, $dbconn);
         array_push($stories,$story);
      }
   }

   $t_comments = 0;
   $t_attachments = 0;
   for($i=0;$i<count($stories);$i++)
   {
      $stories[$i]->getAttachments();
      $t_comments = $t_comments + $stories[$i]->getNumberOfComments();
      $t_attachments = $t_attachments + count($stories[$i]->attachments);

      $stories[$i]->deleteAttachments();
      $stories[$i]->deleteComments();
      $stories[$i]->delete();
   }

   $t_stories = count($stories);


   return array($t_stories, $t_comments, $t_attachments);
}


function deleteAllHiddenComments($time_posted_upper_limit)
{
   global $prefix, $system_config, $dbconn;
   $result = sql_query("SELECT comment_id FROM ".$prefix."_comments WHERE hidden='1' and UNIX_TIMESTAMP(time_posted) < ".($time_posted_upper_limit-$system_config->timezone_offset), $dbconn,2);
   checkForError($result);

   $comments = array();
   if(sql_num_rows( $result ) > 0)
   {
      for ($i=0; $i<sql_num_rows( $result ); $i++)
      {
         $comment = new Comment();
         list($comment->comment_id) = sql_fetch_row($result, $dbconn);
         array_push($comments,$comment);
      }
   }
   $t_attachments = 0;
   for($i=0;$i<count($comments);$i++)
   {
      $comments[$i]->getAttachments();
      $t_attachments = $t_attachments + count($comments[$i]->attachments);
      $comments[$i]->deleteAttachments();
      $comments[$i]->delete();
   }

   $t_comments = count($comments);

   return array($t_comments, $t_attachments);
}


function deleteAllHiddenAttachments($time_posted_upper_limit)
{
   global $prefix, $system_config, $dbconn;
   $result = sql_query("SELECT attachment_id FROM ".$prefix."_attachments WHERE hidden='1' and UNIX_TIMESTAMP(time_posted) < ".($time_posted_upper_limit-$system_config->timezone_offset), $dbconn,2);
   checkForError($result);

   $attachments = array();
   if(sql_num_rows( $result ) > 0)
   {
      for ($i=0; $i<sql_num_rows( $result ); $i++)
      {
         $attachment = new Attachment();
         list($attachment->attachment_id) = sql_fetch_row($result, $dbconn);
         array_push($attachments,$attachment);
      }
   }
   for($i=0;$i<count($attachments);$i++)
   {
      $attachments[$i]->deleteFileIfSingleReference();
      $attachments[$i]->delete();
   }

   return count($attachments);
}
// These were copied from calendar.inc with a slight change
function getStartDate()
{
      global $prefix, $dbconn,$system_config;
      $stmt = "SELECT UNIX_TIMESTAMP(MIN(time_posted)) FROM ".$prefix."_stories";
      $result = sql_query($stmt, $dbconn, 5);
      checkForError($result);
      $tmp_startdate = 0;
      if(sql_num_rows( $result ) > 0)
      {
         list($tmp_startdate) = sql_fetch_row($result, $dbconn);
      }
      return getdate($tmp_startdate+$system_config->timezone_offset);
}



ob_start();

if($editor_session->isSessionOpen())
{
   writeAdminHeader("stats.php","View Statistics");

   if($editor_session->editor->allowedReadAccessTo("bulkdelete") )
   {
      if(isset($_REQUEST['delete']) && isset($_REQUEST['confirm']) && $_REQUEST['delete']!=null && $_REQUEST['confirm']!=null)
      {
         //perform delete
         if($editor_session->editor->allowedWriteAccessTo("bulkdelete"))
         {
            $time_posted_upper_limit = strtotime($_REQUEST['delete'])+$system_config->timezone_offset;
	    // Item=NULL, item_id = time criteria, type = Stories, Comments, Attachments, action = bulk delete
	    $time_str = $_REQUEST['delete'] . " (".strftime("%a %e %b %Y",$time_posted_upper_limit).")";

	    list($deletedStoryCount, $t_deletedCommentCount, $ts_deletedAttachmentCount) = deleteAllHiddenStories($time_posted_upper_limit);
	    $deletedStoryCount = 1033;
	    $t_deletedCommentCount =8944;
	    $ts_deletedAttachmentCount = 3003;
	    $t_comment_total = $t_deletedCommentCount;
	    $t_attach_total = $ts_deletedAttachmentCount;
	    $desc_str = "Bulk delete of ". $deletedStoryCount . " stories ".$t_deletedCommentCount." comments and ".$ts_deletedAttachmentCount." attachments before ".$time_str;
	    logAction(NULL, 0, "story", "bulk delete", $desc_str);


	    list($deletedCommentCount, $tc_deletedAttachmentCount)=deleteAllHiddenComments($time_posted_upper_limit);
	    $deletedCommentCount = 2344;
	    $tc_deletedAttachmentCount=903;
	    $t_comment_total = $t_comment_total + $deletedCommentCount;
	    $t_attach_total = $t_attach_total + $tc_deletedAttachmentCount;

	    $desc_str = "Bulk delete of ". $deletedCommentCount . " comments and ".$tc_deletedAttachmentCount." attachments before ".$time_str;
	    logAction(NULL, 0, "comment", "bulk delete", $desc_str);

            $deletedAttachmentCount = deleteAllHiddenAttachments($time_posted_upper_limit);
	    $deletedAttachmentCount=383;
	    $t_attach_total = $t_attach_total + $deletedAttachmentCount;
	    $desc_str = "Bulk delete of ". $deletedAttachmentCount . " attachments before ".$time_str;
	    logAction(NULL, 0, "attachment", "bulk delete", $desc_str);

            echo("<p align=center><font class=editornotice><B> All hidden items published before ".date("l, M j Y, g:ia",$time_posted_upper_limit)." have been deleted!</B><BR></font></p>");

	    ?>
	    <table align=center class=admini width=80%>
            <TR class=admin>
               <th class=admin colspan=2 align=center>&nbsp;Number of Stories, Comments &amp; Attachments deleted&nbsp;</th>
               <th class=admin colspan=1 align=center>&nbsp;Details&nbsp;</th>
            </tr>
            <tr class=admin>
               <td width="35%" class=admin_highlight><B><font class=editornotice>Total stories deleted:</font></b></TD>
	       <TD class=admin_highlight align=right> <?=$deletedStoryCount?></TD>
	       <TD class=admin_highlight align=right> <?=$deletedStoryCount?> stories with <?=$t_deletedCommentCount?> comments and <?=$ts_deletedAttachmentCount?> attachments</TD>
            </tr>
            <tr class=admin>
               <td class=admin_highlight><B><font class=editornotice>Total comments deleted:</font></b></TD>
	       <TD class=admin_highlight align=right><?=$t_comment_total?></TD>
	       <TD class=admin_highlight align=right><?=$deletedCommentCount?> comments with <?=$tc_deletedAttachmentCount?> attachments</TD>
            </tr>
            <tr class=admin>
               <td class=admin_highlight><B><font class=editornotice>Total attachments deleted:</font></b></TD>
	       <TD class=admin_highlight align=right> <?=$t_attach_total?></TD>
	       <TD class=admin_highlight align=right> <?=$deletedAttachmentCount?> attachments </TD>
            </tr></table>
	    <p>
	    <?
         }
         else $editor_session->writeNoWritePermissionError();
         writeDeleteForm();
      }
      else if(isset($_REQUEST['delete']) && isset($_REQUEST['cancel']) && $_REQUEST['delete']!=null && $_REQUEST['cancel']!=null)
      {
         writeDeleteForm();
      }
      else if(isset($_REQUEST['delete']) && $_REQUEST['delete'] != null)
      {
         writeConfirmDeleteBox();
      }
      else
      {
         writeDeleteForm();
      }
   }
   else $editor_session->writeNoReadPermissionError();
}
else $editor_session->writeNoSessionError();

require_once('adminfooter.inc');

?> 
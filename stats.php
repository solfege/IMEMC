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

$OSCAILT_SCRIPT = "stats.php";
require_once("oscailt_init.inc");
require_once("objects/statistics.inc");
require_once("objects/memorymgmt.inc");
require_once("objects/generatebargraph.inc");

addToPageTitle("Publishing Statistics");

class statsDisplay
{
   var $mnth_link_frwd= "";
   var $mnth_link_bkwd= "";

   var $year_link_frwd= "";
   var $year_link_bkwd= "";

   var $day_link_frwd= "";
   var $day_link_bkwd= "";
   var $days_ago_date= null;

   var $startdate;
   var $first_day;
   var $first_year;
   var $first_month;
   
   
   // This returns an extra string to append to the link to allow the current state to be preserved.
   // Used by the forward and back links.
   function getStateInfoForLink()
   {
      $extraLnkInfo = "";     

      // Really only one or the other of any of these states can be set or should be.
      if ( isset($_REQUEST['daily']) && $_REQUEST['daily'] == 'true') {
          $extraLnkInfo = "&daily=true";
      }
      if ( isset($_REQUEST['monthly']) && $_REQUEST['monthly'] == 'true') {
          $extraLnkInfo = "&monthly=true";
      }
      if ( isset($_REQUEST['yearly']) && $_REQUEST['yearly'] == 'true') {
          $extraLnkInfo = "&yearly=true";
      }

      if ( isset($_REQUEST['bycat']) && $_REQUEST['bycat'] == 'true') {
          $extraLnkInfo = "&bycat=true";
      }
      if ( isset($_REQUEST['viewall']) && $_REQUEST['viewall'] == 'true') {
          $extraLnkInfo = "&viewall=true";
      }

      return $extraLnkInfo;
   }


   function getYearlyStats($start_year=0)
   {
      global $system_config, $OSCAILT_SCRIPT;
   
      // Do 4 years worth. Always go from Jan 1st to Dec 31st.
      $yearly_stats = array();
      $year_count = 4;
   
      $today_array = getdate( (time()+$system_config->timezone_offset) );
   
      $this_month = $today_array['mon'];
      $this_year = $today_array['year'];
      if ($start_year == 0 ) $start_year = $this_year;

      // Add code here to make sure we never link forward past this year or back before the first year.
      // There is no link forward if it is the default mode.
      $create_frwd_link = true;
      $create_bkwd_link = true;
      if ($start_year == $this_year) $create_frwd_link = false;

      // Do a SQL query to get the start date for the site.
      $this->startdate = $this->getStartDate();
      $this->first_day = $this->startdate['mday'];
      $this->first_year = $this->startdate['year'];
      $this->first_month = $this->startdate['mon'];

      // echo(" First Year " . $this->first_year . "<BR>");
      if ( ($this_year - $this->first_year ) < $year_count ) $year_count = ($this_year - $this->first_year +1);

      // Make sure rubbish values were not entered via the URL. 
      if ( ($start_year-$year_count+1) < $this->first_year ) $start_year = $this_year;

      // echo(" Start Year " . $start_year . " cnt " . $year_count . "<BR>");
      $current_year = $start_year;
   
      // Create link for 1 month forward and back.
      $frwd_year  = $start_year + 1;

      if ($frwd_year > $this_year) $create_frwd_link = false;

      $bkwd_year  = $start_year - 1;

      if ( ($bkwd_year-$year_count+1) < $this->first_year) $create_bkwd_link = false;

      // echo(" Frwd Year " . $frwd_year . " back_year " . $bkwd_year . " Yr Cnt " . $year_count . "<BR>");

      if ($create_frwd_link == true) {
          $this->year_link_frwd= '<a class="stats" href="'.$OSCAILT_SCRIPT .'?yearly=true&year=' .$frwd_year . $this->getStateInfoForLink() .'">&lt;&lt; Next</a>';
      } else {
          $this->year_link_frwd= "";
      }

      if ($create_bkwd_link == true) {
          $this->year_link_bkwd= '<a class="stats" href="'.$OSCAILT_SCRIPT .'?yearly=true&year=' .$bkwd_year . $this->getStateInfoForLink().'">Previous &gt;&gt;</a>';
      } else {
          $this->year_link_bkwd= "";
      }

      for ($iIndex = 0; $iIndex < $year_count; $iIndex++) {
   
          $after_txt = "1 Jan " . $current_year . " 12 am";
          $before_txt = "31 Dec " . $current_year . " 12 am ";

          $head_txt = $current_year;
   
          $yearly_stats[$iIndex] = new Statistics();
   
          // Add code here to tell stats to use db cache 2 for the current year.
          if ($iIndex == 0 ) {
              $yearly_stats[$iIndex]->setDbCache(2);
          }
          // echo(" yr " . $current_year . " Lower (after) " .$after_txt . " Upper (bef) " .$before_txt ."<BR>");
          $yearly_stats[$iIndex]->setHeaderText($head_txt);
          $yearly_stats[$iIndex]->setTimePostedLowerLimit($after_txt);
          $yearly_stats[$iIndex]->setTimePostedUpperLimit($before_txt);

          $current_year--;
      }
   
      return $yearly_stats;
   }


   function getMonthlyStats($start_month =0, $start_year =0)
   {
      global $system_config, $OSCAILT_SCRIPT;
   
      $month_array = Array( 1 => "January", 2 => "February", 3 => "March", 4 => "April", 5 => "May", 6 => "June", 7 => "July", 8 => "August", 9 => "September", 10 => "October", 11 => "November", 12 => "December");

      // Do 6 months worth. And start back from the current month unless input says otherwise.
      $monthly_stats = array();
      $month_count = 6;
   
      $today_array = getdate( (time()+$system_config->timezone_offset) );
   
      $this_month = $today_array['mon'];
      $this_year = $today_array['year'];

      if ( $start_month == 0) $current_month = $today_array['mon'];
      else $current_month = $start_month;
   
      if ( $start_year == 0) $current_year = $today_array['year'];
      else $current_year = $start_year;
   
      // Add code here to make sure the year and month -6 months do not move back the start of the archives.
      // There is no link forward if it is the default mode.
      $create_frwd_link = true;
      $create_bkwd_link = true;
      if ($start_month == 0) $create_frwd_link = false;

      // Do a SQL query to get the start date for the site.
      $this->startdate = $this->getStartDate();
      $this->first_day = $this->startdate['mday'];
      $this->first_year = $this->startdate['year'];
      $this->first_month = $this->startdate['mon'];

      // Make sure rubbish values were not entered via the URL. Actually the code should check
      // these values for 6 months after the date.

      $adjusted_first_year = $this->first_year;
      $adjusted_first_month = $this->first_month + ($month_count -1);
      if ($adjusted_first_month > 12) {
          $adjusted_first_month = $adjusted_first_month - 12;
          $adjusted_first_year++;
      }

      if (($current_year == $adjusted_first_year && $current_month < $adjusted_first_month) || 
           ($current_year < $adjusted_first_year)) {
           $current_month = $this_month;
           $current_year = $this_year;
      }
   
      // Create link for 1 month forward and back.
      $frwd_month = $current_month +1;
      $frwd_year  = $current_year;
      if ($frwd_month > 12 ) {
          $frwd_month = 1;
          $frwd_year++;
      }
      if ($frwd_year > $this_year) $create_frwd_link = false;
      if ($frwd_month > $this_month && $frwd_year >= $this_year) $create_frwd_link = false;

      $bkwd_month = $current_month -1;
      $bkwd_year  = $current_year;
      if ($bkwd_month < 1 ) {
          $bkwd_month = 12;
          $bkwd_year--;

      }
      if ($bkwd_year < $this->first_year) $create_bkwd_link = false;
      if ($bkwd_month < $this->first_month && $bkwd_year <= $this->first_year) $create_bkwd_link = false;


      if ($create_frwd_link == true) {
          $this->mnth_link_frwd= '<a class="stats" href="'.$OSCAILT_SCRIPT .'?monthly=true&month=' . $frwd_month . '&year=' .$frwd_year . $this->getStateInfoForLink().'">&lt;&lt; Next</a>';
          // $this->mnth_link_frwd= '<a href="'.$OSCAILT_SCRIPT .'?month=' . $frwd_month . '&year=' .$frwd_year .'">&lt;&lt; Next</a>';
      } else {
          $this->mnth_link_frwd= "";
      }

      if ($create_bkwd_link == true) {
          $this->mnth_link_bkwd= '<a class="stats" href="'.$OSCAILT_SCRIPT .'?monthly=true&month=' . $bkwd_month . '&year=' .$bkwd_year . $this->getStateInfoForLink().'">Previous &gt;&gt;</a>';
          // $this->mnth_link_bkwd= '<a href="'.$OSCAILT_SCRIPT .'?month=' . $bkwd_month . '&year=' .$bkwd_year .'" >Previous &gt;&gt;</a>';
      } else {
          $this->mnth_link_bkwd= "";
      }

      // When doing previous months, the prev then has to be +1
      $prev_month = $current_month;
      $prev_year  = $current_year;

      if ($start_month != 0) {
          $prev_month++;
          if ($prev_month > 12) {
              $prev_month = 1;
              $prev_year++;
          }
      }

      for ($iIndex = 0; $iIndex < $month_count; $iIndex++) {
   
          $after_txt = "1 " . $month_array[$current_month] . " " . $current_year . " 12 am";

	  // Only in the special default case is this true.
          if ($iIndex == 0  && $start_month == 0) {
   	   // The first one will include up to today.
              $before_txt = "12 am today";
          } else {
              $before_txt = "1 " . $month_array[$prev_month] . " " . $prev_year . " 12 am ";
          }
          $prev_month = $current_month;
          $prev_year  = $current_year;
   
          if ($current_month == 12 || $iIndex == 0 ) {
             $head_txt = $current_year . "<BR>" . substr($month_array[$current_month],0 ,3);
          } else {
             $head_txt = substr($month_array[$current_month], 0, 3);
          }
   
          $monthly_stats[$iIndex] = new Statistics();
   
          // Add code here to tell stats to use db cache 2 for the current month.
          if ($iIndex == 0 ) {
              $monthly_stats[$iIndex]->setDbCache(2);
          }
          //echo("Mon: " .$current_month . " yr " . $current_year . " Lower (after) " .$after_txt . " Upper (bef) " .$before_txt ."<BR>");
          $monthly_stats[$iIndex]->setHeaderText($head_txt);
          $monthly_stats[$iIndex]->setTimePostedLowerLimit($after_txt);
          $monthly_stats[$iIndex]->setTimePostedUpperLimit($before_txt);
   
          $current_month--;
          if ($current_month < 1 ) {
              $current_month = 12;
              $current_year--;
          }
   
      }
   
      return $monthly_stats;
   }
   
   // This function is still experimental. It will eventually replace the other display ones because 
   // it is more generic.
   function writeStatsObjectsBox($show_details, $heading_mode, $stat_objs, $by_category)
   {
      global $system_config;
   
      // $show_details = true; 
      $obj_count = count($stat_objs);
      $cell_count = $obj_count * 2;
      // It better be an even number originally
      $half_count = floor($obj_count/2 + .5) * 2;
   
      if ( $heading_mode == "month") {
          $sub_heading = "Montly Statistics";
          $link_frwd= $this->mnth_link_frwd;
          $link_bkwd= $this->mnth_link_bkwd;

      } elseif ( $heading_mode == "yearly") {
          $sub_heading = "Yearly Statistics";
          $link_frwd= $this->year_link_frwd;
          $link_bkwd= $this->year_link_bkwd;
      } else {
          $sub_heading = "Daily Statistics";
          $link_frwd= $this->day_link_frwd;
          $link_bkwd= $this->day_link_bkwd;
      }
   
      ?>
      <p>
      <table width=100%>
      <tr class="stats">
         <th class="stats"><?=$sub_heading?></th>
         <th class="stats_links" align=left cellspacing=0 colspan=<?=$half_count?>> <?=$link_frwd?> </th>
         <th class="stats_links" align=right cellspacing=0 colspan=<?=$half_count?>><?=$link_bkwd?></th>
      </tr>
      <tr class="stats">
         <th class="stats" rowspan=2>Item </th>
         <th class="stats" colspan=<?=$cell_count?>> Amount Posted Within Last ... </th>
      </tr>
      <tr>
      <?

      for ($iIndex = 0; $iIndex < $obj_count; $iIndex++) {
         $header = $stat_objs[$iIndex]->getHeaderText();
   
         ?>
         <th class="stats" colspan=2> <?=$header?> </th>
         <?
      }

      $grand_total = array();
      $hide_total  = array();
      $vis_total   = array();
      ?>
   
      </tr>
      <tr class="stats">
         <td>All</td>
      <?
      for ($iIndex = 0; $iIndex < $obj_count; $iIndex++) {
         $grand_total[$iIndex] = $stat_objs[$iIndex]->getItemCount();
         $hide_total[$iIndex]  = $stat_objs[$iIndex]->getItemCount(true);
         $vis_total[$iIndex]   = $grand_total[$iIndex] - $hide_total[$iIndex];
         ?>
         <td align=right colspan=2><?=$grand_total[$iIndex]?> </td>
         <?
      }
      ?>
      </tr>
      <?
      if ($show_details == true)
          $this->writeVisHideColumns($obj_count, $grand_total, $vis_total, $hide_total);

      ?>
      <tr class="stats">
         <td><img src="graphics/subgrouping.gif"> Features</td>
      <?
      for ($iIndex = 0; $iIndex < $obj_count; $iIndex++) {
         $grand_total[$iIndex] = $stat_objs[$iIndex]->getFeatureStoryCount();
         $hide_total[$iIndex]  = $stat_objs[$iIndex]->getFeatureStoryCount(true);
         $vis_total[$iIndex]   = $grand_total[$iIndex] - $hide_total[$iIndex];
         ?>
         <td align=right colspan=2><?=$grand_total[$iIndex]?> </td>
         <?
      }
      ?>
      </tr>
      <?
      if ($show_details == true)
          $this->writeVisHideColumns($obj_count, $grand_total, $vis_total, $hide_total);

      ?>
      <tr class="stats">
         <td><img src="graphics/subgrouping.gif"> Stories </TD>
      <?
      for ($iIndex = 0; $iIndex < $obj_count; $iIndex++) {
         $grand_total[$iIndex] = $stat_objs[$iIndex]->getStoryCount();
         $hide_total[$iIndex]  = $stat_objs[$iIndex]->getStoryCount(true);
         $vis_total[$iIndex]   = $grand_total[$iIndex] - $hide_total[$iIndex];
         ?>
         <td align=right colspan=2><?=$grand_total[$iIndex]?> </td>
         <?
      }
      ?>
      </tr>
      <?
      if ($show_details == true)
          $this->writeVisHideColumns($obj_count, $grand_total, $vis_total, $hide_total);

      // This will draw the by category button working out the mode to display. It is embedded
      // in the table
      $this->writeCategoryButton(($obj_count*2));

      if ( $by_category == "bycat") {
          for ($iIndex = 0; $iIndex < $obj_count; $iIndex++) {
              // Each one is actually an array.
              $grand_total[$iIndex] = $stat_objs[$iIndex]->getStoryByCategoryCount();
          }
	  // Now need to loop by category and then by month to go across table
	  // Pick the first element as that has the list of topic names.
	  $topicNameKeys = array_keys($grand_total[0]);
          foreach ($topicNameKeys as $categoryName ) {
              ?>
              <tr class="stats">
                 <td>&nbsp;<?=$categoryName?> </td>
              <?
              for ($iIndex = 0; $iIndex < $obj_count; $iIndex++) {
                 $statObjElement = $grand_total[$iIndex];
                 ?>
                 <td align=right colspan=2><?=$statObjElement[$categoryName]?> </td>
                 <?
              }
              ?>
              </tr>
              <?
          }
      }

      ?>
      <tr class="stats">
         <td>&nbsp;&nbsp;<img src="graphics/subgrouping.gif"> Non Featured Stories</td>
      <?
      for ($iIndex = 0; $iIndex < $obj_count; $iIndex++) {
         $grand_total[$iIndex] = $stat_objs[$iIndex]->getNonFeatureStoryCount();
         $hide_total[$iIndex]  = $stat_objs[$iIndex]->getNonFeatureStoryCount(true);
         $vis_total[$iIndex]   = $grand_total[$iIndex] - $hide_total[$iIndex];
         ?>
         <td align=right colspan=2><?=$grand_total[$iIndex]?> </td>
         <?
      }
      ?>
      </tr>
      <?
      if ($show_details == true)
          $this->writeVisHideColumns($obj_count, $grand_total, $vis_total, $hide_total);

      ?>
      <tr class="stats">
         <td>&nbsp;&nbsp;<img src="graphics/subgrouping.gif"> Events</td>
      <?
      for ($iIndex = 0; $iIndex < $obj_count; $iIndex++) {
         $grand_total[$iIndex] = $stat_objs[$iIndex]->getEventsCount();
         $hide_total[$iIndex]  = $stat_objs[$iIndex]->getEventsCount(true);
         $vis_total[$iIndex]   = $grand_total[$iIndex] - $hide_total[$iIndex];
         ?>
         <td align=right colspan=2><?=$grand_total[$iIndex]?> </td>
         <?
      }
      ?>
      </tr>
      <?
      if ($show_details == true)
          $this->writeVisHideColumns($obj_count, $grand_total, $vis_total, $hide_total);

      ?>
      <tr class="stats">
         <td><img src="graphics/subgrouping.gif"> Comments</td>
      <?
      for ($iIndex = 0; $iIndex < $obj_count; $iIndex++) {
         $grand_total[$iIndex] = $stat_objs[$iIndex]->getCommentCount();
         $hide_total[$iIndex]  = $stat_objs[$iIndex]->getCommentCount(true);
         $vis_total[$iIndex]   = $grand_total[$iIndex] - $hide_total[$iIndex];
         ?>
         <td align=right colspan=2><?=$grand_total[$iIndex]?> </td>
         <?
      }
      ?>
      </tr>
      <?
      if ($show_details == true)
          $this->writeVisHideColumns($obj_count, $grand_total, $vis_total, $hide_total);

      ?>
      <tr class="stats">
         <td><img src="graphics/subgrouping.gif"> Attachments</td>
      <?
      for ($iIndex = 0; $iIndex < $obj_count; $iIndex++) {
         $grand_total[$iIndex] = $stat_objs[$iIndex]->getAttachmentCount();
         $hide_total[$iIndex]  = $stat_objs[$iIndex]->getAttachmentCount(true);
         $vis_total[$iIndex]   = $grand_total[$iIndex] - $hide_total[$iIndex];
         ?>
         <td align=right colspan=2><?=$grand_total[$iIndex]?> </td>
         <?
      }
      ?>
      </tr>
      <?
      if ($show_details == true)
          $this->writeVisHideColumns($obj_count, $grand_total, $vis_total, $hide_total);

      ?>
      <tr class="stats">
         <td>&nbsp;&nbsp;<img src="graphics/subgrouping.gif"> Image</td>
      <?
      for ($iIndex = 0; $iIndex < $obj_count; $iIndex++) {
         $grand_total[$iIndex] = $stat_objs[$iIndex]->getImageAttachmentCount();
         $hide_total[$iIndex]  = $stat_objs[$iIndex]->getImageAttachmentCount(true);
         $vis_total[$iIndex]   = $grand_total[$iIndex] - $hide_total[$iIndex];
         ?>
         <td align=right colspan=2><?=$grand_total[$iIndex]?> </td>
         <?
      }
      ?>
      </tr>
      <?
      if ($show_details == true)
          $this->writeVisHideColumns($obj_count, $grand_total, $vis_total, $hide_total);

      ?>
      <tr class="stats">
         <td>&nbsp;&nbsp;<img src="graphics/subgrouping.gif"> Video</td>
      <?
      for ($iIndex = 0; $iIndex < $obj_count; $iIndex++) {
         $grand_total[$iIndex] = $stat_objs[$iIndex]->getVideoAttachmentCount();
         $hide_total[$iIndex]  = $stat_objs[$iIndex]->getVideoAttachmentCount(true);
         $vis_total[$iIndex]   = $grand_total[$iIndex] - $hide_total[$iIndex];
         ?>
         <td align=right colspan=2><?=$grand_total[$iIndex]?> </td>
         <?
      }
      ?>
      </tr>
      <?
      if ($show_details == true)
          $this->writeVisHideColumns($obj_count, $grand_total, $vis_total, $hide_total);

      ?>

      <tr class="stats">
         <td>&nbsp;&nbsp;<img src="graphics/subgrouping.gif"> Embedded Video</td>
      <?
      for ($iIndex = 0; $iIndex < $obj_count; $iIndex++) {
         $grand_total[$iIndex] = $stat_objs[$iIndex]->getEmbeddedVideoAttachmentCount();
         $hide_total[$iIndex]  = $stat_objs[$iIndex]->getEmbeddedVideoAttachmentCount(true);
         $vis_total[$iIndex]   = $grand_total[$iIndex] - $hide_total[$iIndex];
         ?>
         <td align=right colspan=2><?=$grand_total[$iIndex]?> </td>
         <?
      }
      ?>
      </tr>
      <?
      if ($show_details == true)
          $this->writeVisHideColumns($obj_count, $grand_total, $vis_total, $hide_total);

      ?>
      <tr class="stats">
         <td>&nbsp;&nbsp;<img src="graphics/subgrouping.gif"> Audio</td>
      <?
      for ($iIndex = 0; $iIndex < $obj_count; $iIndex++) {
         $grand_total[$iIndex] = $stat_objs[$iIndex]->getAudioAttachmentCount();
         $hide_total[$iIndex]  = $stat_objs[$iIndex]->getAudioAttachmentCount(true);
         $vis_total[$iIndex]   = $grand_total[$iIndex] - $hide_total[$iIndex];
         ?>
         <td align=right colspan=2><?=$grand_total[$iIndex]?> </td>
         <?
      }
      ?>
      </tr>
      <?
      if ($show_details == true)
          $this->writeVisHideColumns($obj_count, $grand_total, $vis_total, $hide_total);

      ?>

      <tr class="stats">
         <td>&nbsp;&nbsp;<img src="graphics/subgrouping.gif"> Embedded Audio</td>
      <?
      for ($iIndex = 0; $iIndex < $obj_count; $iIndex++) {
         $grand_total[$iIndex] = $stat_objs[$iIndex]->getEmbeddedAudioAttachmentCount();
         $hide_total[$iIndex]  = $stat_objs[$iIndex]->getEmbeddedAudioAttachmentCount(true);
         $vis_total[$iIndex]   = $grand_total[$iIndex] - $hide_total[$iIndex];
         ?>
         <td align=right colspan=2><?=$grand_total[$iIndex]?> </td>
         <?
      }
      ?>
      </tr>
      <?
      if ($show_details == true)
          $this->writeVisHideColumns($obj_count, $grand_total, $vis_total, $hide_total);

      ?>

      <tr class="stats">
         <td>&nbsp;&nbsp;<img src="graphics/subgrouping.gif"> Miscellaneous</td>
      <?
      for ($iIndex = 0; $iIndex < $obj_count; $iIndex++) {
         $grand_total[$iIndex] = $stat_objs[$iIndex]->getMiscellaneousAttachmentCount();
         $hide_total[$iIndex]  = $stat_objs[$iIndex]->getMiscellaneousAttachmentCount(true);
         $vis_total[$iIndex]   = $grand_total[$iIndex] - $hide_total[$iIndex];
         ?>
         <td align=right colspan=2><?=$grand_total[$iIndex]?> </td>
         <?
      }
      ?>
      </tr>
      <?
      if ($show_details == true)
          $this->writeVisHideColumns($obj_count, $grand_total, $vis_total, $hide_total);

      if ($this->days_ago_date != null) {
          ?><th class="admin" colspan=<?=($cell_count+2)?>> <?=$this->days_ago_date?> </th><?
      }
      ?>
      </table>
      <?
   }
   
   // Inputs are arrays
   function writeVisHideColumns($obj_count, $grand_total, $vis_total, $hide_total)
   {
      ?>
      <tr class="stats_vis">
         <td class="stats_vis">&nbsp;&nbsp;&nbsp;&nbsp; <i>Visible</i></td>
      <?
      for ($iIndex = 0; $iIndex < $obj_count; $iIndex++) {
         if ($grand_total[$iIndex] <= 0) $vis_percent = 0;
	 else $vis_percent =floor($vis_total[$iIndex] / $grand_total[$iIndex] * 100 + .5);

         if ($vis_total[$iIndex] == 0) $vis_total_str = "&nbsp; 0";
	 else $vis_total_str = $vis_total[$iIndex];
         ?>
         <td align=right class="stats_vis"><?=$vis_percent?>%</td>
         <td align=right class="stats_vis"><?=$vis_total_str?></td>
         <?
      }
      ?>
      </tr>
      <tr class="stats_hide">
         <td class="stats_hide">&nbsp;&nbsp;&nbsp;&nbsp; <i>Hidden</i></td>
      <?
      for ($iIndex = 0; $iIndex < $obj_count; $iIndex++) {
         if ($grand_total[$iIndex] <= 0) $hide_percent = 0;
	 else $hide_percent =floor($hide_total[$iIndex] / $grand_total[$iIndex] * 100 + .5);
         ?>
         <td align=right class="stats_hide"><?=$hide_percent?>%</td>
         <td align=right class="stats_hide"> <?=$hide_total[$iIndex]?></td>
         <?
      }
      ?> </tr> <?
   }
   
   function getDailyStats()
   {
      global $system_config, $OSCAILT_SCRIPT;

      $max_days_back = 90;
      $create_frwd_link = false;
      $create_bkwd_link = true;
      $frwd_day = 0;
      $bkwd_day = 1;

      $day_ago = 0;
      if (isset($_REQUEST['dayago']) && $_REQUEST['dayago'] > 0) {
         $day_ago = $_REQUEST['dayago']; 
         $create_frwd_link = true;

	 // Only go back a month at the most.
	 if ($day_ago > $max_days_back ) {
            $day_ago = $max_days_back;
            $create_bkwd_link = true;
         }

	 if ($day_ago > 0 ) $bkwd_day = $day_ago + 1;
	 if ($day_ago > 0 ) $frwd_day = $day_ago - 1;
      }


      // Setup daily frd + bck links
      if ($create_frwd_link == true) {
          $this->day_link_frwd= '<a class="stats" href="'.$OSCAILT_SCRIPT .'?dayago=' . $frwd_day . $this->getStateInfoForLink().  '">&lt;&lt; Next</a>';
      } else {
          $this->day_link_frwd= "";
      }

      if ($create_bkwd_link == true) {
          $this->day_link_bkwd= '<a class="stats" href="'.$OSCAILT_SCRIPT .'?dayago=' . $bkwd_day . $this->getStateInfoForLink().'">Previous &gt;&gt;</a>';
      } else {
          $this->day_link_bkwd= "";
      }
      if ($day_ago > 0 ) {
          $this->days_ago_date = $day_ago . " days ago was on: " .strftime($system_config->default_strftime_format, strtotime($day_ago." days ago"));
      }

      // Setup the times for the last 7 days.
      $todaystats = array();
      $day_count = 8;
      // The 8th is the total. See below. That is why we count to 7 here.
      for ($iDay = 0; $iDay < ($day_count-1); $iDay++) {

	  $reference_day = $iDay + $day_ago;

          $todaystats[$iDay] = new Statistics();
          if ($reference_day == 0 ) {
              $todaystats[$iDay]->setTimePostedLowerLimit("12 am yesterday");
          } else {
              $upper_day =$reference_day - 1;
              $todaystats[$iDay]->setTimePostedLowerLimit("12 am ".$reference_day." days ago");
              $todaystats[$iDay]->setTimePostedUpperLimit("12 am ".$upper_day." days ago");
          }
          $todaystats[$iDay]->setDbCache(2);
   
          // This block of code generates the header text for the table display.
          //if ($iDay == 0) $day_txt = "1 day ago";
          if ($reference_day == 0) $day_txt = "1 day ago";
          else $day_txt = ($reference_day+1) . " days ago";
   
          $tmp_time = strtotime($day_txt);
          $today_array = getdate($tmp_time);
          $day_of_week = substr($today_array['weekday'],0, 3);
   
          $head_txt = $day_of_week . "<BR>&nbsp;" . $day_txt . "&nbsp;";
          $todaystats[$iDay]->setHeaderText($head_txt);
      }
   
      $todaystats[($day_count-1)] = new Statistics();
      $todaystats[($day_count-1)]->setDbCache(2);
      $todaystats[($day_count-1)]->setHeaderText("&nbsp;Total&nbsp;");
      // Need to fix this.
      if ($day_ago > 0 ) {
         $todaystats[($day_count-1)]->setTimePostedLowerLimit("12 am ".($day_ago + $day_count-1)." days ago");
         $todaystats[($day_count-1)]->setTimePostedUpperLimit("12 am ".$day_ago." days ago");
      }
      else
      {
         $todaystats[($day_count-1)]->setTimePostedLowerLimit("12 am ".($day_count-1)." days ago");
         $todaystats[($day_count-1)]->setTimePostedUpperLimit("now");
      }
   
      return $todaystats;
   }  
   
      // This displays the option button to toggle the display of details which are the counts for the
   // visible and hidden items and their percentages.
   function writeOptionButton()
   {
      global $OSCAILT_SCRIPT;
      $show_all = false;
      if( isset($_REQUEST['viewall'])) {
	      if ($_REQUEST['viewall'] == 'true') $show_all = true;
	      if ($_REQUEST['viewall'] == 'false') $show_all = false;
      }

      if($show_all == false) $btn_txt = "Show Visible and Hidden Counts";
      else $btn_txt = "Show Items Totals Only " ;

      $btn_txt = "<<<   " . $btn_txt . "   >>>";

      echo '<DIV align="center">';
      echo '<FORM name="adminstats_form1" enctype="multipart/form-data" action="';
      $lnk = $OSCAILT_SCRIPT;
      echo $lnk;
      if(isset($_REQUEST['month']) && isset($_REQUEST['year'])) {
          $extra_url = '?month=' . $_REQUEST['month'] . '&year=' .$_REQUEST['year'];
	  echo $extra_url;
      }
      echo '" method="post">';
      echo "<INPUT type='submit' name='viewallbtn' value='$btn_txt'>\n";

      if ($show_all == false) echo "<INPUT type='hidden' name='viewall' value='true'>";
      else echo "<INPUT type='hidden' name='viewall' value='false'>";

      // To help track the display mode write the current mode
      // Writes the state of the display mode covering daily, montly and yearly.
      $this->writeStatsModeHiddenBtns();

      // Only if it was set then carry it forward.
      if(isset($_REQUEST['bycat']) && ($_REQUEST['bycat'] == 'true')) {
          echo "<INPUT type='hidden' name='bycat' value='true'>";
      }

      echo "\n</FORM></DIV>";

      return  $show_all;
   }

   // This displays the by category button to toggle the display of stats by category
   function writeCategoryButton($cell_span_width)
   {
      global $OSCAILT_SCRIPT;
      $show_all = false;
      if( isset($_REQUEST['viewall'])) {
	      if ($_REQUEST['viewall'] == 'true') $show_all = true;
	      if ($_REQUEST['viewall'] == 'false') $show_all = false;
      }

       if(isset($_REQUEST['bycat']) && ($_REQUEST['bycat'] == 'true')) {
          $by_cat_btn = "<INPUT type='submit' name='bycat_btn' value='Hide Stories By Category  &gt;&gt;'>";
      } else {
          $by_cat_btn = "<INPUT type='submit' name='bycat_btn' value='Stories By Category  &gt;&gt;'>";
      }
      ?>
          <tr class="stats">
             <td colspan=<?=$cell_span_width?>>
      <?
      echo '<FORM name="adminstats_form2" enctype="multipart/form-data" action="';
      $lnk = $OSCAILT_SCRIPT;
      echo $lnk;
      if(isset($_REQUEST['month']) && isset($_REQUEST['year'])) {
          $extra_url = '?month=' . $_REQUEST['month'] . '&year=' .$_REQUEST['year'];
	  echo $extra_url;
      }
      echo '" method="post">';

      if ($show_all == false) echo "<INPUT type='hidden' name='viewall' value='false'>";
      else echo "<INPUT type='hidden' name='viewall' value='true'>";

      // To help track the display mode write the current mode
      // Writes the state of the display mode covering daily, montly and yearly.
      $this->writeStatsModeHiddenBtns();

      // Only if it was set then carry it forward.
      if(isset($_REQUEST['bycat']) && ($_REQUEST['bycat'] == 'true')) {
          echo "<INPUT type='hidden' name='bycat' value='false'>";
      } else {
          echo "<INPUT type='hidden' name='bycat' value='true'>";
      }
      echo $by_cat_btn;
      ?>
	  </FORM></td>
          </tr>
      <?
   }

   function writeStatsModeHiddenBtns()
   {
      // To help track the display mode write the current mode
      if ( isset($_REQUEST['daily']) && $_REQUEST['daily'] == 'true') {
          echo "<INPUT type='hidden' name='daily' value='true'>";
      }
      if ( isset($_REQUEST['monthly']) && $_REQUEST['monthly'] == 'true') {
          echo "<INPUT type='hidden' name='monthly' value='true'>";
      }
      if ( isset($_REQUEST['yearly']) && $_REQUEST['yearly'] == 'true') {
          echo "<INPUT type='hidden' name='yearly' value='true'>";
      }
   }

   function writeStatsBox()
   {
      global $system_config;

      $todaystats = new Statistics();
      $todaystats->setTimePostedLowerLimit("12 am yeserday");
   
   
      $weekstats = new Statistics();
      $weekstats->setTimePostedLowerLimit("12 am 7 days ago");
   
      // Setup the times for the last 12 months.
      $monthstats = new Statistics();
      $monthstats->setTimePostedLowerLimit("12 am 1 month ago");
   
      $yearstats = new Statistics();
      $yearstats->setTimePostedLowerLimit("12 am 1 year ago");
   
      $totalstats = new Statistics();

      $current_time = time();
      $timeMsg = strftime($system_config->default_strftime_format, $current_time + $system_config->timezone_offset);

      $StartdateArray = $this->getStartDate();
      $first_year = $StartdateArray['year'];
      $first_month = $StartdateArray['month'];

      ?>
      <p>
      <table width=100%>
      <tr class=admin>
         <th class=admin colspan=6>&nbsp;<?=$timeMsg?>&nbsp;</th>
      </tr>
      <tr class=admin>
          <td class=admin colspan=6>The screen allows you to see basic statistics on the number of stories, 
          comments and attachments published including a breakdown of totals by year, month and day and then
          subdividied into categories. There is also an option to display hidden and visible counts for each.
          <BR><BR>
          The other panels display shared memory statistics collected from the point when the shared memory was
          activated. <BR><BR>
          <small>Note: The first story on this site was published in <?=$first_month?> <?=$first_year?> </small>
          </td>
      </tr>

   
      <tr class="stats">
         <th class="stats" rowspan=2>Item</th>
         <th class="stats" colspan=5>Amount Posted Within Last ...</th>
      </tr>
      <tr class="stats">
         <th class="stats">&nbsp;Day&nbsp;</th>
         <th class="stats">&nbsp;Week&nbsp;</th>
         <th class="stats">&nbsp;Month&nbsp;</th>
         <th class="stats">&nbsp;Year&nbsp;</th>
         <th class="stats">&nbsp;Total&nbsp;</th>
      </tr>
      <tr class="stats">
         <td>All</td>
         <td align=center><?=$todaystats->getItemCount()?></td>
         <td align=center><?=$weekstats->getItemCount()?></td>
         <td align=center><?=$monthstats->getItemCount()?></td>
         <td align=center><?=$yearstats->getItemCount()?></td>
         <td align=center><?=$totalstats->getItemCount()?></td>
      </tr>
      <tr class="stats">
         <td><img src="graphics/subgrouping.gif"> Features</td>
         <td align=center><?=$todaystats->getFeatureStoryCount()?></td>
         <td align=center><?=$weekstats->getFeatureStoryCount()?></td>
         <td align=center><?=$monthstats->getFeatureStoryCount()?></td>
         <td align=center><?=$yearstats->getFeatureStoryCount()?></td>
         <td align=center><?=$totalstats->getFeatureStoryCount()?></td>
      </tr>
      <tr class="stats">
         <td><img src="graphics/subgrouping.gif"> Stories</td>
         <td align=center><?=$todaystats->getNonFeatureStoryCount()?></td>
         <td align=center><?=$weekstats->getNonFeatureStoryCount()?></td>
         <td align=center><?=$monthstats->getNonFeatureStoryCount()?></td>
         <td align=center><?=$yearstats->getNonFeatureStoryCount()?></td>
         <td align=center><?=$totalstats->getNonFeatureStoryCount()?></td>
      </tr>
      <tr class="stats">
         <td><img src="graphics/subgrouping.gif"> Comments</td>
         <td align=center><?=$todaystats->getCommentCount()?></td>
         <td align=center><?=$weekstats->getCommentCount()?></td>
         <td align=center><?=$monthstats->getCommentCount()?></td>
         <td align=center><?=$yearstats->getCommentCount()?></td>
         <td align=center><?=$totalstats->getCommentCount()?></td>
      </tr>
      <tr class="stats">
         <td><img src="graphics/subgrouping.gif"> Attachments</td>
         <td align=center><?=$todaystats->getAttachmentCount()?></td>
         <td align=center><?=$weekstats->getAttachmentCount()?></td>
         <td align=center><?=$monthstats->getAttachmentCount()?></td>
         <td align=center><?=$yearstats->getAttachmentCount()?></td>
         <td align=center><?=$totalstats->getAttachmentCount()?></td>
      </tr>
      <tr class="stats">
         <td>&nbsp;&nbsp;<img src="graphics/subgrouping.gif"> Image</td>
         <td align=center><?=$todaystats->getImageAttachmentCount()?></td>
         <td align=center><?=$weekstats->getImageAttachmentCount()?></td>
         <td align=center><?=$monthstats->getImageAttachmentCount()?></td>
         <td align=center><?=$yearstats->getImageAttachmentCount()?></td>
         <td align=center><?=$totalstats->getImageAttachmentCount()?></td>
      </tr>
      <tr class="stats">
         <td>&nbsp;&nbsp;<img src="graphics/subgrouping.gif"> Video</td>
         <td align=center><?=$todaystats->getVideoAttachmentCount()?></td>
         <td align=center><?=$weekstats->getVideoAttachmentCount()?></td>
         <td align=center><?=$monthstats->getVideoAttachmentCount()?></td>
         <td align=center><?=$yearstats->getVideoAttachmentCount()?></td>
         <td align=center><?=$totalstats->getVideoAttachmentCount()?></td>
      </tr>
      <tr class="stats">
         <td>&nbsp;&nbsp;<img src="graphics/subgrouping.gif"> Embedded Video</td>
         <td align=center><?=$todaystats->getEmbeddedVideoAttachmentCount()?></td>
         <td align=center><?=$weekstats->getEmbeddedVideoAttachmentCount()?></td>
         <td align=center><?=$monthstats->getEmbeddedVideoAttachmentCount()?></td>
         <td align=center><?=$yearstats->getEmbeddedVideoAttachmentCount()?></td>
         <td align=center><?=$totalstats->getEmbeddedVideoAttachmentCount()?></td>
      </tr>

      <tr class="stats">
         <td>&nbsp;&nbsp;<img src="graphics/subgrouping.gif"> Audio</td>
         <td align=center><?=$todaystats->getAudioAttachmentCount()?></td>
         <td align=center><?=$weekstats->getAudioAttachmentCount()?></td>
         <td align=center><?=$monthstats->getAudioAttachmentCount()?></td>
         <td align=center><?=$yearstats->getAudioAttachmentCount()?></td>
         <td align=center><?=$totalstats->getAudioAttachmentCount()?></td>
      </tr>
      
      <tr class="stats">
         <td>&nbsp;&nbsp;<img src="graphics/subgrouping.gif"> Embedded Audio</td>
         <td align=center><?=$todaystats->getEmbeddedAudioAttachmentCount()?></td>
         <td align=center><?=$weekstats->getEmbeddedAudioAttachmentCount()?></td>
         <td align=center><?=$monthstats->getEmbeddedAudioAttachmentCount()?></td>
         <td align=center><?=$yearstats->getEmbeddedAudioAttachmentCount()?></td>
         <td align=center><?=$totalstats->getEmbeddedAudioAttachmentCount()?></td>
      </tr>

      <tr class="stats">
         <td>&nbsp;&nbsp;<img src="graphics/subgrouping.gif"> Miscellaneous</td>
         <td align=center><?=$todaystats->getMiscellaneousAttachmentCount()?></td>
         <td align=center><?=$weekstats->getMiscellaneousAttachmentCount()?></td>
         <td align=center><?=$monthstats->getMiscellaneousAttachmentCount()?></td>
         <td align=center><?=$yearstats->getMiscellaneousAttachmentCount()?></td>
         <td align=center><?=$totalstats->getMiscellaneousAttachmentCount()?></td>
      </tr>
      </table>
      <?
   }
   // These were copied from calendar.inc
   function getStartDate()
   {
      return getdate($this->getStartTime());
   }

   function getStartTime()
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
      return $tmp_startdate+$system_config->timezone_offset;
   }

   function writeShmemLinkHeader()
   {
       global $OSCAILT_SCRIPT, $system_config;

       $hour_mode = "true";
       $hour_msg = "Hourly Stats";
    
       $total_mode = "true";
       $total_msg = "Stats Totals";

       $usage_mode = "true";
       $usage_msg = "Memory Usage Stats";
       
       $contact_ip_mode = "true";
       $contact_ip_msg = "Contact IP Usage";

       $popular_mode = "true";
       $popular_msg = "Most Popular Story";

       ?>
         <TABLE class='admin'>
            <TR class='admin'><TD class='admin'> <a href="<?=$OSCAILT_SCRIPT?>?shmem=true&total=<?=$total_mode?>"><?=$total_msg?></a> | <a href="<?=$OSCAILT_SCRIPT?>?shmem=true&hourly=<?=$hour_mode?>"><?=$hour_msg?></a> | <a href="<?=$OSCAILT_SCRIPT?>?shmem=true&usage=<?=$usage_mode?>"><?=$usage_msg?></a> 
            | <a href="<?=$OSCAILT_SCRIPT?>?shmem=true&contact_ip=<?=$contact_ip_mode?>"><?=$contact_ip_msg?></a>
            | <a href="<?=$OSCAILT_SCRIPT?>?shmem=true&popular=<?=$popular_mode?>"><?=$popular_msg?></a>
            </TD></TR>
         </TABLE>
       <?
   }

   function writeLinkHeader()
   {
       global $OSCAILT_SCRIPT, $system_config;

       // Allow this just to determine if the first year of the website is this year in which case
       // we do not show the year stats link. There must be a easier way!
       $site_startdate = $this->getStartDate();
       $first_year     = $site_startdate['year'];
       $today_array = getdate( (time()+$system_config->timezone_offset) );
       $this_year = $today_array['year'];

       $month_mode = "true";
       $month_msg = "Monthly Stats";
    
       $daily_mode = "true";
       $daily_msg = "Daily Stats";

       ?>
         <TABLE class='admin'>
            <TR class='admin'><TD class='admin'><a href="<?=$OSCAILT_SCRIPT?>">Basic Stats</a> | <a href="<?=$OSCAILT_SCRIPT?>?daily=<?=$daily_mode?>"><?=$daily_msg?></a> | <a href="<?=$OSCAILT_SCRIPT?>?monthly=<?=$month_mode?>"><?=$month_msg?></a> 
       <?
       if ($first_year < $this_year ) {
            ?> | <a href="<?=$OSCAILT_SCRIPT?>?yearly=true">Yearly Stats</a> <?
       }    
       ?>
            </TD></TR>
         </TABLE>
       <?
   }

   // Writes a choice for controlling the last N stories or attachments.
   function writeSizeSelection($hidden_data_name)
   {
       $ActionFormData ="<select name='size_filter' onchange=submit()><option value='5'>5</option><option value='10' >10</option><option value='15' selected>15</option><option value='20'>20</option><option value='25'>25</option><option value='30'>30</option><option value='40'>40</option><option value='50'>50</option><option value='60'>60</option></select> ";

       ?>
       <FORM name='stats_last_size_filter' action='' method=POST> 
            <BR> Display Last <?=$ActionFormData?> <?=ucfirst($hidden_data_name)?> <BR>
            <input type=hidden name=<?=$hidden_data_name?> value="true">
       </FORM>
       <?
   }
   // Show most recent stories published
   function writeStoryRecentIds()
   {
       global $prefix, $dbconn, $system_config;

       $last_recs = 15;
       if (isset($_REQUEST['size_filter']) && $_REQUEST['size_filter'] > 0) $last_recs = $_REQUEST['size_filter'];

       $http_mode = isset($_SERVER['HTTPS']) ? "https" : "http";
       $url_base = '<a href="'.$http_mode.'://'.$system_config->site_url.'/';

       $result = sql_query("SELECT story_id, story_title, author_name,UNIX_TIMESTAMP(time_posted), hidden FROM ".$prefix."_stories ORDER BY time_posted DESC LIMIT 0, ".$last_recs, $dbconn,0);
       checkForError($result);

       $total = 0;
       if(sql_num_rows( $result ) > 0)
       {
           $stories = array();
           $story_titles = array();
           $story_author = array();
           $story_times = array();
           $story_stati = array();
           for ($j = 0; $j < sql_num_rows( $result ); $j++ )
	   {
               list($s_id, $s_title, $s_author, $s_time, $s_hide) = sql_fetch_row($result, $dbconn);
               $stories[$j] = $s_id;
               $story_titles[$j] = $s_title;
               $story_author[$j] = $s_author;
               $story_times[$j] = $s_time;
               $story_stati[$j] = $s_hide;
	       $total++;
           }
	   ?>
           <p>
           <table width=100% cellspacing=2 cellpadding=2>
           <tr class=admin>
           <th class=admin colspan=7>&nbsp; Most Recently Published <?=$total?> Stories &nbsp;</th>
           </tr>
	   <tr class="admin">
              <th class="admin" align=center>&nbsp;#&nbsp;</th>
              <th class="admin" align=center>&nbsp;Story Id&nbsp;</th>
              <th class="admin" align=center>Story Title</th>
              <th class="admin" align=center>&nbsp;by Author&nbsp;</th>
              <th class="admin" align=center>&nbsp;Time Posted&nbsp;</th>
              <th class="admin" align=center>&nbsp;Hidden Status&nbsp;</th>
           </tr>
           <?
           for ($j = 0; $j < $total; $j++ )
	   {
               $s_id = $stories[$j] ;
               $s_title = $story_titles[$j] ;
               $s_author = $story_author[$j] ;
	       $s_id_url = $url_base . 'article/' . $s_id. '">' .$s_id.'</a>';
	       $s_title_url = $url_base . 'article/' . $s_id. '">' .$s_title.'</a>';

               $s_time = strftime("%a %b %d, %T %H:%M", $story_times[$j]);
               if ($story_stati[$j] == null || $story_stati[$j] == 0 ) $hide_str = "Visible";
	       else $hide_str = "<b>Hidden</b>";

	      ?>
	      <tr class="stats">
              <td align=center><?=($j+1)?></td>
              <td align=center><?=$s_id_url?></td>
              <td align=left >&nbsp;<?=$s_title_url ?></td>
              <td align=left>&nbsp;<?=$s_author ?></td>
              <td align=right><?=$s_time ?></td>
              <td align=center><?=$hide_str ?></td>
              </tr>
              <?
           }
	   ?>
	      <tr class="stats">
              <td align=center colspan=7>
           <?
           $this->writeSizeSelection("stories");
	   ?>
              </td>
              </tr>
           </table>
           <?
       }
   }

   // Show most recent attachments published
   function writeAttachmentRecentIds()
   {
       global $prefix, $dbconn, $system_config;

       $last_recs = 15;
       if (isset($_REQUEST['size_filter']) && $_REQUEST['size_filter'] > 0) $last_recs = $_REQUEST['size_filter'];

       $http_mode = isset($_SERVER['HTTPS']) ? "https" : "http";
       $url_base = '<a href="'.$http_mode.'://'.$system_config->site_url.'/';
       $url_base_attach = $url_base . $system_config->attachment_store;


       // $result = sql_query("UPDATE ".$prefix."_attachments SET attachment_file='mar2009/image045_j.jpg' WHERE attachment_file='attachments/mar2009/image045_j.jpg'", $dbconn,0);

       $result = sql_query("SELECT attachment_id, story_id, comment_id,attachment_file, UNIX_TIMESTAMP(time_posted), hidden FROM ".$prefix."_attachments ORDER BY time_posted DESC LIMIT 0, ".$last_recs, $dbconn,0);
       checkForError($result);

       $total = 0;
       if(sql_num_rows( $result ) > 0)
       {
           $attachments = array();
           $attachment_story = array();
           $attachment_comment = array();
           $attachment_files = array();
           $attachment_times = array();
           $attachment_status = array();
           for ($j = 0; $j < sql_num_rows( $result ); $j++ )
	   {
               list($a_id, $a_story, $a_comment, $a_file, $a_time, $a_hide) = sql_fetch_row($result, $dbconn);
               $attachments[$j] = $a_id;
               $attachment_story[$j] = $a_story;
               $attachment_comment[$j]=$a_comment;
               $attachment_files[$j] = $a_file;
               $attachment_times[$j] = $a_time;
               $attachment_status[$j]= $a_hide;
	       $total++;
           }
	   ?>
           <p>
           <table width=100% cellspacing=2 cellpadding=2>
           <tr class=admin>
           <th class=admin colspan=7>&nbsp; Most Recently Published <?=$total?> Attachments &nbsp;</th>
           </tr>
	   <tr class="admin">
              <th class="admin" align=center>&nbsp;#&nbsp;</th>
              <th class="admin" align=center>&nbsp;Attachment Id&nbsp;</th>
              <th class="admin" align=center>&nbsp;Story Id&nbsp;</th>
              <th class="admin" align=center>&nbsp;Comment Id&nbsp;</th>
              <th class="admin" align=center>Filename </th>
              <th class="admin" align=center>&nbsp;Time Posted&nbsp;</th>
              <th class="admin" align=center>&nbsp;Hidden Status&nbsp;</th>
           </tr>
           <?
           for ($j = 0; $j < $total; $j++ )
	   {
               $a_id = $attachments[$j] ;
               $a_story = $attachment_story[$j] ;
               $a_comment = $attachment_comment[$j] ;
	       if ($a_comment > 0 ) {
	           $a_story_url = $url_base . 'article/' . $a_story. '">' .$a_story.'</a>';
	           $a_comment_url = $url_base . 'article/' . $a_story.'#comment'.$a_comment. '">' .$a_comment.'</a>';
               } else {
	           $a_story_url = $url_base . 'article/' . $a_story. '">' .$a_story.'</a>';
	           $a_comment_url = "N/A";
               }
	       $a_file     =  $attachment_files[$j];
	       if (substr($a_file,0,11) == "embedvideo:" || substr($a_file,0,11) == "embedaudio:") {
		   $extra_info = "";
	           if (substr($a_file,0,11) == "embedvideo:") {
		       $v_id = (int) substr($a_file,11,2);
		       $extra_info = " (". getEmbeddedVideoTypes($v_id,false) . " Id: ".substr($a_file,14).")";
		   }
		   if (strlen($a_file) > 70 ) {
		       $t_real_file=substr($a_file,14);
		       $t_path=dirname($t_real_file);
		       $t_name=basename($t_real_file);
		       $a_file_url=substr($a_file,0,13).$extra_info."<br><small>URL=".$t_path."/<BR>Filename=".$t_name."</small>";
		   } else {
		       $a_file_url =  $a_file . $extra_info;
		   }
               } else {
	           $a_file_url =  $url_base_attach .$a_file .'">' .$a_file.'</a>';
	           $info_url =  $url_base."/editimage.php?subpage=edit&image=".$system_config->attachment_store.$a_file.'">Info</a>';
	           $a_file_url .= " " . $info_url;
               }

               $a_time = strftime("%a %b %d, %T %H:%M", $attachment_times[$j]);
               if ($attachment_status[$j] == null || $attachment_status[$j] == 0 ) $hide_str = "Visible";
	       else $hide_str = "<b>Hidden</b>";

	      ?>
	      <tr class="stats">
              <td align=center><?=($j+1)?></td>
              <td align=center><?=$a_id?></td>
              <td align=left >&nbsp;<?=$a_story_url ?></td>
              <td align=center><?=$a_comment_url ?></td>
              <td align=left ><?=$a_file_url?></td>
              <td align=right><?=$a_time?></td>
              <td align=center><?=$hide_str?></td>
              </tr>
              <?
           }
 
	   ?>
	      <tr class="stats">
              <td align=center colspan=7>
           <?
           $this->writeSizeSelection("attachments");
	   ?>
              </td>
              </tr>
           </table>
           <?
       }
   }

   function writeInfoItem($colspan, $header,$details)
   {
   ?>
   <tr class=admin>
      <td class=admin><?=$header?></td><td class=admin colspan=<?=$colspan?>> <?=$details?></td>
   </tr>
   <?
   }

   // Show shared memory gathered statistics
   function displayMemoryHourlyStats($hourly_mode)
   {
       global $system_config, $counterNames;

       // Really this should never change.
       $no_hours = 24;
       $format_hour = false;
       if (isset($_REQUEST['formathour'])) $format_hour = true;

       if ($hourly_mode == true) $col_count = $no_hours + 1;
       else $col_count = 2;

       $begin_txt = strftime($system_config->default_strftime_format,$system_config->memory_mgmt_activate_time+$system_config->timezone_offset);

       $time_diff = time() - $system_config->memory_mgmt_activate_time;
       $days_active = floor( (time() - $system_config->memory_mgmt_activate_time) / 86400);
       $time_diff_str = getTimeAgoString($time_diff, true);

       if ($hourly_mode == false && $days_active > 1 ) $col_count++;

       ?>
           <p>
           <table width=100% cellspacing=2 cellpadding=2>
           <tr class=admin>
           <th class=admin colspan=<?=$col_count?>> Statistics Gathered by Shared Memory Counters </th>
           </tr>
           <tr class=admin>
           <td class=admin align=center colspan=<?=$col_count?>> Shared Memory was activated ( <?=$time_diff_str?> ) on <?=$begin_txt?><BR> Totals are since that time.</td>
           </tr>
       <?
       if ($hourly_mode == true) {
	   $cur_hour = strftime("%H");
           ?>
           <tr class=admin> <th class=admin> &nbsp; </th>
           <?
           for ($iCount = 0; $iCount < $no_hours; $iCount++) {
               ?><th class=admin> Hour </th><?
           }
           ?></tr> <tr class=admin><th class=admin>Counter Name </th> <?
           for ($iCount = 0; $iCount < $no_hours; $iCount++) {
               if ($format_hour == true) {
                   if ($iCount == 0) $t_str = "12 pm";
		   else if ($iCount <= 12) $t_str = $iCount . "am";
		   else $t_str = ($iCount-12) . "pm";
	       } else {
		   $t_str = $iCount;
	       }
               if ($iCount == $cur_hour) {
		   ?><th class=admin><u> <?=$t_str?></u> </th><?
	       } else {
	           ?><th class=admin> <?=$t_str?> </th><?
	       }
           }
           ?></tr><?
       } else {
           if ($days_active > 1 ) {
               ?> <tr class=admin> 
	          <td class=admin colspan=<?=($col_count-1)?>> &nbsp;</td>
	          <td class=admin align=center><b>Daily Average</b> </td>
		  </tr>
               <?
	   } 
       }

       $t_main_value = SharedMemoryRead("Main", false);
       $total_all = 0;
       foreach (array_keys($counterNames) as $t_key ) {
          // Those entries that are arrays are other types which we don't display here
          if (is_array($counterNames[$t_key])) continue;

          // Return an array
          $t_value_array = SharedMemoryRead($t_key, true);
          if ( $hourly_mode == true) {

              if ($t_key == "Main" ) {
                  ?> <tr class=admin> <td class=admin>Memory Counter <?=$t_key?></td> <?
		  /*
                  $LineGrp = new GenerateLineGrp();
                  $LineGrp->setTitle("Hourly Counts for 'Main' Counter");
                  $LineGrp->setGraphBasename("hourly_main");
                  $LineGrp->setWidthHeight(600, 180);
                  $LineGrp->setHeightRange(140);
                  $LineGrp->displayLineGraph($t_value_array);
		  */
	      } else {
                  ?> <tr class=admin> <td class=admin> <?=$t_key?> </td> <?
	      }

	      foreach ($t_value_array as $t_hr => $t_value ) {
                  if ($t_key != "Main" && $t_value > 0 )
                     $t_str = $t_value . " (" . floor($t_value/$t_main_value * 100 + .5)."%)";
                  else if ($t_key == "Main" )
                     $t_str = $t_value . " (100%)";
		  else
                     $t_str = $t_value;

	          $state_class = "admin";
                  if ($t_hr == (int)$cur_hour) $state_class = "admin_fade"; 
                  ?> <td class=<?=$state_class?> align=center><?=$t_str?></td> <?
	      }
              ?> </tr> <?
	  } else {
              // This would just get the current hour's readings.
              // $t_value = SharedMemoryRead($t_key, false);
              // Generate a total across all hours.
              // A VERY BIG ASSUMPTION IS THAT MAIN IS READ FIRST FROM THE ARRAY
              $total_value = 0;
	      foreach ($t_value_array as $t_hr => $t_value ) {
                  $total_value = $total_value + $t_value;
	      }

              if ($t_key != "Main" && $total_value > 0 ) {
                  $t_str = $total_value . " (" . floor($total_value/$total_main_value * 100 + .5)."%)";
	      } else if ($t_key == "Main" ) {
                  $total_main_value = $total_value;
                  $t_str = $total_value . " (100%)";
	      } else {
                  $t_str = $total_value;
	      }
              if ($days_active > 1 ) {
                  if ($total_value == 0 ) $t_average = 0;
		  else $t_average = floor($total_value/$days_active * 10 + .5) /10;
                  ?> <tr class=admin> <td class=admin>Memory Counter <?=$t_key?></td>
                     <td class="admin" align=center><?=$t_str?></td>
		     <td class="admin" align=center><?=$t_average?></td>
                  <?
              } else {
                  if ($t_key != "Main" && $t_key !="DbOpens" && $t_key != "DbNoOpens" && $total_value > 0 ) $total_all = $total_all + $total_value;
                  $this->writeInfoItem(1, "Memory Counter ".$t_key, $t_str );
              }
          }
       }
       if ( $hourly_mode == false) {
           $t_str = "<b>".$total_all . " (" . floor($total_all/$total_main_value * 100 + .5)."%) </b>";
           $this->writeInfoItem(1, "<b>Totals (excl Main)</b>", $t_str );
       }
 
       ?>
       </tr></table>
       <?
   }

   // Show shared memory gathered for memory usage gathered once for every 10 hits on Main
   function displayMemoryUsageGraph()
   {
       global $system_config;

       $os_str = "";
       if (isset($_ENV['OS'])) {
           $os_str  = "( Operating System: ". $_ENV['OS'] . " )";
       } else {
           $os_str  = "( Operating System: ". PHP_OS . " )";
       }
       ?>
       <p>
       <table width=100% cellspacing=2 cellpadding=2>
       <tr class=admin>
       <th class=admin colspan=1> Graph of Server Peak Memory Usage <?=$os_str?> </th>
       </tr>
       <tr class=admin>
       <td class=admin align=center>
       <?
  
       // May need to check the version of PHP because the get_memory is not available in earlier versions
       // nor in Windows versions.
       // if (isset($_ENV['OS']) && strcasecmp(substr($_ENV['OS'],0,3), "WIN") == 0) {
       if (strcasecmp(substr(PHP_OS,0,3), "WIN") == 0) {
           // Function also for: memory_get_peak_usage 
           ?> Windows does not support memory_get_usage function. Using randowm numbers for demo graph <BR><?
           $peak_value = 200;
           $peak_value = 100 + rand(100,300);
       } else {
           $php_version_info = phpversion();
           if ($php_version_info > 4.3 ) {
                $peak_value = floor(memory_get_usage()/1024);
           } else {
                ?> No support for memory_get_usage function in PHP version <?=$php_version_info?> <BR>
                  </td> </tr>
                  </table>
                <?
                return;
           }
       }
       list($MemoryUsageId, $MemTrkSize) = GetSharedMemoryId("MemoryUsage", true);

       $max_grph_pts = 25;
       $tmp_array = MemoryTracker($MemoryUsageId, $max_grph_pts, $peak_value, $MemTrkSize, false);

       $list_count = count($tmp_array);
       echo("Number of memory usage readings: ".$list_count);
           
       if ($list_count >= $max_grph_pts ) {
           $LineGrp = new GenerateLineGrp();
           $LineGrp->setWidthHeight(600, 180);
           $LineGrp->setHeightRange(140);
           $LineGrp->displayLineGraph($tmp_array);
       } else {
           ?> Not enough data yet collected for a memory usage graph <?
       } 
       ?>
       </td> </tr>
       </table>
       <?
   }

   // Display the spam related data collected from the contact form by shared memory. 
   // Shared memory activation checks should be already done when this function is called.
   function displayMemoryContactFormSpamData()
   {
       global $system_config, $semaphore_id;

       ?>
       <p>
       <table width=100% cellspacing=2 cellpadding=2>
       <tr class=admin>
       <th class=admin colspan=2> Detection of Spamming IPs in Contact Form <br>
       <small> Multiple Posts by the same IP address in quick succession causes automatic ban. Action log will be updated in such an event.</small>
       </th>
       </tr>
       <?
  
       // May need to check the version of PHP because the get_memory is not available in earlier versions
       // Memory assumed to exist.
       // Store IP and time in a dual memory lifo buffer
       $ShmemTiId = GetSharedMemoryId("ContactTimeStamp");
       $ShmemIpId = GetSharedMemoryId("ContactIpList");

       sem_acquire($semaphore_id);
       $IpListMem = new MemoryStore($ShmemIpId, STANDARD_MEM, MEM_MODE_READ, 600);
       $ip_list = $IpListMem->readMemory();
       $IpListMem->close();

       $TimeListMem = new MemoryStore($ShmemTiId, STANDARD_MEM, MEM_MODE_READ, 600);
       $time_list = $TimeListMem->readMemory();
       $TimeListMem->close();
       sem_release($semaphore_id);

       $IpArray = explode(" ",$ip_list);
       $TimeArray = explode(" ",$time_list);
       // First entry is usually junk.
       // echo("Data: ".count($TimeArray)."<br>");

       $real_data = 0;
       for ($iCount = 0; $iCount < count($IpArray); $iCount++)
       {
           if (trim($IpArray[$iCount]) == "") continue;
           if ($TimeArray[$iCount] == 0) continue;

           $t_str = $IpArray[$iCount] . " at " . strftime($system_config->default_strftime_format, $TimeArray[$iCount] + $system_config->timezone_offset);
           $this->writeInfoItem(1, "IP Address ", $t_str );
           $real_data++;
       }

       if ($real_data == 0 OR count($IpArray) == 0) {
	   ?>
	   <tr class=admin>
	   <td class=admin colspan=2><BR><BR>No data has been collected yet. <BR><BR>
	   </td>
	   </tr>
	   <?
       }

       ?>
       </table>
       <?
   }

   function getStoryTitles($StoryIdList)
   {
       global $prefix, $dbconn, $system_config;

       // echo("QUERY: ". "SELECT story_id, story_title, UNIX_TIMESTAMP(time_posted), hidden FROM ".$prefix."_stories WHERE story_id IN (".$StoryIdList.")<BR>");
       $result = sql_query("SELECT story_id, story_title, UNIX_TIMESTAMP(time_posted), hidden FROM ".$prefix."_stories WHERE story_id IN (".$StoryIdList.")", $dbconn,2);
       checkForError($result);

       $stories_array = array();
       if(sql_num_rows( $result ) > 0)
       {
           for ($j = 0; $j < sql_num_rows( $result ); $j++ )
	   {
               list($s_id, $s_title, $s_time, $s_hide) = sql_fetch_row($result, $dbconn);
               $sub_array = array($s_title, $s_time, $s_hide);
               $stories_array[$s_id] = $sub_array;
           }
       }
       return $stories_array;

   }
   // Display the most popular article id.
   // Shared memory 
   function displayMemoryPopular()
   {
       ?>
       <p>
       <table width=100% cellspacing=2 cellpadding=2>
       <tr class=admin>
           <th class=admin colspan=4> Most Popular Article Listing <br>
           </th>
       </tr>
       <tr class=admin>
           <th class=admin>&nbsp;#&nbsp;</th>
           <th class=admin> Article Id</th><th class=admin> Title </th> <th class=admin> Ranking<br>Number of Hits </th>
       </tr>
       <?
  
       global $system_config;
       $http_mode = isset($_SERVER['HTTPS']) ? "https" : "http";
       $url_base = '<a href="'.$http_mode.'://'.$system_config->site_url.'/';

       // May need to check the version of PHP because the get_memory is not available in earlier versions
       // Memory assumed to exist.
       // Store IP and time in a dual memory lifo buffer
       $ShmemId = GetSharedMemoryId("MostPopular");
       if ($ShmemId == null) {
          ?>
          <tr class=admin> <td class=admin colspan=4> Shared Memory Not Activated </td> </tr>
          </table>
          <?
          return;
       }
       // $DuumyArray = MemoryFrequentTracker($ShmemId, 1, false, true);
       /*
       $DuumyArray = MemoryFrequentTracker($ShmemId, 1, false, true);
       for ($iRndCnt = 0; $iRndCnt < 0;$iRndCnt++) {
           $DuumyArray = MemoryFrequentTracker($ShmemId, $article_id, false);
           $article_id = rand(1,5);
           $DuumyArray = MemoryFrequentTracker($ShmemId, $article_id, false);
       }
       */

       $DataArray = MemoryFrequentTracker($ShmemId, 1, true);
       if ($DataArray == null OR count($DataArray) == 0 ) {
          ?>
          <tr class=admin>
               <td class=admin colspan=4> No data collected yet </td>
          </tr>
          </table>
          <?
          return;
       }

       arsort($DataArray, SORT_NUMERIC );

       // This is the max number that we will display.
       $MaxListSize = 15;
       $t_Count = 0;
       $ArticleList = array();
       foreach ($DataArray as $Article => $Count) {
           if ($t_Count > $MaxListSize) break;
           $t_Count++;
           $ArticleIds[] = $Article;
       }
       $ArticleList = implode(",",$ArticleIds);

       // Next 2 lines only works if tracking list is same size. In reality tracking list will be far bigger.
       // $ArticleIds = array_keys($DataArray);
       // $ArticleList = implode(",",$ArticleIds);

       $TitleArray = $this->getStoryTitles($ArticleList);

       if ($TitleArray != null AND count($TitleArray) > 0 ) {
           $t_Count = 0;
           foreach ($DataArray as $Article => $Count) 
           {
               $t_Count++;
               if ($t_Count > $MaxListSize) break;
               //$this->writeInfoItem(2, $Article, $Count);
	       if (isset($TitleArray[$Article][0]) ) {
	           $title_text = $TitleArray[$Article][0];
	           $title_str= $url_base . 'article/' . $Article. '">' .$title_text.'</a>';
               } else {
	           $title_str = "Cannot find story title for this article id.";
               }
               ?>
               <tr class=admin>
               <td class=admin align=center><?=$t_Count?></td>
               <td class=admin><?=$Article?></td><td class=admin><?=$title_str?></td><td class=admin><?=$Count?></td>
               </tr>
               <?
           }
       }

       ?>
       </table>
       <?
   }


   // Show shared memory gathered statistics
   function writeMemoryStats()
   {
       global $system_config; 

       if ($system_config->memory_mgmt_installed == false) {
	   ?>
           <p> &nbsp; Shared Memory not installed. No statistics to display. </p>
           <?
           return;
       }
       if ($system_config->memory_mgmt_activated == false) {
	   ?>
           <p> &nbsp; Shared Memory not activated. No statistics to display. </p>
           <?
           return;
       }

       // There is a mode that display memory usage.
       if ( isset($_REQUEST['contact_ip']) && $_REQUEST['contact_ip'] == 'true') {
           $this->displayMemoryContactFormSpamData();
           return;
       }
       

       // There is a mode that display memory usage.
       if ( isset($_REQUEST['usage']) && $_REQUEST['usage'] == 'true') {
           $this->displayMemoryUsageGraph();
           return;
       }
       
       // There is a mode that display memory usage.
       if ( isset($_REQUEST['popular']) && $_REQUEST['popular'] == 'true') {
           $this->displayMemoryPopular();
           return;
       }
       
       $hourly_mode = true;
       if (isset($_REQUEST['total']) && $_REQUEST['total'] == 'true') $hourly_mode = false;
       if (isset($_REQUEST['hourly']) && $_REQUEST['hourly'] == 'true') $hourly_mode = true;

       // Actually it does 2 modes: totals and hourly.
       $this->displayMemoryHourlyStats($hourly_mode);
   }

}

ob_start();

if($editor_session->isSessionOpen())
{
   $adminStats = new statsDisplay();

   // Found in adminutils.inc
   writeAdminHeader($OSCAILT_SCRIPT."?shmem=true","Shared Memory Stats",array("?stories=true" => "List of last 15 Stories published","?attachments=true" => "List of last 15 Attachments published"));

   if ( isset($_REQUEST['shmem']) && $_REQUEST['shmem'] == 'true') 
       $adminStats->writeShmemLinkHeader();
   else if ( isset($_REQUEST['stories']) && $_REQUEST['stories'] == 'true') ;
   else if ( isset($_REQUEST['attachments']) && $_REQUEST['attachments'] == 'true') ;
   else
       $adminStats->writeLinkHeader();

   $memory_mode = false;
   if ( isset($_REQUEST['shmem']) && $_REQUEST['shmem'] == 'true') $memory_mode = true;
   if ( isset($_REQUEST['popular']) && $_REQUEST['popular'] == 'true') $memory_mode = true;

   $recent_mode = false;
   if ( isset($_REQUEST['stories']) && $_REQUEST['stories'] == 'true') $recent_mode = true;
   if ( isset($_REQUEST['attachments']) && $_REQUEST['attachments'] == 'true') $recent_mode = true;

   $basic_mode = true;
   if ( isset($_REQUEST['monthly']) && $_REQUEST['monthly'] == 'true') $basic_mode = false;
   if ( isset($_REQUEST['daily']) && $_REQUEST['daily'] == 'true') $basic_mode = false;
   if ( isset($_REQUEST['yearly']) && $_REQUEST['yearly'] == 'true') $basic_mode = false;

   // There are 3 modes of display....

   if ($recent_mode == true) {
       if ( isset($_REQUEST['stories'])) $adminStats->writeStoryRecentIds();
       if ( isset($_REQUEST['attachments'])) $adminStats->writeAttachmentRecentIds();

   } else if ($memory_mode == true) {

       $adminStats->writeMemoryStats();

   } else if ($basic_mode == true) {

       $adminStats->writeStatsBox();
   } else {

       $show_details = $adminStats->writeOptionButton();
    
       if(isset($_REQUEST['bycat']) && ($_REQUEST['bycat'] == 'true')) {
           $bycat_mode = "bycat";
       } else {
           $bycat_mode = "";
       }
    
       if ( isset($_REQUEST['daily']) && $_REQUEST['daily'] == 'true')
       {
           $daily_objs = $adminStats->getDailyStats();
           $adminStats->writeStatsObjectsBox($show_details, "daily", $daily_objs, $bycat_mode);
       }
    
       if ( isset($_REQUEST['monthly']) && $_REQUEST['monthly'] == 'true')
       {
           if(isset($_REQUEST['month']) && isset($_REQUEST['year']))
           {
               // Put in code to make sure they are valid numbers later...
               $month_objs = $adminStats->getMonthlyStats($_REQUEST['month'], $_REQUEST['year']);
           } else {
               $month_objs = $adminStats->getMonthlyStats();
           }
           $adminStats->writeStatsObjectsBox($show_details, "month", $month_objs, $bycat_mode);
       }

       if ( isset($_REQUEST['yearly']) && $_REQUEST['yearly'] == 'true')
       {
           if(isset($_REQUEST['year']))
           {
               // Put in code to make sure they are valid numbers later...
               $yearly_objs = $adminStats->getYearlyStats($_REQUEST['year']);
           } else {
               $yearly_objs = $adminStats->getYearlyStats();
           }
           $adminStats->writeStatsObjectsBox($show_details, "yearly", $yearly_objs, $bycat_mode);
       }
   
   }

   // Found in adminutils.inc
   writeAdminPageFooter();
}
else
{
   $editor_session->writeNoSessionError();
}

// The footer calls the code to disconnect from the db and whatever else needs to be done.
require_once("adminfooter.inc");
?>
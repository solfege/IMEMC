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
include("config/attachments.php");
require_once("oscailt_init.inc");
require_once("objects/indyobjects/indyobject.inc");
require_once("objects/indyobjects/indyitemset.inc");

require_once("objects/statistics.inc");
require_once("objects/memorymgmt.inc");
require_once("objects/generatebargraph.inc");

$textLabels = array("title" => "Publishing Statistics",
	            "monthly_text" => "Monthly Statistics",
	            "yearly_text" => "Yearly Statistics",
	            "weekly_text" => "Weekly Statistics",
	            "daily_text" => "Daily Statistics",
	            "item" => "Item",
	            "amount_posted" => "Amount Posted Within Last ...",
	            "all_word" => "All",
	            "prev" => "Previous",
	            "next" => "Next",
	            "hour" => "Hour",
	            "day" => "Day",
	            "week" => "Week",
	            "month" => "Month",
	            "year" => "Year",
	            "total" => "Total",
	            "features" => "Features",
	            "stories" => "Stories",
	            "non_features" => "Non Featured Stories",
	            "events" => "Events",
	            "comments" => "Comments",
	            "attachments" => "Attachments",
	            "image" => "Image",
	            "video" => "Video",
	            "embed_video" => "Embedded Video",
	            "audio" => "Audio",
	            "embed_audio" => "Embedded Audio",
	            "misc" => "Miscellaneous",
	            "visible" => "Visible",
	            "hidden" => "Hidden",
	            "show_vis_hide" => "Show Visible and Hidden Counts",
	            "show_item_only" => "Show Items Totals Only",
	            "hide_by_category" => "Hide Stories by Category",
	            "show_by_category" => "Stories by Category",
	            "show_two_day" => "Display Two Days",
	            "show_one_day" => "Display One Day",
	            "stats_text" => "The screen allows you to see basic statistics on the number of stories, 
          comments and attachments published including a breakdown of totals by year, month and day and then
          subdividied into categories. There is also an option to display hidden and visible counts for each.
          <BR><BR>
          The other panels display shared memory statistics collected from the point when the shared memory was
          activated. <BR><BR>
          <small>Note: The first story on this site was published in </small>"
	            );



$textObj = new indyItemSet();
$system_config->user_error_reporting=8;
if($textObj->load($system_config->xml_store, "stats") === false)
{
    $textObj->setUserMessage( array( USER_ERROR, "Failed to get text strings for the Stats. -Using defaults",""));
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

class mySQLPerformance
{
    var $version = 0;
    var $str_version;
    var $variable_vars;

    var $read_ratio;
    var $write_ratio;
    var $cache_ratio;

    var $status_results;
    var $variable_results;


    function mySQLPerformance($target_variables)
    {
        $this->variable_vars = $target_variables;
	array_push($this->variable_vars, "version");
        $this->getMySQLStatus();
        $this->getMySQLVariables();
        $this->parseVersion();

        $this->calcReadRatio();
        $this->calcWriteRatio();
	if ($this->version >= 5.0) $this->calcCacheRatio();
    }

    function getStatus($status_var)
    {
        if (isset($this->status_results[$status_var]))
        {
            return $this->status_results[$status_var];
        }
        return null;
    }
    function getVariable($variable_var)
    {
        if (isset($this->variable_results[$variable_var]))
        {
            return $this->variable_results[$variable_var];
        }
        return null;
    }

    function calcReadRatio()
    {
        if (isset($this->status_results["Key_reads"]) && isset($this->status_results["Key_read_requests"]) ) 
        {
            $this->read_ratio = (100 * round(10000 * $this->status_results["Key_reads"]/$this->status_results["Key_read_requests"])/10000) ;
        }
    }
    function calcWriteRatio()
    {
        // Ratio: Key_writes / Key_write_requests
        if (isset($this->status_results["Key_writes"]) && isset($this->status_results["Key_write_requests"]) ) 
        {
            $this->write_ratio = (100 * round(10000 * $this->status_results["Key_writes"]/$this->status_results["Key_write_requests"])/10000) ;
        }
    }

    function calcCacheRatio()
    {
        // Ratio: 1 - ((Key_blocks_unused * key_cache_block_size) / key_buffer_size)

        if (isset($this->status_results["Key_blocks_unused"]) && isset($this->variable_results["key_cache_block_size"]) && isset($this->variable_results["key_buffer_size"]) ) 
        {
            // Using tmp vars to read more easily
            $t_key_cache_blk_size= $this->variable_results["key_cache_block_size"];
            $t_key_buffer        = $this->variable_results["key_buffer_size"];
            $t_key_blocks_unused = $this->status_results["Key_blocks_unused"];
            $this->cache_ratio = 100 * round(100*(1 - (($t_key_blocks_unused * $t_key_cache_blk_size ) / $t_key_buffer_size)))/100; 
        }
    }

    // Return the uptime for mysql and convert it to days, hrs, mins
    function getUpTime()
    {
        return getTimeAgoString($this->status_results["Uptime"] );
    }
    function getUpTimeSecs()
    {
        return $this->status_results["Uptime"];
    }

    function getMySQLStatus()
    {
        global $dbconn;
        // Zero means no db cache
        $result = sql_query("SHOW STATUS ", $dbconn, 0);
        checkForError($result);
    
        $db_status_name = "";
        $db_status_value = "";
        $this->status_results = array();
    
        if(sql_num_rows( $result ) > 0)
        {
            for ($iresult =0; $iresult < sql_num_rows( $result );$iresult++)
            {
                list($db_status_name,$db_status_value) = sql_fetch_row($result, $dbconn);
                // echo $db_status_name. " = ".$db_status_value."<BR>";
                $this->status_results[$db_status_name] = $db_status_value;
            }
        } 
    }
    function getMySQLVariables()
    {
        global $dbconn;
        // Zero means no db cache
    
        $db_status_name = "";
        $db_var = "";
        $this->variable_results = array();
    
        $result = sql_query("SHOW VARIABLES ", $dbconn, 0);
        checkForError($result);
    
        if(sql_num_rows( $result ) > 0)
        {
            for ($iresult =0; $iresult < sql_num_rows( $result );$iresult++)
            {
                list($db_status_name,$db_var) = sql_fetch_row($result, $dbconn);
                if (in_array($db_status_name, $this->variable_vars)) {
                    $this->variable_results[$db_status_name] = $db_var;
                }
            }
        }

    }
    function parseVersion()
    {
        if(isset($this->variable_results["version"]))
        {
            $this->str_version = $this->variable_results["version"];
            $this->version = substr($this->str_version,0,3);
        }
    }
}

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
      if ( isset($_REQUEST['misc_details']) && $_REQUEST['misc_details'] == 'true') {
          $extraLnkInfo = "&misc_details=true";
      }
      if ( isset($_REQUEST['viewall']) && $_REQUEST['viewall'] == 'true') {
          $extraLnkInfo = "&viewall=true";
      }

      return $extraLnkInfo;
   }


   function getYearlyStats($start_year=0)
   {
      global $system_config, $OSCAILT_SCRIPT;
      global $textLabels;
   
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
          $this->year_link_frwd= '<a class="stats" href="'.$OSCAILT_SCRIPT .'?yearly=true&year=' .$frwd_year . $this->getStateInfoForLink() .'">&lt;&lt; '.$textLabels["next"].'</a>';
      } else {
          $this->year_link_frwd= "";
      }

      if ($create_bkwd_link == true) {
          $this->year_link_bkwd= '<a class="stats" href="'.$OSCAILT_SCRIPT .'?yearly=true&year=' .$bkwd_year . $this->getStateInfoForLink().'">'.$textLabels["prev"].' &gt;&gt;</a>';
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
      global $textLabels;
   
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
          $this->mnth_link_frwd= '<a class="stats" href="'.$OSCAILT_SCRIPT .'?monthly=true&month=' . $frwd_month . '&year=' .$frwd_year . $this->getStateInfoForLink().'">&lt;&lt; '.$textLabels["next"].'</a>';
          // $this->mnth_link_frwd= '<a href="'.$OSCAILT_SCRIPT .'?month=' . $frwd_month . '&year=' .$frwd_year .'">&lt;&lt; Next</a>';
      } else {
          $this->mnth_link_frwd= "";
      }

      if ($create_bkwd_link == true) {
          $this->mnth_link_bkwd= '<a class="stats" href="'.$OSCAILT_SCRIPT .'?monthly=true&month=' . $bkwd_month . '&year=' .$bkwd_year . $this->getStateInfoForLink().'">'.$textLabels["prev"].' &gt;&gt;</a>';
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
   function writeStatsObjectsBox($show_details, $heading_mode, $stat_objs, $by_category, $show_misc_details)
   {
      global $system_config, $textLabels, $OSCAILT_SCRIPT;
      global $fileExtensions;
   
      // $show_details = true; 
      $obj_count = count($stat_objs);
      $cell_count = $obj_count * 2;
      // It better be an even number originally
      $half_count = floor($obj_count/2 + .5) * 2;
   
      if ( $heading_mode == "month") {
          // "Monthly Statistics";
          $sub_heading = $textLabels["monthly_text"];
          $link_frwd= $this->mnth_link_frwd;
          $link_bkwd= $this->mnth_link_bkwd;

      } elseif ( $heading_mode == "yearly") {
          // "Yearly Statistics";
          $sub_heading = $textLabels["yearly_text"];
          $link_frwd= $this->year_link_frwd;
          $link_bkwd= $this->year_link_bkwd;
      } else {
          // "Daily Statistics";
          $sub_heading = $textLabels["daily_text"];
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
      <th class="stats" rowspan=2><?=$textLabels["item"]?> </th>
      <th class="stats" colspan=<?=$cell_count?>> <?=$textLabels["amount_posted"]?> </th>
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
      <td><?=$textLabels["all_word"]?></td>
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
      <td><img src="graphics/subgrouping.gif"> <?=$textLabels["features"]?></td>
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
      <td><img src="graphics/subgrouping.gif"> <?=$textLabels["stories"]?> </TD>
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
      $this->writeCategoryButton(($obj_count*2+1));

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
      <td>&nbsp;&nbsp;<img src="graphics/subgrouping.gif"> <?=$textLabels["non_features"]?> </td>
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
      <td>&nbsp;&nbsp;<img src="graphics/subgrouping.gif"> <?=$textLabels["events"]?></td>
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
      <td><img src="graphics/subgrouping.gif"> <?=$textLabels["comments"]?></td>
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
         <td><img src="graphics/subgrouping.gif"> <?=$textLabels["attachments"]?></td>
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
         <td>&nbsp;&nbsp;<img src="graphics/subgrouping.gif"> <?=$textLabels["image"]?></td>
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
         <td>&nbsp;&nbsp;<img src="graphics/subgrouping.gif"><?=$textLabels["video"]?> </td>
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
         <td>&nbsp;&nbsp;<img src="graphics/subgrouping.gif"> <?=$textLabels["embed_video"]?></td>
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
         <td>&nbsp;&nbsp;<img src="graphics/subgrouping.gif"> <?=$textLabels["audio"]?></td>
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
         <td>&nbsp;&nbsp;<img src="graphics/subgrouping.gif"> <?=$textLabels["embed_audio"]?> </td>
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


      $misc_details_form = '<FORM name="adminstats_form3" enctype="multipart/form-data" action="'.$OSCAILT_SCRIPT;
 
      // Handle month and year
      $misc_details_form .= $this->stackUrlParameters();
      $misc_details_form .= '" method="post">';

      $misc_details_form .= '&nbsp;&nbsp;<img src="graphics/subgrouping.gif"> '. $textLabels["misc"];
      if(isset($_REQUEST['misc_details']) && ($_REQUEST['misc_details'] == 'true')) {
          $misc_details_form .= "<INPUT type='submit' name='viewallbtn' value='&lt;&lt;'>\n";
          $misc_details_form .= "<INPUT type='hidden' name='misc_details' value='false'>";
      } else {
          $misc_details_form .= "<INPUT type='submit' name='viewallbtn' value='&gt;&gt;'>\n";
          $misc_details_form .= "<INPUT type='hidden' name='misc_details' value='true'>";
      }

      $misc_details_form .= "</FORM>";

      ?>

      <tr class="stats">
         <td> <?=$misc_details_form?></td>
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

      if ($show_misc_details == false) {
          if ($this->days_ago_date != null) {
              ?><th class="admin" colspan=<?=($cell_count+2)?>> <?=$this->days_ago_date?> </th><?
          }
          ?>
          </table>
          <?
	  return;
      }

      // Handle the different types of Miscellaneous files
      foreach (explode(" ",$fileExtensions['misc']) as $each_file_type) 
      {
          ?>
          <tr class="stats">
	     <td> &nbsp; &nbsp; &nbsp; &nbsp; <img src='graphics/subgrouping.gif'> <?=$each_file_type?> </td>
          <?
    
          for ($iIndex = 0; $iIndex < $obj_count; $iIndex++) {
             $grand_total[$iIndex] = $stat_objs[$iIndex]->getMiscellaneousAttachmentCount(false,$each_file_type);
             $hide_total[$iIndex]  = $stat_objs[$iIndex]->getMiscellaneousAttachmentCount(true, $each_file_type);
             $vis_total[$iIndex]   = $grand_total[$iIndex] - $hide_total[$iIndex];

             ?> <td align=right colspan=2> <?=$grand_total[$iIndex]?> <BR> </td> <?
          }
          ?>
          </tr>
          <?
          if ($show_details == true)
              $this->writeVisHideColumns($obj_count, $grand_total, $vis_total, $hide_total);
      }

      if ($this->days_ago_date != null) {
          ?><th class="admin" colspan=<?=($cell_count+2)?>> <?=$this->days_ago_date?> </th><?
      }
      ?>
      </table>
      <INPUT type='hidden' name='misc_details' value='true'>
      <?
   }
   
   // Inputs are arrays
   function writeVisHideColumns($obj_count, $grand_total, $vis_total, $hide_total)
   {
      global $textLabels;
      ?>
      <tr class="stats_vis">
         <td class="stats_vis">&nbsp;&nbsp;&nbsp;&nbsp; <i><?=$textLabels["visible"]?></i></td>
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
         <td class="stats_hide">&nbsp;&nbsp;&nbsp;&nbsp; <i><?=$textLabels["hidden"]?></i></td>
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
      global $OSCAILT_SCRIPT, $textLabels;
      $show_all = false;
      if( isset($_REQUEST['viewall'])) {
	      if ($_REQUEST['viewall'] == 'true') $show_all = true;
	      if ($_REQUEST['viewall'] == 'false') $show_all = false;
      }

      //if($show_all == false) $btn_txt = "Show Visible and Hidden Counts";
      //else $btn_txt = "Show Items Totals Only " ;
      if($show_all == false) $btn_txt = $textLabels["show_vis_hide"];
      else $btn_txt = $textLabels["show_item_only"];

      $btn_txt = "<<<   " . $btn_txt . "   >>>";

      echo '<DIV align="center">';
      echo '<FORM name="adminstats_form1" enctype="multipart/form-data" action="'.$OSCAILT_SCRIPT;
 
      // Handle month and year
      $extra_url = $this->stackUrlParameters();

      echo $extra_url.'" method="post">';
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

   // Handle detection of month and year parameters for adding to URL.
   function stackUrlParameters()
   {
      $extra_url = "";
      $next_parm_symbol = "?";
      $parm_list = array("month", "year", "monthly", "yearly", "bycat", "misc_details");

      foreach ($parm_list as $t_parameter) {
          if(isset($_REQUEST[$t_parameter]) ) {
               $extra_url .= $next_parm_symbol . $t_parameter .'=' .$_REQUEST[$t_parameter];
               $next_parm_symbol = "&";
          }
      }

      return $extra_url;

   }

   // This displays the by category button to toggle the display of stats by category
   function writeCategoryButton($cell_span_width)
   {
      global $OSCAILT_SCRIPT, $textLabels;
      $show_all = false;
      if( isset($_REQUEST['viewall'])) {
	      if ($_REQUEST['viewall'] == 'true') $show_all = true;
	      if ($_REQUEST['viewall'] == 'false') $show_all = false;
      }

      ?>
          <tr class="stats">
             <td colspan=<?=$cell_span_width?>>
      <?

      // Handle month and year
      $extra_url = $this->stackUrlParameters();
      echo '<FORM name="adminstats_form2" enctype="multipart/form-data" action="',$OSCAILT_SCRIPT.$extra_url.'" method="post">';

      if ($show_all == false) echo "<INPUT type='hidden' name='viewall' value='false'>";
      else echo "<INPUT type='hidden' name='viewall' value='true'>";

      // To help track the display mode write the current mode
      // Writes the state of the display mode covering daily, montly and yearly.
      $this->writeStatsModeHiddenBtns();

      // Only if it was set then carry it forward.
      if(isset($_REQUEST['bycat']) && ($_REQUEST['bycat'] == 'true')) {
          echo "<INPUT type='hidden' name='bycat' value='false'>";
          echo "<INPUT type='submit' name='bycat_btn' value='".$textLabels['hide_by_category']."  &gt;&gt;'>";
      } else {
          echo "<INPUT type='hidden' name='bycat' value='true'>";
          echo "<INPUT type='submit' name='bycat_btn' value='".$textLabels['show_by_category']."  &gt;&gt;'>";
      }

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

   function writeStatsCommentModeHiddenBtns()
   {
      // To help track the display mode write the current mode
      if ( isset($_REQUEST['twoday_mode']) && $_REQUEST['twoday_mode'] == 'true') {
          echo "<INPUT type='hidden' name='twoday_mode' value='true'>";
      }
   }

   function writeStatsCommentModeBtns()
   {
      global $OSCAILT_SCRIPT, $textLabels;
      // To help track the display mode write the current mode

      if(isset($_REQUEST['twoday_mode']) && ($_REQUEST['twoday_mode'] == 'true')) {
          $twoday_btn = "<INPUT type='submit' name='daymode_btn' value='".$textLabels['show_one_day']."  &gt;&gt;'>";
      } else {
          $twoday_btn = "<INPUT type='submit' name='daymode_btn' value='".$textLabels['show_two_day']."  &gt;&gt;'>";
      }

      echo '<FORM name="adminstats_form3" enctype="multipart/form-data" action="'.$OSCAILT_SCRIPT.'" method="post">';
      $this->writeStatsCommentModeHiddenBtns();
      echo $twoday_btn;
      if ( isset($_REQUEST['twoday_mode']) && $_REQUEST['twoday_mode'] == 'true') {
          echo "<INPUT type='hidden' name='twoday_mode' value='false'>";
      } else {
          echo "<INPUT type='hidden' name='twoday_mode' value='true'>";
      }
      ?>
          <INPUT type='hidden' name='comments' value='true'>
	  </FORM>
      <?

   }

   function writeStatsBox()
   {
      global $system_config, $textLabels;

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
	  <td class=admin colspan=6><?=$textLabels["stats_text"]?>
          <!--The screen allows you to see basic statistics on the number of stories, 
          comments and attachments published including a breakdown of totals by year, month and day and then
          subdividied into categories. There is also an option to display hidden and visible counts for each.
          <BR><BR>
          The other panels display shared memory statistics collected from the point when the shared memory was
          activated. <BR><BR>
          <small>Note: The first story on this site was published in --><small><?=$first_month?> <?=$first_year?> </small>
          </td>
      </tr>

   
      <tr class="stats">
         <th class="stats" rowspan=2><?=$textLabels["item"]?></th>
         <th class="stats" colspan=5><?=$textLabels["amount_posted"]?></th>
      </tr>
      <tr class="stats">
         <th class="stats">&nbsp;<?=$textLabels["day"]?>&nbsp;</th>
         <th class="stats">&nbsp;<?=$textLabels["week"]?>&nbsp;</th>
         <th class="stats">&nbsp;<?=$textLabels["month"]?>&nbsp;</th>
         <th class="stats">&nbsp;<?=$textLabels["year"]?>&nbsp;</th>
         <th class="stats">&nbsp;<?=$textLabels["total"]?>&nbsp;</th>
      </tr>
      <tr class="stats">
         <td><?=$textLabels["all_word"]?></td>
         <td align=center><?=$todaystats->getItemCount()?></td>
         <td align=center><?=$weekstats->getItemCount()?></td>
         <td align=center><?=$monthstats->getItemCount()?></td>
         <td align=center><?=$yearstats->getItemCount()?></td>
         <td align=center><?=$totalstats->getItemCount()?></td>
      </tr>
      <tr class="stats">
         <td><img src="graphics/subgrouping.gif"> <?=$textLabels["features"]?></td>
         <td align=center><?=$todaystats->getFeatureStoryCount()?></td>
         <td align=center><?=$weekstats->getFeatureStoryCount()?></td>
         <td align=center><?=$monthstats->getFeatureStoryCount()?></td>
         <td align=center><?=$yearstats->getFeatureStoryCount()?></td>
         <td align=center><?=$totalstats->getFeatureStoryCount()?></td>
      </tr>
      <tr class="stats">
         <td><img src="graphics/subgrouping.gif"> <?=$textLabels["stories"]?></td>
         <td align=center><?=$todaystats->getNonFeatureStoryCount()?></td>
         <td align=center><?=$weekstats->getNonFeatureStoryCount()?></td>
         <td align=center><?=$monthstats->getNonFeatureStoryCount()?></td>
         <td align=center><?=$yearstats->getNonFeatureStoryCount()?></td>
         <td align=center><?=$totalstats->getNonFeatureStoryCount()?></td>
      </tr>
      <tr class="stats">
         <td><img src="graphics/subgrouping.gif"> <?=$textLabels["comments"]?></td>
         <td align=center><?=$todaystats->getCommentCount()?></td>
         <td align=center><?=$weekstats->getCommentCount()?></td>
         <td align=center><?=$monthstats->getCommentCount()?></td>
         <td align=center><?=$yearstats->getCommentCount()?></td>
         <td align=center><?=$totalstats->getCommentCount()?></td>
      </tr>
      <tr class="stats">
         <td><img src="graphics/subgrouping.gif"> <?=$textLabels["attachments"]?></td>
         <td align=center><?=$todaystats->getAttachmentCount()?></td>
         <td align=center><?=$weekstats->getAttachmentCount()?></td>
         <td align=center><?=$monthstats->getAttachmentCount()?></td>
         <td align=center><?=$yearstats->getAttachmentCount()?></td>
         <td align=center><?=$totalstats->getAttachmentCount()?></td>
      </tr>
      <tr class="stats">
         <td>&nbsp;&nbsp;<img src="graphics/subgrouping.gif"> <?=$textLabels["image"]?></td>
         <td align=center><?=$todaystats->getImageAttachmentCount()?></td>
         <td align=center><?=$weekstats->getImageAttachmentCount()?></td>
         <td align=center><?=$monthstats->getImageAttachmentCount()?></td>
         <td align=center><?=$yearstats->getImageAttachmentCount()?></td>
         <td align=center><?=$totalstats->getImageAttachmentCount()?></td>
      </tr>
      <tr class="stats">
         <td>&nbsp;&nbsp;<img src="graphics/subgrouping.gif"> <?=$textLabels["video"]?></td>
         <td align=center><?=$todaystats->getVideoAttachmentCount()?></td>
         <td align=center><?=$weekstats->getVideoAttachmentCount()?></td>
         <td align=center><?=$monthstats->getVideoAttachmentCount()?></td>
         <td align=center><?=$yearstats->getVideoAttachmentCount()?></td>
         <td align=center><?=$totalstats->getVideoAttachmentCount()?></td>
      </tr>
      <tr class="stats">
         <td>&nbsp;&nbsp;<img src="graphics/subgrouping.gif"> <?=$textLabels["embed_video"]?></td>
         <td align=center><?=$todaystats->getEmbeddedVideoAttachmentCount()?></td>
         <td align=center><?=$weekstats->getEmbeddedVideoAttachmentCount()?></td>
         <td align=center><?=$monthstats->getEmbeddedVideoAttachmentCount()?></td>
         <td align=center><?=$yearstats->getEmbeddedVideoAttachmentCount()?></td>
         <td align=center><?=$totalstats->getEmbeddedVideoAttachmentCount()?></td>
      </tr>

      <tr class="stats">
         <td>&nbsp;&nbsp;<img src="graphics/subgrouping.gif"> <?=$textLabels["audio"]?></td>
         <td align=center><?=$todaystats->getAudioAttachmentCount()?></td>
         <td align=center><?=$weekstats->getAudioAttachmentCount()?></td>
         <td align=center><?=$monthstats->getAudioAttachmentCount()?></td>
         <td align=center><?=$yearstats->getAudioAttachmentCount()?></td>
         <td align=center><?=$totalstats->getAudioAttachmentCount()?></td>
      </tr>
      
      <tr class="stats">
         <td>&nbsp;&nbsp;<img src="graphics/subgrouping.gif"> <?=$textLabels["embed_audio"]?></td>
         <td align=center><?=$todaystats->getEmbeddedAudioAttachmentCount()?></td>
         <td align=center><?=$weekstats->getEmbeddedAudioAttachmentCount()?></td>
         <td align=center><?=$monthstats->getEmbeddedAudioAttachmentCount()?></td>
         <td align=center><?=$yearstats->getEmbeddedAudioAttachmentCount()?></td>
         <td align=center><?=$totalstats->getEmbeddedAudioAttachmentCount()?></td>
      </tr>

      <tr class="stats">
         <td>&nbsp;&nbsp;<img src="graphics/subgrouping.gif"> <?=$textLabels["misc"]?></td>
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

   function formatHour($t_hour)
   {
      if ($t_hour < 0 )  return 24-$t_hour;
      if ($t_hour < 10 )  return "0".$t_hour;
      return $t_hour;
   }
   function formatTimeOffset($dayBase, $altDayBase, $t_hour)
   {
      if ($t_hour < 0 )  {
          $t_hour = 21 - $t_hour ;
          $t_hour = $this->formatHour($t_hour);
	  return $altDayBase . $t_hour;
      } else if ($t_hour > 23 ) {
          $t_hour = $t_hour - 24;
          $t_hour = $this->formatHour($t_hour);
	  return $altDayBase . $t_hour;
      }

      $t_hour = $this->formatHour($t_hour);
      return $dayBase . $t_hour ;
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


   // Show text bargraph of number of comments published per hour for the last day or 2 days
   function displayCommentHourlyStats()
   {
      global $prefix, $dbconn,$system_config;
      global $textLabels;
      $current_time = time();
      $current_host_hr = strftime("%H", $current_time);
      $current_hr = strftime("%H", $current_time + $system_config->timezone_offset);
      $current_day = strftime("%d", $current_time + $system_config->timezone_offset);
      $twoDayMode = false;

      $twoday_offset = 0;
      if (isset($_REQUEST["twoday_mode"]) && $_REQUEST["twoday_mode"] == 'true' ) {
          $twoDayMode = true;
          $twoday_offset = 86400;
      }

      $timeMsg = strftime($system_config->default_strftime_format, $current_time + $system_config->timezone_offset);
      $hr_offset = floor(($system_config->timezone_offset/3600 + 0.5));

      // Day before or after  ..... this is all tricky to code up since timezones will span days.
      // It is based on the 24 hour period for the site but if site host if offset in a different
      // timezone, then the selects have to be adjusted and then adjusted again for the display.
      if ($system_config->timezone_offset == 0 ) {
          $dayBase1 = strftime("%Y%m%d", ($current_time - $twoday_offset));
          $dayBaseM = strftime("%Y%m%d", $current_time);
          $dayBase2 = $dayBaseM;
      } else if ($system_config->timezone_offset > 0 ) {
          $dayBase1 = strftime("%Y%m%d", $current_time - 86400 -$twoday_offset );
          $dayBaseM = strftime("%Y%m%d", $current_time - 86400 );
          $dayBase2 = strftime("%Y%m%d", $current_time );
      } else {
          $dayBase1 = strftime("%Y%m%d", ($current_time - $twoday_offset));
          $dayBaseM = strftime("%Y%m%d", $current_time );
          $dayBase2 = strftime("%Y%m%d", $current_time + 86400);
      }

      if ($hr_offset == 0 ) {
          $startTime = $dayBase1 . "00";
          $endTime = $dayBase2 . "23";
	  $start_hr = 0;
      } else if ($hr_offset > 0 ) {
	  $start_hr = $this->formatHour(24-$hr_offset);
          $startTime = $this->formatTimeOffset($dayBase1, $dayBase2, $start_hr) ;
          $endTime = $this->formatTimeOffset($dayBase2, $dayBase2, 24+$start_hr) ;
      } else if ($hr_offset < 0 ) {
          $startTime = $dayBase1.  $this->formatHour(23 - abs($hr_offset+1)) ;
	  // No need for minus as it is minus already
          $endTime = $dayBase2.  $this->formatHour((23+$hr_offset)) ;
	  $start_hr = $this->formatHour($hr_offset);
      }
      // echo "day Base  " .$dayBase1. " Start ".$startTime. "<BR>";
      // echo "day Base  " .$dayBase2. " End- ".$endTime. "<BR>";
      // echo "<BR>";

      if ($twoDayMode == true) {
          $prevDayBase = strftime("%Y%m%d", ($current_time-86400) + $system_config->timezone_offset);
          $prevDayBase = $dayBase1;
      }

      if (isset($_REQUEST["date"]) && $_REQUEST["date"] != "" ) {
          $dayBase = $_REQUEST["date"];
          $twoDayMode = false;
      }

      if ($twoDayMode == true) {
          $stmt = "SELECT substring(date_format(time_posted,'%Y%m%d%H'),1,10),COUNT(1) FROM ".$prefix."_comments where substring(date_format(time_posted,'%Y%m%d%H'),1,10) >='".$startTime."' and substring(date_format(time_posted,'%Y%m%d%H'),1,10) <='".$endTime."' GROUP BY 1 ";
      } else {
          $stmt = "SELECT substring(date_format(time_posted,'%Y%m%d%H'),1,10),COUNT(1) FROM ".$prefix."_comments where substring(date_format(time_posted,'%Y%m%d%H'),1,10) >='".$startTime."' and substring(date_format(time_posted,'%Y%m%d%H'),1,10) <='".$endTime."' GROUP BY 1 ";
          // $stmt = "SELECT substring(date_format(time_posted,'%Y%m%d%H'),9,10),COUNT(1) FROM ".$prefix."_comments where substring(date_format(time_posted,'%Y%m%d%H'),1,10) >='".$startTime."' and substring(date_format(time_posted,'%Y%m%d%H'),1,10) <='".$endTime."' GROUP BY 1 ";
      }

      // $stmt = "SELECT substring(time_posted,1,10),COUNT(1) FROM ".$prefix."_comments group by 1 ";
      // echo "<BR>".  $stmt.  "<BR><BR>";
      $result = sql_query($stmt, $dbconn, 0);
      checkForError($result);

      $comments = array();
      $dayBase = $dayBase1;

      $jj=0;
      $save_start_hr = $start_hr;
      $mapping = array();
      if ($twoDayMode == true) {
          for ($i=0; $i <= 23; $i++)
          {
              if ($start_hr < 10 ) $comments[$dayBase."0".$start_hr] = 0;
	      else $comments[$dayBase .$start_hr] = 0;
              // if ($i < 10 ) $comments[$prevDayBase."0".$i] = 0;
	      // else $comments[$prevDayBase .$i] = 0;
              // if ($start_hr < 10 ) echo $jj." CCmts ".$dayBase."0".$start_hr . "<BR>";
	      // else echo $jj." CCmts ".$dayBase .$start_hr. "<BR>";
              $mapping[$this->formatHour($start_hr)] = $this->formatHour($i);
              $jj++;
              $start_hr++;
	      if ($start_hr > 23 ) {
	          $start_hr = 0;
                  $dayBase = $dayBaseM;
	      }
          }
      }

      $dayBase = $dayBaseM;

      $start_hr = $save_start_hr ;

      for ($i=0; $i <= 23; $i++)
      {
          if ($start_hr < 10 ) $comments[$dayBase."0".$start_hr] = 0;
	  else $comments[$dayBase .$start_hr] = 0;

          $mapping[$this->formatHour($start_hr)] = $this->formatHour($i);
          $start_hr++;
          $jj++;
	  if ($start_hr > 23 ) {
	      $start_hr = 0;
              $dayBase = $dayBase2;
	  }
      }

      $ii=0;

      $max_cnt =0;
      if(sql_num_rows( $result ) > 0)
      {
          for ($i=0; $i<sql_num_rows( $result ); $i++)
          {
              list($t_time, $t_count) = sql_fetch_row($result, $dbconn);
              // echo("Query Results ".$t_time . " ".$t_count . "<BR>");
              $comments[$t_time] = $t_count;
	      if ($t_count > $max_cnt) $max_cnt = $t_count;
          }
      }
      /* Test Data 
      for ($i=0; $i <= 23; $i++)
      {
              $t_time = $this->formatTimeOffset($dayBase1, $dayBaseM, $start_hr+$i) ;
              $comments[$t_time] = 24-$i;
      }
      for ($i=0; $i <= 23; $i++)
      {
              $t_time = $this->formatTimeOffset($dayBaseM, $dayBase2, $start_hr+$i) ;
              $comments[$t_time] = $i+5;
      }
      */

      $dayTag = substr($dayBase,0,4). "-" . substr($dayBase,4,2). "-".substr($dayBase,6,2)." ".substr($dayBase,8,2);
     
      ?>
      <p>
      <table width=100%>
      <tr class=admin>
         <th class=admin colspan=6>&nbsp;<?=$timeMsg?>&nbsp;</th>
      </tr>
      <tr class=admin>
	  <td class=admin colspan=6> This screen displays the number of comments published per hour.  </td>
      </tr>
      <tr class=admin>
	  <td class=admin align="center" colspan=6> <?=$this->writeStatsCommentModeBtns();?> </td>
      </tr>
      <tr class="stats">
	 <th class="stats" colspan=2><?=$textLabels["hour"]?> (<?=$dayTag?>)</th>
         <th class="stats" colspan=4><?=$textLabels["amount_posted"]?> <?=$textLabels["hour"]?> </th>
      </tr>
      <?

      if ($twoDayMode == true) {
          if ($system_config->timezone_offset > 0 ) $dayBase = $dayBaseM;
          else $dayBase = $dayBase1;
      } else {
          if ($system_config->timezone_offset > 0 ) $dayBase = $dayBase1;
          else $dayBase = $dayBase2;
      }

      $c_row =0;
      $c_day =0;
      $c_cnt =0;
      $c_total1 =0;
      $c_total2 =0;
      foreach ($comments as $elem => $e_data  ) {  

	   //echo $c_cnt. " elem ".$elem . " e_data ".$e_data."<BR>";
	   // $c_cnt++;

	   if ($c_row == 0 ) {
	       $t_hour = $mapping[substr($elem,8,2)] ;
	       $t_hour .= " <b>".substr($dayBase,0,4). "-" . substr($dayBase,4,2). "-".substr($dayBase,6,2)." ".substr($dayBase,8,2)."</b> ";
	   }
	   // if ($c_row == 0 ) $t_hour = "<b>".substr($dayBase,0,4). "-" . substr($dayBase,4,2). "-".substr($dayBase,6,2)." 00</b>";
	   // else if ($c_row <= 12) $t_hour = substr($elem,8,2). " (" .$c_row . " am)";
	   // else $t_hour = substr($elem,8,2). " (" .($c_row-12) . " pm)";
	   else
	   $t_hour = $mapping[substr($elem,8,2)] ;

	   if ($max_cnt > 20 ) $t_data_adjusted = floor(20 * $e_data / $max_cnt);
           else $t_data_adjusted = $e_data;
	   if ($e_data > 0 ) $t_bar = str_repeat("#",$t_data_adjusted);
	   else $t_bar = str_repeat("#",$t_data_adjusted);

	   if ($c_day == 0 ) {
               $c_total1 = $c_total1 + $e_data;
	   } else {
               $c_total2 = $c_total2 + $e_data;
	   }
	   $disp_class="stats";

	   if (substr($dayBase,6,2) == $current_day && $mapping[substr($elem,8,2)] == $current_hr) $disp_class="stats_bar";
	   //if ($twoDayMode == false && $mapping[substr($elem,8,2)] == $current_hr) $disp_class="stats_bar";

          ?>
          <tr class="<?=$disp_class?>" >
	  <td class="stats" colspan=1><?=$t_hour?> </td>
	     <td class="stats" colspan=1><?=$e_data?></td>
	     <td class="stats" colspan=4><?=$t_bar?> </td>
          </tr>
          <?
	  $c_row++;
	  if ($c_row > 23) {
              $c_row = 0;
              $dayBase = $dayBase2;
	      if ($twoDayMode == true && $c_day == 0) {
                  ?>
                    <tr> <th class="<?=$disp_class?>" colspan=6> </th> </tr>
                    <tr class="<?=$disp_class?>" >
	            <td class="stats" colspan=1> <b>Total</b>  </td>
	            <td class="stats" colspan=1><?=$c_total1?></td>
	            <td class="stats" colspan=4> &nbsp;  </td>
                    </tr>
                  <?
	      }
              $c_day++ ;
	  }
      }
      if ($twoDayMode == true ) $c_total = $c_total2;
      else $c_total = $c_total1;

      ?>
          <tr> <th class="<?=$disp_class?>" colspan=6> </th> </tr>
          <tr class="<?=$disp_class?>" >
	  <td class="stats" colspan=1> <b>Total</b>  </td>
	     <td class="stats" colspan=1><?=$c_total?></td>
	     <td class="stats" colspan=4> &nbsp;  </td>
          </tr>
      </table>
      <?
   }

   function writeShmemLinkHeader()
   {
       global $OSCAILT_SCRIPT, $system_config;

       $summary_mode = "true";
       $summary_msg = "Summary Stats";

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

       $summary_mode = "true";
       $summary_msg = "Summary Stats";

       $month_mode = "true";
       $month_msg = "Monthly Stats";
    
       $daily_mode = "true";
       $daily_msg = "Daily Stats";

       $comment_hourly = "true";
       $comment_msg = "Hourly Stats";

       ?>
         <TABLE class='admin'>
	    <TR class='admin'><TD class='admin'>
<a href="<?=$OSCAILT_SCRIPT?>">MySQL Stats</a> | 
<a href="<?=$OSCAILT_SCRIPT?>?summary=<?=$summary_mode?>"><?=$summary_msg?></a> |
<a href="<?=$OSCAILT_SCRIPT?>?comments=<?=$comment_hourly?>"><?=$comment_msg?></a> |
 <a href="<?=$OSCAILT_SCRIPT?>?daily=<?=$daily_mode?>"><?=$daily_msg?></a> | <a href="<?=$OSCAILT_SCRIPT?>?monthly=<?=$month_mode?>"><?=$month_msg?></a> 
       <?
       if ($first_year < $this_year ) {
            ?> | <a href="<?=$OSCAILT_SCRIPT?>?yearly=true">Yearly Stats</a> <?
       }    
       ?>
            | <a href="<?=$OSCAILT_SCRIPT?>?shmem=true">Shared Memory Stats</a> 
            </TD></TR>
         </TABLE>
       <?
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

   // Display last few Http Referers
   function displayMemoryHttpRefers()
   {
       ?>
       <p>
       <table width=100% cellspacing=2 cellpadding=2>
       <tr class=admin>
           <th class=admin colspan=3> Most Recent HTTP REFERERS Listing <br>
           </th>
       </tr>
       <tr class=admin>
           <th class=admin>&nbsp;#&nbsp;</th>
           <th class=admin colspan=2> HTTP REFERER </th>
       </tr>
       <?
  
       global $system_config;
       global $semaphore_id;
       $http_mode = isset($_SERVER['HTTPS']) ? "https" : "http";
       $url_base = '<a href="'.$http_mode.'://'.$system_config->site_url.'/';

       // May need to check the version of PHP because the get_memory is not available in earlier versions
       // Memory assumed to exist.
       // Store IP and time in a dual memory lifo buffer
       $ShmemId = GetSharedMemoryId("HttpReferList");
       if ($ShmemId == null) {
          ?>
          <tr class=admin> <td class=admin colspan=3> Shared Memory Not Activated </td> </tr>
          </table>
          <?
          return;
       }

       sem_acquire($semaphore_id);
       $ReferListMem = new MemoryStore($ShmemId, STANDARD_MEM, MEM_MODE_READ, 12000);
       $refer_list = $ReferListMem->readMemory();
       $ReferListMem->close();
       sem_release($semaphore_id);

       $ReferArray = explode(" ",$refer_list);
       
       if ($ReferArray == null OR count($ReferArray) == 0 ) {
          ?>
          <tr class=admin>
               <td class=admin colspan=3> No data collected yet </td>
          </tr>
          </table>
          <?
          return;
       }

       $t_Count = 0;
       for ($iCount = 0; $iCount < count($ReferArray); $iCount++)
       {
           if (trim($ReferArray[$iCount]) == "") continue;

           $t_str = $ReferArray[$iCount];
           //$this->writeInfoItem(1, "IP Address ", $t_str );
           $t_Count++;
           ?>
           <tr class=admin>
               <td class=admin align=center><?=$t_Count?></td>
               <td class=admin colspan=2><?=$t_str?></td>
           </tr>
           <?
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
       
       // This is a mode to display last few external HTTP referers
       if ( isset($_REQUEST['referer']) && $_REQUEST['referer'] == 'true') {
           $this->displayMemoryHttpRefers();
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
   function displayPerformance()
   {
      global $system_config, $textLabels;

      ?>
      <p>
           <table width=100% cellspacing=2 cellpadding=2>
           <tr class=admin>
           <th class=admin colspan="2"> MySQL Performance Data </th>
           </tr>
      <?


      $mysql_obj = new mySQLPerformance(array("key_cache_block_size", "key_buffer_size", "max_connections"));

      $this->writeInfoItem(1, "MySQL Version:", $mysql_obj->str_version );


      $this->writeInfoItem(1, "Uptime (secs)", $mysql_obj->getUpTimeSecs());
      $this->writeInfoItem(1, "Uptime ", $mysql_obj->getUpTime());

      $this->writeInfoItem(1, "Connnections", $mysql_obj->getStatus("Connections"));
      $this->writeInfoItem(1, "Aborted_connects", $mysql_obj->getStatus("Aborted_connects"));
      $this->writeInfoItem(1, "Aborted_clients", $mysql_obj->getStatus("Aborted_clients"));
      $this->writeInfoItem(1, "Max Connections", $mysql_obj->getVariable("max_connections"));
      $this->writeInfoItem(1, "Max_used_connections", $mysql_obj->getStatus("Max_used_connections"));
      $this->writeInfoItem(1, "Threads_connected<br><small>Instantaneous value: Refresh screen to sample more</small>", $mysql_obj->getStatus("Threads_connected"));

      $this->writeInfoItem(1,"Key_reads", $mysql_obj->getStatus("Key_reads"));
      $this->writeInfoItem(1,"Key_read_requests", $mysql_obj->getStatus("Key_read_requests") );
      $this->writeInfoItem(1,"<b>Ratio: Key_reads / Key_read_requests</b><br><small>Should be around 0.01%</small>", $mysql_obj->read_ratio ."%" ); 

      $this->writeInfoItem(1,"Key_writes", $mysql_obj->getStatus("Key_writes") );
      $this->writeInfoItem(1,"Key_write_requests", $mysql_obj->getStatus("Key_write_requests"));
      $this->writeInfoItem(1,"<b>Ratio: Key_writes / Key_write_requests</b>", $mysql_obj->write_ratio."%" );

      if ($mysql_obj->version >= 5.0) {
         // 1 - ((Key_blocks_unused * key_cache_block_size) / key_buffer_size)
         $this->writeInfoItem(1,"Key_blocks_unused", $mysql_obj->getStatus("Key_blocks_unused"));
         $this->writeInfoItem(1,"Key_cache_block_size", $mysql_obj->getVariable("key_cache_block_size"));
         $this->writeInfoItem(1,"Key_buffer_size ", $mysql_obj->getVariable("key_buffer_size"));

         $this->writeInfoItem(1,"<b>Ratio: 1 - ((Key_blocks_unused * key_cache_block_size) / key_buffer_size)</b><br><small>Should be close to 100%</small>", $mysql_obj->cache_ratio ."%" );
         $this->writeInfoItem(1,"Key_buffer_size (in blocks)", $mysql_obj->getVariable("key_buffer_size")/ $mysql_obj->getVariable("key_cache_block_size"));
         $this->writeInfoItem(1,"Key_blocks_used", $mysql_obj->getStatus("Key_blocks_used"));
      }

      $this->writeInfoItem(1, "Select_full_join", $mysql_obj->getStatus("Select_full_join"));
      $this->writeInfoItem(1, "Select_scan", $mysql_obj->getStatus("Select_scan"));
      $this->writeInfoItem(1, "Slow_queries", $mysql_obj->getStatus("Slow_queries"));
      $this->writeInfoItem(1, "Table_locks_immediate", $mysql_obj->getStatus("Table_locks_immediate"));
      $this->writeInfoItem(1, "Table_locks_waited", $mysql_obj->getStatus("Table_locks_waited"));
      $t_locks_per_hr = (3600 * $mysql_obj->getStatus("Table_locks_waited"))/$mysql_obj->getUpTimeSecs();
      $t_locks_per_hr = round(100 * $t_locks_per_hr) / 100;
      $this->writeInfoItem(1, "Table_locks_waited (per hour)", $t_locks_per_hr);

      ?>
           </tr>
           </table>
      <?
   }

}
ob_start();

if($editor_session->isSessionOpen())
{
   $adminStats = new statsDisplay();

   // Found in adminutils.inc
   writeAdminHeader($OSCAILT_SCRIPT."",null);

   if ( isset($_REQUEST['shmem']) && $_REQUEST['shmem'] == 'true') 
       $adminStats->writeShmemLinkHeader();
   else
       $adminStats->writeLinkHeader();

   $memory_mode = false;
   if ( isset($_REQUEST['shmem']) && $_REQUEST['shmem'] == 'true') $memory_mode = true;
   if ( isset($_REQUEST['popular']) && $_REQUEST['popular'] == 'true') $memory_mode = true;

   $basic_mode = true;
   if ( isset($_REQUEST['summary']) && $_REQUEST['summary'] == 'true') $basic_mode = false;
   if ( isset($_REQUEST['monthly']) && $_REQUEST['monthly'] == 'true') $basic_mode = false;
   if ( isset($_REQUEST['daily']) && $_REQUEST['daily'] == 'true') $basic_mode = false;
   if ( isset($_REQUEST['yearly']) && $_REQUEST['yearly'] == 'true') $basic_mode = false;
   if ( isset($_REQUEST['comments']) && $_REQUEST['comments'] == 'true') $basic_mode = false;

   // There are 2 modes of display....

   if ($memory_mode == true) {

       $adminStats->writeMemoryStats();

   } else if ($basic_mode == true) {

       $adminStats->displayPerformance();
   } else {

       // $show_details = $adminStats->writeOptionButton();
    
       if(isset($_REQUEST['bycat']) && ($_REQUEST['bycat'] == 'true')) {
           $bycat_mode = "bycat";
       } else {
           $bycat_mode = "";
       }
    
       if(isset($_REQUEST['misc_details']) && ($_REQUEST['misc_details'] == 'true')) {
           $misc_details_mode = true;
       } else {
           $misc_details_mode = false;
       }
    
       if ( isset($_REQUEST['summary']) && $_REQUEST['summary'] == 'true')
       {
           $adminStats->writeStatsBox();
       }
    
       if ( isset($_REQUEST['daily']) && $_REQUEST['daily'] == 'true')
       {
           $show_details = $adminStats->writeOptionButton();
           $daily_objs = $adminStats->getDailyStats();
           $adminStats->writeStatsObjectsBox($show_details, "daily", $daily_objs, $bycat_mode, $misc_details_mode);
       }
    
       if ( isset($_REQUEST['monthly']) && $_REQUEST['monthly'] == 'true')
       {
           $show_details = $adminStats->writeOptionButton();
           if(isset($_REQUEST['month']) && isset($_REQUEST['year']))
           {
               // Put in code to make sure they are valid numbers later...
               $month_objs = $adminStats->getMonthlyStats($_REQUEST['month'], $_REQUEST['year']);
           } else {
               $month_objs = $adminStats->getMonthlyStats();
           }
           $adminStats->writeStatsObjectsBox($show_details, "month", $month_objs, $bycat_mode, $misc_details_mode);
       }

       if ( isset($_REQUEST['yearly']) && $_REQUEST['yearly'] == 'true')
       {
           $show_details = $adminStats->writeOptionButton();
           if(isset($_REQUEST['year']))
           {
               // Put in code to make sure they are valid numbers later...
               $yearly_objs = $adminStats->getYearlyStats($_REQUEST['year']);
           } else {
               $yearly_objs = $adminStats->getYearlyStats();
           }
           $adminStats->writeStatsObjectsBox($show_details, "yearly", $yearly_objs, $bycat_mode, $misc_details_mode);
       }
       if ( isset($_REQUEST['comments']) && $_REQUEST['comments'] == 'true')
       {
           $adminStats->displayCommentHourlyStats();
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

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

require_once("objects/indyruntime/sitefiltereddisplay.inc");
require_once('objects/languagedates.inc');

class eventsDisplay extends siteFilteredDisplay
{
   var $language_dates;

   function eventsDisplay ($obj_id)
   {
      $this->siteFilteredDisplay($obj_id);
   }

   function initialize()
   {
      // Creates an object holding language specific strings for dates like months and days of week
      $this->language_dates = new languageDates();
   }

   function processRequest()
   {
      parent::processRequest();
      $this->initialize();

      SharedMemoryUpdate("events");

      $this->setDisplayOption('showhidden', 'true');
      $this->setDisplayOption('usestatusicons', 'true');
      $this->setDisplayOption('showlocked', 'true');
      $this->setDisplayOption('showsticky', 'true');
      $this->display();
   }

   function display($res = 0)
   {
      global $system_config, $editor_session, $page_title;
      if($this->empty_display)
      {
         echo "<p class=editornotice>problem with site filter settings</p>";
         return;
      }

      $monthly_override = false;
      if( isset($_REQUEST['event_display'])) {
          if( $_REQUEST['event_display'] == "monthly" ) $monthly_override = true;
      }
      // For monthly mode we display a legend
      if($this->getDisplayOption('event_display') == "monthly" || $monthly_override == true) $this->writeEventsHeader(1);
      else $this->writeEventsHeader();

      if($this->getDisplayOption('event_display') == "weekly" && $monthly_override == false)
      {
         addToPageTitle($this->getDisplayOption('weekly_text'));
         $this->getEventsData(1);
      }
      else if($this->getDisplayOption('event_display') == "biweekly" && $monthly_override == false)
      {
         addToPageTitle($this->getDisplayOption('biweekly_text'));
         $this->getEventsData(2);
      }
      else
      {
         addToPageTitle($this->getDisplayOption('monthly_text'));
         $this->getEventsData(0);
      }
      $this->writeEventsFooter();
   }

   function writeEventsFooter()
   {
      echo "</DIV>";
   }

   function setQueryOptions($show_month, $show_year, $loop_count)
    {
      global $system_config, $editor_session;
      $this->query->story_id=null;
      $this->query->use_excludes = true;
      $this->query->comment_id=null;
      $this->query->selectTickerMode();
      //set an upper limit to prevent massive query
      $this->query->results_limit = 500;//max query $this->getDisplayOption('pagelimit');
      $this->query->orderByEventTime();
      $this->query->orderAscending();
      // grab events one month prior to listing request so week display will work
      $this->query->event_time_lower_limit=mktime(0,0,0, $show_month-1, 1, $show_year)-$system_config->timezone_offset;
      $this->query->event_time_upper_limit=mktime(0,0,0, $show_month+$loop_count, 1, $show_year)-$system_config->timezone_offset;
      // override any other time fields passed in request
      $this->query->time_posted_lower_limit=null;
      $this->query->time_posted_upper_limit=null;
      $this->query->event_time=null;
      $this->query->time_posted=null;
      $this->integrateFilters();

      # Forces the type_id to be set to the value for Events
      $this->query->type_id = array($system_config->event_type_id);
      if(count($this->author_names) > 0)
      {
         $this->query->author_name = null;
         $auths = array();
         foreach($this->author_names as $auth_name)
         {
            $auths[]= addslashes($auth_name);
         }
         if($system_config->use_vague_author_name)
         {
            $custom_str = "s.author_name LIKE '%". implode("%' OR s.author_name LIKE '%", $auths)."%'";
         }
         else
         {
            $custom_str = "s.author_name='". implode("' OR s.author_name='", $auths)."'";
         }
         $this->query->addCustomWhereClause($custom_str);
      }
      $this->query->selectFromStoryTableOnly();
   }

   function postQueryFiddling()
   {
      # set fields back to defaults so they don't show in query string
      $this->query->type_id=null;
      $this->query->event_time_lower_limit=null;
      $this->query->event_time_upper_limit=null;
      $this->query->orderDescending();
      $this->query->orderByTimePosted();
      global $pageRegion, $pageTopic, $pageLanguage;
      if(isset($pageRegion))
      {
         $this->onwards_query->region_id = $pageRegion->region_id;
      }
      if(isset($pageTopic))
      {
         $this->onwards_query->topic_id = $pageTopic->topic_id;
      }
      if(isset($pageLanguage))
      {
         $this->onwards_query->language_id = $pageTopic->language_id;
      }
   }

   function writeEventsHeader( $show_legend=0)
   {
      global $pageFilterText, $graphics_store;
      $event_icon = $graphics_store."event.png";
      $img_txt = "<IMG style='vertical-align: middle' alt='Event' src='$event_icon'>";
      $cls_str = "class='events-header'";
      echo "<DIV $cls_str>\n";
      echo '<DIV class="events-viewoptions">';
      $this->writeCalendarViewOptions();
      echo "</DIV>";

      echo "<H3 $cls_str>$img_txt ".$pageFilterText." ".$this->getDisplayOption('title')."</H3>\n";
      echo "<P $cls_str>".$this->getDisplayOption('introtext')."</P>";
      $this->writePublishLink();
      echo "</DIV>";
      if ($show_legend == 1) $this->writeEventsLegend() ;
      echo "<DIV class='eventcalendar'>\n";
   }

   function writeEventsLegend()
   {
      echo "<DIV class='eventcalendar_legend'>";
      echo "<TABLE BORDER=0>";
      echo "<TR><TD class='eventcalendarpast'> &nbsp; Past Events </TD></TR>";
      echo "<TR><TD class='eventcalendarhighlighted'> &nbsp; <b> <a href='#todays_events'>Today's Events</a></b> &nbsp;</TD></TR>";
      echo "<TR><TD class='eventcalendarfuture'> &nbsp; Future Events </TD></TR>";
      echo "</TABLE>";
      echo "</DIV>";
   }

   function writePublishLink()
   {
      global $system_config;
      $cls_str = "class='events-publishlink'";
      echo "<div $cls_str>\n";
      $publishquery = new StoryQuery();
      if(isset($this->onwards_query) and $this->onwards_query != null)
      {
         $publishquery = $this->onwards_query->copyFilters($publishquery);
      }
      $publishquery->type_id = $system_config->event_type_id;
      $lnk = $this->getFilterLink($this->site_settings['publishobject'], false, $publishquery);
      # echo "Site settings $this->site_settings['publishobject'] <br> and link: $lnk";
      echo "<a href='$lnk' rel='nofollow' $cls_str>Publish An Event Notice</a>";
      echo "</div>\n";
   }




   function writeCalendarViewOptions()
   {
      global $user_prefs;
      //all of our non-standard view stuff should be added to links....
      $msg = strtolower($this->getDisplayOption('weekly_text'));
      if($this->getDisplayOption('event_display') == "weekly")
      {
         echo '<span class="selected-option">'.$msg.'</span>';
      }
      else
      {
         $user_prefs->setQueryOption("event_display", "weekly");
         echo '<A class="view-option" href="'.$this->getFullLink().'">'.$msg."</a>\n";
         $user_prefs->unsetQueryOptions();
      }
      echo "<br />";

      $msg = strtolower($this->getDisplayOption('biweekly_text'));
      if($this->getDisplayOption('event_display') == "biweekly")
      {
         echo '<span class="selected-option">'.$msg.'</span>';
      }
      else
      {
         $user_prefs->setQueryOption("event_display", "biweekly");
         echo '<A class="view-option" href="'.$this->getFullLink().'">'.$msg."</a>\n";
         $user_prefs->unsetQueryOptions();
      }
      echo "<br />";
      $msg = strtolower($this->getDisplayOption('monthly_text'));
      if($this->getDisplayOption('event_display') == "monthly")
      {
         echo '<span class="selected-option">'.$msg.'</span>';
      }
      else
      {
         $user_prefs->setQueryOption("event_display", "monthly");
         echo '<A class="view-option" href="'.$this->getFullLink().'">'.$msg."</a>  \n";
         $user_prefs->unsetQueryOptions();
      }
      echo "<br />";
      $this->writeSavePrefsLink();
   }


   function generateMonthlyEventCounts($current_year, $events )
   {
      global $system_config;
      $myArray = Array( 0 => "0", 1 => "0", 2 => "0", 3 => "0", 4 => "0", 5 => "0",
              6 => "0", 7 => "0", 8 => "0", 9 => "0", 10 => "0", 11 => "0");
      for ($each_event = 0; $each_event < count($events); $each_event++)
      {
         # Check the event start date so that only events for the current
         # month are displayed.
         $this_event = $events[$each_event];
         # date pulls out month valued between 1 and 12, no leading zeros
         $adjusted_time = $this_event->event_time + $system_config->timezone_offset;
         $event_month = date("n", $adjusted_time);
         if ( (($event_month > 0 && $event_month < 13 )) &&
              (date("Y", $adjusted_time) == $current_year ) )
         {
            $myArray[ ($event_month -1) ]++;
         }
         else
         {
            //echo "<p>date $event_month /".date("Y", $adjusted_time)." is not $current_year</p>";
         }
      }
      return $myArray;
   }

   function writeEventsWeekList($events, $show_day, $dofw, $show_month, $current_year, $display_year, $week_mode )
   {
      global $system_config;
      # Do a weekly display. The prev/next links are tricky, because should prev month go back to
      # the start of the month or what.
      # Also previous week is awkard to handle, because a month boundary might fall in the middle.

      # Depends what the day of the week is of the passed day, subtracting -6 could put you back
      # into the previous month. We know Dec is 31 days

      $active_month = $show_month;
      $month_changes = 0;
      //echo("day to show $show_day day_of_week $dofw display_year $display_year <br>");
      if ($show_day <= $dofw )
      {
         if ($show_month > 1 ) $day_value = monthSize( $show_month-1, $display_year ) + ($show_day - $dofw );
         else $day_value = 31 + ($show_day - $dofw);

         if ($show_month > 1 ) $active_month--;
         else $active_month = 12;

         $month_changes = 1;
      }
      else
         $day_value = $show_day - $dofw;

      #echo("day_value is now $day_value <br>");

      if ( ($day_value + 7) > monthSize($show_month, $display_year) ) $month_changes = 1;
      $first_day_text = $day_value;
      $last_day_text = $day_value + 6;
      if ($last_day_text > monthSize($show_month, $display_year) )
      {
         if ($show_month < 12 ) $last_day_text = $last_day_text - monthSize($show_month, $display_year);
         else $last_day_text = $last_day_text - 31;
      }
      $th_text = $this->getNumber_Th_Text( $day_value );
      $first_day_text .= $th_text;

      $th_text = $this->getNumber_Th_Text( $last_day_text);
      $last_day_text .= $th_text;

      $year = $display_year;
      # Display the selected Month and Year
      $month_text = $this->monthString($show_month);

      ?> <p align="center"> <strong><font size="6"> <?

      global $user_prefs;
      if($week_mode == 1)
      {
         $user_prefs->setQueryOption("event_display", "weekly");
         $event_url_prefix = $this->getFullLink();
         $event_url_prefix = addLinkParameters($event_url_prefix, 'display=weekly');
         $user_prefs->unsetQueryOptions();
      }
      else
      {
         $user_prefs->setQueryOption("event_display", "biweekly");
         $event_url_prefix = $this->getFullLink();
         $event_url_prefix = addLinkParameters($event_url_prefix, 'display=biweekly');
         $user_prefs->unsetQueryOptions();
      }

      $url_prev_month = $event_url_prefix;
      $url_next_month = $event_url_prefix;

      $url_prev_week = $event_url_prefix;
      $url_next_week = $event_url_prefix;

      $prev_month = $show_month -1;
      $next_month = $show_month +1;

      $prev_wk_month = $show_month;
      $next_wk_month = $show_month;
      $prev_wk_yr = $display_year;
      $next_wk_yr = $display_year;

      $prev_week = $show_day -7;
      // echo("prev week day no is $prev_week  <br>");
      if ( $prev_week < 1 )
      {
         if ($prev_wk_month > 1 ) { $prev_week = monthSize($prev_wk_month-1,$display_year) + $prev_week; $prev_wk_month--; }
         else { $prev_week = 31 + $prev_week; $prev_wk_month = 12; $prev_wk_yr = $display_year - 1; }
      }
      // echo("prev week day no is $prev_week  <br>");
      $next_week = $show_day +7;
      # $dm =  monthSize($show_month) ;
      #echo("next week day no is $next_week  $dm <br>");
      if ( $next_week > monthSize($show_month, $display_year) )
      {
         if ($next_wk_month < 12 ) { $next_week = $next_week - monthSize($next_wk_month,$display_year); $next_wk_month++; }
         else { $next_week = $next_week - 31; $next_wk_month = 1; $next_wk_yr = $display_year + 1; }
      }

      $url_prev_month .= "&amp;day=1";
      $url_next_month .= "&amp;day=1";

      $url_prev_week .= "&amp;day=" . $prev_week;
      #if ($prev_wk_month != $show_month ) $url_prev_week .= "&amp;month=" . $prev_wk_month;
      $url_prev_week .= "&amp;month=" . $prev_wk_month;
      if ($prev_wk_yr != $current_year ) $url_prev_week .= "&amp;year=" . $prev_wk_yr;

      $url_next_week .= "&amp;day=" . $next_week;
      #if ($next_wk_month != $show_month ) $url_next_week .= "&amp;month=" . $next_wk_month;
      $url_next_week .= "&amp;month=" . $next_wk_month;
      if ($next_wk_yr != $current_year ) $url_next_week .= "&amp;year=" . $next_wk_yr;

      $add_year_tag = 0;
      if ($year != $current_year)
      {
         $add_year_tag = 1;
         $year_url_text = "&amp;year=" . $year;
      }

      if ($prev_month < 1 ) { $prev_month = 12; $url_prev_month .= "&amp;year=" . ($year-1) ; }
      else if ($add_year_tag == 1) { $url_prev_month .= $year_url_text; }
      if ($next_month > 12 ) { $next_month = 1; $url_next_month .= "&amp;year=" . ($year+1) ; }
      else if ($add_year_tag == 1) { $url_next_month .= $year_url_text; }
      ?>
      <a href="<?=$url_prev_month?>&amp;month=<?=$prev_month?>"> &lt;&lt;&lt;</a>
      &nbsp; <?=$month_text?> <?=$year?> &nbsp;
      <a href="<?=$url_next_month?>&amp;month=<?=$next_month?>"> &gt;&gt;&gt;</a>
      </font></strong>
      <br>

      <small>
      <a href="<?=$url_prev_week?>"> &lt;&lt;&lt; prev week</a>
      &nbsp; Sun <?=$first_day_text?>  - Sat <?=$last_day_text?> &nbsp;
      <a href="<?=$url_next_week?>">next week &gt;&gt;&gt;</a>
      </small>
      <p>
      <center>
      <table class=eventcalendar cellpadding="3" cellspacing="0">
      <tr class=eventcalendar>
      <?
      # The code here writes out the event table. First the rows with Sun Mon ... Sat with dates
      # followed below by the actual events.
      # Write out 7 days headings with a box for each.
      $day_count = $day_value;
      $month_count = $active_month;
      for ($i = 1; $i <= 7; $i++)
      {
         $cell_text = "";
         if ( ($i == 1 ) && ($month_changes == 1)) $cell_text = "<b>" . $this->monthString($active_month) . "</b><BR>";
         if ( ($day_count == 1 ) && ($month_changes == 1)) $cell_text = "<b>" . $this->monthString($month_count) . "</b><BR>";
         $th_text = $this->getNumber_Th_Text($day_count );
         $cell_text .= $this->weekDayLongString($i-1) . " " . $day_count . $th_text;
         if ($day_count == $show_day ) { ?> <th class=eventcalendarhighlighted> <?=$cell_text?> </th> <?
         }
         else { ?> <th class=eventcalendar> <?=$cell_text?> </th> <?  }
         $day_count = handleDayRollover($day_count, $month_count, $display_year );
      }
      ?>
      </tr><tr class=eventcalendar>
      <?
      # Write out whether there are events on these days now.
      $day_count = $day_value;
      $month_count = $active_month;
      $save_query = $this->query->copy();
      $this->query = new StoryQuery();
      for ($i = 1; $i <= 7; $i++)
      {
         $daily_count = 0;
         if ($day_count == $show_day ) { ?> <td class=eventcalendarhighlighted> <?  }
         else { ?> <td class=eventcalendar> <? }
         for ($each_event = 0; $each_event < count($events); $each_event++)
         {
            # Check the event start date so that only events for the current
            # month are displayed.
            $this_event = $events[$each_event];
            $adjusted_time = $this_event->event_time+$system_config->timezone_offset;
            # Let outer loop check for month, because we actually trawl up a lot more events
            # from other months, so most of the time this check is enough to eliminate them
            if (date("n", $adjusted_time) == $month_count )
            {
               if ( (date("j", $adjusted_time) == $day_count ) &&
                    (date("Y", $adjusted_time) == $display_year ) )
               {

                  $lnk = $this->getStoryLink($this_event->story_id);
                  echo $this->getItemHighlights($this_event, false);
                  echo "<a href='$lnk' style='text-decoration:none'> ";
                  echo $this_event->story_title;
                  echo '</a><br><br>';
                  $daily_count++;
               }
            }
         }
         if ($daily_count == 0)
         {
            $cell_text = "No events";
            ?> <?=$cell_text?> <?
         }
         ?> </td> <?
         $day_count = handleDayRollover($day_count, $month_count, $display_year );
      }
      $this->query = $save_query;
      ?>
      </tr>
      <!--
      </table>
      </center>
      </p>
      -->
      <?
      if ($week_mode == 2) {
         ?>
         <tr class=eventcalendar><td colspan=7 > &nbsp; </td></tr>
         <tr class=eventcalendar><td colspan=7 > &nbsp; </td></tr>
         <tr class=eventcalendar>
         <?
         # ****************************** Code for drawing the second week *************************
         # Write out 7 days with a box for each.
         $month_changed = $month_changes;
         $month_changes = 0;
         $start_day = $day_count;
         $start_month = $month_count;
         if ( ($day_count + 7) > monthSize($show_month, $display_year) ) $month_changes = 1;
         for ($i = 1; $i <= 7; $i++)
         {
            $cell_text = "";
            if ( ($i == 1 ) && ($month_changes == 1)) $cell_text = "<b>" . $this->monthString($active_month) . "</b><BR>";
            if ( ($day_count == 1 ) && ($month_changes == 1)) $cell_text = "<b>" . $this->monthString($month_count) . "</b><BR>";
            if ( ($day_count == 1 ) && ($month_changed == 1)) $cell_text = "<b>" . $this->monthString($month_count) . "</b><BR>";
            $th_text = $this->getNumber_Th_Text($day_count );
            $cell_text .= $this->weekDayLongString($i-1) . " " . $day_count . $th_text;
            ?> <th class=eventcalendar> <?=$cell_text?> </th> <?
         $day_count = handleDayRollover($day_count, $month_count, $display_year );
         }
         ?>
         </tr><tr class=eventcalendar>
         <?
         # Write out whether there are events on these days now. Month already set.
         // $day_count = $day_value + 7;
         $day_count = $start_day;
         $month_count = $start_month;
         $save_query = $this->query->copy();
         $this->query = new StoryQuery();
         for ($i = 1; $i <= 7; $i++)
         {
            $daily_count = 0;
            ?> <td class=eventcalendar> <?
            for ($each_event = 0; $each_event < count($events); $each_event++)
            {
               # Check the event start date so that only events for the current
               # month are displayed.
               $this_event = $events[$each_event];
               $adjusted_time = $this_event->event_time+$system_config->timezone_offset;
               # Let outer loop check for month, because we actually trawl up a lot more events
               # from other months, so most of the time this check is enough to eliminate them
               if (date("n", $adjusted_time) == $month_count )
               {
                  if ( (date("j", $adjusted_time) == $day_count ) &&
                       (date("Y", $adjusted_time) == $display_year ) )
                  {
                     $lnk = $this->getStoryLink($this_event->story_id);
                     echo $this->getItemHighlights($this_event, false);
                     echo "<a href='$lnk' style='text-decoration:none'> ";
                     echo $this_event->story_title;
                     echo '</a><br><br>';
                     $daily_count++;
                  }
               }
            }
            if ($daily_count == 0)
            {
               ?> No events <?
            }
            ?> </td> <?
            $day_count = handleDayRollover($day_count, $month_count, $display_year );
         }
         $this->query = $save_query;
         ?> </tr> <?
      }
      ?>
      </table>
      </center>
      <?

   }

   // This displays the events listing in a monthly fashion.
   function writeEventsList($events, $start_month, $loop_count, $current_year, $display_year, $today_text )
   {
      global $system_config;
      # This function does the display for the monthly list. Currently shows 2 months at
      # a time with forward and backward links.

      # For dates in the futures, display in increasing order, otherwise reverse
      $year = $display_year;
      $month_index = $start_month;
      /*$querystring=$query->getQueryString();
      if(strlen($querystring)>0) $querystring=$querystring."&";
      else $querystring="?";
      $url_prev_script = "eventcalendar.php".$querystring;*/

      $url_prev_script = $this->getFullLink().'&';
      $url_next_script = $url_prev_script;

      $prev_month = $start_month -1;
      $next_month = $start_month +1;

      $add_year_tag = 0;
      if ($year != $current_year)
      {
         $add_year_tag = 1;
         $year_url_text = "year=" . $year;
      }

      if ($prev_month < 1 ) { $prev_month = 12; $url_prev_script .= "year=" . ($year-1) . "&"; }
      else if ($add_year_tag == 1) { $url_prev_script = $url_prev_script . $year_url_text ."&"; }

      if ($next_month > 12 ) { $next_month = 1; $url_next_script .= "year=" . ($year+1) . "&" ; }
      else if ($add_year_tag == 1) { $url_next_script = $url_next_script . $year_url_text ."&"; }

      ?>
      <table class=eventcalendar border="0" cellpadding="0" cellspacing="0" width="100%">
      <tr>
      <td align=left><small><b><a href="<?=$url_prev_script?>month=<?=$prev_month?>">&lt;&lt;&lt; <?=$this->getDisplayOption('prevtext')?></a></b></small></td>
      <td align=right><small><b><a href="<?=$url_next_script?>month=<?=$next_month?>"> <?=$this->getDisplayOption('nexttext')?> &gt;&gt;&gt;</a></b></small></td>
      </tr>
      </table>
      <table class=eventcalendar cellpadding="3" cellspacing="0" width="100%">
      <?
      $monthly_counts = $this->generateMonthlyEventCounts($year, $events);

      $save_query = $this->query->copy();
      $this->query = new StoryQuery();

      // This is only used for the special case where the first month display is the current month
      $show_past_events = true;
      if( isset($_REQUEST['showpast'])) {
          if ($_REQUEST['showpast'] == 'true') {
              $show_past_events = true;
          }
	  if ($_REQUEST['showpast'] == 'false') $show_past_events = false;
      }

      $none_today_drawn = 0;

      $date_label = "Date";
      if ($this->getDisplayOption('datelabel_text') != "") $date_label = $this->getDisplayOption('datelabel_text');
      $title_label = "Title";
      if ($this->getDisplayOption('titlelabel_text') != "") $title_label = $this->getDisplayOption('titlelabel_text');
      $region_label = "Region";
      if ($this->getDisplayOption('regionlabel_text') != "") $region_label = $this->getDisplayOption('regionlabel_text');

      for ($i = 1; $i <= $loop_count; $i++)
      {
         if ($month_index > 12 )
         {
            $month_index = 1;
            $year++;
         }

         # Display the current month and start the table
         $month_text = $this->monthString($month_index);
         # For the first loop, add in the URL links for Next/Prev
         ?> <tr class=eventcalendar> <?
         if ( $month_index == date("n") && $year == date("Y"))
            { ?> <th class=eventcalendarhighlighted colspan = "3"> <?;}
         else { ?> <th class=eventcalendar colspan = "3"> <? ; }
         ?> <center><font size="+2"> <?=$month_text?> <?=$year?> <?
         ?> </font> (<i>Total of <?=$monthly_counts[($month_index -1)] ?> events</i>)</center></th></tr>
         <tr class=eventcalendar>
             <th class=eventcalendar width="90"><?=$date_label?></td>
             <th class=eventcalendar width="75%"><?=$title_label?></td>
             <th class=eventcalendar ><?=$region_label?></td>
             </tr>
         <?
         if (count ($events) <= 0 )
         {
            ?><tr class=eventcalendar ><td class=eventcalendar colspan=3> <br>No events planned for this month yet <br></td> </tr><?
         }
         else
         {
            $month_count = 0;
            $today_total = 0;
            $past_total = 0;

            for ($each_event = 0; $each_event < count($events); $each_event++)
            {
               # Check the event start date so that only events for the current
               # month are displayed.
               $this_event = $events[$each_event];
               // Is this quicker by saving 2 adds and 3 some of the time?
               $adjusted_time = $this_event->event_time+$system_config->timezone_offset;
               if ( (date("n", $adjusted_time) == $month_index ) &&
                    (date("Y", $adjusted_time) == $year ) )
               {
                  $display_style = "";
                  $region = $this_event->getRegion();
                  // For numeric comparison, go Year, Month, Day as day, Month Year would be wrong.
                  $event_date_text = date("Ymd", $adjusted_time);
                  if ($event_date_text == $today_text) 
		  { 
                      $calendar_class = "eventcalendarhighlighted";
                      $today_total++;
	          }
                  else if ($event_date_text < $today_text) { 
                      $past_total++;
                      $calendar_class = "eventcalendarpast"; 
		      if($show_past_events == true) $display_style = "style = 'display: none;'";

		      if($past_total == 1 ) {
                          if($show_past_events == true) $btn_txt = "Show Past Events for " . $month_text;
			  else $btn_txt = "Hide Past Events for " .$month_text;

                          $btn_txt = "<<<   " . $btn_txt . "   >>>";

                          ?> <tr class=eventcalendar><td colspan=3 class='eventcalendarpast' style='text-align:center'><?
                          $this->writeShowHideBtn($btn_txt, $show_past_events, $start_month, $display_year);
                          ?></td></tr><?
                      }
		  }
                  else { $calendar_class = "eventcalendarfuture"; }

                  $lnk = $this->getStoryLink($this_event->story_id);
                  if ($event_date_text > $today_text && $month_count > 0 && $today_total==0 && $none_today_drawn==0 && $month_index == date("n") && $year == date("Y"))
                  {
                      $none_today_drawn = 1;
                      $today = time()+$system_config->timezone_offset;
		      // Code was: date("D jS",$today);
                      ?> <tr class=eventcalendar> 
		      <TD class="eventcalendarhighlighted"> <?=$this->formatEventDate($today);?> </td>
		      <TD class="eventcalendarhighlighted"> <a name="todays_events"> <b>No Events Today </b></TD>
		      <TD class="eventcalendarhighlighted"> N/A </td></tr>
                      <?
                  }
		  $formatted_date_text = $this->formatEventDate($adjusted_time);
                  ?> <tr class=eventcalendar <?=$display_style?>> 
		  <TD class=<?=$calendar_class?>> <?=$formatted_date_text?> </td>
                  <TD class=<?=$calendar_class?>>
                  <?
                  if ($event_date_text == $today_text && $today_total ==1)
                  {
		      echo('<a name="todays_events">');
                  }
                  ?> 
                  <?=$this->getItemHighlights($this_event, false);?>
                  <a href="<?=$lnk?>"> <?=$this_event->story_title?> </a></td>
                  <TD class=<?=$calendar_class?> width="6%">
                  <?=$region->region_name?> </td></tr>
                  <?
                  $month_count++;
               }
            }
            if ($month_count == 0 )
            {
               ?> <tr><td colspan=3> <br> No events planned for this month yet <br>&nbsp;</td> </tr><?
            } else {
               // You can have the situation where it displays events before today, but there is none
               // for today or the days afterwards. We still need to say no events today though.
               if ($today_total==0 && $none_today_drawn==0 && $month_index == date("n") && $year == date("Y"))
               {
                      $none_today_drawn = 1;
                      $today = time()+$system_config->timezone_offset;
		      // Code was: date("D jS",$today);
                      ?> <tr class=eventcalendar> 
		      <TD class="eventcalendarhighlighted"> <?=date("D jS",$today);?> </td>
		      <TD class="eventcalendarhighlighted"> <a name="todays_events"> <b>No Events Today </b></TD>
		      <TD class="eventcalendarhighlighted"> N/A </td></tr>
                      <?
               }
            }
         }
         $month_index++;
      }
      $this->query = $save_query;
      ?>
      </table>
      <table border="0" cellpadding="0" cellspacing="0" width="100%">
      <tr>
      <td align=left><small><b><a href="<?=$url_prev_script?>month=<?=$prev_month?>">&lt;&lt;&lt; <?=$this->getDisplayOption('prevtext')?></a></b></small></td>
      <td align=right><small><b><a href="<?=$url_next_script?>month=<?=$next_month?>"> <?=$this->getDisplayOption('nexttext')?> &gt;&gt;&gt;</a></b></small></td>
      </tr> </table>
      <br><br>
      <?
   }

   function writeShowHideBtn($btn_txt, $showpast_value, $start_month, $display_year)
   {
      echo '<FORM name="calendarform" enctype="multipart/form-data" action="';
      $lnk = $this->getSimpleLink();
      $lnk = addLinkParameters($lnk, "event_display=monthly");
      echo $lnk;
      echo '" method="post">';
      echo "<INPUT type='submit' name='showpastbtn' value='$btn_txt'>\n";

      if ($showpast_value == false) echo "<INPUT type='hidden' name='showpast' value='true'>";
      else echo "<INPUT type='hidden' name='showpast' value='false'>";

      echo "<INPUT type='hidden' name='year' value='$display_year'>";
      echo "<INPUT type='hidden' name='month' value='$start_month'>";

      echo "\n</FORM>";

   }

   function getEventsData($mode)
   {
      global $system_config;
      # Assume user wants to display info for today but selected days, months and years
      # can be passed thru'
      # However the internals of the display like to know what is the current year so as to
      # cut out uncessary crap in the url generation.
      $today = getdate(time()+$system_config->timezone_offset);
      // For numeric comparison, go Year, Month, Day as day, Month Year would be wrong.
      $formatted_today = date("Ymd", (time()+$system_config->timezone_offset));
      $cur_day = $today['mday'];
      $cur_year = $today['year'];
      $cur_month = $today['mon'];
      $show_year = $cur_year;

      if (isset($_REQUEST['year']))
      {
         $show_year = $_REQUEST['year'];
         # prevent calendar from going back before start of site and no more than one year ahead
         if ($show_year < 2001 || $show_year > ($cur_year+1) ) $show_year = $today['year'];
      }
      if (isset($_REQUEST['month']))
      {
         $cur_month = $_REQUEST['month'];
         if ($cur_month <= 0 || $cur_month > 12 ) $cur_month = $today['mon'];
      }

      $typeList = new Type();

      # Control the start month to display and number to show for the by monthly display
      $start_month = $cur_month;
      $loop_count = 3;

      $show_day = $cur_day;
      $show_month = $cur_month;

      if (isset($_REQUEST['day']))
      {
         $show_day = $_REQUEST['day'];
         if ($show_day <= 0 || $show_day > 31 ) $show_day = $cur_day;
         // Have to take account of time zone difference and add +1 sec
                   $timezone_hours = $system_config->timezone_offset / 3600;
         $day_of_week = date("w", mktime($timezone_hours,0,1, $show_month, $show_day, $show_year));
                   //echo("mon $show_month day $show_day year $show_year day_of_week $day_of_week <br>");
      }
      else
         $day_of_week = $today['wday'];
      $this->setQueryOptions($show_month, $show_year, $loop_count);
      $events = $this->query->execute();
      $this->postQueryFiddling();


      if ($mode == 0 )
      {
         $this->writeEventsList($events, $start_month, $loop_count, $cur_year, $show_year, $formatted_today);
      }
      else
      {
         # Mode can now be 1 or 2 which is show 1 week or 2 at a time
         $this->writeEventsWeekList($events, $show_day, $day_of_week, $show_month, $cur_year, $show_year, $mode);
      }
   }

   function formatEventDate($event_datetime) 
   {

       if ($this->language_dates->isEnglish() == true )
       {
           return date("D jS",$event_datetime);
       }

       // Otherwise we have to convert it ourselves because setlocale only works on strftime and it doesn't
       // produce what we want. So return a day number 0-6 (Sun-Sat) and then the month number. 1-31
       $tmp_date = date("wj",$event_datetime);
       $day_text = $this->language_dates->weekDayLongString( substr($tmp_date,0,1));
       $th_string = $this->language_dates->getNumber_Th_Text( substr($tmp_date,1));
       $day_text = substr($day_text,0,3) . " ". substr($tmp_date,1) . $th_string;
       return $day_text;
   }

   function monthString($month_number) 
   {
       return $this->language_dates->monthString( $month_number);
   }
   function weekDayLongString($weekday) 
   {
       return $this->language_dates->weekDayLongString( $weekday);
   }

   function getNumber_Th_Text($day_value )
   {
       return $this->language_dates->getNumber_Th_Text( $day_value);
   }
}


function monthSize($month_number, $year_number) {
   $myArray = Array( 0 => "31", 1 => "28", 2 => "31", 3 => "30", 4 => "31", 5 => "30",
           6 => "31", 7 => "31", 8 => "30", 9 => "31", 10 => "30", 11 => "31");

   if ($month_number == 2)
   {
      // If Feb then see if this is a leap year.
      if ( date("L", mktime(0,0,0,2,1,$year_number)) )
         return 29;
      else
         return 28;
   }
   else
      return $myArray[$month_number-1];
}

function handleDayRollover($current_day, &$this_month, $the_year )
{
   $current_day++;
   if ($current_day > monthSize( $this_month, $the_year) )
   {
      $current_day = 1;
      $this_month++;
      if ($this_month > 12 ) $this_month = 1;
   }

   return $current_day;
}

//function getNumber_Th_Text($day_value )
//{
//   $th_text = "th";
//   if ($day_value > 3 && $day_value < 21 ) return $th_text;
//   else if ($day_value > 23 && $day_value < 31 ) return $th_text;
//   else if ($day_value == 1 ) $th_text = "st";
//   else if ($day_value == 2 ) $th_text = "nd";
//   else if ($day_value == 3 ) $th_text = "rd";
//   else if ($day_value == 21 ) $th_text = "st";
//   else if ($day_value == 22 ) $th_text = "nd";
//   else if ($day_value == 23 ) $th_text = "rd";
//   else if ($day_value == 31 ) $th_text = "st";
//   return $th_text;
//}

?>

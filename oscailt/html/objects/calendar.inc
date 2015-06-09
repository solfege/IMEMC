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

require_once('objects/languagedates.inc');

class Calendar
{
   var $today;
   var $cur_day;
   var $cur_year;
   var $cur_month;
   var $startdate;
   var $first_day;
   var $first_year;
   var $first_month;

   var $own_url;
   var $year;
   var $prevtxt;
   var $nexttxt;


   var $partial_display;
   var $begin_month;
   var $end_month;

   var $language_dates;

   var $override_url_gen;

   var $url="newswire.php";

   function Calendar()
   {
      global $system_config;
      # Default is to display them
      $this->override_url_gen = true;
      $this->partial_display = false;
      $this->begin_month = 3;
      $this->end_month = 1;


      $this->today = getdate(time()+$system_config->timezone_offset);
      $this->cur_day = $this->today['mday'];
      $this->cur_year = $this->today['year'];
      $this->cur_month = $this->today['mon'];

      $this->startdate = $this->getStartDate();
      $this->first_day = $this->startdate['mday'];
      $this->first_year = $this->startdate['year'];
      $this->first_month = $this->startdate['mon'];

      $this->language_dates = new languageDates();
      # Avoid any strange hacks to crash it.
      #echo(" year $cur_year month $cur_month day $cur_day ");
      if ($this->cur_year < $this->first_year ) $this->cur_year = $this->first_year;

      #for ($year=$cur_year; $year >= $first_year; $year--)
      #{
      #   $this->writeYearBlock($year, $cur_year, $cur_month, $cur_day, $first_year, $first_month, $first_day);
      #}
   }
   function showNoUrls()
   {
      $this->override_url_gen = false;
   }

   function drawFullCalendarYear( $target_year)
   {
      if ($target_year > $this->cur_year ) $target_year = $this->cur_year;

      $this->writeYearBlock($target_year, $this->cur_year, $this->cur_month, $this->cur_day,
             $this->first_year, $this->first_month, $this->first_day, true);
   }

   function drawFullCalendar()
   {
      for ($year = $this->cur_year; $year >= $this->first_year; $year--)
      {
         $this->writeYearBlock($year, $this->cur_year, $this->cur_month, $this->cur_day,
                     $this->first_year, $this->first_month, $this->first_day, false);
      }
   }

   function drawMonth( $display_month)
   {
      if ($display_month < 1 || $display_month > 12 ) $display_month = $first_month;

      $this->partial_display = true;

      if ($display_month > 9) $this->begin_month = 12;
      else $this->begin_month = $display_month;

      if ($display_month > 2 ) $this->end_month = $this->begin_month - 2;
      else $this->end_month = 1;

      #echo("begin $this->begin_month end $this->end_month <br> ");
      #for ($year=$cur_year; $year >= $first_year; $year--)

      $this->year = $this->cur_year;

      #echo("year $this->year cur_year $this->cur_year disp_mon $display_month day $this->cur_day 1st yr $this->first_year <br>");

      $this->writeYearBlock($this->year, $this->cur_year, $display_month, $this->cur_day,
                  $this->first_year, $this->first_month, $this->first_day, false);
   }

   function weekDayString($weekday) {
      $myArray = Array( 0 => "Sun", 1 => "Mon", 2 => "Tue", 3 => "Wed", 4 => "Thu", 5 => "Fri", 6 => "Sat");
      return $myArray[$weekday];
   }
   function monthString($month_number)
   {

      //$myArray = Array( 0 => "January", 1 => "February", 2 => "March", 3 => "April", 4 => "May", 5 => "June",
      //        6 => "July", 7 => "August", 8 => "September", 9 => "October", 10 => "November", 11 => "December");
      //return $myArray[$month_number-1];

      return $this->language_dates->monthString( $month_number);
   }
   function abrevMonthString($month_number)
   {

      $myArray = Array( 0 => "Jan", 1 => "Feb", 2 => "Mar", 3 => "Apr", 4 => "May", 5 => "Jun",
              6 => "Jul", 7 => "Aug", 8 => "Sep", 9 => "Oct", 10 => "Nov", 11 => "Dec");
      return $myArray[$month_number-1];
   }

   function monthSize($month_number)
   {
      $myArray = Array( 0 => "31", 1 => "28", 2 => "31", 3 => "30", 4 => "31", 5 => "30",
              6 => "31", 7 => "31", 8 => "30", 9 => "31", 10 => "30", 11 => "31");

   return $myArray[$month_number-1];
   }

   function writeYearBlock( $year, $this_year, $this_month, $this_day, $first_year, $first_month, $first_day, $single_year)
   {
      $col_width  = 3; # Number of months to display across page
      $last_day  = 0; # Used for rolling over day of the week for the 1st of each month
      $active_start  = 0; # => All days active if set to zero
      # colspan=3 could be replaced by a variable in case you want 4 months across

      if ($year < $first_year ) $year = $first_year;

      $year_header_text = $year;
      if ($this->partial_display == true ) $year_header_text = "";

   ?>
      <TABLE class=calendar cellSpacing="8" cellPadding="2" border="0" align="center">
      <TBODY>
      <TR class=calendar>
   <?
      if ( $single_year == true )
      {
         $url_string = $this->own_url;
         if(isset($_REQUEST["archive"]) && $_REQUEST["archive"]=="gallery")
         {
            $url_string = addLinkParameters($url_string, array('archive=gallery'));
         }
         $url_string .= '&amp;year=';

         if (($year-1) >= $first_year)
            $prev_url_string = $url_string . ($year -1);
         else
            $prev_url_string = $url_string . $year;

         $next_url_string = $url_string . ($year +1);
         echo '<TH class=calendar colspan=3 align="center"><big>';
         echo '<a href="'.$prev_url_string.'">&lt;&lt;&lt; '.$this->prevtxt.' '.$this->yeartxt.'</a>';
         echo '&nbsp; &nbsp; '. $year_header_text .' &nbsp; &nbsp;';
         echo '<a href="'.$next_url_string.'">'.$this->nexttxt.' '.$this->yeartxt.' &gt;&gt;&gt;</a> </big> </TH>';
      }
      else
      {
   ?>
      <TH class=calendar colspan=3><big><?= $year_header_text ?> </big> </TH>
   <?
      }
   ?>
      </TR>
      <TR class=calendar>
   <?
         $reverse_chk = false;

         # Check if the indicator for the display of only a few months is active.
         if ($this->partial_display == true )
         {
            $start_month = $this->begin_month;
            $last_month = $this->end_month;
         }
         else
         {
            $start_month = 12;
            $last_month = 1;

            # If it is this year and it is June, then start then calendar from then. But if it was say July
            # you actually need to start in Sept and inactive Sept + Aug. This is because you want nice rows
            if ($year == $this_year )
            {
               $start_month = $this_month;
               # Should replace the 3 with $col_width, but then you may need to modify lines below
               $remainder =  $this_month % 3 ;
               if ($remainder == 2 ) $start_month = $start_month + 1;
               if ($remainder == 1 ) $start_month = $start_month + 2;
            }
            else if ($year == $first_year )
            {
               # The last month for the first year was actually the first month
               $last_month = $first_month ;
               # Should replace the 3 with $col_width, but then you may need to modify lines below
               if ($first_month > 2 )
               {
                  $remainder =  $first_month % 3 ;
                  if ($remainder == 0 ) $last_month = $last_month - 2;
                  if ($remainder == 2 ) $last_month = $last_month - 1;
               }
            }
         }

         $t_column = 1;
         #echo(" first year month day $first_year $first_month $first_day ");
         for ($m = $start_month; $m >= $last_month; $m--)
         {
            $active_start = 0;
            $reverse_chk = false;
            # If this year and month hasn't arrived set day 32 as the active one, otherwise we
            # could be in the middle of the month
            if (($year == $this_year) && ($m > $this_month)) $active_start = 37;
            else if (($year == $this_year) && ($m == $this_month))
            { $active_start = $this_day;
              $reverse_chk = true;
            }
            else if (($year == $first_year) && ($m == $first_month )) $active_start = $first_day;
            else if (($year == $first_year) && ($m < $first_month )) $active_start = 37;
            # The 37 is for 31 + 6 for checks later on graying

            #echo("writing month block with $m, $year, $col_width, $last_day, $active_start");
            $this->writeMonthBlock($m, $year, $col_width, $last_day, $active_start, $reverse_chk);
            if ($t_column >= $col_width)
            {
               # Start a new row in the calendar table
               ?>
               </TR>
               <?
               # And if there is another row to go, then spit out the row-start for it.
               if ($m >= $col_width )
               {
                  ?>
                  <TR class=calendar>
                  <?
               }
               $t_column = 1;
            }
            else $t_column++;
         }
   ?>
      </TR>
      </TBODY>
      </TABLE>
   <?
      if ($this->partial_display == false )
      {
         ?> <br> <br> <br> <?
      }
   }

   function writeMonthBlock( $the_month,$year, $cal_width, &$last_day, $active_start, $reverse_chk)
   {
      # Probably should have ?topic_id=&region_id=&type_id=&media_type etc in the 'stem'
      if(!strstr($this->url,"?")) $url_stem = $this->url."?convert_dates=true";
      else $url_stem = $this->url."&amp;convert_dates=true";
      $url_date_upper = "&amp;time_posted_upper_limit=";
      $url_date_lower = "&amp;time_posted_lower_limit=";

      $month_text = $this->monthString($the_month);
      # These abrevated forms of the text are used in the URLs and since it makes up the bulk of
      # the transmitted data, shortening them actually cuts off 30k of data at least, thus helping
      # take pressure off bandwidth. (Data transmitted is 210k which is too high!)

      $month_length = $this->monthSize($the_month);
      # Check Leap years for Febtuary, -returns true if a leap year
      if ($the_month == 2)
      {
         if (date('L', mktime(0,0,0,2,1,$year))) $month_length = 29;
      }

      $abrev_month_text = $this->abrevMonthString($the_month);
      $abrev_year =substr($year,2,2);
      $mnth_url= '<a href="'.$url_stem . $url_date_lower. date("d+M+y",mktime(0,0,0,$the_month,1,$year)). $url_date_upper.date("d+M+y",mktime(0,0,0,$the_month,$month_length,$year)).'">&gt;&gt;</a>';
      ?>
      <td class=calendar valign="top"> <TABLE class=calendar cellspacing=0 cellpadding=2 border=0>
      <tr class=calendar>
         <th class=calendar align="center" valign="top">&nbsp;</th>
	 <th class=calendar align="center" valign="top" colspan="5"> <?=$month_text?> <?=$year?> <?=$mnth_url?></th>
         <th class=calendar align="center" valign="top">&nbsp;</th>
      </TR> <TR class=calendar>

      <?
      // Returns <th>Su</th> <th>Mo</th> <th>Tu</th> <th>We</th> <th>Th</th> <th>Fr</th> <th>Sa</th> <th>[..]</th>
      // Need to add <th>[..]</th> at the end
      echo($this->language_dates->getWeekdaysBar());
      ?>
         <th>[..]</th>
      </tr>

      <?php
      # Most of time no. of rows will be 5, but can be 4 in Feb and 6, when 1st is near end of week
      $n_rows = 5;
      # Get the day of the week (0,6) for the 1st of this month. Actually need only do it
      # for the 'first' month we generate as we can pick of from the last day of the previous
      # month
      # Get day of week for the 1st of this month+year
      # mktime args: hr, min, sec, mon, day, year
      $day_of_week = date('w', mktime(0,0,0,$the_month,1,$year));

      if ($day_of_week > 5 && $month_length == 31) $n_rows = 6;
      else if ($day_of_week == 0 && $month_length == 28) $n_rows = 4;

      $t_day = 0;

      #echo("active start = $active_start dayOfWeek $day_of_week reverse_chk $reverse_chk ");

      for ($t_row = 1; $t_row <= $n_rows; $t_row++)
      {
         # Gray the whole row if any active URLs on it, but do not coloring if we are
         # overriding the generation of URLs

         if ( $this->override_url_gen == false)
         {
            $tr_text = "<tr class=calendar align=\"right\" valign=\"top\">";
         }
         else
         {
            $tr_text = "<tr class=calendarhighlighted align=\"right\" valign=\"top\">";
         }
         # See note below about this reverse check
         if ($reverse_chk)
         {
            if ($t_row == 1 )
            {
               if (7-$day_of_week >= $active_start) $tr_text = "<tr class=calendar align=\"right\" valign=\"top\">";
            }
            else
            {
               if ($t_day+0 >= $active_start) $tr_text = "<tr class=calendar align=\"right\" valign=\"top\">";
            }
         }
         else
         {
            if ($t_row == 1 )
            {
               if (7-$day_of_week <= $active_start) $tr_text = "<tr class=calendar align=\"right\" valign=\"top\">";
            }
            else
            {
               if ($t_day+6 <= $active_start) $tr_text = "<tr class=calendar align=\"right\" valign=\"top\">";
            }
         }
         echo ( $tr_text );

         $wk_url_date_upper = "";
         $wk_url_date_lower = "";
         $wk_lower_url_set  = false;
         $show_url_count = 0;

         for ($t_pos = 0; $t_pos <= 6; $t_pos++)
         {
            if (($t_row == 1 && $t_pos >= $day_of_week) || ($t_row > 1 )) $t_day++;

            if ( ($t_pos < $day_of_week && $t_row == 1) || ($t_day > $month_length) )
            {
               # Draw blank cell(s)
               #<td align="right" valign="top">&nbsp;</td>
               ?>
               <td class=calendar>&nbsp;</td>
               <?php
            }
            else
            {
               # Should also now be check of 'this' date to see if ahead of today, as there can be
               # no URL to it yet.
               # May have to change dates to do a after-xxxx and before-yyyy which means it could
               # be messy on month year boundaries -now done.

               $show_url = false;
               # This reverse crap is: for the current month the 'active_start' comparison
               # check is reversed around. There is probably a neater way of doing things.
               if ($reverse_chk == true )
               {
                  if ($t_day <= $active_start ) $show_url = true;
               }
               else
               {
                  if ($t_day >= $active_start ) $show_url = true;
               }

               $this_day="$t_day+$abrev_month_text+$abrev_year";
               if ( ($show_url == true) && ($this->override_url_gen == true) )
               {
                  // $this_day="$t_day+$abrev_month_text+$abrev_year";
                  $next_day = date("d+M+y",mktime(0,0,0,$the_month,$t_day+1,$year));

                  $full_url= $url_stem . $url_date_upper . $next_day . $url_date_lower . $this_day;
                  ?>
                  <td class=calendar>
                  <a href="<?=$full_url?>"><?=$t_day?></a></td>
                  <?php
               }
               else
               {
                  #<td align="right" valign="top"> ..........</td>
                  ?>
                  <td class=calendar><?=$t_day?></td>
                  <?php
               }
               if ( ($wk_lower_url_set  == false) && ($t_pos < 6) )
               {
                  $wk_url_date_lower = $url_date_lower . $this_day;
                  $wk_lower_url_set  = true;
                  // echo ("<br> flag " . $wk_lower_url_set . ' for ' . $this_day );
               }
               if ($show_url == true) $show_url_count++;
            }

            if ( (($t_pos == 6 ) && ($show_url == true) ) ||
                 (($wk_lower_url_set == true) && ($show_url_count > 0)) )
            {
               if ($t_day < $month_length )
                  $wk_url_date_upper = $url_date_upper . $next_day;
               else
                  $wk_url_date_upper = $url_date_upper . $this_day;
            }
         }
         if ( ($show_url_count > 0) && ( $wk_url_date_lower != "") && ( $wk_url_date_upper != "") )
         {
            $full_url= $url_stem . $wk_url_date_upper . $wk_url_date_lower;
            ?>
            <td class=calendar > <a href="<?=$full_url?>">&gt;&gt;</a></td> <?
         }
         else
         {
            ?><td class="calendar" > &nbsp;</td><?;
         }
         ?>
         </tr>
         <?php
      }
   ?>
      </TABLE> </td>
   <?
   }

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
}


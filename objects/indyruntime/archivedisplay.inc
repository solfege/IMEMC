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

require_once('newswiredisplay.inc');
require_once('objects/calendar.inc');

class archiveDisplay extends newswireDisplay
{

   function archiveDisplay ($obj_id)
   {
      $this->newswireDisplay($obj_id);
   }

   function drawNavigationOptions()
   {
      echo "<div class=events-viewoptions>";
      $gl = $this->getDisplayOption('gltitle');
      $ar = $this->getDisplayOption('nwtitle');

      if(isset($_REQUEST["archive"]) && $_REQUEST["archive"]=="gallery")
      {
         echo "<span class='selected-navigation'>$gl</span>";
         $lnk = $this->getFilterLink();
         echo "<br /><span class='top-navigation'><a class='top-navigation' href='$lnk'>$ar</a></span>\n";
      }
      else
      {
         $lnk = $this->getFilterLink();
         $lnk = addLinkParameters($lnk, array('archive=gallery'));
         echo "<span class='top-navigation'><a class='top-navigation' href='$lnk'>$gl</a></span>\n";
         echo "<br /><span class='selected-navigation'>$ar</span>";
      }
      echo "</div>";
   }


   function drawGalleryArchiveHeader()
   {
      echo "<H3>".$this->getDisplayOption('gltitle')."</H3>";
      echo "<p class='archive'>";
      echo $this->getDisplayOption('gltext')."</p>";
   }

   function drawNewswireArchiveHeader()
   {
      echo "<H3>".$this->getDisplayOption('nwtitle')."</H3>";
      echo "<p class='archive'>";
      echo $this->getDisplayOption('nwtext')."</p>";
   }


   function drawArchiveHeader()
   {
      $this->drawNavigationOptions();
      if(isset($_REQUEST["archive"]) && $_REQUEST["archive"]=="gallery")
      {
         $this->drawGalleryArchiveHeader();
      }
      else $this->drawNewswireArchiveHeader();
      //echo "<div class='calendar'>";
   }

   function display($res = 0)
   {
      SharedMemoryUpdate("archive");

      $this->writeDisplayHeader();
      $this->drawArchiveHeader();
      $calendar = new Calendar();
      $calendar->own_url = $this->getFullLink();
      if(isset($_REQUEST["archive"]) && $_REQUEST["archive"]=="gallery")
      {
         $calendar->url = $this->getFilterLink($this->site_settings['galleryobject']);
         $calendar->url = addLinkParameters($calendar->url, 'newswire_mode=archive');
      }
      else
      {
         $calendar->url = $this->getFilterLink($this->site_settings['newswireobject']);
         $calendar->url = addLinkParameters($calendar->url, 'newswire_mode=archive');
      }
      $calendar->yeartxt = $this->getDisplayOption('yeartxt');
      $calendar->prevtxt = $this->getDisplayOption('prevtxt');
      $calendar->nexttxt = $this->getDisplayOption('nexttxt');
      // Changed to displayed just one year.

      if (isset($_REQUEST['year']))
      {
          $calendar->drawFullCalendarYear( $_REQUEST['year']);
      }
      else
      {
          $calendar->drawFullCalendarYear($calendar->cur_year);
      }
      $this->drawArchiveFooter();
      // Old version drew the whole calendar -file size returned was 240k -too big
      // $calendar->drawFullCalendar();
   }

   function drawArchiveFooter()
   {
      echo "</div>";
      $this->writeDisplayFooter();
   }

}
?>

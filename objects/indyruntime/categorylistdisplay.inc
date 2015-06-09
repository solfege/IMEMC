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
require_once('sitefiltereddisplay.inc');


class categorylistDisplay extends siteFilteredDisplay
{
  function categorylistDisplay($obj_id)
  {
     $this->siteFilteredDisplay($obj_id);
  }



   function retrieveHeadlines()
   {
      global $topicList,$regionList,$languageList, $typeList;
      $ret = "";
      $num = 0;
      $this->setDisplayOptions();
      if($this->getDisplayOption('listing') == 'topic')
      {
         $activeList = $topicList->getfTopics();
         foreach($activeList as $item)
         {
            if($this->includesTopicID($item->topic_id))
            {
               $val = $item->getStrippedName();
               $this->getListEntry($item->getName(), $val, $num);
               $num++;
            }
         }
      }
      elseif($this->getDisplayOption('listing') == 'region')
      {
         $activeList = $regionList->getfRegions();
         foreach($activeList as $item)
         {
            if($this->includesRegionID($item->region_id))
            {
               $val = $item->getStrippedName();
               $this->getListEntry($item->getName(), $val, $num);
               $num++;
            }
         }
      }
      elseif($this->getDisplayOption('listing') == 'language')
      {
         $activeList = $languageList->getfLanguages();
         foreach($activeList as $item)
         {
            if($this->includesLanguageID($item->language_id))
            {
               $val = $item->getStrippedCode();
               $this->getListEntry($item->getName(), $val, $num);
               $num++;
            }
         }
      }
      else
      {
         $activeList = $typeList->getfTypes();
         foreach($activeList as $item)
         {
            if($this->includesTypeID($item->type_id))
            {
               $val = $item->getStrippedName();
               $this->getListEntry($item->getName(), $val, $num);
               $num++;
            }
         }
      }
   }

   function getListEntry($name, $val, $num=0)
   {
      global $page_style;
      $divider = $this->getDisplayOption('divider');
      if($divider == 1)
      {
         echo "<option value='$val'>$name</option>";
      }
      else
      {
         if($num > 0)
         {
            if($divider == 2) echo " | ";
            else echo "<br>";
         }
         $nw = $this->getDisplayOption("linksto");
         if(!$nw)
         {
            $nw = $this->site_settings['featureobject'];
         }
         $href = $this->getSimpleLink($nw);
         $href = addLinkParameters($href, array($this->getDisplayOption('listing').'='.$val));
         echo "<a class='$page_style' href='$href'>".$name."</a>";

      }
   }
}
?>